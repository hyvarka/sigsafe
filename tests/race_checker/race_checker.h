/**
* @legal
 * Copyright &copy; 2004 Scott Lamb &lt;slamb@slamb.org&gt;.
 * This file is released under the MIT license.
 * @version         $Id$
 * @author          Scott Lamb &lt;slamb@slamb.org&gt;
 */

#ifndef RACECHECKER_H
#define RACECHECKER_H

enum error_return_type {
    DIRECT,
    NEGATIVE,
    ERRNO
};

int error_wrap(int, const char*, enum error_return_type);

enum run_result {
    INTERRUPTED,
    NORMAL,
    WEIRD
};

/**
 * @defgroup races_generic Generic routines
 */
/*@{*/
extern volatile sig_atomic_t signal_received;

void install_safe(void*);
void install_unsafe(void*);
/*@}*/

/**
 * @defgroup races_io Input/output system calls
 */
/*@{*/
void* create_pipe(void);
void cleanup_pipe(void*);

enum run_result do_safe_read(void*);
enum run_result do_unsafe_read(void*);
void nudge_read(void*);
/*@}*/

#endif /* !RACECHECKER_H */