#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>

#include "util.h"
#include "platform.h"

struct irq_entry {
    struct irq_entry *next;
    unsigned int irq;
    int (*handler)(unsigned int irq, void *dev);
    int flags;
    char name[16];
    void *dev; 
};

// Global list of irq entries.
static struct irq_entry *irqs;

// Global signal mask
static sigset_t sigmask;

static pthread_t tid;
static pthread_barrier_t barrier;

// Registers a new irq handler.
// SAFETY: must be called before `intr_run`.
int intr_ruquest_irq(unsigned int irq, int (*handler)(unsigned int irq, void *dev), int flags, const char *name, void *dev) {
    struct irq_entry *entry;

    debugf("irq=%u, flags=%d, name=%s", irq, flags, name);
    for (entry = irqs; entry; entry = entry->next) {
        if (entry->irq == irq) {
            // If `irq` is already registered, `INTR_IRQ_SHARED` must be set.
            if (entry->flags ^ INTR_IRQ_SHARED || flags ^ INTR_IRQ_SHARED) {
                errorf("conflicts with already registered IRQs");
                return -1;
            }
        }
    }

    entry = memory_alloc(sizeof(*entry));
    if (!entry) {
        errorf("memory_alloc() failure");
        return -1;
    }
    entry->irq = irq;
    entry->handler = handler;
    entry->flags = flags;
    strncpy(entry->name, name, sizeof(entry->name) - 1);
    entry->dev = dev;

    entry->next = irqs;
    irqs = entry;
    sigaddset(&sigmask, irq);
    debugf("registered: irq=%u, name=%s", irq, name);

    return 0;
}

static void *intr_thread(void *arg) {
    int terminate = 0, sig, err;
    struct irq_entry *entry;

    debugf("start...");
    pthread_barrier_wait(&barrier);
    while (!terminate) {
        err = sigwait(&sigmask, &sig);
        if (err) {
            errorf("sigwait() %s", strerror(err));
            break;
        }
        switch (sig) {
            case SIGHUP:
                terminate = 1;
                break;
            default:
                for (entry = irqs; entry; entry = entry->next) {
                    if (entry->irq == (unsigned int)sig) {
                        debugf("irq=%d, name=%s", entry->irq, entry->name);
                        entry->handler(entry->irq, entry->dev);
                    }
                }
                break;
         }
    }
    debugf("terminated");
    return NULL;
}

int intr_run(void) {
    int err;

    err = pthread_sigmask(SIG_BLOCK, &sigmask, NULL);
    if (err) {
        errorf("pthread_sigmask() %s", strerror(err));
        return -1;
    }
    err = pthread_create(&tid, NULL, intr_thread, NULL);
    if (err) {
        errorf("pthread_create() %s", strerror(err));
        return -1;
    }
    // SAFETY: must wait until `intr_thread` is ready.
    pthread_barrier_wait(&barrier);
    return 0;
}

void intr_shutdown(void) {
    // `intr_thread` is not created, so there is nothing to do.
    if (pthread_equal(tid, pthread_self()) != 0) {
        return;
    }
    // shutdown `intr_thread`.
    pthread_kill(tid, SIGHUP);
    pthread_join(tid, NULL);
}

int intr_init(void) {
    tid = pthread_self();
    pthread_barrier_init(&barrier, NULL, 2);
    sigemptyset(&sigmask);
    sigaddset(&sigmask, SIGHUP);
    return 0;
}

int intr_raise_irq(unsigned int irq) {
    return pthread_kill(tid, (int)irq);
}