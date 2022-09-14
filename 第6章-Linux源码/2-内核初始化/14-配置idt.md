继续执行各种 init 函数，其中有：

```c
void main(void) {
    ...
    trap_init();
    ...
}
```



# 执行 `trp_init`

`trp_init()` 函数是来配置中断的；

## 配置中断入口函数

````
void trap_init(void) {
    int i;
    set_trap_gate(0,&divide_error);
    set_trap_gate(1,&debug);
    set_trap_gate(2,&nmi);
    set_system_gate(3,&int3);   /* int3-5 can be called from all */
    set_system_gate(4,&overflow);
    set_system_gate(5,&bounds);
    set_trap_gate(6,&invalid_op);
    set_trap_gate(7,&device_not_available);
    set_trap_gate(8,&double_fault);
    set_trap_gate(9,&coprocessor_segment_overrun);
    set_trap_gate(10,&invalid_TSS);
    set_trap_gate(11,&segment_not_present);
    set_trap_gate(12,&stack_segment);
    set_trap_gate(13,&general_protection);
    set_trap_gate(14,&page_fault);
    set_trap_gate(15,&reserved);
    set_trap_gate(16,&coprocessor_error);
    for (i=17;i<48;i++)
        set_trap_gate(i,&reserved);
    set_trap_gate(45,&irq13);
    set_trap_gate(39,&parallel_interrupt);
}
````

有很多重复调用的函数 `set_trap_gate()` ，与 `set_system_gate()`，简化一下代码：

````c
void trap_init(void) {
    int i;
    // set 了一堆 trap_gate
    set_trap_gate(0, &divide_error);
    ... 
    // 又 set 了一堆 system_gate
    set_system_gate(45, &bounds);
    ...
    // 又又批量 set 了一堆 trap_gate
    for (i=17;i<48;i++)
        set_trap_gate(i, &reserved);
    ...
}
````

整个函数就是不断调用这个两个函数；

````c
#define _set_gate(gate_addr,type,dpl,addr) \
__asm__ ("movw %%dx,%%ax\n\t" \
    "movw %0,%%dx\n\t" \
    "movl %%eax,%1\n\t" \
    "movl %%edx,%2" \
    : \
    : "i" ((short) (0x8000+(dpl<<13)+(type<<8))), \
    "o" (*((char *) (gate_addr))), \
    "o" (*(4+(char *) (gate_addr))), \
    "d" ((char *) (addr)),"a" (0x00080000))

#define set_trap_gate(n,addr) \
    _set_gate(&idt[n],15,0,addr)

#define set_system_gate(n,addr) \
    _set_gate(&idt[n],15,3,addr)
````

函数的作用就是向中断描述符表 idt 的某一项写入某个函数的入口地址；写入后，CPU 执行 int 指令就可以根据中断号来 idt 查，进入各种中断处理程序；

### 执行中断的过程

比如：

````c
set_trap_gate(0,&divide_error);
````

设置 0 号中断，对应的中断处理程序为 `divide_error`；

等 CPU 执行了一条除零指令的时候，会从硬件层面发起一个 0 号异常中断，然后执行由我们操作系统定义的 divide_error 也就是除法异常处理程序，执行完之后再返回。

再比如：

````c
set_system_gate(5,&overflow);
````

设置 5 号中断，对应的中断处理程序是 overflow，是边界出错中断；

> 这个 system 与 trap 的区别仅仅在于，设置的中断描述符的特权级不同，前者是 0（内核态），后者是 3（用户态），这块展开将会是非常严谨的、绕口的、复杂的特权级相关的知识，不明白的话先不用管，就理解为都是设置一个中断号和中断处理程序的对应关系就好了；

## 配置默认处理函数 `reserved`

分别执行了很多  `set_trap_gate()` 与 `set_system_gate()` 后，来到最后一个批量处理的操作：

````c
void trap_init(void) {
    ...
    for (i=17;i<48;i++)
        set_trap_gate(i,&reserved);
    ...
}
````

将 17 号都 48 号中断都暂时设置成了 `reserved()` 函数，这个函数是暂时放置，之后会覆盖；

执行完 `trap_init()` 后，idt 现在是：

![640 (2)](14-配置idt.assets/640 (2).png)

----

执行完 `trap_init()` 后，并不是所有的中断都可以起作用，比如键盘中断，中断号为 0x21，就还没有配置；

要再之后的，`tty_init()` 才会有，最后进入到 `con_init` 配置 0x21 号中断处理函数为 `keyborad_interrupt`；

````c
void main(void) {
    ...
    trap_init();
    ...
    tty_init();
    ...
}

void tty_init(void) {
    rs_init();
    con_init();
}

void con_init(void) {
    ...
    set_trap_gate(0x21,&keyboard_interrupt);
    ...
}
````

----

之前系统开启了关闭中断指令；

还需要开启：

```c
void main(void) {
    ...
    trap_init();
    ...
    tty_init();
    ...
    sti();
    ...
}
```

在配置完所有的中断函数后，`sti()` 执行汇编指令 `sti` 开启中断，之后，中断就可以正常允许生效；

