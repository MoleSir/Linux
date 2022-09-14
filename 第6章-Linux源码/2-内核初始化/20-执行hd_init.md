# 执行 `hd_init`

完成了 `buffer_init` 后，接着执行 `hd_init` ：

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
    hd_init(); //本文重点
    floppy_init();
    
    sti();
    move_to_user_mode();
    if (!fork()) {init();}
    for(;;) pause();
}
````

进入 `hd_init`：

````c
//struct blk_dev_struct {
//    void (*request_fn)(void);
//    struct request * current_request;
//};
//extern struct blk_dev_struct blk_dev[NR_BLK_DEV];

void hd_init(void) {
    blk_dev[3].request_fn = do_hd_request;
    set_intr_gate(0x2E,&hd_interrupt);
    outb_p(inb_p(0x21)&0xfb,0x21);
    outb(inb_p(0xA1)&0xbf,0xA1); 
}
````

总体可以分为三个步骤：

1. 向某个 IO 端口上写一些数据，表示开启；
2. 然后再向中断向量表中添加一个中断，使 CPU 能够响应这个硬件设备的动作；
3. 最后再初始化一些数据结构来管理；

## 1. 绑定硬盘处理函数

````c
void hd_init(void) {
    blk_dev[3].request_fn = do_hd_request;
    ...
}
````

把 `blk_dev` 数组索引为 3 处的块设备管理结构 `blk_dev_struct`  的 `request_fn` 赋值为 `do_hd_request`；

> 计算器中存在很多设备，Linux 0.11 内核使用 `blk_dev` 数组来管理，每个索引代表一个设备：
>
> ````c
> struct blk_dev_struct blk_dev[NR_BLK_DEV] = {
>     { NULL, NULL },     /* no_dev */
>     { NULL, NULL },     /* dev mem */
>     { NULL, NULL },     /* dev fd */
>     { NULL, NULL },     /* dev hd */
>     { NULL, NULL },     /* dev ttyx */
>     { NULL, NULL },     /* dev tty */
>     { NULL, NULL }      /* dev lp */
> };
> ````
>
> 索引为 3 的位置就是给硬盘 hd 使用的；
>
> 每个设备执行读写请求都有字节具体的函数实现，在上层看来都是一个统一函数 **request_fn** 即可（就是 `blk_dev_struct` 数据的一个成员），具体实现各有不同，对于硬盘来说，这个实现就是 **do_hd_request** 函数。

## 2. 设置硬盘中断

继续：

````c
void hd_init(void) {
    ...
    set_intr_gate(0x2E,&hd_interrupt);
    ...
}
````

中断号是 0x2E，中断处理函数是 `hd_interrupt`，也就是说硬盘发生读写时，硬盘会发出中断信号给 CPU，之后 CPU 便会陷入中断处理程序，也就是执行 `hd_interrupt` 函数；

## 3. 配置硬盘端口

最后：

````c
void hd_init(void) {
    ...
    outb_p(inb_p(0x21)&0xfb,0x21);
    outb(inb_p(0xA1)&0xbf,0xA1); 
}
````

就是往几个 IO 端口上读写，其作用是允许硬盘控制器发送中断请求信号；



# 硬盘读写

至此完成了磁盘的初始化，而读写磁盘的方式之前已经说了，就是往端口写数据，而操作系统确确实实也是这样做的：

````c
static void hd_out(unsigned int drive,unsigned int nsect,unsigned int sect,
        unsigned int head,unsigned int cyl,unsigned int cmd,
        void (*intr_addr)(void)) {
    ...
    do_hd = intr_addr;
    outb_p(hd_info[drive].ctl,HD_CMD);
    port = 0x1f0;
    outb_p(hd_info[drive].wpcom>>2,++port);
    outb_p(nsect,++port);
    outb_p(sect,++port);
    outb_p(cyl,++port);
    outb_p(cyl>>8,++port);
    outb_p(0xA0|(drive<<4)|head,++port);
    outb(cmd,++port);
}
````

就是向端口写入数据，硬件具体如何完成这些功能，与操作系统无关；