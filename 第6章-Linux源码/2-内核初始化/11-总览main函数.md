# main 函数总览

````c
void main(void) {
    ROOT_DEV = ORIG_ROOT_DEV;
    drive_info = DRIVE_INFO;
    memory_end = (1<<20) + (EXT_MEM_K<<10);
    memory_end &= 0xfffff000;
    if (memory_end > 16*1024*1024)
        memory_end = 16*1024*1024;
    if (memory_end > 12*1024*1024) 
        buffer_memory_end = 4*1024*1024;
    else if (memory_end > 6*1024*1024)
        buffer_memory_end = 2*1024*1024;
    else
        buffer_memory_end = 1*1024*1024;
    main_memory_start = buffer_memory_end;

    mem_init(main_memory_start,memory_end);
    trap_init();
    blk_dev_init();
    chr_dev_init();
    tty_init();
    time_init();
    sched_init();
    buffer_init(buffer_memory_end);
    hd_init();
    floppy_init();

    sti();
    move_to_user_mode();
    if (!fork()) {
        init();
    }

    for(;;) pause();
}
````

总共三十多行，可以将这些代码分成几个部分：



## 1. 计算内存边界

第一部分：一些参数的取值与计算：

````c
void main(void) {
    ROOT_DEV = ORIG_ROOT_DEV;
    drive_info = DRIVE_INFO;
    memory_end = (1<<20) + (EXT_MEM_K<<10);
    memory_end &= 0xfffff000;
    if (memory_end > 16*1024*1024)
        memory_end = 16*1024*1024;
    if (memory_end > 12*1024*1024) 
        buffer_memory_end = 4*1024*1024;
    else if (memory_end > 6*1024*1024)
        buffer_memory_end = 2*1024*1024;
    else
        buffer_memory_end = 1*1024*1024;
    main_memory_start = buffer_memory_end;
    ...
}
````

- 根设备 ROOT_DEV；

- 之前在汇编语言中获取的各个设备的参数信息 drive_info；

- 通过计算得到的内存边界：

    - main_memory_start
    - main_memory_end
    - buffer_memory_start
    - buffer_memory_end

- 以上信息都是由 setup 程序中调用 BIOS 中断获取的各个设备信息，并且保存在约定的内存地址 0x90000 处：

    | 内存地址 | 长度 | 名称         |
    | -------- | ---- | ------------ |
    | 0x90000  | 2    | 光标位置     |
    | 0x90002  | 2    | 拓展内存页数 |
    | 0x90004  | 2    | 显示页面     |
    | 0x90006  | 1    | 显示模式     |
    | 0x90007  | 1    | 字符列数     |
    | 0x90008  | 2    | 未知         |
    | 0x9000A  | 1    | 显示内存     |
    | 0x9000B  | 1    | 显示状态     |
    | 0x9000C  | 2    | 显卡特性参数 |
    | 0x9000E  | 1    | 屏幕行数     |
    | 0x9000F  | 1    | 屏幕列数     |
    | 0x90080  | 16   | 硬盘1参数表  |
    | 0x90090  | 16   | 硬件2参数表  |
    | 0x901FC  | 2    | 根设备号     |



## 2. 系统初始化

第二部分：各种初始化 init 操作：

````c
void main(void) {
    ...
    mem_init(main_memory_start,memory_end);
    trap_init();
    blk_dev_init();
    chr_dev_init();
    tty_init();
    time_init();
    sched_init();
    buffer_init(buffer_memory_end);
    hd_init();
    floppy_init();
    ...
}
````

- 内存初始化 `mem_init()`；
- 中断初始化 `trap_init()`；
- 进程调度初始化 `sched_init()`；
- ...



## 3. 切换用户态

第三部分：切换到用户态模式，并在一个新进程中做一个最终的初始化 init：

````c
void main(void) {
    ...
    sti();
    move_to_user_mode();
    if (!fork()) {
        init();
    }
    ...
}
````

这个 `init()` 函数会创建一个进程，设置终端的标准 IO，并且再创建出一个执行 shell 程序的进程来接收用户的命令，之后就可以在屏幕上显示终端；



## 4. 死循环

第四部分：死循环

````c
void main(void) {
    ...
    for(;;) pause();
}
````

操作系统就陷入这个死循环中，直到有其他进程需要操作系统的服务；

​			