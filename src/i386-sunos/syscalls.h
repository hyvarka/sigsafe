/** @file
 * Lists implemented raw system calls on i386-sunos.
 * @legal
 * Copyright &copy; 2004 Scott Lamb &lt;slamb@slamb.org&gt;.
 * This file is part of sigsafe, which is released under the MIT license.
 * @version     $Id$
 * @author      Scott Lamb &lt;slamb@slamb.org&gt;
 */

SYSCALL(accept, 3)
SYSCALL(connect, 3)
SYSCALL(nanosleep, 2)
SYSCALL(open, 3)
SYSCALL(pause, 0)
SYSCALL(poll, 3)
SYSCALL(read, 3)
SYSCALL(readv, 3)
SYSCALL(recv, 4)
SYSCALL(recvfrom, 6)
SYSCALL(recvmsg, 3)
SYSCALL(send, 4)
SYSCALL(sendmsg, 3)
SYSCALL(sendto, 6)
SYSCALL(sigsuspend, 1)
SYSCALL(write, 3)
SYSCALL(writev, 3)
