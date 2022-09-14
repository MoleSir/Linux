# 执行 `time_init`

接下来，执行 `time_init()` 函数：

````c
#define CMOS_READ(addr) ({ \
    outb_p(0x80|addr,0x70); \
    inb_p(0x71); \
})

#define BCD_TO_BIN(val) ((val)=((val)&15) + ((val)>>4)*10)

static void time_init(void) {
    struct tm time;
    do {
        time.tm_sec = CMOS_READ(0);
        time.tm_min = CMOS_READ(2);
        time.tm_hour = CMOS_READ(4);
        time.tm_mday = CMOS_READ(7);
        time.tm_mon = CMOS_READ(8);
        time.tm_year = CMOS_READ(9);
    } while (time.tm_sec != CMOS_READ(0));
    BCD_TO_BIN(time.tm_sec);
    BCD_TO_BIN(time.tm_min);
    BCD_TO_BIN(time.tm_hour);
    BCD_TO_BIN(time.tm_mday);
    BCD_TO_BIN(time.tm_mon);
    BCD_TO_BIN(time.tm_year);
    time.tm_mon--;
    startup_time = kernel_mktime(&time);
}
````

主要就是使用 CMOS_READ，与 BCD_TO_BIN；

## 1. `CMOS_READ`

先看 CMOS_READ：

````c
#define CMOS_READ(addr) ({ \
    outb_p(0x80|addr,0x70); \
    inb_p(0x71); \
})
````

`outb_p` 写入某个端口，`inb_p` 读取某个端口，都是对 CMOS 这个硬件做操作；

> 端口 CPU 与外设交互的一个基本玩法，CPU 与外设打交道基本是通过端口，往某些端口写值来表示要这个外设干嘛，然后从另一些端口读值来接受外设的反馈；
>
> 这些外设内部由硬件厂商实现，对使用它的操作系统而言，是个黑盒，无需关心。
>
> 比如 CPU 与硬盘的交互也是使用硬盘提供的各个端口，硬件厂家会提供端口表，提供每个端口对应的读写位置：
>
> | 端口  | 读取哪里的数据                   | 写入到哪里                       |
> | ----- | -------------------------------- | -------------------------------- |
> | 0x1F0 | 数据寄存器                       | 数据寄存器                       |
> | 0x1F1 | 错误寄存器                       | 特征寄存器                       |
> | 0x1F2 | 扇区计数寄存器                   | 扇区计数寄存器                   |
> | 0x1F3 | 扇区号寄存器或 LBA 块地址0-7     | 扇区号或 LBA 块地址 0 - 7        |
> | 0x1F4 | 磁道数低8未或LBA块地址8-15       | 磁道数低8未或LBA块地址8-15       |
> | 0x1F5 | 磁道数高 8 位或 LBA 块地址 16~23 | 磁道数高 8 位或 LBA 块地址 16~23 |
> | 0x1F6 | 驱动器/磁头或 LBA 块地址 24~27   | 驱动器/磁头或 LBA 块地址 24~27   |
> | 0x1F7 | 命令寄存器或状态寄存器           | 命令寄存器                       |
>
> 那么读磁盘就是：往除了第一个以外的后面几个端口写数据，告诉要读硬盘的哪个扇区，读多少，然后从 0x1F0 端口一个字节一个字节读数据：
>
> 1. 在 0x1F2 写入要读取的扇区数量；
> 2. 在 0x1F3 - 0x1F6 四个端口写入计算号的起始 LBA 地址；
> 3. 在 0x1F7 写入读命令的命令号；
> 4. 不断检测 0x1F7 的忙位；
> 5. 若干步骤 4 不忙，就不断从 0x1F0 读取数据到内存；

CMOS 是主板上的一个可读写的 RAM 芯片，通过这两个函数就可以与其交互，获取数据；

那么上述代码中的：

````c
do {
    time.tm_sec = CMOS_READ(0);
    time.tm_min = CMOS_READ(2);
    time.tm_hour = CMOS_READ(4);
    time.tm_mday = CMOS_READ(7);
    time.tm_mon = CMOS_READ(8);
    time.tm_year = CMOS_READ(9);
} while (time.tm_sec != CMOS_READ(0));
````

就是不断获取这个芯片的数据，去配置系统时间，而 CMOS 如何实现获取时间的功能，这又是硬件厂商需要解决的问题，与操作系统无关；

## 2. `BCD_TO_BIN`

获取时间后，开始将 BCD 码转换位 BIN 二进制保存：

````c
BCD_TO_BIN(time.tm_sec);
BCD_TO_BIN(time.tm_min);
BCD_TO_BIN(time.tm_hour);
BCD_TO_BIN(time.tm_mday);
BCD_TO_BIN(time.tm_mon);
BCD_TO_BIN(time.tm_year);
````

最后 `startup_time = kernel_mktime(&time)`，就是根据得到的数据，计算从  1970 年 1 月 1 日 0 时开始到开机时间所经过的秒数，作为开机时间，存储在 `startup_time` 里：

````c
long kernel_mktime(struct tm * tm)
{
    long res;
    int year;
    year = tm->tm_year - 70;
    res = YEAR*year + DAY*((year+1)/4);
    res += month[tm->tm_mon];
    if (tm->tm_mon>1 && ((year+2)%4))
        res -= DAY;
    res += DAY*(tm->tm_mday-1);
    res += HOUR*tm->tm_hour;
    res += MINUTE*tm->tm_min;
    res += tm->tm_sec;
    return res;
}
````

至此完成了系统时间的设置；

