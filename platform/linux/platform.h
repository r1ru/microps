#ifndef PLATFORM_H
#define PLATFORM_H

#include <stddef.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>

/*
 * Memory
 */

static inline void *
memory_alloc(size_t size)
{
    return calloc(1, size);
}

static inline void
memory_free(void *ptr)
{
    free(ptr);
}

/*
 * Mutex
 */

typedef pthread_mutex_t mutex_t;

#define MUTEX_INITIALIZER PTHREAD_MUTEX_INITIALIZER

static inline int
mutex_init(mutex_t *mutex)
{
    return pthread_mutex_init(mutex, NULL);
}

static inline int
mutex_lock(mutex_t *mutex)
{
    return pthread_mutex_lock(mutex);
}

static inline int
mutex_unlock(mutex_t *mutex)
{
    return pthread_mutex_unlock(mutex);
}

/* Interrup */

// On Linux, applications can use `[SIGTMIN, SIGTMAX]` but we do not use SIGMIN since glibc uses it.
#define INTR_IRQ_BASE   (SIGRTMIN + 1)
#define INTR_IRQ_SHARED 0x0001
#define INTR_IRQ_SOFTIRQ SIGUSR1

int intr_ruquest_irq(unsigned int irq, int (*handler)(unsigned int irq, void *dev), int flags, const char *name, void *dev);
int intr_raise_irq(unsigned int irq);

int intr_run(void);
void intr_shutdown(void);
int intr_init(void);

#endif
