# 重新设置 gdt 与 idt

现在进入了保护模式，来到 system 执行代码，来简答回顾一下：

1. 首先配置了全局描述表 gdt 和中断描述表 idt：

    ````assembly
    lidt  idt_48
    lgdt  gdt_48
    ````

2. 打开 A20 地址线：

    ````assembly
    mov al,#0xD1        ; command write
    out #0x64,al
    mov al,#0xDF        ; A20 on
    out #0x60,al
    ````

3. 然后更改 cr0 寄存器开启保护模式：

    ```assembly
    mov ax,#0x0001
    lmsw ax
    ```

4. 最后跳转到了内存地址 0 处开始执行代码：

    `````assembly
    jmpi 0, 8
    `````

0 位置处存储着操作系统全部核心代码，是由 head.s 和 main.c 以及后面的无数源代码文件编译并链接在一起而成的 system 模块；

---

首先来执行 system 中的 head.s，为进入 c 语言做准备：

````assembly
_pg_dir:
_startup_32:
    mov eax,0x10
    mov ds,ax
    mov es,ax
    mov fs,ax
    mov gs,ax
    lss esp,_stack_start
````

_pg_dir 表示页目录，之后设置分页时，页目录会保存在这里；

接下来 5 个 mov，给 ds、es、fs、gs 寄存器赋值为 0x10（0001 0000），其 3 -15 位是 10，即为 2，所以这些都代表了 gdt 的第二个索引，也即数据段描述符；

最后 `lss esp,_stack_start`，将 esp 既栈顶指针，指向 _stack_start（原来栈顶位于 0x9FF00）；

stack_start 定义在 sched.c 中：

````c
long user_stack[4096 >> 2];

struct
{
  long *a;
  short b;
}
stack_start = {&user_stack[4096 >> 2], 0x10};
````

其中先定义了一个 long 类型的数组 user_stack；

之后创建一个数据结构，其由一 long 类型地址 a 与 short 变量 b 组成；之后马上创建了一个此类型对象 stack_start。并且其中的 long 类型地址就是 user_stack 的最后一个元素地址，short 变量为 0x10；

之后赋值给 esp 寄存器；

继续看：

````assembly
call setup_idt ;设置中断描述符表
call setup_gdt ;设置全局描述符表
mov eax,10h
mov ds,ax
mov es,ax
mov fs,ax
mov gs,ax
lss esp,_stack_start
````

先设置了 **idt** 和 **gdt**，然后又重新执行了一遍刚刚执行过的代码；

为什么要重新设置这些段寄存器呢？因为上面修改了 gdt，所以要重新设置一遍以刷新才能生效。那我们接下来就把目光放到设置 idt 和 gdt 上；

---

`call setup_idt` 执行了 `setup_idt` 标签：

````assembly
setup_idt:
    lea edx,ignore_int
    mov eax,00080000h
    mov ax,dx
    mov dx,8E00h
    lea edi,_idt
    mov ecx,256
rp_sidt:
    mov [edi],eax
    mov [edi+4],edx
    add edi,8
    dec ecx
    jne rp_sidt
    lidt fword ptr idt_descr
    ret

idt_descr:
    dw 256*8-1
    dd _idt

_idt:
    DQ 256 dup(0)
````

> idt 中就保存着一个个中断描述符，每一个中断号对应一个中断描述符，而中断描述符中保存的主要信息是中断程序的地址。这样一个中断号过来后，CPU 就会自动寻找相应的中断程序，然后去执行它，还是类似一个数组寻址的过程；

执行完 `setup_idt` 后，中断描述表 idt 中被设置了 256 个中断描述符，并且让每一个中断描述符中的中断程序例程都指向一个 **ignore_int** 的函数地址，这个是个**默认的中断处理程序**，之后会逐渐被各个具体的中断程序所覆盖。比如之后键盘模块会将自己的键盘中断处理程序，覆盖过去；

---

`call setup_gdt` 执行 `setup_dgt` 标签：

执行完毕后，gdt 设置为：

````assembly
_gdt:
    DQ 0000000000000000h    ;/* NULL descriptor */
    DQ 00c09a0000000fffh    ;/* 16Mb */
    DQ 00c0920000000fffh    ;/* 16Mb */
    DQ 0000000000000000h    ;/* TEMPORARY - don't use */
    DQ 252 dup(0)
````

其实和我们原先设置好的 gdt 类似：

有**代码段描述符**和**数据段描述符**，然后第四项**系统段描述符**并没用到；

最后还留了 252 项的空间，这些空间后面会用来放置**任务状态段描述符 TSS** 和**局部描述符 LDT**；

![640 (3)](8-重新设置gdt与idt.assets/640 (3).png)

---

为什么要重新设置一次 gdt 与 idt 呢，那是因为原来我们在 setup 中执行，两个表都保存在 setup 中。现在 setup 执行完毕，其内存之后就会被使用，而现在进入了 system，就需要把表搬过来；

![640 (4)](8-重新设置gdt与idt.assets/640 (4).png)

---

之后我们会先开启分页机制，最后进入 main：

````assembly
jmp after_page_tables
...
after_page_tables:
    push 0
    push 0
    push 0
    push L6
    push _main
    jmp setup_paging
L6:
    jmp L6
````

