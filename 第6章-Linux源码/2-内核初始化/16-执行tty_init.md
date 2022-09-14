# 执行 `tty_init`

现在已经建立好了内存管理结构 `mem_map` 、中断描述符表格 idt 和 `request` 数组；

这个数组是块设备驱动程序与内存缓冲区的桥梁，可以完整表示一个块设备读写操作要做的事情；

那么解析往下执行 `tty_init`：

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
    
    sti();
    move_to_user_mode();
    if (!fork()) {init();}
    for(;;) pause();
}
````

这个方法执行完成之后，将会具备键盘输入到显示器输出字符这个最常用的功能；`tty_init` 就是调用了其他两个函数：

````c
void tty_init(void)
{
    rs_init();
    con_init();
}
````

## 1. 执行 `rs_init`

打开 `rs_init()`：

````c
void rs_init(void)
{
    set_intr_gate(0x24,rs1_interrupt);
    set_intr_gate(0x23,rs2_interrupt);
    init(tty_table[1].read_q.data);
    init(tty_table[2].read_q.data);
    outb(inb_p(0x21)&0xE7,0x21);
}
````

这个方法是串口中断的开启，以及设置对应的中断处理程序，串口在现在的 PC 机上已经很少用到，就不展开；

## 2. 执行 `con_init`

接着打开 `con_init()`，这个函数很长，大体框架是：

````c
void con_init(void) {
    ...
    if (ORIG_VIDEO_MODE == 7) {
        ...
        if ((ORIG_VIDEO_EGA_BX & 0xff) != 0x10) {...}
        else {...}
    } else {
        ...
        if ((ORIG_VIDEO_EGA_BX & 0xff) != 0x10) {...}
        else {...}
    }
    ...
}
````

其中有非常多的 `if else`；

这是为了应对不同的显示模式，来分配不同的变量值，这里仅仅找出一个显示模式，只看一个分支；

> **显示模式**？
>
> 如何操作内存、CPU 等设备才能在显示器上显示想要的内容；
>
> 内存中有一个区域，与显存映射。当我们向这个内存区域写入数据，相当于写入显存，而在显存中写数据，就相当于在屏幕中输出；
>
> 比如：
>
> ````assembly
> mov [0xB8000], 'h'
> ````
>
> 就是向内存 0xB8000 写入 'h'，只要写上，屏幕就会显示：
>
> ![640 (5)](16-执行tty_init.assets/640 (5).png)
>
> 内存区域每两个字节一组，一个字节表示字符的编码，代表显示什么字符，另一个就是字符的颜色：
>
> ````
> mov [0xB8000],'h'
> mov [0xB8002],'e'
> mov [0xB8004],'l'
> mov [0xB8006],'l'
> mov [0xB8008],'o'
> ````
>
> 输出：
>
> ![640 (6)](16-执行tty_init.assets/640 (6).png)

将代码只留下一个分支：

````c
#define ORIG_X          (*(unsigned char *)0x90000)
#define ORIG_Y          (*(unsigned char *)0x90001)
void con_init(void) {
    register unsigned char a;
    // 第一部分 获取显示模式相关信息
    video_num_columns = (((*(unsigned short *)0x90006) & 0xff00) >> 8);
    video_size_row = video_num_columns * 2;
    video_num_lines = 25;
    video_page = (*(unsigned short *)0x90004);
    video_erase_char = 0x0720;
    // 第二部分 显存映射的内存区域 
    video_mem_start = 0xb8000;
    video_port_reg  = 0x3d4;
    video_port_val  = 0x3d5;
    video_mem_end = 0xba000;
    // 第三部分 滚动屏幕操作时的信息
    origin  = video_mem_start;
    scr_end = video_mem_start + video_num_lines * video_size_row;
    top = 0;
    bottom  = video_num_lines;
    // 第四部分 定位光标并开启键盘中断
    gotoxy(ORIG_X, ORIG_Y);
    set_trap_gate(0x21,&keyboard_interrupt);
    outb_p(inb_p(0x21)&0xfd,0x21);
    a=inb_p(0x61);
    outb_p(a|0x80,0x61);
    outb(a,0x61);
}
````

### 获取显示模式相关信息

第一部分获取 0x90006 地址的数据，获取显示模式等信息；这个地址在很早之前就保存着设备相关数据：

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

### 显存映射的内存区域

第二部分：显存映射的地址范围，假设是 CGA 类型的文本模式，所以映射的内存就从 0xB8000 到 0xBA000；

### 滚动屏幕操作时的信息

第三部分：设置一些滚动屏幕时需要的参数，定义顶行和低行在哪里，这设置顶行为第一行，底行为最后一行；

### 定位光标并开启键盘中断

第四部分：把光标移动到之前保存的光标位置处（取出内存为 0x90000 处的数据），然后设置并开启键盘中断；

开启键盘中断后，键盘上敲击一个按键后就会触发中断，中断程序就会读键盘码转换成 ASCII 码，然后写到光标处的内存地址，也就相当于往显存写，于是这个键盘敲击的字符就显示在了屏幕上。

至此已经可以：

- 根据已有信息已经可以实现往屏幕上的任意位置写字符了，而且还能指定颜色；
- 能接受键盘中断，根据键盘码中断处理程序就可以得知哪个键按下；

有了这个两个功能，再回到 `con_init()`：

````c
#define ORIG_X          (*(unsigned char *)0x90000)
#define ORIG_Y          (*(unsigned char *)0x90001)
void con_init(void) {
    ...
    // 第四部分 定位光标并开启键盘中断
    gotoxy(ORIG_X, ORIG_Y);
    ...
}
````

可以看到定义的两个宏，取出 0x90000 与 0x90001 的内存数据，作为 X ，Y 坐标值，因为早就规定好了可以在这里区光标位置；

进入函数 `gotoxy`：

````c
static inline void gotoxy(unsigned int new_x,unsigned int new_y) {
   ...
   x = new_x;
   y = new_y;
   pos = origin + y*video_size_row + (x<<1);
}
````

就是给 **x y pos** 这三个参数附上了值；

其中 **x** 表示光标在哪一列，**y** 表示光标在哪一行，**pos** 表示根据列号和行号计算出来的内存指针，也就是往这个 pos 指向的地址处写数据，就相当于往控制台的 x 列 y 行处写入字符了；

当按下按键，除法键盘中断，程序的调用链为：

````c
_keyboard_interrupt:
    ...
    call _do_tty_interrupt
    ...
    
void do_tty_interrupt(int tty) {
   copy_to_cooked(tty_table+tty);
}

void copy_to_cooked(struct tty_struct * tty) {
    ...
    tty->write(tty);
    ...
}

// 控制台时 tty 的 write 为 con_write 函数
void con_write(struct tty_struct * tty) {
    ...
    __asm__("movb _attr,%%ah\n\t"
      "movw %%ax,%1\n\t"
      ::"a" (c),"m" (*(short *)pos)
      :"ax");
     pos += 2;
     x++;
    ...
}
````

最重要的就是 `con_write` 函数：

- `__asm__` 内联汇编，就是把键盘输入的字符 **c** 写入 **pos** 指针指向的内存，相当于往屏幕输出了；

- 之后两行 pos+=2 和 x++，就是调整所谓的**光标**；

    写入一个字符，最底层，其实就是往内存的某处写个数据，然后顺便调整一下光标；

    而光标的本质，其实就是这里的 x y pos；

还可以做**换行效果**，当发现光标位置处于某一行的结尾时就把光标计算出一个新值，让其处于下一行的开头：

````c
void con_write(struct tty_struct * tty) {
    ...
    // 判断是都大于列数
    if (x>=video_num_columns) {
        x -= video_num_columns;
        pos -= video_size_row;
        lf();
  }
  ...
}

static void lf(void) {
   if (y+1<bottom) {
      y++;
      pos += video_size_row;
      return;
   }
 ...
}
````

相似的，还可以实现滚屏的效果，当检测到光标已经出现在最后一行最后一列了，那就把每一行的字符，都复制到它上一行，其实就是算好哪些内存地址上的值，拷贝到哪些内存地址；

 console.c 中主要就是实现了**回车**、**换行**、**删除**、**滚屏**、**清屏**等操作：

````c
// 定位光标的
static inline void gotoxy(unsigned int new_x, unsigned int new_y){}
// 滚屏，即内容向上滚动一行
static void scrup(void){}
// 光标同列位置下移一行
static void lf(int currcons){}
// 光标回到第一列
static void cr(void){}
...
// 删除一行
static void delete_line(void){}
````

---

现在完成了 `tty_init`，内核代码就可以用它来方便地在控制台输出字符；

