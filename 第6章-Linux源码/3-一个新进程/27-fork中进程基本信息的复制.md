现在已经知道了，`fork()` 最后调用的是 `_sys_fork` ：

```assembly
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
```

其中有两个函数:

- `_find_empty_process`：找到空闲的进程槽位；
- `_copy_process`：复制进程；





# 复制进程信息

## 执行 `_find_empty_process`

`_find_empty_process` 方法比较简单，在 task 数组中找到一个空闲的位置来放置新的进程结构：

````c
long last_pid = 0;

int find_empty_process(void) {
    int i;
    repeat:
        if ((++last_pid)<0) last_pid=1;
        for(i=0 ; i<64 ; i++)
            if (task[i] && task[i]->pid == last_pid) goto repeat;
    for(i=1 ; i<64; i++)
        if (!task[i])
            return i;
    return -EAGAIN;
}
````

一共三步骤：

1. 判断 ++last_pid 是不是小于零了，小于零说明已经超过 long 的最大值了，重新赋值为 1，起到一个保护作用；
2. 一个 for 循环，看看刚刚的 last_pid 在所有 task[] 数组中，是否已经被某进程占用了。如果被占用了，那就重复执行，再次加一，然后再次判断，直到找到一个 pid 号没有被任何进程用为止；
3. 又是个 for 循环，刚刚已经找到一个可用的 pid 号了，那这一步就是再次遍历这个 task[] 试图找到一个空闲项，找到了就返回素组索引下标；

**最终，这个方法就返回 task[] 数组的索引，表示找到了一个空闲项**，之后就开始往这里塞一个新的进程；

现在系统只有 0 号进程，所以返回得到的结构就是 1，表示 task 数组的 1 号索引；



## 执行 `_copy_process`

````c
int copy_process(int nr,long ebp,long edi,long esi,long gs,long none,
        long ebx,long ecx,long edx,
        long fs,long es,long ds,
        long eip,long cs,long eflags,long esp,long ss)
{
    struct task_struct *p;
    int i;
    struct file *f;


    p = (struct task_struct *) get_free_page();
    if (!p)
        return -EAGAIN;
    task[nr] = p;
    *p = *current;  /* NOTE! this doesn't copy the supervisor stack */
    p->state = TASK_UNINTERRUPTIBLE;
    p->pid = last_pid;
    p->father = current->pid;
    p->counter = p->priority;
    p->signal = 0;
    p->alarm = 0;
    p->leader = 0;      /* process leadership doesn't inherit */
    p->utime = p->stime = 0;
    p->cutime = p->cstime = 0;
    p->start_time = jiffies;
    p->tss.back_link = 0;
    p->tss.esp0 = PAGE_SIZE + (long) p;
    p->tss.ss0 = 0x10;
    p->tss.eip = eip;
    p->tss.eflags = eflags;
    p->tss.eax = 0;
    p->tss.ecx = ecx;
    p->tss.edx = edx;
    p->tss.ebx = ebx;
    p->tss.esp = esp;
    p->tss.ebp = ebp;
    p->tss.esi = esi;
    p->tss.edi = edi;
    p->tss.es = es & 0xffff;
    p->tss.cs = cs & 0xffff;
    p->tss.ss = ss & 0xffff;
    p->tss.ds = ds & 0xffff;
    p->tss.fs = fs & 0xffff;
    p->tss.gs = gs & 0xffff;
    p->tss.ldt = _LDT(nr);
    p->tss.trace_bitmap = 0x80000000;
    if (last_task_used_math == current)
        __asm__("clts ; fnsave %0"::"m" (p->tss.i387));
    if (copy_mem(nr,p)) {
        task[nr] = NULL;
        free_page((long) p);
        return -EAGAIN;
    }
    for (i=0; i<NR_OPEN;i++)
        if (f=p->filp[i])
            f->f_count++;
    if (current->pwd)
        current->pwd->i_count++;
    if (current->root)
        current->root->i_count++;
    if (current->executable)
        current->executable->i_count++;
    set_tss_desc(gdt+(nr<<1)+FIRST_TSS_ENTRY,&(p->tss));
    set_ldt_desc(gdt+(nr<<1)+FIRST_LDT_ENTRY,&(p->ldt));
    p->state = TASK_RUNNING;    /* do this last, just in case */
    return last_pid;
}
````

大部分都是 tss 结构的复制，以及一些无关紧要的分支，来化简一下：

````c
int copy_process(int nr, ...) {
    struct task_struct p = 
        (struct task_struct *) get_free_page();
    task[nr] = p;
    *p = *current;

    p->state = TASK_UNINTERRUPTIBLE;
    p->pid = last_pid;
    p->counter = p->priority;
    ..
    p->tss.edx = edx;
    p->tss.ebx = ebx;
    p->tss.esp = esp;
    ...
    copy_mem(nr,p);
    ...
    set_tss_desc(gdt+(nr<<1)+FIRST_TSS_ENTRY,&(p->tss));
    set_ldt_desc(gdt+(nr<<1)+FIRST_LDT_ENTRY,&(p->ldt));
    p->state = TASK_RUNNING;
    return last_pid;
}
````

### 1. 申请空闲页面

`get_free_page` 函数向主存申请空闲页面，

<img src="./pics/27-fork中进程基本信息的复制.assets/640 (13).png" alt="640 (13)" style="zoom: 67%;" />

**遍历 mem_map[] 数组，找出值为零的项，就表示找到了空闲的一页内存**。然后把该项置为 1，表示该页已经被使用。最后，算出这个页的内存起始地址，返回；

然后，拿到的这个内存起始地址，就给了 task_struct 结构的 p；

所以一个进程结构 task_struct 就在内存中有了一块空间，但此时还没有赋值具体的字段；

### 2. 保存页面到 task 数组

将得到的地址保存到 task 数组中：

````c
task[nr] = p;
````

### 3. 复制当前进程结构体

将当前进程的 `task_struct` 中的数据原样复制到 p 中：

````c
*p = *current;
````

得到了这样的布局：

<img src="./pics/27-fork中进程基本信息的复制.assets/640 (14).png" alt="640 (14)" style="zoom:67%;" />

task 数组的 0 1 号元素指向不同的地址，但内容完全相同；

### 4. 修改新进程 PCB

修改一下进程 1 的值：

````c
int copy_process(int nr, ...) {
    ...
    p->state = TASK_UNINTERRUPTIBLE;
    p->pid = last_pid;
    p->counter = p->priority;
    ..
    p->tss.edx = edx;
    p->tss.ebx = ebx;
    p->tss.esp = esp;
    ...
    p->tss.esp0 = PAGE_SIZE + (long) p;
    p->tss.ss0 = 0x10;
    ...
}
````

像 state、pid、counter 这样的数据，还比如 tss 里面保存的上下文；

注意进程 1 的用户栈顶 esp 与进程 0 的相同，并且进程 1 eip 的值是通过产生中断时压入内核栈中的 eip 赋值的，所以之后调度到进程 1 会从那里开始执行；

这里有两个寄存器的值的赋值有些特殊，就是 ss0 和 esp0，这个表示 0 特权级也就是内核态时的 ss:esp 的指向；

根据代码得知，其含义是将代码在内核态时使用的堆栈栈顶指针指向进程 task_struct 所在的 4K 内存页的最顶端，而且之后的每个进程都是这样被设置的：

<img src="./pics/27-fork中进程基本信息的复制.assets/640 (15).png" alt="640 (15)" style="zoom:67%;" />

一个进程拥有两个栈顶指针：用户栈栈顶 esp 与内核栈栈顶 esp0；

用户程序在用户栈执行，当中断发送，进入内核态，程序在内核栈中执行；

