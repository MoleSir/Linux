现在已经切换到用户态，回来main函数主流程，继续执行：

````c
void main(void) {
    ...    
    move_to_user_mode();
    if (!fork()) {
        init();
    }
    for(;;) pause();
}
````



# 执行 `fork()`

````c
static _inline _syscall0(int,fork)

#define _syscall0(type,name) \
type name(void) \
{ \
long __res; \
__asm__ volatile ("int $0x80" \
    : "=a" (__res) \
    : "0" (__NR_##name)); \
if (__res >= 0) \
    return (type) __res; \
errno = -__res; \
return -1; \
}
````

展开用定义：

````c
int fork(void) {
     volatile long __res;
    _asm {
        _asm mov eax,__NR_fork
        _asm int 80h
        _asm mov __res,eax
    }
    if (__res >= 0)
        return (void) __res;
    errno = -__res;
    return -1;
}
````



## 执行 `int 80`

关键指令是执行了 `int 80h` 这个触发中断指令，CPU 到中断向量表找到 0x80 号中断处理函数 `system_call`：

````c
set_system_gate(0x80, &system_call);
````

再进入 `system_call`：

````c
_system_call:
    ...
    call [_sys_call_table + eax*4]
    ...
````

其中的 `call[_sys_call_table + eax * 4]` 使用了 eax 寄存器；

再回到 `fork` 的代码中，可以看到这样一句：

````c
_asm mov eax,__NR_fork
````

 将 __NR_fork 的值 2 放入寄存器 eax，那么这个 call 就是

````c
call [_sys_call_table + 2*4]
````

而其中的 `_sys_call_table` 是一张保存函数的表：

````c
fn_ptr sys_call_table[] = { sys_setup, sys_exit, sys_fork, sys_read,
  sys_write, sys_open, sys_close, sys_waitpid, sys_creat, sys_link,
  sys_unlink, sys_execve, sys_chdir, sys_time, sys_mknod, sys_chmod,
  sys_chown, sys_break, sys_stat, sys_lseek, sys_getpid, sys_mount,
  sys_umount, sys_setuid, sys_getuid, sys_stime, sys_ptrace, sys_alarm,
  sys_fstat, sys_pause, sys_utime, sys_stty, sys_gtty, sys_access,
  sys_nice, sys_ftime, sys_sync, sys_kill, sys_rename, sys_mkdir,
  sys_rmdir, sys_dup, sys_pipe, sys_times, sys_prof, sys_brk, sys_setgid,
  sys_getgid, sys_signal, sys_geteuid, sys_getegid, sys_acct, sys_phys,
  sys_lock, sys_ioctl, sys_fcntl, sys_mpx, sys_setpgid, sys_ulimit,
  sys_uname, sys_umask, sys_chroot, sys_ustat, sys_dup2, sys_getppid,
  sys_getpgrp, sys_setsid, sys_sigaction, sys_sgetmask, sys_ssetmask,
  sys_setreuid, sys_setregid
};
````

这是一张系统调用函数表；

一个函数地址 4 个字节，所以这个 call 就是调用系统调用函数表中的第二个函数：`sys_fork`；

### 执行 `sys_fork`

这其中才是 `fork` 函数的主要逻辑：

````c
_sys_fork:
    call _find_empty_process
    testl %eax,%eax
    js 1f
    push %gs
    pushl %esi
    pushl %edi
    pushl %ebp
    pushl %eax
    call _copy_process
    addl $20,%esp
1:  ret
````

综上，可以看出，调用任何一个系统调用其实就是先把这个系统调用在 `_sys_call_table` 中的索引放入 eax 寄存器，然后调用 `int 0x80`；

<img src="./pics/26-通过fork看系统调用.assets/640 (12).png" alt="640 (12)" style="zoom:80%;" />

-  `fork` 函数主要就是将 eax 设置为 2，再调用 `int 0x80`；
- 系统进入 0x80 中断对应的中断处理程序 `system_call`；
- 在 `system_call` 中再去查找 `syscall_table` 获得最后要调用的函数 `sys_fork`；
- 最后才进入 `sys_fork` 创建新进程；

一个系统调用要经过两次查表；

---

再多说两句，刚刚定义 fork 的系统调用模板函数时，用的是 **syscall0**，其实这个表示参数个数为 0，也就是 sys_fork 函数并不需要任何参数：

````
static _inline _syscall0(int,fork)

#define _syscall0(type,name) \
type name(void) \
{ \
long __res; \
__asm__ volatile ("int $0x80" \
    : "=a" (__res) \
    : "0" (__NR_##name)); \
if (__res >= 0) \
    return (type) __res; \
errno = -__res; \
return -1; \
}
````

所以其实，在 unistd.h 头文件里，还定义了 syscall0 ~ syscall3 一共四个宏：

````c
#define _syscall0(type,name)
#define _syscall1(type,name,atype,a)
#define _syscall2(type,name,atype,a,btype,b)
#define _syscall3(type,name,atype,a,btype,b,ctype,c)
````

看都能看出来，其实 **syscall1** 就表示有**一个参数**，**syscall2** 就表示有**两个参数**

那这些参数放在哪里了呢？

以 execve 为例：

````c
_syscall3(int,execve,const char *,file,char **,argv,char **,envp)

#define _syscall3(type,name,atype,a,btype,b,ctype,c) \
type name(atype a,btype b,ctype c) { \
    volatile long __res; \
    _asm { \
        _asm mov eax,__NR_##name \
        _asm mov ebx,a \
        _asm mov ecx,b \
        _asm mov edx,c \
        _asm int 80h \
        _asm mov __res,eax\
    } \
    if (__res >= 0) \
        return (type) __res; \
    errno = -__res; \
    return -1; \
}
````

可以看出，**参数 a 被放在了 ebx 寄存器，参数 b 被放在了 ecx 寄存器，参数 c 被放在了 edx 寄存器**；

之前对 fork 只看了 `system_call` 的一行关键代码，现在再来看 `system_call` 函数的全貌：

````c
_system_call:
    cmpl $nr_system_calls-1,%eax
    ja bad_sys_call
    push %ds
    push %es
    push %fs
    pushl %edx
    pushl %ecx      # push %ebx,%ecx,%edx as parameters
    pushl %ebx      # to the system call
    movl $0x10,%edx     # set up ds,es to kernel space
    mov %dx,%ds
    mov %dx,%es
    movl $0x17,%edx     # fs points to local data space
    mov %dx,%fs
    call _sys_call_table(,%eax,4)
    pushl %eax
    movl _current,%eax
    cmpl $0,state(%eax)     # state
    jne reschedule
    cmpl $0,counter(%eax)       # counter
    je reschedule
ret_from_sys_call:
    movl _current,%eax      # task[0] cannot have signals
    cmpl _task,%eax
    je 3f
    cmpw $0x0f,CS(%esp)     # was old code segment supervisor ?
    jne 3f
    cmpw $0x17,OLDSS(%esp)      # was stack segment = 0x17 ?
    jne 3f
    movl signal(%eax),%ebx
    movl blocked(%eax),%ecx
    notl %ecx
    andl %ebx,%ecx
    bsfl %ecx,%ecx
    je 3f
    btrl %ecx,%ebx
    movl %ebx,signal(%eax)
    incl %ecx
    pushl %ecx
    call _do_signal
    popl %eax
3:  popl %eax
    popl %ebx
    popl %ecx
    popl %edx
    pop %fs
    pop %es
    pop %ds
    iret
````

因为系统调用属于中断，而中断之前都要把**SS、ESP、EFLAGS、CS、EIP**压入栈中（压入内核栈）；

`system_call ` 也不例外，并且其还将 **ds、es、fs、edx、ecx、ebx、eax** 都压入；

在 `system_call.c` 中，Linus 也给出了此时堆栈的状态：

````c
/*
 * Stack layout in 'ret_from_system_call':
 *
 *   0(%esp) - %eax
 *   4(%esp) - %ebx
 *   8(%esp) - %ecx
 *   C(%esp) - %edx
 *  10(%esp) - %fs
 *  14(%esp) - %es
 *  18(%esp) - %ds
 *  1C(%esp) - %eip
 *  20(%esp) - %cs
 *  24(%esp) - %eflags
 *  28(%esp) - %oldesp
 *  2C(%esp) - %oldss
 */
````

就是 CPU 中断压入的 5 个值，加上 `system_call` 手动压入的 7 个值

之后，中断处理程序如果有需要的话，就可以从这里取出它想要的值，包括 CPU 压入的那五个值，或者 system_call 手动压入的 7 个值；

比如 **sys_execve** 这个中断处理函数，一开始就取走了位于栈顶 0x1C 位置处的 EIP 的值

````assembly
EIP = 0x1C
_sys_execve:
    lea EIP(%esp),%eax
    pushl %eax
    call _do_execve
    addl $4,%esp
    ret
````

随后在 **do_execve** 函数中，又通过 C 语言函数调用的约定，取走了 **filename，argv，envp** 等参数

````c
int do_execve(
        unsigned long * eip,
        long tmp,
        char * filename,
        char ** argv,
        char ** envp) {
    ...
}
````



系统调用通过 `int 0x80` 与给 eax 寄存器赋值实现（查中断向量表与系统调用函数表两张表），并且参数 a 被放在了 ebx 寄存器，参数 b 被放在了 ecx 寄存器，参数 c 被放在了 edx 寄存器；