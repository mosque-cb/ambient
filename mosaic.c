#define HURLEY

#ifdef SMALLLINUX
#define __LIBRARY__
#include <unistd.h>
#include <time.h>
static inline fork(void) __attribute__((always_inline));
static inline pause(void) __attribute__((always_inline));
static inline _syscall0(int,fork)
static inline _syscall0(int,pause)
static inline _syscall1(int,setup,void *,BIOS)
static inline _syscall0(int,sync)

#include <linux/tty.h>
#include <linux/sched.h>
#include <linux/head.h>
#include <asm/system.h>
#include <asm/io.h>
#include <stddef.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <linux/fs.h>

#define size_t  int
#endif

#ifdef HURLEY
#include "common.h"
#include "timer.h"
#include "debug.h"
#include "sched.h"
#include "gdt.h"
#include "pmm.h"
#include "vmm.h"
#include "string.h"

#include "console.h"
#include "vargs.h"
#include "elf.h"

#include "heap.h"
#include "task.h"

#include "idt.h"

#define size_t  int
#endif


#define   BUF_SIZE 1024 * 100

/*(pthread system)*/
// 全局 pid 值
pid_t now_pid = 0;

// 内核线程创建
int32_t kernel_thread(int (*fn)(void *), void *arg)
{
	struct task_struct *new_task = (struct task_struct *)kmalloc(STACK_SIZE);
	assert(new_task != NULL, "kern_thread: kmalloc error");

	// 将栈低端结构信息初始化为 0 
	bzero(new_task, sizeof(struct task_struct));

	new_task->state = TASK_RUNNABLE;
	new_task->stack = current;
	new_task->pid = now_pid++;
	new_task->mm = NULL;

	uint32_t *stack_top = (uint32_t *)((uint32_t)new_task + STACK_SIZE);

	*(--stack_top) = (uint32_t)arg;
	*(--stack_top) = (uint32_t)kthread_exit;
	*(--stack_top) = (uint32_t)fn;

	new_task->context.esp = (uint32_t)new_task + STACK_SIZE - sizeof(uint32_t) * 3;

	// 设置新任务的标志寄存器未屏蔽中断，很重要
	new_task->context.eflags = 0x200;
	new_task->next = running_proc_head;
	
	// 找到当前进任务队列，插入到末尾
	struct task_struct *tail = running_proc_head;
	assert(tail != NULL, "Must init sched!");

	while (tail->next != running_proc_head) {
		tail = tail->next;
	}
	tail->next = new_task;

	return new_task->pid;
}

void kthread_exit()
{
	register uint32_t val asm ("eax");

	printk("Thread exited with value %d\n", val);

	while (1);
}
/*(pthread system end)*/

/*(elf system)*/
// 从 multiboot_t 结构获取ELF信息
elf_t elf_from_multiboot(multiboot_t *mb)
{
	int i;
	elf_t elf;
	elf_section_header_t *sh = (elf_section_header_t *)mb->addr;

	uint32_t shstrtab = sh[mb->shndx].addr;
	for (i = 0; i < mb->num; i++) {
		const char *name = (const char *)(shstrtab + sh[i].name) + PAGE_OFFSET;
		// 在 GRUB 提供的 multiboot 信息中寻找内核 ELF 格式所提取的字符串表和符号表
		if (strcmp(name, ".strtab") == 0) {
			elf.strtab = (const char *)sh[i].addr + PAGE_OFFSET;
			elf.strtabsz = sh[i].size;
		}
		if (strcmp(name, ".symtab") == 0) {
			elf.symtab = (elf_symbol_t *)(sh[i].addr + PAGE_OFFSET);
			elf.symtabsz = sh[i].size;
		}
	}

	return elf;
}

// 查看ELF的符号信息
const char *elf_lookup_symbol(uint32_t addr, elf_t *elf)
{
	int i;

	for (i = 0; i < (elf->symtabsz / sizeof(elf_symbol_t)); i++) {
		if (ELF32_ST_TYPE(elf->symtab[i].info) != 0x2) {
		      continue;
		}
		// 通过函数调用地址查到函数的名字(地址在该函数的代码段地址区间之内)
		if ( (addr >= elf->symtab[i].value) && (addr < (elf->symtab[i].value + elf->symtab[i].size)) ) {
			return (const char *)((uint32_t)elf->strtab + elf->symtab[i].name);
		}
	}

	return NULL;
}
/*(elf system end)*/

/*(gdt system)*/
// 全局描述符表长度
#define GDT_LENGTH 5

// 全局描述符表定义
gdt_entry_t gdt_entries[GDT_LENGTH];

// GDTR
gdt_ptr_t gdt_ptr;


// 全局描述符表构造函数，根据下标构造
static void gdt_set_gate(int32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);

// 初始化全局描述符表
void init_gdt()
{
	// 全局描述符表界限 e.g. 从 0 开始，所以总长要 - 1
	gdt_ptr.limit = sizeof(gdt_entry_t) * GDT_LENGTH - 1;
	gdt_ptr.base = (uint32_t)&gdt_entries;

	// 采用 Intel 平坦模型
	gdt_set_gate(SEG_NULL,  0x0, 0x0, 0x0, 0x0); 	// 按照 Intel 文档要求，第一个描述符必须全 0
	gdt_set_gate(SEG_KTEXT, 0x0, 0xFFFFFFFF, 0x9A, 0xCF); 	// 内核指令段
	gdt_set_gate(SEG_KDATA, 0x0, 0xFFFFFFFF, 0x92, 0xCF); 	// 内核数据段
	gdt_set_gate(SEG_UTEXT, 0x0, 0xFFFFFFFF, 0xFA, 0xCF); 	// 用户模式代码段
	gdt_set_gate(SEG_UDATA, 0x0, 0xFFFFFFFF, 0xF2, 0xCF); 	// 用户模式数据段

	// 加载全局描述符表地址到 GPTR 寄存器
	gdt_flush((uint32_t)&gdt_ptr);
}

// 全局描述符表构造函数，根据下标构造
// 参数分别是 数组下标、基地址、限长、访问标志，其它访问标志
/* 结构体定义如下：
typedef struct
{
	uint16_t limit_low;     // 段界限   15～0
	uint16_t base_low;      // 段基地址 15～0
	uint8_t  base_middle;   // 段基地址 23～16
	uint8_t  access;        // 段存在位、描述符特权级、描述符类型、描述符子类别
	uint8_t  granularity; 	// 其他标志、段界限 19～16
	uint8_t  base_high;     // 段基地址 31～24
} __attribute__((packed)) gdt_entry_t;
*/
static void gdt_set_gate(int32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran)
{
	gdt_entries[num].base_low     = (base & 0xFFFF);
	gdt_entries[num].base_middle  = (base >> 16) & 0xFF;
	gdt_entries[num].base_high    = (base >> 24) & 0xFF;

	gdt_entries[num].limit_low    = (limit & 0xFFFF);
	gdt_entries[num].granularity  = (limit >> 16) & 0x0F;

	gdt_entries[num].granularity |= gran & 0xF0;
	gdt_entries[num].access       = access;
}



/*(dt system end)*/

/*(time system)*/
void timer_callback(pt_regs *regs)
{
	schedule();
}

void init_timer(uint32_t frequency)
{
	register_interrupt_handler(IRQ0, timer_callback);

	uint32_t divisor = 1193180 / frequency;

	outb(0x43, 0x36);

	uint8_t low = (uint8_t)(divisor & 0xFF);
	uint8_t hign = (uint8_t)((divisor >> 8) & 0xFF);
	
	outb(0x40, low);
	outb(0x40, hign);
}
/*(time system end)*/

/*(string system)*/
#define _U      0x01 
#define _L      0x02  
#define _D      0x04 
#define _C      0x08  
#define _P      0x10  
#define _S      0x20  
#define _X      0x40  
#define _SP     0x80 
const unsigned char _local_ctype[]= {
    _C,_C,_C,_C,_C,_C,_C,_C, /* 0-7 */
    _C,_C|_S,_C|_S,_C|_S,_C|_S,_C|_S,_C,_C, /* 8-15 */
    _C,_C,_C,_C,_C,_C,_C,_C, /* 16-23 */
    _C,_C,_C,_C,_C,_C,_C,_C, /* 24-31 */
    _S|_SP,_P,_P,_P,_P,_P,_P,_P, /* 32-39 */
    _P,_P,_P,_P,_P,_P,_P,_P, /* 40-47 */
    _D,_D,_D,_D,_D,_D,_D,_D, /* 48-55 */
    _D,_D,_P,_P,_P,_P,_P,_P, /* 56-63 */
    _P,_U|_X,_U|_X,_U|_X,_U|_X,_U|_X,_U|_X,_U, /* 64-71 */
    _U,_U,_U,_U,_U,_U,_U,_U, /* 72-79 */
    _U,_U,_U,_U,_U,_U,_U,_U, /* 80-87 */
    _U,_U,_U,_P,_P,_P,_P,_P, /* 88-95 */
    _P,_L|_X,_L|_X,_L|_X,_L|_X,_L|_X,_L|_X,_L, /* 96-103 */
    _L,_L,_L,_L,_L,_L,_L,_L, /* 104-111 */
    _L,_L,_L,_L,_L,_L,_L,_L, /* 112-119 */
    _L,_L,_L,_P,_P,_P,_P,_C, /* 120-127 */
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 128-143 */
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 144-159 */
    _S|_SP,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P, /* 160-175 */
    _P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P, /* 176-191 */
    _U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U, /* 192-207 */
    _U,_U,_U,_U,_U,_U,_U,_P,_U,_U,_U,_U,_U,_U,_U,_L, /* 208-223 */
    _L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L, /* 224-239 */
    _L,_L,_L,_L,_L,_L,_L,_P,_L,_L,_L,_L,_L,_L,_L,_L}; /* 240-255 */

#define __ismask(x)(_local_ctype[(int)(unsigned char)(x)])
#define isalnum(c)      ((__ismask(c)&(_U|_L|_D)) != 0)   
#define isalpha(c)      ((__ismask(c)&(_U|_L)) != 0)          
#define iscntrl(c)      ((__ismask(c)&(_C)) != 0)             
#define isdigit(c)      ((__ismask(c)&(_D)) != 0)         
#define isgraph(c)      ((__ismask(c)&(_P|_U|_L|_D)) != 0)    
#define islower(c)      ((__ismask(c)&(_L)) != 0)            
#define isprint(c)      ((__ismask(c)&(_P|_U|_L|_D|_SP)) != 0)
#define ispunct(c)      ((__ismask(c)&(_P)) != 0)   
#define isspace(c)      ((__ismask(c)&(_S)) != 0) 
#define isupper(c)      ((__ismask(c)&(_U)) != 0)            
#define isxdigit(c)     ((__ismask(c)&(_D|_X)) != 0)          
#define isascii(c) (((unsigned char)(c))<=0x7f)              
#define toascii(c) (((unsigned char)(c))&0x7f)               


inline void memcpy(uint8_t *dest, const uint8_t *src, uint32_t len)
{
	for (; len != 0; len--) {
		*dest++ = *src++;
	}
}

inline void memset(void *dest, uint8_t val, uint32_t len)
{
	uint8_t *dst = (uint8_t *)dest;

	for ( ; len != 0; len--) {
		*dst++ = val;
	}
}

inline void bzero(void *dest, uint32_t len)
{
	memset(dest, 0, len);
}

inline int strcmp(const char *str1, const char *str2)
{
	while (*str1 && *str2 && (*str1++ == *str2++))
	      ;

	if (*str1 == '\0' && *str2 == '\0') {
	      return 0;
	}

	if (*str1 == '\0') {
	      return -1;
	}
	
	return 1;
}

inline char *strcpy(char *dest, const char *src)
{
	char *tmp = dest;

	while (*src) {
	      *dest++ = *src++;
	}

	*dest = '\0';
	
	return tmp;
}

inline char *strcat(char *dest, const char *src)
{
	char *cp = dest;

	while (*cp) {
	      cp++;
	}

	while ((*cp++ = *src++))
	      ;

	return dest;
}

inline int strlen(const char *src)
{
	const char *eos = src;

        while (*eos++)
	      ;
	        
	return (eos - src - 1);
}


int atoi(const char *nptr)
{
    int c;              /* current char */
    int total;         /* current total */
    int sign;           /* if '-', then negative, otherwise positive */

    /* skip whitespace */
    while ( isspace((int)(unsigned char)*nptr) )
        ++nptr;

    c = (int)(unsigned char)*nptr++;
    sign = c;           /* save sign indication */
    if (c == '-' || c == '+')
        c = (int)(unsigned char)*nptr++;    /* skip sign */

    total = 0;

    while (isdigit(c)) {
        total = 10 * total + (c - '0');     /* accumulate digit */
        c = (int)(unsigned char)*nptr++;    /* get next char */
    }

    if (sign == '-')
        return -total;
    else
        return total;   /* return result, negated if necessary */
} 

int cross_memcmp(const void * left, const void * right, size_t count) {
    if (!count)
        return(0);
 
    while ( --count && *(char *)left == *(char *)right ) {
 
        left = (char *)left + 1;
        right = (char *)right + 1;
         
    }
 
    return( *((unsigned char *)left) - *((unsigned char *)right) );
}

static int rand_next=1;
unsigned int cross_rand(void) {
    rand_next = rand_next * 1103515245 + 12345;   
    return (rand_next/65536) % 32768;
}

char * cross_strchr(const char * string, int ch) {
    while (*string && *string != (char)ch)
        string++;
 
    if (*string == (char)ch)
        return((char *)string);
    return(NULL);
}

int cross_tolower(int c) {
    if(c<='Z' && c>='A') return (c+32);
    return (c);
}

int cross_atoi( char *nptr)
{
    int c; /* current char */
    int total; /* current total */
    int sign; /* if '-', then negative, otherwise positive */

    /* skip whitespace */
    while ( isspace((int)(unsigned char)*nptr) )
        ++nptr;

    c = (int)(unsigned char)*nptr++;
    sign = c; /* save sign indication */
    if (c == '-' || c == '+')
        c = (int)(unsigned char)*nptr++; /* skip sign */

    total = 0;

    while (isdigit(c)) {
        total = 10 * total + (c - '0'); /* accumulate digit */
        c = (int)(unsigned char)*nptr++; /* get next char */
    }

    /*
      if(new_ptr != NULL){
      *new_ptr = nptr - 1;}
      */

    if (sign == '-')
        return -total;
    else
        return total; /* return result, negated if necessary */
}

char * cross_strstr (register char *source, register char *pattern) {  
    register char *bp;  
    register char *sp;  
    if (!*pattern)  
        return source;  
    while (*source) {  
        bp = source;  
        sp = pattern;  
        do  {  
            if (!*sp)  
                return source;  
        } while (*bp++ == *sp++);  
        source += 1;
    }  
    return 0;  
}

int cross_strlen (const char* str) {
    const char *char_ptr;  
    const unsigned long int *longword_ptr;  
    unsigned long int longword, himagic, lomagic;  
  
    /* Handle the first few characters by reading one character at a time. 
       Do this until CHAR_PTR is aligned on a longword boundary.  */  
    for (char_ptr = str; ((unsigned long int) char_ptr  
                          & (sizeof (longword) - 1)) != 0;  
         ++char_ptr)  
        if (*char_ptr == '\0')  
            return char_ptr - str;  
  
    /* All these elucidatory comments refer to 4-byte longwords, 
       but the theory applies equally well to 8-byte longwords.  */  
  
    longword_ptr = (unsigned long int *) char_ptr;  
  
    /* Bits 31, 24, 16, and 8 of this number are zero.  Call these bits 
       the "holes."  Note that there is a hole just to the left of 
       each byte, with an extra at the end: 
 
       bits:  01111110 11111110 11111110 11111111 
       bytes: AAAAAAAA BBBBBBBB CCCCCCCC DDDDDDDD 
 
       The 1-bits make sure that carries propagate to the next 0-bit. 
       The 0-bits provide holes for carries to fall into.  */  
    himagic = 0x80808080L;  
    lomagic = 0x01010101L;  
    if (sizeof (longword) > 4)  
        {  
            /* 64-bit version of the magic.  */  
            /* Do the shift in two steps to avoid a warning if long has 32 bits.  */  
            himagic = ((himagic << 16) << 16) | himagic;  
            lomagic = ((lomagic << 16) << 16) | lomagic;  
        }  
    /*
    if (sizeof (longword) > 8)  
        abort ();  
    */
  
    /* Instead of the traditional loop which tests each character, 
       we will test a longword at a time.  The tricky part is testing 
       if *any of the four* bytes in the longword in question are zero.  */  
    for (;;)  
        {  
            longword = *longword_ptr++;  
  
            if (((longword - lomagic) & ~longword & himagic) != 0)  
                {  
                    /* Which of the bytes was the zero?  If none of them were, it was 
                       a misfire; continue the search.  */  
  
                    const char *cp = (const char *) (longword_ptr - 1);  
  
                    if (cp[0] == 0)  
                        return cp - str;  
                    if (cp[1] == 0)  
                        return cp - str + 1;  
                    if (cp[2] == 0)  
                        return cp - str + 2;  
                    if (cp[3] == 0)  
                        return cp - str + 3;  
                    if (sizeof (longword) > 4)  
                        {  
                            if (cp[4] == 0)  
                                return cp - str + 4;  
                            if (cp[5] == 0)  
                                return cp - str + 5;  
                            if (cp[6] == 0)  
                                return cp - str + 6;  
                            if (cp[7] == 0)  
                                return cp - str + 7;  
                        }  
                }  
        }  
}  

void cross_strcpy(void * left, void * right) {
    char * target = left;
    int count = cross_strlen(right);
    while ((*(char *)target++) = (*(char *)right++)) {
        if (--count == 0) {
            *(char *)target = '\0'; break;
        }
    }
}

void cross_system(char* cmd) {
#if defined(LINUX)
    system(cmd);
#endif
} 

int skip_number(char *nptr, char **new_ptr) {
    int c; /* current char */
    int total; /* current total */
    int sign; /* if '-', then negative, otherwise positive */

    /* skip whitespace */
    while (isspace((int) (unsigned char) *nptr))
        ++nptr;

    c = (int) (unsigned char) *nptr++;
    sign = c; /* save sign indication */
    if (c == '-' || c == '+')
        c = (int) (unsigned char) *nptr++; /* skip sign */

    total = 0;

    while (isdigit(c)) {
        total = 10 * total + (c - '0'); /* accumulate digit */
        c = (int) (unsigned char) *nptr++; /* get next char */
    }

    if (new_ptr != NULL) {
        *new_ptr = nptr - 1;
    }

    if (sign == '-')
        return -total;
    else
        return total; /* return result, negated if necessary */
}
/*(end string system)*/

/*(console system)*/

inline void outb(uint16_t port, uint8_t value)
{
	asm volatile ("outb %1, %0" : : "dN" (port), "a" (value));
}

inline uint8_t inb(uint16_t port)
{
	uint8_t ret;

	asm volatile("inb %1, %0" : "=a" (ret) : "dN" (port));

	return ret;
}

inline uint16_t inw(uint16_t port)
{
	uint16_t ret;

	asm volatile ("inw %1, %0" : "=a" (ret) : "dN" (port));

	return ret;
}

inline void enable_intr()
{
    asm volatile ("sti");
}

inline void disable_intr()
{
    asm volatile ("cli" ::: "memory");
}


static uint16_t *video_memory = (uint16_t *)(0xB8000 + PAGE_OFFSET);

static uint8_t cursor_x = 0;
static uint8_t cursor_y = 0;

static void move_cursor()
{
	uint16_t cursorLocation = cursor_y * 80 + cursor_x;
	
	outb(0x3D4, 14);                 
	outb(0x3D5, cursorLocation >> 8);
	outb(0x3D4, 15);                 
	outb(0x3D5, cursorLocation);     
}

static void scroll()
{
	uint8_t attribute_byte = (0 << 4) | (15 & 0x0F);
	uint16_t blank = 0x20 | (attribute_byte << 8);  // space 是 0x20

	if (cursor_y >= 25) {
		int i;
		for (i = 0 * 80; i < 24 * 80; i++) {
		      video_memory[i] = video_memory[i+80];
		}

		for (i = 24 * 80; i < 25 * 80; i++) {
		      video_memory[i] = blank;
		}

		cursor_y = 24;
	}
}

void console_clear()
{
	uint8_t attribute_byte = (0 << 4) | (15 & 0x0F);
	uint16_t blank = 0x20 | (attribute_byte << 8);

	int i;
	for (i = 0; i < 80 * 25; i++) {
	      video_memory[i] = blank;
	}

	cursor_x = 0;
	cursor_y = 0;
	move_cursor();
}

void console_putc_color(char c, real_color_t back, real_color_t fore)
{
	uint8_t back_color = (uint8_t)back;
	uint8_t fore_color = (uint8_t)fore;

	uint8_t attribute_byte = (back_color << 4) | (fore_color & 0x0F);
	uint16_t attribute = attribute_byte << 8;

	if (c == 0x08 && cursor_x) {
	      cursor_x--;
	} else if (c == 0x09) {
	      cursor_x = (cursor_x+8) & ~(8-1);
	} else if (c == '\r') {
	      cursor_x = 0;
	} else if (c == '\n') {
		cursor_x = 0;
		cursor_y++;
	} else if (c >= ' ') {
		video_memory[cursor_y*80 + cursor_x] = c | attribute;
		cursor_x++;
	}

	if (cursor_x >= 80) {
		cursor_x = 0;
		cursor_y ++;
	}

	scroll();

	move_cursor();
}

void console_write(char *cstr)
{
	while (*cstr) {
	      console_putc_color(*cstr++, rc_black, rc_white);
	}
}

void console_write_color(char *cstr, real_color_t back, real_color_t fore)
{
	while (*cstr) {
	      console_putc_color(*cstr++, back, fore);
	}
}

void console_write_hex(uint32_t n, real_color_t back, real_color_t fore)
{
	int tmp;
	char noZeroes = 1;

	console_write_color("0x", back, fore);

	int i;
	for (i = 28; i >= 0; i -= 4) {
		tmp = (n >> i) & 0xF;
		if (tmp == 0 && noZeroes != 0) {
		      continue;
		}
		noZeroes = 0;
		if (tmp >= 0xA) {
		      console_putc_color(tmp-0xA+'a', back, fore);
		} else {
		      console_putc_color(tmp+'0', back, fore);
		}
	}
}

void console_write_dec(uint32_t n, real_color_t back, real_color_t fore)
{
	if (n == 0) {
		console_putc_color('0', back, fore);
		return;
	}

	uint32_t acc = n;
	char c[32];
	int i = 0;
	while (acc > 0) {
		c[i] = '0' + acc % 10;
		acc /= 10;
		i++;
	}
	c[i] = 0;

	char c2[32];
	c2[i--] = 0;

	int j = 0;
	while(i >= 0) {
	      c2[i--] = c[j++];
	}

	console_write_color(c2, back, fore);
}



 int vsprintf(char *buff, const char *format, va_list args);

void printk(const char *format, ...)
{
	// 避免频繁创建临时变量，内核的栈很宝贵
	static char buff[1024];
	va_list args;
	int i;

	va_start(args, format);
	i = vsprintf(buff, format, args);
	va_end(args);

	buff[i] = '\0';

	console_write(buff);
}

void printk_color(real_color_t back, real_color_t fore, const char *format, ...)
{
	// 避免频繁创建临时变量，内核的栈很宝贵
	static char buff[1024];
	va_list args;
	int i;

	va_start(args, format);
	i = vsprintf(buff, format, args);
	va_end(args);

	buff[i] = '\0';

	console_write_color(buff, back, fore);
}

#define is_digit(c)	((c) >= '0' && (c) <= '9')

static int skip_atoi(const char **s)
{
	int i = 0;

	while (is_digit(**s)) {
		i = i * 10 + *((*s)++) - '0';
	}

	return i;
}

#define ZEROPAD		1	// pad with zero
#define SIGN	 	2   	// unsigned/signed long
#define PLUS    	4	// show plus
#define SPACE	  	8   	// space if plus
#define LEFT	 	16  	// left justified
#define SPECIAL		32  	// 0x
#define SMALL	  	64  	// use 'abcdef' instead of 'ABCDEF'

#define do_div(n,base) ({ \
		int __res; \
		__asm__("divl %4":"=a" (n),"=d" (__res):"0" (n),"1" (0),"r" (base)); \
		__res; })

static char *isnumber(char *str, int num, int base, int size, int precision, int type)
{
	char c, sign, tmp[36];
	const char *digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	int i;

	if (type & SMALL) {
		digits ="0123456789abcdefghijklmnopqrstuvwxyz";
	}
	if (type & LEFT) {
		type &= ~ZEROPAD;
	}
	if (base < 2 || base > 36) {
		return 0;
	}

	c = (type & ZEROPAD) ? '0' : ' ' ;

	if (type & SIGN && num < 0) {
		sign = '-';
		num = -num;
	} else {
		sign = (type&PLUS) ? '+' : ((type&SPACE) ? ' ' : 0);
	}

	if (sign) {
	      size--;
	}
	if (type & SPECIAL) {
		if (base == 16) {
			size -= 2;
		} else if (base == 8) {
			size--;
		}
	}
	i = 0;
	if (num == 0) {
		tmp[i++] = '0';
	} else {
		while (num != 0) {
			tmp[i++] = digits[do_div(num,base)];
		}
	}

	if (i > precision) {
		precision = i;
	}
	size -= precision;

	if (!(type&(ZEROPAD+LEFT))) {
		while (size-- > 0) {
			*str++ = ' ';
		}
	}
	if (sign) {
		*str++ = sign;
	}
	if (type & SPECIAL) {
		if (base == 8) {
			*str++ = '0';
		} else if (base == 16) {
			*str++ = '0';
			*str++ = digits[33];
		}
	}
	if (!(type&LEFT)) {
		while (size-- > 0) {
			*str++ = c;
		}
	}
	while (i < precision--) {
		*str++ = '0';
	}
	while (i-- > 0) {
		*str++ = tmp[i];
	}
	while (size-- > 0) {
		*str++ = ' ';
	}

	return str;
}

 int vsprintf(char *buff, const char *format, va_list args)
{
	int len;
	int i;
	char *str;
	char *s;
	int *ip;

	int flags;		// flags to number()

	int field_width;	// width of output field
	int precision;		// min. # of digits for integers; max number of chars for from string

	for (str = buff ; *format ; ++format) {
		if (*format != '%') {
			*str++ = *format;
			continue;
		}
			
		flags = 0;
		repeat:
			++format;		// this also skips first '%'
			switch (*format) {
				case '-': flags |= LEFT;
					  goto repeat;
				case '+': flags |= PLUS;
					  goto repeat;
				case ' ': flags |= SPACE;
					  goto repeat;
				case '#': flags |= SPECIAL;
					  goto repeat;
				case '0': flags |= ZEROPAD;
					  goto repeat;
			}
		
		// get field width
		field_width = -1;
		if (is_digit(*format)) {
			field_width = skip_atoi(&format);
		} else if (*format == '*') {
			// it's the next argument
			field_width = va_arg(args, int);
			if (field_width < 0) {
				field_width = -field_width;
				flags |= LEFT;
			}
		}

		// get the precision
		precision = -1;
		if (*format == '.') {
			++format;	
			if (is_digit(*format)) {
				precision = skip_atoi(&format);
			} else if (*format == '*') {
				// it's the next argument
				precision = va_arg(args, int);
			}
			if (precision < 0) {
				precision = 0;
			}
		}

		// get the conversion qualifier
		//int qualifier = -1;	// 'h', 'l', or 'L' for integer fields
		if (*format == 'h' || *format == 'l' || *format == 'L') {
			//qualifier = *format;
			++format;
		}

		switch (*format) {
		case 'c':
			if (!(flags & LEFT)) {
				while (--field_width > 0) {
					*str++ = ' ';
				}
			}
			*str++ = (unsigned char) va_arg(args, int);
			while (--field_width > 0) {
				*str++ = ' ';
			}
			break;

		case 's':
			s = va_arg(args, char *);
			len = strlen(s);
			if (precision < 0) {
				precision = len;
			} else if (len > precision) {
				len = precision;
			}

			if (!(flags & LEFT)) {
				while (len < field_width--) {
					*str++ = ' ';
				}
			}
			for (i = 0; i < len; ++i) {
				*str++ = *s++;
			}
			while (len < field_width--) {
				*str++ = ' ';
			}
			break;

		case 'o':
			str = isnumber(str, va_arg(args, unsigned long), 8,
				field_width, precision, flags);
			break;

		case 'p':
			if (field_width == -1) {
				field_width = 8;
				flags |= ZEROPAD;
			}
			str = isnumber(str, (unsigned long) va_arg(args, void *), 16,
				field_width, precision, flags);
			break;

		case 'x':
			flags |= SMALL;
		case 'X':
			str = isnumber(str, va_arg(args, unsigned long), 16,
				field_width, precision, flags);
			break;

		case 'd':
		case 'i':
			flags |= SIGN;
		case 'u':
			str = isnumber(str, va_arg(args, unsigned long), 10,
				field_width, precision, flags);
			break;
		case 'b':
			str = isnumber(str, va_arg(args, unsigned long), 2,
				field_width, precision, flags);
			break;

		case 'n':
			ip = va_arg(args, int *);
			*ip = (str - buff);
			break;

		default:
			if (*format != '%')
				*str++ = '%';
			if (*format) {
				*str++ = *format;
			} else {
				--format;
			}
			break;
		}
	}
	*str = '\0';

	return (str -buff);
}


static void print_stack_trace();

static elf_t kernel_elf;

void init_debug()
{
	// 从 GRUB 提供的信息中获取到内核符号表和代码地址信息
	kernel_elf = elf_from_multiboot(glb_mboot_ptr);
}

void print_cur_status()
{
	static int round = 0;
	uint16_t reg1, reg2, reg3, reg4;

	asm volatile ( 	"mov %%cs, %0;"
			"mov %%ds, %1;"
			"mov %%es, %2;"
			"mov %%ss, %3;"
			: "=m"(reg1), "=m"(reg2), "=m"(reg3), "=m"(reg4));

	// 打印当前的运行级别
	printk("%d: @ring %d\n", round, reg1 & 0x3);
	printk("%d:  cs = %x\n", round, reg1);
	printk("%d:  ds = %x\n", round, reg2);
	printk("%d:  es = %x\n", round, reg3);
	printk("%d:  ss = %x\n", round, reg4);
	++round;
}

void panic(const char *msg)
{
	printk("*** System panic: %s\n", msg);
	print_stack_trace();
	printk("***\n");
	
	// 致命错误发生后打印栈信息后停止在这里
	while(1);
}

void print_stack_trace()
{
	uint32_t *ebp, *eip;

	asm volatile ("mov %%ebp, %0" : "=r" (ebp));
	while (ebp) {
		eip = ebp + 1;
		printk("   [0x%x] %s\n", *eip, elf_lookup_symbol(*eip, &kernel_elf));
		ebp = (uint32_t*)*ebp;
	}
}


void cross_printf(const char *fmt, ...)  
{  
    va_list ap;
    char format_string[BUF_SIZE];

    va_start(ap, fmt);
    vsprintf(format_string, fmt, ap);
    va_end(ap);

#if defined(LINUX)  || defined(SMALLLINUX)
    printf(format_string);
    /*    fflush(stdout); */
#endif

#if defined(HURLEY)
    printk(format_string);
#endif 
}  

void cross_sprintf(char * format_string, const char *fmt, ...) {  
    va_list ap;
    va_start(ap, fmt);

#if defined(LINUX)
    vsnprintf(format_string, BUF_SIZE, fmt, ap);
#endif

#if defined(HURLEY) || defined(SMALLLINUX)
    vsprintf(format_string, fmt, ap);
#endif

    va_end(ap);
}  
/*(end console system)*/

/*(idt system)*/

/*(idt system end)*/

/*(file system)*/
void cross_write(char *filename, char *data, int single, int length) {
#ifdef _WIN32
    FILE *serial = NULL;
    serial = fopen(filename, "wb");
    fwrite(data, single, length, serial);
    fclose(serial);
#endif

#if defined(LINUX)
    FILE *serial = NULL;
    serial = fopen(filename, "w");
    fwrite(data, single, length, serial);
    fclose(serial);
#endif
}

void cross_read(char *file_name, int *sign, char *buf) {
#if defined(LINUX)
    FILE  * in, * fp;
    char file_tmp_name[256]="\0";
    int  file_size = 0;

    cross_sprintf(file_tmp_name,"%s",file_name);
    fp = fopen(file_tmp_name, "rb" );
    if( fp == 0 ) {
        return;	  
    }
    else {
        fseek( fp, 0, SEEK_END );
        file_size = ftell( fp );
        if (file_size >= BUF_SIZE) {
            *sign = BUF_SIZE;
            fclose(fp);
            return;
        }
        fseek( fp, 0, SEEK_SET );
        fread(buf, file_size, 1, fp );
        *sign = file_size;
    }
    fclose(fp);
#endif
}
/*(end file system)*/

/*(memeory system)*/
// 内核页目录区域
pgd_t pgd_kern[PGD_SIZE] __attribute__ ((aligned(PAGE_SIZE)));

// 内核页表区域
static pte_t pte_kern[PTE_COUNT][PTE_SIZE] __attribute__ ((aligned(PAGE_SIZE)));

void init_vmm()
{
	// 0xC0000000 这个地址在页目录的索引
	uint32_t kern_pte_first_idx = PGD_INDEX(PAGE_OFFSET);
	
	uint32_t i, j;
	for (i = kern_pte_first_idx, j = 0; i < PTE_COUNT + kern_pte_first_idx; i++, j++) {
		// 此处是内核虚拟地址，MMU 需要物理地址，所以减去偏移，下同
		pgd_kern[i] = ((uint32_t)pte_kern[j] - PAGE_OFFSET) | PAGE_PRESENT | PAGE_WRITE;
	}

	uint32_t *pte = (uint32_t *)pte_kern;
	// 不映射第 0 页，便于跟踪 NULL 指针
	for (i = 1; i < PTE_COUNT * PTE_SIZE; i++) {
		pte[i] = (i << 12) | PAGE_PRESENT | PAGE_WRITE;
	}

	uint32_t pgd_kern_phy_addr = (uint32_t)pgd_kern - PAGE_OFFSET;

	// 注册页错误中断的处理函数 ( 14 是页故障的中断号 )
	register_interrupt_handler(14, &page_fault);

	switch_pgd(pgd_kern_phy_addr);
}

void switch_pgd(uint32_t pd)
{
	asm volatile ("mov %0, %%cr3" : : "r" (pd));
}

void map(pgd_t *pgd_now, uint32_t va, uint32_t pa, uint32_t flags)
{ 	
	uint32_t pgd_idx = PGD_INDEX(va);
	uint32_t pte_idx = PTE_INDEX(va); 
	
	pte_t *pte = (pte_t *)(pgd_now[pgd_idx] & PAGE_MASK);
	if (!pte) {
		pte = (pte_t *)pmm_alloc_page();
		pgd_now[pgd_idx] = (uint32_t)pte | PAGE_PRESENT | PAGE_WRITE;

		// 转换到内核线性地址并清 0
		pte = (pte_t *)((uint32_t)pte + PAGE_OFFSET);
		bzero(pte, PAGE_SIZE);
	} else {
		// 转换到内核线性地址
		pte = (pte_t *)((uint32_t)pte + PAGE_OFFSET);
	}

	pte[pte_idx] = (pa & PAGE_MASK) | flags;

	// 通知 CPU 更新页表缓存
	asm volatile ("invlpg (%0)" : : "a" (va));
}

void unmap(pgd_t *pgd_now, uint32_t va)
{
	uint32_t pgd_idx = PGD_INDEX(va);
	uint32_t pte_idx = PTE_INDEX(va);

	pte_t *pte = (pte_t *)(pgd_now[pgd_idx] & PAGE_MASK);

	if (!pte) {
		return;
	}

	// 转换到内核线性地址
	pte = (pte_t *)((uint32_t)pte + PAGE_OFFSET);

	pte[pte_idx] = 0;

	// 通知 CPU 更新页表缓存
	asm volatile ("invlpg (%0)" : : "a" (va));
}

uint32_t get_mapping(pgd_t *pgd_now, uint32_t va, uint32_t *pa)
{
	uint32_t pgd_idx = PGD_INDEX(va);
	uint32_t pte_idx = PTE_INDEX(va);

	pte_t *pte = (pte_t *)(pgd_now[pgd_idx] & PAGE_MASK);
	if (!pte) {
	      return 0;
	}
	
	// 转换到内核线性地址
	pte = (pte_t *)((uint32_t)pte + PAGE_OFFSET);

	// 如果地址有效而且指针不为NULL，则返回地址
	if (pte[pte_idx] != 0 && pa) {
		 *pa = pte[pte_idx] & PAGE_MASK;
		return 1;
	}

	return 0;
}


// 物理内存页面管理的栈
static uint32_t pmm_stack[PAGE_MAX_SIZE+1];

// 物理内存管理的栈指针
static uint32_t pmm_stack_top;

// 物理内存页的数量
uint32_t phy_page_count;

void show_memory_map()
{
	uint32_t mmap_addr = glb_mboot_ptr->mmap_addr;
	uint32_t mmap_length = glb_mboot_ptr->mmap_length;

	printk("Memory map:\n");

	mmap_entry_t *mmap = (mmap_entry_t *)mmap_addr;
	for (mmap = (mmap_entry_t *)mmap_addr; (uint32_t)mmap < mmap_addr + mmap_length; mmap++) {
		printk("base_addr = 0x%X%08X, length = 0x%X%08X, type = 0x%X\n",
			(uint32_t)mmap->base_addr_high, (uint32_t)mmap->base_addr_low,
			(uint32_t)mmap->length_high, (uint32_t)mmap->length_low,
			(uint32_t)mmap->type);
	}
}

void init_pmm()
{
	mmap_entry_t *mmap_start_addr = (mmap_entry_t *)glb_mboot_ptr->mmap_addr;
	mmap_entry_t *mmap_end_addr = (mmap_entry_t *)glb_mboot_ptr->mmap_addr + glb_mboot_ptr->mmap_length;

	mmap_entry_t *map_entry;

	for (map_entry = mmap_start_addr; map_entry < mmap_end_addr; map_entry++) {

		// 如果是可用内存 ( 按照协议，1 表示可用内存，其它数字指保留区域 )
		if (map_entry->type == 1 && map_entry->base_addr_low == 0x100000) {
			
			// 把内核结束位置到结束位置的内存段，按页存储到页管理栈里
			// 最多支持512MB的物理内存
			uint32_t page_addr = map_entry->base_addr_low + (uint32_t)(kern_end - kern_start);
			uint32_t length = map_entry->base_addr_low + map_entry->length_low;

			while (page_addr < length && page_addr <= PMM_MAX_SIZE) {
				pmm_free_page(page_addr);
				page_addr += PMM_PAGE_SIZE;
				phy_page_count++;
			}
		}
	}
}

uint32_t pmm_alloc_page()
{
	assert(pmm_stack_top != 0, "out of memory");

	uint32_t page = pmm_stack[pmm_stack_top--];

	return page;
}

void pmm_free_page(uint32_t p)
{
	assert(pmm_stack_top != PAGE_MAX_SIZE, "out of pmm_stack stack");

	pmm_stack[++pmm_stack_top] = p;
}



// 申请内存块
static void alloc_chunk(uint32_t start, uint32_t len);

// 释放内存块
static void free_chunk(header_t *chunk);

// 切分内存块
static void split_chunk(header_t *chunk, uint32_t len);

// 合并内存块
static void glue_chunk(header_t *chunk);

static uint32_t heap_max = HEAP_START;

// 内存块管理头指针
static header_t *heap_first;

void init_heap()
{
	heap_first = 0;
}

void *kmalloc(uint32_t len)
{
	// 所有申请的内存长度加上管理头的长度
	// 因为在内存申请和释放的时候要通过该结构去管理
	len += sizeof(header_t);

	header_t *cur_header = heap_first;
	header_t *prev_header = 0;

	while (cur_header) {
		// 如果当前内存块没有被申请过而且长度大于待申请的块
		if (cur_header->allocated == 0 && cur_header->length >= len) {
			// 按照当前长度切割内存
			split_chunk(cur_header, len);
			cur_header->allocated = 1;
			// 返回的时候必须将指针挪到管理结构之后
			return (void *)((uint32_t)cur_header + sizeof(header_t));
		}
		// 逐次推移指针
		prev_header = cur_header;
		cur_header = cur_header->next;
	}

	uint32_t chunk_start;

	// 第一次执行该函数则初始化内存块起始位置
	// 之后根据当前指针加上申请的长度即可
	if (prev_header) {
		chunk_start = (uint32_t)prev_header + prev_header->length;
	} else {
		chunk_start = HEAP_START;
		heap_first = (header_t *)chunk_start;
	}

	// 检查是否需要申请内存页
	alloc_chunk(chunk_start, len);
	cur_header = (header_t *)chunk_start;
	cur_header->prev = prev_header;
	cur_header->next = 0;
	cur_header->allocated = 1;
	cur_header->length = len;
	
	if (prev_header) {
		prev_header->next = cur_header;
	}

	return (void*)(chunk_start + sizeof(header_t));
}

void kfree(void *p)
{
	// 指针回退到管理结构，并将已使用标记置 0
	header_t *header = (header_t*)((uint32_t)p - sizeof(header_t));
	header->allocated = 0;

	// 粘合内存块
	glue_chunk(header);
}

void alloc_chunk(uint32_t start, uint32_t len)
{
	// 如果当前堆的位置已经到达界限则申请内存页
	// 必须循环申请内存页直到有到足够的可用内存
	while (start + len > heap_max) {
		uint32_t page = pmm_alloc_page();
		map(pgd_kern, heap_max, page, PAGE_PRESENT | PAGE_WRITE);
		heap_max += PAGE_SIZE;
	}
}

void free_chunk(header_t *chunk)
{
	if (chunk->prev == 0) {
		heap_first = 0;
	} else {
		chunk->prev->next = 0;
	}

	// 空闲的内存超过 1 页的话就释放掉
	while ((heap_max - PAGE_SIZE) >= (uint32_t)chunk) {
		heap_max -= PAGE_SIZE;
		uint32_t page;
		get_mapping(pgd_kern, heap_max, &page);
		unmap(pgd_kern, heap_max);
		pmm_free_page(page);
	}
}

void split_chunk(header_t *chunk, uint32_t len)
{
	// 切分内存块之前得保证之后的剩余内存至少容纳一个内存管理块的大小
	if (chunk->length - len > sizeof (header_t)) {
		header_t *newchunk = (header_t *)((uint32_t)chunk + len);
		newchunk->prev = chunk;
		newchunk->next = chunk->next;
		newchunk->allocated = 0;
		newchunk->length = chunk->length - len;

		chunk->next = newchunk;
		chunk->length = len;
	}
}

void glue_chunk(header_t *chunk)
{
	// 如果该内存块后面有链内存块且未被使用则拼合
	if (chunk->next && chunk->next->allocated == 0) {
		chunk->length = chunk->length + chunk->next->length;
		if (chunk->next->next) {
			chunk->next->next->prev = chunk;
		}
		chunk->next = chunk->next->next;
	}

	// 如果该内存块前面有链内存块且未被使用则拼合
	if (chunk->prev && chunk->prev->allocated == 0) {
		chunk->prev->length = chunk->prev->length + chunk->length;
		chunk->prev->next = chunk->next;
		if (chunk->next) {
			chunk->next->prev = chunk->prev;
		}
		chunk = chunk->prev;
	}

	// 假如该内存后面没有链表内存块了直接释放掉
	if (chunk->next == 0) {
		free_chunk(chunk);
	}
}

void test_heap()
{
	printk_color(rc_black, rc_magenta, "Test kmalloc() && kfree() now ...\n\n");

	void *addr1 = kmalloc(50);
	printk("kmalloc    50 byte in 0x%X\n", addr1);
	void *addr2 = kmalloc(500);
	printk("kmalloc   500 byte in 0x%X\n", addr2);
	void *addr3 = kmalloc(5000);
	printk("kmalloc  5000 byte in 0x%X\n", addr3);
	void *addr4 = kmalloc(50000);
	printk("kmalloc 50000 byte in 0x%X\n\n", addr4);

	printk("free mem in 0x%X\n", addr1);
	kfree(addr1);
	printk("free mem in 0x%X\n", addr2);
	kfree(addr2);
	printk("free mem in 0x%X\n", addr3);
	kfree(addr3);
	printk("free mem in 0x%X\n\n", addr4);
	kfree(addr4);
}


void page_fault(pt_regs *regs)
{
	uint32_t cr2;
	asm volatile ("mov %%cr2, %0" : "=r" (cr2));

	printk("Page fault at EIP: 0x%x, virtual faulting address 0x%x\n", regs->eip, cr2);
	printk("Error code: %x\n", regs->err_code);

	// bit 0 为 0 指页面不存在内存里
	if ( !(regs->err_code & 0x1)) {
		printk_color(rc_black, rc_red, "Because the page wasn't present.\n");
	}
	// bit 1 为 0 表示读错误，为 1 为写错误
	if (regs->err_code & 0x2) {
		printk_color(rc_black, rc_red, "Write error.\n");
	} else {
		printk_color(rc_black, rc_red, "Read error.\n");
	}
	// bit 2 为 1 表示在用户模式打断的，为 0 是在内核模式打断的
	if (regs->err_code & 0x4) {
		printk_color(rc_black, rc_red, "In user mode.\n");
	} else {
		printk_color(rc_black, rc_red, "In kernel mode.\n");
	}
	// bit 3 为 1 表示错误是由保留位覆盖造成的
	if (regs->err_code & 0x8) {
		printk_color(rc_black, rc_red, "Reserved bits being overwritten.\n");
	}
	// bit 4 为 1 表示错误发生在取指令的时候
	if (regs->err_code & 0x10) {
		printk_color(rc_black, rc_red, "The fault occurred during an instruction fetch.\n");
	}

	while (1);
}


/*(memeory system)*/

void * raw_new_object(void);
void   recycle_raw_object(void * left);
void * cross_calloc(int count, int length) {
    void * mem = NULL;

    if(count * length == BUF_SIZE * sizeof(char)){
        mem = raw_new_object();
        memset(mem + sizeof(int), 0, BUF_SIZE - sizeof(int));
        *(int*)mem = BUF_SIZE;
    }
    else{
#ifdef HURLEY
        mem = kmalloc(sizeof(char) * ( sizeof(int) + count * length));
#endif

#if defined(LINUX) || defined(SMALLLINUX)
        mem = malloc(sizeof(char) * (sizeof(int) + count * length));
        if (mem == 0) {
            _exit(-1);
        }
#endif
        *(int*)mem = 0;
    }

    return (char*)mem + sizeof(int);
}

void * cross_malloc(int length) {
    return  cross_calloc(sizeof(char), length);
}

void cross_memcpy(void * dst, void * src, int length) {
#ifdef HURLEY
    memcpy((unsigned char*)dst, (unsigned char*)src, length);
#endif

#if defined(LINUX) || defined(SMALLLINUX)
    memcpy(dst, src, length);
#endif
}

void   cross_free(void *p) {
    if( *((int*)p - 1) == BUF_SIZE){
        recycle_raw_object( (void*)( *(long*)( (char*)p - sizeof(int) - sizeof(long) ) ));
    }
    else {
#if defined(LINUX) || defined(SMALLLINUX)
        free( (int*)p - 1);
#endif

#ifdef HURLEY
        kfree( (int*)p - 1);
#endif
    }
}

#define  NULLVALUE  999999
#define  ARABIC     26
typedef void * (*original_callback)(void * _left);
enum tokens {
    NUMBER = 'n', NAME, VALUE
};

typedef enum {
    EMPTY = 1,
    CLOSURE,
    INTEGER,
    CHARACTER,
    NETWORK,
    NOSQL,
    EVENT,
    TIME,
    STORAGE,
    DEBUG,
    JSON,
    SET,
    REDISLIST,
    HASH,
    THREAD,
    YIELD,
    STOP,
    NEXT,
    ISSTOP,
    STDIN,
    EOFSTDIN,
    DICTORY,
    FUN,
    DEFUN,
    DEFMACRO,
    VAR,
    QUOTE,
    LIST,
    QUOTEX,
    IF,
    PROGN,
    WHOLE,
    COMBI,
    EXCHANGE,
    TOP,
    EJECT,
    APPEND,
    PRESS,
    EVAL,
    SETQ,
    DEFINE,
    EQ,
    SYMBOL,
    LAMBDA,
    FORMAL,
    FUNCALL,
    SELF,
    EVIF = 50,
    EVMACRO,
    EVSETQ,
    EVDEFINE,
    EVARGSCOMBI,
    EVARGS,
    EVUNBINDVARS,
    EVPROGN,
    EVFUNCALL,
    EVFUNCALLMID,
    EVFUNCALLEND,
    EVPRESS,
    EVAPPEND,
    EVPROGNMID,
    EVPROGNEND,
    EFFUN = 100,
    EFYIELD,
    EFSTOP,
    EFNEXT,
    EFISSTOP,
    EFSTDIN,
    EFEOFSTDIN,
    EFDEFUN,
    EFDEFUNEND,
    EFDEFUNTAIL,
    EFDEFMACRO,
    EFDEFMACROEND,
    EFSAPPLY,
    EFSAPPLYX,
    EFSAPPLYXBEGIN,
    EFIF,
    EFELSEIF,
    EFENDIF,
    EFPROGNMID,
    EFQUOTE,
    EFQUOTEX,
    EFQUOTEXEND,
    EFSETQ,
    EFDEFINE,
    EFLAMBDA,
    EFFUNCALL,
    EFSELF,
    EFLAMBDAEND,
    EFFUNCALLEND,
    EFEJECT,
    EFWHOLE,
    EFAPPEND,
    EFPRESS,
    EFTOP,
    EFEXCHANGE,
    EFCOMBI,
    EFLISTBEGIN,
    EFLISTEND,
#ifdef LINUX
    ZFFORTH = 150,
    ZFCOLON,
    ZFDOT,
    ZFDUP,
    ZFEQ,
    ZFSAPPLY,
    ZFEND,
    ZFIF,
    ZFELSEIF,
    ZFTHEN,
    ZFADD,
    ZFMINUS,
    ZFSWAP,
    ZFMOD,
    ZFBIG,
    ZFOVER,
    ZFDROP,
    ZFRANDOM,
    ZFQUOTE,
    ZFDOTCHAR
#endif
} native_operator;


typedef struct type {
    native_operator em;
    union {
        original_callback f_data;
        long n_network;
        int i_data;
        long i_address;
        char* a_storage;
        char s_data[ARABIC];
        struct type * n_data;
    } u_data;
    int ref_count;
    int obj_length;
    struct type * next;
    void * mother;
} type;

typedef struct {
    void * mem_next;
    type value;
} wraptype;

type * primitive_empty = NULL;
type * global_defmacro = NULL;

#define  AMOUNT 10
type * primitive_small[2 * AMOUNT + 1 + 1] = {NULL};
type * primitive_evif = NULL;
type * primitive_evfuncall = NULL;
type * primitive_evfuncallmid = NULL;
type * primitive_evfuncallend = NULL;
type * primitive_evmacro = NULL;
type * primitive_evsetq = NULL;
type * primitive_evdefine = NULL;
type * primitive_evargscombi = NULL;
type * primitive_evargs = NULL;
type * primitive_evunbindvars = NULL;
type * primitive_evprogn = NULL;
type * primitive_evprognmid = NULL;
type * primitive_evprognend = NULL;
type * primitive_evpress = NULL;
type * primitive_evappend = NULL;
type * foundation_env; /*add  by  rosslyn ,2013.8.16,  for  ancient stone plan*/
type * primitive_effun = NULL;
type * primitive_efyield = NULL;
type * primitive_efstop = NULL;
type * primitive_efnext = NULL;
type * primitive_efisstop = NULL;
type * primitive_efstdin = NULL;
type * primitive_efeofstdin = NULL;
type * primitive_efdefun = NULL;
type * primitive_efdefunend = NULL;
type * primitive_efdefuntail = NULL;
type * primitive_efdefmacro = NULL;
type * primitive_efdefmacroend = NULL;
type * primitive_efsapply = NULL;
type * primitive_efsapplyx = NULL;
type * primitive_efsapplyxbegin = NULL;
type * primitive_efif = NULL;
type * primitive_efelseif = NULL;
type * primitive_efendif = NULL;
type * primitive_efprognmid = NULL;
type * primitive_efquote = NULL;
type * primitive_efquotex = NULL;
type * primitive_efquotexend = NULL;
type * primitive_efeval = NULL;
type * primitive_efevalend = NULL;
type * primitive_efsetq = NULL;
type * primitive_efdefine = NULL;
type * primitive_eflambda = NULL;
type * primitive_efself = NULL;
type * primitive_effuncall = NULL;
type * primitive_eflambdaend = NULL;
type * primitive_effuncallend = NULL;
type * primitive_efeject = NULL;
type * primitive_eftop = NULL;
type * primitive_efcombi = NULL;
type * primitive_efexchange = NULL;
type * primitive_efwhole = NULL;
type * primitive_efappend = NULL;
type * primitive_efpress = NULL;
type * primitive_eflistbegin = NULL;  /*add by rosslyn, 2014.6.14*/
type * primitive_eflistend = NULL;


type * primitive_zfsapply = NULL;
type * primitive_zfcolon = NULL;
type * primitive_zfend = NULL;
type * primitive_zfif = NULL;
type * primitive_zfelseif = NULL;
type * primitive_zfthen = NULL;

char global_filename[24] = "\0";

wraptype * mem_manager_unused = NULL;
wraptype * mem_manager_used = NULL;
wraptype * primitive_used = NULL;

wraptype * mem_manager_reserved = NULL;
wraptype * primitive_reserved = NULL;




typedef struct raw_type {
    void * content;
    void * mother;
} raw_type;

typedef struct {
    void * mem_next;
    raw_type value;
} raw_wraptype;


raw_wraptype * raw_mem_manager_unused = NULL;
raw_wraptype * raw_mem_manager_used = NULL;
raw_wraptype * raw_primitive_used = NULL;


raw_wraptype * raw_mem_manager_reserved = NULL;
raw_wraptype * raw_primitive_reserved = NULL;



#define  BIT_VOLUME           128
#define  EXTRA          20

/*forth_code*/
#define  FORTH_WORD   5000

void** global_forth_code = (void**)0;
long global_forth_code_ipc = 0;

void** local_forth_code = (void**)0;
long local_forth_code_ipc = 0;

void** forth_code_skip = (void**)0;
int forth_code_skip_ipc = 0;

void** local_recycle_contain = (void**)0;
int local_recycle_contain_ipc = 0;

void** forth_code_unfix = (void**)0;
int forth_code_unfix_ipc = 0;

void** forth_macro_skip = (void**)0;
long forth_macro_skip_ipc = 0;

int global_forth_current_code_ipc = 0;

int raw_global_count = 10;
void * raw_withdraw(void);
void * raw_new_object(void) {
    raw_type * outcome;
    void * content = NULL;

    if (!raw_mem_manager_unused) {
        if (raw_primitive_used->mem_next == NULL)
            raw_withdraw();
        raw_mem_manager_unused = raw_primitive_used->mem_next;
        raw_primitive_used->mem_next = NULL;
        raw_mem_manager_used = raw_primitive_used;
    }

    outcome = &(raw_mem_manager_unused->value);
    outcome->mother = raw_mem_manager_unused;
    raw_mem_manager_unused = raw_mem_manager_unused->mem_next;
    ((raw_wraptype *) outcome->mother)->mem_next = NULL;

    content = outcome->content;
    *(long *)content = (long)outcome;
    return content + sizeof(long);
}


void   recycle_raw_object(void * left){
    ((raw_wraptype*)(((raw_type*)(left))->mother))->mem_next = NULL;
    raw_mem_manager_used->mem_next = ((raw_wraptype*)(((raw_type*)(left))->mother));
    raw_mem_manager_used = ((raw_wraptype*)(((raw_type*)(left))->mother));
}

int global_count = 43021; /*modify  to  handle  macro  massive  character*/
int allocation = 0;
int recycled = 0;
int local_recycled = 0, local_allocation = 0;
void count_object(void) {
    cross_printf("%d  ", allocation);
}

void * original_withdraw(void * _left);
type * new_object(void) {
    type * outcome;

    if (!mem_manager_unused) {
        if (primitive_used->mem_next == NULL)
            original_withdraw(primitive_empty); /*modify  by  rosslyn  for High Order Machine 2013.4.23*/
        mem_manager_unused = primitive_used->mem_next;
        primitive_used->mem_next = NULL;
        mem_manager_used = primitive_used;
    }

    outcome = &(mem_manager_unused->value);
    outcome->mother = mem_manager_unused;
    mem_manager_unused = mem_manager_unused->mem_next;
    ((wraptype *) outcome->mother)->mem_next = NULL;
    outcome->ref_count = 1;
    outcome->obj_length = 1;

    allocation++;
    return outcome;
}

void * int_type(int i);

void * c_copy_atom(void * _right) {
    type * right = _right;
    if (right->em == EMPTY)
        return right;
    right->ref_count++;
    return  right;
}


void * c_cons(void * _left, void * _right) {
    type *type_data, * left = _left, * right = _right;
    char debug_inf[256] = "\0";

    if ((right-> em != EMPTY) && (right->em != LIST)) {
        cross_strcpy(debug_inf, "cons right value shoule be list or nil\r\n");
        cross_printf(debug_inf);
        return primitive_empty;
    }

    type_data = (type*)new_object();
    type_data->em = LIST;
    type_data->u_data.n_data = left;
    type_data->next = right;
    if (right->em == EMPTY) {
        type_data->obj_length = 1;
    }
    else {
        type_data->obj_length = right->obj_length + 1;
    }
    return type_data;
}

#define   c_atom(_left)   ( (type*)(_left))->em  == LIST ? 0 : 1
#define   c_car(_left)    ( (type*)(_left))->u_data.n_data
#define   c_cdr(_left)    ( (type*)(_left))->next

#define   c_caar(_left)   c_car(c_car(_left))
#define   c_cddr(_left)   c_cdr(c_cdr(_left))
#define   c_caddr(_left)  c_car( c_cddr(_left) )
#define   c_cdar(_left)   c_cdr(c_car(_left))
#define   c_cadr(_left)   c_car(c_cdr(_left))
#define   c_cadar(_left)  c_car(c_cdr(c_car(_left)))
#define   c_cadadr(_left) c_car(c_cdr(c_car(c_cdr(_left))))
#define   c_cddar(_left)  c_cdr(c_cdr(c_car(_left)))
#define   c_cdaar(_left)  c_cdr(c_car(c_car(_left)))
#define   c_caaar(_left)  c_car(c_car(c_car(_left)))
#define   c_caddar(_left) c_cadr(c_cdar(_left))
#define   c_cdddar(_left) c_cddr(c_cdar(_left))
#define   c_cadaar(_left) c_cadr(c_caar(_left))
#define   c_cddaar(_left) c_cddr(c_caar(_left))

#define   gc_atom_reserved_x(left)  { ((wraptype*)(((type*)(left))->mother))->mem_next = NULL; mem_manager_reserved->mem_next = ((wraptype*)(((type*)(left))->mother)); mem_manager_reserved = ((wraptype*)(((type*)(left))->mother));}
#define   gc_atom_x(left)  { ((wraptype*)(((type*)(left))->mother))->mem_next = NULL; mem_manager_used->mem_next = ((wraptype*)(((type*)(left))->mother));  mem_manager_used = ((wraptype*)(((type*)(left))->mother)); recycled++ ;}

void gc_atom_reserved(void * _left){
    type  * left = _left;
    if( (--left->ref_count ) == 0){
        gc_atom_reserved_x(left);
    }
}

void gc_atom(void * _left){
    type  * left = _left;
    if( (--left->ref_count ) == 0){
        if(left->em == STORAGE)     /*2014.7.24*/
            cross_free(left->u_data.a_storage);
        /* 2015.5.30
        if(left->em == VAR)    
            memset(left->u_data.s_data, 0, 10);
        left->em = EMPTY;
        */
        gc_atom_x(left);
    }
    if(left->ref_count < 0){
        cross_printf("ref_count error\r\n");
    }
}

void gc_reserved(void * _left) {
    type * left = _left;
    if (!left) {
        return;
    }
    if (left->em == EMPTY) {
        return;
    }
    if (left->em != LIST) {
        gc_atom_reserved(left);
    } 
    else {
        gc_reserved(c_car(left));
        gc_reserved(c_cdr(left));
        gc_atom_reserved(left);
    }
}

void gc_frame(void * _left) {
    type * left = _left;
    type * right = primitive_empty;
    if (left->em == EMPTY)
        return;
    else {
        right = c_cdr(left);
        gc_atom(left);
        gc_frame(right);
    }
}


void * left_print(void * _left) {
    type * left = _left;
    type * present;
    if (!left) {
        return primitive_empty;
    }
    switch (left->em) {
    case DICTORY:
        cross_printf("%s ", "dictory");
        break;
    case EMPTY:
        cross_printf("%s ", "nil");
        break;
    case DEBUG:
    case STORAGE:
        cross_printf("%s ", left->u_data.a_storage + sizeof(int));
        break;
    case LAMBDA:
        cross_printf("lambda ");
        break;
    case EVAL:
        cross_printf("eval ");
        break;
    case INTEGER:
        if (left->u_data.i_data == NULLVALUE)
            cross_printf("%s ", "nil");
        else
            cross_printf("%d ", left->u_data.i_data);
        break;
    case PROGN:
        cross_printf("%s  ", "progn");
        break;
    case VAR:
        cross_printf("%s  ", left->u_data.s_data);
        break;
        /*huuu , for less memory,there is no room for s_data of FUN,so use address instead.*/
    case FUN:
        cross_printf("%s   ", "fun");
        break;
    case QUOTE:
        cross_printf("%s  ", "quote");
        break;
    case DEFUN:
        cross_printf("%s   ", "defun");
        break;
    case FUNCALL:
        cross_printf("%s   ", "funcall");
        break;
    case SELF:
        cross_printf("%s   ", "self");
        break;
    case DEFMACRO:
        cross_printf("%s   ", "defmacro");
        break;
    case SETQ:
        cross_printf("%s  ", "setq");
        break;
    case DEFINE:
        cross_printf("%s  ", "define");
        break;
    case IF:
        cross_printf("%s  ", "if");
        break;
#ifdef LINUX
    case ZFSWAP:
        cross_printf("%s  ", "zfswap");
        break;
    case ZFBIG:
        cross_printf("%s  ", "zfbig");
        break;
    case ZFOVER:
        cross_printf("%s  ", "zfover");
        break;
    case ZFDROP:
        cross_printf("%s  ", "zfdrop");
        break;
    case ZFRANDOM:
        cross_printf("%s  ", "zfrandom");
        break;
    case ZFQUOTE:
        cross_printf("%s  ", "zfquote");
        break;
    case ZFDOTCHAR:
        cross_printf("%s  ", "zfdotchar");
        break;
    case ZFMOD:
        cross_printf("%s  ", "zfmod");
        break;
    case ZFMINUS:
        cross_printf("%s  ", "zfminus");
        break;
    case ZFADD:
        cross_printf("%s  ", "zfadd");
        break;
    case ZFEQ:
        cross_printf("%s  ", "zfeq");
        break;
    case ZFTHEN:
        cross_printf("%s  ", "zfthen");
        break;
    case ZFELSEIF:
        cross_printf("%s  ", "zfelseif");
        break;
    case ZFIF:
        cross_printf("%s  ", "zfif");
        break;
    case ZFDUP:
        cross_printf("%s  ", "zfdup");
        break;
    case ZFDOT:
        cross_printf("%s  ", "zfdot");
        break;
    case ZFCOLON:
        cross_printf("%s  ", "zfcolon");
        break;
    case ZFFORTH:
        cross_printf("%s  ", "zfforth");
        break;
#endif
    case LIST:
        cross_printf("  (  ");
        for (present = left; present->em != EMPTY; present = c_cdr (present)) {
            left_print(c_car(present));
        }
        cross_printf("  ) ");
        break;
    default:
        cross_printf(" type is  %d ", left->em);
        break;
    }
    return _left;
}

void gc(void * _left) {
    type * left = _left;
    if (!left) {
        return;
    }
    if (left->em == EMPTY) {
        return;
    }
    else if (left->em != LIST) {
        gc_atom(left);
    } 
    else {
        gc(c_car(left));
        gc(c_cdr(left));
        gc_atom(left);
    }
}

void * c_copy_tree(void * _right) {
    type * right = _right;
    if (right->em == EMPTY)
        return right;
    if ( c_atom ( c_car(right) )
         )
        return c_cons(c_copy_atom(c_car (right)), c_copy_tree(c_cdr (right)));
    return c_cons(c_copy_tree(c_car (right)), c_copy_tree(c_cdr (right)));
}

void * c_copy_type(void * _right) {
    type * right = _right;
    if (right->em == EMPTY)
        return right;
    if (right->em == LIST)
        return c_copy_tree(right);
    return c_copy_atom(right);
}

void * c_car_address(void * _left) {
    type * left = _left;
    if (left->em == EMPTY)
        return primitive_empty;
    /*  assert(left->em == LIST);*/
    return &(left->u_data.n_data);
}

void * original_cons(void * _left) {
    type * left = c_car(_left);
    type * right = c_cadr(_left);
    type * outcome = c_cons(left, right);
    gc_atom( c_cdr(_left));
    gc_atom(_left);

    return outcome;
}

void * original_cdr(void * _left) {
    /*macro  can't do ->em==EMPTY check on  the outter function call  mymachine  do  these forward  text replacement, so there needs special handleing  */
    type *left = c_car(_left);
    type *outcome;
    if (left->em == EMPTY) {
        outcome = primitive_empty;
        gc_atom(_left);
    } 
    else {
        outcome = c_cdar(_left);
        gc(c_caar(_left));
        gc_atom(c_car (_left));
        gc_atom(_left);
    }

    return outcome;
}

void * original_car(void * _left) {
    /*macro  can't do ->em==EMPTY check on  the outter function call mymachine  do  these forward  text replacement, so there needs special handleing  */
    type *left = c_car(_left);
    type *outcome;
    if (left->em == EMPTY) {
        outcome = primitive_empty;
        gc_atom(_left);
    } 
    else {
        outcome = c_caar(_left);
        gc(c_cdar(_left));
        gc_atom(c_car (_left));
        gc_atom(_left);
    }

    return outcome;
}

void * c_appdix(void * _left, void * _right);
void * original_cadr(void * _left) {
    type * outcome = c_car(c_cdar(_left));
    gc(c_cddar(_left));
    gc_atom(c_cdar (_left));

    gc(c_caar(_left));
    gc_atom(c_car (_left));
    gc_atom(_left);

    return outcome;
}

void * original_cddr(void * _left) {
    type * outcome = c_cddar(_left);
    gc(c_cadar(_left));
    gc_atom(c_cdar (_left));

    gc(c_caar(_left));
    gc_atom(c_car (_left));
    gc_atom(_left);

    return outcome;
}

void * original_caar(void * _left) {
    type * outcome = c_car(c_caar(_left));
    gc(c_cdr(c_caar(_left)));
    gc_atom(c_caar (_left));

    gc(c_cdar(_left));
    gc_atom(c_car (_left));
    gc_atom(_left);

    return outcome;
}

void * original_cdar(void * _left) {
    type * outcome = c_cdaar(_left);
    gc(c_caaar(_left));
    gc_atom(c_caar (_left));

    gc(c_cdar(_left));
    gc_atom(c_car (_left));
    gc_atom(_left);

    return outcome;
}

void * original_caddr(void * _left) {
    type * outcome = c_caddar(_left);

    gc(c_cdddar(_left));
    gc_atom(c_cddar (_left));

    gc(c_cadar(_left));
    gc_atom(c_cdar (_left));

    gc(c_caar(_left));
    gc_atom(c_car (_left));
    gc_atom(_left);

    return outcome;
}

void * original_cddar(void * _left) {
    type * outcome = c_cddaar(_left);

    gc(c_cadaar(_left));
    gc_atom(c_cdaar (_left));

    gc(c_caaar(_left));
    gc_atom(c_caar (_left));

    gc(c_cdar(_left));
    gc_atom(c_car (_left));
    gc_atom(_left);

    return outcome;
}


int c_eq(void * _left, void * _right) {
    type * left = _left;
    type * right = _right;
    int outcome;
    int outcomex;

    if (c_atom (left ) && c_atom (right)) {
        if (left->em == right->em) {
            switch (left->em) {
            case STORAGE:
                if( *(int*)(left->u_data.a_storage) != *(int*)(right->u_data.a_storage)){
                    outcome = 0;
                }
                else{
                    outcome = !strcmp(left->u_data.a_storage + sizeof(int), right->u_data.a_storage + sizeof(int)); 
                }
                break;
            case VAR:
                outcome = !strcmp(left->u_data.s_data, right->u_data.s_data); /*modify  by  rosslyn   2013.5.12*/
                break;
            case FUN:
                outcome = !strcmp(left->u_data.s_data, right->u_data.s_data); /*modify  by  rosslyn   2013.5.12*/
                outcomex = (left->u_data.f_data == right->u_data.f_data); /*modify  by  rosslyn   2014.6.14*/
                if(outcome != outcomex){
                    cross_printf("\r\n interesting\r\n");
                }
                break;
            case INTEGER:
                outcome = !(left->u_data.i_data - right->u_data.i_data); /*modify  by  rosslyn   2013.3.17*/
                break;
            default:
                outcome = 1;
                break;
            }
        } 
        else if( left->em == STORAGE && right->em == VAR ){
            outcome = !strcmp(left->u_data.a_storage + sizeof(int), right->u_data.s_data);
        }
        else if( right->em == STORAGE && left->em == VAR ){
            outcome = !strcmp(right->u_data.a_storage + sizeof(int), left->u_data.s_data);
        }
        else {
            outcome = 0;
        }
        /* follows  modify by rosslyn
        if (left->em == EMPTY && right->em == INTEGER
            && right->u_data.i_data == 0)
            outcome = 1;
        if (right->em == EMPTY && left->em == INTEGER
            && left->u_data.i_data == 0)
            outcome = 1;
        */
    } else
        outcome = 0;

    gc(_left);
    gc(_right);
    return outcome;
}

void * original_eq(void * _left) {
    type * left = _left;
    type * right =
        c_eq(c_car (left ), c_cadr (left)) ? int_type(1) : int_type(0);
    /*  gc(left);*/
    gc(c_cddr(left));
    gc_atom(c_cdr(left));
    gc_atom(left);
    /*this code is very important, becasue c_eq  has a little special meaning , and it destroy  the parameter,*/
    return right;
}

void * new_debug(char* _material, int size) {
    char head[26] = "\r\nDEBUG:\r\n";
    char* response = NULL; 
    int storage_size = 0;

    type * present = primitive_empty;
    present = new_object();
    present->em = DEBUG;
    storage_size = cross_strlen(head) + size + 1 + sizeof(int);
    response = (char*) cross_calloc(storage_size, sizeof(char));
    cross_memcpy(response + sizeof(int), head, cross_strlen(head));
    cross_memcpy(response + sizeof(int) + cross_strlen(head), _material, size);
    *(int*)response = storage_size - sizeof(int);
    present->u_data.a_storage = response;
    return present;
}

void * new_storage(char* _material, int size) {
    char* response = NULL; 
    int storage_size = 0;

    type * present = primitive_empty;
    present = new_object();
    present->em = STORAGE;
    storage_size = size + 1 + sizeof(int);
    response = (char*) cross_calloc(storage_size, sizeof(char));
    cross_memcpy(response + sizeof(int), _material, size);
    *(int*)response = storage_size - sizeof(int);
    present->u_data.a_storage = response;
    return present;
}

void * original_not(void * _left) {
    type * left = c_car(_left);
    type * outcome = primitive_empty;
    char debug_inf[256] = "\0";
    if (left->em != INTEGER) {
        cross_strcpy(debug_inf, "not first value should be a INTEGER\r\n");
        gc(_left);
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }

    if (left->u_data.i_data == 0){
        outcome = int_type(1);
    }
    else{
        outcome = int_type(0);
    }
    gc(_left);
    return outcome;
}

void * original_and(void * _left) {
    type * left = c_car(_left);
    type * right = c_cadr(_left);
    type  *outcome = primitive_empty;
    char debug_inf[256] = "\0";

    if (left->em != INTEGER) {
        cross_strcpy(debug_inf, "and first value should be a INTEGER\r\n");
        gc(_left);
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }
    if (right->em != INTEGER) {
        cross_strcpy(debug_inf, "and second value should be a INTEGER\r\n");
        gc(_left);
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }

    if(left->u_data.i_data == 1  &&  right->u_data.i_data == 1){
        outcome = int_type(1);
    }
    else{
        outcome = int_type(0);
    }

    gc(_left);
    return outcome;
}

void * original_or(void * _left) {
    type * left = c_car(_left);
    type * right = c_cadr(_left);
    type  *outcome = primitive_empty;
    char debug_inf[256] = "\0";

    if (left->em != INTEGER) {
        cross_strcpy(debug_inf, "and first value should be a INTEGER\r\n");
        gc(_left);
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }
    if (right->em != INTEGER) {
        cross_strcpy(debug_inf, "and second value should be a INTEGER\r\n");
        gc(_left);
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }

    if( (left->u_data.i_data == 1  ||  right->u_data.i_data == 1)){
        outcome = int_type(1);
    }
    else{
        outcome = int_type(0);
    }

    gc(_left);
    return outcome;
}

void * original_atom(void * _left) {
    type * left = _left;

    int value = c_atom ( c_car(left) );
    type * type_data = int_type(value);

    gc(_left);
    return type_data;
}

void * original_list(void * _left) {
    return _left;
}

void * c_appdix(void * _left, void * _right) {
    type * left = _left;
    type * right = _right;

    type * outcome = primitive_empty;

    if (left->em == EMPTY) {
        if(right->em == LIST){
            outcome = right;
        }
        else{
            outcome = c_cons(right, primitive_empty);
        } 
    } 
    else {
        outcome = c_cons(c_car ( left), c_appdix(c_cdr (left ), right));
        gc_atom(left);
    }
    return outcome;
}

void * c_append (void * _left,void * _right)
{
    type * left = (type*)_left;
    type * right = (type*)_right;

    type * result = primitive_empty;

    if( left->em == EMPTY) {
        result = right;
    }
    else {
        result = (type*)c_cons ( c_car ( left) ,
                                 c_append ( c_cdr (left ) , right ) );
        gc_atom(left);
    }
    return result;

}

void * c_list(void * left, ...) {
    type * ele_left;
    type * ele_right;
    va_list ap;
    ele_left = left;
    ele_left = c_cons(ele_left, primitive_empty);
    va_start(ap, left);

    while (1) {
        ele_right = va_arg(ap, void * );
        if (ele_right)
            ele_left = c_appdix(ele_left, c_cons(ele_right, primitive_empty));
        else {
            break;
        }
    }
    va_end(ap);
    return ele_left;
}

void * closure_type(void) {
    type * outcome = new_object();
    outcome->em = CLOSURE;
    return outcome;
}

void * int_type(int i) {
    type * outcome = primitive_empty;
    int abs_i = i;
    if (i < 0) {
        abs_i = 0 - i;
    }

    if(abs_i < AMOUNT){
        return c_copy_atom(primitive_small[AMOUNT + i]);
    }
    else{
        outcome = new_object();
        outcome->em = INTEGER;
        outcome->u_data.i_data = i;
        return outcome;
    }
}

void * operate_type(native_operator indicator) {
    type * outcome = new_object();
    outcome->em = indicator;
    return outcome;
}

void gc_frame(void *);
void gc(void *);
void * original_print(void * _left) {
    type * outcome;
    outcome = left_print(c_car (_left)); /*modify by chebing  2011.3.11*/
    cross_printf("\r\n");

    gc_atom(_left);
    return outcome;
}

typedef struct carriage {
    struct carriage* next;
}carriage;

typedef struct memory {
    struct memory* next;
    struct memory* length;
}memory;

typedef struct flexhash {
	memory** m_phash;
	int m_blocksize;
	size_t m_keysize;
	size_t m_valuesize;
	size_t m_datasize;
	carriage* m_pblocks;
	int (*m_compar)(void *, void *);
	unsigned int (*m_hashkey)(void *key);

	int m_max_count;
	int m_lastkey;

	int curoff;
	carriage* m_curblock;
	memory* m_pfreelist;

	int m_count;
	int m_hashsize;
}flexhash;

unsigned int hashkey(flexhash* obj, void* key) {
	unsigned int ihash = 0;
	char *pkey = (char *)key;
    size_t label = 0;
    int length = cross_strlen(key);
    if (length > obj->m_keysize) {
        length = obj->m_keysize;
    }

	for (label = 0; label < length; label++)
		ihash = (ihash << 5) + ihash + *pkey++;

	return ihash;
}

inline memory* getarea(flexhash* obj, void* key, unsigned int* ihash) {
    int length = cross_strlen(key);
	memory* pmemory;
    if (length > obj->m_keysize) {
        length = obj->m_keysize;
    }

	if (obj->m_hashkey == NULL)
		*ihash = hashkey(obj, key) % obj->m_hashsize;
	else
		*ihash = (*obj->m_hashkey)(key) % obj->m_hashsize;

	for (pmemory = obj->m_phash[*ihash]; pmemory != NULL; pmemory = pmemory->next) {
		if (obj->m_compar == NULL) {
			if ((*(int*)(&pmemory->length) == length) && (cross_memcmp(pmemory + 2, key, length) == 0)) {
				return pmemory;
            }
		}
		else {
			if ((*obj->m_compar)(pmemory + 2, key)) {
				return pmemory;
            }
		}
	}
	return NULL;
}

memory* getarea_delete(flexhash* obj, void* key, unsigned int* ihash) {
	memory* pmemory;
	memory* prevpmemory;
    int length = cross_strlen(key);
    if (length > obj->m_keysize) {
        length = obj->m_keysize;
    }

	if (obj->m_hashkey == NULL)
		*ihash = hashkey(obj, key) % obj->m_hashsize;
	else
		*ihash = (*obj->m_hashkey)(key) % obj->m_hashsize;

	for (pmemory = prevpmemory = obj->m_phash[*ihash]; pmemory != NULL; pmemory = pmemory->next) {
		if (obj->m_compar == NULL) {
			if ((*(int*)(&pmemory->length) == length) && (cross_memcmp(pmemory + 2, key, length) == 0))
				break;
		}
		else {
			if ((*obj->m_compar)(pmemory + 2, key)) {
				break;
            }
		}
		prevpmemory = pmemory;
	}

    if (prevpmemory != NULL && pmemory != NULL) {
        if (prevpmemory == pmemory) {
            obj->m_phash[*ihash] = pmemory->next;
            pmemory->next = NULL;
        }
        else {
            prevpmemory->next = pmemory->next;
            pmemory->next = NULL;
        }
    }

	return pmemory;
}

size_t getkeysize(flexhash* obj) {
    return obj->m_keysize;
}

size_t getvalsize(flexhash* obj) {
    return obj->m_valuesize;
}

carriage* create(carriage** phead, int block_size, int data_size) {
    carriage* carry = (carriage*) cross_calloc(sizeof(carriage) + block_size * data_size, sizeof(char));
    carry->next = *phead;
    *phead = carry;
    return carry;
}

void freedatachain(carriage* obj) {
	carriage* carry = obj;	
	
	while (carry != NULL) {	
		char* bytes = (char*)carry;		
		carriage* next = carry->next;		
		
		cross_free(bytes);
		carry = next;	
	}
}

memory* newmemory(flexhash* obj) {
    int i = 0;
    memory* pmemory = NULL;

	if (obj->m_pfreelist == NULL) {
		carriage* newblock = create(&obj->m_pblocks, obj->m_blocksize, obj->m_datasize * sizeof(memory));

		memory* pmemory = (memory*) (newblock + 1);
		pmemory += (obj->m_blocksize - 1) * (obj->m_datasize);
		for (i = obj->m_blocksize - 1; i >= 0; i--) {
			pmemory->next = obj->m_pfreelist;
            *(int*)(&pmemory->length) = 0; /* add by  rosslyn, for null value detect in dump_allitems */
			obj->m_pfreelist = pmemory;
			pmemory -= obj->m_datasize;
		}
	}

	pmemory = obj->m_pfreelist;
	obj->m_pfreelist = obj->m_pfreelist->next;
	obj->m_count++;
	obj->m_max_count++;

	return pmemory;
}

void freememory(flexhash* obj, memory* pmemory) {
    if (pmemory == NULL) {
        return;
    }

	memset(pmemory + 2, 0, (obj->m_datasize - 2) * sizeof(memory));
	pmemory->next = obj->m_pfreelist;
	obj->m_pfreelist = pmemory;
	obj->m_count--;
	/*if (m_count == 0) 	removeall();*/
}

flexhash* init_flexhash(int ihash, int iblock, size_t ikey, size_t ivalue, int (*compar)(void *, void *), unsigned int (*outhashkey)(void *key)) {
	int imod = (ikey + ivalue) % sizeof(memory);
	int idiv = (ikey + ivalue) / sizeof(memory);

    flexhash* obj = (flexhash*) cross_calloc(sizeof(flexhash), 1);

	obj->m_hashsize = ihash;
	obj->m_blocksize = iblock;
	obj->m_keysize = ikey;
	obj->m_valuesize = ivalue;
	obj->m_compar = compar;
	obj->m_hashkey = outhashkey;
	if (imod) {
		obj->m_datasize = 3 + idiv;
    }
	else {
		obj->m_datasize = 2 + idiv;
    }

	obj->m_phash = NULL;
	obj->m_count = 0;
	obj->m_max_count = 0;
	obj->m_pfreelist = NULL;
	obj->m_pblocks = NULL;

	obj->m_phash = (memory**)cross_calloc(obj->m_hashsize, sizeof(memory*));
	obj->m_lastkey = obj->m_hashsize / 2;
	
    /*	obj->m_block = (char *)calloc(obj->m_keysize, obj->m_valuesize);*/
    return obj;
}

void removeall(flexhash* obj) {
	if (obj->m_phash != NULL)
	{
		cross_free(obj->m_phash);
		obj->m_phash = NULL;
	}

	obj->m_pfreelist = NULL;
    freedatachain(obj->m_pblocks);
	obj->m_count = 0;
	obj->m_max_count = 0;
	obj->m_pblocks = NULL;
}

void recreate(flexhash* obj) {
	removeall(obj);
	obj->m_phash = (memory**)cross_calloc(obj->m_hashsize, sizeof(memory*));
}

void destroy(flexhash* obj) {
	removeall(obj);
    /*	free(obj->m_block);*/
}

int setat(flexhash* obj, void* key, void* newvalue) {
	memory* pmemory;
	unsigned int ihash;
    int length = cross_strlen(key);
    if (length > obj->m_keysize) {
        length = obj->m_keysize;
    }

	if ((pmemory = getarea(obj, key, &ihash)) == NULL) {
		pmemory = newmemory(obj);
		
        *(int*)(&pmemory->length) = length;  /*add  by rosslyn, 2015/6/27*/
		cross_memcpy(pmemory + 2, key, length);
		cross_memcpy((char*)(pmemory + 2) + obj->m_keysize, newvalue, obj->m_valuesize);
		pmemory->next = obj->m_phash[ihash];
		obj->m_phash[ihash] = pmemory;
		return 0;
	}

    *(int*)(&pmemory->length) = length;  /*add  by rosslyn, 2015/6/27*/
	cross_memcpy((char*)(pmemory + 2) + obj->m_keysize, newvalue, obj->m_valuesize);
	return 1;
}

int delete(flexhash* obj, void* key) {
	unsigned int ihash = 0;
	memory* pmemory = getarea_delete(obj, key, &ihash);
	if (pmemory == NULL)	return 0;

    *(int*)(&pmemory->length) = 0;
	freememory(obj, pmemory);
	return 1;
}

int getat(flexhash* obj, void* key, void* value) {
	unsigned int ihash = 0;
	memory* pmemory = getarea(obj, key, &ihash);
	if (pmemory == NULL)	{
        return 0;
    }
	
	if(value)
		cross_memcpy(value, (char*)(pmemory + 2) + obj->m_keysize, obj->m_valuesize);
	return 1;
}

int getflexhashsize(flexhash* obj) {
	return obj->m_count;
}

int dump_allitems(flexhash* obj, void* buffer) {
	int count = 0;
	carriage* carry = obj->m_pblocks;

	while (carry != NULL) {
		carriage* next = carry->next;
		memory *single = (memory *)(carry + 1);

		int label;
		for (label = 0; label < obj->m_blocksize; label++) {
            if( *(int*)(&single->length) != 0 ) {
                /* if (cross_memcmp(obj->m_block, single + 2, obj->m_keysize + obj->m_valuesize) != 0) { */
                cross_memcpy(buffer, (single + 2), obj->m_keysize + obj->m_valuesize);
                buffer = (char*)buffer + obj->m_keysize + obj->m_valuesize;

                count++;
                if(count > obj->m_count) {
                    return 1;
                }
            }
			single += obj->m_datasize;
		}
		carry = next;
	}

	return 0;
}

int next(flexhash* obj, void* key, void* val, size_t* iter) {
	int block_no = (int)(*iter / obj->m_blocksize);
	int block_offset = (int)(*iter % obj->m_blocksize);
    int label = 0;

	carriage* carry = obj->m_pblocks;

	while (carry != NULL && block_no-- > 0)
		carry = carry->next;

	while(carry) {
		memory *single = (memory *)(carry + 1) + block_offset * obj->m_datasize;
		
		for (label = block_offset; label< obj->m_blocksize; label++) {
			*iter += 1;
            if( *(int*)(&single->length) != 0 ) {
                /* if (cross_memcmp(obj->m_block, single + 2, obj->m_keysize + obj->m_valuesize) != 0) { */
				if(key) {
					cross_memcpy(key, single + 2, obj->m_keysize);
                }
				if(val) {
					cross_memcpy(val, (char*)(single + 2) + obj->m_keysize , obj->m_valuesize);
                }
				return 1;
			}
			single += obj->m_datasize;
		}
		carry = carry->next;
		block_offset = 0;
	}

	return 0;
}

#define FLEXHASH

#ifdef FLEXHASH
typedef struct entry {
    char* word;
    void* _env;
    struct entry* left;
    struct entry* right;
    union {
        void* _expr;
        original_callback _address;
    } u_data;
}entry;

flexhash * global_frame = NULL;
flexhash * global_stack = NULL;
flexhash * global_stream = NULL;
flexhash * global_lambda = NULL;
flexhash * global_defun = NULL;
flexhash * global_var = NULL;
flexhash * global_define = NULL;
flexhash * global_primitive = NULL;

void assign(char* word, void* _expr, void* _env, flexhash** e) { /* flexhash** only for compatility */
    entry obj;
    obj.word = word;
    obj._env = _env;
    obj.u_data._expr = _expr;
    setat(*e, word, &obj);
}

entry lookup(char* word, flexhash* e) {
    entry outcome;
    if (0 == getat(e, word, &outcome)) {
        outcome._env = NULL;
        outcome.u_data._expr = NULL;
        return outcome;
    }
    else {
        return outcome;
    }
}

flexhash * freeentry(char* word, flexhash* e) {
    entry outcome;
    if (0 == getat(e, word, &outcome)) {
        cross_printf("\r\n impossible entry\r\n");
    }
    else {
        delete(e, word);
    }
    return e; /* global_lambda = freeentry("kkk", global_lambda);*/
}

void destroy_lambda_avl(flexhash * e) {
    int count = getflexhashsize(e);
    int keysize = getkeysize(e);
    int valuesize = getvalsize(e);
    char* buf = (char*)cross_calloc(count, keysize + valuesize);
    int status = dump_allitems(e, buf);
    int i = 0;
    char* key = NULL;
    char* value = NULL;
    entry outcome;
    void * _env = NULL;
    if (status == 0 ) {
        for (i = 0; i < count; i++) {
            key = buf + i * (keysize + valuesize);
            value = buf + i * (keysize + valuesize) + keysize;
            cross_memcpy(&outcome, value, valuesize);
            _env = outcome._env;
            if (_env == NULL) {
            } 
            else {
                gc(c_car (_env));
            }
            gc_atom(_env);
            delete(e, key);
        } 
    }
    cross_free (buf);
}

void destroy_var_avl(flexhash * e) {
    int count = getflexhashsize(e);
    int keysize = getkeysize(e);
    int valuesize = getvalsize(e);
    char* buf = (char*)cross_calloc(count, keysize + valuesize);
    int status = dump_allitems(e, buf);
    int i = 0;
    char* key = NULL;
    char* value = NULL;
    entry outcome;
    type * mid_expr = NULL;
    if (status == 0 ) {
        for (i = 0; i < count; i++) {
            key = buf + i * (keysize + valuesize);
            value = buf + i * (keysize + valuesize) + keysize;
            cross_memcpy(&outcome, value, valuesize);
            mid_expr = outcome.u_data._expr;
            if (mid_expr == NULL) {
            } 
            else {
                gc(mid_expr);
            }
            delete(e, key);
        } 
    }
    cross_free (buf);
    /*
    type * mid_expr = NULL;

    if(e == NULL){
        return;
    }
    destroy_var_avl(e->left);
    destroy_var_avl(e->right);

    mid_expr = e->u_data._expr;
    if (mid_expr == NULL) {
    } else {
        gc(mid_expr);
    }

    cross_free(e);
    */
}

void assign_primitive(char* word, original_callback _address, flexhash** e) {
    entry obj;
    obj.word = word;
    obj.u_data._address = _address;
    setat(*e, word, &obj);
}

original_callback lookup_primitive(char* word, flexhash* e) {
    entry outcome;
    if (0 == getat(e, word, &outcome)) {
        return NULL;
    }
    else{
        return outcome.u_data._address;
    }
}


void * original_destroy(void * _left) {
    destroy_var_avl(global_var);

    destroy_lambda_avl(global_lambda);

    original_withdraw(primitive_empty);
    gc(_left);
    return primitive_empty;
}

#else
typedef struct entry {
    char *word;
    void * _env;
    struct entry *left;
    struct entry *right;
    union {
        void * _expr;
        original_callback _address;
    } u_data;
}entry;

entry * global_frame = NULL;
entry * global_stack = NULL;
entry * global_stream = NULL;
entry * global_lambda = NULL;
entry * global_defun = NULL;
entry * global_var = NULL;
entry * global_define = NULL;

entry * global_primitive = NULL;



void  * copystring(char *_str){
    char * str = (char *) cross_calloc (cross_strlen(_str) + 5, sizeof(char));
    cross_strcpy(str, _str);
    return  str;
}

int wordcompare(char *s1, char *s2){
    if (s1[0] > s2[0]) {
        return  1;
    }
    else if (s1[0] < s2[0]) {
        return -1;
    }
    else{
        return strcmp(s1, s2);
    }
}

void assign(char * word, void * _expr, void * _env, entry ** e) {
    int isbigger;
	
    if (!*e) {
        *e = ( entry *) cross_calloc(sizeof( entry), 1);
        (*e)->word = (char *) copystring(word);
        (*e)->_env = _env;
        (*e)->u_data._expr = _expr;
        (*e)->left = (*e)->right = NULL;
    }
    else {
        isbigger = wordcompare((*e)->word, word);
        if (isbigger == 0) {
            (*e)->_env = _env;
            (*e)->u_data._expr = _expr;
        }
        else if (isbigger > 0){
            assign(word, _expr, _env, &((*e)->left));
        }
        else{
            assign(word, _expr, _env, &((*e)->right));
        }
    }	
}

entry * freeentry_internel(char * word, entry * e, int depth) {
    int isbigger;
    entry *contain = NULL;

    if (e) {
        isbigger = wordcompare(e->word, word);
        if (isbigger == 0) {
            if( e->left ){
                if( e->right ){
                    contain = e->left;
                    while(contain->right){
                        contain = contain->right;
                    }
                    contain->right = e->right;
                }
                contain = e->left;
            }
            else {
                contain = e->right;
            }
            cross_free(e->word);
            cross_free(e);
            return  contain;
        }
        else if (isbigger > 0){
            e->left = freeentry_internel(word, e->left, depth + 1);
            return e;
        }
        else{
            e->right = freeentry_internel(word, e->right, depth + 1);
            return e;
        }
    }	
    return NULL;
}

entry * freeentry(char * word, entry * e) {
    return freeentry_internel(word, e , 0);
}

void destroy_lambda_avl(entry * e) {
    void * _env = NULL;
    if(e == NULL){
        return;
    }
    destroy_lambda_avl(e->left);
    destroy_lambda_avl(e->right);

    _env = e->_env;
    if (_env == NULL){
    } 
    else {
        gc(c_car (_env));
    }
    gc_atom(_env);

    cross_free(e->word);
    cross_free(e);
}


void destroy_var_avl(entry * e) {
    type * mid_expr = NULL;

    if(e == NULL){
        return;
    }
    destroy_var_avl(e->left);
    destroy_var_avl(e->right);

    mid_expr = e->u_data._expr;
    if (mid_expr == NULL) {
    } 
    else {
        gc(mid_expr);
    }

    cross_free(e->word);
    cross_free(e);
}


entry lookup(char * word, entry * e) {
    int isbigger;
    entry outcome;
	
    if (e == NULL) {
        outcome._env = NULL;
        outcome.u_data._expr = NULL;
        return outcome;
    }
    else {
        isbigger = wordcompare(e->word, word);
        if (isbigger == 0) {
            outcome._env = e->_env;
            outcome.u_data._expr = e->u_data._expr;
            return outcome;
        }
        else if (isbigger > 0)
            {
                return  lookup(word, e->left);
            }
        else
            {
                return  lookup(word, e->right);
            }
    }	
}

void assign_primitive(char * word, original_callback _address, entry **e) {
    int isbigger;
	
    if (!*e) {
        *e = ( entry *) cross_calloc(sizeof( entry), 1);
        (*e)->word = (char *) copystring(word);
        (*e)->u_data._address = _address;
        (*e)->left = (*e)->right = NULL;
    }
    else {
        isbigger = wordcompare((*e)->word, word);
        if (isbigger == 0) {
            (*e)->u_data._address = _address;
        }
        else if (isbigger > 0)
            {
                assign_primitive(word, _address, &((*e)->left));
            }
        else
            {
                assign_primitive(word, _address, &((*e)->right));
            }
    }	
}

original_callback lookup_primitive(char * word, entry * e) {
    int isbigger;
	
    if (e == NULL) {
        return NULL;
    }
    else {
        isbigger = wordcompare(e->word, word);
        if (isbigger == 0) {
            return e->u_data._address;
        }
        else if (isbigger > 0)
            {
                return  lookup_primitive(word, e->left);
            }
        else
            {
                return  lookup_primitive(word, e->right);
            }
    }	
}


void * original_destroy(void * _left) {
    /*need not do  follow code, think why, 2014.8.15 */

    destroy_var_avl(global_var);
    global_var = NULL;

    destroy_lambda_avl(global_lambda);
    global_lambda = NULL;

    original_withdraw(primitive_empty); /*modify  by  rosslyn  for High Order Machine 2013.4.23 */
    gc(_left);
    return primitive_empty;
}


#endif

void * fun_type(void * name);

void * original_display(void * _left) {
    char buf[256] = "\0";
    char debug_inf[256] = "\0";
    cross_sprintf(buf, "\r\nmemory unit increment is  %d\r\n", allocation - local_allocation);
    local_allocation = allocation;
    cross_sprintf(debug_inf, "%smemory unit decrement is  %d\r\n", buf, recycled - local_recycled);
    local_recycled = recycled;
    gc(_left);
    return left_print(new_storage(debug_inf, cross_strlen(debug_inf)));
}

void * original_withdraw(void * _left) {
    wraptype * handle = NULL;

    mem_manager_reserved = primitive_reserved->mem_next;

    while (mem_manager_reserved) {
        handle = mem_manager_reserved->mem_next;
        /*    assert(mem_manager_reserved);*/
        mem_manager_used->mem_next = mem_manager_reserved;
        mem_manager_used = mem_manager_reserved;
        mem_manager_reserved = handle;
        recycled++;
    }

    mem_manager_reserved = primitive_reserved;
    primitive_reserved->mem_next = NULL;
    gc(_left);
    return primitive_empty;
}

void * raw_withdraw(void) {
    raw_wraptype * handle = NULL;
    raw_mem_manager_reserved = raw_primitive_reserved->mem_next;

    while (raw_mem_manager_reserved) {
        handle = raw_mem_manager_reserved->mem_next;
        raw_mem_manager_used->mem_next = raw_mem_manager_reserved;
        raw_mem_manager_used = raw_mem_manager_reserved;
        raw_mem_manager_reserved = handle;
    }

    raw_mem_manager_reserved = raw_primitive_reserved;
    raw_primitive_reserved->mem_next = NULL;
    return NULL;
}

void * original_big(void * _left) {
    int outcome;
    char debug_inf[256] = "\0";
    type * left = c_car (_left ), *right = c_cadr (_left);
    if (left->em != INTEGER) {
        cross_strcpy(debug_inf, "big first value should be a INTEGER\r\n");
        gc(_left);
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }
    if (right->em != INTEGER) {
        cross_strcpy(debug_inf, "big second value should be a INTEGER\r\n");
        gc(_left);
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }

    outcome = ((type *) left)->u_data.i_data - ((type *) right)->u_data.i_data;
    gc(_left);
    if (outcome > 0){
        return int_type(1);
    }
    else{
        return int_type(0);
    }
}

void * original_small(void * _left) {
    int outcome;
    char debug_inf[256] = "\0";
    type * left = c_car (_left ), *right = c_cadr (_left);
    if (left->em != INTEGER) {
        cross_strcpy(debug_inf, "small first value should be a INTEGER\r\n");
        gc(_left);
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }
    if (right->em != INTEGER) {
        cross_strcpy(debug_inf, "small second value should be a INTEGER\r\n");
        gc(_left);
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }

    outcome = ((type *) left)->u_data.i_data - ((type *) right)->u_data.i_data;
    gc(_left);
    if (outcome < 0) {
        return int_type(1);
    }
    else{
        return int_type(0);
    }
}

void * original_mul(void * _left) {
    int outcome;
    char debug_inf[256] = "\0";
    type * left = c_car (_left ), *right = c_cadr (_left);
    if (left->em != INTEGER) {
        cross_strcpy(debug_inf, "mul first value should be a INTEGER\r\n");
        gc(_left);
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }
    if (right->em != INTEGER) {
        cross_strcpy(debug_inf, "mul second value should be a INTEGER\r\n");
        gc(_left);
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }

    outcome = ((type *) left)->u_data.i_data * ((type *) right)->u_data.i_data;
    gc(_left);
    return int_type(outcome);
}

void * original_divi(void * _left) {
    int outcome;
    char debug_inf[256] = "\0";
    type * left = c_car (_left ), *right = c_cadr (_left);
    if (left->em != INTEGER) {
        cross_strcpy(debug_inf, "divi first value should be a INTEGER\r\n");
        gc(_left);
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }
    if (right->em != INTEGER) {
        cross_strcpy(debug_inf, "divi second value should be a INTEGER\r\n");
        gc(_left);
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }

    outcome = ((type *) left)->u_data.i_data
        / ((type *) right)->u_data.i_data;
    gc(_left);
    return int_type(outcome);
}

void * original_mod(void * _left) {
    int outcome;
    char debug_inf[256] = "\0";
    type * left = c_car (_left ), *right = c_cadr (_left);
    if (left->em != INTEGER) {
        cross_strcpy(debug_inf, "mod first value should be a INTEGER\r\n");
        gc(_left);
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }
    if (right->em != INTEGER) {
        cross_strcpy(debug_inf, "mod second value should be a INTEGER\r\n");
        gc(_left);
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }

    outcome = ((type *) left)->u_data.i_data
        % ((type *) right)->u_data.i_data;
    gc(_left);
    return int_type(outcome);
}

void gc(void * _left);
void * original_add(void * _left) {
    type * present;
    type * left = _left;
    int outcome = 0;
    for (present = left; present->em != EMPTY; present = c_cdr (present)
         )
        outcome += ((type *) c_car(present))->u_data.i_data;
    gc(_left);
    return int_type(outcome);
}

void * original_minus(void * _left) {
    type * present;
    type * left = _left;
    int value = ((type *) c_car(left))->u_data.i_data;
    for (present = c_cdr (left ); present->em != EMPTY; present =
             c_cdr (present)
         )
        value -= ((type *) c_car(present))->u_data.i_data;
    gc(_left);
    return int_type(value);
}

void * original_charp(void * _left) {
    type * left = (type *) c_car(_left);
    type * outcome = primitive_empty;
    char * p = (char *) (left->u_data.s_data);
    int len = cross_strlen(p);

    if (left->em == VAR && len != (ARABIC - 1) ) {
        outcome = int_type(1);
    }
    else {
        outcome = int_type(0);
    }

    gc(_left);
    return outcome;
}

void * original_storagep(void * _left) {
    type * left = (type *) c_car(_left);
    type * outcome = primitive_empty;

    if (left->em == STORAGE) {
        outcome = int_type(1);
    }
    else {
        outcome = int_type(0);
    }

    gc(_left);
    return outcome;
}

void * original_debugp(void * _left) {
    type * left = (type *) c_car(_left);
    type * outcome = primitive_empty;

    if (left->em == DEBUG) {
        outcome = int_type(1);
    }
    else {
        outcome = int_type(0);
    }

    gc(_left);
    return outcome;
}

void * original_digitp(void * _left) {
    type * left = _left;
    type * outcome =
        ((type *) c_car(left))->em == INTEGER ?
        int_type(1) : int_type(0);
    gc(_left);
    return outcome;
}

void * original_random(void * _left) {
    int randvalue = 0, leftvalue = 0;
    unsigned int unixtime = 0;
    type * outcome = primitive_empty;
    type * left = c_car(_left);
    char debug_inf[256] = "\0";

    if (left->em != INTEGER) {
        cross_strcpy(debug_inf, "random first value should be a INTEGER\r\n");
        gc(_left);
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }
    leftvalue = left->u_data.i_data;
    unixtime = cross_rand();
    randvalue = unixtime % leftvalue;

    if (randvalue == 0) /*rand == left  is  ok  in  lisa progn , but  0 is not , 2013.3.2*/
        randvalue = leftvalue;

    outcome = int_type(randvalue);
    gc(_left);
    return outcome;
}

void* handleurl( char* url, int* _protocol, char* _host, int* _port, char* _file);
void* requesturl( int protocol, char* host, unsigned short port, char* file, char* referer, char* user_agent, char* auth_token, char** args, int argc);
void * original_eval(void *_left);

void* original_fflush(void *_left) {
    gc(_left);
    return primitive_empty;
}

void * original_itoa(void *_left) {
    type * left = c_car(_left);
    type * outcome = primitive_empty;
    int val = left->u_data.i_data;
    outcome = new_object();
    outcome->em = VAR;
    cross_sprintf(outcome->u_data.s_data, "%d", val);

    gc(_left);
    return outcome;
}


void * original_len(void *_left) {
    type * outcome = primitive_empty;
    type * left = c_car(_left);
    int val = left->obj_length;
    outcome = int_type(val);

    gc(_left);
    return outcome;
}

void * original_strlen(void *_left) {
    type * outcome = primitive_empty;
    type * left = c_car(_left);
    int value = 0;
    char debug_inf[256] = "\0";

    if(left->em == VAR){
        value = cross_strlen(left->u_data.s_data);
    }
    else if(left->em == STORAGE){
        value = *(int*)(left->u_data.a_storage) - 1;
    }
    else if(left->em == EMPTY){
        value = 0;
    }
    else {
        cross_strcpy(debug_inf, "cross_strlen first value should be a VAR or STORAGE\r\n");
        gc(_left);
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }

    outcome = int_type(value);
    gc(_left);
    return outcome;
}

void * original_atoi(void *_left) {
    type * left = c_car(_left);
    type * outcome = primitive_empty;

    outcome = new_object();
    outcome->em = INTEGER;
    if(left->em == EMPTY){
        outcome->u_data.i_data = 0;
    }
    else if(left->em == VAR){
        outcome->u_data.i_data = cross_atoi(left->u_data.s_data);
    }
    else if(left->em == STORAGE){
        outcome->u_data.i_data = cross_atoi(left->u_data.a_storage + sizeof(int));
    }
    else{}

    gc(_left);

    return outcome;
}

void ** c_bindvars(void * _left, void * _right, void ** _env);
void c_unbindvars(void ** _env);
void * var_type(char * name);

#ifdef LINUX
/*(network system)*/
int close_tcp_socket(int sockfd) {
#ifdef _WIN32
    return closesocket(sockfd);
#else
    return close(sockfd);
#endif
}

static void strencode( char* to, char* from ) {
    int tolen;
    for ( tolen = 0; *from != '\0'; ++from ){
        if ( isalnum(*from) || strchr( "/_.", *from ) != (char*) 0 ){
            *to = *from;
            ++to;
            ++tolen;
        }
        else{
            (void) sprintf( to, "%c%02x", '%', *from );
            to += 3;
            tolen += 3;
        }
    }
    *to = '\0';
}

void * original_urlencode(void * _left) {
    type * left = c_car(_left);
    type * present = NULL;
    type * outcome = primitive_empty;
    unsigned char * source = NULL;
    char   response[1024] = "\0";
    char * responsex = NULL;
    int    storage_size = 0;
    char debug_inf[256] = "\0";

    if(left->em == VAR){
        source = left->u_data.s_data;
    }
    else if(left->em == STORAGE){
        source = left->u_data.a_storage + sizeof(int);
    }
    else{
        cross_printf("urlencode first value shoule be VAR or STORAGE");
        gc(_left);
        return primitive_empty;
    }
    strencode(response, source);
    outcome = new_storage(response, strlen(response));
    cross_free(response);
    gc(_left);
    return outcome;
}

void * recv_mesg(int sockfd){
    int len = 0, sign = 0 , file_buffer_count = 0;
    int status = 0;
    int batch_size = BUF_SIZE;
    type* outcome = primitive_empty;
    char * buffer = (char*) cross_calloc(BUF_SIZE, sizeof(char));
    char * file_buffer = (char*) cross_calloc(BUF_SIZE, sizeof(char));
    char * response = NULL;
    size_t  storage_size = 0;
    fd_set rdfdset;
    struct timeval tv; 
    int wait_time = 1000000; 
    int cycle_time = 1;

    memset(buffer, 0, BUF_SIZE);
    while (1) {
        FD_ZERO(&rdfdset);
        FD_SET(sockfd, &rdfdset);
        tv.tv_sec = 0;
        tv.tv_usec = wait_time * cycle_time++;
        switch(select(sockfd + 1, &rdfdset, NULL, NULL, &tv)){
        case -1:
            goto out;
        case 0:
            if((file_buffer_count != 0) || (cycle_time > 5)){
                goto out;
            }
            break;
        default:
            if (FD_ISSET(sockfd, &rdfdset)){
                status = recv(sockfd, buffer, batch_size, 0);
                if (status == -1) {
                    goto out;
                }
                if (status == 0) {
                    goto out;
                }
                memcpy(file_buffer + file_buffer_count, buffer, sizeof(char) * status);
                file_buffer_count += status;
            }
            else{
                printf  ("\n4\n");
                break;
            }
        }
    }
 out:
    cross_free(buffer);
    outcome = new_storage(file_buffer, file_buffer_count);
    cross_free(file_buffer);
    return  outcome;
}

void * original_close(void *_left) {
    type * left = c_car(_left);
    char debug_inf[256] = "\0";

    if (left->em == EMPTY) {
        gc(_left);
        snprintf(debug_inf, 256, "close first value Shoule Be NETWORK Type\r\n");
        cross_printf(debug_inf);
        return primitive_empty;
    }

    if (left->em != NETWORK) {
        gc(_left);
        snprintf(debug_inf, 256, "close first value Shoule Be NETWORK Type\r\n");
        return new_debug(debug_inf, strlen(debug_inf));
    }

    int new_fd = left->u_data.n_network;
    close_tcp_socket(new_fd);
    gc(_left);
    return primitive_empty;
}

void * send_mesg(int sockfd, char* buf, int size){
    int len = 0, sign = 0;
    int status = 0;
    int batch_size = BUF_SIZE;
    type *outcome = primitive_empty;

    fd_set rdfdset;
    struct timeval tv; 
    int wait_time = 50000; 
    int cycle_time = 1;

    while (1) {
        FD_ZERO(&rdfdset);
        FD_SET(sockfd, &rdfdset);
        tv.tv_sec = 0;
        tv.tv_usec = wait_time * cycle_time++;
        switch(select(sockfd + 1, NULL, &rdfdset, NULL, &tv)){
        case -1:
            goto out;
        case 0:
            if(cycle_time > 20){
                goto out;
            }
            break;
        default:
            if (FD_ISSET(sockfd, &rdfdset)){
                status = send(sockfd, buf, size, MSG_NOSIGNAL);
                if (status == -1) {
                    goto out;
                }
                if (status > 0) {
                    outcome = int_type(1);
                    goto out;
                }
            }
            else{
                break;
            }
        }
    }
 out:
    return  outcome;
}

void * original_send(void *_left) {
    type * outcome = primitive_empty;
    type * left = c_car(_left);
    char  int_buf[256] = "\0";
    char * buf = NULL;
    int file_size = 0;
    char debug_inf[256] = "\0";

    if(left->em != NETWORK) {
        gc(_left);
        snprintf(debug_inf, 256, "send first value Shoule Be NETWORK Type\r\n");
        return new_debug(debug_inf, strlen(debug_inf));
    }

    int new_fd = left->u_data.n_network;
    type * right = c_cadr(_left);

    if((right->em == STORAGE) || (right->em == DEBUG)){
        buf = right->u_data.a_storage + sizeof(int);
        file_size = *(int*)right->u_data.a_storage - 1;
        outcome = send_mesg(new_fd, buf, file_size);
    }
    else if(right->em == VAR){
        buf = right->u_data.s_data;
        outcome = send_mesg(new_fd, buf, strlen(buf));
    }
    else if(right->em == INTEGER){
        snprintf(int_buf, 256, "%d", right->u_data.i_data);
        outcome = send_mesg(new_fd, int_buf, strlen(int_buf));
    }
    else {
        gc(_left);
        snprintf(debug_inf, 256, "send second value INVALID\r\n");
        return new_debug(debug_inf, strlen(debug_inf));
    }

    gc(_left);
    return outcome;
}

void * original_recv(void *_left) {
    type * left = c_car(_left);
    type * outcome = primitive_empty;
    int new_fd = 0;
    char debug_inf[256] = "\0";

    if (left->em == EMPTY) {
        gc(_left);
        return primitive_empty;
    }
    if(left->em != NETWORK) {
        gc(_left);
        snprintf(debug_inf, 256, "recv first value Shoule Be NETWORK Type\r\n");
        return new_debug(debug_inf, strlen(debug_inf));
    }
    new_fd = left->u_data.n_network;
    outcome = recv_mesg(new_fd);
    gc(_left);
    return outcome;
}

void * original_accept(void *_left) {
    type * left = c_car(_left);
    type * outcome = primitive_empty;
    int new_fd;
    struct sockaddr_in client_addr;
    int client_len = sizeof(client_addr);
    char debug_inf[256] = "\0";

    if (left->em != NETWORK) {
        gc(_left);
        snprintf(debug_inf, 256, "accept first value Shoule Be NETWORK Type\r\n");
        return new_debug(debug_inf, strlen(debug_inf));
    }
    outcome = new_object();
    new_fd = left->u_data.n_network;
    memset(&client_addr, 0, client_len);
    new_fd = accept(new_fd, (struct sockaddr *) (&client_addr), &client_len);
    outcome->em = NETWORK;
    outcome->u_data.n_network = new_fd;
    gc(_left);
    return outcome;
}

char* dns(char * host_name) {
    struct hostent *hostentobj = NULL;
    struct in_addr iaddr;
    hostentobj = gethostbyname(host_name);
    if (hostentobj == NULL) {
        return NULL;
    }
    iaddr = *((struct in_addr *) *hostentobj->h_addr_list);
    return inet_ntoa(iaddr);
}

int connect_host(int *new_socket, char *host_name, unsigned short remote_port) {
    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));

    server.sin_family = AF_INET;
    server.sin_port = htons(remote_port);

    if (inet_addr(host_name) != INADDR_NONE) {
        server.sin_addr.s_addr = inet_addr(host_name);
    }
    else {
        if (dns(host_name) != NULL){
            server.sin_addr.s_addr = inet_addr(dns(host_name));
        }
        else{
            return -1;
        }
    }

    *new_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (*new_socket == -1){
        return -1;
    }
    unsigned int timeout = 10000;
    setsockopt(*new_socket, SOL_SOCKET, SO_RCVTIMEO, (char *) &timeout,
               sizeof(timeout));
    if (connect(*new_socket, (struct sockaddr *) &server, sizeof(server)) == -1) {
        close_tcp_socket(*new_socket);
        return -1;
    }
    return 1;
}


void *
requesturl( int protocol, char* host, unsigned short port, char* file, char* referer, char* user_agent, char* auth_token, char** args, int argc){
    char head_buf[4096] = "\0"; 
    int max_arg, total_bytes;
    int multipart, next_arg_is_file;
    const char* const sep = "http_post-content-separator";
    unsigned char data_buf[1024] = "\0";
    unsigned char enc_buf[1024] = "\0";
    int head_bytes, data_bytes, i, header_state;
    char* eq = NULL;
    type *result = primitive_empty;
    char debug_inf[256] = "\0";

    int sockfd ;
    if (-1 == connect_host(&sockfd, host, port)){
        snprintf(debug_inf, 256, "connect_host error\r\n");
        /*        return new_debug(debug_inf, strlen(debug_inf));*/
        return primitive_empty;
    }
    /* Run through the arguments and figure out the total and max sizes,
    ** then allocate enough space.
    */
    multipart = 0;
    total_bytes = max_arg = 0;
    next_arg_is_file = 0;
    for ( i = 0; i < argc ; ++i ) {
        int l = strlen( args[i] );
        if ( strcmp( args[i], "-f" ) == 0 ) {
            multipart = 1;
            next_arg_is_file = 1;
            continue;
        }
        total_bytes += l;
        if ( l > max_arg )
            max_arg = l;
        if ( next_arg_is_file ) {
            eq = strchr( args[i], '=' );
            next_arg_is_file = 0;
        }
    }

    total_bytes *= 4;
    /* Encode the POST data. */
    /* Not multipart. */
    data_bytes = 0;
    for ( i = 0; i < argc ; ++i ) {
        if ( data_bytes > 0 )
            data_bytes += sprintf( &data_buf[data_bytes], "&" );
        eq = strchr( args[i], '=' );
        if ( eq == (char*) 0 ) {
            /*	 	  strencode( enc_buf, args[i] );
            / data_bytes += sprintf( &data_buf[data_bytes], "%s", enc_buf );*/
            data_bytes += sprintf( &data_buf[data_bytes], "%s", args[i] );
        }
        else {
            *eq++ = '\0';
            strencode( enc_buf, args[i] );
            data_bytes += sprintf( &data_buf[data_bytes], "%s=", enc_buf );
            strencode( enc_buf, eq );
            data_bytes += sprintf( &data_buf[data_bytes], "%s", enc_buf );
        }
    }

    /* Build request buffer, starting with the POST. */
    if (!strcmp(user_agent, "http_post")) {
        head_bytes = snprintf( head_buf, sizeof(head_buf), "POST %s HTTP/1.1\r\n", file );
    }
    else{
        head_bytes = snprintf( head_buf, sizeof(head_buf), "GET %s HTTP/1.0\r\n", file );
    }
    /* HTTP/1.1 host header - some servers want it even in HTTP/1.0. */
    head_bytes += snprintf( &head_buf[head_bytes], sizeof(head_buf) - head_bytes, "Host: %s\r\n", host );
    /*for yeelink*/
    head_bytes += snprintf( &head_buf[head_bytes], sizeof(head_buf) - head_bytes, "U-ApiKey:8b6c51b8a18ccbdae3c7ac74169ec3da\r\n");

    /* Content-length. */
    if (!strcmp(user_agent, "http_post")) {
        head_bytes += snprintf( &head_buf[head_bytes], sizeof(head_buf) - head_bytes, "Content-Length: %d\r\n", data_bytes);
    }
    else{
        head_bytes += snprintf( &head_buf[head_bytes], sizeof(head_buf) - head_bytes, "Content-Length: %d\r\n", 0);
    }

    if ( referer != (char*) 0 )
        /* Referer. */
        head_bytes += snprintf( &head_buf[head_bytes], sizeof(head_buf) - head_bytes, "Referer: %s\r\n", referer );
    /* User-agent. */
    head_bytes += snprintf( &head_buf[head_bytes], sizeof(head_buf) - head_bytes, "User-Agent: %s\r\n", user_agent );
    /* Content-type. */
    /*  "Content-type": "application/x-www-form-urlencoded",*/
    head_bytes += snprintf( &head_buf[head_bytes], sizeof(head_buf) - head_bytes, "Content-Type: application/json\r\n");

    /*
      if ( multipart )
      head_bytes += snprintf( &head_buf[head_bytes], sizeof(head_buf) - head_bytes, "Content-type: multipart/form-data; boundary=\"%s\"\r\n", sep );
      else
      head_bytes += snprintf( &head_buf[head_bytes], sizeof(head_buf) - head_bytes, "Content-type: application/x-www-form-urlencoded\r\n" );
    */


    /* Fixed headers. */
    head_bytes += snprintf( &head_buf[head_bytes], sizeof(head_buf) - head_bytes, "Accept: */*\r\n" );
    /*  head_bytes += snprintf( &head_buf[head_bytes], sizeof(head_buf) - head_bytes, "Accept-Encoding: gzip, compress\r\n" );*/
    head_bytes += snprintf( &head_buf[head_bytes], sizeof(head_buf) - head_bytes, "Accept-Language: utf8\r\n" );
    head_bytes += snprintf( &head_buf[head_bytes], sizeof(head_buf) - head_bytes, "Accept-Charset: iso-8859-1,*,utf-8\r\n" );



    {
        /* Basic Auth info. */
        head_bytes += snprintf( &head_buf[head_bytes], sizeof(head_buf) - head_bytes, "Authorization: Basic %s\r\n", "YWRtaW46YWRtaW4=");
    }

    /* Cookies. */
    if(!strcmp(user_agent, "http_post")){
    }

    if(1){
        head_bytes += snprintf( &head_buf[head_bytes], sizeof(head_buf) - head_bytes, "Connection: Keep-Alive\r\n" );
    }
    else{
    }
    /* Blank line. */
    head_bytes += snprintf( &head_buf[head_bytes], sizeof(head_buf) - head_bytes, "\r\n" );
    send(sockfd, head_buf, head_bytes, MSG_NOSIGNAL);

    if(!strcmp(user_agent, "http_post")){
        /* And send the POST data too. */
        send(sockfd, data_buf, data_bytes, MSG_NOSIGNAL);
    }
    cross_printf("socket is  %d \r\n", sockfd);

    result = (type*)new_object();
    result->em = NETWORK;
    result->u_data.n_network = sockfd;
    return result;
}


void* handleurl(char* url, int* _protocol, char* _host, int* _port, char* _file) {
    char* s;
    char host[256];
    int host_len;
    unsigned short port;
    char* file = 0;
    int proto_len;

    char* http = "http://";
    int http_len = strlen( http );

    char* tcp = "tcp://";
    int tcp_len = strlen( tcp );

    char debug_inf[256];

    if ( strncmp( http, url, http_len ) == 0 ){
        proto_len = http_len;
        *_protocol = 0;
    }
    else if ( strncmp( tcp, url, tcp_len ) == 0 ){
        proto_len = tcp_len;
        *_protocol = 1;
    }
    else{
        snprintf(debug_inf, 256, "handleurl %s error.\r\n", url);
        return new_debug(debug_inf, strlen(debug_inf));
    }

    for (s = url + proto_len; *s != '\0' && *s != ':' && *s != '/'; ++s );

    host_len = s - url;
    host_len -= proto_len;
    strncpy( host, url + proto_len, host_len );
    host[host_len] = '\0';
    strcpy(_host, host);

    if ( *s == ':' ) {
        port = (unsigned short) atoi( ++s );
        while ( *s != '\0' && *s != '/' )
            ++s;
    }
    else{
        port = 80;
    }

    *_port = port;

    if ( *s == '\0' ) {
        file = "/";
    }
    else {
        file = s;
    }

    strcpy(_file, file);
    return primitive_empty;
}

void * original_get(void * _left) {
    type * left = NULL;
    type * right = NULL;
    type * outcome = NULL;
    type * present = NULL;
    char * function_id = NULL;

    char* referer;
    char* user_agent;
    char* auth_token;
    int verbose = 0;
    char  * post_body = NULL;
    char* post[1024] = {NULL};
    int  argc = 0, i = 0;

    int protocol = 0;
    char host[256] = "\0";
    int port = 0;
    char file[256] = "\0";
    char debug_inf[256] = "\0";

    referer = (char*) 0;
    user_agent = "http_get";
    auth_token = (char*) 0;

    left  = c_car(_left);
    right  = c_cdr(_left);
    if(right->em == EMPTY){
    }
    else{
        right = c_car(right);
        while (1) {
            if(right->em == EMPTY){
                break;
            }

            present = c_car(right);
            post_body = calloc(sizeof(char), 1024);
            if(present->em == STORAGE){
                sprintf(post_body, "%s", present->u_data.a_storage + sizeof(int));
            }
            else if(present->em == VAR){
                sprintf(post_body, "%s", present->u_data.s_data);
            }
            else{
                snprintf(debug_inf, 256, "get para value should be STORAGE or VAR\r\n");
                gc(_left);
                return new_debug(debug_inf, strlen(debug_inf));
            }
            post[argc++] = post_body;
            right = c_cdr(right);
        }
    }
    if (left->em != STORAGE) {
        snprintf(debug_inf, 256, "get first value should be STORAGE\r\n");
        gc(_left);
        return new_debug(debug_inf, strlen(debug_inf));
    }

    outcome = handleurl( (char*)(left->u_data.a_storage) + sizeof(int), &protocol, host, &port, file);
    if(outcome->em == DEBUG) {
        for(i = 0; i < argc; i++){
            free(post[i]);
        } 
        gc(_left);
        return outcome;
    } 
    outcome = requesturl( protocol, host, port, file, referer, user_agent, auth_token, (char**)post , argc);

    for(i = 0; i < argc; i++){
        free(post[i]);
    } 
    gc(_left);
    return outcome;
}


void * original_post(void * _left) {
    type * left = NULL;
    type * right = NULL;
    type * present = NULL;
    type * outcome = NULL;
    char * function_id;

    char* referer;
    char* user_agent;
    char* auth_token;
    int verbose = 0;
    char  * post_body = NULL;
    char* post[1024] = {NULL};
    int  argc = 0, i = 0;

    int protocol = 0;
    char host[256] = "\0";
    int port = 0;
    char file[256] = "\0";
    char debug_inf[256] = "\0";

    referer = (char*) 0;
    user_agent = "http_post";
    auth_token = (char*) 0;

    left  = c_car(_left);
    right  = c_cadr(_left);

    while(1){
        if(right->em == EMPTY){
            break;
        }

        present = c_car(right);
        post_body = calloc(sizeof(char), 1024);
        if(present->em == STORAGE){
            sprintf(post_body, "%s", present->u_data.a_storage + sizeof(int));
        }
        else if(present->em == VAR){
            sprintf(post_body, "%s", present->u_data.s_data);
        }
        else{
            snprintf(debug_inf, 256, "post para value should be STORAGE or VAR\r\n");
            gc(_left);
            return new_debug(debug_inf, strlen(debug_inf));
        }
        post[argc++] = post_body;
        right = c_cdr(right);
    }

    if (left->em != STORAGE) {
        snprintf(debug_inf, 256, "post first value should be STORAGE\r\n");
        gc(_left);
        return new_debug(debug_inf, strlen(debug_inf));
    }
    outcome = handleurl((char*)(left->u_data.a_storage) + sizeof(int), &protocol, host, &port, file);
    if(outcome->em == DEBUG) {
        gc(_left);
        return outcome;
    } 
    outcome = requesturl( protocol, host, port, file, referer, user_agent, auth_token, (char**)post , argc);
    for(i = 0; i < argc; i++){
        free(post[i]);
    } 
    gc(_left);
    return outcome;
}

void * original_hs(void *_left) {
    type * left = _left;
    type * outcome;
    char * request = (((type *) c_car(left))->u_data.a_storage + sizeof(int));
    char * file_token = NULL;

    char * token = strtok( request, " " );
    char * uri = strtok( 0, " " );
    char   file[256] = "\0";
    long   file_size, file_sizex;
    type * mid_expr;
    entry  result;
    char * response = NULL;
    char * responsex = NULL;
    size_t storage_size = 0;
    char * referer;
    char * user_agent;
    char * auth_token;
    char * url = NULL;
    int new_fd = 0;

    int protocol = 0;
    char host[256] = "\0";
    int port = 0;
    char request_file[256] = "\0";

    if(uri == NULL || strlen(uri) == 0){
        url = (char*) cross_calloc(BUF_SIZE, sizeof(char));
        strcat(url, "HTTP/1.1 404 NOT FOUND\r\n\r\n");
        gc(_left);
        outcome = new_storage(url, strlen(url));
        cross_free(url);
        return outcome;
    }

    cross_printf("request is %s  %s\r\n", request, uri);  
    if((file_token = strstr(uri, "websocket?"))){
        url = (char*) cross_calloc(BUF_SIZE, sizeof(char));
        sprintf(url, "%s", file_token + strlen("websocket?"));  /*url has other meaning here */

        referer = (char*) 0;
        user_agent = "http_get";
        auth_token = (char*) 0;
        outcome = handleurl( url, &protocol, host, &port, request_file);
        if(outcome->em == DEBUG) {
            gc(_left);
            return outcome;
        } 
        outcome = requesturl( protocol, host, port, request_file, referer, user_agent, auth_token, (char**)0 , 0);

        new_fd = outcome->u_data.n_network;
        gc(outcome);
        outcome = recv_mesg(new_fd);

        gc(_left);
        cross_free(url);
    }
    else{
        file_token = strstr(uri, "?");
        if(file_token){
            file[0] = '.';
            memcpy( file + 1, uri, file_token - uri);
        }
        else{
            sprintf( file, ".%s", uri );
        }
        cross_printf("request is %s  %s\r\n", request, file);
        result = lookup(file, global_var);
        mid_expr = result.u_data._expr;
        response = (char*) cross_calloc(BUF_SIZE, sizeof(char));

        if (mid_expr == NULL) {
            FILE *fp = fopen( file, "rb" );
            if ( fp == 0 ) {
                /* response 404 status code */
                strcat(response, "HTTP/1.1 404 NOT FOUND\r\n\r\n");
                file_size = strlen(response);
            }
            else {
                fseek( fp, 0, SEEK_END );
                file_size = ftell( fp );
                fseek( fp, 0, SEEK_SET );
                char *content = (char*)cross_calloc( file_size + 10, sizeof(char));
                fread( content, file_size, 1, fp );
                if((file_token = strstr(file, ".html"))){
                    content[file_size] = 0;
                    sprintf( response, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: %ld\r\n\r\n%s", file_size, content );
                    file_size = strlen(response); 
                }
                else if((file_token = strstr(file, ".png"))){
                    sprintf( response, "HTTP/1.1 200 OK\r\nContent-Type: image/png\r\nContent-Length: %ld\r\n\r\n", file_size);
                    file_sizex = strlen(response); 
                    memcpy(response + file_sizex, content, file_size);
                    file_size += file_sizex;
                }
                else{
                    /*	    sprintf( response, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: %ld\r\n\r\n%s", file_size, content );*/
                    sprintf( response, "%s", content );
                }
                assign(file, content, (void*)(file_size), &global_var);
            }
        } 
        else {
            file_size = (long)(result._env);
            if((file_token = strstr(file, ".html"))){
                sprintf( response, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: %ld\r\n\r\n%s", file_size, (char*)mid_expr );
            }
            else if((file_token = strstr(file, ".png"))){
                sprintf( response , "HTTP/1.1 200 OK\r\nContent-Type: image/png\r\nContent-Length: %ld\r\n\r\n", file_size);
                file_sizex = strlen(response); 
                memcpy(response + file_sizex, mid_expr, file_size);
                file_size += file_sizex;
            }
            else{
                sprintf( response, "%s", (char*)mid_expr );
            }
        }
        outcome = new_storage(response, file_size);
        cross_free(response);
        gc(_left);
    }
    return outcome;
}

void * original_gethostbyname(void *_left) {
    type * present = c_car(_left);
    type * outcome = primitive_empty;
    char * material = NULL;
    char debug_inf[256] = "\0";
  
    if(present->em == VAR){
        material = present->u_data.s_data;
    }
    else if(present->em == STORAGE){
        material = present->u_data.a_storage + sizeof(int);
    }
    else {
        snprintf(debug_inf, 256, "gethostbyname first value should be a VAR or STORAGE\r\n");
        gc(_left);
        return new_debug(debug_inf, strlen(debug_inf));
    }

    snprintf(debug_inf, 256, "%s", dns(material));
    gc(_left);
    return new_storage(debug_inf, strlen(debug_inf));   /* not  new_debug, it's true value */
}

int bind_host(int *new_socket, char *host_name, unsigned short remote_port) {
    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));

    server.sin_family = AF_INET;
    server.sin_port = htons(remote_port);

    if (inet_addr(host_name) != INADDR_NONE)
        server.sin_addr.s_addr = inet_addr(host_name);
    else {
        if (dns(host_name) != NULL)
            server.sin_addr.s_addr = inet_addr(dns(host_name));
        else
            return -1;
    }

    *new_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (*new_socket == -1)
        return -1;
    unsigned int timeout = 10000;
    setsockopt(*new_socket, SOL_SOCKET, SO_RCVTIMEO, (char *) &timeout,
               sizeof(timeout));


    if (bind(*new_socket, (struct sockaddr *) &server, sizeof(server))
        == -1) {
        close_tcp_socket(*new_socket);
        return -1;
    }
    return 1;
}


void * original_tcpclient(void *_left) {
    type * present;
    type * right = c_car(_left);
    type * outcome = primitive_empty;

    int sockfd;
    int result = -1;
    char* url = NULL;

    int protocol = 0;
    char host[256] = "\0";
    int port = 0;
    char file[256] = "\0";
    char debug_inf[256] = "\0";

    if(right->em == VAR){
        url = (char*)right->u_data.s_data;
    }
    else if(right->em == STORAGE){
        url = (char*)(right->u_data.a_storage) + sizeof(int);
    }
    else{
        snprintf(debug_inf, 256, "tcpclient url format error.\r\n");
        gc(_left);
        return new_debug(debug_inf, strlen(debug_inf));
    }

    outcome = handleurl(url, &protocol, host, &port, file);
    if(outcome->em == DEBUG) {
        gc(_left);
        return outcome;
    } 
    result = connect_host(&sockfd, host, port);
    if (result == -1) {
        gc(_left);
        return primitive_empty;
    }
    outcome = new_object();
    outcome->em = NETWORK;
    outcome->u_data.n_network = sockfd;
    gc(_left);
    return outcome;
}


void * original_tcpserver(void *_left) {
    type * left = _left;
    type * outcome = new_object();
    int sockfd;
    int port;
    int result = -1;
    char * ip = NULL;
    char debug_inf[256] = "\0";

    type * ip_type = c_cadr(_left);
    if ((port = ((type *) c_car(left))->u_data.i_data) < 0) {
        snprintf(debug_inf, 256, "port number error.\r\n");
        gc(_left);
        return new_debug(debug_inf, strlen(debug_inf));
    }
    ip = ip_type->u_data.a_storage + sizeof(int);

    if (!bind_host(&sockfd, ip, port)) {
        snprintf(debug_inf, 256, "bind host error.\r\n");
        gc(_left);
        return new_debug(debug_inf, strlen(debug_inf));
    }

    result = listen(sockfd, 100);
    if (result == -1) {
        snprintf(debug_inf, 256, "Listen error:%d\r\n", port);
        close_tcp_socket(sockfd);
        gc(_left);
        return new_debug(debug_inf, strlen(debug_inf));
    }
    cross_printf(">>>>>>>>>>!!!\r\n");
    /*follow  modify  by  rosslyn  ,2013.8.16*/
    outcome->em = NETWORK;
    outcome->u_data.n_network = sockfd;
    gc(_left);
    return outcome;
}
/*(network system end)*/
#endif

/*(callback system)*/
typedef void * (*original_callback_user)(void * _left, void *_right);
typedef struct  callback_event{
    int mask;
    original_callback_user proc;
    void *thunk;
} callback_event;

#define AE_SETSIZE (1024*10)    /* Max number of fd supported */
#define AE_OK 0
#define AE_ERR -1
#define AE_NONE 0
#define AE_READABLE 1
#define AE_WRITABLE 2
#define AE_FILE_EVENTS 1
#define AE_TIME_EVENTS 2
#define AE_ALL_EVENTS (AE_FILE_EVENTS|AE_TIME_EVENTS)
#define AE_DONT_WAIT 4
#define AE_NOMORE -1
#define AE_NOTUSED(V) ((void) V)

#define AT_OK 0
#define AT_ERR -1
#define AT_NONE 0
#define AT_FILE_EVENTS 1
#define AT_TIME_EVENTS 2
#define AT_ALL_EVENTS (AE_FILE_EVENTS|AE_TIME_EVENTS)
#define AT_DONT_WAIT 4
#define AT_NOMORE -1
#define AT_NOTUSED(V) ((void) V)



void * iterate_address(void *_left);
void * execution(void** _env, void * forth_code[], long begin, long end, char * this);
void * callback_worker(void * _left, void *_right) {
    type * left = NULL, * outcome = NULL,* para = NULL;
    entry result;
    int forth_find, suicide = 0 ;
    type *m_env = primitive_empty;
    void **_env = (void**) &m_env;
    /*  m_env = c_cons(m_env, primitive_empty); */
    left = c_car(_left);

    result = lookup((char *) (left->u_data.s_data),
                    global_lambda);
    if (result.u_data._expr == NULL) {
        if (iterate_address(left) == NULL) {
            /*impossible*/
        } 
        else {
            result.u_data._expr =
                (void*) ((iterate_address(left) + 1));
        }
    } 
    else {
        suicide = 1 ;
    }

    if (!result._env) {
    } 
    else {
        *_env = result._env;
    }

    forth_find = (long) result.u_data._expr;
    if (forth_find != 0){
        para = global_forth_code[forth_find ];
        _env = c_bindvars(para, c_append(c_copy_type(c_cdr(_left)), c_copy_type(_right) ), _env);
        outcome =  execution(_env, global_forth_code, forth_find + 1, global_forth_code_ipc, NULL);
        c_unbindvars(_env);
        if(suicide && (outcome->em == VAR) && !strcmp(outcome->u_data.s_data, "suicide") ){
            m_env = result._env;
            if (m_env == NULL) {
            } 
            else {
                gc(m_env);
            }
            global_lambda = freeentry(left->u_data.s_data, global_lambda);
        }
    }

    /*think why?*/
    return outcome ;  /*modify pjoin mean,so there need change according.  2014.5.31*/
}
/*(callback system end)*/




/*ordinal operator*/
typedef struct fun_info {
    char name[15];
    original_callback address;
    char parameters_num;
} fun_info;
typedef struct type_info {
    char name[10];
    native_operator type;
} type_info;

void * var_type(char * name) {
    type * outcome = new_object();
    outcome->em = VAR;
    cross_strcpy(outcome->u_data.s_data, name);
    return outcome;
}

void * c_find_var_value_help(void * _left, void * _right) {
    type * left = _left, *right = _right;
    type * t;
    if (right->em == EMPTY) {
        return NULL;
    }
    t = c_car (right);
    if (!strcmp(left->u_data.s_data, ((type *) c_car (t))->u_data.s_data)) {
        return c_cadr (t );
    } 
    else {
        return c_find_var_value_help(left, c_cdr (right));
    }
}

void * c_find_var_value(void * _left, void * _env) {
    type * left = _left, *env = _env, *outcome = NULL;
    type * mid_expr;
    entry result;
    while (env->em != EMPTY) {
        if ((outcome = c_find_var_value_help(left, c_car (env))))
            return outcome;
        env = c_cdr (env);
    }


    result = lookup((char *) (left->u_data.s_data), global_define);
    mid_expr = result.u_data._expr;
    if (mid_expr == NULL) {
    } 
    else {
        return mid_expr;
    }


    result = lookup((char *) (left->u_data.s_data), global_var);
    mid_expr = result.u_data._expr;
    if (mid_expr == NULL) {
    } 
    else {
        return mid_expr;
    }

    return NULL;
}

void * random_name(void) {
    int i = 0;
    char name[ARABIC] = "\0";

    for (i = 0; i < ARABIC - 1; i++) {
        name[i] = cross_rand() % ARABIC + 'a';
    }
    return var_type(name);
}

void * original_randomname(void * _left) {
    type * outcome = random_name();
    gc(_left);
    return outcome;
}

void * original_help(void * _left);

fun_info normal_fun[] = {
#if defined (LINUX)
    {"atpoll", original_atpoll, -1}, 
    {"atadd", original_atadd, 3}, 
    {"atwait", original_atwait, 1}, 
    {"atcreate", original_atcreate, 0}, 
    {"atpoll", original_atpoll, -1}, 
    {"atadd", original_atadd, 3}, 
    {"atwait", original_atwait, 1}, 
    {"atcreate", original_atcreate, 0}, 
#endif
#ifdef LINUX
    {"sleep", original_sleep, 1}, 
    {"aedel", original_aedel, 3}, 
    {"aepoll", original_aepoll, -1}, 
    {"aeadd", original_aeadd, 4}, 
    {"aecreate", original_aecreate, 1}, 
    {"close", original_close, 1}, 
    {"urlencode", original_urlencode, 1}, 
    {"get", original_get, -1}, 
    {"post", original_post, 2}, 
    {"recv", original_recv, 1}, 
    {"hs", original_hs, 1}, 
    {"recv", original_recv, 1}, 
    {"send", original_send, 2}, 
    {"accept", original_accept, 1}, 
    {"tcpclient", original_tcpclient, 1}, 
    {"tcpserver", original_tcpserver, 2},
#endif
    {"fflush", original_fflush, 0}, 
    {"itoa", original_itoa, 1}, 
    {"atoi", original_atoi, 1},  
    {"len", original_len, 1},  
#ifdef LINUX
    {"jaddarray", original_jaddarray, 2}, 
    {"jaddobject", original_jaddobject, 3}, 
    {"jupdateobject", original_jupdateobject, 3}, 
    {"jcreatearray", original_jcreatearray, 0}, 
    {"jcreatestring", original_jcreatestring, 1},  
    {"jcreateint", original_jcreateint, 1}, 
    {"jcreate", original_jcreate, 0}, 
    {"makejson", original_makejson, 1}, 
    {"killjson", original_killjson, 1}, 
    {"jtos", original_jtos, 1}, 
    {"jtosx", original_jtosx, 1}, 
    {"jgetarrayitem", original_jgetarrayitem, 2}, 
    {"jgetkeys", original_jgetkeys, 1}, 
    {"jgetarraysize", original_jgetarraysize, 1}, 
    {"jgetint", original_jgetint, 1}, 
    {"jgetstring", original_jgetstring, 1}, 
    {"jgetobject", original_jgetobject, 2}, 
#endif
#ifdef LINUX
    {"decode", original_decode, 1}, 
    {"encode", original_encode, 1}, 
    {"md", original_md, 1}, 
#endif
    {"eval", original_eval, 1}, 
    {"cddar", original_cddar, 1}, 
    {"caar", original_caar, 1}, 
    {"cddr", original_cddr, 1}, 
    {"caddr", original_caddr, 1}, 
    {"cadr", original_cadr, 1}, 
    {"cdar", original_cdar, 1}, 
    {"cons", original_cons, 2}, 
    {"charp", original_charp, 1}, 
    {"storagep", original_storagep, 1}, 
    {"debugp", original_debugp, 1}, 
    {"digitp", original_digitp, 1}, 
    {"display", original_display, 0},
    {"help", original_help, 0},
    {"destroy", original_destroy, 0}, 
    {"random", original_random, 1}, 
    {"randomname", original_randomname, 0}, 
    {"print", original_print, 1}, 
    {"minus", original_minus, -1}, 
    {"add", original_add, -1}, 
    {"mod", original_mod, 2}, 
    {"car", original_car, 1}, 
    {"cdr", original_cdr, 1}, 
    {"atom", original_atom, 1}, 
    {"list", original_list, -1}, 
    {"and", original_and, 2}, 
    {"or", original_or, 2}, 
    {"eq", original_eq, 2}, 
    {"not", original_not, 1}, 
    {"big", original_big, 2}, 
    {"div", original_divi, 2}, 
    {"mul", original_mul, 2}, 
    {"small", original_small, 2}, 
    {"", NULL, 0 } 
};

/*for milestone ,2014.6.7*/
type_info original_type[] = { 
    /*basic*/
    {"setq", SETQ}, 
    {"define", DEFINE}, 
    {"defun", DEFUN}, 
    {"if", IF}, 
    {"progn", PROGN}, 
    {"lambda", LAMBDA}, 
    {"defmacro", DEFMACRO}, 
    {"funcall", FUNCALL}, 
    {"quote", QUOTE}, 
    /*continuation*/
    {"yield", YIELD}, 
    {"stop", STOP}, 
    {"next", NEXT}, 
    {"eofstdin", EOFSTDIN}, 
    {"stdin", STDIN}, 
    {"isstop", ISSTOP}, 
    {"self", SELF}, 
    /*low-level structure*/
    {"press", PRESS}, 
    {"whole", WHOLE}, 
    {"combi", COMBI}, 
    {"exchange", EXCHANGE}, 
    {"top", TOP}, 
    {"eject", EJECT}, 
    {"append", APPEND}, 
    /*all forth special symbol*/
#ifdef LINUX
    {"zfdot", ZFDOT}, 
    {"zfeq", ZFEQ},
    {"zfdup", ZFDUP}, 
    {"zfforth", ZFFORTH},
    {"zfcolon", ZFCOLON}, 
    {"zfend", ZFEND}, 
    {"zfif", ZFIF}, 
    {"zfelseif", ZFELSEIF}, 
    {"zfthen", ZFTHEN},
    {"zfadd", ZFADD},
    {"zfminus", ZFMINUS}, 
    {"zfswap", ZFSWAP}, 
    {"zfmod", ZFMOD}, 
    {"zfbig", ZFBIG}, 
    {"zfover", ZFOVER}, 
    {"zfdrop", ZFDROP}, 
    {"zfrandom", ZFRANDOM}, 
    {"zfquote", ZFQUOTE}, 
    {"zfdotchar", ZFDOTCHAR}, 
#endif
    {"", 0 } 
};

void * original_help(void * _left) {
    int sign = 0;
    char debug_inf[4096] = "VERSION 2015.7.28\r\nFUN parameters_num\r\n";

    while (1) {
        if (!strcmp("", normal_fun[sign].name)) {
            break;
        }

        cross_sprintf(debug_inf + cross_strlen(debug_inf), 
                 "(%s %d)\n", 
                 normal_fun[sign].name, 
                 normal_fun[sign].parameters_num);
        sign++;
    }

    gc(_left);
    return left_print(new_storage(debug_inf, cross_strlen(debug_inf)));
}

void init_primitive(void) {
    int sign = 0;
    while (1) {
        if (!strcmp("", normal_fun[sign].name)) {
            break;
        }
        if (lookup_primitive(normal_fun[sign].name, global_primitive) != NULL) {
            cross_printf("exit %s alread exist", normal_fun[sign].name);
        }

        assign_primitive(normal_fun[sign].name, normal_fun[sign].address,
                         &global_primitive);
        sign++;
    }

}

/*similar  to  the  macro  dispatch*/
void * globaleval(void * _left, void ** _env);
void * left_print(void *);

void * c_bindvar_help(void * name, void * value);
void * c_set_global_var_value(void * _name, void * value) {
    type *name = _name;
    entry result = lookup((char *) (name->u_data.s_data), global_var);
    type * mid_expr = result.u_data._expr;
    if (mid_expr == NULL) {
    } 
    else {
        gc(mid_expr);
        global_var = freeentry((char *) (name->u_data.s_data), global_var); /*add by rosslyn  2015/6/27 doing flexhash*/
    }

    assign((char *) (name->u_data.s_data), value, NULL, &global_var);
    return c_copy_type(name);
}

void * c_set_global_define_value(void * _name, void * value) {
    type * name = _name;
    assign((char *) (name->u_data.s_data), value, primitive_empty, &global_define);
    return c_copy_type(name);
}


void * c_bindvar_help(void * name, void * value) {
    return c_cons(c_copy_atom(name), c_cons(value, primitive_empty));
}

void * c_bindvar(void * _left, void * _right) {
    type * left = _left, *right = _right, *outcome;
    if (left->em == EMPTY || right->em == EMPTY) {/*2013.6.5 for c_car speed modify*/
        return primitive_empty;
    } 
    else {
        outcome = c_cons(c_bindvar_help(c_car (left), c_car (right)),
                         c_bindvar(c_cdr (left), c_cdr (right)));
        return outcome;
    }
}

void ** c_bindvars(void * _left, void * _right, void ** _env) {
    type * left = _left;
    type * right = _right;
    /*solve  autotest  null-parameter cause memory leak*/
    if (left->em != EMPTY) {
        *_env = c_cons(c_bindvar(left, right), *_env);
        gc_frame(right);
        return _env;
    } 
    else {
        return _env;
    }
}

void c_unbindvar_help(void * _left) {
    type * left = _left, *outcome;
    outcome = c_cadr (left);

    if (outcome->em == LIST)
        gc_frame(outcome);
    else
        gc_atom(outcome);
}

void c_unbindvar(void * _left) {
    type * left = _left, *present;
    if (left->em == EMPTY)
        return;
    else {
        c_unbindvar_help(c_car (left)); /*consist with the inital decision*/

        present = c_cdr(left);
        gc_frame(c_car (left));
        c_unbindvar(present); /*reason as  above*/
    }
}

void c_unbindvars(void ** _env) {
    type * right = c_cdr( * _env);

    /*
      gc_reserved(c_car( * _env));
      gc_atom_reserved( * _env);
    */

    gc(c_car( * _env));
    gc_atom( * _env);

    *_env = right;
}

void * c_find_defmacro_arg(void * name, void * mem) {
    type * _env = mem;
    type * label;
    while (_env) {
        label = c_car ( _env );
        if (!strcmp(((type *) c_car (label))->u_data.s_data,
                    ((type *) name)->u_data.s_data)) {
            return c_cadr(label);
        }
        _env = c_cdr (_env);
    }
    return NULL;
}
void * c_find_defmacro_expr(void * name, void * mem) {
    type * _env = mem;
    type * label;
    while (_env) {
        label = c_car ( _env);
        if (!strcmp(((type *) c_car (label))->u_data.s_data,
                    ((type *) name)->u_data.s_data)) {
            return c_caddr(label);
        }
        _env = c_cdr (_env);
    }
    return NULL;
}

int c_find_defun_arg(void * _name, void ** _arg) {
    type * mid_expr;
    /*  tries result;*/
    entry result;
    type *name = _name;
    result = lookup((char *) (name->u_data.s_data), global_defun);
    mid_expr = result.u_data._expr;
    if (mid_expr == NULL) {
        result = lookup((char *) (name->u_data.s_data), global_lambda);
        mid_expr = result.u_data._expr;
        if (mid_expr == NULL) {
            *_arg = NULL;
        } 
        else {
            *_arg = c_car(mid_expr);
        }
    } 
    else {
        *_arg = c_car(mid_expr);
    }
    return 1;
}

int c_find_defun_expr(void * _name, void ** _expr) {
    type * mid_expr;
    entry result;
    type *name = _name;
    result = lookup((char *) (name->u_data.s_data), global_defun);
    mid_expr = result.u_data._expr;
    if (mid_expr == NULL) {
        result = lookup((char *) (name->u_data.s_data), global_lambda);
        mid_expr = result.u_data._expr;
        if (mid_expr == NULL) {
            *_expr = NULL;
        } 
        else {
            *_expr = c_cadr(mid_expr);
        }
    } 
    else {
        *_expr = c_cadr(mid_expr);
    }
    return 1;
}

void * add_quote(void * _left) {
    type * left = _left;
    type * outcome = NULL;
    if (left->em == EMPTY) {
        outcome = primitive_empty;
    } 
    else {
        outcome = c_cons(c_list(operate_type(QUOTE), c_car (left), 0),
                         add_quote(c_cdr (left)));
        gc_atom(left);
        /*modify  by  rosslyn   2013.3.10*/
    }
    return outcome;
}

void * eval_para(void * _left, void ** _env);
void compare(void * _left, void * _right);


typedef struct {
    int _carry;
    int _out;
} semi_outcome;

semi_outcome semi_add(int first, int second) {
    semi_outcome present;
    present._carry = first & second;
    present._out = first ^ second;
    return present;
}

int gene_clock = 1;
int oscillator(void) {
    return gene_clock = 1 - gene_clock;
}

void rs_trigger(void) {
    int r = 0;
    int s = 0;
    int circuit_r = 0;
    int circuit_s = 0;
    circuit_r = 1 - (r | circuit_s);
    circuit_s = 1 - (s | circuit_r);
}

int D_trigger(int clock, int D, int address, int *array) {
    int D_ = 1 - D;
    int circuit_Q = 0;
    int circuit_Q_ = 1;

    if (clock & 1) {
        circuit_Q_ = 1 - ((clock & D) | circuit_Q);
        circuit_Q = 1 - ((clock & D_) | circuit_Q_);
        *array |= circuit_Q << address;
        return 0;
    } 
    else {
        /*follow  too slow
          return  (*array & ADD_INDICATOR[address])>>address;*/
        return (*array >> address) & 1;
    }
}

void write_circuit(int address, int value, int * trigger_array) {
    /*
      D_trigger(1, select_circuit(address, value), address, trigger_array);
    */
}

int read_circuit(int address, int * trigger_array) {
    return D_trigger(0, 0, address, trigger_array);
}

typedef struct {
    int clock;
    int last_clock;
} oscillator_clock;

typedef struct {
    int circuit_Q_mid;
    int circuit_Q_mid_;

    int circuit_Q;
    int circuit_Q_;
} trigger_status;

void border_D_trigger_single(oscillator_clock *time, int *signal,
                             trigger_status *status) {
    /*  oscillator_clock time;*/
    int D = status->circuit_Q_;
    if (time->clock == 0) {
        if (D) {
            status->circuit_Q_mid =
                (D == 0 && (status->circuit_Q_mid_ == 0)) ? 1 : 0;
            status->circuit_Q_mid_ =
                (D != 0 && (status->circuit_Q_mid == 0)) ? 1 : 0;
        } 
        else {
            status->circuit_Q_mid_ =
                (D != 0 && (status->circuit_Q_mid == 0)) ? 1 : 0;
            status->circuit_Q_mid =
                (D == 0 && (status->circuit_Q_mid_ == 0)) ? 1 : 0;
        }
    }
    if (time->clock != 0 && time->last_clock == 0) {
        if (status->circuit_Q_mid ^ 1) {
            status->circuit_Q_ =
                ((status->circuit_Q_mid_ == 0) && (status->circuit_Q == 0)) ?
                1 : 0;
            status->circuit_Q =
                ((status->circuit_Q_mid == 0) && (status->circuit_Q_ == 0)) ?
                1 : 0;
        } 
        else {
            status->circuit_Q =
                ((status->circuit_Q_mid == 0) && (status->circuit_Q_ == 0)) ?
                1 : 0;
            status->circuit_Q_ =
                ((status->circuit_Q_mid_ == 0) && (status->circuit_Q == 0)) ?
                1 : 0;
        }
    }
    *signal = status->circuit_Q;
    time->last_clock = time->clock;
}

#define ARRAY_NUM  9
void border_D_trigger_ip(void) {
    oscillator_clock time[ARRAY_NUM];
    trigger_status status[ARRAY_NUM];
    int signal = 0, i = 0;
    int outcome = 0;

    for (i = 0; i < ARRAY_NUM; i++) {
        time[i].clock = time[i].last_clock = -1;
        status[i].circuit_Q_mid = 1;
        status[i].circuit_Q_mid_ = 0;
        status[i].circuit_Q = 0;
        status[i].circuit_Q_ = 1;
    }
    /*
      int  circuit_Q_mid=1;
      int  circuit_Q_mid_=0;

      int  circuit_Q=0;
      int  circuit_Q_=1;
    */
 label: outcome = 0;
    time->clock = oscillator();
    outcome |= 1 - time->clock; /*1-  very important*/
    border_D_trigger_single(time, &signal, status);
    outcome |= signal << 1;

    for (i = 1; i < ARRAY_NUM; i++) {
        time[i].clock = 1 - signal;
        border_D_trigger_single(time + i, &signal, status + i);
        outcome |= signal << (i + 1);
    }
    goto label;
}

void** binary_return_array = (void **)0;
unsigned int binary_return_label = 0;
void** binary_data_array = (void**)0;
unsigned int binary_data_label = 0;

long* fetch_memory(void** trigger_array) {
    return (long*)trigger_array;
}

void batch_write_circuit(void * value, void** trigger_array) {
    long* address_real;
    address_real = fetch_memory(trigger_array);
    *address_real = 0;
    *address_real = (long)value;
}

void * batch_read_circuit(void** trigger_array) {
    long* address_real;
    address_real = fetch_memory(trigger_array);
    return (void*)*address_real;
}

/*for milestone*/
void * iterate_address(void *_left) {
    long i = 0;
    type *left = _left, *right = NULL;
    for (i = 0; i < global_forth_code_ipc; i++) {
        right = global_forth_code[i];
        if(right && right->em == EFDEFUN){
            right = global_forth_code[i+1];
            if (right && right->em == VAR &&  !strcmp(left->u_data.s_data, right->u_data.s_data)) {
                return (void*)(i + 1);
            }
        }
    }
    return NULL;
}

void * fetch_code_skip_address(void *_left, native_operator em) {
    int i = 0;
    type *left = _left, *right = NULL;
    for (i = 0; i < forth_code_skip_ipc;) {
        right = forth_code_skip[i];
        if(right && right->em == em){
            right = forth_code_skip[i+1];
            if (right && right->em == VAR &&  !strcmp(left->u_data.s_data, right->u_data.s_data)) {
                return forth_code_skip[i + 2];
            }
        }
        i += 3;
    }
    return NULL;
}

void fix_code_address(void *_left, int real_address, int is_local) {
    if(is_local == 0){
        local_recycle_contain[local_recycle_contain_ipc++] = global_forth_code[((type*)_left)->u_data.i_data] = int_type(real_address);
    }
    else{
        local_recycle_contain[local_recycle_contain_ipc++] = local_forth_code[((type*)_left)->u_data.i_data] = int_type(real_address);
    }
}

void fix_code_relative_address(void *_left, int real_address, int is_local) {
    if(is_local == 0){
        local_recycle_contain[local_recycle_contain_ipc++] = global_forth_code[((type*)_left)->u_data.i_data] = int_type(real_address - ((type*)_left)->u_data.i_data);
    }
    else{
        local_recycle_contain[local_recycle_contain_ipc++] = local_forth_code[((type*)_left)->u_data.i_data] = int_type(real_address - ((type*)_left)->u_data.i_data);
    }
}

void fix_unfix_code(void *_left, int real_address) {
    long i = 0;
    type *left = _left, *right = NULL;
    for (i = 0; i < forth_code_unfix_ipc;) {
        right = forth_code_unfix[i];
        if (!strcmp(left->u_data.s_data, right->u_data.s_data)) {
            local_recycle_contain[local_recycle_contain_ipc++] = global_forth_code[((type*)(forth_code_unfix[i + 1]))->u_data.i_data] = int_type(real_address);
        }
        i += 2;
    }
}

/*follow  code not use now ,it's different  is  different  calculate style  2013.10.10*/
void * handle_materialjit(char *buf, int head);
void * original_eval(void *_left) {
    type * outcome = primitive_empty;
    type * left = c_car(_left);
    int size = 0;
    char *a_storage = NULL;
    char debug_inf[256] = "\0";

    if (left->em != STORAGE) {
        gc(_left);
        cross_strcpy(debug_inf, "eval parameter NOT a storage");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }

    a_storage  = left->u_data.a_storage + sizeof(int);
    size = *(int*)left->u_data.a_storage;
    if ((size == 0) || (size == 1)) {
        gc(_left);
        cross_strcpy(debug_inf, "eval parameter length is 0");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }
    cross_printf("eval material is %s  \r\n", a_storage);
    outcome = handle_materialjit(a_storage, 0);
    gc(_left);
    return outcome;
}

void * eval_impl(void * _left, void ** _env, void * forth_code[], long * forth_code_ipc, int is_local);
void *localeval(void *_left, void **_env);
void * execution(void** _env, void * forth_code[], long begin, long end, char *this) {
#define  STREAM_STOP 99999
    long * forth_return_array_inner;
    long forth_return_ipc_inner;


    void * * forth_data_array_inner ; 
    long forth_data_ipc_inner = 0;
    /*
    char * response = NULL;
    char * responsex = NULL;
    size_t storage_size = 0;
    */
    char debug_inf[256] = "\0";

    long i = 0, j = 0, eval_flag = 1;
    type * typology = NULL, *present = NULL, *left = NULL;

    entry result, result_x;
    long forth_return_ipc_inner_storage = 0;

    void * m_env = NULL;

    if(this == NULL){
        forth_return_array_inner = (long*) cross_calloc(sizeof(long), FORTH_WORD * 3);
        forth_return_ipc_inner = 0;

        forth_data_array_inner = (void **) cross_calloc(sizeof(void*), FORTH_WORD );
        forth_data_ipc_inner = 0;
    }
    else{
        result = lookup((char *) this,
                        global_stack);

        result_x = lookup((char *) this,
                          global_frame);

        if (result._env && result_x._env){
            forth_return_ipc_inner = (long)result.u_data._expr;
            forth_return_array_inner = result._env;

            forth_data_ipc_inner = (long)result_x.u_data._expr;
            forth_data_array_inner = result_x._env;
        }
        else{
            cross_printf("unset g_frame \r\n");
            forth_return_array_inner = (long*) cross_calloc(sizeof(long), FORTH_WORD * 3);
            forth_return_ipc_inner = 0;
            assign(this, (void *) forth_return_ipc_inner,
                   (void*)forth_return_array_inner,
                   &global_stack);

            forth_data_array_inner = (void**) cross_calloc(sizeof(void **), FORTH_WORD );
            forth_data_ipc_inner = 0;
            assign(this, (void *) forth_data_ipc_inner,
                   (void*)forth_data_array_inner,
                   &global_frame);
        }
    }

    for (i = begin; i < end; i++) {
        typology = forth_code[i];
        switch (typology->em) {
        case EFSETQ:
            forth_data_array_inner[forth_data_ipc_inner - 1] = c_set_global_var_value(
                                                                                      forth_code[++i], forth_data_array_inner[forth_data_ipc_inner - 1]);
            break;
        case EFDEFINE:
            forth_data_array_inner[forth_data_ipc_inner - 1] = c_set_global_define_value(
                                                                                         forth_code[++i], forth_data_array_inner[forth_data_ipc_inner - 1]);
            break;
        case EFQUOTE:
            forth_data_array_inner[forth_data_ipc_inner++] = c_copy_type(forth_code[++i]);
            break;
        case EFQUOTEX:
            while(1){
                i++;
                typology = forth_code[i];
                if(typology->em == EFQUOTEXEND)break;
            }
            break;
        case VAR:
            present = c_find_var_value(typology, *_env);
            if(present == NULL){
                cross_sprintf(debug_inf, "execution var %s value is None\r\n", typology->u_data.s_data);
                typology = new_debug(debug_inf, cross_strlen(debug_inf));
                goto yield_out;
                /*                forth_data_array_inner[forth_data_ipc_inner++] = primitive_empty;*/
            }
            else{
                forth_data_array_inner[forth_data_ipc_inner++] = c_copy_type(present);
            } 
            break;
        case STORAGE:
        case EMPTY:
        case INTEGER:
            forth_data_array_inner[forth_data_ipc_inner++] = c_copy_type(typology);
            break;
        case EVARGSCOMBI:
            forth_data_array_inner[forth_data_ipc_inner - 2] = c_appdix(
                                                                        forth_data_array_inner[forth_data_ipc_inner - 2],
                                                                        c_cons(forth_data_array_inner[forth_data_ipc_inner - 1], primitive_empty));
            forth_data_ipc_inner--;
            break;
        case EFFUN:
            typology = forth_code[++i];
            present = typology->u_data.f_data(forth_data_array_inner[forth_data_ipc_inner - 1]);
            if ((present->em == DEBUG) && (typology->u_data.f_data != original_eval)) {
                cross_sprintf(debug_inf, "execution find DEBUG\r\n%s\r\n", present->u_data.a_storage + sizeof(int));
                gc(present);
                typology = new_debug(debug_inf, cross_strlen(debug_inf));
                goto yield_out;
            }
            forth_data_array_inner[forth_data_ipc_inner - 1] = present;
            if(forth_return_ipc_inner > (unsigned int)(0.99 * FORTH_WORD * 3)){
                cross_printf("fun call stack status  is  %ld  %ld  \r\n", forth_return_ipc_inner, forth_data_ipc_inner);
            }
            break;
        case EFSTOP:
            typology = forth_data_array_inner[forth_data_ipc_inner - 1];
            assign(this, (void *) STREAM_STOP,
                   *_env,
                   &global_stream);
            c_unbindvars(_env);

            result = lookup((char *) this,
                            global_stack);
            result_x = lookup((char *) this,
                              global_frame);

            if (result._env && result_x._env){
                forth_return_ipc_inner = (long)result.u_data._expr;
                forth_return_array_inner = result._env;
                cross_free(forth_return_array_inner);
                global_stack = freeentry(this, global_stack);

                forth_data_ipc_inner = (long)result_x.u_data._expr;
                forth_data_array_inner = result_x._env;
                cross_free(forth_data_array_inner);
                global_frame = freeentry(this, global_frame);

                global_lambda = freeentry(this, global_lambda);
            }
            else{
                cross_printf("exit efstop entry error \r\n");
            }
            /*can't delete the reference count, otherwise would core down nexttime.
            gc(typology);
            */
            goto yield_out;
            /*      break;*/
        case EFYIELD:
            typology = forth_data_array_inner[forth_data_ipc_inner - 1];
            left = c_copy_type(c_caar(typology));
            present = c_cadar(typology);

            if(present->u_data.i_data == 1){
                assign(this, (void *) STREAM_STOP,
                       *_env,
                       &global_stream);
                c_unbindvars(_env);

                result = lookup((char *) this,
                                global_stack);
                result_x = lookup((char *) this,
                                  global_frame);

                if (result._env && result_x._env){
                    forth_return_ipc_inner = (long)result.u_data._expr;
                    forth_return_array_inner = result._env;
                    cross_free(forth_return_array_inner);
                    global_stack = freeentry(this, global_stack);

                    forth_data_ipc_inner = (long)result_x.u_data._expr;
                    forth_data_array_inner = result_x._env;
                    cross_free(forth_data_array_inner);
                    global_frame = freeentry(this, global_frame);

                    global_lambda = freeentry(this, global_lambda);
                    gc(typology);   /*never come back, so we need delete it. */
                }
                else{
                    cross_printf("exit efstop entry error \r\n");
                }
            }
            else{
                assign(this, (void *) (i + 1),
                       *_env,
                       &global_stream);

                assign(this, (void *) forth_return_ipc_inner,
                       (void*)forth_return_array_inner,
                       &global_stack);

                assign(this, (void *) forth_data_ipc_inner,
                       (void*)forth_data_array_inner,
                       &global_frame);
            }

            typology = left;
            goto  yield_out;
        case EFNEXT:
            typology = forth_data_array_inner[forth_data_ipc_inner - 1];
            present = c_car(typology);
            result = lookup((char *) (present->u_data.s_data),
                            global_stream);

            result_x = lookup((char *) (present->u_data.s_data),
                              global_lambda);
            if (!result.u_data._expr){
                j = (long)result_x.u_data._expr + 1;
                m_env = result_x._env;
            }
            else{
                j = (long)result.u_data._expr;
                m_env = result._env;
            }
            forth_data_array_inner[forth_data_ipc_inner - 1] =  execution(&m_env, global_forth_code, j, global_forth_code_ipc, present->u_data.s_data);

            gc(typology);
            break;
        case EFEOFSTDIN:
            /*
            typology = forth_data_array_inner[forth_data_ipc_inner - 1];
            if( !feof(stdin)){
                present = int_type(0);
            }
            else{
                present = int_type(1);
            }
            forth_data_array_inner[forth_data_ipc_inner - 1] = present;

            gc(typology);
            */
            break;
        case EFSTDIN:
            /*some input too long, so use BUF_SIZE instead of 1024*/
            /*
            typology = forth_data_array_inner[forth_data_ipc_inner - 1];
            response = (char*) cross_calloc(BUF_SIZE, sizeof(char));
            fgets(response, BUF_SIZE, stdin); 

            present = new_storage(response, cross_strlen(response));
            cross_free(response);
            forth_data_array_inner[forth_data_ipc_inner - 1] = present;
            gc(typology);
            */
            break;
        case EFISSTOP:
            typology = forth_data_array_inner[forth_data_ipc_inner - 1];
            present = c_car(typology);
            result = lookup((char *) (present->u_data.s_data),
                            global_stream);
            if (!result.u_data._expr){
                forth_data_array_inner[forth_data_ipc_inner - 1] = int_type(0);
            }
            else{
                j = (long)result.u_data._expr;
                if(j == STREAM_STOP){
                    forth_data_array_inner[forth_data_ipc_inner - 1] = int_type(1);
                    global_stream = freeentry((char *) (present->u_data.s_data), global_stream);
                }
                else{
                    forth_data_array_inner[forth_data_ipc_inner - 1] = int_type(0);
                }
            }
            gc(typology);
            break;
        case EFSAPPLYX:
            forth_return_array_inner[forth_return_ipc_inner++] = i + 2;
            i = ((type*)forth_code[i + 1])->u_data.i_data;
            forth_return_array_inner[forth_return_ipc_inner++] = i;
            present = forth_code[++i];
            _env = c_bindvars(present, forth_data_array_inner[--forth_data_ipc_inner], _env);
            break;
        case EFSAPPLYXBEGIN:
            present = primitive_empty;
            while(1){
                i++;
                typology = forth_code[i];
                if(typology->em == EFSAPPLYX)break;
                present = c_appdix(present, c_cons(typology, primitive_empty));
            }
            forth_data_array_inner[forth_data_ipc_inner++] = present;
            i--;
            break;
        case EFSAPPLY:
            /*eliminate  tail-recurrsion*/
            typology = forth_code[i + 2];
            if ((typology != NULL) && (typology->em == EFDEFUNEND || typology->em == EFDEFUNTAIL)) {
                while ( forth_return_ipc_inner >= 1  ) {
                    typology = forth_code[forth_return_array_inner[forth_return_ipc_inner - 1]];
                    if ((typology != NULL) && (typology->em == EFDEFUNEND || typology->em == EFDEFUNTAIL)) {
                        if (((type*) (*_env))->em == EMPTY) {
                        } 
                        else {
                            c_unbindvars(_env);
                        }
                        --forth_return_ipc_inner;
                    } 
                    else {
                        break;
                    }
                }
            }

            forth_return_array_inner[forth_return_ipc_inner++] = i + 2;
            i = ((type*)forth_code[i + 1])->u_data.i_data;
            present = forth_code[++i];
            _env = c_bindvars(present, forth_data_array_inner[--forth_data_ipc_inner],
                              _env);
            break;
        case EFFUNCALL:
            break;
        case EFSELF:
            typology = forth_data_array_inner[forth_data_ipc_inner - 1];
            result = lookup((char *) this,
                            global_lambda);
            if (!result.u_data._expr){
                cross_printf("efself impossible efself \r\n");
            }
            else{
                i = (long)result.u_data._expr;
            }

            present = forth_code[i];
            c_unbindvars(_env);
            /*      --forth_return_ipc_inner;*/
            _env = c_bindvars(present, forth_data_array_inner[--forth_data_ipc_inner],
                              _env);
            /*      forth_return_array_inner[forth_return_ipc_inner++] = i + 2;*/
            break;
        case EFIF:
            if (c_eq(forth_data_array_inner[--forth_data_ipc_inner], int_type(0))) {
                i += ((type*)forth_code[i + 1])->u_data.i_data + 1;
                i--;
            } 
            else {
                i++;
            }
            break;
        case EFELSEIF:
            i += ((type*)forth_code[i + 1])->u_data.i_data + 1;
            i--;
            break;
        case EFPROGNMID:
            /* for yield special situation*/
            if(forth_data_ipc_inner >= 1){
                gc(forth_data_array_inner[--forth_data_ipc_inner]);
            }
            break;
        case EFDEFUN:
            /*statement ends*/
            forth_data_array_inner[forth_data_ipc_inner++] = c_copy_type( forth_code[i+1]);
            while(1){
                i++;
                typology = forth_code[i];
                if(typology->em == EFDEFUNEND)break;
            }
            break;
        case EFDEFMACRO:
            forth_data_array_inner[forth_data_ipc_inner++] = c_copy_type( forth_code[i+1]);
            while(1){
                i++;
                typology = forth_code[i];
                if(typology->em == EFDEFUNEND)break;
            }
            break;
        case EFDEFUNTAIL:
        case EFDEFUNEND:
            if (forth_return_ipc_inner == forth_return_ipc_inner_storage) {
                /*	forth_data_array_inner[forth_data_ipc_inner++] = var_type("defunend");*/
                i = end;
            } 
            else{
                if (((type*) (*_env))->em == EMPTY) {
                } 
                else {
                    c_unbindvars(_env);
                }
                i = forth_return_array_inner[--forth_return_ipc_inner];
                i--;
            }
            break;
        case EFDEFMACROEND:
            j = i;
            i = forth_return_array_inner[--forth_return_ipc_inner] + 2;
            forth_return_array_inner[forth_return_ipc_inner++] = global_forth_code_ipc;
            while(1){
                typology = global_forth_code[i];

                if(typology->em == LIST ){
                    present = c_car(typology);
                    if(present->em == EVAL){
                        global_forth_code[global_forth_code_ipc++] = localeval(c_cadr(typology), _env);
                    }
                }
                else if(typology->em == VAR ){
                    if(eval_flag){
                        present = c_find_var_value(typology, *_env);
                        eval_impl(present, _env, global_forth_code, &global_forth_code_ipc, 0);
                    }
                    else{
                        global_forth_code[global_forth_code_ipc++] = c_copy_type(typology);
                    }
                }
                else if(typology->em == EFQUOTEX){
                    eval_flag = 0;
                }
                else if(typology->em == EFQUOTEXEND){
                    eval_flag = 1;
                }
                else if(typology->em == EFDEFMACROEND){
                    global_forth_code[global_forth_code_ipc++] = primitive_efdefunend;
                    break;
                }
                else{
                    global_forth_code[global_forth_code_ipc++] = c_copy_type(typology);
                }
                i++;
            }
            i = forth_return_array_inner[--forth_return_ipc_inner];
            execution(_env, global_forth_code, i, global_forth_code_ipc, NULL);
            i = j;
            break;
        case EFPRESS:
            typology = forth_code[++i];
            result = lookup((char *) (typology->u_data.s_data), global_var);
            if(result.u_data._expr == NULL){
                assign((char *) (typology->u_data.s_data),
                       c_cons(forth_data_array_inner[--forth_data_ipc_inner],
                              primitive_empty), NULL, &global_var);
            }
            else{
                assign((char *) (typology->u_data.s_data),
                       c_cons(forth_data_array_inner[--forth_data_ipc_inner],
                              result.u_data._expr), NULL, &global_var);
            }

            forth_data_array_inner[forth_data_ipc_inner++] = primitive_empty;
            break;
        case EFAPPEND:
            typology = forth_code[++i];
            result = lookup((char *) (typology->u_data.s_data), global_var);
            assign((char *) (typology->u_data.s_data),
                   c_appdix(result.u_data._expr,
                            c_cons(forth_data_array_inner[--forth_data_ipc_inner], primitive_empty)), NULL,
                   &global_var);
            forth_data_array_inner[forth_data_ipc_inner++] = primitive_empty;
            break;
        case EFWHOLE:
            typology = forth_code[++i];
            result = lookup((char *) (typology->u_data.s_data), global_var);
            if(result.u_data._expr == NULL){
                forth_data_array_inner[forth_data_ipc_inner++] = primitive_empty;
            }
            else{
                forth_data_array_inner[forth_data_ipc_inner++] = c_copy_type(
                                                                             result.u_data._expr);
            }
            break;
        case EFEXCHANGE:
            typology = forth_code[++i];
            result = lookup((char *) (typology->u_data.s_data), global_var);
            assign((char *) (typology->u_data.s_data),
                   c_cons(c_cadr(result.u_data._expr),
                          c_cons(c_car(result.u_data._expr),
                                 c_cddr(result.u_data._expr))), NULL,
                   &global_var);
            gc_atom(c_cdr(result.u_data._expr));
            gc_atom(result.u_data._expr);
            forth_data_array_inner[forth_data_ipc_inner++] = primitive_empty;
            break;
        case EFCOMBI:
            typology = forth_code[++i];
            result = lookup((char *) (typology->u_data.s_data), global_var);
            assign((char *) (typology->u_data.s_data),
                   c_cons(
                          c_appdix(c_cadr(result.u_data._expr),
                                   c_cons(c_car(result.u_data._expr), primitive_empty)),
                          c_cddr(result.u_data._expr)), NULL, &global_var);
            gc_atom(c_cdr(result.u_data._expr));
            gc_atom(result.u_data._expr);
            forth_data_array_inner[forth_data_ipc_inner++] = primitive_empty;
            break;
        case EFTOP:
            typology = forth_code[++i];
            result = lookup((char *) (typology->u_data.s_data), global_var);
            present = result.u_data._expr;
            if (present == NULL || present->em == EMPTY) {
                forth_data_array_inner[forth_data_ipc_inner++] = primitive_empty;
            } 
            else {
                forth_data_array_inner[forth_data_ipc_inner++] = c_copy_type(
                                                                             c_car(present));
            }
            break;
        case EFEJECT:
            /* follow code doesn't used  eval (left,  ...) style ,  as the statement would copy  the used default,  but we can used the essential code*/
            typology = forth_code[++i];
            result = lookup((char *) (typology->u_data.s_data), global_var);
            present = result.u_data._expr;
            if (present == NULL || present->em == EMPTY) {
                assign((char *) (typology->u_data.s_data), primitive_empty,
                       NULL, &global_var);
                forth_data_array_inner[forth_data_ipc_inner++] = primitive_empty;
            } 
            else {
                assign((char *) (typology->u_data.s_data), c_cdr(present), NULL,
                       &global_var);
                forth_data_array_inner[forth_data_ipc_inner++] = c_car(present);
                gc_atom(present);
            }
            break;
        case EFFUNCALLEND:
            present = forth_data_array_inner[--forth_data_ipc_inner];
            if (present->em == EMPTY) {
                forth_data_array_inner[forth_data_ipc_inner++] = primitive_empty;
                --forth_data_ipc_inner;
                cross_printf("forth_data_ipc_inner  is  %ld  \r\n", forth_data_ipc_inner);
            } 
            else {
                if (present->em != VAR) {
                    cross_strcpy(debug_inf, "FUNCALL abstract name is INVALID, shoule be VAR\r\n");
                    typology = new_debug(debug_inf, cross_strlen(debug_inf));
                    gc(present);
                    goto yield_out;
                }
                result = lookup((char *) (present->u_data.s_data),
                                global_lambda);
                if (result.u_data._expr == NULL) {
                    if (iterate_address(present) == NULL) {
                        cross_strcpy(debug_inf, "FUNCALL abstract name doesn't exist\r\n");
                        typology = new_debug(debug_inf, cross_strlen(debug_inf));
                        gc(present);
                        goto yield_out;
                    } 
                    else {
                        result.u_data._expr =
                            (void*) ((iterate_address(present) + 1));
                    }
                } 
                else {
                    forth_return_array_inner[forth_return_ipc_inner++] = (long) *_env;
                }
                if (!result._env) {
                } 
                else {
                    *_env = result._env;
                }

                forth_return_array_inner[forth_return_ipc_inner++] = i + 1;
                i = (long) result.u_data._expr;
                gc_atom(present);
                /* think why? */
                present = forth_code[i];
                _env = c_bindvars(present, forth_data_array_inner[--forth_data_ipc_inner],
                                  _env);
            }
            break;
        case EFLAMBDAEND:   /*modify  by rosslyn  for  dynamic calc,  20140810*/
            if (forth_return_ipc_inner == forth_return_ipc_inner_storage) {
                i = end;
            } 
            else{
                if (((type*) (*_env))->em == EMPTY) {
                } 
                else {
                    c_unbindvars(_env);
                }
                i = forth_return_array_inner[--forth_return_ipc_inner];
                i--;
                *_env = (void*) forth_return_array_inner[--forth_return_ipc_inner];
            }
            break;
        case EFLAMBDA:
            present = random_name();
            if (((type*) (*_env))->em == EMPTY) {
                assign(present->u_data.s_data, (void *) (i + 2),
                       c_cons(primitive_empty, primitive_empty),
                       &global_lambda);
            } 
            else {
                assign(present->u_data.s_data, (void *) (i + 2),
                       c_cons(c_copy_type(c_car ( * _env )), primitive_empty),
                       &global_lambda);
            }

            forth_data_array_inner[forth_data_ipc_inner++] = present;
            i = ((type*)forth_code[i + 1])->u_data.i_data;
            break;
        default:
            cross_printf("unhandled typology \r\n");
            break;
        }
    }

    if(forth_data_ipc_inner != 1){
        cross_printf("$$$$$$$$$$$$$$$   %ld   $$$$$$$$$$$$$$$$$$$$$$$$$", forth_data_ipc_inner);
    }
    typology = forth_data_array_inner[forth_data_ipc_inner - 1];


    if(this == NULL){
        cross_free(forth_return_array_inner);
        forth_return_ipc_inner = 0;

        cross_free(forth_data_array_inner);
        forth_data_ipc_inner = 0;
    }

 yield_out:
    return typology;
}


void modify_fun_content(type *right){
    int signx = 0;

    signx = 0;
    while (1) {
        if (!strcmp("", normal_fun[signx].name)) {
            break;
        }
        if(normal_fun[signx].address == right->u_data.f_data){
            cross_strcpy(right->u_data.s_data, normal_fun[signx].name);
            break;
        }
        signx++;
    }
}

void  modify_fun_content_x(type *left){
    int signx = 0;

    signx = 0;
    while (1) {
        if (!strcmp("", normal_fun[signx].name)) {
            break;
        }
        if(!strcmp(normal_fun[signx].name, left->u_data.s_data)){
            left->u_data.f_data = normal_fun[signx].address;
            break;
        }
        signx++;
    }
}

void * execution_strategy(void** _env, void *forth_code[], long begin, long end){
    char file_tmp_name[256] = "";

    if(cross_strlen(global_filename) == 0){  /*for  debug reason, normal change 0 to 1*/
        return execution(_env, forth_code, begin, end, NULL);
    }
    else{
        cross_sprintf(file_tmp_name, "%sf", global_filename);
#ifdef LINUX
        write_forth_code(file_tmp_name);
        read_forth_code(file_tmp_name);
#endif
        return execution(_env, forth_code, begin, end, NULL);
    }
}

void * eval_impl(void * _left, void ** _env, void * forth_code[], long *_forth_code_ipc, int is_local) {
    type * present, *outcome = NULL;
    type * left = _left;
    int skip = 0;
    unsigned long forth_code_ipc = *_forth_code_ipc;
    char debug_inf[256] = "\0";

 label: 
    switch (left->em) {
    case EMPTY:
        forth_code[forth_code_ipc++] = left;
        goto popjreturn;
        break;
    case STORAGE:
    case VAR:
        forth_code[forth_code_ipc++] = left;
        goto popjreturn;
        break;
    case INTEGER:
        if (left->u_data.i_data == NULLVALUE) {
            forth_code[forth_code_ipc++] = primitive_empty;
            goto popjreturn;
        } 
        else {
            forth_code[forth_code_ipc++] = left;
            goto popjreturn;
        }
        break;
    default:
        /*    assert(left->em == LIST);*/
        present = c_car (left );
        switch (present->em) {
        case EMPTY:
            batch_write_circuit(primitive_empty,
                                binary_data_array + binary_data_label++);
            goto popjreturn;
        case SYMBOL:
            goto popjreturn;
            break;
        case FUNCALL:
            batch_write_circuit(c_cadr (left),
                                binary_return_array + (binary_return_label++));
            batch_write_circuit(primitive_evfuncall,
                                binary_return_array + (binary_return_label++));
            batch_write_circuit(c_cddr(left),
                                binary_return_array + (binary_return_label++));
            forth_code[forth_code_ipc++] = primitive_empty;
            goto evargs;
            break;
        case SELF:
            batch_write_circuit(present,
                                binary_return_array + (binary_return_label++));
            batch_write_circuit(c_cdr(left),
                                binary_return_array + (binary_return_label++));
            forth_code[forth_code_ipc++] = primitive_empty;
            goto evargs;
            break;
        case LAMBDA:
            present = c_cdr(left);
            batch_write_circuit(int_type (forth_code_ipc + 1),
                                binary_return_array + (binary_return_label++));
            batch_write_circuit(primitive_eflambda,
                                binary_return_array + (binary_return_label++));

            forth_code[forth_code_ipc++] = primitive_eflambda;
            forth_code_ipc++; /*for  skip*/
            forth_code[forth_code_ipc++] = c_car(present);
            left = c_cadr(present);
            goto label;
            break;
        case SETQ:
            /*only  consider  one  key -value  situation , 2013.5.16*/
            batch_write_circuit(c_cadr (left),
                                binary_return_array + (binary_return_label++));
            batch_write_circuit(primitive_evsetq,
                                binary_return_array + (binary_return_label++));
            left = c_caddr(left);
            goto label;
            break;
        case DEFINE:
            /*only  consider  one  key -value  situation , 2013.5.16*/
            batch_write_circuit(c_cadr (left),
                                binary_return_array + (binary_return_label++));
            batch_write_circuit(primitive_evdefine,
                                binary_return_array + (binary_return_label++));
            left = c_caddr(left);
            goto label;
            break;
        case IF:
            batch_write_circuit(c_cddr (left),
                                binary_return_array + (binary_return_label++));
            batch_write_circuit(primitive_evif,
                                binary_return_array + (binary_return_label++));
            left = c_cadr(left);
            goto label;
            break;
        case PROGN:
            batch_write_circuit(c_cdr (left),
                                binary_return_array + (binary_return_label++));
            goto evprogn;
        case EJECT:
            left = c_cadr(left);
            forth_code[forth_code_ipc++] = primitive_efeject;
            forth_code[forth_code_ipc++] = left;
            goto popjreturn;
            break;
        case WHOLE:
            left = c_cadr(left);
            forth_code[forth_code_ipc++] = primitive_efwhole;
            forth_code[forth_code_ipc++] = left;
            goto popjreturn;
            break;
            break;
        case EXCHANGE:
            left = c_cadr(left);
            forth_code[forth_code_ipc++] = primitive_efexchange;
            forth_code[forth_code_ipc++] = left;
            goto popjreturn;
            break;
        case COMBI:
            left = c_cadr(left);
            forth_code[forth_code_ipc++] = primitive_efcombi;
            forth_code[forth_code_ipc++] = left;
            goto popjreturn;
            break;
        case TOP:
            left = c_cadr(left);
            forth_code[forth_code_ipc++] = primitive_eftop;
            forth_code[forth_code_ipc++] = left;
            goto popjreturn;
            break;
        case APPEND:
            /* follow code doesn't used  eval (left, ...) style , as the statement would copy  the used default, but we can used the essential code*/
            outcome = c_caddr(left);
            left = c_cadr(left);

            batch_write_circuit(left,
                                binary_return_array + (binary_return_label++));
            batch_write_circuit(primitive_evappend,
                                binary_return_array + (binary_return_label++));
            left = outcome;
            goto label;
            break;
        case PRESS:
            /* follow code doesn't used  eval (left, ...) style , as the statement would copy  the used default, but we can used the essential code*/
            outcome = c_caddr(left);
            left = c_cadr(left);

            batch_write_circuit(left,
                                binary_return_array + (binary_return_label++));

            batch_write_circuit(primitive_evpress,
                                binary_return_array + (binary_return_label++));
            left = outcome;
            goto label;
            break;
        case INTEGER:
            break;
        case LIST: /*for macro forth*/
            forth_code[forth_code_ipc++] = left;
            goto popjreturn;
            break;
        case FUN:
            batch_write_circuit(present,
                                binary_return_array + (binary_return_label++));
            batch_write_circuit(c_cdr(left),
                                binary_return_array + (binary_return_label++));
            forth_code[forth_code_ipc++] = primitive_empty;
            goto evargs;
            break;
        case EOFSTDIN:
        case STDIN:
        case NEXT:
        case ISSTOP:
        case STOP:
        case YIELD:
            batch_write_circuit(present,
                                binary_return_array + (binary_return_label++));
            batch_write_circuit(c_cdr(left),
                                binary_return_array + (binary_return_label++));
            forth_code[forth_code_ipc++] = primitive_empty;
            goto evargs;
            break;
        case DEFUN:
            /*      left = c_copy_type(left);*/
            present = c_cdr(left);
            batch_write_circuit(primitive_efdefun,
                                binary_return_array + (binary_return_label++));

            forth_code[forth_code_ipc++] = primitive_efdefun;

            forth_code_skip[forth_code_skip_ipc++] = primitive_efdefun;
            forth_code_skip[forth_code_skip_ipc++] = c_car(present);
            local_recycle_contain[local_recycle_contain_ipc++] = forth_code_skip[forth_code_skip_ipc++] = int_type( forth_code_ipc);	
            fix_unfix_code(c_car(present), forth_code_ipc);

            /*	forth_code[forth_code_ipc++] = c_copy_atom(c_car(present));*/
            forth_code[forth_code_ipc++] = c_car(present);
            forth_code[forth_code_ipc++] = c_cadr(present);
            left = c_caddr(present);
            goto label;
            break;
        case VAR:
            if (1) {
                if (fetch_code_skip_address(present, EFDEFUN)) {
                    batch_write_circuit(c_car(left),
                                        binary_return_array + (binary_return_label++));
                    batch_write_circuit(c_cdr(left),
                                        binary_return_array + (binary_return_label++));
                    forth_code[forth_code_ipc++] = primitive_empty;
                    goto evargs;
                }
                else if (fetch_code_skip_address(present, EFDEFMACRO)) {
                    forth_code[forth_code_ipc++] = primitive_efsapplyxbegin;
                    outcome = c_cdr(left);
                    while(outcome->em != EMPTY){
                        forth_code[forth_code_ipc++] = c_car(outcome);
                        outcome = c_cdr(outcome);
                    }
                    forth_code[forth_code_ipc++] = primitive_efsapplyx;
                    if (fetch_code_skip_address(present, EFDEFMACRO) == NULL) {
                        forth_code_unfix[forth_code_unfix_ipc++] = c_car(left);
                        forth_code_unfix[forth_code_unfix_ipc++] = int_type(forth_code_ipc++);
                    } 
                    else {
                        forth_code[forth_code_ipc++] = fetch_code_skip_address(present, EFDEFMACRO);
                    }
                    goto popjreturn;
                } 
                else { /*un resolved symbol,fixit later*/
                    batch_write_circuit(c_car(left),
                                        binary_return_array + (binary_return_label++));
                    batch_write_circuit(c_cdr(left),
                                        binary_return_array + (binary_return_label++));
                    forth_code[forth_code_ipc++] = primitive_empty; /* primitive_empty is parameter, use for args combinate*/
                    goto evargs;
                }
            }
            break;
        case DEFMACRO:
            left = c_copy_type(left);
            present = c_cdr(left);
            batch_write_circuit(primitive_efdefmacro,
                                binary_return_array + (binary_return_label++));

            forth_code[forth_code_ipc++] = primitive_efdefmacro;

            forth_code_skip[forth_code_skip_ipc++] = primitive_efdefmacro;
            forth_code_skip[forth_code_skip_ipc++] = c_car(present);
            forth_code_skip[forth_code_skip_ipc++] = int_type( forth_code_ipc);
            fix_unfix_code(c_car(present), forth_code_ipc);

            forth_code[forth_code_ipc++] = c_copy_atom(c_car(present));
            forth_code[forth_code_ipc++] = c_cadr(present);
            left = c_caddr(present);
            goto label;
        case QUOTE:
            forth_code[forth_code_ipc++] = primitive_efquote;
            forth_code[forth_code_ipc++] = c_cadr (left);
            goto popjreturn;
            break;
        case QUOTEX:
            batch_write_circuit(primitive_efquotex,
                                binary_return_array + (binary_return_label++));
            forth_code[forth_code_ipc++] = primitive_efquotex;
            left = c_cadr (left);
            goto label;
            break;
        case EVAL:
            forth_code[forth_code_ipc++] = left;
            goto popjreturn;
            break;
        default:
            /*
            eval_forth(_left, _env, forth_code, _forth_code_ipc);
            return execution_forth(forth_code, 0, *_forth_code_ipc);
            */
            cross_printf("maybe forth code\r\n");
            break;
        }
    } 
    /*modify by  rosslyn  .2013.6.4
      binary_data_array[binary_data_label ++ ] =  NULL;*/
    goto popjreturn;
 evsetq: 
    forth_code[forth_code_ipc++] = primitive_efsetq;
    forth_code[forth_code_ipc++] = batch_read_circuit(
                                                      binary_return_array + (--binary_return_label));
    goto popjreturn;
 evdefine: 
    forth_code[forth_code_ipc++] = primitive_efdefine;
    forth_code[forth_code_ipc++] = batch_read_circuit(
                                                      binary_return_array + (--binary_return_label));
    goto popjreturn;
 evappend: 
    left = batch_read_circuit(
                              binary_return_array + --binary_return_label);
    forth_code[forth_code_ipc++] = primitive_efappend;
    forth_code[forth_code_ipc++] = left;
    goto popjreturn;
 evpress: 
    left = batch_read_circuit(
                              binary_return_array + --binary_return_label);
    forth_code[forth_code_ipc++] = primitive_efpress;
    forth_code[forth_code_ipc++] = left;
    goto popjreturn;
 evprognmid: 
    forth_code[forth_code_ipc++] = primitive_efprognmid;
    goto evprogn;
 evprognend: 
    goto popjreturn;
 evprogn: 
    left = batch_read_circuit(
                              binary_return_array + --binary_return_label);
    if (((type *) c_cdr(left))->em == EMPTY) {
        batch_write_circuit(primitive_evprognend,
                            binary_return_array + binary_return_label++);
        left = c_car(left);
        goto label;
    } 
    else {
        batch_write_circuit(c_cdr(left),
                            binary_return_array + binary_return_label++);
        batch_write_circuit(primitive_evprognmid,
                            binary_return_array + binary_return_label++);
        left = c_car(left);
        goto label;
    }
 evif: 
    forth_code[forth_code_ipc++] = primitive_efif;

    left = batch_read_circuit(binary_return_array + --binary_return_label);
    local_recycle_contain[local_recycle_contain_ipc++] = present = int_type (forth_code_ipc++);
    batch_write_circuit(present,
                        binary_return_array + binary_return_label++);

    /*handle  no  else statement situation*/
    if (((type*) c_cdr(left))->em == EMPTY) {
        batch_write_circuit(primitive_empty,
                            binary_return_array + binary_return_label++);
    } 
    else {
        batch_write_circuit(c_cadr(left),
                            binary_return_array + binary_return_label++);
    }
    batch_write_circuit(primitive_efelseif,
                        binary_return_array + binary_return_label++);
    left = c_car(left);
    goto label;
 evfuncallmid: 
    forth_code[forth_code_ipc++] = primitive_effuncallend;
    goto popjreturn;
 evfuncallend: 
    goto popjreturn;
 evfuncall: 
    left = batch_read_circuit(
                              binary_return_array + --binary_return_label);
    forth_code[forth_code_ipc++] = primitive_effuncall;
    batch_write_circuit(primitive_evfuncallmid,
                        binary_return_array + binary_return_label++);
    goto label;
 evargslast: 
    switch (((type *) batch_read_circuit(
                                         binary_return_array + binary_return_label - 1))->em) {
 case EVFUNCALL:
     binary_return_label--;
     goto evfuncall;
     break;
 case EVFUNCALLMID:
     binary_return_label--;
     goto evfuncallmid;
     break;
 case FUN:
     forth_code[forth_code_ipc++] = primitive_effun;
     forth_code[forth_code_ipc++] = batch_read_circuit(
                                                       binary_return_array + binary_return_label - 1);
     binary_return_label--;
     goto popjreturn;
     break;
 case STOP:
     forth_code[forth_code_ipc++] = primitive_efstop;
     binary_return_label--;
     goto popjreturn;
     break;
 case YIELD:
     forth_code[forth_code_ipc++] = primitive_efyield;
     binary_return_label--;
     goto popjreturn;
     break;
 case NEXT:
     forth_code[forth_code_ipc++] = primitive_efnext;
     binary_return_label--;
     goto popjreturn;
     break;
 case ISSTOP:
     forth_code[forth_code_ipc++] = primitive_efisstop;
     binary_return_label--;
     goto popjreturn;
     break;
 case EOFSTDIN:
     forth_code[forth_code_ipc++] = primitive_efeofstdin;
     binary_return_label--;
     goto popjreturn;
     break;
 case STDIN:
     forth_code[forth_code_ipc++] = primitive_efstdin;
     binary_return_label--;
     goto popjreturn;
     break;
 case SELF:
     forth_code[forth_code_ipc++] = primitive_efself;
     binary_return_label--;
     goto popjreturn;
     break;
 default:
     goto sapply;
     break;
 }
 unbindvars: 
    c_unbindvars(_env);
    goto popjreturn;
 macro: 
    left = batch_read_circuit(binary_data_array + --binary_data_label);
    batch_write_circuit(primitive_evunbindvars,
                        binary_return_array + binary_return_label++);
    goto label;
 sapply: 
    left = batch_read_circuit(
                                   binary_return_array + --binary_return_label);
    forth_code[forth_code_ipc++] = primitive_efsapply;
    if (fetch_code_skip_address(left, EFDEFUN) == NULL) {
        forth_code_unfix[forth_code_unfix_ipc++] = left;
        cross_sprintf(debug_inf, "function name %s doesn't exist\r\n", left->u_data.s_data);
        local_recycle_contain[local_recycle_contain_ipc++] = forth_code_unfix[forth_code_unfix_ipc++] = int_type(forth_code_ipc);
        forth_code[forth_code_ipc++] = new_debug(debug_inf, cross_strlen(debug_inf));
    } 
    else {
        forth_code[forth_code_ipc++] = fetch_code_skip_address(left, EFDEFUN);
    }
    goto popjreturn;
 
    c_find_defun_arg(left, (void **) &present);
    c_find_defun_expr(left, (void **) &left);

    _env = c_bindvars(present,
                      batch_read_circuit(binary_data_array + --binary_data_label), _env);
    batch_write_circuit(primitive_evunbindvars,
                        binary_return_array + binary_return_label++);
    goto label;
 evargscombi: 
    forth_code[forth_code_ipc++] = primitive_evargscombi;
    goto evargs;
 evargs: 
    left = batch_read_circuit(
                              binary_return_array + --binary_return_label);
    if (left->em == EMPTY) {
        goto evargslast;
    } 
    else {
        batch_write_circuit(c_cdr(left),
                            binary_return_array + binary_return_label++);
        batch_write_circuit(primitive_evargscombi,
                            binary_return_array + binary_return_label++);
        left = c_car(left);
        goto label;
    }
 popjreturn: 
    if (binary_return_label == 0) {
        cross_printf("\r\neval_impl result label is  %d  \r\n", binary_data_label);
        *_forth_code_ipc = forth_code_ipc;
        return  NULL;
    }
    switch (((type *) batch_read_circuit(
                                         binary_return_array + --binary_return_label))->em) {
    case EVFUNCALLMID:
        goto evfuncallmid;
        break;
    case EVPROGNMID:
        goto evprognmid;
        break;
    case EVPROGNEND:
        goto evprognend;
        break;
    case EVFUNCALLEND:
        goto evfuncallend;
        break;
    case EVFUNCALL:
        goto evfuncall;
        break;
    case EVIF:
        goto evif;
        break;
    case EVMACRO:
        goto macro;
        break;
    case EVSETQ:
        goto evsetq;
        break;
    case EVDEFINE:
        goto evdefine;
        break;
    case EVARGSCOMBI:
        goto evargscombi;
        break;
    case EVARGS:
        goto evargs;
        break;
    case EVUNBINDVARS:
        goto unbindvars;
        break;
    case EVPRESS:
        goto evpress;
        break;
    case EVAPPEND:
        goto evappend;
    case EFDEFUN:
        goto efdefun;
        break;
    case EFQUOTEX:
        goto efquotex;
        break;
    case EFDEFMACRO:
        goto efdefmacro;
        break;
    case EFLAMBDA:
        goto eflambda;
        break;
    case EFELSEIF:
        goto efelseif;
        break;
    case EFENDIF:
        goto efendif;
        break;
    default:
        break;
    }
    /* return  binary_data_array[--binary_data_label];*/
 efendif: 
    fix_code_relative_address(
                              batch_read_circuit(binary_return_array + --binary_return_label),
                              forth_code_ipc, is_local);
    goto popjreturn;
 efelseif: 
    left = batch_read_circuit(
                              binary_return_array + --binary_return_label);

    fix_code_relative_address(
                              batch_read_circuit(binary_return_array + --binary_return_label),
                              forth_code_ipc + 2, is_local);

    /*for  else real tail-recursion  ,  2014.6.15*/
    skip = 1;
    if (binary_return_label > skip) {
        while (1) {
            present = batch_read_circuit(binary_return_array + binary_return_label - skip);
            if (binary_return_label < skip) {
                forth_code[forth_code_ipc++] = primitive_efelseif;
                break;
            }

            if(present->em == EFDEFUN){
                forth_code[forth_code_ipc++] = primitive_efdefuntail;  /*if  go this way, efendif fix_code_address wouldn't be  nessary,but keep it also harmless*/
                break;
            }
            else if(present->em == EVPROGNEND){
                skip++;
            }
            else if(present->em == EFENDIF){
                skip += 2;
            }
            else{
                cross_printf("tail elements never found, do it normal way \r\n ");
                forth_code[forth_code_ipc++] = primitive_efelseif;
                break;
            }
        }
    }
    else {
        forth_code[forth_code_ipc++] = primitive_efelseif;
    }

    local_recycle_contain[local_recycle_contain_ipc++] = present = int_type (forth_code_ipc++);

    batch_write_circuit(present,
                        binary_return_array + binary_return_label++);
    batch_write_circuit(primitive_efendif,
                        binary_return_array + binary_return_label++);
    goto label;
 efdefun: 
    /*modify  by  rosslyn ,for milestone, can slide down to defunend now,2014.6.21*/
    forth_code[forth_code_ipc++] = primitive_efdefunend;
    goto popjreturn;
 efquotex: 
    forth_code[forth_code_ipc++] = primitive_efquotexend;
    goto popjreturn;
 efdefmacro: 
    forth_code[forth_code_ipc++] = primitive_efdefmacroend;
    forth_code[forth_code_ipc++] = primitive_efdefunend;
    goto popjreturn;
 eflambda: 
    fix_code_address(
                     batch_read_circuit(binary_return_array + --binary_return_label),
                     forth_code_ipc, is_local);
    forth_code[forth_code_ipc++] = primitive_eflambdaend; /*think  why?*/
    goto popjreturn;
    return primitive_empty;
}

void * analyse_forth(void *forth_code[], long begin, long end){
    long i = 0;
    char debug_inf[256] = "\0";
    type *right = NULL;

    for (i = begin; i < end; i++) {
        right = (type*)forth_code[i];
        if (!right ) {
            cross_sprintf(debug_inf, "forth_code  null occurs, offset is %d, please check \r\n ", (int)i);
            return new_debug(debug_inf, cross_strlen(debug_inf));
        }
        if (right->em == DEBUG) {
            return right;
        }
    }
    return primitive_empty;
}

void * localeval(void * _left, void ** _env){
    type * status = NULL;
    long  begin = local_forth_code_ipc;
    eval_impl(_left, _env, local_forth_code, &local_forth_code_ipc, 1);
    status = analyse_forth(local_forth_code, begin, local_forth_code_ipc); 
    if(status->em == DEBUG){
        return status;
    }
    else{
        return execution(_env, local_forth_code, begin, local_forth_code_ipc, NULL);    
    }
}

void * globaleval(void * _left, void ** _env){
    type * status = NULL;
    long  begin = global_forth_code_ipc;
    eval_impl(_left, _env, global_forth_code, &global_forth_code_ipc, 0);
    status = analyse_forth(global_forth_code, begin, global_forth_code_ipc); 
    if(status->em == DEBUG){
        return status;
    }
    else{
        return execution_strategy(_env, global_forth_code, begin, global_forth_code_ipc);    
    }
}

int calc_length(void * _left){
    type * left = (type*)_left;
    return left->obj_length;
}

void * check_fun_parameters(type *right, int now_num){
    type * outcome = primitive_empty;
    int signx = 0;

    signx = 0;
    while (1) {
        if (!strcmp("", normal_fun[signx].name)) {
            break;
        }
        if(normal_fun[signx].address == right->u_data.f_data){
            if(normal_fun[signx].parameters_num == -1 || now_num == normal_fun[signx].parameters_num){
                return outcome;
            }
            else{
                return NULL;
            }
        }
        signx++;
    }
    return  NULL;
}

void * c_defun(void * name, void * arg, void * expr, void ** mem) {
    cross_printf("defun");
    *mem = c_cons(c_list(name, arg, expr, 0), *mem);
    return name;
}


#define   analyse_procedure(name) {    present = c_cdr(left);     if (present->em == EMPTY) {          cross_sprintf(debug_inf, "%s FORMAT ERROR\r\n", name);         return new_debug(debug_inf, cross_strlen(debug_inf));     }      left = c_cadr(left);     if(left->em != VAR){          cross_sprintf(debug_inf, "%s should be VAR\r\n", name);         return new_debug(debug_inf, cross_strlen(debug_inf));     }      return primitive_empty;}


void * analyse_para(void * _left, void ** _env);
void * analyse(void * _left, void ** _env)
{
    type * mid_x;
    type * present, * right, * presentarg, * outcome;
    type * left = _left;
    type * head = NULL;
    char debug_inf[1024] = "\0";
    if(left->em == EMPTY)
        return primitive_empty;
    else if(left->em == VAR || left->em == STORAGE) {
        return primitive_empty;
    }
    else if (left->em == INTEGER && left->u_data.i_data == NULLVALUE)
        return primitive_empty;
    else if (left->em == INTEGER)
        return primitive_empty;
    if(left->em != LIST){
        if(left->em == FUN){
            modify_fun_content(left);
            cross_sprintf(debug_inf, "s-express %s should be a list, but it's an original fun \r\n", left->u_data.s_data);
        }
        else if(left->em == VAR){
            cross_sprintf(debug_inf, "s-express %s  should be a list, but it's an VAR \r\n", left->u_data.s_data);
        }
        else{}
        return new_debug(debug_inf, cross_strlen(debug_inf));
    } 
    left_print(left);
    head = c_car (left );
    switch (head->em) {
        case FUNCALL:
            present = c_cdr(left);
            if (present->em == EMPTY) {
                cross_strcpy(debug_inf, "FUNCALL shoule follows a statement\r\n");
                return new_debug(debug_inf, cross_strlen(debug_inf));
            }

            present = c_cadr(left);
            if ((present->em != LIST) && (present->em != VAR)) {
                cross_strcpy(debug_inf, "FUNCALL fist value should be a VAR or statement \r\n");
                return new_debug(debug_inf, cross_strlen(debug_inf));
            }

            present = analyse(c_cadr (left ), _env);
            if(present->em == DEBUG){
                cross_sprintf(debug_inf, "FUNCALL:%s", present->u_data.a_storage + sizeof(int));
                gc_atom(present);
                outcome = new_debug(debug_inf, cross_strlen(debug_inf));
            }
            else{
                outcome = analyse_para( c_cddr (left ), _env );
            }
            return outcome;
            break;
        case SETQ:
            left = c_cdr (left);
            if (left->em == EMPTY) {
                cross_strcpy(debug_inf, "SETQ shoule follows a statement\r\n");
                return new_debug(debug_inf, cross_strlen(debug_inf));
            }
            present = c_car (left);
            if(present->em != VAR){
                cross_strcpy(debug_inf, "setq first value should be VAR\r\n");
                outcome = new_debug(debug_inf, cross_strlen(debug_inf));
            } 
            else{
                present = c_cdr (left);
                if(present->em != LIST){
                    cross_strcpy(debug_inf, "setq second value should exist\r\n");
                    outcome = new_debug(debug_inf, cross_strlen(debug_inf));
                }
                else {
                    outcome = analyse(c_car(present), _env);
                }
            }
            break;
        case DEFINE:
            present = c_cdr (left);
            if (present->em == EMPTY) {
                cross_strcpy(debug_inf, "DEFINE shoule follows a statement\r\n");
                return new_debug(debug_inf, cross_strlen(debug_inf));
            }
            left = c_car (present );
            if(left->em != VAR){
                cross_strcpy(debug_inf, "define first value should be VAR\r\n");
                outcome = new_debug(debug_inf, cross_strlen(debug_inf));
            } 
            else{
                present = c_cdr (present);
                if(present->em != LIST){
                    cross_strcpy(debug_inf, "define second value should exist\r\n");
                    outcome = new_debug(debug_inf, cross_strlen(debug_inf));
                }
                else {
                    outcome = analyse(c_car(present), _env);
                }
            }
            break;
        case EOFSTDIN:
        case STDIN:
        case STOP:
        case ISSTOP:
        case NEXT:
        case YIELD:
        case SELF:
            outcome = primitive_empty;
            break;
        case IF:
            present = c_cdr(left);
            if (present->em == EMPTY) {
                cross_strcpy(debug_inf, "IF shoule have prediction statement\r\n");
                return new_debug(debug_inf, cross_strlen(debug_inf));
            }
            present = analyse ( c_cadr ( left ) , _env );
            if (present->em == DEBUG) {
                cross_sprintf(debug_inf, "IF-CHOICE:%s", present->u_data.a_storage + sizeof(int));
                gc_atom(present);
                return new_debug(debug_inf, cross_strlen(debug_inf));
            }
            present = c_cddr(left);
            if (present->em == EMPTY) {
                cross_strcpy(debug_inf, "IF shoule have then statement");
                return new_debug(debug_inf, cross_strlen(debug_inf));
            }
            present = c_cdr(present);
            if(present->em != EMPTY){      /* if  statement may not have else condition.*/
                present = analyse (c_car(present ) , _env );
                if(present->em == DEBUG){
                    cross_sprintf(debug_inf, "IF-LAST:%s", present->u_data.a_storage + sizeof(int));
                    gc_atom(present);
                    return new_debug(debug_inf, cross_strlen(debug_inf));
                }
            }
            present = analyse ( c_caddr(left) , _env );
            if(present->em == DEBUG){
                cross_sprintf(debug_inf, "IF-FIRST:%s", present->u_data.a_storage + sizeof(int));
                gc_atom(present);
                return new_debug(debug_inf, cross_strlen(debug_inf));
            }
            return primitive_empty;
            break;
        case PROGN:
            left = c_cdr(left);
            if (left->em == EMPTY) {
                cross_strcpy(debug_inf, "PROGN shoule have at least one statement\r\n");
                return new_debug(debug_inf, cross_strlen(debug_inf));
            }

            while ((( type * )c_cdr (left))->em != EMPTY){
                present = * _env;
                cross_printf("\r\n");
                left_print(c_car(left));
                right = analyse(c_car(left) , (void**)&present);
                if(right->em == DEBUG ){
                    cross_sprintf(debug_inf, "PROGN-MID:%s", right->u_data.a_storage + sizeof(int));
                    gc_atom(right);
                    return new_debug(debug_inf, cross_strlen(debug_inf));
                }
                left = c_cdr(left);
            }
            left = c_car(left);
            present = analyse(left, _env);
            if(present->em == DEBUG){
                cross_sprintf(debug_inf, "PROGN-LAST:%s", present->u_data.a_storage + sizeof(int));
                gc_atom(present);
                return new_debug(debug_inf, cross_strlen(debug_inf));
            }
            return primitive_empty;
            break;
        case EJECT:
            analyse_procedure("EJECT");
            break;
        case WHOLE:
            analyse_procedure("WHOLE");
            break;
        case EXCHANGE:
            analyse_procedure("EXCHANGE");
            break;
        case COMBI:
            analyse_procedure("COMBI");
            break;
        case TOP:
            analyse_procedure("TOP");
            break;
        case APPEND:
            present = c_cdr(left);
            if (present->em == EMPTY) {
                cross_strcpy(debug_inf, "APPEND shoule follows a statement\r\n");
                return new_debug(debug_inf, cross_strlen(debug_inf));
            }

            present = c_cadr(left);
            if(present->em != VAR){
                cross_strcpy(debug_inf, "append should be VAR\r\n");
                return new_debug(debug_inf, cross_strlen(debug_inf));
            } 
            right = c_caddr(left);
            return analyse(right , _env);
            break;
        case PRESS:
            present = c_cdr(left);
            if (present->em == EMPTY) {
                cross_strcpy(debug_inf, "PRESS shoule follows a statement\r\n");
                return new_debug(debug_inf, cross_strlen(debug_inf));
            }

            present = c_cadr(left);
            if(present->em != VAR){
                cross_strcpy(debug_inf, "press should be VAR\r\n");
                return new_debug(debug_inf, cross_strlen(debug_inf));
            } 
            right = c_caddr(left);
            return analyse(right , _env);
            break;
        case FUN:
            present = c_cdr(left);
            if(present->em == EMPTY){
                present = check_fun_parameters(head, 0);
            }
            else{
                present = check_fun_parameters(head, present->obj_length);
            }

            if(present != NULL){
                return  analyse_para(c_cdr (left ), _env);
            }
            else{
                modify_fun_content(head);
                cross_sprintf(debug_inf, "%s original fun should have the same parameter num\r\n", head->u_data.s_data);
                return new_debug(debug_inf, cross_strlen(debug_inf));
            }
            break;
        case LAMBDA:
            present = c_cdr(left);
            if (present->em == EMPTY) {
                cross_strcpy(debug_inf, "lambda shoule have parameter list\r\n");
                return new_debug(debug_inf, cross_strlen(debug_inf));
            }
            mid_x = c_car(present);
            if (mid_x->em != LIST) {
                cross_strcpy(debug_inf, "lambda first value shoule be parameter list\r\n");
                return new_debug(debug_inf, cross_strlen(debug_inf));
            }
            mid_x = c_cdr(present);
            if (mid_x->em == EMPTY) {
                cross_strcpy(debug_inf, "lambda shoule have statement implement\r\n");
                return new_debug(debug_inf, cross_strlen(debug_inf));
            }
            return  analyse_para (c_cdr(present ) , _env);
            break;
        case DEFMACRO:
        case DEFUN:
            present = c_cdr(left);
            if (present->em == EMPTY) {
                cross_strcpy(debug_inf, "DEFUN/DEFMACRO shoule follows a statement\r\n");
                return new_debug(debug_inf, cross_strlen(debug_inf));
            }
            mid_x = c_car(present);
            if (mid_x->em != VAR) {
                cross_strcpy(debug_inf, "DEFUN/DEFMACRO shoule follows VAR as function name\r\n");
                return new_debug(debug_inf, cross_strlen(debug_inf));
            }

            mid_x = c_cdr(present);
            if (mid_x->em == EMPTY) {
                cross_strcpy(debug_inf, "DEFUN/DEFMACRO shoule have parameter list\r\n");
                return new_debug(debug_inf, cross_strlen(debug_inf));
            }

            mid_x = c_car(mid_x);
            if (mid_x->em != LIST) {
                cross_strcpy(debug_inf, "DEFUN/DEFMACRO shoule be a parameter list\r\n");
                return new_debug(debug_inf, cross_strlen(debug_inf));
            }

            mid_x = c_cddr(present);
            if (mid_x->em == EMPTY) {
                cross_strcpy(debug_inf, "DEFUN/DEFMACRO shoule have statement implement\r\n");
                return new_debug(debug_inf, cross_strlen(debug_inf));
            }

            return  analyse_para ( c_cddr (present ) , _env );
            break;
        case VAR:
            if((c_find_defun_arg(head, (void ** )&presentarg) && presentarg)){
                if(calc_length(presentarg) != calc_length(c_cdr(left))){
                    cross_sprintf(debug_inf, "\r\n%s user fun should have the same parameter num\r\n", head->u_data.s_data);
                    return new_debug(debug_inf, cross_strlen(debug_inf));
                }
                else{
                    return analyse_para ( c_cdr(left) , _env);
                }
            }
            else if (fetch_code_skip_address(head, EFDEFMACRO)) {
            } 
            else{
                 cross_printf("\r\n%s should be a function name, maybe error, leave for link detect \r\n", head->u_data.s_data);
                 return primitive_empty;
            }
            return primitive_empty;
            break;
        case QUOTE:
            present = c_cdr(left);
            if (present->em == EMPTY) {
                cross_strcpy(debug_inf, "QUOTE shoule follows a statement\r\n");
                return new_debug(debug_inf, cross_strlen(debug_inf));
            }
            return primitive_empty;
            break;
        case LIST:
            cross_strcpy(debug_inf, "list can't begin with LIST, that's scheme style, not be supported\r\n");
            return new_debug(debug_inf, cross_strlen(debug_inf));
            break;
        case INTEGER:
            cross_strcpy(debug_inf, "list can't begin with INTEGER\r\n");
            return new_debug(debug_inf, cross_strlen(debug_inf));
            break;
        default:
            cross_strcpy(debug_inf, "list begin with abnormal symbol\r\n");
            return new_debug(debug_inf, cross_strlen(debug_inf));
            break;
        }
    return outcome;
}


void * pcreate_worker(void * _left) {
    type * left = primitive_empty;
    type * outcome = primitive_empty;
    type * worker = primitive_empty;
    type * para = primitive_empty;

    int forth_find;

    type *m_env = primitive_empty;
    void **_env = (void**) &m_env;

    left = c_car(_left);
    worker = c_cadr(_left);
    if(worker->em == FUN){
        outcome = worker->u_data.f_data(c_copy_type(c_cddr(_left)));
    }
    else{
        forth_find = (long)iterate_address(worker);
        if (forth_find != 0){
            para = global_forth_code[forth_find + 1];
            _env = c_bindvars(para, c_copy_type(c_cddr(_left)), _env);   /*different  from below ,think why?*/
            outcome =  execution(_env, global_forth_code, forth_find + 2, global_forth_code_ipc, NULL);
            c_unbindvars(_env);
        }
    }      

    forth_find = (long)iterate_address(left);
    if (forth_find != 0){
        para = global_forth_code[forth_find + 1];
        _env = c_bindvars(para, c_cons(outcome, primitive_empty), _env);
        outcome =  execution(_env, global_forth_code, forth_find + 2, global_forth_code_ipc, NULL);
        c_unbindvars(_env);
    }

    gc(_left);
    return outcome ;  /*modify pjoin mean,so there need change according.  2014.5.31*/
}

typedef void  (*thread_worker)(void * _left);

void compare(void * _left, void * _right) {
    type * left = _left, *right = _right, *present;
    if (!(left - right))
        return;
    else {
        c_unbindvar(c_car (left));
        /*aux function below*/
        gc_frame(c_car (left));
        present = c_cdr(left);
        gc_atom(left);
        compare(present, right);
    }

}

void * analyse_para(void * _left, void ** _env) {
    type * present, *env = *_env;
    type * left = _left;
    char debug_inf[1024] = "\0";

    if (left->em == EMPTY){
        return primitive_empty;
    }
    else {
        present = analyse(c_car (left), (void **) &env);
        compare(env, *_env); /*important is  Tail-Recursion*/
        if(present->em == DEBUG){
            cross_sprintf(debug_inf, "analyse_para:%s", present->u_data.a_storage + sizeof(int));
            gc_atom(present);
            return new_debug(debug_inf, cross_strlen(debug_inf));
        }
        return analyse_para(c_cdr (left), _env);
    }
}

 int number; /*  if NUMBER: numerical value  */
 char symbol[20];
 char alpha_ex[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_!.:LC#=&?*$/@";

int isalpha_ex(char test) {
    int i = 0;
    for (i = 0; alpha_ex[i] != '\0'; i++)
        if (alpha_ex[i] == test)
            return 1;
    return 0;
}

char * scan(char * buf, enum tokens* localtoken)
{
    static char * bp;
    int sign = 0;
    enum tokens token; 
    memset(symbol, 0, sizeof(symbol));

    if (buf)
        bp = buf;

    while (isspace(*bp & 0xff))
        ++bp;
    if (isdigit(*bp & 0xff)) {
        token = NUMBER;
        number = cross_atoi(bp);
        skip_number(bp, &bp);
    } 
    else if (isalpha_ex(*bp & 0xff) || *bp == '.') {
        token = NAME;
        while (isalpha_ex(*bp & 0xff))
            symbol[sign++] = *bp++;
    } 
    else {
        token = VALUE;
        symbol[sign++] = *bp ? *bp++ : 0;
    }
    *localtoken = token;
    return bp;
}

void * fun_type(void * name) {
    type * outcome = new_object();
    outcome->em = FUN;
    outcome->u_data.f_data = lookup_primitive(name, global_primitive);
    if (outcome->u_data.f_data == NULL) {
        gc(outcome);
        return NULL;
    }
    /*  cross_strcpy(outcome->u_data.s_data, name);  //if  add would be a proce,see type defination */
    return outcome;
}

native_operator is_operate_type(void * _name) {
    char * name = _name;
    int sign = 0;
    while (1) {
        if (!strcmp("", original_type[sign].name)) {
            return (native_operator) NULL;
        } else if (!strcmp(name, original_type[sign].name)) {
            return original_type[sign].type;
            break;
        } else
            sign++;
    }
}

int  is_quote = 0;

 void * factor(char **_now) {
    type * outcome;
    type * ele_left;
    type * ele_right;
    char debug_inf[256] = "\0";
    native_operator type;
    enum tokens token;
    char * file_token = NULL;
    char * response = NULL;
    char * responsex = NULL;
    size_t storage_size = 0;
    int pattern = 1;

    *_now = scan(*_now, &token);
    switch (token) {
    case NAME:
        if ((is_quote == 0) && (outcome = fun_type(symbol))) {
            return outcome;
        } else if ((is_quote == 0) && (type = is_operate_type(symbol))) {
            return operate_type(type);
        } else if (!strcmp("nil", symbol)) {
            return primitive_empty;
        } else if (!strcmp("LINE", symbol)) {
            return var_type("\r\n");
        } else if (!strcmp("ENTER", symbol)) {
            return var_type("\n");
        } else if (!strcmp("SPACE", symbol)) {
            return var_type(" ");
        } else if (!strcmp("t", symbol)) {
            return int_type(1);
        } 
        else {
            return var_type(symbol);
        }
    case NUMBER:
        return int_type(number);
        break;
    case VALUE:
        switch (symbol[0]) {
        case '(':
            ele_left = factor(_now);
            if (!ele_left) {
                return c_cons(primitive_empty, primitive_empty);
            }
            else if(ele_left->em == DEBUG) return ele_left;
            else if(ele_left->em == VAR && !strcmp(ele_left->u_data.s_data, "comment")){
                gc(ele_left);
                while (1) {
                    ele_right = factor(_now);
                    if (!ele_right) break;
                    if(ele_right->em == DEBUG) {
                        return ele_right;
                    }
                    left_print(ele_right);
                    gc(ele_right); /* comment memory recycled*/
                }
                return factor(_now);
            }
            else if(ele_left->em == VAR && !strcmp(ele_left->u_data.s_data, "storage")){
                while (isspace(**_now & 0xff))
                    *_now += 1; 
                pattern = 1;
                file_token = *_now; 
                while(1){
                    if(*file_token == '(') {pattern++;} 
                    else if(*file_token == ')') {pattern--;} 
                    else{}
                    if(0 == pattern)break;
                    file_token += 1; 
                }
                if (file_token == *_now) {
                    gc(ele_left);
                    cross_strcpy(debug_inf, "storage material is empty\r\n");
                    return new_debug(debug_inf, cross_strlen(debug_inf));
                }
                response = (char*) cross_calloc(BUF_SIZE, sizeof(char));
                cross_memcpy(response, *_now, file_token - *_now);
                ele_left->em = STORAGE;

                storage_size = cross_strlen(response) + 1 + sizeof(int);
                responsex = (char*)cross_calloc(storage_size, sizeof(char));
                cross_memcpy((responsex + sizeof(int)), response, storage_size - sizeof(int) - 1);
                *(int*)responsex = storage_size - sizeof(int);
                ele_left->u_data.a_storage = responsex;
                cross_printf("storage is  %s  \r\n  ", responsex + sizeof(int));
                cross_free(response);

                *_now = file_token + 1; 
                return ele_left;
            }
            else{
                ele_left = c_cons(ele_left, primitive_empty);

                while (1) {
                    ele_right = factor(_now);
                    if (ele_right) {
                        if(ele_right->em == DEBUG) {
                            gc(ele_left);
                            return ele_right;
                        }
                        ele_left = c_appdix(ele_left, c_cons(ele_right, primitive_empty));
                    } 
                    else {
                        break;
                    }

                }
                return ele_left;
            }
            break;
        case '$':
            return closure_type();
            break;
        case ')':
            return NULL;
            break;
        case '\'':
            if ( is_quote != 0) {
                is_quote++;
                ele_right = factor(_now);
                if( ele_right == NULL) {
                    is_quote--;
                    cross_strcpy(debug_inf, "' shoule follow with symbol\r\n");
                    return new_debug(debug_inf, cross_strlen(debug_inf));
                }
                if(ele_right->em == DEBUG) {
                    is_quote--;
                    return ele_right;
                }
                outcome = c_list(var_type("quote"), ele_right, 0);
            }
            else {
                is_quote++;
                ele_right = factor(_now);
                if( ele_right == NULL) {
                    is_quote--;
                    cross_strcpy(debug_inf, "' shoule follow with symbol\r\n");
                    return new_debug(debug_inf, cross_strlen(debug_inf));
                }
                if(ele_right->em == DEBUG) {
                    is_quote--;
                    return ele_right;
                }
                outcome = c_list(operate_type(QUOTE), ele_right, 0);
            }
            is_quote--;
            return outcome;
        case '`':
            ele_right = factor(_now);
            if( ele_right == NULL) {
                cross_strcpy(debug_inf, "` shoule follow with symbol\r\n");
                return new_debug(debug_inf, cross_strlen(debug_inf));
            }
            if(ele_right->em == DEBUG) return ele_right;
            return c_list(operate_type(QUOTEX), ele_right, 0);
        case ',':
            ele_right = factor(_now);
            if( ele_right == NULL) {
                cross_strcpy(debug_inf, ", shoule follow with symbol\r\n");
                return new_debug(debug_inf, cross_strlen(debug_inf));
            }
            if(ele_right->em == DEBUG) return ele_right;
            return c_list(operate_type(EVAL), ele_right, 0);  /*modify 2014.6.30, for macro forth */
        default:
            if( symbol[0] == 0) {
                cross_strcpy(debug_inf, "shoule end with ), you can type (help) for help");
                return new_debug(debug_inf, cross_strlen(debug_inf));
            }
            cross_sprintf(debug_inf, "%s error fun symbol\r\n", symbol);
            return new_debug(debug_inf, cross_strlen(debug_inf));
            break;
        }
    default:
        cross_strcpy(debug_inf, "unexpect character");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }
    return NULL;
}

void init_raw_var(void) {
    void * raw = NULL;
    raw_type *raw_left;

    raw = raw_new_object();
    raw_left = (raw_type*) (*(long*)( (char *)raw - sizeof(long)) );
    raw_mem_manager_reserved = raw_primitive_reserved = raw_left->mother;

    raw = raw_new_object();
    raw_left = (raw_type*) (*(long*)( (char *)raw - sizeof(long)) );
    raw_mem_manager_used = raw_primitive_used = raw_left->mother;
}


void init_object(void) {
    int i = 0;

    /*follow code is special too. use calloc instead of cross_calloc*/
    mem_manager_unused = (wraptype *) cross_calloc(sizeof(wraptype), global_count);

    for (i = 0; i < global_count - 1; i++) {
        mem_manager_unused[i].mem_next = &mem_manager_unused[i + 1];
    }
    mem_manager_unused[global_count - 1].mem_next = NULL;

#ifdef _WIN32
    init_socket();
#endif

    /*
    time_t now;  
    int unixtime = time(&now);  
    cross_printf("initrand unixtime is %d\r\n", unixtime);
    srand(unixtime);
    */

#ifdef FLEXHASH
    global_frame = init_flexhash(7, 10, ARABIC, sizeof(entry), NULL, NULL);
    global_stack = init_flexhash(7, 10, ARABIC, sizeof(entry), NULL, NULL);
    global_stream = init_flexhash(7, 10, ARABIC, sizeof(entry), NULL, NULL);
    global_lambda = init_flexhash(7, 10, ARABIC, sizeof(entry), NULL, NULL);
    global_defun = init_flexhash(13, 10, ARABIC, sizeof(entry), NULL, NULL);
    global_var = init_flexhash(13, 10, ARABIC, sizeof(entry), NULL, NULL);
    global_define = init_flexhash(7, 10, ARABIC, sizeof(entry), NULL, NULL);
    global_primitive = init_flexhash(7, 10, ARABIC, sizeof(entry), NULL, NULL);
#endif

    binary_return_array = (void**)cross_calloc(sizeof(void*), FORTH_WORD);
    binary_data_array = (void**)cross_calloc(sizeof(void*), FORTH_WORD);

    global_forth_code = (void**)cross_calloc(sizeof(void*), FORTH_WORD);
    local_forth_code = (void**)cross_calloc(sizeof(void*), FORTH_WORD);
    forth_code_skip = (void**)cross_calloc(sizeof(void*), FORTH_WORD);
    local_recycle_contain = (void**)cross_calloc(sizeof(void*), FORTH_WORD);
    forth_code_unfix = (void**)cross_calloc(sizeof(void*), FORTH_WORD);
    forth_macro_skip = (void**)cross_calloc(sizeof(void*), FORTH_WORD);
}


void init_raw_object(void) {
    int i = 0;
    raw_type * current = NULL;

    /*last one didn't used*/
    raw_mem_manager_unused = (raw_wraptype *) cross_calloc(sizeof(raw_wraptype), raw_global_count + 1);

    for (i = 0; i < raw_global_count; i++) {
        current =  &raw_mem_manager_unused[i].value;
#ifdef HURLEY
        current->content = (void*)kmalloc(BUF_SIZE * sizeof(char));
#endif

#if defined(LINUX) || defined(SMALLLINUX)
        current->content = (void*)malloc(BUF_SIZE * sizeof(char));
#endif

        raw_mem_manager_unused[i].mem_next = &raw_mem_manager_unused[i + 1];
    }

    raw_mem_manager_unused[raw_global_count - 1].mem_next = NULL;
}


void init_primitive_var(void) {
    type *left;
    int i = 0;

    for(i = 0; i <= AMOUNT; i++){
        primitive_small[i] = new_object();
        primitive_small[i]->em = INTEGER;
        primitive_small[i]->u_data.i_data = 0 - (AMOUNT - i);
        primitive_small[i]->ref_count = 100;
    }

    for(i = AMOUNT + 1; i < 2 * AMOUNT + 1 + 1; i++){
        primitive_small[i] = new_object();
        primitive_small[i]->em = INTEGER;
        primitive_small[i]->u_data.i_data = i - AMOUNT;
        primitive_small[i]->ref_count = 100;
    }

    left = new_object();
    mem_manager_reserved = primitive_reserved = left->mother;

    left = new_object();
    mem_manager_used = primitive_used = left->mother;

    primitive_empty = new_object();
    primitive_empty->em = EMPTY;
    primitive_empty->u_data.i_data = NULLVALUE;

    primitive_evif = new_object();
    primitive_evif->em = EVIF;

    primitive_evfuncall = new_object();
    primitive_evfuncall->em = EVFUNCALL;

    primitive_evfuncallmid = new_object();
    primitive_evfuncallmid->em = EVFUNCALLMID;

    primitive_evfuncallend = new_object();
    primitive_evfuncallend->em = EVFUNCALLEND;

    primitive_evmacro = new_object();
    primitive_evmacro->em = EVMACRO;

    primitive_evsetq = new_object();
    primitive_evsetq->em = EVSETQ;

    primitive_evdefine = new_object();
    primitive_evdefine->em = EVDEFINE;

    primitive_evargscombi = new_object();
    primitive_evargscombi->em = EVARGSCOMBI;

    primitive_evargs = new_object();
    primitive_evargs->em = EVARGS;

    primitive_evunbindvars = new_object();
    primitive_evunbindvars->em = EVUNBINDVARS;

    primitive_evprogn = new_object();
    primitive_evprogn->em = EVPROGN;

    primitive_evprognmid = new_object();
    primitive_evprognmid->em = EVPROGNMID;

    primitive_evprognend = new_object();
    primitive_evprognend->em = EVPROGNEND;

    primitive_evpress = new_object();
    primitive_evpress->em = EVPRESS;

    primitive_evappend = new_object();
    primitive_evappend->em = EVAPPEND;

    primitive_effun = new_object();
    primitive_effun->em = EFFUN;

    primitive_efyield = new_object();
    primitive_efyield->em = EFYIELD;

    primitive_efstop = new_object();
    primitive_efstop->em = EFSTOP;

    primitive_efnext = new_object();
    primitive_efnext->em = EFNEXT;

    primitive_efisstop = new_object();
    primitive_efisstop->em = EFISSTOP;

    primitive_efstdin = new_object();
    primitive_efstdin->em = EFSTDIN;

    primitive_efeofstdin = new_object();
    primitive_efeofstdin->em = EFEOFSTDIN;

    primitive_efdefun = new_object();
    primitive_efdefun->em = EFDEFUN;

    primitive_efdefunend = new_object();
    primitive_efdefunend->em = EFDEFUNEND;

    primitive_efdefuntail = new_object();
    primitive_efdefuntail->em = EFDEFUNTAIL;

    primitive_efdefmacro = new_object();
    primitive_efdefmacro->em = EFDEFMACRO;

    primitive_efdefmacroend = new_object();
    primitive_efdefmacroend->em = EFDEFMACROEND;

    primitive_efsapply = new_object();
    primitive_efsapply->em = EFSAPPLY;

    primitive_efsapplyx = new_object();
    primitive_efsapplyx->em = EFSAPPLYX;

    primitive_efsapplyxbegin = new_object();
    primitive_efsapplyxbegin->em = EFSAPPLYXBEGIN;

    primitive_efif = new_object();
    primitive_efif->em = EFIF;

    primitive_efelseif = new_object();
    primitive_efelseif->em = EFELSEIF;

    primitive_efendif = new_object();
    primitive_efendif->em = EFENDIF;

    primitive_efprognmid = new_object();
    primitive_efprognmid->em = EFPROGNMID;

    primitive_efquote = new_object();
    primitive_efquote->em = EFQUOTE;

    primitive_efquotex = new_object();
    primitive_efquotex->em = EFQUOTEX;

    primitive_efquotexend = new_object();
    primitive_efquotexend->em = EFQUOTEXEND;

    primitive_efsetq = new_object();
    primitive_efsetq->em = EFSETQ;

    primitive_efdefine = new_object();
    primitive_efdefine->em = EFDEFINE;

    primitive_eflambda = new_object();
    primitive_eflambda->em = EFLAMBDA;

    primitive_effuncall = new_object();
    primitive_effuncall->em = EFFUNCALL;

    primitive_efself = new_object();
    primitive_efself->em = EFSELF;

    primitive_eflambdaend = new_object();
    primitive_eflambdaend->em = EFLAMBDAEND;

    primitive_effuncallend = new_object();
    primitive_effuncallend->em = EFFUNCALLEND;

    primitive_efeject = new_object();
    primitive_efeject->em = EFEJECT;

    primitive_eftop = new_object();
    primitive_eftop->em = EFTOP;

    primitive_efcombi = new_object();
    primitive_efcombi->em = EFCOMBI;

    primitive_efexchange = new_object();
    primitive_efexchange->em = EFEXCHANGE;

    primitive_efwhole = new_object();
    primitive_efwhole->em = EFWHOLE;

    primitive_efappend = new_object();
    primitive_efappend->em = EFAPPEND;

    primitive_efpress = new_object();
    primitive_efpress->em = EFPRESS;

#ifdef LINUX
    primitive_zfsapply = new_object();
    primitive_zfsapply->em = ZFSAPPLY;

    primitive_zfcolon = new_object();
    primitive_zfcolon->em = ZFCOLON;

    primitive_zfend = new_object();
    primitive_zfend->em = ZFEND;

    primitive_zfif = new_object();
    primitive_zfif->em = ZFIF;

    primitive_zfelseif = new_object();
    primitive_zfelseif->em = ZFELSEIF;

    primitive_zfthen = new_object();
    primitive_zfthen->em = ZFTHEN;
#endif

    primitive_eflistbegin = new_object();
    primitive_eflistbegin->em = EFLISTBEGIN;

    primitive_eflistend = new_object();
    primitive_eflistend->em = EFLISTEND;

}

void * handle_material(char *buf) {
    int circle = 1;
    type * ele_left;
    type * ele_right, * present = primitive_empty;
    type * ele_property = primitive_empty;
    type * outcome = primitive_empty;
    enum tokens token;
    char *now = NULL;
    char debug_inf[256] = "\0";

    now = scan(buf, &token);
    while ((token == VALUE) && (symbol[0] == '(')) {
        ele_left = factor(&now);
        if(ele_left->em == DEBUG) return ele_left;
        ele_left = c_cons(ele_left, primitive_empty);
        while (1) {
            ele_right = factor(&now);
            if (ele_right) {
                if(ele_right->em == DEBUG) {
                    gc(ele_left);
                    return ele_right;
                }
                ele_left = c_appdix(ele_left, c_cons(ele_right, primitive_empty));
            }
            else {
                left_print(ele_left);
                ele_property = c_appdix(ele_property, c_cons(ele_left, primitive_empty));
                break;
            }
        }
        now = scan(now, &token);
        circle += 1;
    }

    cross_printf("circle is %d \r\n", circle);
    if (1 == circle) {
        cross_strcpy(debug_inf, "shoule begin with (, you can type (help) for help\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }

    while (1) {
        ele_left = c_car(ele_property);
        /*
          ele_left = c_list ( var_type("mymachine"), c_list (ele_left, 0), 0);
        */
        gc(outcome);
        present = analyse(ele_left, (void **) &foundation_env);
        if(present->em != DEBUG){
            outcome = left_print(globaleval(ele_left, (void **) &foundation_env));
        }
        else{
            cross_read("stdout_file", &circle, buf);
            outcome = present;
        }
        cross_printf("\r\n");
        ele_property = c_cdr(ele_property);
        if (ele_property->em == EMPTY)
            break;
    }
    return outcome;
}

void * handle_materialjit(char *buf, int head) {
    int circle = 1;
    type * local_env = primitive_empty;
    type * ele_left;
    type * ele_right;
    type * present = primitive_empty;
    type * ele_property = primitive_empty;
    type * outcome = primitive_empty;
    long  begin_code = local_forth_code_ipc;
    long  begin_contain = local_recycle_contain_ipc;
    long  begin_skip = forth_code_skip_ipc;
    long  begin_unfix = forth_code_unfix_ipc;

    enum tokens token;
    char *now = NULL;
    char debug_inf[256] = "\0";

    now = scan(buf, &token);
    while ((token == VALUE) && (symbol[0] == '(')) {
        ele_left = factor(&now);
        if(ele_left->em == DEBUG) return ele_left;
        ele_left = c_cons(ele_left, primitive_empty);
        while (1) {
            ele_right = factor(&now);
            if (ele_right) {
                if(ele_right->em == DEBUG) {
                    gc(ele_left);
                    return ele_right;
                }
                ele_left = c_appdix(ele_left, c_cons(ele_right, primitive_empty));
            }
            else {
                left_print(ele_left);
                if (head != 0) {
                    if (head == circle) {
                        ele_property = c_appdix(ele_property, c_cons(ele_left, primitive_empty));
                        goto out;
                    } 
                    else {
                        gc(ele_left); /*add  by  rosslyn,2013.8.24  22:55*/
                    }
                } 
                else {
                    ele_property = c_appdix(ele_property, c_cons(ele_left, primitive_empty));
                }
                break;
            }
        }
        now = scan(now, &token);
        circle += 1;
    }

    if (1 == circle) {
        cross_strcpy(debug_inf, "shoule begin with (, you can type (help) for help\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf)); 
    }

 out: 
    ele_right = ele_property;
    while (1) {
        ele_left = c_car(ele_property);
        gc(outcome);
        present = analyse(ele_left, (void **) &local_env);
        if(present->em != DEBUG){
            outcome = localeval(ele_left, (void **) &local_env);
        }
        else{
            outcome = present;
        }

        ele_property = c_cdr(ele_property);
        if (ele_property->em == EMPTY)
            break;
    }

    gc(ele_right);
    gc(local_env);
    for(circle = begin_contain; circle < local_recycle_contain_ipc; circle++){
        gc(local_recycle_contain[circle]);
    } 

    memset(local_forth_code, 0, local_forth_code_ipc * sizeof(void*));

    local_forth_code_ipc = begin_code;
    local_recycle_contain_ipc = begin_contain;
    forth_code_skip_ipc = begin_skip;
    forth_code_unfix_ipc = begin_unfix;
    return outcome;
}

int main_pivot(int argc, char ** argv) {
    int sign;
    type * ele_left;
    char* buf = NULL;
    char* buf_another = NULL;
    char newline[1024];

    init_raw_object();
    init_raw_var();

    init_object();
    init_primitive_var();
    init_primitive();

    buf = (char*) cross_calloc(BUF_SIZE, sizeof(char));
    buf_another = (char*) cross_calloc(BUF_SIZE, sizeof(char));

    foundation_env = primitive_empty;
    /*advance  high-tech*/
    /*
    ele_left = c_list(operate_type(DEFMACRO), var_type("demo"),
                      c_list(var_type("expr"), 0),
                      c_list(fun_type("print"), var_type("expr"), 0), 0);
    globaleval(ele_left, (void **) &foundation_env);

    ele_left = c_list(operate_type(DEFMACRO), var_type("mymachine"),
                      c_list(var_type("exprs"), 0),
                      c_list(operate_type(QUOTEX),
                             c_list(operate_type(IF), c_list(operate_type(EVAL),
                                                                c_list(fun_type("eq"), var_type("exprs"),
                                                                       primitive_empty, 0), 0), primitive_empty,
                                    c_list(operate_type(PROGN),
                                           c_list(fun_type("print"),
                                                  c_list(var_type("demo"),
                                                         c_list(operate_type(EVAL),
                                                                c_list(
                                                                       fun_type(
                                                                                       "car"),
                                                                       var_type("exprs"),
                                                                       0), 0), 0), 0),
                                           c_list(var_type("mymachine"),
                                                  c_list(operate_type(EVAL),
                                                         c_list(fun_type("cdr"),
                                                                var_type("exprs"), 0), 0),0),
                                           0), 0), 0), 0);

    globaleval(ele_left, (void **) &foundation_env);
    */

    sign = 0;
    if (argc == 1) {
#if defined(SMALLLINUX) || defined(HURLEY)
                 gc(left_print(handle_materialjit("(progn (defun haha(aaa)(progn (print aaa) (haha (add aaa 10))))  (haha 1000)) ", 0)));   /* jit means less memory leak */
                 //                  gc(left_print(handle_materialjit("(progn (defun fibs(aaa)(if (eq aaa  1) 1 (if (eq aaa 2) 2 (print (add (fibs (minus aaa 1)) (fibs (minus aaa 2))))))) (fibs 100)) ", 0))); /* jit means less memory leak */
#endif
#if defined(LINUX)
        gc(original_display(primitive_empty));
        cross_printf("\r\n WELCOME!!\r\n");
        cross_printf("arabic>");
        while (fgets(newline, 1024, stdin) != NULL) {
            gc(left_print(handle_materialjit(newline, 0))); /* jit means less memory leak */
            gc(original_display(primitive_empty));
            cross_printf("\r\narabic>");
        }
#endif
    } 
    else {
        if(cross_strstr(argv[1], ".elf")){
            /*milestone vm machine.*/
            char file_tmp_name[256] = "";
            cross_sprintf(file_tmp_name, "%s", argv[1]);
#ifdef LINUX
            read_forth_code(file_tmp_name);
#endif
            execution((void **)&foundation_env, global_forth_code, 0, global_forth_code_ipc, NULL);      
        }
        else{
#ifdef LINUX
            init_indicator();
            cross_read(argv[1], &sign, buf);
            if(cross_strstr(argv[1], ".elc")) {
                cross_strcpy(global_filename, argv[1]);
                global_filename[ cross_strlen(global_filename) - 1] = '\0'; 
                huffman_decryption((unsigned char  *)buf, sign);
                release_indicator();
                gc(left_print(handle_material(buf)));
            }
            else{
                cross_strcpy(global_filename, argv[1]);
                /*	strcat(buf, "$$$");*/
                cross_sprintf(buf_another, "(progn%s)$$$", buf);
                wrap_process(buf_another, sign + 10, argv[1]); /*modify by rosslyn.2013.6.9*/
                release_indicator();
                gc(left_print(handle_material(buf_another)));
            }
#else
        cross_read(argv[1], &sign, buf);
        cross_strcpy(global_filename, argv[1]);
        /*	strcat(buf, "$$$");*/
        cross_sprintf(buf_another, "(progn%s)$$$", buf);
        gc(left_print(handle_material(buf_another)));
#endif
        }
    }

    cross_free(buf);
    cross_free(buf_another);
    return 0;
}
