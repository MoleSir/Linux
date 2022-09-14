# 执行 `buffer_init`

执行完 `sched_init`，继续执行 `buffer_init`：

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
    if (!fork()) {
        init();
    }
    for(;;) pause();
}
````

## 1. 传递参数

这个函数是需要参数的，就是之前计算得到的 `buffer_memory_end`：

<img src="19-执行buffer_init.assets/640 (7).png" alt="640 (7)" style="zoom:50%;" />

在 `mem_init` 中还设置了 `mem_map` 来管理内存：

![640 (8)](19-执行buffer_init.assets/640 (8).png)

## 2. 初始化缓冲区

现在需要把缓冲区初始化，假设内存 8Ｍ，只留下一个分支：

````c
extern int end;
struct buffer_head * start_buffer = (struct buffer_head *) &end;

void buffer_init(long buffer_end) {
    struct buffer_head * h = start_buffer;
    void * b = (void *) buffer_end;
    while ( (b -= 1024) >= ((void *) (h+1)) ) {
        h->b_dev = 0;
        h->b_dirt = 0;
        h->b_count = 0;
        h->b_lock = 0;
        h->b_uptodate = 0;
        h->b_wait = NULL;
        h->b_next = NULL;
        h->b_prev = NULL;
        h->b_data = (char *) b;
        h->b_prev_free = h-1;
        h->b_next_free = h+1;
        h++;
    }
    h--;
    free_list = start_buffer;
    free_list->b_prev_free = h;
    h->b_next_free = free_list;
    for (int i=0;i<307;i++)
        hash_table[i]=NULL;
}
````

### `end` 指向缓冲区起始位置

首先声明外部变量 `end`，这个变量的内存地址，就等于缓冲区的起始位置 `start_buffer`；

这个变量不是由操作系统写入的，而是链接器 ld 在连接整个程序时设置的一个外部变量，计算出整个内核代码的末尾地址，那里就时缓冲区的起始位置；

<img src="19-执行buffer_init.assets/640 (9).png" alt="640 (9)" style="zoom:80%;" />

### 组织缓冲区内存布局为链表

之后进入函数的这个部分：

````c
void buffer_init(long buffer_end) {
    struct buffer_head * h = start_buffer;
    void * b = (void *) buffer_end;
    while ( (b -= 1024) >= ((void *) (h+1)) ) {
        ...
        h->b_data = (char *) b;
        h->b_prev_free = h-1;
        h->b_next_free = h+1;
        h++;
    }
    ...
}
````

第一行，把 `start_buffer` 作为一个 `buffer_head  `结构体的地址，`start_buffer` 就是缓冲区在内存的起始地址；

所以意思就是把缓冲区开启的位置视为一个 `buffer_head` 缓存表头；

之后定义指针 b 的值为 `buffer_end`，既缓冲区的结束内存地址，图中的 2 M 位置；

之后的循环就是从缓冲区的结尾 b 开始，每次减去 1024，而缓冲区起始地址开始，每次增加一个 `buffer_head` 大小，直到二者相遇；

![640 (10)](19-执行buffer_init.assets/640 (10).png)

并且在每次循环中设置各种 `buffer_head` 结构体的值，特别是这三个：

- `h->b_data` 被赋值为 b，指向缓冲块；
- `h->b_prev_free` 被赋值为 h 的上一个 `buffer_head`；
- `h->b_next_free` 被赋值为 h 的下一个 `buffer_head`；

最后形成这样的结构：

![640 (13)](19-执行buffer_init.assets/640 (13).png)

构成了类似链表的数据结构；

### 连接头尾形成环形链表

之后退出循环，继续执行：

````c
void buffer_init(long buffer_end) {
    ...
    free_list = start_buffer;
    free_list->b_prev_free = h;
    h->b_next_free = free_list;
    ...
}
````

把缓冲区最开始的一个缓冲头的上一个换成头设置为 h；

而 h 经过了循环后，已经称为了最后一个换成头了，所以这里就是把第一个换成头的下一个指向最后；

之后又把最后一个缓存头的上一个缓存头设置为第一个缓存头；

最后形成缓存头组成的双向循环链表，并且每个缓存头指向一个缓冲块：

![640 (14)](19-执行buffer_init.assets/640 (14).png)

现在使用 firee_list 就可以遍历所有的缓存头，找到所有的缓冲块了；

### 构建 `hash_table`

继续还有最后一步：

````c
void buffer_init(long buffer_end) {
    ...
    for (i=0;i<307;i++)
        hash_table[i]=NULL;
}
````

将一个 307 大小的 hash_table 数组所有元素指向 NULL；

这个函数代码在 buffer.c 中，今后是为文件系统而服务，具体是内核程序如果需要访问块设备中的数据，就都需要经过缓冲区来间接地操作；

也就是说，读取块设备的数据（硬盘中的数据），需要先读到缓冲区中，如果缓冲区已有了，就不用从块设备读取了，直接取走。

需要一个 hashmap 的结构方便快速查找，这就是 hash_table 这个数组的作用；

至此完成了 `buffer_init`，将内存中的缓冲区组织为一个双向链表；