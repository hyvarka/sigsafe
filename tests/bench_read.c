/*
 * $Id$
 * Scott Lamb <slamb@slamb.org>
 *
 * Tests throughput speed of a pipe with variations of read(), to know how
 * sigsafe compares performance-wise to other methods of signal handling.
 */

#include <limits.h>
#include <unistd.h>
#include <setjmp.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/select.h>
#ifdef DO_SAFE
#include <sigsafe.h>
#endif

#define BYTES_TO_TRANSFER           4294967295uL
                            /* max: 4294967295uL */

#define min(a,b) ((a)<(b)?(a):(b))

#ifdef DO_SAVE
#define MYREAD sigsafe_read
#else
#define MYREAD read
#endif

enum PipeHalf {
    READ,
    WRITE
};

int main(void) {
    int mypipe[2];
    char buffer[PIPE_BUF];
#ifdef DO_SETJMP
    sigjmp_buf env;
#endif
    size_t total_transferred = 0;

#ifdef DO_SAFE
    sigsafe_install_tsd(NULL, NULL);
#endif

    pipe(mypipe);
    if (fork() == 0) {
        /* Child */
        while (total_transferred < BYTES_TO_TRANSFER) {
            ssize_t retval = write(mypipe[WRITE], buffer,
                    min(PIPE_BUF, BYTES_TO_TRANSFER-total_transferred));
            assert(retval > 0);
            total_transferred += retval;
        }
    } else {
        /* Parent */
#ifdef DO_SELECT
        /* Set to non-blocking */
        int flags;
        flags = fcntl(mypipe[READ], F_GETFL);
        flags |= O_NONBLOCK;
        fcntl(mypipe[READ], F_SETFL, flags);
#endif
        while (total_transferred < BYTES_TO_TRANSFER) {
            ssize_t retval;
#ifdef DO_SELECT
            fd_set readset;
            FD_ZERO(&readset);
            FD_SET(mypipe[READ], &readset);
            select(mypipe[READ]+1, &readset, NULL, NULL, NULL);
#endif
#ifdef DO_SETJMP
            sigsetjmp(env, 0);
#endif
            retval = MYREAD(mypipe[READ], buffer,
                    min(PIPE_BUF, BYTES_TO_TRANSFER - total_transferred));
            assert(retval > 0);
            total_transferred += retval;
        }
    }
    return 0;
}
