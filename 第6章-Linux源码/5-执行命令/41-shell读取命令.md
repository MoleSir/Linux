根据上回读取命令的过程分析，假设用户输入了一条：

````bash
cat info.txt | wc -l
````

希望查看 `info.txt` 的内容，分析这个命令是如何执行的；



# shell 读取命令

现在用户已经输入了命令，那么这些字符应该被保存在控制台终端 `tty` 结构体中的 `secondary` 队列中；

来康康 shell 的源码（这个部分不是操作系统）：

````c
// xv6-public sh.c
int main(void) {
    static char buf[100];
    // 读取命令
    while(getcmd(buf, sizeof(buf)) >= 0){
        // 创建新进程
        if(fork() == 0)
            // 执行命令
            runcmd(parsecmd(buf));
        // 等待进程退出
        wait();
    }
}
````



## 1. 执行 `getcmd` 

shell 使用一个 `while` 循环来不断读取着用户的命令：

````c
int getcmd(char *buf, int nbuf) {
    ...
    gets(buf, nbuf);
    ...
}

char* gets(char *buf, int max) {
    int i, cc;
    char c;
  
    for(i=0; i+1 < max; ){
      cc = read(0, &c, 1);
      if(cc < 1)
        break;
      buf[i++] = c;
      if(c == '\n' || c == '\r')
        break;
    }
    buf[i] = '\0';
    return buf;
}
````

根据源码，`getcmd` 函数通过 `gets` 最后调用系统调用 `read` 函数一个一个字符读取，直到遇到 `\r` 或者 `\n` 后返回，获得用户输入的一条命令；



## 2. `read` 读取字符

### 2.1 根据文件类型找到处理函数

所以最大的问题是 `read` 如何读取字符；

`read` 是一个用户态的库函数，类型 `fork`，其产生系统调用，通过两次查表执行 `sys_read`：

````c
// read_write.c
// fd = 0, count = 1
int sys_read(unsigned int fd,char * buf,int count) {
    struct file * file = current->filp[fd];
    // 校验 buf 区域的内存限制
    verify_area(buf,count);
    struct m_inode * inode = file->f_inode;
    // 管道文件
    if (inode->i_pipe)
        return (file->f_mode&1)?read_pipe(inode,buf,count):-EIO;
    // 字符设备文件
    if (S_ISCHR(inode->i_mode))
        return rw_char(READ,inode->i_zone[0],buf,count,&file->f_pos);
    // 块设备文件
    if (S_ISBLK(inode->i_mode))
        return block_read(inode->i_zone[0],&file->f_pos,buf,count);
    // 目录文件或普通文件
    if (S_ISDIR(inode->i_mode) || S_ISREG(inode->i_mode)) {
        if (count+file->f_pos > inode->i_size)
            count = inode->i_size - file->f_pos;
        if (count<=0)
            return 0;
        return file_read(inode,file,buf,count);
    }
    // 不是以上几种，就报错
    printk("(Read)inode->i_mode=%06o\n\r",inode->i_mode);
    return -EINVAL;
}
````

这个函数分支比较多，把**管道文件**、**字符设备文件**、**块设备文件**、**目录文件**或**普通文件**的处理函数统一放入一个接口中；

所以整体逻辑不复杂：

- 根据文件描述符找到文件；
- 根据文件类型进入不同的处理函数；
- 没有此文件类型报错；

而文件由 `read` 函数的参数获得，`read` 有三个参数，在 `gets` 中的调用格式为 `read(0, &c, 1)`；

参数一 `fd = 0`，表示读取的是此进程的 0 号文件描述符，`shell` 由 `init` 进程创建，`dev/tty0` 为 0 号文件；

````c
// main.c
void init(void) {
    setup((void *) &drive_info);
    (void) open("/dev/tty0",O_RDWR,0);
    (void) dup(0);
    (void) dup(0);
}
````

而这个 /dev/tty0 的文件类型，也就是其 inode 结构中表示文件类型与属性的 `i_mode` 字段，表示为**字符型设备**，所以最终会走到 `rw_char` 这个子方法下；

### 2.2 执行 `rw_char`

接下来执行 `rw_char` 函数：

```c
// char_dev.c
static crw_ptr crw_table[]={
    NULL,       /* nodev */
    rw_memory,  /* /dev/mem etc */
    NULL,       /* /dev/fd */
    NULL,       /* /dev/hd */
    rw_ttyx,    /* /dev/ttyx */
    rw_tty,     /* /dev/tty */
    NULL,       /* /dev/lp */
    NULL};      /* unnamed pipes */

int rw_char(int rw,int dev, char * buf, int count, off_t * pos) {
    crw_ptr call_addr;

    if (MAJOR(dev)>=NRDEVS)
        return -ENODEV;
    if (!(call_addr=crw_table[MAJOR(dev)]))
        return -ENODEV;
    return call_addr(rw,MINOR(dev),buf,count,pos);
}
```

`sys_read` 中调用 `rw_char(READ,inode->i_zone[0],buf,count,&file->f_pos)`；

根据 dev 这个参数，计算出主设备号为 4，次设备号为 0，所以将会走到 `rw_ttyx` 方法继续执行：

### 2.3 执行 `rw_ttyx`

````c
// char_dev.c
static int rw_ttyx(int rw,unsigned minor,char * buf,int count,off_t * pos) {
    return ((rw==READ)?tty_read(minor,buf,count):
        tty_write(minor,buf,count));
}
````

根据 `rw == READ` 走到读操作分支 `tty_read`；

### 2.4 执行 `tty_read`

````c#
// tty_io.c
// channel=0, nr=1
int tty_read(unsigned channel, char * buf, int nr) {
    struct tty_struct * tty = &tty_table[channel];
    char c, * b=buf;
    while (nr>0) {
        ...
        if (EMPTY(tty->secondary) ...) {
            sleep_if_empty(&tty->secondary);
            continue;
        }
        do {
            GETCH(tty->secondary,c);
            ...
            put_fs_byte(c,b++);
            if (!--nr) break;
        } while (nr>0 && !EMPTY(tty->secondary));
        ...
    }
    ...
    return (b-buf);
}
````

````c
tty_write(minor,buf,count));
````

传入的参数：

- `channel` 为 0，表示 `tty_table` 中的第 0 号，即控制台终端设备；
- `buf` 是数据的目的指针（由 `read` 一步步传下来）；
- `nr` 为 1，表示读出一个字符（由 `read` 一步步传下来）；

 整个方法，就是不断从 `secondary` 队列里取出字符，然后放入 `buf` 指所指向的内存；

- 首先判断如果 `nr > 0` ，但是 `secondary` 是空的话，说明用户此时还没输入，所以阻塞等待用户继续输入字符，输入后就会唤醒进程继续读取；（上一回最后有提到）；
- 之后就是不断读取 `secondary` 队列到 `buf` 中，直到读完规定的数量或者队列为空；

PS：其中 GETCH 就是个宏，改变 `secondary` 队列的队头队尾指针：

````c
#define GETCH(queue,c) \
(void)({c=(queue).buf[(queue).tail];INC((queue).tail);})
````

同理，判空逻辑就更为简单了，就是队列头尾指针是否相撞：

````c
#define EMPTY(a) ((a).head == (a).tail)
````

### 2.5 进程的阻塞与唤醒

队列为空，即不满足继续读取条件的时候，让进程阻塞的 `sleep_if_empty` 的实现：

````c
sleep_if_empty(&tty->secondary);

// tty_io.c
static void sleep_if_empty(struct tty_queue * queue) {
    cli();
    while (!current->signal && EMPTY(*queue))
        interruptible_sleep_on(&queue->proc_list);
    sti();
}

// sched.c
void interruptible_sleep_on(struct task_struct **p) {
    struct task_struct *tmp;
    ...
    tmp=*p;
    *p=current;
repeat: current->state = TASK_INTERRUPTIBLE;
    schedule();
    if (*p && *p != current) {
        (**p).state=0;
        goto repeat;
    }
    *p=tmp;
    if (tmp)
        tmp->state=0;
}
````

首先关闭中断，执行 `interruptible_sleep_on` 函数；

其中最关键的就是：

````c
current->state = TASK_INTERRUPTIBLE;
````

将当前进程的状态设置为可中断等待；

那么执行到进程调度程序时，当前进程将不会被调度，也就相当于阻塞了（选择进程调度时需要判断进程状态为就绪态）；

而唤醒进程就是再次按下键盘，使得 `secondary` 队列中有字符，也就打破了为空的条件，此时就应该将之前的进程唤醒了，这在上一回：

````c
// tty_io.c
void do_tty_interrupt(int tty) {
    copy_to_cooked(tty_table+tty);
}

void copy_to_cooked(struct tty_struct * tty) {
    ...
    wake_up(&tty->secondary.proc_list);
}
````

可以看到，在 `copy_to_cooked` 里，在将 `read_q` 队列中的字符处理后放入 `secondary` 队列中的最后一步，就是唤醒 `wake_up` 这个队列里的等待进程：

而 wake_up 函数更为简单，就是修改一下状态，使其变成可运行的状态

````c
// sched.c
void wake_up(struct task_struct **p) {
    if (p && *p) {
        (**p).state=0;
    }
}
````

等到下次进程调度，就会继续执行；

关于进程的阻塞还唤醒还有一些细节，下次再说；



## 3. 执行命令

得到了命令后，执行：

````c
runcmd(parsecmd(buf));
````



# 总结

<img src="./pics/41-shell读取命令.assets/640 (3)-16616783939041.png" alt="640 (3)" style="zoom:67%;" />

一个 shell 程序，会不断读取用户的输入，遇到 `\n` 或者 `\r` 就将其视为一句指令调用 `runcmd` 执行；

1. 在 shell 中，使用 `getcmd` 来获取一串用户输入作为命令，其中调用了 `read`， `read` 经过一些步骤最后调用了 `tty_read`，这个函数中有一个阻塞机制，就是当控制台终端没有输入时，shell 进程会阻塞；
2. 所以平时没有输入命令的时候，shell 进程就阻塞着；当用户输入一个字符，系统将 shell 唤醒，shell 读取字符；
3. 由于执行 `read` 传入的读取数量是 1，调用一次  `read` 只能获得一个字符，所以 shell 总是一个一个得处理读取的字符；
4. 如果读取到的字符不是  `\n` 或者 `\r`，那么 shell 就继续执行 `read`，再阻塞等待用户的下一个字符输入；
5. 如果是  `\n` 或者 `\r` ，shell 将之前的输入视为指令，交给 `runcmd` 处理；
6. 处理完毕后，shell 继续来到 `getcmd` 中的 `read` 等待下一个字符的输入；