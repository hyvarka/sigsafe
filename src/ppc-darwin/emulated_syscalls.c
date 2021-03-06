/** @file
 * Emulated system calls under ppc-darwin.
 * @legal
 * Copyright &copy; 2004 Scott Lamb &lt;slamb@slamb.org&gt;.
 * This file is part of sigsafe, which is released under the MIT license.
 * sigsafe_nanosleep is derived from Apple libc and thus is under the APSL
 * 1.1.
 * @version     $Id: emulated_syscalls.c 773 2004-05-19 13:51:56Z slamb$
 * @author      Scott Lamb &lt;slamb@slamb.org&gt;
 */

#include "sigsafe_internal.h"
#include <mach/mach.h>
#include <mach/clock.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <errno.h>
#include <assert.h>

extern mach_port_t clock_port; /* see Libc/mach/mach_init_ports.c */

INTERNAL_DEC kern_return_t sigsafe_clock_sleep_trap(
        mach_port_name_t        clock_name,
        sleep_type_t            sleep_type,
        int                     sleep_sec,
        int                     sleep_nsec,
        mach_timespec_t         *abs_time_after);

/* Derived from Libc/gen/nanosleep.c */
int
sigsafe_nanosleep(const struct timespec *req, struct timespec *rem)
{
    kern_return_t ret;
    mach_timespec_t abs_time_after;
    mach_timespec_t abs_time_before;

    /* Validate arguments */
    if ((req == NULL) || (req->tv_sec < 0) || (req->tv_nsec > NSEC_PER_SEC)) {
        return -EINVAL;
    }

    if (rem != NULL) { /* we might have to calculate rem; get current time */
        ret = clock_get_time(clock_port, &abs_time_before);
        assert(ret == KERN_SUCCESS); /* should never fail */
    }

    ret = sigsafe_clock_sleep_trap(clock_port, TIME_RELATIVE,
                                   req->tv_sec, req->tv_nsec, NULL);

    if (ret == KERN_ABORTED) { /* this is Mach's equivalent of EINTR */
        if (rem != NULL) { /* update remaining time */
            ret = clock_get_time(clock_port, &abs_time_after);
            assert(ret == KERN_SUCCESS);

            /* rem = abs_time_before + req - abs_time_after */
            ADD_MACH_TIMESPEC(&abs_time_before, req);
            SUB_MACH_TIMESPEC(&abs_time_before, &abs_time_after);
            rem->tv_sec = abs_time_before.tv_sec;
            rem->tv_nsec = abs_time_before.tv_nsec;
        }
        return -EINTR;
    }

    return (ret == KERN_SUCCESS) ? 0 : -EINVAL;
}

INTERNAL_DEC int sigsafe_sigsuspend_(const sigset_t mask);

int
sigsafe_sigsuspend(const sigset_t *mask_p)
{
    sigset_t mask;

    if (mask_p)
        mask = *mask_p;
    else
        sigemptyset(&mask);

    return sigsafe_sigsuspend_(mask);
}

int
sigsafe_pause(void)
{
    sigset_t mask;
    sigemptyset(&mask);
    return sigsafe_sigsuspend_(mask);
}

ssize_t
sigsafe_recv(int s, void *buf, size_t len, int flags)
{
    return sigsafe_recvfrom(s, buf, len, flags, NULL, 0);
}

ssize_t
sigsafe_send(int s, const void *buf, size_t len, int flags)
{
    return sigsafe_sendto(s, buf, len, flags, NULL, 0);
}

pid_t
sigsafe_wait(int *status)
{
    return sigsafe_wait4(WAIT_ANY, status, 0, NULL);
}

pid_t
sigsafe_wait3(int *status, int options, struct rusage *rusage)
{
    return sigsafe_wait4(WAIT_ANY, status, options, rusage);
}
