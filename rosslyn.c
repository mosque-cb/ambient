#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#define  size_t int

#ifdef _WIN32
#include  <winsock2.h>
#define sleep(n) Sleep(1000  *  (n))
#define MSG_NOSIGNAL 0
#else
#include<sys/wait.h>
#ifdef MYSELECT
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <getopt.h>
#endif

#ifdef MYEPOLL
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <getopt.h>
#endif
#endif

#define   BUF_SIZE 1024 * 1024

/*(string system)*/
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
    if(c<='Z' && c>='A') return (c + 32);
    return (c);
}

int cross_toupper(int c) {
    if(c<='z' && c>='a') return (c - 32);
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

int cross_strlen(const char* str) {
    return strlen(str);
}  

void cross_strcpy(void * left, void * right) {
    char * target = left;
    int count = cross_strlen(right);
    while (1) {
        *(char *)target++ = *(char *)right++;
        if (--count == 0) {
            *(char *)target = '\0'; 
            break;
        }
    }
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
void cross_fprintf(FILE* where, const char *fmt, ...)  
{  
    va_list ap;
    char format_string[BUF_SIZE];

    va_start(ap, fmt);
    vsprintf(format_string, fmt, ap);
    va_end(ap);

    fprintf(where, "%s", format_string);
    fflush(where);
}  

#define COLOR_MAXLEN 75

#define COLOR_NORMAL	""
#define COLOR_RESET		"\033[m"
#define COLOR_BOLD		"\033[1m"
#define COLOR_RED		"\033[31m"
#define COLOR_GREEN		"\033[32m"
#define COLOR_YELLOW	"\033[33m"
#define COLOR_BLUE		"\033[34m"
#define COLOR_MAGENTA	"\033[35m"
#define COLOR_CYAN		"\033[36m"
#define COLOR_BOLD_RED	"\033[1;31m"
#define COLOR_BOLD_GREEN	"\033[1;32m"
#define COLOR_BOLD_YELLOW	"\033[1;33m"
#define COLOR_BOLD_BLUE	"\033[1;34m"
#define COLOR_BOLD_MAGENTA	"\033[1;35m"
#define COLOR_BOLD_CYAN	"\033[1;36m"
#define COLOR_BG_RED	"\033[41m"
#define COLOR_BG_GREEN	"\033[42m"
#define COLOR_BG_YELLOW	"\033[43m"
#define COLOR_BG_BLUE	"\033[44m"
#define COLOR_BG_MAGENTA	"\033[45m"
#define COLOR_BG_CYAN	"\033[46m"
#define COLOR_NIL "NIL"

#define COLOR_UNKNOWN -1
#define COLOR_NEVER  0
#define COLOR_ALWAYS 1
#define COLOR_AUTO   2

static int color_vfprintf(FILE *fp, const char *color, const char *fmt,
                          va_list args, const char *trail) {
    int r = 0;

    if (*color)
        r += fprintf(fp, "%s", color);
    r += vfprintf(fp, fmt, args);
    if (*color)
        r += fprintf(fp, "%s", COLOR_RESET);
    if (trail)
        r += fprintf(fp, "%s", trail);
    return r;
}

int color_fprintf(FILE *fp, const char *color, const char *fmt, ...) {
    va_list args;
    int r = 0;
    va_start(args, fmt);

#ifdef _WIN32
    char format_string[BUF_SIZE];
    vsprintf(format_string, fmt, args);
    fprintf(fp, "%s", format_string);
    fflush(fp);
    //    cross_fprintf(fp, fmt, args, NULL);
#else
    r = color_vfprintf(fp, color, fmt, args, NULL);
#endif
    va_end(args);
    return r;
}

void cross_sprintf(char * format_string, const char *fmt, ...) {  
    va_list ap;
    va_start(ap, fmt);
    vsprintf(format_string, fmt, ap);
    va_end(ap);
}  

void cross_snprintf(char * format_string, size_t size, const char *fmt, ...) {  
    va_list ap;
    va_start(ap, fmt);

    /*size didn't use for linux 0.11 reason, it haven't vsnprintf */
    vsprintf(format_string, fmt, ap);
    va_end(ap);
}  
/*(end console system)*/

/*(file system)*/
void cross_fwrite(char *filename, char *data, int single, int length) {
    FILE *serial = NULL;
    serial = fopen(filename, "wb");
    fwrite(data, single, length, serial);
    fclose(serial);
}

int cross_ftell(char *file_tmp_name) {
    int count = 0;
    signed char ch;
    FILE* stream = fopen(file_tmp_name, "rb");
    if(stream == 0 ) {
        return -1;    
    }

    fseek(stream, 0, SEEK_SET);
    do {
        ch = fgetc(stream);
        count++;
    } 
    while (!feof(stream));
    count--;

    /*
    fseek(stream, 0L, SEEK_END);
    count = ftell(stream);
    */

    fclose(stream);
    return count;
} 

void cross_read(char *file_name, int *sign, char *buf) {
    FILE * fp;
    char file_tmp_name[256]="\0";
    int  file_size = 0;

    cross_sprintf(file_tmp_name,"%s",file_name);
    file_size = cross_ftell(file_tmp_name);
    if( file_size == -1 ) {
        *sign = -1;
        return;   
    }
    else {
        fp = fopen(file_tmp_name, "rb");
        cross_fprintf(stderr, "size is  %d \r\n", file_size);
        if (file_size >= BUF_SIZE) {
            *sign = BUF_SIZE;
            fclose(fp);
            return;
        }
        fread(buf, file_size, 1, fp );
        *sign = file_size;
        fclose(fp);
    }
}
/*(end file system)*/

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
        mem = calloc(sizeof(int) + count * length, sizeof(char));
        if (mem == 0) {
            exit(-1);
        }
        *(int*)mem = 0;
    }

    return (char*)mem + sizeof(int);
}

void * cross_malloc(int length) {
    return  cross_calloc(sizeof(char), length);
}

void cross_memcpy(void * dst, void * src, int length) {
    memcpy(dst, src, length);
}

void   cross_free(void *p) {
    if (*((int*)p - 1) == BUF_SIZE) {
        recycle_raw_object( (void*)( *(long*)( (char*)p - sizeof(int) - sizeof(long) ) ));
    }
    else {
        free(p - sizeof(int));
    }
}

#define  NULLVALUE  999999
#define  ARABIC     18
typedef void * (*original_callback)(void * _left);
enum tokens {
    NUMBER = 'n', NAME, VALUE
};

typedef enum {
    EMPTY = 1,
    CONT,
    BYTES,
    NET,
    STREAM,
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
    JOIN,
    PRESS,
    EVAL,
    SETQ,
    DEFINE,
    EQ,
    SYMBOL,
    ITER,
    SLEEP,
    PGET,
    LAMBDA,
    FORMAL,
    FUNCALL,
    SELF,
    NOP,
    EVIF = 120,
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
    EVJOIN,
    EVPROGNMID,
    EVPROGNEND,
    EFFUN = 150,
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
    EFITER,
    EFSLEEP,
    EFPGET,
    EFLAMBDA,
    EFFUNCALL,
    EFSELF,
    EFITEREND,
    EFLAMBDAEND,
    EFFUNCALLEND,
    EFEJECT,
    EFWHOLE,
    EFJOIN,
    EFPRESS,
    EFTOP,
    EFEXCHANGE,
    EFCOMBI,
    EFLISTBEGIN,
    EFLISTEND,
#ifdef FORTH
    ZFFORTH = 200,
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
        unsigned long i_data;
        char* a_storage;
        char s_data[ARABIC];
        struct type * n_data;
    } u_data;
    int ref_count;
    int obj_length;
    struct type* next;
    void* mother;
} type;

typedef struct {
    void* mem_next;
    type value;
} wraptype;

type * primitive_empty = NULL;
type * primitive_nop = NULL;
type * primitive_cont = NULL;
type * global_defmacro = NULL;

#define  AMOUNT 2000
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
type * primitive_evjoin = NULL;
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
type * primitive_efiter = NULL;
type * primitive_efsleep = NULL;
type * primitive_efpget = NULL;
type * primitive_efself = NULL;
type * primitive_effuncall = NULL;
type * primitive_eflambdaend = NULL;
type * primitive_efiterend = NULL;
type * primitive_effuncallend = NULL;
type * primitive_efeject = NULL;
type * primitive_eftop = NULL;
type * primitive_efcombi = NULL;
type * primitive_efexchange = NULL;
type * primitive_efwhole = NULL;
type * primitive_efjoin = NULL;
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

void** forth_code_unfix = (void**)0;
int forth_code_unfix_ipc = 0;

void** forth_macro_skip = (void**)0;
long forth_macro_skip_ipc = 0;

int global_forth_current_code_ipc = 0;

#if defined(HUFFMAN)
/*(code system)*/
typedef unsigned short compress_type;
compress_type * MOVE_INDICATOR;

typedef struct huff_tree {
    int number;
    int rank; /*modify  by  rosslyn  , 2012.2.20*/
    int occur_time; /*modify  by  rosslyn  , 2012.2.20*/
    struct huff_tree *parent; /*add by rosslyn*/
    struct huff_tree *son[2];  /*modify by  rosslyn, 2015.5.10*/
} huff_tree;

typedef struct {
    int bit_value;
    int bit_length;
} huff_measure;

/*#define  N   120*/
/*add  by  rosslyn ,because avr machine has little memeory,the execution file must be small*/
typedef struct {
    huff_tree * min_heap[BIT_VOLUME];
    int count;
} huff_object;

void huff_init(huff_object* min_heap_contain) {
    min_heap_contain->count = 1;
}

void huff_insert(huff_tree * newnode, huff_object* min_heap_contain) {
    huff_tree * present;
    int p, num;
    min_heap_contain->min_heap[min_heap_contain->count] =
        newnode;
    num = min_heap_contain->count;
    while (num / 2) {
        p = num / 2;
        if (min_heap_contain->min_heap[num]->occur_time
            < min_heap_contain->min_heap[p]->occur_time) {
            present = min_heap_contain->min_heap[num];
            min_heap_contain->min_heap[num] =
                min_heap_contain->min_heap[p];
            min_heap_contain->min_heap[p] = present;
        } else
            break;
        num = p;
    }
    min_heap_contain->count++;
}

#define  huff_left(i)  2*(i)
#define  huff_right(i)  2*(i)+1

huff_tree * huff_pop(huff_object* min_heap_contain) {
    huff_tree * present, *result = min_heap_contain->min_heap[1];
    int exchange;
    int label = 1;
    min_heap_contain->min_heap[label] =
        min_heap_contain->min_heap[min_heap_contain->count - 1];
    while (huff_left(label) <= min_heap_contain->count - 1) {
        if (huff_right(label) <= min_heap_contain->count - 1) {
            if (min_heap_contain->min_heap[huff_left (label)]->occur_time
                < min_heap_contain->min_heap[huff_right (label)]->occur_time)
                exchange = huff_left (label);
            else
                exchange = huff_right (label);
        } else
            exchange = huff_left (label);

        if (min_heap_contain->min_heap[exchange]->occur_time
            < min_heap_contain->min_heap[label]->occur_time) {
            present = min_heap_contain->min_heap[exchange];
            min_heap_contain->min_heap[exchange] =
                min_heap_contain->min_heap[label];
            min_heap_contain->min_heap[label] = present;
        } else
            break;
        label = exchange;
    }
    min_heap_contain->count--;
    return result;
}

#define  get_head(encryption_data, length)  (encryption_data) & MOVE_INDICATOR[ (length) ]

void set_head_value(char *encryption_data, int bit_length,
                    compress_type real_value, int *present_label) {
    compress_type *p_encryption_data;
    compress_type finally_value;
    int move_length;
    compress_type value;
    finally_value = real_value;
    p_encryption_data = (compress_type *) (encryption_data
                                           + (*present_label) / 8);
    move_length = *present_label % 8;
    value = (compress_type) get_head (*p_encryption_data, move_length);

    *p_encryption_data = finally_value;
    *p_encryption_data = *p_encryption_data << move_length;
    *p_encryption_data = *p_encryption_data | value;
    *present_label += bit_length;
}

huff_tree *newnode(void) {
    static int occur_time = 0;
    huff_tree *new = cross_calloc(1, sizeof(huff_tree));
    new->parent = new->son[0] = new->son[1] = NULL;
    new->number = -1;
    new->rank = 0;

    new->occur_time = occur_time++;
    return new;
}

void exchange(huff_tree* first, int is_left_first, huff_tree* second, int is_left_second) {
    huff_tree* first_node;
    int time_present_second;

    first_node = first->son[is_left_first];
    time_present_second = second->son[is_left_second]->occur_time;

    first->son[is_left_first] = second->son[is_left_second];
    first->son[is_left_first]->parent = first;
    first->son[is_left_first]->occur_time = first_node->occur_time;

    second->son[is_left_second] = first_node;
    second->son[is_left_second]->parent = second;
    second->son[is_left_second]->occur_time = time_present_second;
}

void add_node(huff_tree* node, huff_object* min_contain) {
    huff_tree* parent;
    huff_tree* big;
    int is_left_first, is_left_second;
    huff_tree* parent_backup = NULL;

    while (1) {
        parent_backup = NULL;
        while (1) {
            big = huff_pop(&min_contain[node->rank]); /*something in the heap  haven't  update */
            if (big->rank != node->rank) {
                continue;
            } 
            else {
                if (big->son[0] == node || big->son[1] == node) {
                    parent_backup = big;
                    continue; /*avoid  father-son  relation*/
                }
                if (parent_backup) {
                    huff_insert(parent_backup, &min_contain[parent_backup->rank]); /*mistaken  pop  big, so push  it back*/
                }
                break;
            }
        }

        if (big->occur_time != node->occur_time) {
            if (big->parent == NULL) {
                big->rank++;
                huff_insert(big, &min_contain[big->rank]);
                return;
            }
            if (big->parent->son[0] == big) {
                is_left_first = 0;
            } else {
                is_left_first = 1;
            }

            if (node->parent->son[0] == node) {
                is_left_second = 0;
            } else {
                is_left_second = 1;
            }

            exchange(big->parent, is_left_first, node->parent, is_left_second); /*using  father  node*/
            huff_insert(big, &min_contain[big->rank]); /*mistaken  pop  big, so push  it back*/
        }

        node->rank++;
        huff_insert(node, &min_contain[node->rank]);
        parent = node->parent;
        if (parent != NULL) {
            node = parent;
        } 
        else {
            break;
        }
    }
}

void adapt_node(int value, huff_tree *now, huff_object* min_contain) {
    huff_tree* left = NULL, * right = NULL;
    left = newnode();
    right = newnode(); /*according  to  the  book  for  demostration*/
    right->number = value;
    right->rank = 1;

    left->parent = right->parent = now;

    /*update  current now*/
    now->son[0] = left;
    now->son[1] = right;
    now->rank = 0;

    huff_insert(right, &min_contain[right->rank]);
    huff_insert(now, &min_contain[now->rank]);
}

unsigned int invert_bits(unsigned int value, int bit_length) {
    unsigned int tail = 0;
    unsigned int new_value = 0;
    int i = 0;
    for (i = 0; i < bit_length; i++) {
        tail = get_head(value, 1);
        value = value >> 1;
        new_value += tail << (bit_length - i - 1);
    }
    return new_value;
}

void get_huff_ruler(huff_tree *root, int bits, huff_measure * huff_ruler,
                    int depth) {
    int present_bits;
    if (root->son[0] == NULL && root->son[1] == NULL && root->number == -1) {
        return;
    }
    if (root->son[0] == NULL && root->son[1] == NULL && root->number != -1) {
        huff_ruler[root->number].bit_value = invert_bits(bits, depth);
        huff_ruler[root->number].bit_length = depth;
    } else {
        present_bits = bits << 1;
        get_huff_ruler(root->son[0], present_bits, huff_ruler, depth + 1);
        get_huff_ruler(root->son[1], present_bits + 1, huff_ruler, depth + 1);
    }
}

void free_huff_tree(huff_tree *root) {
    if (root->son[0] == NULL && root->son[1] == NULL && root->number == -1) {
        cross_free(root);
        return;
    }
    if (root->son[0] == NULL && root->son[1] == NULL && root->number != -1) {
        cross_free(root);
        return;
    } else {
        free_huff_tree(root->son[0]);
        free_huff_tree(root->son[1]);
        cross_free(root);
        return;
    }
}

int huff_tree_check(huff_tree *root) {
    if (root->son[0] == NULL && root->son[1] == NULL) {
        return root->rank;
    } 
    else {
        if (root->rank != huff_tree_check(root->son[0]) + huff_tree_check(root->son[1])) {
            color_fprintf(stderr, COLOR_RED, "exit huff_tree_check error\r\n");
        }
        return root->rank;
    }
}

void init_indicator(void) {
    int i = 0;
    MOVE_INDICATOR = (compress_type *) cross_calloc(8 * sizeof(compress_type) + 5,
                                                    sizeof(compress_type));
    for (i = 0; i < 8 * sizeof(compress_type); i++) {
        MOVE_INDICATOR[i] = (compress_type) (1 << i) - 1;
    }
    MOVE_INDICATOR[8 * sizeof(compress_type)] = (compress_type) (1 << (8 * sizeof(compress_type))) - 1;
}

void release_indicator(void) {
    cross_free(MOVE_INDICATOR);
}

void encryption(compress_type *data, char * encryption_data, int *present_label,
                int count, huff_tree **root, huff_object* min_contain) {
    compress_type value;
    compress_type single_data;
    int whole_count = 0;
    int length;
    huff_tree * label[BIT_VOLUME] = {NULL}, *now = NULL;
    huff_measure huff_ruler[BIT_VOLUME];

    *root = now = newnode();
    while (whole_count < count) {
        single_data = data[whole_count];
        cross_fprintf(stderr, "%d ", single_data);
        if(single_data > BIT_VOLUME) {
            color_fprintf(stderr, COLOR_RED, "exit encryption bytes incorrect %d %d %d %d\r\n", whole_count, count, single_data, BIT_VOLUME);
        }
        if (label[single_data] == NULL) {
            adapt_node(single_data, now, min_contain);

            get_huff_ruler(*root, 0, huff_ruler, 0); /*keep  as  usual*/
            value = huff_ruler[single_data].bit_value;
            length = huff_ruler[single_data].bit_length;
            set_head_value(encryption_data, length, value, present_label);
            set_head_value(encryption_data, 8 * sizeof(compress_type),
                           single_data, present_label); /* it's  new  symbol , so  needs  record */

            add_node(now, min_contain);
            /*update  label*/
            label[single_data] = now->son[1];
            /*update new  now*/
            now = now->son[0];
        } 
        else {
            get_huff_ruler(*root, 0, huff_ruler, 0); /*keep  as  usual*/
            value = huff_ruler[single_data].bit_value;
            length = huff_ruler[single_data].bit_length;
            set_head_value(encryption_data, length, value, present_label);

            add_node(label[single_data], min_contain);
        }
        huff_tree_check(*root);
        whole_count++;
    }
    cross_fprintf(stderr, "present_label is %d \r\n", *present_label);
}

compress_type get_head_value(unsigned char *encryption_data, int *label, int bit_length) {
    compress_type *p_encryption_data;
    int move_length;
    compress_type value;
    p_encryption_data = (compress_type *) (encryption_data + *label / 8);
    move_length = (*label) % 8;
    value = *p_encryption_data;
    value = value >> move_length;
    *label += bit_length;
    return get_head(value, bit_length);
}

int decryption(unsigned char* encryption_data, int present_label,
               compress_type* decryption_contain, huff_tree** root, huff_object* min_contain) {
    int label = 0;
    compress_type direction;
    compress_type value; /*must  bu  unsigned , add  by  rosslyn   2012.2.2*/
    unsigned int i = 0;
    huff_tree* search;
    huff_tree* now;
    *root = now = newnode();

    cross_fprintf(stderr, "compress_type is %d present_label is %d \r\n", sizeof(compress_type), present_label);
    while (label < present_label) {
        search = *root;
        while (1) {
            direction = get_head_value(encryption_data, &label, 1);
            if (search->son[direction] == NULL) {
                value = get_head_value(encryption_data, &label,
                                       8 * sizeof(compress_type));
                cross_fprintf(stderr, "%d ", value);
                if(value > BIT_VOLUME) {
                    color_fprintf(stderr, COLOR_RED, "exit \r\ndecryption bytes incorrect %d %d %d %d\r\n", label, present_label, value, BIT_VOLUME);
                }
                decryption_contain[i++] = value;

                adapt_node(value, search, min_contain);
                break;
            }
            else{
                search = search->son[direction];
            }

            if (search->son[0] == NULL && search->son[1] == NULL && search->number != -1) {
                decryption_contain[i++] = search->number;
                break;
            }
        }
        add_node(search, min_contain);
    }
    return i;
}

void huffman_encryption(unsigned char *_lisa, int _length, char *filename) {
    int i = 0;
    compress_type *encryption_contain = NULL;
    char *encryption_data = NULL;
    int present_label = 0;
    int length = 2 * _length;

    huff_tree *root = NULL;
    char big_char = (char) 0;
    char file_tmp_name[256] = "\0";

    huff_object* min_contain = (huff_object *) cross_calloc(6 * length, sizeof(huff_object));
    for (i = 0; i < 6 * length; i++){
        huff_init(&min_contain[i]);
    }

    encryption_contain = (compress_type *) cross_calloc(length + EXTRA, sizeof(compress_type));
    encryption_data = (char *) cross_calloc(length, 20 * sizeof(int));

    for (i = 0; i < _length; i++) {
        encryption_contain[2 * i] = _lisa[i] & 0x0F;
        /*    encryption_contain[2*i + 1] = _lisa[i] & 0xF0;
              follow code can increase the compress ratio*/
        encryption_contain[2 * i + 1] = (_lisa[i] & 0xF0) >> 4;
        if (_lisa[i] > big_char)
            big_char = _lisa[i];
    }

    cross_fprintf(stderr, "this big_char is  %d  %d  %d \r\n", big_char, big_char & 0x0F, big_char & 0xF0);
    encryption(encryption_contain, encryption_data, &present_label, length, &root, min_contain);

    cross_sprintf(file_tmp_name, "%sc", filename);

    cross_fwrite(file_tmp_name, encryption_data, sizeof(char), present_label / 8 + 1);

    cross_free(min_contain); /*clean  heap  memory*/
    free_huff_tree(root);
    cross_free(encryption_contain);
    cross_free(encryption_data);
}

void huffman_decryption(unsigned char *_lisa, int _length) {
    int i = 0, j = 0;
    huff_object* min_contain = NULL;
    compress_type * decryption_contain = NULL;
    huff_tree* root = NULL;
    int length = 4 * _length;

    decryption_contain = (compress_type *) cross_calloc(5 * length + EXTRA,
                                                        sizeof(compress_type));
    min_contain = (huff_object *) cross_calloc(6 * length,
                                               sizeof(huff_object));

    for (i = 0; i < 6 * length; i++){
        huff_init(&min_contain[i]);
    }

    j = decryption(_lisa, 8 * _length, decryption_contain, &root, min_contain);

    for (i = 0; i < j / 2; i++) {
        _lisa[i] = (unsigned char) decryption_contain[2 * i]
            + (((unsigned char) decryption_contain[2 * i + 1]) << 4);
    }

    free_huff_tree(root);
    cross_free(decryption_contain);
    cross_free(min_contain); /*clean  heap  memory*/
}

void wrap_process(char *_lisa, int length, char *filename) {
    int sign = 0;
    char file_tmp_name[256] = "\0";
    huffman_encryption((unsigned char*)_lisa, length, filename);
    cross_sprintf(file_tmp_name, "%sc", filename);
    cross_read(file_tmp_name, &sign, _lisa);
    huffman_decryption((unsigned char*)_lisa, sign);
}
/*(code system end)*/
#endif

int raw_global_count = 40;
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

int global_count = 43021;
int allocation = 0;
int recycled = 0;
int local_recycled = 0, local_allocation = 0;
void count_object(void) {
    cross_fprintf(stderr, "%d  ", allocation);
}

void * withdraw(void * _left);
type * new_object(void) {
    type * outcome;

    if (!mem_manager_unused) {
        if (primitive_used->mem_next == NULL) {
            withdraw(primitive_empty); /*modify  by  rosslyn  for High Order Machine 2013.4.23*/
        }
        mem_manager_unused = primitive_used->mem_next;
        primitive_used->mem_next = NULL;
        mem_manager_used = primitive_used;
    }

    outcome = &(mem_manager_unused->value);
    outcome->mother = mem_manager_unused;
    mem_manager_unused = mem_manager_unused->mem_next;
    ((wraptype *)outcome->mother)->mem_next = NULL;
    outcome->ref_count = 1;
    outcome->obj_length = 1;

    allocation++;
    return outcome;
}

void * long_type(unsigned long i);

void * c_copy_atom(void * _right) {
    type * right = _right;
    if (right->em == EMPTY) {
        return right;
    }
    right->ref_count++;
    return  right;
}

void * c_cons(void * _left, void * _right) {
    type *type_data, * left = _left, * right = _right;

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

#define   gc_atom_inner(left)  { ((wraptype*)(((type*)(left))->mother))->mem_next = NULL; mem_manager_used->mem_next = ((wraptype*)(((type*)(left))->mother));  mem_manager_used = ((wraptype*)(((type*)(left))->mother)); recycled++ ;}

void * left_fprint(FILE*, void * _left);
void remove_lambda(void *_left);

void gc_atom(void * _left) {
    type  * left = _left;
    if ((--left->ref_count) == 0) {
        /*2014.7.24*/
        if (left->em == STORAGE) {
            cross_free(left->u_data.a_storage);
        }
        else if (left->em == VAR) {
            remove_lambda(left);
        }
        gc_atom_inner(left);
    }
    if(left->ref_count < 0) {
        color_fprintf(stderr, COLOR_RED, "ref_count error %d\r\n", left->ref_count);
        left_fprint(stderr, left);
        cross_fprintf(stderr, "\r\n");
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

void * left_fprint(FILE* where, void * _left) {
    type * left = _left;
    type * present;
    if (!left) {
        return primitive_empty;
    }
    switch (left->em) {
    case DICTORY:
        cross_fprintf(where, "%s", "dictory");
        break;
    case EMPTY:
        cross_fprintf(where, "%s", "nil");
        break;
    case NOP:
        cross_fprintf(where, "%s", "nop");
        break;
    case DEBUG:
        color_fprintf(where, COLOR_RED, "%s", left->u_data.a_storage + sizeof(int));
        break;
    case STORAGE:
        cross_fprintf(where, "%s", left->u_data.a_storage + sizeof(int));
        break;
    case CONT:
        cross_fprintf(where, "continue");
        break;
    case ITER:
        cross_fprintf(where, "iter");
        break;
    case SLEEP:
        cross_fprintf(where, "sleep");
        break;
    case PGET:
        cross_fprintf(where, "pget");
        break;
    case LAMBDA:
        cross_fprintf(where, "lambda");
        break;
    case EVAL:
        cross_fprintf(where, "eval");
        break;
    case BYTES:
        if (left->u_data.i_data == NULLVALUE)
            cross_fprintf(where, "%s", "nil");
        else
            cross_fprintf(where, "%lu", left->u_data.i_data);
        break;
    case PROGN:
        cross_fprintf(where, "%s", "progn");
        break;
    case VAR:
        cross_fprintf(where, "%s", left->u_data.s_data);
        break;
        /*huuu , for less memory,there is no room for s_data of FUN,so use address instead.*/
    case FUN:
        cross_fprintf(where, "%s", "fun");
        break;
    case QUOTE:
        cross_fprintf(where, "%s", "quote");
        break;
    case DEFUN:
        cross_fprintf(where, "%s", "defun");
        break;
    case FUNCALL:
        cross_fprintf(where, "%s", "funcall");
        break;
    case SELF:
        cross_fprintf(where, "%s", "self");
        break;
    case DEFMACRO:
        cross_fprintf(where, "%s", "defmacro");
        break;
    case SETQ:
        cross_fprintf(where, "%s", "setq");
        break;
    case DEFINE:
        cross_fprintf(where, "%s", "define");
        break;
    case IF:
        cross_fprintf(where, "%s", "if");
        break;
#ifdef FORTH
    case ZFSWAP:
        cross_fprintf(where, "%s", "zfswap");
        break;
    case ZFBIG:
        cross_fprintf(where, "%s", "zfbig");
        break;
    case ZFOVER:
        cross_fprintf(where, "%s", "zfover");
        break;
    case ZFDROP:
        cross_fprintf(where, "%s", "zfdrop");
        break;
    case ZFRANDOM:
        cross_fprintf(where, "%s", "zfrandom");
        break;
    case ZFQUOTE:
        cross_fprintf(where, "%s", "zfquote");
        break;
    case ZFDOTCHAR:
        cross_fprintf(where, "%s", "zfdotchar");
        break;
    case ZFMOD:
        cross_fprintf(where, "%s", "zfmod");
        break;
    case ZFMINUS:
        cross_fprintf(where, "%s", "zfminus");
        break;
    case ZFADD:
        cross_fprintf(where, "%s", "zfadd");
        break;
    case ZFEQ:
        cross_fprintf(where, "%s", "zfeq");
        break;
    case ZFTHEN:
        cross_fprintf(where, "%s", "zfthen");
        break;
    case ZFELSEIF:
        cross_fprintf(where, "%s", "zfelseif");
        break;
    case ZFIF:
        cross_fprintf(where, "%s", "zfif");
        break;
    case ZFDUP:
        cross_fprintf(where, "%s", "zfdup");
        break;
    case ZFDOT:
        cross_fprintf(where, "%s", "zfdot");
        break;
    case ZFCOLON:
        cross_fprintf(where, "%s", "zfcolon");
        break;
    case ZFFORTH:
        cross_fprintf(where, "%s", "zfforth");
        break;
#endif
    case LIST:
        cross_fprintf(where, "(");
        for (present = left; present->em != EMPTY; present = c_cdr(present)) {
            left_fprint(stderr, c_car(present));
            cross_fprintf(where, " ");
        }
        cross_fprintf(where, ")");
        break;
    default:
        cross_fprintf(where, " type is %d ", left->em);
        break;
    }
    return _left;
}

void * new_debug(char* _material, int size) {
    char head[26] = "!DEBUG:\r\n"; 
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

void gc(void * _left) {
    type * left = _left;
    if (left->em == EMPTY) {
        return;
    }
    else if (left->em != LIST) {
        gc_atom(left);
    } 
    else {
        if (left->ref_count == 1) {
            gc(c_car(left));
            gc(c_cdr(left));
        }
        gc_atom(left);
    }
}

void * c_normal_copy(void * _right) {
    type * right = _right;
    if (right->em == EMPTY) {
        return right;
    }
    return c_copy_atom(right);
}

void* original_memory(void * _left) {
    int i = 0;
    type* right = c_car(_left);
    char debug_inf[256] = "\0";
    if (right->em != BYTES) {
        cross_strcpy(debug_inf, "1st shoule be BYTES\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }

    unsigned long count = right->u_data.i_data;
    /*follow code is special too. use calloc instead of cross_calloc*/
    wraptype* unused = 
        (wraptype *) cross_calloc(sizeof(wraptype), count);

    for (i = 0; i < count - 1; i++) {
        unused[i].mem_next = &unused[i + 1];
    }
    unused[count - 1].mem_next = mem_manager_unused;
    mem_manager_unused = unused;
    return primitive_empty;
}

void * original_cons(void * _left) {
    type * left = c_car(_left);
    type * right = c_cadr(_left);
    type * present = _left;

    char debug_inf[256] = "\0";
    if ((right-> em != EMPTY) && (right->em != LIST)) {
        cross_strcpy(debug_inf, "right value shoule be list or nil\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }
    return c_cons(c_normal_copy(left), c_normal_copy(right));
}

void * original_car(void * _left) {
    type *left = c_car(_left);
    char debug_inf[256] = "\0";
    type* outcome = primitive_empty;
    type* present = _left;
    if (left->em == EMPTY) {
        return outcome;
    } 
    else if (left->em != LIST) {
        cross_strcpy(debug_inf, "first should be a list\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }
    else {
        outcome = c_car(left);
        return c_normal_copy(outcome);
    }
}

void * original_cdr(void * _left) {
    type* left = c_car(_left);
    char debug_inf[256] = "\0";
    type* outcome = primitive_empty;
    type* present = _left;
    if (left->em == EMPTY) {
        return outcome;
    }
    else if (left->em != LIST) {
        cross_strcpy(debug_inf, "first should be a list\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    } 
    else {
        outcome = c_cdr(left);
        return c_normal_copy(outcome);
    }
}

int c_eq(void * _left, void * _right) {
    type * left = _left;
    type * right = _right;
    int outcome;
    int outcomex;

    if (c_atom(left ) && c_atom(right)) {
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
                if(outcome != outcomex) {
                    color_fprintf(stderr, COLOR_RED, "interesting\r\n");
                }
                break;
            case BYTES:
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
    } else {
        outcome = 0;
    }

    return outcome;
}

void* original_eq(void * _left) {
    type * left = _left;
    type * right =
        c_eq(c_car(left), 
             c_cadr(left))
        ? long_type(1) : long_type(0);
    /*this code is very important, becasue c_eq  has a little special meaning , and it destroy  the parameter,*/
    return right;
}

void * original_not(void * _left) {
    type * left = c_car(_left);
    type * outcome = primitive_empty;
    char debug_inf[256] = "\0";
    if (left->em != BYTES) {
        cross_strcpy(debug_inf, "first para should be a BYTES\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }

    if (left->u_data.i_data == 0){
        outcome = long_type(1);
    }
    else{
        outcome = long_type(0);
    }
    return outcome;
}

void * original_and(void * _left) {
    type  *left = _left, * right = NULL;
    char debug_inf[256] = "\0";
    if (left->em != LIST) {
        cross_strcpy(debug_inf, " first para should be a LIST\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }

    while (left->em == LIST) {
        right = c_car(left);
        if (right->em != BYTES) {
            cross_strcpy(debug_inf, "second para should be a BYTES\r\n");
            return new_debug(debug_inf, cross_strlen(debug_inf));
        }

        if(right->u_data.i_data != 1){
            return long_type(0);
        }
        left = c_cdr(left);
    }
    return long_type(1);
}

void * original_or(void * _left) {
    type  *left = _left, * right = NULL;
    char debug_inf[256] = "\0";
    if (left->em != LIST) {
        cross_strcpy(debug_inf, " first para should be a LIST\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }

    while (left->em == LIST) {
        right = c_car(left);
        if (right->em != BYTES) {
            cross_strcpy(debug_inf, "second para should be a BYTES\r\n");
            return new_debug(debug_inf, cross_strlen(debug_inf));
        }

        if(right->u_data.i_data == 1){
            return long_type(1);
        }
        left = c_cdr(left);
    }
    return long_type(0);
}

void * original_atom(void * _left) {
    int value = c_atom(c_car(_left));
    return long_type(value);
}

void* c_append(void * _left, void * _right) {
    type * left = (type*)_left;
    type * outcome = (type*)_right;
    type * present = NULL;

    if (left->em != EMPTY) {
        while (1) {
            left->obj_length += outcome->obj_length;
            present = c_cdr(left);
            if (present->em == EMPTY) {
                left->next = outcome;
                break;
            } 
            left = present;
        }
        outcome = _left;
    }

    return outcome;
}

void * c_list(void * left, ...) {
    type * ele_left;
    type * ele_right;
    va_list ap;
    ele_left = left;
    ele_left = c_cons(ele_left, primitive_empty);
    va_start(ap, left);

    while (1) {
        ele_right = va_arg(ap, void*);
        if (ele_right) {
            ele_left = c_append(ele_left, c_cons(ele_right, primitive_empty));
        }
        else {
            break;
        }
    }
    va_end(ap);
    return ele_left;
}

void * original_list(void * _left) {
    return c_normal_copy(_left);
}

void * long_type(unsigned long i) {
    type * outcome = primitive_empty;
    unsigned long abs_i = i;
    if (i < 0) {
        abs_i = 0 - i;
    }

    if(abs_i < AMOUNT){
        return c_copy_atom(primitive_small[AMOUNT + i]);
    }
    else{
        outcome = new_object();
        outcome->em = BYTES;
        outcome->u_data.i_data = i;
        return outcome;
    }
}

void * operate_type(native_operator indicator) {
    type * outcome = new_object();
    outcome->em = indicator;
    return outcome;
}

void gc(void *);
void * original_print(void * _left) {
    type * outcome;
    outcome = left_fprint(stdout, c_car (_left)); /*modify by chebing  2011.3.11*/
    cross_fprintf(stdout, "\r\n");

    return c_normal_copy(outcome);
}

void * original_printerr(void * _left) {
    type * outcome;
    outcome = left_fprint(stderr, c_car (_left)); /*modify by chebing  2011.3.11*/
    cross_fprintf(stderr, "\r\n");

    return c_normal_copy(outcome);
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

unsigned int hashkey(flexhash* obj, void* key, int length) {
    unsigned int ihash = 0;
    char *pkey = (char *)key;
    size_t label = 0;

    for (label = 0; label < length; label++)
        ihash = (ihash << 5) + ihash + *pkey++;

    return ihash;
}

memory* getarea(flexhash* obj, void* key, unsigned int* ihash, int length) {
    memory* pmemory = NULL;
    if (obj->m_hashkey == NULL) {
        *ihash = hashkey(obj, key, length) % obj->m_hashsize;
    }
    else {
        *ihash = (*obj->m_hashkey)(key) % obj->m_hashsize;
    }

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
        *ihash = hashkey(obj, key, length) % obj->m_hashsize;
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

void freememory(flexhash* obj, memory* pmemory) {
    if (pmemory == NULL) {
        return;
    }

    memset(pmemory + 2, 0, (obj->m_datasize - 2) * sizeof(memory));
    pmemory->next = obj->m_pfreelist;
    obj->m_pfreelist = pmemory;
    obj->m_count--;
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
    
    /*  obj->m_block = (char *)calloc(obj->m_keysize, obj->m_valuesize);*/
    return obj;
}

void recreate(flexhash* obj) {
    removeall(obj);
    obj->m_phash = (memory**)cross_calloc(obj->m_hashsize, sizeof(memory*));
}

void destroy(flexhash* obj) {
    removeall(obj);
    /*  free(obj->m_block);*/
}

int setat(flexhash* obj, void* key, void* newvalue) {
    memory* pmemory;
    unsigned int ihash;
    int length = cross_strlen(key);
    if (length > obj->m_keysize) {
        length = obj->m_keysize;
    }

    if ((pmemory = getarea(obj, key, &ihash, length)) == NULL) {
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
    if (pmemory == NULL) {
        return 0;
    }

    *(int*)(&pmemory->length) = 0;
    freememory(obj, pmemory);
    return 1;
}

int getat(flexhash* obj, void* key, void* value) {
    unsigned int ihash = 0;
    int length = cross_strlen(key);
    if (length > obj->m_keysize) {
        length = obj->m_keysize;
    }

    memory* pmemory = getarea(obj, key, &ihash, length);
    if (pmemory == NULL) {
        return 0;
    }
    
    if(value) {
        cross_memcpy(value, (char*)(pmemory + 2) + obj->m_keysize, obj->m_valuesize);
    }
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
flexhash * global_code = NULL;
flexhash * global_status = NULL;
flexhash * global_stream = NULL;

flexhash * global_lambda = NULL;
flexhash * global_defun = NULL;
flexhash * global_var = NULL;
flexhash * global_define = NULL;
flexhash * global_primitive = NULL;

void assign(char* word, void* _expr, void* _env, flexhash** e) {
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
        color_fprintf(stderr, COLOR_RED, "impossible entry\r\n");
    }
    else {
        delete(e, word);
    }
    return e;
    /* global_lambda = freeentry("kkk", global_lambda);*/
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

void * original_lambdap(void * _left) {
    type * mid_expr, *outcome;
    type * left = (type *)c_car(_left);
    entry result;
    if (left->em != VAR) {
        return long_type(0);
    }
    result = lookup((char *)(left->u_data.s_data), global_lambda);
    mid_expr = result.u_data._expr;
    if (mid_expr == NULL) {
        outcome = long_type(0);
    } 
    else {
        outcome = long_type(1);
    }
    return outcome;
}

void * original_fload(void * _left) {
    type* left = primitive_empty;
    type* present = primitive_empty;
    char* response = NULL; 
    char debug_inf[256] = "\0";
    int sign = 0;
    char* material = NULL;

    left = c_car (_left);
    if(left->em == VAR){
        material = left->u_data.s_data;
    }
    else if(left->em == STORAGE){
        material = left->u_data.a_storage + sizeof(int);
    }
    else {
        cross_strcpy(debug_inf, "first para should be a VAR or STORAGE\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }

    response = (char*) cross_calloc(BUF_SIZE, sizeof(char));
    cross_read(material, &sign, response);
    if(sign == -1){
        cross_sprintf(debug_inf, "%s Not Exists\r\n", material);
        cross_free(response);
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }
    if(sign == BUF_SIZE){
        cross_sprintf(debug_inf, "File Too Big\r\n");
        cross_free(response);
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }
    present = new_storage(response, sign);
    cross_free(response);
    return present;
}

void* original_funload(void * _left) {
    type* left = c_car (_left);
    type * present = c_cadr(_left);
    type* outcome = primitive_empty;
    char * material = NULL;
    char* filename = NULL;
    int len = 0;
    char debug_inf[256] = "\0";

    if(left->em == VAR){
        filename = left->u_data.s_data;
    }
    else if(left->em == STORAGE){
        filename = left->u_data.a_storage + sizeof(int);
    }
    else {
        cross_strcpy(debug_inf, "first para should be a VAR or STORAGE\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }

    if(present->em == VAR){
        material = present->u_data.s_data;
        len = cross_strlen(material);
    }
    else if(present->em == STORAGE){
        material = present->u_data.a_storage + sizeof(int);
        len = *(int*)present->u_data.a_storage - 1;
    }
    else{
        cross_sprintf(debug_inf, "second para shoule be VAR or STORAGE type\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }

    cross_fwrite(filename, material, sizeof(char), len);  /*some input too long, so use BUF_SIZE instead of 1024*/
    outcome = c_copy_atom(present);
    return outcome;
}

void * var_type(char * name);

void * original_ftell(void * _left) {
    type* left = primitive_empty;
    char debug_inf[256] = "\0";
    char* material = NULL;
    int size = 0;

    left = c_car (_left);
    if(left->em == VAR){
        material = left->u_data.s_data;
    }
    else if(left->em == STORAGE){
        material = left->u_data.a_storage + sizeof(int);
    }
    else {
        cross_strcpy(debug_inf, "1st should be a VAR or STORAGE\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }
    size = cross_ftell(material);
    if (size == -1) {
        return primitive_empty;   
    }
    else {
        return long_type(size);
    }
}

void * original_fopen(void * _left) {
    type* left = primitive_empty;
    type* present = primitive_empty;
    char debug_inf[256] = "\0";
    char* material = NULL;
    char* mode = NULL;
    FILE* output = NULL;

    left = c_car (_left);
    if(left->em == VAR){
        material = left->u_data.s_data;
    }
    else if(left->em == STORAGE){
        material = left->u_data.a_storage + sizeof(int);
    }
    else {
        cross_strcpy(debug_inf, "1st should be a VAR or STORAGE\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }

    left = c_cadr (_left);
    if(left->em == VAR){
        mode = left->u_data.s_data;
    }
    else if(left->em == STORAGE){
        mode = left->u_data.a_storage + sizeof(int);
    }
    else {
        cross_strcpy(debug_inf, "2st should be a VAR or STORAGE\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }

    output = fopen(material, mode);
    if (output == NULL) {
        cross_snprintf(debug_inf, 256, "file %s doesn't exist\r\n", material);
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }
    present = (type*)new_object();
    present->em = STREAM;
    present->u_data.i_data = (long)output;
    return present;
}

void * original_fwrite(void * _left) {
    type* left = primitive_empty;
    char debug_inf[256] = "\0";
    FILE* output = NULL;
    char* material = NULL;
    size_t size = 0;

    left = c_car (_left);
    if(left->em == STREAM){
        output = (FILE*)left->u_data.i_data;
    }
    else {
        cross_strcpy(debug_inf, "1st should be a STREAM type\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }

    left = c_cadr (_left);
    if(left->em == VAR){
        material = left->u_data.s_data;
        size = strlen(material);
    }
    else if(left->em == STORAGE){
        material = left->u_data.a_storage + sizeof(int);
        size = *(int*)left->u_data.a_storage - 1;
    }
    else {
        cross_strcpy(debug_inf, "2st should be a VAR or STORAGE\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }

    fwrite(material, 1, size, output);
    return primitive_empty;
}

void * original_fread(void * _left) {
    type* left = primitive_empty;
    type* present = primitive_empty;
    char debug_inf[256] = "\0";
    FILE* output = NULL;
    size_t size = 0;
    char * response = (char*) cross_calloc(BUF_SIZE, sizeof(char));

    left = c_car (_left);
    if(left->em == STREAM){
        output = (FILE*)left->u_data.i_data;
    }
    else {
        cross_strcpy(debug_inf, "1st should be a STREAM type\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }

    left = c_cadr (_left);
    if(left->em != BYTES){
        cross_strcpy(debug_inf, "2st should be a BYTES\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }

    size = left->u_data.i_data;

    fread(response, 1, size, output);
    present = new_storage(response, size);
    cross_free(response);
    return present;
}

void * original_feof(void * _left) {
    type* left = primitive_empty;
    type* present = primitive_empty;
    char debug_inf[256] = "\0";
    FILE* output = NULL;

    left = c_car (_left);
    if(left->em == STREAM){
        output = (FILE*)left->u_data.i_data;
    }
    else {
        cross_strcpy(debug_inf, "1st should be a STREAM type\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }

    if(!feof(output)){
        present = long_type(0);
    }
    else{
        present = long_type(1);
    }
    return present;
}

void * original_fgets(void * _left) {
    type* left = primitive_empty;
    type* present = primitive_empty;
    char debug_inf[256] = "\0";
    FILE* output = NULL;
    char * response = (char*) cross_calloc(BUF_SIZE, sizeof(char));

    left = c_car (_left);
    if(left->em == STREAM){
        output = (FILE*)left->u_data.i_data;
    }
    else {
        cross_strcpy(debug_inf, "1st should be a STREAM type\r\n");
        cross_free(response);
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }
    fgets(response, BUF_SIZE, output);
    present = new_storage(response, cross_strlen(response));
    cross_free(response);
    return present;
}

void * original_fclose(void * _left) {
    type* left = primitive_empty;
    char debug_inf[256] = "\0";
    FILE* output = NULL;

    left = c_car (_left);
    if(left->em == STREAM){
        output = (FILE*)left->u_data.i_data;
    }
    else {
        cross_strcpy(debug_inf, "1st should be a STREAM type\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }
    fclose(output);
    return primitive_empty;
}

void * original_printchar(void * _left) {
    type * outcome;
    outcome = left_fprint(stderr, c_car (_left)); /*modify by rosslyn  2011.3.11 */
    return c_normal_copy(outcome);
}

void * fun_type(void * name);
void * original_primitive(void * _left) {
    type * outcome = primitive_empty;
    type * fun = primitive_empty;
    type * left = (type *) c_car(_left);
    char * material = NULL;
    char debug_inf[256] = "\0";
  
    if(left->em == VAR){
        material = left->u_data.s_data;
    }
    else if(left->em == STORAGE){
        material = left->u_data.a_storage + sizeof(int);
    }
    else {
        cross_strcpy(debug_inf, "1st should be a VAR or STORAGE\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }

    fun = (type*)fun_type(material);
    if (fun) {
    }
    else{
        cross_strcpy(debug_inf, "2st haven't found correspond function\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }
    outcome = fun->u_data.f_data(c_cadr(_left));
    gc_atom(fun);
    return outcome;
}

void * original_primitivep(void * _left) {
    type * outcome;
    type * left = (type *) c_car(_left);
    /*   follow code is different with the lambdap, left->em have not this assumption,
         because you can write the code ( primitivep  'cons ...)  then  left->em is VAR
         also  you can  fetch  the left value from  a list , then  at  the parse  phrase the em  would be determined as  FUN, so now we just do  nothing.
         if (fun_type ((char  * )(left->u_data.s_data))) */

    char * material = NULL;
  
    if(left->em == VAR){
        material = left->u_data.s_data;
    }
    else if(left->em == STORAGE){
        material = left->u_data.a_storage + sizeof(int);
    }
    else{
        outcome = long_type(0);
        return outcome;
    }

    outcome = (type*)fun_type(material);
    if (outcome) {
        gc_atom(outcome);
        outcome = long_type(1);
    }
    else{
        outcome = long_type(0);
    }

    return outcome;
}

void * original_display(void * _left) {
    char buf[256] = "\0";
    char debug_inf[256] = "\0";
    cross_sprintf(buf, "\r\nallocation: %d\r\n", allocation - local_allocation);
    local_allocation = allocation;
    cross_sprintf(debug_inf, "%srecycled: %d\r\n", buf, recycled - local_recycled);
    local_recycled = recycled;
    return left_fprint(stderr, new_storage(debug_inf, cross_strlen(debug_inf)));
}

void * withdraw(void * _left) {
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
    type * left = c_car(_left), *right = c_cadr(_left);
    if (left->em != BYTES) {
        cross_strcpy(debug_inf, "1st should be a BYTES\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }
    if (right->em != BYTES) {
        cross_strcpy(debug_inf, "2st should be a BYTES\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }

    outcome = ((type *)left)->u_data.i_data - ((type *)right)->u_data.i_data;
    if (outcome > 0){
        return long_type(1);
    }
    else{
        return long_type(0);
    }
}

int power(int x, int n) {
    if(0 == n) {
        return 1;
    }
    else if(1 == n) {
        return x;
    }
    else if(2 == n) {
        return x * x;
    }

    if(0 == (n % 2)) {
        return power(power(x, n /2), 2);
    }
    else {
        return x * power(x, n - 1);
    }
}

void * original_power(void * _left) {
    int outcome;
    char debug_inf[256] = "\0";
    type * left = c_car (_left ), *right = c_cadr (_left);
    if (left->em != BYTES) {
        cross_strcpy(debug_inf, "1st should be a BYTES\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }
    if (right->em != BYTES) {
        cross_strcpy(debug_inf, "2st should be a BYTES\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }

    outcome = power(left->u_data.i_data,
                    right->u_data.i_data);
    return long_type(outcome);
}

void * original_add(void * _left) {
    type * present;
    type * left = _left;
    type * right = primitive_empty;
    unsigned long outcome = 0;
    char debug_inf[256] = "\0";
    for (present = left; present->em != EMPTY; present = c_cdr (present)) {
        right = c_car(present);
        if (right->em != BYTES) {
            cross_strcpy(debug_inf, "value should be a BYTES\r\n");
            return new_debug(debug_inf, cross_strlen(debug_inf));
        }
        outcome += right->u_data.i_data;
    }
    return long_type(outcome);
}

void * original_minus(void * _left) {
    type * present = NULL;
    type * left = _left;
    type * right = primitive_empty;
    char debug_inf[256] = "\0";
    unsigned long value = ((type *)c_car(left))->u_data.i_data;
    for (present = c_cdr(left); present->em != EMPTY; present = c_cdr(present)) {
        right = c_car(present);
        if (right->em != BYTES) {
            cross_strcpy(debug_inf, "value should be a BYTES\r\n");
            return new_debug(debug_inf, cross_strlen(debug_inf));
        }
        value -= right->u_data.i_data;
    }
    return long_type(value);
}

void * original_div(void * _left) {
    type * present = NULL;
    type * left = _left;
    char debug_inf[256] = "\0";
    type * right = primitive_empty;
    unsigned long value = ((type *)c_car(left))->u_data.i_data;
    for (present = c_cdr(left); present->em != EMPTY; present = c_cdr(present)) {
        right = c_car(present);
        if (right->em != BYTES) {
            cross_strcpy(debug_inf, "value should be a BYTES\r\n");
            return new_debug(debug_inf, cross_strlen(debug_inf));
        }
        value /= right->u_data.i_data;
    }
    return long_type(value);
}

void * original_mod(void * _left) {
    type * present = NULL;
    type * left = _left;
    type * right = primitive_empty;
    char debug_inf[256] = "\0";
    unsigned long value = ((type *)c_car(left))->u_data.i_data;
    for (present = c_cdr(left); present->em != EMPTY; present = c_cdr(present)) {
        right = c_car(present);
        if (right->em != BYTES) {
            cross_strcpy(debug_inf, "value should be a BYTES\r\n");
            return new_debug(debug_inf, cross_strlen(debug_inf));
        }
        value %= right->u_data.i_data;
    }
    return long_type(value);
}

void * original_mul(void * _left) {
    type * present;
    type * left = _left;
    unsigned long outcome = 1;
    char debug_inf[256] = "\0";
    type * right = primitive_empty;
    for (present = left; present->em != EMPTY; present = c_cdr (present)) {
        right = c_car(present);
        if (right->em != BYTES) {
            cross_strcpy(debug_inf, "value should be a BYTES\r\n");
            return new_debug(debug_inf, cross_strlen(debug_inf));
        }
        outcome *= right->u_data.i_data;
    }
    return long_type(outcome);
}

void * original_charp(void * _left) {
    type * left = (type *)c_car(_left);
    type * outcome = primitive_empty;
    char* p = NULL;
    int len = 0;

    if (left->em == VAR) {
        p = (char *)(left->u_data.s_data);
        len = cross_strlen(p);
        if (len != ARABIC) {
            outcome = long_type(1);
        }
        else {
            outcome = long_type(0);
        }
    }
    else {
        outcome = long_type(0);
    }
    return outcome;
}

void * original_storagep(void * _left) {
    type * left = (type *) c_car(_left);
    type * outcome = primitive_empty;

    if (left->em == STORAGE) {
        outcome = long_type(1);
    }
    else {
        outcome = long_type(0);
    }
    return outcome;
}

void * original_debugp(void * _left) {
    type * left = (type *) c_car(_left);
    type * outcome = primitive_empty;

    if (left->em == DEBUG) {
        outcome = long_type(1);
    }
    else {
        outcome = long_type(0);
    }
    return outcome;
}

void * original_digitp(void * _left) {
    type * left = _left;
    type * outcome =
        ((type *) c_car(left))->em == BYTES ?
        long_type(1) : long_type(0);
    return outcome;
}

/*(dict system)*/
#define PREFIX_SIZE (sizeof(size_t))

void *zmalloc(size_t size);
void *zcalloc(size_t size);
void zfree(void *ptr);

static void zmalloc_oom(int size) {
    cross_fprintf(stderr, "zmalloc: Out of memory trying to allocate %zu bytes\n");
    fflush(stderr);
    abort();
}

void *zmalloc(size_t size) {
    void *ptr = cross_calloc(size+PREFIX_SIZE, sizeof(char));

    if (!ptr) zmalloc_oom(size);

    *((size_t*)ptr) = size;
    return (char*)ptr+PREFIX_SIZE;
}

void *zcalloc(size_t size) {
    void *ptr = cross_calloc(1, size+PREFIX_SIZE);

    if (!ptr) zmalloc_oom(size);

    *((size_t*)ptr) = size;
    return (char*)ptr+PREFIX_SIZE;
}

void zfree(void *ptr) {
    void *realptr;
    size_t oldsize;

    if (ptr == NULL) return;
    realptr = (char*)ptr-PREFIX_SIZE;
    oldsize = *((size_t*)realptr);
    cross_free(realptr);
}


#define DICT_OK 0
#define DICT_ERR 1

/* Unused arguments generate annoying warnings... */
#define DICT_NOTUSED(V) ((void) V)

typedef struct dictEntry {
    void *key;
    void *val;
    struct dictEntry *next;
} dictEntry;

typedef struct dictType {
    unsigned int (*hashFunction)(const void *key);
    void *(*keyDup)(void *privdata, const void *key);
    void *(*valDup)(void *privdata, const void *obj);
    int (*keyCompare)(void *privdata, const void *key1, const void *key2);
    void (*keyDestructor)(void *privdata, void *key);
    void (*valDestructor)(void *privdata, void *obj);
} dictType;

/* This is our hash table structure. Every dictionary has two of this as we
 * implement incremental rehashing, for the old to the new table. */
typedef struct dictht {
    dictEntry **table;
    unsigned long size;
    unsigned long sizemask;
    unsigned long used;
} dictht;

typedef struct dict {
    dictType *type;
    void *privdata;
    dictht ht[2];
    int rehashidx; /* rehashing not in progress if rehashidx == -1 */
    int iterators; /* number of iterators currently running */
} dict;

/* If safe is set to 1 this is a safe iteartor, that means, you can call
 * dictAdd, dictFind, and other functions against the dictionary even while
 * iterating. Otherwise it is a non safe iterator, and only dictNext()
 * should be called while iterating. */
typedef struct dictIterator {
    dict *d;
    int table, index, safe;
    dictEntry *entry, *nextEntry;
} dictIterator;

/* This is the initial size of every hash table */
#define DICT_HT_INITIAL_SIZE     4

/* ------------------------------- Macros ------------------------------------*/
#define dictFreeEntryVal(d, entry)    if ((d)->type->valDestructor)   (d)->type->valDestructor((d)->privdata, (entry)->val)

#define dictSetHashVal(d, entry, _val_) do {     if ((d)->type->valDup)      entry->val = (d)->type->valDup((d)->privdata, _val_);     else         entry->val = (_val_); } while(0)

#define dictFreeEntryKey(d, entry)     if ((d)->type->keyDestructor)         (d)->type->keyDestructor((d)->privdata, (entry)->key)

#define dictSetHashKey(d, entry, _key_) do {     if ((d)->type->keyDup)         entry->key = (d)->type->keyDup((d)->privdata, _key_);     else         entry->key = (_key_); } while(0)

#define dictCompareHashKeys(d, key1, key2)     (((d)->type->keyCompare) ?         (d)->type->keyCompare((d)->privdata, key1, key2) :         (key1) == (key2))

#define dictHashKey(d, key) (d)->type->hashFunction(key)

#define dictGetEntryKey(he) ((he)->key)
#define dictGetEntryVal(he) ((he)->val)
#define dictSlots(d) ((d)->ht[0].size+(d)->ht[1].size)
#define dictSize(d) ((d)->ht[0].used+(d)->ht[1].used)
#define dictIsRehashing(ht) ((ht)->rehashidx != -1)

/* API */
dict *dictCreate(dictType *type, void *privDataPtr);
int dictExpand(dict *d, unsigned long size);
int dictAdd(dict *d, void *key, void *val);
int dictReplace(dict *d, void *key, void *val);
int dictDelete(dict *d, const void *key);
int dictDeleteNoFree(dict *d, const void *key);
void dictRelease(dict *d);
dictEntry * dictFind(dict *d, const void *key);
void *dictFetchValue(dict *d, const void *key);
int dictResize(dict *d);
dictIterator *dictGetIterator(dict *d);
dictIterator *dictGetSafeIterator(dict *d);
dictEntry *dictNext(dictIterator *iter);
void dictReleaseIterator(dictIterator *iter);
dictEntry *dictGetRandomKey(dict *d);
void dictPrintStats(dict *d);
unsigned int dictGenHashFunction(const unsigned char *buf, int len);
unsigned int dictGenCaseHashFunction(const unsigned char *buf, int len);
void dictEmpty(dict *d);
void dictEnableResize(void);
void dictDisableResize(void);
int dictRehash(dict *d, int n);

static int dict_can_resize = 1;
static unsigned int dict_force_resize_ratio = 5;

/* -------------------------- private prototypes ---------------------------- */

static int _dictExpandIfNeeded(dict *ht);
static unsigned long _dictNextPower(unsigned long size);
static int _dictKeyIndex(dict *ht, const void *key);
static int _dictInit(dict *ht, dictType *type, void *privDataPtr);

/* -------------------------- hash functions -------------------------------- */

/* Thomas Wang's 32 bit Mix Function */
unsigned int dictIntHashFunction(unsigned int key)
{
    key += ~(key << 15);
    key ^=  (key >> 10);
    key +=  (key << 3);
    key ^=  (key >> 6);
    key += ~(key << 11);
    key ^=  (key >> 16);
    return key;
}

/* Identity hash function for integer keys */
unsigned int dictIdentityHashFunction(unsigned int key)
{
    return key;
}

/* Generic hash function (a popular one from Bernstein).
 * I tested a few and this was the best. */
unsigned int dictGenHashFunction(const unsigned char *buf, int len) {
    unsigned int hash = 5381;

    while (len--)
        hash = ((hash << 5) + hash) + (*buf++); /* hash * 33 + c */
    return hash;
}

/* And a case insensitive version */
unsigned int dictGenCaseHashFunction(const unsigned char *buf, int len) {
    unsigned int hash = 5381;

    while (len--)
        hash = ((hash << 5) + hash) + (cross_tolower(*buf++)); /* hash * 33 + c */
    return hash;
}

/* ----------------------------- API implementation ------------------------- */

/* Reset an hashtable already initialized with ht_init().
 * NOTE: This function should only called by ht_destroy(). */
static void _dictReset(dictht *ht)
{
    ht->table = NULL;
    ht->size = 0;
    ht->sizemask = 0;
    ht->used = 0;
}

/* Create a new hash table */
dict *dictCreate(dictType *type,
                 void *privDataPtr)
{
    dict *d = zmalloc(sizeof(*d));

    _dictInit(d,type,privDataPtr);
    return d;
}

/* Initialize the hash table */
int _dictInit(dict *d, dictType *type,
              void *privDataPtr)
{
    _dictReset(&d->ht[0]);
    _dictReset(&d->ht[1]);
    d->type = type;
    d->privdata = privDataPtr;
    d->rehashidx = -1;
    d->iterators = 0;
    return DICT_OK;
}

/* Resize the table to the minimal size that contains all the elements,
 * but with the invariant of a USER/BUCKETS ratio near to <= 1 */
int dictResize(dict *d)
{
    int minimal;

    if (!dict_can_resize || dictIsRehashing(d)) return DICT_ERR;
    minimal = d->ht[0].used;
    if (minimal < DICT_HT_INITIAL_SIZE)
        minimal = DICT_HT_INITIAL_SIZE;
    return dictExpand(d, minimal);
}

/* Expand or create the hashtable */
int dictExpand(dict *d, unsigned long size)
{
    dictht n; /* the new hashtable */
    unsigned long realsize = _dictNextPower(size);

    /* the size is invalid if it is smaller than the number of
     * elements already inside the hashtable */
    if (dictIsRehashing(d) || d->ht[0].used > size)
        return DICT_ERR;

    /* Allocate the new hashtable and initialize all pointers to NULL */
    n.size = realsize;
    n.sizemask = realsize-1;
    n.table = zcalloc(realsize*sizeof(dictEntry*));
    n.used = 0;

    /* Is this the first initialization? If so it's not really a rehashing
     * we just set the first hash table so that it can accept keys. */
    if (d->ht[0].table == NULL) {
        d->ht[0] = n;
        return DICT_OK;
    }

    /* Prepare a second hash table for incremental rehashing */
    d->ht[1] = n;
    d->rehashidx = 0;
    return DICT_OK;
}

/* Performs N steps of incremental rehashing. Returns 1 if there are still
 * keys to move from the old to the new hash table, otherwise 0 is returned.
 * Note that a rehashing step consists in moving a bucket (that may have more
 * thank one key as we use chaining) from the old to the new hash table. */
int dictRehash(dict *d, int n) {
    if (!dictIsRehashing(d)) return 0;

    while(n--) {
        dictEntry *de, *nextde;

        /* Check if we already rehashed the whole table... */
        if (d->ht[0].used == 0) {
            zfree(d->ht[0].table);
            d->ht[0] = d->ht[1];
            _dictReset(&d->ht[1]);
            d->rehashidx = -1;
            return 0;
        }

        /* Note that rehashidx can't overflow as we are sure there are more
         * elements because ht[0].used != 0 */
        while(d->ht[0].table[d->rehashidx] == NULL) d->rehashidx++;
        de = d->ht[0].table[d->rehashidx];
        /* Move all the keys in this bucket from the old to the new hash HT */
        while(de) {
            unsigned int h;

            nextde = de->next;
            /* Get the index in the new hash table */
            h = dictHashKey(d, de->key) & d->ht[1].sizemask;
            de->next = d->ht[1].table[h];
            d->ht[1].table[h] = de;
            d->ht[0].used--;
            d->ht[1].used++;
            de = nextde;
        }
        d->ht[0].table[d->rehashidx] = NULL;
        d->rehashidx++;
    }
    return 1;
}

/* This function performs just a step of rehashing, and only if there are
 * no safe iterators bound to our hash table. When we have iterators in the
 * middle of a rehashing we can't mess with the two hash tables otherwise
 * some element can be missed or duplicated.
 *
 * This function is called by common lookup or update operations in the
 * dictionary so that the hash table automatically migrates from H1 to H2
 * while it is actively used. */
static void _dictRehashStep(dict *d) {
    if (d->iterators == 0) dictRehash(d,1);
}

/* Add an element to the target hash table */
int dictAdd(dict *d, void *key, void *val)
{
    int index;
    dictEntry *entry;
    dictht *ht;

    if (dictIsRehashing(d)) _dictRehashStep(d);

    /* Get the index of the new element, or -1 if
     * the element already exists. */
    if ((index = _dictKeyIndex(d, key)) == -1)
        return DICT_ERR;

    /* Allocates the memory and stores key */
    ht = dictIsRehashing(d) ? &d->ht[1] : &d->ht[0];
    entry = zmalloc(sizeof(*entry));
    entry->next = ht->table[index];
    ht->table[index] = entry;
    ht->used++;

    /* Set the hash entry fields. */
    dictSetHashKey(d, entry, key);
    dictSetHashVal(d, entry, val);
    return DICT_OK;
}

/* Add an element, discarding the old if the key already exists.
 * Return 1 if the key was added from scratch, 0 if there was already an
 * element with such key and dictReplace() just performed a value update
 * operation. */
int dictReplace(dict *d, void *key, void *val)
{
    dictEntry *entry, auxentry;

    /* Try to add the element. If the key
     * does not exists dictAdd will suceed. */
    if (dictAdd(d, key, val) == DICT_OK)
        return 1;
    /* It already exists, get the entry */
    entry = dictFind(d, key);
    /* Free the old value and set the new one */
    /* Set the new value and free the old one. Note that it is important
     * to do that in this order, as the value may just be exactly the same
     * as the previous one. In this context, think to reference counting,
     * you want to increment (set), and then decrement (free), and not the
     * reverse. */
    auxentry = *entry;
    dictSetHashVal(d, entry, val);
    dictFreeEntryVal(d, &auxentry);
    return 0;
}

/* Search and remove an element */
static int dictGenericDelete(dict *d, const void *key, int nofree)
{
    unsigned int h, idx;
    dictEntry *he, *prevHe;
    int table;

    if (d->ht[0].size == 0) return DICT_ERR; /* d->ht[0].table is NULL */
    if (dictIsRehashing(d)) _dictRehashStep(d);
    h = dictHashKey(d, key);

    for (table = 0; table <= 1; table++) {
        idx = h & d->ht[table].sizemask;
        he = d->ht[table].table[idx];
        prevHe = NULL;
        while(he) {
            if (dictCompareHashKeys(d, key, he->key)) {
                /* Unlink the element from the list */
                if (prevHe)
                    prevHe->next = he->next;
                else
                    d->ht[table].table[idx] = he->next;
                if (!nofree) {
                    dictFreeEntryKey(d, he);
                    dictFreeEntryVal(d, he);
                }
                zfree(he);
                d->ht[table].used--;
                return DICT_OK;
            }
            prevHe = he;
            he = he->next;
        }
        if (!dictIsRehashing(d)) break;
    }
    return DICT_ERR; /* not found */
}

int dictDelete(dict *ht, const void *key) {
    return dictGenericDelete(ht,key,0);
}

int dictDeleteNoFree(dict *ht, const void *key) {
    return dictGenericDelete(ht,key,1);
}

/* Destroy an entire dictionary */
int _dictClear(dict *d, dictht *ht)
{
    unsigned long i;

    /* Free all the elements */
    for (i = 0; i < ht->size && ht->used > 0; i++) {
        dictEntry *he, *nextHe;

        if ((he = ht->table[i]) == NULL) continue;
        while(he) {
            nextHe = he->next;
            dictFreeEntryKey(d, he);
            dictFreeEntryVal(d, he);
            zfree(he);
            ht->used--;
            he = nextHe;
        }
    }
    /* Free the table and the allocated cache structure */
    zfree(ht->table);
    /* Re-initialize the table */
    _dictReset(ht);
    return DICT_OK; /* never fails */
}

/* Clear & Release the hash table */
void dictRelease(dict *d)
{
    _dictClear(d,&d->ht[0]);
    _dictClear(d,&d->ht[1]);
    zfree(d);
}

dictEntry *dictFind(dict *d, const void *key)
{
    dictEntry *he;
    unsigned int h, idx, table;

    if (d->ht[0].size == 0) return NULL; /* We don't have a table at all */
    if (dictIsRehashing(d)) _dictRehashStep(d);
    h = dictHashKey(d, key);
    for (table = 0; table <= 1; table++) {
        idx = h & d->ht[table].sizemask;
        he = d->ht[table].table[idx];
        while(he) {
            if (dictCompareHashKeys(d, key, he->key))
                return he;
            he = he->next;
        }
        if (!dictIsRehashing(d)) return NULL;
    }
    return NULL;
}

void *dictFetchValue(dict *d, const void *key) {
    dictEntry *he;

    he = dictFind(d,key);
    return he ? dictGetEntryVal(he) : NULL;
}

dictIterator *dictGetIterator(dict *d)
{
    dictIterator *iter = zmalloc(sizeof(*iter));

    iter->d = d;
    iter->table = 0;
    iter->index = -1;
    iter->safe = 0;
    iter->entry = NULL;
    iter->nextEntry = NULL;
    return iter;
}

dictIterator *dictGetSafeIterator(dict *d) {
    dictIterator *i = dictGetIterator(d);

    i->safe = 1;
    return i;
}

dictEntry *dictNext(dictIterator *iter)
{
    while (1) {
        if (iter->entry == NULL) {
            dictht *ht = &iter->d->ht[iter->table];
            if (iter->safe && iter->index == -1 && iter->table == 0)
                iter->d->iterators++;
            iter->index++;
            if (iter->index >= (signed) ht->size) {
                if (dictIsRehashing(iter->d) && iter->table == 0) {
                    iter->table++;
                    iter->index = 0;
                    ht = &iter->d->ht[1];
                } 
                else {
                    break;
                }
            }
            iter->entry = ht->table[iter->index];
        } 
        else {
            iter->entry = iter->nextEntry;
        }
        if (iter->entry) {
            /* We need to save the 'next' here, the iterator user
             * may delete the entry we are returning. */
            iter->nextEntry = iter->entry->next;
            return iter->entry;
        }
    }
    return NULL;
}

void dictReleaseIterator(dictIterator *iter)
{
    if (iter->safe && !(iter->index == -1 && iter->table == 0))
        iter->d->iterators--;
    zfree(iter);
}

/* Return a random entry from the hash table. Useful to
 * implement randomized algorithms */
dictEntry *dictGetRandomKey(dict *d)
{
    dictEntry *he, *orighe;
    unsigned int h;
    int listlen, listele;

    if (dictSize(d) == 0) return NULL;
    if (dictIsRehashing(d)) _dictRehashStep(d);
    if (dictIsRehashing(d)) {
        do {
            h = cross_rand() % (d->ht[0].size+d->ht[1].size);
            he = (h >= d->ht[0].size) ? d->ht[1].table[h - d->ht[0].size] :
                d->ht[0].table[h];
        } while(he == NULL);
    } 
    else {
        do {
            h = cross_rand() & d->ht[0].sizemask;
            he = d->ht[0].table[h];
        } while(he == NULL);
    }

    /* Now we found a non empty bucket, but it is a linked
     * list and we need to get a random element from the list.
     * The only sane way to do so is counting the elements and
     * select a random index. */
    listlen = 0;
    orighe = he;
    while(he) {
        he = he->next;
        listlen++;
    }
    listele = cross_rand() % listlen;
    he = orighe;
    while(listele--) he = he->next;
    return he;
}

/* ------------------------- private functions ------------------------------ */

/* Expand the hash table if needed */
static int _dictExpandIfNeeded(dict *d)
{
    /* Incremental rehashing already in progress. Return. */
    if (dictIsRehashing(d)) return DICT_OK;

    /* If the hash table is empty expand it to the intial size. */
    if (d->ht[0].size == 0) return dictExpand(d, DICT_HT_INITIAL_SIZE);

    /* If we reached the 1:1 ratio, and we are allowed to resize the hash
     * table (global setting) or we should avoid it but the ratio between
     * elements/buckets is over the "safe" threshold, we resize doubling
     * the number of buckets. */
    if (d->ht[0].used >= d->ht[0].size &&
        (dict_can_resize ||
         d->ht[0].used/d->ht[0].size > dict_force_resize_ratio))
        {
            return dictExpand(d, ((d->ht[0].size > d->ht[0].used) ?
                                  d->ht[0].size : d->ht[0].used)*2);
        }
    return DICT_OK;
}

/* Our hash table capability is a power of two */
static unsigned long _dictNextPower(unsigned long size)
{
    unsigned long i = DICT_HT_INITIAL_SIZE;

    /*    if (size >= BYTES_MAX) return BYTES_MAX;*/
    while(1) {
        if (i >= size)
            return i;
        i *= 2;
    }
}

/* Returns the index of a free slot that can be populated with
 * an hash entry for the given 'key'.
 * If the key already exists, -1 is returned.
 *
 * Note that if we are in the process of rehashing the hash table, the
 * index is always returned in the context of the second (new) hash table. */
static int _dictKeyIndex(dict *d, const void *key)
{
    unsigned int h, idx, table;
    dictEntry *he;

    /* Expand the hashtable if needed */
    if (_dictExpandIfNeeded(d) == DICT_ERR)
        return -1;
    /* Compute the key hash value */
    h = dictHashKey(d, key);
    for (table = 0; table <= 1; table++) {
        idx = h & d->ht[table].sizemask;
        /* Search if this slot does not already contain the given key */
        he = d->ht[table].table[idx];
        while(he) {
            if (dictCompareHashKeys(d, key, he->key))
                return -1;
            he = he->next;
        }
        if (!dictIsRehashing(d)) break;
    }
    return idx;
}

void dictEmpty(dict *d) {
    _dictClear(d,&d->ht[0]);
    _dictClear(d,&d->ht[1]);
    d->rehashidx = -1;
    d->iterators = 0;
}

#define DICT_STATS_VECTLEN 50
static void _dictPrintStatsHt(dictht *ht) {
    unsigned long i, slots = 0, chainlen, maxchainlen = 0;
    unsigned long totchainlen = 0;
    unsigned long clvector[DICT_STATS_VECTLEN];

    if (ht->used == 0) {
        printf("No stats available for empty dictionaries\n");
        return;
    }

    for (i = 0; i < DICT_STATS_VECTLEN; i++) clvector[i] = 0;
    for (i = 0; i < ht->size; i++) {
        dictEntry *he;

        if (ht->table[i] == NULL) {
            clvector[0]++;
            continue;
        }
        slots++;
        /* For each hash entry on this slot... */
        chainlen = 0;
        he = ht->table[i];
        while(he) {
            chainlen++;
            he = he->next;
        }
        clvector[(chainlen < DICT_STATS_VECTLEN) ? chainlen : (DICT_STATS_VECTLEN-1)]++;
        if (chainlen > maxchainlen) maxchainlen = chainlen;
        totchainlen += chainlen;
    }
    printf("Hash table stats:\n");
    printf(" table size: %ld\n", ht->size);
    printf(" number of elements: %ld\n", ht->used);
    printf(" different slots: %ld\n", slots);
    printf(" max chain length: %ld\n", maxchainlen);
    printf(" avg chain length (counted): %.02f\n", (float)totchainlen/slots);
    printf(" avg chain length (computed): %.02f\n", (float)ht->used/slots);
    printf(" Chain length distribution:\n");
    for (i = 0; i < DICT_STATS_VECTLEN-1; i++) {
        if (clvector[i] == 0) continue;
        printf("   %s%ld: %ld (%.02f%%)\n",(i == DICT_STATS_VECTLEN-1)?">= ":"", i, clvector[i], ((float)clvector[i]/ht->size)*100);
    }
}

void dictPrintStats(dict *d) {
    _dictPrintStatsHt(&d->ht[0]);
    if (dictIsRehashing(d)) {
        printf("-- Rehashing into ht[1]:\n");
        _dictPrintStatsHt(&d->ht[1]);
    }
}

void dictEnableResize(void) {
    dict_can_resize = 1;
}

void dictDisableResize(void) {
    dict_can_resize = 0;
}



/* The following are just example hash table types implementations.
 * Not useful for Redis so they are commented out.
 */

/* ----------------------- StringCopy Hash Table Type ------------------------*/

static unsigned int _dictStringCopyHTHashFunction(const void *key)
{
    return dictGenHashFunction(key, strlen(key));
}

static void *_dictStringDup(void *privdata, const void *key)
{
    int len = strlen(key);
    char *copy = zmalloc(len+1);
    DICT_NOTUSED(privdata);

    memcpy(copy, key, len);
    copy[len] = '\0';
    return copy;
}

static int _dictStringCopyHTKeyCompare(void *privdata, const void *key1,
                                       const void *key2)
{
    DICT_NOTUSED(privdata);

    return strcmp(key1, key2) == 0;
}

static void _dictStringDestructor(void *privdata, void *key)
{
    DICT_NOTUSED(privdata);

    zfree(key);
}

static void _dictValDestructor(void *privdata, void *key)
{
    DICT_NOTUSED(privdata);

    gc(key);
}

dictType dictTypeHeapStringCopyKey = {
    _dictStringCopyHTHashFunction, /* hash function */
    _dictStringDup,                /* key dup */
    NULL,                          /* val dup */
    _dictStringCopyHTKeyCompare,   /* key compare */
    _dictStringDestructor,         /* key destructor */
    _dictValDestructor                           /* val destructor */
};

/* This is like StringCopy but does not auto-duplicate the key.
 * It's used for intepreter's shared strings. */
dictType dictTypeHeapStrings = {
    _dictStringCopyHTHashFunction, /* hash function */
    NULL,                          /* key dup */
    NULL,                          /* val dup */
    _dictStringCopyHTKeyCompare,   /* key compare */
    _dictStringDestructor,         /* key destructor */
    NULL                           /* val destructor */
};

/* This is like StringCopy but also automatically handle dynamic
 * allocated C strings as values. */
dictType dictTypeHeapStringCopyKeyValue = {
    _dictStringCopyHTHashFunction, /* hash function */
    _dictStringDup,                /* key dup */
    _dictStringDup,                /* val dup */
    _dictStringCopyHTKeyCompare,   /* key compare */
    _dictStringDestructor,         /* key destructor */
    _dictStringDestructor,         /* val destructor */
};

type* hashTypeGet(type *entry, type *key, unsigned char **v, unsigned int *vlen) {
    char * key_string = NULL;
    char debug_inf[256] = "\0";
    dictEntry* de;

    if(key->em == VAR){
        key_string = key->u_data.s_data;
    }
    else if(key->em == STORAGE){
        key_string = key->u_data.a_storage + sizeof(int);
    }
    else {
        cross_snprintf(debug_inf, 256, "hashTypeGet key value should be a VAR or STORAGE\r\n");
        return new_debug(debug_inf, strlen(debug_inf));
    }

    de = dictFind((dict*)(entry->u_data.i_data), key_string);
    if (de == NULL) {
        return NULL;
    }
    return dictGetEntryVal(de);
}

/* Add an element, discard the old if the key already exists.
 * Return 0 on insert and 1 on update. */
void* hashTypeSet(type * entry, type *key, type *value) {
    int update = 0;
    char * key_string = NULL;
    char debug_inf[256] = "\0";

    if(key->em == VAR){
        key_string = key->u_data.s_data;
    }
    else if(key->em == STORAGE){
        key_string = key->u_data.a_storage + sizeof(int);
    }
    else{
        gc(value);
        cross_snprintf(debug_inf, 256, "hashTypeSet 1st should be a VAR or STORAGE\r\n");
        return new_debug(debug_inf, strlen(debug_inf));
    }

    if (dictReplace((dict*)(entry->u_data.i_data), key_string, value)) {
    } 
    else {
        update = 1;
    }
    return primitive_empty;
}

/* Delete an element from a hash.
 * Return 1 on deleted and 0 on not found. */
#define REDIS_HT_MINFILL        10      /* Minimal hash table fill 10% */

int htNeedsResize(dict *dict) {
    long size, used;

    size = dictSlots(dict);
    used = dictSize(dict);
    return (size && used && size > DICT_HT_INITIAL_SIZE &&
            (used*100/size < REDIS_HT_MINFILL));
}

int hashTypeDelete(type * entry, type * key) {
    int deleted = 0;
    char * key_string = NULL;
    if(key->em == VAR){
        key_string = key->u_data.s_data;
    }
    else if(key->em == STORAGE){
        key_string = key->u_data.a_storage + sizeof(int);
    }
    else{
    }

    deleted = dictDelete((dict*)entry->u_data.i_data, key_string) == DICT_OK;
    /* Always check if the dictionary needs a resize after a delete. */
    if (deleted && htNeedsResize((dict*)entry->u_data.i_data)) dictResize((dict*)entry->u_data.i_data);

    return deleted;
}


unsigned long hashTypeLength(type * entry) {
    return dictSize((dict*)entry->u_data.i_data);
}

type * lookupKey(dict  * dic, char  *key) {
    dictEntry *de = dictFind(dic, key);
    if (de) {
        type *val = dictGetEntryVal(de);
        return  val;
    } 
    else {
        return NULL;
    }
}

type *hashTypeLookupWriteOrCreate(type  * dic, void *key) {
    type * entry = lookupKey((dict*)dic->u_data.i_data, key);
    if (entry == NULL) {
        entry = new_object();
        entry->em = HASH;
        entry->u_data.i_data = (long)dictCreate(&dictTypeHeapStringCopyKey,NULL);
        dictAdd((dict*)dic->u_data.i_data, key, entry);
    } 
    else {
    }
    return entry;
}

unsigned int dictSdsHash(const void *key) {
    return dictGenHashFunction((unsigned char*)key, strlen((char*)key));
}

int dictSdsKeyCompare(void *privdata, const void *key1,
                      const void *key2)
{
    int l1,l2;
    DICT_NOTUSED(privdata);

    l1 = strlen(key1);
    l2 = strlen(key2);
    if (l1 != l2) return 0;
    return memcmp(key1, key2, l1) == 0;
}

void dictSdsDestructor(void *privdata, void *val)
{
    DICT_NOTUSED(privdata);
}

void dictRedisObjectDestructor(void *privdata, void *val)
{
    DICT_NOTUSED(privdata);

    if (val == NULL) return; /* Values of swapped out keys as set to NULL */
}

dictType copyValueDictType = {
    dictSdsHash,                /* hash function */
    NULL,                       /* key dup */
    NULL,                       /* val dup */
    dictSdsKeyCompare,          /* key compare */
    dictSdsDestructor,          /* key destructor */
    dictRedisObjectDestructor   /* val destructor */
};


void * original_dcreate(void  * _left) {
    type   * outcome = new_object();
    outcome->em = DICTORY;
    outcome->u_data.i_data = (long)dictCreate(&copyValueDictType, NULL);
    return outcome;    
}

void * original_dset(void  * _left) {
    type * dic = c_car(_left);
    type * key = c_cadr(_left);
    type * second = c_car(c_cddr(_left));
    type * third = c_cadr (c_cddr(_left));
    type * outcome = primitive_empty;
    type * table = primitive_empty;
    char debug_inf[256] = "\0";
    char * key_string = NULL;

    if (dic->em != DICTORY) {
        cross_snprintf(debug_inf, 256, "1st should be a DICTORY\r\n");
        return new_debug(debug_inf, strlen(debug_inf));
    }

    if(key->em == VAR){
        key_string = key->u_data.s_data;
    }
    else if(key->em == STORAGE){
        key_string = key->u_data.a_storage + sizeof(int);
    }
    else {
        cross_snprintf(debug_inf, 256, "2st should be a VAR or STORAGE\r\n");
        return new_debug(debug_inf, strlen(debug_inf));
    }

    if ((table = hashTypeLookupWriteOrCreate(dic, key_string)) == NULL) {
        return outcome;    
    }

    outcome = hashTypeSet(table, second, c_normal_copy(third));
    if (outcome->em == DEBUG) {
        cross_snprintf(debug_inf, 1024, "call subprocess error:\r\n%s", outcome->u_data.a_storage + sizeof(int));
        gc_atom(outcome);
        return new_debug(debug_inf, strlen(debug_inf));
    }
 
    outcome = c_normal_copy(dic);
    return outcome;    
}

void * original_dget(void  * _left) {
    type * dic = c_car(_left);
    type * key = c_cadr(_left);
    type * second = c_caddr(_left);
    type * entry = NULL;
    type * outcome = primitive_empty;
    unsigned char *v;
    unsigned int vlen;
    char debug_inf[256] = "\0";
    char * key_string = NULL;

    if (dic->em != DICTORY) {
        cross_snprintf(debug_inf, 256, "1st should be a DICTORY\r\n");
        return new_debug(debug_inf, strlen(debug_inf));
    }

    if(key->em == VAR){
        key_string = key->u_data.s_data;
    }
    else if(key->em == STORAGE){
        key_string = key->u_data.a_storage + sizeof(int);
    }
    else{
        cross_snprintf(debug_inf, 256, "2st should be a VAR or STORAGE\r\n");
        return new_debug(debug_inf, strlen(debug_inf));
    }

    if ((entry = lookupKey((dict*)dic->u_data.i_data, key_string)) == NULL ||
        entry->em != HASH) {
        return outcome;
    }

    outcome = hashTypeGet(entry, second, &v, &vlen);
    if (outcome == NULL) {
        return primitive_empty;
    }

    if (outcome->em == DEBUG) {
        cross_snprintf(debug_inf, 1024, "call subprocess error:\r\n%s", outcome->u_data.a_storage + sizeof(int));
        gc_atom(outcome);
        return new_debug(debug_inf, strlen(debug_inf));
    }
    return c_normal_copy(outcome);
}

void * original_ddel(void  * _left) {
    type * dic = c_car(_left);
    type * key = c_cadr(_left);
    type * second = c_cddr(_left);
    type * entry = NULL;
    int deleted = 0;
    char * key_string = NULL;
    char debug_inf[256] = "\0";

    if (dic->em != DICTORY) {
        cross_snprintf(debug_inf, 256, "1st should be a DICTORY\r\n");
        return new_debug(debug_inf, strlen(debug_inf));
    }

    if(key->em == VAR){
        key_string = key->u_data.s_data;
    }
    else if(key->em == STORAGE){
        key_string = key->u_data.a_storage + sizeof(int);
    }
    else{
        cross_snprintf(debug_inf, 256, "2st should be a VAR or STORAGE\r\n");
        return new_debug(debug_inf, strlen(debug_inf));
    }

    if ((entry = lookupKey((dict*)dic->u_data.i_data, key_string)) == NULL ||
        entry->em == HASH) {
        return primitive_empty;
    }

    while(1){
        if(second->em == EMPTY)break;
        key = c_car(second);
        if (hashTypeDelete(entry, key)) {
            deleted++;
            if (hashTypeLength(entry) == 0) {
                dictDelete((dict*)dic->u_data.i_data, key);
            }
        }
        second = c_cdr(second);
    }
    return long_type(deleted);
}

void * original_dlen(void  * _left) {
    type * dic = c_car(_left);
    type * key = c_cadr(_left);
    type * entry = NULL;
    type * outcome = NULL;
    char * key_string = NULL;
    char debug_inf[256] = "\0";

    if (dic->em != DICTORY) {
        cross_snprintf(debug_inf, 256, "1st should be a DICTORY\r\n");
        return new_debug(debug_inf, strlen(debug_inf));
    }

    if(key->em == VAR){
        key_string = key->u_data.s_data;
    }
    else if(key->em == STORAGE){
        key_string = key->u_data.a_storage + sizeof(int);
    }
    else {
        cross_snprintf(debug_inf, 256, "2st should be a VAR or STORAGE\r\n");
        return new_debug(debug_inf, strlen(debug_inf));
    }

    if ((entry = lookupKey((dict*)dic->u_data.i_data, key_string)) == NULL ||
        entry->em == HASH) {
        return primitive_empty;
    }

    outcome = long_type(hashTypeLength(entry));
    return outcome;
}

typedef struct listNode {
    struct listNode *prev;
    struct listNode *next;
    void *value;
} listNode;

typedef struct listIter {
    listNode *next;
    int direction;
} listIter;

typedef struct list {
    listNode *head;
    listNode *tail;
    void *(*dup)(void *ptr);
    void (*free)(void *ptr);
    int (*match)(void *ptr, void *key);
    unsigned int len;
} list;

/* Functions implemented as macros */
#define listLength(l) ((l)->len)
#define listFirst(l) ((l)->head)
#define listLast(l) ((l)->tail)
#define listPrevNode(n) ((n)->prev)
#define listNextNode(n) ((n)->next)
#define listNodeValue(n) ((n)->value)

#define listSetDupMethod(l,m) ((l)->dup = (m))
#define listSetFreeMethod(l,m) ((l)->free = (m))
#define listSetMatchMethod(l,m) ((l)->match = (m))

#define listGetDupMethod(l) ((l)->dup)
#define listGetFree(l) ((l)->free)
#define listGetMatchMethod(l) ((l)->match)

list *listCreate(void)
{
    struct list *list;

    if ((list = zmalloc(sizeof(*list))) == NULL)
        return NULL;
    list->head = list->tail = NULL;
    list->len = 0;
    list->dup = NULL;
    list->free = NULL;
    list->match = NULL;
    return list;
}

/* Free the whole list.
 *
 * This function can't fail. */
void listRelease(list *list)
{
    unsigned int len;
    listNode *current, *next;

    current = list->head;
    len = list->len;
    while(len--) {
        next = current->next;
        if (list->free) list->free(current->value);
        zfree(current);
        current = next;
    }
    zfree(list);
}

/* Add a new node to the list, to head, contaning the specified 'value'
 * pointer as value.
 *
 * On error, NULL is returned and no operation is performed (i.e. the
 * list remains unaltered).
 * On success the 'list' pointer you pass to the function is returned. */
list *listAddNodeHead(list *list, void *value)
{
    listNode *node;

    if ((node = zmalloc(sizeof(*node))) == NULL)
        return NULL;
    node->value = value;
    if (list->len == 0) {
        list->head = list->tail = node;
        node->prev = node->next = NULL;
    } 
    else {
        node->prev = NULL;
        node->next = list->head;
        list->head->prev = node;
        list->head = node;
    }
    list->len++;
    return list;
}

/* Add a new node to the list, to tail, contaning the specified 'value'
 * pointer as value.
 *
 * On error, NULL is returned and no operation is performed (i.e. the
 * list remains unaltered).
 * On success the 'list' pointer you pass to the function is returned. */
list *listAddNodeTail(list *list, void *value)
{
    listNode *node;

    if ((node = zmalloc(sizeof(*node))) == NULL)
        return NULL;
    node->value = value;
    if (list->len == 0) {
        list->head = list->tail = node;
        node->prev = node->next = NULL;
    } 
    else {
        node->prev = list->tail;
        node->next = NULL;
        list->tail->next = node;
        list->tail = node;
    }
    list->len++;
    return list;
}

list *listInsertNode(list *list, listNode *old_node, void *value, int after) {
    listNode *node;

    if ((node = zmalloc(sizeof(*node))) == NULL)
        return NULL;
    node->value = value;
    if (after) {
        node->prev = old_node;
        node->next = old_node->next;
        if (list->tail == old_node) {
            list->tail = node;
        }
    } 
    else {
        node->next = old_node;
        node->prev = old_node->prev;
        if (list->head == old_node) {
            list->head = node;
        }
    }
    if (node->prev != NULL) {
        node->prev->next = node;
    }
    if (node->next != NULL) {
        node->next->prev = node;
    }
    list->len++;
    return list;
}

/* Remove the specified node from the specified list.
 * It's up to the caller to free the private value of the node.
 *
 * This function can't fail. */
void listDelNode(list *list, listNode *node)
{
    if (node->prev)
        node->prev->next = node->next;
    else
        list->head = node->next;
    if (node->next)
        node->next->prev = node->prev;
    else
        list->tail = node->prev;
    if (list->free) list->free(node->value);
    zfree(node);
    list->len--;
}

/* Returns a list iterator 'iter'. After the initialization every
 * call to listNext() will return the next element of the list.
 *
 * This function can't fail. */
#define AL_START_HEAD 0
#define AL_START_TAIL 1
listIter *listGetIterator(list *list, int direction)
{
    listIter *iter;
    
    if ((iter = zmalloc(sizeof(*iter))) == NULL) return NULL;
    if (direction == AL_START_HEAD)
        iter->next = list->head;
    else
        iter->next = list->tail;
    iter->direction = direction;
    return iter;
}

/* Release the iterator memory */
void listReleaseIterator(listIter *iter) {
    zfree(iter);
}

/* Create an iterator in the list private iterator structure */
void listRewind(list *list, listIter *li) {
    li->next = list->head;
    li->direction = AL_START_HEAD;
}

void listRewindTail(list *list, listIter *li) {
    li->next = list->tail;
    li->direction = AL_START_TAIL;
}

/* Return the next element of an iterator.
 * It's valid to remove the currently returned element using
 * listDelNode(), but not to remove other elements.
 *
 * The function returns a pointer to the next element of the list,
 * or NULL if there are no more elements, so the classical usage patter
 * is:
 *
 * iter = listGetIterator(list,<direction>);
 * while ((node = listNext(iter)) != NULL) {
 *     doSomethingWith(listNodeValue(node));
 * }
 *
 * */
listNode *listNext(listIter *iter)
{
    listNode *current = iter->next;

    if (current != NULL) {
        if (iter->direction == AL_START_HEAD)
            iter->next = current->next;
        else
            iter->next = current->prev;
    }
    return current;
}

/* Duplicate the whole list. On out of memory NULL is returned.
 * On success a copy of the original list is returned.
 *
 * The 'Dup' method set with listSetDupMethod() function is used
 * to copy the node value. Otherwise the same pointer value of
 * the original node is used as value of the copied node.
 *
 * The original list both on success or error is never modified. */
list *listDup(list *orig)
{
    list *copy;
    listIter *iter;
    listNode *node;

    if ((copy = listCreate()) == NULL)
        return NULL;
    copy->dup = orig->dup;
    copy->free = orig->free;
    copy->match = orig->match;
    iter = listGetIterator(orig, AL_START_HEAD);
    while((node = listNext(iter)) != NULL) {
        void *value;

        if (copy->dup) {
            value = copy->dup(node->value);
            if (value == NULL) {
                listRelease(copy);
                listReleaseIterator(iter);
                return NULL;
            }
        } else
            value = node->value;
        if (listAddNodeTail(copy, value) == NULL) {
            listRelease(copy);
            listReleaseIterator(iter);
            return NULL;
        }
    }
    listReleaseIterator(iter);
    return copy;
}

/* Search the list for a node matching a given key.
 * The match is performed using the 'match' method
 * set with listSetMatchMethod(). If no 'match' method
 * is set, the 'value' pointer of every node is directly
 * compared with the 'key' pointer.
 *
 * On success the first matching node pointer is returned
 * (search starts from head). If no matching node exists
 * NULL is returned. */
listNode *listSearchKey(list *list, void *key)
{
    listIter *iter;
    listNode *node;

    iter = listGetIterator(list, AL_START_HEAD);
    while((node = listNext(iter)) != NULL) {
        if (list->match) {
            if (list->match(node->value, key)) {
                listReleaseIterator(iter);
                return node;
            }
        } 
        else {
            if (key == node->value) {
                listReleaseIterator(iter);
                return node;
            }
        }
    }
    listReleaseIterator(iter);
    return NULL;
}

/* Return the element at the specified zero-based index
 * where 0 is the head, 1 is the element next to head
 * and so on. Negative integers are used in order to count
 * from the tail, -1 is the last element, -2 the penultimante
 * and so on. If the index is out of range NULL is returned. */
listNode *listIndex(list *list, int index) {
    listNode *n;

    if (index < 0) {
        index = (-index)-1;
        n = list->tail;
        while(index-- && n) n = n->prev;
    } 
    else {
        n = list->head;
        while(index-- && n) n = n->next;
    }
    return n;
}

#define REDIS_HEAD 0
#define REDIS_TAIL 1
void listTypePush(type *subject, type *value, int where) {
    if (where == REDIS_HEAD) {
        listAddNodeHead((list*)subject->u_data.i_data, value);
    } 
    else {
        listAddNodeTail((list*)subject->u_data.i_data, value);
    }
}

type *listTypePop(type *subject, int where) {
    type* value = NULL;
    list* list_content = (list*)subject->u_data.i_data;
    listNode* ln;
    if (where == REDIS_HEAD) {
        ln = listFirst(list_content);
    } 
    else {
        ln = listLast(list_content);
    }
    if (ln != NULL) {
        value = listNodeValue(ln);
        listDelNode(list_content, ln);
    }
    return value;
}

unsigned long listTypeLength(type *subject) {
    return listLength((list*)subject->u_data.i_data);
}


type *listTypeLookupWriteOrCreate(type  * dic, void *key) {
    char* key_mem = NULL;
    type* entry = lookupKey((dict*)dic->u_data.i_data, key);
    if (entry == NULL) {
        entry = new_object();
        entry->em = REDISLIST;
        entry->u_data.i_data = (long)listCreate();
        key_mem = (char * )cross_calloc(1, strlen(key) + 1);
        strcpy(key_mem, key);
        dictAdd((dict*)dic->u_data.i_data, key_mem, entry);
    }
    return entry;
}

void *  pushGenericCommand(void  *_left, int where) {
    type* dic = c_car(_left);
    type* first = c_cadr(_left);
    type* second = c_car(c_cddr(_left));
    type* outcome = NULL;
    type* entry;
    char* key = NULL;
    char debug_inf[256] = "\0";

    if (dic->em != DICTORY) {
        cross_snprintf(debug_inf, 256, "pushGenericCommand 1st should be a DICTORY\r\n");
        gc(_left);
        return new_debug(debug_inf, strlen(debug_inf));
    }

    if(first->em == VAR){
        key = first->u_data.s_data;
    }
    else if(first->em == STORAGE){
        key = first->u_data.a_storage + sizeof(int);
    }
    else{
        cross_snprintf(debug_inf, 256, "pushGenericCommand 2st should be a VAR or STORAGE\r\n");
        gc(_left);
        return new_debug(debug_inf, strlen(debug_inf));
    }

    if ((entry = listTypeLookupWriteOrCreate(dic, key)) == NULL){
        gc(_left);
        return primitive_empty;
    }

    listTypePush(entry, c_normal_copy(second), where);
    outcome = c_normal_copy(second);
    return outcome;    
}

void * original_rpush(void  *_left) {
    char debug_inf[256] = "\0";
    type* outcome = pushGenericCommand(_left, REDIS_TAIL);
    if (outcome->em == DEBUG) {
        cross_snprintf(debug_inf, 1024, "call subprocess error:%s", outcome->u_data.a_storage + sizeof(int));
        gc_atom(outcome);
        return new_debug(debug_inf, strlen(debug_inf));
    }
    return outcome;
}

void * original_lpush(void  *_left) {
    char debug_inf[256] = "\0";
    type* outcome = pushGenericCommand(_left, REDIS_HEAD);
    if (outcome->em == DEBUG) {
        cross_snprintf(debug_inf, 1024, "call subprocess error:%s", outcome->u_data.a_storage + sizeof(int));
        gc_atom(outcome);
        return new_debug(debug_inf, strlen(debug_inf));
    }
    return outcome;
}

void* original_llen(void *_left) {
    type* dic = c_car(_left);
    type* key = c_cadr(_left);
    type* entry = NULL;
    type* outcome = NULL;
    char* key_string = NULL;
    char debug_inf[256] = "\0";

    if (dic->em != DICTORY) {
        cross_snprintf(debug_inf, 256, "1st should be a DICTORY\r\n");
        return new_debug(debug_inf, strlen(debug_inf));
    }

    if(key->em == VAR){
        key_string = key->u_data.s_data;
    }
    else if(key->em == STORAGE){
        key_string = key->u_data.a_storage + sizeof(int);
    }
    else {
        cross_snprintf(debug_inf, 256, "2st should be a VAR or STORAGE\r\n");
        return new_debug(debug_inf, strlen(debug_inf));
    }

    if ((entry = lookupKey((dict*)dic->u_data.i_data, key_string)) == NULL ||
        entry->em == REDISLIST) {
        return primitive_empty;
    }

    outcome = long_type(listTypeLength(entry));
    return outcome;
}

void* original_lrem(void* _left){
    type* dic = c_car(_left);
    type* first = c_cadr(_left);
    type* second = c_car(c_cddr(_left));
    type* outcome = NULL;
    type* entry;
    char* key = NULL;
    int start = 0;
    int end = 0;
    int rangelen = 0, llen = 0;
    char debug_inf[256] = "\0";
    listNode *ln;

    if (dic->em != DICTORY) {
        cross_snprintf(debug_inf, 256, "1st should be a DICTORY\r\n");
        return new_debug(debug_inf, strlen(debug_inf));
    }

    if(first->em == VAR){
        key = first->u_data.s_data;
    }
    else if(first->em == STORAGE){
        key = first->u_data.a_storage + sizeof(int);
    }
    else{
        cross_snprintf(debug_inf, 256, "2st should be a VAR or STORAGE\r\n");
        return new_debug(debug_inf, strlen(debug_inf));
    }

    if ((entry = listTypeLookupWriteOrCreate(dic, key)) == NULL){
        return primitive_empty;
    }

    llen = listTypeLength(entry);
    start = 0;
    end = llen - 1;
    rangelen = (end - start) + 1;

    ln = listIndex((list*)entry->u_data.i_data, start);
    while(rangelen--) {
        if(second->em == BYTES){
            if(((type*)ln->value)->u_data.i_data == second->u_data.i_data){
                listDelNode((list*)entry->u_data.i_data, ln);
            }
        }
        else if(second->em == NET){
            if(((type*)ln->value)->u_data.i_data == second->u_data.i_data){
                listDelNode((list*)entry->u_data.i_data, ln);
            }
        }
        else if(second->em == VAR){
            if(!strcmp( ((type*)ln->value)->u_data.s_data, second->u_data.s_data)){
                listDelNode((list*)entry->u_data.i_data, ln);
            }
        }
        else {
        }
        ln = ln->next;
    }

    outcome = c_normal_copy(second);
    return outcome;    
}

void  * original_lindex(type  *_left) {
    type* dic = c_car(_left);
    type* key = c_cadr(_left);
    type* second = c_car(c_cddr(_left));
    type* outcome = primitive_empty;

    type *obj;
    int index = second->u_data.i_data;
    listNode* ln;

    char * key_string = NULL;
    if(key->em == VAR){
        key_string = key->u_data.s_data;
    }
    else if(key->em == STORAGE){
        key_string = key->u_data.a_storage + sizeof(int);
    }
    else{
    }

    if ((obj = listTypeLookupWriteOrCreate(dic, key_string)) == NULL) {
        return primitive_empty;
    }

    ln = listIndex((list*)dic->u_data.i_data, index);
    if (ln == NULL) {
    } 
    else {
        outcome = (type*)listNodeValue(ln);
    }
    return outcome;    
}

void * original_lset(type *_left) {
    type* dic = c_car(_left);
    type* key = c_cadr(_left);
    type* second = c_car(c_cddr(_left));
    type* third = c_cadr(c_cddr(_left));
    type* outcome = primitive_empty;
    type* entry = NULL;
    char* key_string = NULL;
    int index = second->u_data.i_data;
    listNode* ln;

    if(key->em == VAR){
        key_string = key->u_data.s_data;
    }
    else if(key->em == STORAGE){
        key_string = key->u_data.a_storage + sizeof(int);
    }
    else{
    }

    if ((entry = listTypeLookupWriteOrCreate(dic, key_string)) == NULL){
        return primitive_empty;
    }

    ln = listIndex((list*)dic->u_data.i_data, index);
    if (ln != NULL) {
        gc((type*)listNodeValue(ln));
        listNodeValue(ln) = third;
    }

    return outcome;    
}

void * popGenericCommand(type  * _left, int where) {
    type* dic = c_car(_left);
    type* first = c_cadr(_left);
    type* outcome = NULL;
    type* dic_entry = NULL;
    char* key = NULL;
    char debug_inf[256] = "\0";

    if (dic->em != DICTORY) {
        cross_snprintf(debug_inf, 256, "popGenericCommand 1st should be a DICTORY\r\n");
        return new_debug(debug_inf, strlen(debug_inf));
    }

    if(first->em == VAR){
        key = first->u_data.s_data;
    }
    else if(first->em == STORAGE){
        key = first->u_data.a_storage + sizeof(int);
    }
    else{
        cross_snprintf(debug_inf, 256, "popGenericCommand 2st should be a VAR or STORAGE\r\n");
        return new_debug(debug_inf, strlen(debug_inf));
    }

    if ((dic_entry = listTypeLookupWriteOrCreate(dic, key)) == NULL){
        return primitive_empty;
    }

    outcome = listTypePop(dic_entry, where);
    if (outcome == NULL) {
        outcome = primitive_empty;
    } 
    else {
        if (listTypeLength(dic_entry) == 0) {
            dictDelete((dict*)dic->u_data.i_data, key);
            gc_atom(dic_entry);
        }
    }
    return outcome;
}

void * original_lpop(void  *_left) {
    char debug_inf[256] = "\0";
    type* outcome = popGenericCommand(_left, REDIS_HEAD);
    if (outcome->em == DEBUG) {
        cross_snprintf(debug_inf, 1024, "call subprocess error:%s", outcome->u_data.a_storage + sizeof(int));
        gc_atom(outcome);
        return new_debug(debug_inf, strlen(debug_inf));
    }
    return outcome;
}

void * original_rpop(void  *_left) {
    char debug_inf[256] = "\0";
    type* outcome = popGenericCommand(_left, REDIS_TAIL);
    if (outcome->em == DEBUG) {
        cross_snprintf(debug_inf, 1024, "call subprocess error:%s", outcome->u_data.a_storage + sizeof(int));
        gc_atom(outcome);
        return new_debug(debug_inf, strlen(debug_inf));
    }
    return outcome;
}

void * original_lrange(void *_left) {
    type* dic = c_car(_left);
    type* first = c_cadr(_left);
    type* left = c_car(c_cddr(_left));
    type* right = c_cadr(c_cddr(_left));
    type* outcome = primitive_empty;
    char* key = NULL;
    int start = 0;
    int end = 0;
    int llen;
    int rangelen;
    char debug_inf[256] = "\0";
    type * entry = NULL;
    listNode *ln;

    if (dic->em != DICTORY) {
        cross_snprintf(debug_inf, 256, "1st should be a DICTORY\r\n");
        return new_debug(debug_inf, strlen(debug_inf));
    }

    if (first->em == VAR){
        key = first->u_data.s_data;
    }
    else if (first->em == STORAGE) {
        key = first->u_data.a_storage + sizeof(int);
    }
    else{
        cross_snprintf(debug_inf, 256, "2st should be a VAR or STORAGE\r\n");
        return new_debug(debug_inf, strlen(debug_inf));
    }

    if ((entry = listTypeLookupWriteOrCreate(dic, key)) == NULL) {
        return primitive_empty;
    }
    llen = listTypeLength(entry);

    start = left->u_data.i_data;
    end = right->u_data.i_data;

    if (start < 0) start = llen + start;
    if (end < 0) end = llen + end;
    if (start < 0) start = 0;

    if (start > end || start >= llen) {
        return outcome;
    }
    if (end >= llen) end = llen - 1;
    rangelen = (end - start) + 1;

    if (start > llen/2) start -= llen;
    ln = listIndex((list*)entry->u_data.i_data, start);

    while(rangelen--) {
        outcome = c_cons(c_normal_copy(ln->value), outcome);
        ln = ln->next;
    }
    return outcome;    
}

#define ZSKIPLIST_MAXLEVEL 32 /* Should be enough for 2^32 elements */
#define ZSKIPLIST_P 0.25      /* Skiplist P = 1/4 */
int compareStringObjects(type *first, type *second) {
    char * a = NULL;
    char * b = NULL;
    if(first->em == VAR){
        a = first->u_data.s_data;
    }
    else if(first->em == STORAGE){
        a = first->u_data.a_storage + sizeof(int);
    }
    else{
    }

    if (second->em == VAR) {
        b = second->u_data.s_data;
    }
    else if(second->em == STORAGE){
        b = second->u_data.a_storage + sizeof(int);
    }
    else{
    }

    return  strcmp(a, b);
}

typedef struct zskiplistNode {
    type* obj;
    int score;
    struct zskiplistNode *backward;
    struct zskiplistLevel {
        struct zskiplistNode *forward;
        unsigned int span;
    } level[ZSKIPLIST_MAXLEVEL];
} zskiplistNode;

typedef struct zskiplist {
    struct zskiplistNode *header, *tail;
    unsigned long length;
    int level;
} zskiplist;

typedef struct zset {
    dict *dict;
    zskiplist *zsl;
} zset;

zskiplistNode *zslCreateNode(int level, int score, type *obj) {
    zskiplistNode *zn = zmalloc(sizeof(*zn)+level*sizeof(struct zskiplistLevel));
    zn->score = score;
    zn->obj = obj;
    return zn;
}

zskiplist *zslCreate(void) {
    int j;
    zskiplist *zsl;

    zsl = zmalloc(sizeof(*zsl));
    zsl->level = 1;
    zsl->length = 0;
    zsl->header = zslCreateNode(ZSKIPLIST_MAXLEVEL,0,NULL);
    for (j = 0; j < ZSKIPLIST_MAXLEVEL; j++) {
        zsl->header->level[j].forward = NULL;
        zsl->header->level[j].span = 0;
    }
    zsl->header->backward = NULL;
    zsl->tail = NULL;
    return zsl;
}

void zslFreeNode(zskiplistNode *node) {
    gc(node->obj);
    zfree(node);
}

void zslFree(zskiplist *zsl) {
    zskiplistNode *node = zsl->header->level[0].forward, *next;

    zfree(zsl->header);
    while(node) {
        next = node->level[0].forward;
        zslFreeNode(node);
        node = next;
    }
    zfree(zsl);
}

int zslRandomLevel(void) {
    int level = 1;
    while ((cross_rand()&0xFFFF) < (ZSKIPLIST_P * 0xFFFF))
        level += 1;
    return (level<ZSKIPLIST_MAXLEVEL) ? level : ZSKIPLIST_MAXLEVEL;
}

zskiplistNode *zslInsert(zskiplist *zsl, int score, type *obj) {
    zskiplistNode *update[ZSKIPLIST_MAXLEVEL], *x;
    unsigned int rank[ZSKIPLIST_MAXLEVEL];
    int i, level;

    x = zsl->header;
    for (i = zsl->level-1; i >= 0; i--) {
        /* store rank that is crossed to reach the insert position */
        rank[i] = i == (zsl->level-1) ? 0 : rank[i+1];
        while (x->level[i].forward &&
               (x->level[i].forward->score < score ||
                (x->level[i].forward->score == score &&
                 compareStringObjects(x->level[i].forward->obj,obj) < 0))) {
            rank[i] += x->level[i].span;
            x = x->level[i].forward;
        }
        update[i] = x;
    }
    /* we assume the key is not already inside, since we allow duplicated
     * scores, and the re-insertion of score and redis object should never
     * happpen since the caller of zslInsert() should test in the hash table
     * if the element is already inside or not. */
    level = zslRandomLevel();
    if (level > zsl->level) {
        for (i = zsl->level; i < level; i++) {
            rank[i] = 0;
            update[i] = zsl->header;
            update[i]->level[i].span = zsl->length;
        }
        zsl->level = level;
    }
    x = zslCreateNode(level,score,obj);
    for (i = 0; i < level; i++) {
        x->level[i].forward = update[i]->level[i].forward;
        update[i]->level[i].forward = x;

        /* update span covered by update[i] as x is inserted here */
        x->level[i].span = update[i]->level[i].span - (rank[0] - rank[i]);
        update[i]->level[i].span = (rank[0] - rank[i]) + 1;
    }

    /* increment span for untouched levels */
    for (i = level; i < zsl->level; i++) {
        update[i]->level[i].span++;
    }

    x->backward = (update[0] == zsl->header) ? NULL : update[0];
    if (x->level[0].forward)
        x->level[0].forward->backward = x;
    else
        zsl->tail = x;
    zsl->length++;
    return x;
}

/* Internal function used by zslDelete, zslDeleteByScore and zslDeleteByRank */
void zslDeleteNode(zskiplist *zsl, zskiplistNode *x, zskiplistNode **update) {
    int i;
    for (i = 0; i < zsl->level; i++) {
        if (update[i]->level[i].forward == x) {
            update[i]->level[i].span += x->level[i].span - 1;
            update[i]->level[i].forward = x->level[i].forward;
        } 
        else {
            update[i]->level[i].span -= 1;
        }
    }
    if (x->level[0].forward) {
        x->level[0].forward->backward = x->backward;
    } 
    else {
        zsl->tail = x->backward;
    }

    while(zsl->level > 1 && zsl->header->level[zsl->level-1].forward == NULL)
        zsl->level--;
    zsl->length--;
}

/* Delete an element with matching score/object from the skiplist. */
int zslDelete(zskiplist *zsl, int score, type *obj) {
    zskiplistNode *update[ZSKIPLIST_MAXLEVEL], *x;
    int i;

    x = zsl->header;
    for (i = zsl->level-1; i >= 0; i--) {
        while (x->level[i].forward &&
               (x->level[i].forward->score < score ||
                (x->level[i].forward->score == score &&
                 compareStringObjects(x->level[i].forward->obj, obj) < 0)))
            x = x->level[i].forward;
        update[i] = x;
    }
    /* We may have multiple elements with the same score, what we need
     * is to find the element with both the right score and object. */
    x = x->level[0].forward;
    if (x && score == x->score && (compareStringObjects(x->obj,obj) == 0)) {
        zslDeleteNode(zsl, x, update);
        zslFreeNode(x);
        return 1;
    } 
    else {
        return 0; /* not found */
    }
    return 0; /* not found */
}

/* Struct to hold a inclusive/exclusive range spec. */
typedef struct {
    int min, max;
    int minex, maxex; /* are min or max exclusive? */
} zrangespec;

static int zslValueGteMin(int value, zrangespec *spec) {
    return spec->minex ? (value > spec->min) : (value >= spec->min);
}

static int zslValueLteMax(int value, zrangespec *spec) {
    return spec->maxex ? (value < spec->max) : (value <= spec->max);
}

/* Returns if there is a part of the zset is in range. */
int zslIsInRange(zskiplist *zsl, zrangespec *range) {
    zskiplistNode *x;

    /* Test for ranges that will always be empty. */
    if (range->min > range->max ||
        (range->min == range->max && (range->minex || range->maxex)))
        return 0;
    x = zsl->tail;
    if (x == NULL || !zslValueGteMin(x->score,range))
        return 0;
    x = zsl->header->level[0].forward;
    if (x == NULL || !zslValueLteMax(x->score,range))
        return 0;
    return 1;
}

/* Find the first node that is contained in the specified range.
 * Returns NULL when no element is contained in the range. */
zskiplistNode *zslFirstInRange(zskiplist *zsl, zrangespec range) {
    zskiplistNode *x;
    int i;

    /* If everything is out of range, return early. */
    if (!zslIsInRange(zsl,&range)) return NULL;

    x = zsl->header;
    for (i = zsl->level-1; i >= 0; i--) {
        /* Go forward while *OUT* of range. */
        while (x->level[i].forward &&
               !zslValueGteMin(x->level[i].forward->score,&range))
            x = x->level[i].forward;
    }

    /* This is an inner range, so the next node cannot be NULL. */
    x = x->level[0].forward;
    /* Check if score <= max. */
    if (!zslValueLteMax(x->score,&range)) return NULL;
    return x;
}

/* Find the last node that is contained in the specified range.
 * Returns NULL when no element is contained in the range. */
zskiplistNode *zslLastInRange(zskiplist *zsl, zrangespec range) {
    zskiplistNode *x;
    int i;

    /* If everything is out of range, return early. */
    if (!zslIsInRange(zsl,&range)) return NULL;

    x = zsl->header;
    for (i = zsl->level-1; i >= 0; i--) {
        /* Go forward while *IN* range. */
        while (x->level[i].forward &&
               zslValueLteMax(x->level[i].forward->score,&range))
            x = x->level[i].forward;
    }

    /* Check if score >= min. */
    if (!zslValueGteMin(x->score,&range)) return NULL;
    return x;
}

/* Delete all the elements with score between min and max from the skiplist.
 * Min and mx are inclusive, so a score >= min || score <= max is deleted.
 * Note that this function takes the reference to the hash table view of the
 * sorted set, in order to remove the elements from the hash table too. */
unsigned long zslDeleteRangeByScore(zskiplist *zsl, zrangespec range, dict *dict) {
    zskiplistNode *update[ZSKIPLIST_MAXLEVEL], *x;
    unsigned long removed = 0;
    int i;

    x = zsl->header;
    for (i = zsl->level-1; i >= 0; i--) {
        while (x->level[i].forward && (range.minex ?
                                       x->level[i].forward->score <= range.min :
                                       x->level[i].forward->score < range.min))
            x = x->level[i].forward;
        update[i] = x;
    }

    /* Current node is the last with score < or <= min. */
    x = x->level[0].forward;

    /* Delete nodes while in range. */
    while (x && (range.maxex ? x->score < range.max : x->score <= range.max)) {
        zskiplistNode *next = x->level[0].forward;
        zslDeleteNode(zsl,x,update);
        dictDelete(dict,x->obj);
        zslFreeNode(x);
        removed++;
        x = next;
    }
    return removed;
}

/* Delete all the elements with rank between start and end from the skiplist.
 * Start and end are inclusive. Note that start and end need to be 1-based */
unsigned long zslDeleteRangeByRank(zskiplist *zsl, unsigned int start, unsigned int end, dict *dict) {
    zskiplistNode *update[ZSKIPLIST_MAXLEVEL], *x;
    unsigned long traversed = 0, removed = 0;
    int i;

    x = zsl->header;
    for (i = zsl->level-1; i >= 0; i--) {
        while (x->level[i].forward && (traversed + x->level[i].span) < start) {
            traversed += x->level[i].span;
            x = x->level[i].forward;
        }
        update[i] = x;
    }

    traversed++;
    x = x->level[0].forward;
    while (x && traversed <= end) {
        zskiplistNode *next = x->level[0].forward;
        zslDeleteNode(zsl,x,update);
        dictDelete(dict,x->obj);
        zslFreeNode(x);
        removed++;
        traversed++;
        x = next;
    }
    return removed;
}

/* Find the rank for an element by both score and key.
 * Returns 0 when the element cannot be found, rank otherwise.
 * Note that the rank is 1-based due to the span of zsl->header to the
 * first element. */
unsigned long zslGetRank(zskiplist *zsl, int score, type *o) {
    zskiplistNode *x;
    unsigned long rank = 0;
    int i;

    x = zsl->header;
    for (i = zsl->level-1; i >= 0; i--) {
        while (x->level[i].forward &&
               (x->level[i].forward->score < score ||
                (x->level[i].forward->score == score &&
                 compareStringObjects(x->level[i].forward->obj, o) <= 0))) {
            rank += x->level[i].span;
            x = x->level[i].forward;
        }

        /* x might be equal to zsl->header, so test if obj is non-NULL */
        if (x->obj && (compareStringObjects(x->obj,o) == 0)) {
            return rank;
        }
    }
    return 0;
}

/* Finds an element by its rank. The rank argument needs to be 1-based. */
zskiplistNode* zslGetElementByRank(zskiplist *zsl, unsigned long rank) {
    zskiplistNode *x;
    unsigned long traversed = 0;
    int i;

    x = zsl->header;
    for (i = zsl->level-1; i >= 0; i--) {
        while (x->level[i].forward && (traversed + x->level[i].span) <= rank)
            {
                traversed += x->level[i].span;
                x = x->level[i].forward;
            }
        if (traversed == rank) {
            return x;
        }
    }
    return NULL;
}

long createZsetObject(void) {
    zset *zs = zmalloc(sizeof(*zs));
    zs->dict = dictCreate(&copyValueDictType, NULL);
    zs->zsl = zslCreate();
    return (long)zs;
}

type *setLookupWriteOrCreate(type  * dic, void *key) {
    char* key_mem = NULL;
    type * entry = lookupKey((dict*)dic->u_data.i_data, key);
    if (entry == NULL) {
        entry = new_object();
        entry->em = SET;

        entry->u_data.i_data = (long)createZsetObject();
        key_mem = (char * )cross_calloc(1, strlen(key) + 1);
        strcpy(key_mem, key);
        dictAdd((dict*)dic->u_data.i_data, key_mem, entry);
    } 
    return entry;
}

void* zaddGenericCommand(type* _left, int incr) {
    type * dic = c_car(_left);
    type * first = c_cadr(_left);
    type * second = c_car(c_cddr(_left));
    type * third = c_cadr(c_cddr(_left));
    type * dic_entry = NULL;

    char * dic_key = NULL;
    char debug_inf[256] = "\0";
    char * key = NULL;
    int score, curscore;
    zset* zs = NULL;
    zskiplistNode *znode;
    dictEntry *de;

    if (dic->em != DICTORY) {
        cross_snprintf(debug_inf, 256, "zaddGenericCommand 1st should be a DICTORY\r\n");
        return new_debug(debug_inf, strlen(debug_inf));
    }

    if(first->em == VAR){
        dic_key = first->u_data.s_data;
    }
    else if(first->em == STORAGE){
        dic_key = first->u_data.a_storage + sizeof(int);
    }
    else{
        cross_snprintf(debug_inf, 256, "zaddGenericCommand 2st should be a VAR or STORAGE\r\n");
        return new_debug(debug_inf, strlen(debug_inf));
    }

    if(second->em == VAR){
        key = second->u_data.s_data;
    }
    else if(second->em == STORAGE){
        key = second->u_data.a_storage + sizeof(int);
    }
    else{
        cross_snprintf(debug_inf, 256, "zaddGenericCommand 3st should be a VAR or STORAGE\r\n");
        return new_debug(debug_inf, strlen(debug_inf));
    }

    if ((dic_entry = setLookupWriteOrCreate(dic, dic_key)) == NULL){
        return primitive_empty;
    }

    score = third->u_data.i_data;
    zs = (zset*)dic_entry->u_data.i_data;

    de = dictFind(zs->dict, key);
    if (de != NULL) {
        curscore = *(int*)dictGetEntryVal(de);

        if (incr) {
            score += curscore;
        }

        /* Remove and re-insert when score changed. We can safely
         * delete the key object from the skiplist, since the
         * dictionary still has a reference to it. */
        if (score != curscore) {
            zslDelete(zs->zsl, curscore, second);
            znode = zslInsert(zs->zsl, score, c_normal_copy(second));
            dictGetEntryVal(de) = &znode->score; /* Update score ptr. */
        }
    } 
    else {
        char  *key_mem = (char * )cross_calloc(1, strlen(key) + 1);
        strcpy(key_mem, key);
        znode = zslInsert(zs->zsl, score, c_normal_copy(second));
        dictAdd(zs->dict, key_mem, &znode->score);
    }

    return primitive_empty;
}

void * original_zadd(void  *_left) {
    char debug_inf[256] = "\0";
    type* outcome = zaddGenericCommand(_left, 0);
    if (outcome->em == DEBUG) {
        cross_snprintf(debug_inf, 1024, "call subprocess error:%s", outcome->u_data.a_storage + sizeof(int));
        gc_atom(outcome);
        return new_debug(debug_inf, strlen(debug_inf));
    }
    return outcome;
}

void * original_zincr(void  *_left) {
    char debug_inf[256] = "\0";
    type* outcome = zaddGenericCommand(_left, 1);
    if (outcome->em == DEBUG) {
        cross_snprintf(debug_inf, 1024, "call subprocess error:%s", outcome->u_data.a_storage + sizeof(int));
        gc_atom(outcome);
        return new_debug(debug_inf, strlen(debug_inf));
    }
    return outcome;
}

void* zrangeGenericCommand(void* _left, int reverse) {
    int start, end;
    int rangelen;
    int llen;
    zset* zs;
    zskiplist *zsl;
    zskiplistNode *ln;

    type* dic = c_car(_left);
    type* first = c_cadr(_left);
    type* second = c_car(c_cddr(_left));
    type* third = c_cadr(c_cddr(_left));
    type* outcome = primitive_empty;
    type* dic_entry = primitive_empty;

    char* dic_key = NULL;
    char debug_inf[256] = "\0";

    if (dic->em != DICTORY) {
        cross_snprintf(debug_inf, 256, "zrangeGenericCommand 1st should be a DICTORY\r\n");
        return new_debug(debug_inf, strlen(debug_inf));
    }

    if(first->em == VAR){
        dic_key = first->u_data.s_data;
    }
    else if(first->em == STORAGE){
        dic_key = first->u_data.a_storage + sizeof(int);
    }
    else{
        cross_snprintf(debug_inf, 256, "zrangeGenericCommand 2st should be a VAR or STORAGE\r\n");
        return new_debug(debug_inf, strlen(debug_inf));
    }
 
    start = second->u_data.i_data;
    end = third->u_data.i_data;

    if ((dic_entry = setLookupWriteOrCreate(dic, dic_key)) == NULL){
        return primitive_empty;
    }

    zs = (zset*)dic_entry->u_data.i_data;
    llen = zs->zsl->length;
    if (start < 0) start = llen + start;
    if (end < 0) end = llen + end;
    if (start < 0) start = 0;
    if (start > end || start >= llen) {
        return primitive_empty;
    }
    if (end >= llen) end = llen - 1;
    rangelen = (end-start) + 1;
    zsl = zs->zsl;
    /* Check if starting point is trivial, before doing log(N) lookup. */
    if (reverse) {
        ln = zsl->tail;
        if (start > 0)
            ln = zslGetElementByRank(zsl,llen-start);
    } 
    else {
        ln = zsl->header->level[0].forward;
        if (start > 0)
            ln = zslGetElementByRank(zsl,start + 1);
    }

    while (rangelen--) {
        outcome = c_cons(c_list(c_normal_copy(ln->obj), long_type(ln->score), 0), outcome);
        ln = reverse ? ln->backward : ln->level[0].forward;
    }

    return outcome;
}

void* original_zrange(void* _left) {
    char debug_inf[256] = "\0";
    type* outcome = zrangeGenericCommand(_left, 0);
    if (outcome->em == DEBUG) {
        cross_snprintf(debug_inf, 1024, "call subprocess error:%s", outcome->u_data.a_storage + sizeof(int));
        gc_atom(outcome);
        return new_debug(debug_inf, strlen(debug_inf));
    }
    return outcome;
}

void* original_zrevrange(void* _left) {
    char debug_inf[256] = "\0";
    type* outcome = zrangeGenericCommand(_left, 1);
    if (outcome->em == DEBUG) {
        cross_snprintf(debug_inf, 1024, "call subprocess error:%s", outcome->u_data.a_storage + sizeof(int));
        gc_atom(outcome);
        return new_debug(debug_inf, strlen(debug_inf));
    }
    return outcome;
}

void* zrankGenericCommand(void* _left, int reverse) {
    unsigned long rank;
    dictEntry *de;
    int score;
    zset* zs;
    int llen;
    char* key = NULL;
    type* outcome = primitive_empty;
    type* dic_entry = primitive_empty;
    char* dic_key = NULL;
    char debug_inf[256] = "\0";
    type* dic = c_car(_left);
    type* first = c_cadr(_left);
    type* second = c_car(c_cddr(_left));
    zskiplist *zsl;

    if (dic->em != DICTORY) {
        cross_snprintf(debug_inf, 256, "zrankGenericCommand 1st should be a DICTORY\r\n");
        return new_debug(debug_inf, strlen(debug_inf));
    }

    if(first->em == VAR){
        dic_key = first->u_data.s_data;
    }
    else if(first->em == STORAGE){
        dic_key = first->u_data.a_storage + sizeof(int);
    }
    else{
        cross_snprintf(debug_inf, 256, "zrankGenericCommand 2st should be a VAR or STORAGE\r\n");
        return new_debug(debug_inf, strlen(debug_inf));
    } 

    if(second->em == VAR){
        key = second->u_data.s_data;
    }
    else if(second->em == STORAGE){
        key = second->u_data.a_storage + sizeof(int);
    }
    else{
    }

    if ((dic_entry = setLookupWriteOrCreate(dic, dic_key)) == NULL){
        return primitive_empty;
    }

    zs = (zset*)dic_entry->u_data.i_data;
    llen = zs->zsl->length;
    zsl = zs->zsl;
    de = dictFind(zs->dict, key);
    if (de != NULL) {
        score = *(int*)dictGetEntryVal(de);
        rank = zslGetRank(zsl, score, second);
        if (reverse)
            outcome = long_type(llen - rank);
        else
            outcome = long_type(rank - 1);
    }
    return outcome;
}

void* original_zrank(void* _left) {
    char debug_inf[256] = "\0";
    type* outcome = zrankGenericCommand(_left, 0);
    if (outcome->em == DEBUG) {
        cross_snprintf(debug_inf, 1024, "call subprocess error:%s", outcome->u_data.a_storage + sizeof(int));
        gc_atom(outcome);
        return new_debug(debug_inf, strlen(debug_inf));
    }
    return outcome;
}

void* original_zrevrank(void* _left) {
    char debug_inf[256] = "\0";
    type* outcome = zrankGenericCommand(_left, 1);
    if (outcome->em == DEBUG) {
        cross_snprintf(debug_inf, 1024, "call subprocess error:%s", outcome->u_data.a_storage + sizeof(int));
        gc_atom(outcome);
        return new_debug(debug_inf, strlen(debug_inf));
    }
    return outcome;
}

void* original_zscore(void* _left) {
    int score;
    dictEntry* de;
    zset* zs;
    type* outcome = primitive_empty;
    type* dic_entry = primitive_empty;
    char* key = NULL;
    char* dic_key = NULL;
    char debug_inf[256] = "\0";
    type* dic = c_car(_left);
    type* first = c_cadr(_left);
    type* second = c_car(c_cddr(_left));

    if (dic->em != DICTORY) {
        cross_snprintf(debug_inf, 256, "1st should be a DICTORY\r\n");
        return new_debug(debug_inf, strlen(debug_inf));
    }

    if(first->em == VAR){
        dic_key = first->u_data.s_data;
    }
    else if(first->em == STORAGE){
        dic_key = first->u_data.a_storage + sizeof(int);
    }
    else{
        cross_snprintf(debug_inf, 256, "2st should be a VAR or STORAGE\r\n");
        return new_debug(debug_inf, strlen(debug_inf));
    } 

    if(second->em == VAR){
        key = second->u_data.s_data;
    }
    else if(second->em == STORAGE){
        key = second->u_data.a_storage + sizeof(int);
    }
    else{
        cross_snprintf(debug_inf, 256, "3st should be a VAR or STORAGE\r\n");
        return new_debug(debug_inf, strlen(debug_inf));
    }

    if ((dic_entry = setLookupWriteOrCreate(dic, dic_key)) == NULL){
        return primitive_empty;
    }

    zs = (zset*)(dic_entry->u_data.i_data);

    de = dictFind(zs->dict, key);
    if (de != NULL) {
        score = *(int*)dictGetEntryVal(de);
        outcome = (type*)long_type(score);
    }

    return outcome;
}
/*(dict system end)*/

/*(json system)*/
#define CJSON_FALSE 0
#define CJSON_TRUE 1
#define CJSON_NULL 2
#define CJSON_NUMBER 3
#define CJSON_STRING 4
#define CJSON_ARRAY 5
#define CJSON_OBJECT 6
#define CJSON_ISREFERENCE 256

typedef struct cjson {
    struct cjson *next, *prev;
    struct cjson *child;
    int type;       
    char *valuestring;  
    int valueint;       
    char *string;       
}cjson;

static const char *ep;

const char *cjson_geterror_ptr(void) {return ep;}

static int cjson_strcasecmp(const char *s1, const char *s2) {
    if (!s1) return (s1 == s2) ? 0:1;
    if (!s2) return 1;
    for(; cross_tolower(*s1) == cross_tolower(*s2); ++s1,  ++s2) {
        if(*s1 == 0) {
            return 0;
        }
    }
    return cross_tolower(*(const unsigned char *)s1) - cross_tolower(*(const unsigned char *)s2);
}

void* (*cjson_malloc)(size_t sz) = cross_malloc;
void (*cjson_free)(void *ptr) = cross_free;

static char* cjson_strdup(const char* str) {
    int len;
    char* copy;

    len = cross_strlen((void*)str) + 1;
    if (!(copy = (char*)cjson_malloc(len))) {
        return 0;
    }
    cross_memcpy(copy, (void*)str, len);
    return copy;
}

static cjson *cjson_new_item(void) {
    cjson* node = (cjson*)cjson_malloc(sizeof(cjson));
    if (node) memset(node, 0, sizeof(cjson));
    return node;
}

void cjson_delete(cjson *c) {
    cjson *next;
    while (c) {
        next = c->next;
        if (!(c->type & CJSON_ISREFERENCE) && c->child) {
            cjson_delete(c->child);
        }
        if (!(c->type & CJSON_ISREFERENCE) && c->valuestring) {
            cjson_free(c->valuestring);
        }
        if (c->string) {
            cjson_free(c->string);
        }
        cjson_free(c);
        c = next;
    }
}

static const char *parse_number(cjson *item,  const char *num) {
    int n = 0, sign = 1, scale = 0;int subscale = 0; int signsubscale = 0;

    if (*num == '-') {
        sign = -1, num++;   
    }
    if (*num == '0') {
        num++;      
    }
    if (*num  >=  '1' && *num  <=  '9') {
        do  n = (n*10.0)+(*num++ -'0'); while (*num >= '0' && *num <= '9');
    }
    /*
      if (*num == '.' && num[1]  >=  '0' && num[1] <= '9') {num++;        do  n = (n*10.0)+(*num++ -'0'), scale--; while (*num >= '0' && *num <= '9');}   
    */
    if (*num == '.' && num[1]  >=  '0' && num[1] <= '9') {
        num++; while (*num >= '0' && *num <= '9')num++;
    }   
    if (*num == 'e' || *num == 'E') {
        num++; 
        if (*num == '+') {
            num++;  
        }
        else if (*num == '-') {
            signsubscale = -1, num++;
        }
        while (*num >= '0' && *num <= '9') subscale = (subscale*10)+(*num++ - '0');
    }
    n = sign * n;
    item->valueint = (int)n;
    item->type = CJSON_NUMBER;
    return num;
}

static char *print_number(cjson *item) {
    char *str;
    str = (char*)cjson_malloc(21);  
    if (str) cross_sprintf(str, "%d", item->valueint);
    return str;
}

static unsigned parse_hex4(const char *str) {
    unsigned h = 0;
    if (*str >= '0' && *str <= '9') 
        h += (*str)-'0'; 
    else if (*str >= 'A' && *str <= 'F') h += 10+(*str)-'A'; 
    else if (*str >= 'a' && *str <= 'f') h += 10+(*str)-'a'; 
    else return 0;

    h = h << 4;str++;
    if (*str >= '0' && *str <= '9') h += (*str)-'0'; 
    else if (*str >= 'A' && *str <= 'F') h += 10+(*str)-'A'; 
    else if (*str >= 'a' && *str <= 'f') h += 10+(*str)-'a'; 
    else return 0;

    h = h << 4;str++;
    if (*str >= '0' && *str <= '9') h += (*str)-'0'; 
    else if (*str >= 'A' && *str <= 'F') h += 10+(*str)-'A'; 
    else if (*str >= 'a' && *str <= 'f') h += 10+(*str)-'a'; 
    else return 0;

    h = h << 4;str++;
    if (*str >= '0' && *str <= '9') h += (*str)-'0'; 
    else if (*str >= 'A' && *str <= 'F') h += 10+(*str)-'A'; 
    else if (*str >= 'a' && *str <= 'f') h += 10+(*str)-'a'; 
    else return 0;
    return h;
}


static const unsigned char firstByteMark[7] = { 0x00,  0x00,  0xC0,  0xE0,  0xF0,  0xF8,  0xFC };
static const char *parse_string(cjson *item, const char *str) {
    const char *ptr = str + 1; char *ptr2; char *out; int len = 0; unsigned uc, uc2;
    if (*str != '\"') {
        ep = str;
        return 0;
    }   
    
    while (*ptr != '\"' && *ptr && ++len) {
        if (*ptr++ == '\\') ptr++;  
    }
    
    out = (char*)cjson_malloc(len+1);   
    if (!out) {
        return 0;
    }
    
    ptr = str+1;ptr2 = out;
    while (*ptr != '\"' && *ptr) {
        if (*ptr != '\\') {
            *ptr2++= *ptr++;
        }
        else {
            ptr++;
            switch (*ptr) {
            case 'b': *ptr2++= '\b';    break;
            case 'f': *ptr2++= '\f';    break;
            case 'n': *ptr2++= '\n';    break;
            case 'r': *ptr2++= '\r';    break;
            case 't': *ptr2++= '\t';    break;
            case 'u':   
                uc = parse_hex4(ptr+1);ptr += 4;
                if ((uc >= 0xDC00 && uc <= 0xDFFF) || uc == 0)  break;  /* check for invalid.   */
                if (uc >= 0xD800 && uc <= 0xDBFF) { /* UTF16 surrogate pairs.   */
                    if (ptr[1] != '\\' || ptr[2] != 'u')    break;  /* missing second-half of surrogate.    */
                    uc2 = parse_hex4(ptr+3);ptr += 6;
                    if (uc2 < 0xDC00 || uc2 > 0xDFFF)       break;  /* invalid second-half of surrogate.    */
                    uc = 0x10000 + (((uc&0x3FF) << 10) | (uc2&0x3FF));
                }

                len = 4;if (uc < 0x80) len = 1;else if (uc < 0x800) len = 2;else if (uc < 0x10000) len = 3; ptr2 += len;
                    
                switch (len) {
                case 4: *--ptr2  = ((uc | 0x80) & 0xBF); uc  >>=  6;
                case 3: *--ptr2  = ((uc | 0x80) & 0xBF); uc  >>=  6;
                case 2: *--ptr2  = ((uc | 0x80) & 0xBF); uc  >>=  6;
                case 1: *--ptr2  = (uc | firstByteMark[len]);
                }
                ptr2 += len;
                break;
            default:  *ptr2++= *ptr; break;
            }
            ptr++;
        }
    }
    *ptr2 = 0;
    if (*ptr == '\"') {
        ptr++;
    }
    item->valuestring = out;
    item->type = CJSON_STRING;
    return ptr;
}

/* Render the cstring provided to an escaped version that can be printed. */
static char *print_string_ptr(const char *str) {
    const char *ptr;char *ptr2, *out;int len = 0;unsigned char token;
    
    if (!str) {
        return cjson_strdup("");
    }
    ptr = str;
    while ((token = *ptr) && ++len) {
        if (cross_strchr("\"\\\b\f\n\r\t", token)) len++; 
        else if (token < 32) len += 5;ptr++;
    }
    
    out = (char*)cjson_malloc(len+3);
    if (!out) {
        return 0;
    }

    ptr2 = out;ptr = str;
    *ptr2++= '\"';
    while (*ptr) {
        if ((unsigned char)*ptr > 31 && *ptr != '\"' && *ptr != '\\') *ptr2++= *ptr++;
        else {
            *ptr2++= '\\';
            switch (token = *ptr++) {
            case '\\':  *ptr2++= '\\';  break;
            case '\"':  *ptr2++= '\"';  break;
            case '\b':  *ptr2++= 'b';   break;
            case '\f':  *ptr2++= 'f';   break;
            case '\n':  *ptr2++= 'n';   break;
            case '\r':  *ptr2++= 'r';   break;
            case '\t':  *ptr2++= 't';   break;
            default: cross_sprintf(ptr2, "u%04x", token);ptr2 += 5; break;  /* escape and print */
            }
        }
    }
    *ptr2++= '\"';*ptr2++= 0;
    return out;
}

static char *print_string(cjson *item)  {return print_string_ptr(item->valuestring);}
static const char *parse_value(cjson *item, const char *value);
static char *print_value(cjson *item, int depth, int fmt);
static const char *parse_array(cjson *item, const char *value);
static char *print_array(cjson *item, int depth, int fmt);
static const char *parse_object(cjson *item, const char *value);
static char *print_object(cjson *item, int depth, int fmt);
static const char *skip(const char *in) {while (in && *in && (unsigned char)*in <= 32) in++; return in;}

cjson *cjson_parseWithOpts(const char *value, const char **return_parse_end, int require_null_terminated) {
    const char *end = 0;
    cjson *c = cjson_new_item();
    ep = 0;
    if (!c) {
        return 0;       /* memory fail */
    }

    end = parse_value(c, skip(value));
    if (!end) {
        cjson_delete(c);
        return 0;
    }   /* parse failure. ep is set. */

    if (require_null_terminated) {
        end = skip(end);
        if (*end) {
            cjson_delete(c);
            ep = end;
            return 0;
        }
    }

    if (return_parse_end) {
        *return_parse_end = end;
    }

    return c;
}

cjson *cjson_parse(const char *value) {
    return cjson_parseWithOpts(value, 0, 0);
}

char *cjson_print(cjson *item) {
    return print_value(item, 0, 1);
}

char *cjson_print_unformatted(cjson *item) {
    return print_value(item, 0, 0);
}

static const char *parse_value(cjson *item, const char *value) {
    if (!value) {
        return 0;   /* Fail on null. */
    }
    if (!strcmp(value, "null")) {
        item->type = CJSON_NULL;
        return value + 4;
    }
    if (!strcmp(value, "false")) {
        item->type = CJSON_FALSE;
        return value + 5;
    }
    if (!strcmp(value, "true")) {
        item->type = CJSON_TRUE;
        item->valueint = 1;
        return value + 4;
    }
    if (*value == '\"') {
        return parse_string(item, value);
    }
    if (*value == '-' || (*value >= '0' && *value <= '9')){
        return parse_number(item, value);
    }
    if (*value == '[') { 
        return parse_array(item, value);
    }
    if (*value == '{') { 
        return parse_object(item, value);
    }

    ep = value;
    return 0;   /* failure. */
}

static char *print_value(cjson *item, int depth, int fmt) {
    char *out = 0;
    if (!item) return 0;
    switch ((item->type)&255) {
    case CJSON_NULL:    out = cjson_strdup("null"); break;
    case CJSON_FALSE:   out = cjson_strdup("false");break;
    case CJSON_TRUE:    out = cjson_strdup("true"); break;
    case CJSON_NUMBER:  out = print_number(item);break;
    case CJSON_STRING:  out = print_string(item);break;
    case CJSON_ARRAY:   out = print_array(item, depth, fmt);break;
    case CJSON_OBJECT:  out = print_object(item, depth, fmt);break;
    }
    return out;
}

static const char *parse_array(cjson *item, const char *value) {
    cjson *child;
    if (*value != '[')  {
        ep = value;
        return 0;
    }   /* not an array! */

    item->type = CJSON_ARRAY;
    value = skip(value+1);
    if (*value == ']') {
        return value+1; /* empty array. */
    }

    item->child = child = cjson_new_item();
    if (!item->child) {
        return 0;        /* memory fail */
    }
    value = skip(parse_value(child, skip(value)));  /* skip any spacing,  get the value. */

    if (!value) {
        return 0;
    }

    while (*value == ',') {
        cjson *new_item;
        if (!(new_item = cjson_new_item())) {
            return 0;   /* memory fail */
        }
        child->next = new_item;
        new_item->prev = child;
        child = new_item;
        value = skip(parse_value(child, skip(value+1)));
        if (!value) {
            return 0;   /* memory fail */
        }
    }

    if (*value == ']') {
        return value+1; /* end of array */
    }
    ep = value;
    return 0;   /* malformed. */
}

static char *print_array(cjson *item, int depth, int fmt) {
    char **entries;
    char *out = 0, *ptr, *ret;int len = 5;
    cjson *child = item->child;
    int numentries = 0, i = 0, fail = 0;
    
    /* How many entries in the array? */
    while (child) numentries++, child = child->next;
    /* Explicitly handle numentries == 0 */
    if (!numentries) {
        out = (char*)cjson_malloc(3);
        if (out) cross_strcpy(out, "[]");
        return out;
    }
    /* Allocate an array to hold the values for each */
    entries = (char**)cjson_malloc(numentries*sizeof(char*));
    if (!entries) return 0;
    memset(entries, 0, numentries*sizeof(char*));
    /* Retrieve all the results: */
    child = item->child;
    while (child && !fail) {
        ret = print_value(child, depth+1, fmt);
        entries[i++] = ret;
        if (ret) len += cross_strlen(ret)+2+(fmt?1:0); else fail = 1;
        child = child->next;
    }
    
    /* If we didn't fail,  try to malloc the output string */
    if (!fail) out = (char*)cjson_malloc(len);
    /* If that fails,  we fail. */
    if (!out) fail = 1;

    /* Handle failure. */
    if (fail) {
        for (i = 0;i < numentries;i++) if (entries[i]) cjson_free(entries[i]);
        cjson_free(entries);
        return 0;
    }
    
    /* Compose the output array. */
    *out = '[';
    ptr = out+1;*ptr = 0;
    for (i = 0;i < numentries;i++) {
        cross_strcpy(ptr, entries[i]);ptr += cross_strlen(entries[i]);
        if (i != numentries-1) {*ptr++= ',';if(fmt)*ptr++= ' ';*ptr = 0;}
        cjson_free(entries[i]);
    }
    cjson_free(entries);
    *ptr++= ']';*ptr++= 0;
    return out; 
}

/* Build an object from the text. */
static const char *parse_object(cjson *item, const char *value) {
    cjson *child;
    if (*value != '{') {
        ep = value;
        return 0;
    }   /* not an object! */
    
    item->type = CJSON_OBJECT;
    value = skip(value+1);
    if (*value == '}') {
        return value + 1;   /* empty array. */
    }
    
    item->child = child = cjson_new_item();
    if (!item->child) {
        return 0;
    }

    value = skip(parse_string(child, skip(value)));
    if (!value) {
        return 0;
    }

    child->string = child->valuestring;
    child->valuestring = 0;

    if (*value != ':') {
        ep = value;
        return 0;
    }   /* fail! */

    value = skip(parse_value(child, skip(value+1)));    /* skip any spacing,  get the value. */
    if (!value) {
        return 0;
    }
    
    while (*value == ',') {
        cjson *new_item;
        if (!(new_item = cjson_new_item())) {
            return 0; /* memory fail */
        }
        child->next = new_item;
        new_item->prev = child;
        child = new_item;
        value = skip(parse_string(child, skip(value+1)));
        if (!value) {
            return 0;
        }
        child->string = child->valuestring;child->valuestring = 0;
        if (*value != ':') {
            ep = value;
            return 0;
        }   /* fail! */
        value = skip(parse_value(child, skip(value+1)));    /* skip any spacing,  get the value. */

        if (!value) {
            return 0;
        }
    }
    
    if (*value == '}') {
        return value+1; /* end of array */
    }
    ep = value;
    return 0;   /* malformed. */
}

/* Render an object to text. */
static char *print_object(cjson *item,  int depth,  int fmt) {
    char **entries = 0, **names = 0;
    char *out = 0, *ptr, *ret, *str;int len = 7, i = 0, j;
    cjson *child = item->child;
    int numentries = 0, fail = 0;
    /* Count the number of entries. */
    while (child) numentries++, child = child->next;
    /* Explicitly handle empty object case */
    if (!numentries) {
        out = (char*)cjson_malloc(fmt?depth+4:3);
        if (!out)   return 0;
        ptr = out;*ptr++= '{';
        if (fmt) {*ptr++= '\n';for (i = 0;i < depth-1;i++) *ptr++= '\t';}
        *ptr++= '}';*ptr++= 0;
        return out;
    }
    /* Allocate space for the names and the objects */
    entries = (char**)cjson_malloc(numentries*sizeof(char*));
    if (!entries) return 0;
    names = (char**)cjson_malloc(numentries*sizeof(char*));
    if (!names) {cjson_free(entries);return 0;}
    memset(entries, 0, sizeof(char*)*numentries);
    memset(names, 0, sizeof(char*)*numentries);

    /* Collect all the results into our arrays: */
    child = item->child;depth++;if (fmt) len += depth;
    while (child) {
        names[i] = str = print_string_ptr(child->string);
        entries[i++] = ret = print_value(child, depth, fmt);
        if (str && ret) len += cross_strlen(ret)+cross_strlen(str)+2+(fmt?2+depth:0); else fail = 1;
        child = child->next;
    }
    
    /* Try to allocate the output string */
    if (!fail) out = (char*)cjson_malloc(len);
    if (!out) fail = 1;

    /* Handle failure */
    if (fail) {
        for (i = 0;i < numentries;i++) {if (names[i]) cjson_free(names[i]);if (entries[i]) cjson_free(entries[i]);}
        cjson_free(names);cjson_free(entries);
        return 0;
    }
    
    /* Compose the output: */
    *out = '{';ptr = out+1;if (fmt)*ptr++= '\n';*ptr = 0;
    for (i = 0;i < numentries;i++) {
        if (fmt) for (j = 0;j < depth;j++) *ptr++= '\t';
        cross_strcpy(ptr, names[i]);ptr += cross_strlen(names[i]);
        *ptr++= ':';if (fmt) *ptr++= '\t';
        cross_strcpy(ptr, entries[i]);ptr += cross_strlen(entries[i]);
        if (i != numentries-1) *ptr++= ',';
        if (fmt) *ptr++= '\n';*ptr = 0;
        cjson_free(names[i]);
        cjson_free(entries[i]);
    }
    
    cjson_free(names);cjson_free(entries);
    if (fmt) for (i = 0;i < depth-1;i++) *ptr++= '\t';
    *ptr++= '}';*ptr++= 0;
    return out; 
}

cjson *cjson_getobjectitem(cjson *object, const char *string) {
    cjson *c = object->child; 
    while (c && cjson_strcasecmp(c->string, string)) {
        c = c->next; 
    }
    return c;
}

/* Utility for array list handling. */
static void suffix_object(cjson *prev, cjson *item) {
    prev->next = item;
    item->prev = prev;
}
/* Utility for handling references. */

void   cjson_additem_toarray(cjson *array,  cjson *item) {
    cjson *c = array->child;
    if (!item) return; 
    if (!c) {
        array->child = item;
    } 
    else {
        while (c && c->next) {
            c = c->next; 
        }
        suffix_object(c, item);
    }
}

void   cjson_additem_toobject(cjson *object, const char *string, cjson *item) {
    if (!item) return; 
    if (item->string)cjson_free(item->string);
    item->string = cjson_strdup(string);
    cjson_additem_toarray(object, item);
}

cjson *cjson_DetachItemFromArray(cjson *array, int which) {
    cjson *c = array->child;
    while (c && which > 0) 
        c = c->next, which--;

    if (!c) {
        return 0;
    }

    if (c->prev) 
        c->prev->next = c->next;
    if (c->next) 
        c->next->prev = c->prev;
    if (c == array->child) 
        array->child = c->next;
    c->prev = c->next = 0;
    return c;
}

void   cjson_deleteItemFromArray(cjson *array, int which) {
    cjson_delete(cjson_DetachItemFromArray(array, which));
}

cjson *cjson_DetachItemFromObject(cjson *object, const char *string) {
    int i = 0;
    cjson *c = object->child;
    while (c && cjson_strcasecmp(c->string, string)) 
        i++, c = c->next;

    if (c) 
        return cjson_DetachItemFromArray(object, i);
    return 0;
}

void cjson_delitem_fromobj(cjson *object, const char *string) {
    cjson_delete(cjson_DetachItemFromObject(object, string));
}

/* Replace array/object items with new ones. */
void   cjson_ReplaceItemInArray(cjson *array, int which, cjson *newitem) {
    cjson *c = array->child;
    while (c && which > 0) 
        c = c->next, which--;
    
    if (!c) return;
    newitem->next = c->next;
    newitem->prev = c->prev;
    
    if (newitem->next) newitem->next->prev = newitem;
    if (c == array->child) 
        array->child = newitem; 
    else newitem->prev->next = newitem;

    c->next = c->prev = 0;
    cjson_delete(c);
}

void   cjson_ReplaceItemInObject(cjson *object, const char *string, cjson *newitem) {
    int i = 0;
    cjson *c = object->child;
    while(c && cjson_strcasecmp(c->string, string))i++, c = c->next;

    if(c) {
        newitem->string = cjson_strdup(string);
        cjson_ReplaceItemInArray(object, i, newitem);
    }
}

/* Create basic types: */
cjson *cjson_createnull(void) {
    cjson *item = cjson_new_item();

    if(item)item->type = CJSON_NULL;
    return item;
}

cjson *cjson_createTrue(void) {
    cjson *item = cjson_new_item();

    if(item)item->type = CJSON_TRUE;
    return item;
}

cjson *cjson_createFalse(void) {
    cjson *item = cjson_new_item();

    if(item)item->type = CJSON_FALSE;
    return item;
}

cjson *cjson_CreateBool(int b) {
    cjson *item = cjson_new_item();
    if(item)item->type = b?CJSON_TRUE:CJSON_FALSE;
    return item;
}

cjson *cjson_createnumber(int num) {
    cjson *item = cjson_new_item();

    if(item) {
        item->type = CJSON_NUMBER;
        item->valueint = (int)num;
    }
    return item;
}

cjson *cjson_createstring(const char *string) {
    cjson *item = cjson_new_item();

    if(item) {
        item->type = CJSON_STRING;
        item->valuestring = cjson_strdup(string);
    }

    return item;
}

cjson *cjson_createarray(void) {
    cjson *item = cjson_new_item();
    if(item)
        item->type = CJSON_ARRAY;

    return item;
}

cjson *cjson_createobject(void) {
    cjson *item = cjson_new_item();

    if(item)
        item->type = CJSON_OBJECT;

    return item;
}

/* Create Arrays: */
cjson *cjson_CreateIntArray(const int *numbers, int count) {
    int i;
    cjson *n = 0, *p = 0, *a = cjson_createarray();

    for(i = 0;a && i < count;i++) {
        n = cjson_createnumber(numbers[i]);
        if(!i)a->child = n;
        else suffix_object(p, n);
        p = n;
    }

    return a;
}

cjson *cjson_CreateFloatArray(const float *numbers, int count)  {
    int i;
    cjson *n = 0, *p = 0, *a = cjson_createarray();

    for(i = 0;a && i < count;i++) {
        n = cjson_createnumber(numbers[i]);
        if(!i)a->child = n;
        else suffix_object(p, n);
        p = n;
    }
    return a;
}

cjson *cjson_createstringArray(const char **strings, int count) {
    int i;
    cjson *n = 0, *p = 0, *a = cjson_createarray();

    for(i = 0;a && i < count;i++) {
        n = cjson_createstring(strings[i]);

        if(!i)a->child = n;
        else suffix_object(p, n);
        p = n;
    }
    return a;
}

/* Duplication */
cjson *cjson_Duplicate(cjson *item, int recurse) {
    cjson *newitem, *cptr, *nptr = 0, *newchild;
    /* Bail on bad ptr */
    if (!item) return 0;
    /* Create new item */
    newitem = cjson_new_item();
    if (!newitem) return 0;
    /* Copy over all vars */
    newitem->type = item->type&(~CJSON_ISREFERENCE), newitem->valueint = item->valueint;
    if (item->valuestring) {
        newitem->valuestring = cjson_strdup(item->valuestring); 
        if (!newitem->valuestring) {
            cjson_delete(newitem);
            return 0;
        }
    }
    if (item->string) {
        newitem->string = cjson_strdup(item->string);
        if (!newitem->string) {
            cjson_delete(newitem);
            return 0;
        }
    }
    /* If non-recursive,  then we're done! */
    if (!recurse) {
        return newitem;
    }
    /* Walk the ->next chain for the child. */
    cptr = item->child;
    while (cptr) {
        newchild = cjson_Duplicate(cptr, 1);        /* Duplicate (with recurse) each item in the ->next chain */
        if (!newchild) {cjson_delete(newitem);return 0;}
        if (nptr)   {nptr->next = newchild, newchild->prev = nptr;nptr = newchild;} /* If newitem->child already set,  then crosswire ->prev and ->next and move on */
        else        {newitem->child = newchild;nptr = newchild;}                    /* Set newitem->child and move to it */
        cptr = cptr->next;
    }
    return newitem;
}

void cjson_Minify(char *json) {
    char *into = json;
    while (*json) {
        if (*json == ' ') json++;
        else if (*json == '\t') json++; 
        else if (*json == '\r') json++;
        else if (*json == '\n') json++;
        else if (*json == '/' && json[1] == '/')  while (*json && *json != '\n') json++;
        else if (*json == '/' && json[1] == '*') {
            while (*json && !(*json == '*' && json[1] == '/')) json++;
            json += 2;
        }   
        else if (*json == '\"'){
            *into++= *json++;
            while (*json && *json != '\"') {
                if (*json == '\\') *into++= *json++;*into++= *json++;
            }
            *into++= *json++;
        }
        else {
            *into++= *json++;       
        }
    }
    *into = 0;  
}

void * original_jaddarray(void * _left) {
    type * left = NULL;
    type * right = NULL;
    cjson *json = NULL;
    cjson *jsonx = NULL;
    type * outcome = NULL;
    char debug_inf[256] = "\0";

    left = c_car(_left);
    if(left->em != JSON){
        cross_strcpy(debug_inf, "1st should be a JSON type\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }
    json = (cjson*)left->u_data.i_data;

    right = c_cadr(_left);
    if(right->em != JSON){
        cross_strcpy(debug_inf, "2st should be a JSON type\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }
    jsonx = (cjson*)right->u_data.i_data;

    cjson_additem_toarray(json, jsonx);
    outcome = c_normal_copy(left);
    return outcome;
}

void * original_jaddobject(void * _left) {
    type * left = NULL;
    type * right = NULL;
    cjson *json = NULL;
    cjson *jsonx = NULL;
    char * key = NULL;
    char debug_inf[256] = "\0";

    left = c_car(_left);
    if(left->em != JSON){
        cross_strcpy(debug_inf, "2st should be a JSON type\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }
    json = (cjson*)left->u_data.i_data;

    right = c_cadr(_left);
    if(right->em == VAR){
        key = (char*)right->u_data.s_data;
    }
    else if(right->em == STORAGE){
        key = (char*)(right->u_data.a_storage) + sizeof(int);
    }
    else {
        cross_strcpy(debug_inf, "2st should be a VAR or STORAGE type\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }

    right = c_caddr(_left);
    if(right->em != JSON){
        cross_strcpy(debug_inf, "3st should be a JSON type\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }

    jsonx = (cjson*)right->u_data.i_data;
    cjson_additem_toobject(json, key, jsonx);
    return  c_normal_copy(left);
}

void * original_jupdateobject(void * _left) {
    type * left = NULL;
    cjson *json = NULL;
    cjson *jsonx = NULL;
    char * key = NULL;
    type * result = NULL;
    char debug_inf[256] = "\0";

    left = c_car(_left);
    if(left->em != JSON){
        cross_strcpy(debug_inf, "1st should be a JSON type\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }
    json = (cjson*)left->u_data.i_data;

    left = c_cadr(_left);
    if (left->em != VAR) {
        cross_strcpy(debug_inf, "2st should be a VAR type\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }
    key = (char*)left->u_data.s_data;

    left = c_caddr(_left);
    if(left->em != JSON){
        cross_strcpy(debug_inf, "3st should be a JSON type\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }
    jsonx = (cjson*)left->u_data.i_data;

    cjson_delitem_fromobj(json, key);
    cjson_additem_toobject(json, key, jsonx);

    result = (type*)new_object();
    result->em = JSON;
    result->u_data.i_data = (long)json;
    return result;
}

void * original_jdeleteobject(void * _left) {
    type * left = NULL;
    cjson *json = NULL;
    cjson *jsonx = NULL;
    char * key = NULL;
    type * result = NULL;
    char debug_inf[256] = "\0";

    left = c_car(_left);
    if(left->em != JSON){
        cross_strcpy(debug_inf, "1st should be a JSON type\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }
    json = (cjson*)left->u_data.i_data;

    left = c_cadr(_left);
    if (left->em != VAR) {
        cross_strcpy(debug_inf, "2st should be a VAR type\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }
    key = (char*)left->u_data.s_data;
    cjson_delitem_fromobj(json, key);

    result = (type*)new_object();
    result->em = JSON;
    result->u_data.i_data = (long)json;
    return result;
}

void * original_jcreatearray(void * _left) {
    type * result = NULL;
    cjson *json = NULL;

    json = cjson_createarray();
    result = (type*)new_object();
    result->em = JSON;
    result->u_data.i_data = (long)json;
    return result;
}

void * original_jcreatestring(void * _left) {
    type * left = NULL;
    type * result = NULL;
    cjson *json = NULL;
    char * value = NULL;
    char debug_inf[256] = "\0";

    left = c_car(_left);
    if(left->em == VAR){
        value = (char*)left->u_data.s_data;
    }
    else if(left->em == STORAGE){
        value = (char*)(left->u_data.a_storage) + sizeof(int);
    }
    else if(left->em == EMPTY){
        value = "";
    }
    else{
        cross_strcpy(debug_inf, "1st should be a VAR or STORAGE type\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }

    json = cjson_createstring(value);
    result = (type*)new_object();
    result->em = JSON;
    result->u_data.i_data = (long)json;
    return result;
}

void * original_jcreateint(void * _left) {
    type * left = NULL;
    type * result = NULL;
    cjson *json = NULL;
    char debug_inf[256] = "\0";

    left = c_car(_left);
    if(left->em != BYTES) {
        cross_strcpy(debug_inf, "1st should be a BYTES type\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }
    json = cjson_createnumber(left->u_data.i_data);
    result = (type*)new_object();
    result->em = JSON;
    result->u_data.i_data = (long)json;
    return result;
}

void * original_jcreatetrue(void * _left) {
    type * result = NULL;
    cjson *json = NULL;
    json = cjson_createTrue();
    result = (type*)new_object();
    result->em = JSON;
    result->u_data.i_data = (long)json;
    return result;
}

void * original_jcreatefalse(void * _left) {
    type * result = NULL;
    cjson *json = NULL;
    json = cjson_createFalse();
    result = (type*)new_object();
    result->em = JSON;
    result->u_data.i_data = (long)json;
    return result;
}

void * original_jcreate(void * _left) {
    type * result = NULL;
    cjson *json = NULL;

    json = cjson_createobject();    
    result = (type*)new_object();
    result->em = JSON;
    result->u_data.i_data = (long)json;
    return result;
}


void * original_jtos(void * _left) {
    type * left = NULL;
    type* outcome = NULL;
    cjson *json = NULL;
    char * current = NULL; 
    char debug_inf[256] = "\0";

    left = c_car(_left);
    if (left->em == EMPTY) {
        return primitive_empty;
    }
    if ((left == NULL) || (left->em != JSON)) {
        cross_strcpy(debug_inf, "1st should be a JSON\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }

    json = (cjson*)left->u_data.i_data;
    current = cjson_print(json);
    outcome = new_storage(current, cross_strlen(current));
    cjson_free(current);
    return outcome;
}

void * original_jtosx(void * _left) {
    type * left = NULL;
    type * outcome = NULL;
    cjson *json = NULL;
    char * current = NULL;
    char debug_inf[256] = "\0";

    left = c_car(_left);
    if (left->em == EMPTY) {
        return primitive_empty;
    }
    if ((left == NULL) || (left->em != JSON)) {
        cross_strcpy(debug_inf, "1st should be a JSON\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }
    json = (cjson*)left->u_data.i_data;
    current = cjson_print_unformatted(json);

    outcome = new_storage(current, cross_strlen(current));
    cjson_free(current);
    cjson_delete(json); 
    return outcome;
}

void * original_killjson(void *_left) {
    type * left = NULL;
    cjson * json_result = NULL;
    type * result = primitive_empty;
    char debug_inf[256] = "\0";

    left = c_car(_left);
    if(left->em != JSON) {
        cross_strcpy(debug_inf, "1st shoule be a JSON type\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }
    json_result = (cjson*)left->u_data.i_data;
    cjson_delete(json_result); 
    return result;
}

void * original_jgetobject(void * _left) {
    type  *  left = NULL;
    type  *  right = NULL;
    char  *  key = NULL;
    cjson * json = NULL;
    cjson * json_result = NULL;
    type  * result = NULL;
    char debug_inf[256] = "\0";

    left = c_car(_left);
    if(left->em == EMPTY){
        return primitive_empty;
    }
    if(left->em != JSON) {
        cross_strcpy(debug_inf, "1st shoule be a JSON type\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }
    json = (cjson*)left->u_data.i_data;
    right = c_cadr(_left);
    if(right->em == VAR){
        key = (char*)right->u_data.s_data;
    }
    else if(right->em == STORAGE){
        key = (char*)(right->u_data.a_storage) + sizeof(int);
    }
    else {
        cross_strcpy(debug_inf, "2st shoule be a VAR or STORAGE type\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }

    json_result = cjson_getobjectitem(json, key);
    if(json_result == NULL){
        return primitive_empty;
    }
    result = (type*)new_object();
    result->em = JSON;
    result->u_data.i_data = (long)json_result;
    return result;
}

void * original_jgetstring(void * _left) {
    type *  left = NULL;
    cjson * json_result = NULL;
    type *  result = primitive_empty;
    char * current = NULL;
    char debug_inf[256] = "\0";

    left = c_car(_left);
    if(left->em == EMPTY){
        return primitive_empty;
    }
    if(left->em != JSON) {
        cross_strcpy(debug_inf, "1st shoule be a JSON type\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }
    json_result = (cjson*)left->u_data.i_data;
    if(json_result->type == CJSON_STRING) {
        /*follow code is very important, use var could be risk for rewrite the ref_count slot as memorey leak.*/
        current = json_result->valuestring;
        result = new_storage(current, cross_strlen(current));
    }
    return result;
}

void * original_jgetint(void * _left) {
    type * left = NULL;
    cjson * json_result = NULL;
    int result = 0;
    char debug_inf[256] = "\0";

    left = c_car(_left);
    if(left->em == EMPTY){
        return primitive_empty;
    }
    if(left->em != JSON) {
        cross_strcpy(debug_inf, "1st shoule be a JSON type\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }
    json_result = (cjson*)left->u_data.i_data;
    if(json_result->type == CJSON_NUMBER) {
        result = json_result->valueint;
    }
    return long_type(result);
}

void * original_jgetarraysize(void * _left) {
    type * left = NULL;
    cjson * json = NULL;
    cjson * json_result = NULL;
    int sz = 0;

    left = c_car(_left);
    if(left->em == EMPTY){
        return  long_type(0);
    }

    json_result = (cjson*)left->u_data.i_data;
    json = json_result->child;
    while(json)sz++, json = json->next;
    return long_type(sz);
}

void * original_jgetkeys(void * _left) {
    type *  left = NULL;
    cjson * json = NULL;
    cjson * json_result = NULL;
    type *  result = primitive_empty;
    type *  outcome = primitive_empty;
    char * key = NULL;
    char debug_inf[256] = "\0";

    left = c_car(_left);
    if(left->em != JSON){
        cross_strcpy(debug_inf, "1st shoule be a JSON type\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }
    json_result = (cjson*)left->u_data.i_data;
    json = json_result->child;
    while(json) {
        key = json->string;
        outcome = new_storage(key, cross_strlen(key));
        result = c_cons(outcome, result);
        json = json->next;
    }
    return result;
}

void * original_jgetarrayitem(void * _left) {
    type * left = NULL;
    type * right = NULL;
    cjson * json_result = NULL;
    cjson * json = NULL;
    type * result = primitive_empty;
    int index = 0;
    char debug_inf[256] = "\0";

    left = c_car(_left);
    if(left->em != JSON) {
        cross_strcpy(debug_inf, "1st shoule be a JSON type\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }
    json_result = (cjson*)left->u_data.i_data;

    right = c_cadr(_left);
    if(right->em != BYTES) {
        cross_strcpy(debug_inf, "2st shoule be a BYTES type\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }
    index = right->u_data.i_data;

    json = json_result->child; 
    while (json && index > 0) index--, json = json->next;

    result = new_object();
    result->em = JSON;
    result->u_data.i_data = (long)json;
    return result;
}

void * original_makejson(void *_left) {
    type * left = c_car(_left);
    type * result = primitive_empty;
    cjson *json = NULL;
    char debug_inf[256] = "\0", * begin = NULL;

    if (left->em == EMPTY) {
        return primitive_empty;
    }
    if(left->em != STORAGE) {
        cross_strcpy(debug_inf, "1st should be a STORAGE type\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }

    begin = left->u_data.a_storage + sizeof(int);
    if(begin != NULL && left->em != EMPTY){
        json = cjson_parse(begin);
        if (!json) {
            cross_strcpy(debug_inf, "makejson error\r\n");
            return new_debug(debug_inf, cross_strlen(debug_inf));
        }
        result = (type*)new_object();
        result->em = JSON;
        result->u_data.i_data = (long)json;
    }
    return result;
}
/*(json system end)*/


void network_encryption(unsigned char *_lisa, int length, unsigned char *lisa) {
    int i = 0;
    unsigned int high = 0;
    unsigned int low = 0;

    for (i = 0; i < length; i++) {
        low = _lisa[i] & 0x1F;
        high = (_lisa[i] & 0xE0);
        lisa[i] = (low << 3) + (high >> 5);
    }
}

void network_decryption(unsigned char *_lisa, int length, unsigned char *lisa) {
    int i = 0;
    unsigned int high = 0;
    unsigned int low = 0;

    for (i = 0; i < length; i++) {
        low = _lisa[i] & 0x07;
        high = (_lisa[i] & 0xF8);
        lisa[i] = (low << 5) + (high >> 3);
    }
}

void * original_random(void * _left) {
    int randvalue = 0, leftvalue = 0;
    unsigned int unixtime = 0;
    type * outcome = primitive_empty;
    type * left = c_car(_left);
    char debug_inf[256] = "\0";

    if (left->em != BYTES) {
        cross_strcpy(debug_inf, "1st should be a BYTES\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }
    leftvalue = left->u_data.i_data;
    unixtime = cross_rand();
    randvalue = unixtime % leftvalue;

    if (randvalue == 0) /*rand == left  is  ok  in  lisa progn , but  0 is not , 2013.3.2*/
        randvalue = leftvalue;

    outcome = long_type(randvalue);
    return outcome;
}

void * original_system(void * _left){
    type * left = c_car(_left);
    char * cmd = NULL;

    if(left->em == VAR){
        cmd = left->u_data.s_data;
    }
    else if(left->em == STORAGE){
        cmd = left->u_data.a_storage + sizeof(int);
    }
    else if(left->em == EMPTY){
        return primitive_empty;
    }
    else {}

    system(cmd);
    return long_type(0);
}

void* original_exe(void * _left){
    type * right = _left, *left = primitive_empty;
    char * cmd = NULL;
    int pid, i = 0, first = 0;
    char* exe = NULL; 
    char** argv = NULL;

    argv = (char**)cross_calloc(sizeof(char*), right->obj_length + 1);
    while (1) {
        left = c_car(right);
        if(left->em == VAR){
            cmd = left->u_data.s_data;
        }
        else if(left->em == STORAGE){
            cmd = left->u_data.a_storage + sizeof(int);
        }
        else if(left->em == EMPTY){
            gc(_left);
            return primitive_empty;
        }
        else {}

        if (first == 0) {
            exe = cmd;
            first = 1;
        }

        argv[i++] = cmd;
        right = c_cdr(right);
        if (right->em == EMPTY) {
            break;
        }
    }
    argv[i++] = (char*)0;

#ifdef _WIN32
#else
    if ( (pid = fork()) < 0) {
        color_fprintf(stderr, COLOR_RED, "Fork failed in init\r\n");
    }
    if (!pid) {
        /*
          close(0);close(1);close(2);
          setsid();
          (void) open("/dev/tty0",O_RDWR,0);
        */
        execvp(exe, argv);
        exit(1);
    }

    while (1)
        if (pid == wait(&i))
            break;
#endif

    cross_fprintf(stderr, "\n\rchild %d died with code %04x\n\r", pid, i);

    cross_free(argv);
    return long_type(0);
}

void * original_mktime(void * _left){
    type * present = c_car(_left);
    char * str_time = NULL;
    int unixtime = 0;
    struct tm stm;  
    int year, month, day, hour, min, second;  
    char debug_inf[256] = "\0";
  
    if(present->em == VAR){
        str_time = present->u_data.s_data;
    }
    else if(present->em == STORAGE){
        str_time = present->u_data.a_storage + sizeof(int);
    }
    else {
        cross_strcpy(debug_inf, "1st Shoule Be VAR or STORAGE Type\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }
  
    memset(&stm,0,sizeof(stm));  
    year = atoi(str_time);  
    month = atoi(str_time + 5);  
    day = atoi(str_time + 8);  
    hour = atoi(str_time + 11);  
    min = atoi(str_time + 14);  
    second = atoi(str_time + 17);  
  
    stm.tm_year = year - 1900;  
    stm.tm_mon = month - 1;  
    stm.tm_mday = day;  
    stm.tm_hour = hour;  
    stm.tm_min = min;  
    stm.tm_sec = second;  
  
    unixtime = mktime(&stm);  
    return long_type(unixtime);
}

void * original_unixtime(void * _left){
    time_t now;  
    long unixtime = time(&now);  
    return long_type(unixtime);
}

void * original_timestring(void * _left) {
    type * outcome = primitive_empty;
    char timestring[256] = "\0";
    time_t rawtime;
    struct tm * timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    cross_sprintf(timestring, "%d-%02d-%02d %02d:%02d:%02d", 1900 + timeinfo->tm_year, 1 + timeinfo->tm_mon, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
    outcome = new_storage(timestring, cross_strlen(timestring));
    return outcome;
}

void * original_timetostring(void * _left) {
    type * present = c_car(_left);
    type * outcome = primitive_empty;
    char debug_inf[256] = "\0";
    char timestring[256] = "\0";
    time_t rawtime;
    struct tm * timeinfo;

    if (present->em != BYTES) {
        cross_strcpy(debug_inf, "data format error\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }
    rawtime = (time_t)present->u_data.i_data;
    timeinfo = localtime(&rawtime);

    cross_sprintf(timestring, "%d-%02d-%02d %02d:%02d:%02d", 1900 + timeinfo->tm_year, 1 + timeinfo->tm_mon, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
    outcome = new_storage(timestring, cross_strlen(timestring));
    return outcome;
}

void * original_timetodata(void * _left) {
    type * present = c_car(_left);
    type * outcome = primitive_empty;
    char debug_inf[256] = "\0";
    char timestring[256] = "\0";
    time_t rawtime;
    struct tm * timeinfo;

    if (present->em != BYTES) {
        cross_strcpy(debug_inf, "data format error\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }
    rawtime = (time_t)present->u_data.i_data;
    timeinfo = localtime(&rawtime);

    cross_sprintf(timestring, "%d%02d%02d", 1900 + timeinfo->tm_year, 1 + timeinfo->tm_mon, timeinfo->tm_mday);
    outcome = new_storage(timestring, cross_strlen(timestring));
    return outcome;
}

void * original_mon(void * _left) {
    time_t rawtime;
    struct tm * timeinfo;
    int outcome = 0;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    outcome = timeinfo->tm_mon + 1;
    return long_type(outcome);
}

void * original_mday(void * _left) {
    time_t rawtime;
    struct tm * timeinfo;
    int outcome = 0;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    outcome = timeinfo->tm_mday;
    return long_type(outcome);
}

void * original_hour(void * _left) {
    time_t rawtime;
    struct tm * timeinfo;
    int outcome = 0;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    outcome = timeinfo->tm_hour;
    return long_type(outcome);
}

void * original_minute(void * _left) {
    time_t rawtime;
    struct tm * timeinfo;
    int outcome = 0;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    outcome = timeinfo->tm_min;
    return long_type(outcome);
}

void * original_decrypt(void *_left) {
    type * present = c_car(_left);
    type * outcome = primitive_empty;
    char * material = NULL;
    int len = 0;
    char debug_inf[256] = "\0";
    char * buffer_trans = (char*) cross_calloc(BUF_SIZE, sizeof(char));
  
    if(present->em == VAR){
        material = present->u_data.s_data;
        len = cross_strlen(material);
    }
    else if(present->em == STORAGE){
        material = present->u_data.a_storage + sizeof(int);
        len = *(int*)present->u_data.a_storage - 1;
    }
    else {
        cross_strcpy(debug_inf, "data format error\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }

    network_decryption((unsigned char *) material, len,
                       (unsigned char *) buffer_trans);

    outcome = new_storage(buffer_trans, len);
    cross_free(buffer_trans);
    return outcome;
}

void * original_encrypt(void *_left) {
    type * present = c_car(_left);
    type * outcome = primitive_empty;
    char * material = NULL;
    int len = 0;
    char debug_inf[256] = "\0";
    char * buffer_trans = (char*) cross_calloc(BUF_SIZE, sizeof(char));
  
    if(present->em == VAR){
        material = present->u_data.s_data;
        len = cross_strlen(material);
    }
    else if(present->em == STORAGE){
        material = present->u_data.a_storage + sizeof(int);
        len = *(int*)present->u_data.a_storage - 1;
    }
    else {
        cross_strcpy(debug_inf, " data format error\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }

    network_encryption((unsigned char *) material, len,
                       (unsigned char *) buffer_trans);

    outcome = new_storage(buffer_trans, len);
    cross_free(buffer_trans);
    return outcome;
}

void * original_exact(void *_left){
    type * left = c_car(_left);
    type * right = c_cadr(_left);
    type * result = primitive_empty;
    char * file_buffer = NULL, *begin = NULL, * pattern = NULL;
    char debug_inf[256] = "\0";

    if (left->em == EMPTY) {
        return primitive_empty;
    }
    if (left->em != STORAGE) {
        cross_strcpy(debug_inf, "1st shoule be a STORAGE\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }
    begin = left->u_data.a_storage + sizeof(int);

    if(right->em == VAR){
        pattern = right->u_data.s_data;
    }
    else if(right->em == STORAGE){
        pattern = right->u_data.a_storage + sizeof(int);
    }
    else if (right->em == EMPTY) {
        return primitive_empty;
    }
    else {
        cross_strcpy(debug_inf, "second para should be a VAR or STORAGE\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }

    if(begin != NULL) {
        begin = cross_strstr(begin, pattern);
        if(begin == NULL){
            color_fprintf(stderr, COLOR_GREEN, "found nothing, pattern is %s\r\n", pattern);
            return primitive_empty;
        }
        file_buffer = (char*) cross_calloc(BUF_SIZE, sizeof(char));
        cross_sprintf(file_buffer, "%s", begin);

        result = new_storage(file_buffer, cross_strlen(file_buffer));
        cross_free(file_buffer);
        return result;
    }
    else{
        cross_strcpy(debug_inf, "material is INVALID\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }
}

void * original_decompressbytes(void *_left) {
    type * left = c_car(_left);
    type * result = primitive_empty;
    int size = 0, i = 0;
    unsigned char * begin = NULL;
    char debug_inf[256] = "\0";

    if(left->em == VAR) {
        begin = left->u_data.s_data;
        size = cross_strlen(begin);
    }
    else if(left->em == STORAGE) {
        begin = left->u_data.a_storage + sizeof(int);
        size = *(int*)(left->u_data.a_storage) - 1;
    }
    else if(left->em == EMPTY) {
        return primitive_empty;
    }
    else {
        cross_strcpy(debug_inf, "first para should be a VAR or STORAGE\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }

    if(begin != NULL && left->em != EMPTY){
        for (i = 0; i < size; i++) {
            result = c_append(result, c_cons(long_type((int)*(begin + i)),
                                             primitive_empty));
        }
    }
    return result;
}

void* original_compressbytes(void *_left) {
    type * left = c_car(_left);
    type * right = primitive_empty;
    type * result = primitive_empty;
    int size = 0;
    char * file_buffer = NULL;
    char buf[256] = "\0";
    char debug_inf[256] = "\0";
    if (left->em != LIST) {
        cross_strcpy(debug_inf, "1st should be a LIST\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }

    while (1) {
        right = c_car(left);
        buf[size++] = right->u_data.i_data;
        left = c_cdr(left);
        if(left->em == EMPTY){
            break;
        }
    }

    file_buffer = (char*)cross_calloc(BUF_SIZE, sizeof(char));
    memcpy(file_buffer, buf, size);
    result = new_storage(file_buffer, size);
    cross_free(file_buffer);
    return result;
}

void urldecode(char *p)  
{  
    int i=0;  
    while(*(p+i))  
        {  
            if ((*p=*(p+i)) == '%')  
                {  
                    *p=*(p+i+1) >= 'A' ? ((*(p+i+1) & 0XDF) - 'A') + 10 : (*(p+i+1) - '0');  
                    *p=(*p) * 16;  
                    *p+=*(p+i+2) >= 'A' ? ((*(p+i+2) & 0XDF) - 'A') + 10 : (*(p+i+2) - '0');  
                    i+=2;  
                }  
            else if (*(p+i)=='+')  
                {  
                    *p=' ';  
                }  
            p++;  
        }  
    *p='\0';  
}  

/*(md5 system)*/
typedef struct  
{  
    unsigned int count[2];  
    unsigned int state[4];  
    unsigned char buffer[64];     
}MD5_CTX;  
  
  
#define F(x,y,z) ((x & y) | (~x & z))  
#define G(x,y,z) ((x & z) | (y & ~z))  
#define H(x,y,z) (x^y^z)  
#define I(x,y,z) (y ^ (x | ~z))  
#define ROTATE_LEFT(x,n) ((x << n) | (x >> (32-n)))
#define FF(a,b,c,d,x,s,ac){    a += F(b,c,d) + x + ac;    a = ROTATE_LEFT(a,s);    a += b;}  
#define GG(a,b,c,d,x,s,ac){    a += G(b,c,d) + x + ac;    a = ROTATE_LEFT(a,s);    a += b;}  
#define HH(a,b,c,d,x,s,ac){    a += H(b,c,d) + x + ac;    a = ROTATE_LEFT(a,s);    a += b;}  
#define II(a,b,c,d,x,s,ac){    a += I(b,c,d) + x + ac;    a = ROTATE_LEFT(a,s);    a += b;}                                              

void MD5Init(MD5_CTX *context);  
void MD5Update(MD5_CTX *context,unsigned char *input,unsigned int inputlen);  
void MD5Final(MD5_CTX *context,unsigned char digest[16]);  
void MD5Transform(unsigned int state[4],unsigned char block[64]);  
void MD5Encode(unsigned char *output,unsigned int *input,unsigned int len);  
void MD5Decode(unsigned int *output,unsigned char *input,unsigned int len);  
  

unsigned char PADDING[]={0x80,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  
                         0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  
                         0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  
                         0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};  
  
void MD5Init(MD5_CTX *context)  
{  
    context->count[0] = 0;  
    context->count[1] = 0;  
    context->state[0] = 0x67452301;  
    context->state[1] = 0xEFCDAB89;  
    context->state[2] = 0x98BADCFE;  
    context->state[3] = 0x10325476;  
}  

void MD5Update(MD5_CTX *context, unsigned char *input, unsigned int inputlen)  
{  
    unsigned int i = 0,index = 0,partlen = 0;  
    index = (context->count[0] >> 3) & 0x3F;  
    partlen = 64 - index;  
    context->count[0] += inputlen << 3;  
    if(context->count[0] < (inputlen << 3))  
        context->count[1]++;  
    context->count[1] += inputlen >> 29;  
  
    if(inputlen >= partlen)  
        {  
            cross_memcpy(&context->buffer[index],input,partlen);  
            MD5Transform(context->state,context->buffer);  
            for(i = partlen;i+64 <= inputlen;i+=64)  
                MD5Transform(context->state,&input[i]);  
            index = 0;          
        }    
    else  
        {  
            i = 0;  
        }  
    cross_memcpy(&context->buffer[index],&input[i],inputlen-i);  
}  
void MD5Final(MD5_CTX *context,unsigned char digest[16])  
{  
    unsigned int index = 0,padlen = 0;  
    unsigned char bits[8];  
    index = (context->count[0] >> 3) & 0x3F;  
    padlen = (index < 56)?(56-index):(120-index);  
    MD5Encode(bits,context->count,8);  
    MD5Update(context, PADDING, padlen);  
    MD5Update(context, bits, 8);  
    MD5Encode(digest, context->state, 16);  
}  
void MD5Encode(unsigned char *output,unsigned int *input,unsigned int len)  
{  
    unsigned int i = 0,j = 0;  
    while(j < len)  
        {  
            output[j] = input[i] & 0xFF;    
            output[j+1] = (input[i] >> 8) & 0xFF;  
            output[j+2] = (input[i] >> 16) & 0xFF;  
            output[j+3] = (input[i] >> 24) & 0xFF;  
            i++;  
            j+=4;  
        }  
}  
void MD5Decode(unsigned int *output,unsigned char *input,unsigned int len)  
{  
    unsigned int i = 0,j = 0;  
    while(j < len)  
        {  
            output[i] = (input[j]) |  
                (input[j+1] << 8) |  
                (input[j+2] << 16) |  
                (input[j+3] << 24);  
            i++;  
            j+=4;   
        }  
}  
void MD5Transform(unsigned int state[4],unsigned char block[64])  
{  
    unsigned int a = state[0];  
    unsigned int b = state[1];  
    unsigned int c = state[2];  
    unsigned int d = state[3];  
    unsigned int x[64];  
    MD5Decode(x,block,64);  
    FF(a, b, c, d, x[ 0], 7, 0xd76aa478);   
    FF(d, a, b, c, x[ 1], 12, 0xe8c7b756);   
    FF(c, d, a, b, x[ 2], 17, 0x242070db);   
    FF(b, c, d, a, x[ 3], 22, 0xc1bdceee);   
    FF(a, b, c, d, x[ 4], 7, 0xf57c0faf);   
    FF(d, a, b, c, x[ 5], 12, 0x4787c62a);   
    FF(c, d, a, b, x[ 6], 17, 0xa8304613);   
    FF(b, c, d, a, x[ 7], 22, 0xfd469501);   
    FF(a, b, c, d, x[ 8], 7, 0x698098d8);   
    FF(d, a, b, c, x[ 9], 12, 0x8b44f7af);   
    FF(c, d, a, b, x[10], 17, 0xffff5bb1);   
    FF(b, c, d, a, x[11], 22, 0x895cd7be);   
    FF(a, b, c, d, x[12], 7, 0x6b901122);   
    FF(d, a, b, c, x[13], 12, 0xfd987193);   
    FF(c, d, a, b, x[14], 17, 0xa679438e);   
    FF(b, c, d, a, x[15], 22, 0x49b40821);   
  
      
    GG(a, b, c, d, x[ 1], 5, 0xf61e2562);   
    GG(d, a, b, c, x[ 6], 9, 0xc040b340);   
    GG(c, d, a, b, x[11], 14, 0x265e5a51);   
    GG(b, c, d, a, x[ 0], 20, 0xe9b6c7aa);   
    GG(a, b, c, d, x[ 5], 5, 0xd62f105d);   
    GG(d, a, b, c, x[10], 9,  0x2441453);   
    GG(c, d, a, b, x[15], 14, 0xd8a1e681);   
    GG(b, c, d, a, x[ 4], 20, 0xe7d3fbc8);   
    GG(a, b, c, d, x[ 9], 5, 0x21e1cde6);   
    GG(d, a, b, c, x[14], 9, 0xc33707d6);   
    GG(c, d, a, b, x[ 3], 14, 0xf4d50d87);   
    GG(b, c, d, a, x[ 8], 20, 0x455a14ed);   
    GG(a, b, c, d, x[13], 5, 0xa9e3e905);   
    GG(d, a, b, c, x[ 2], 9, 0xfcefa3f8);   
    GG(c, d, a, b, x[ 7], 14, 0x676f02d9);   
    GG(b, c, d, a, x[12], 20, 0x8d2a4c8a);   
  
      
    HH(a, b, c, d, x[ 5], 4, 0xfffa3942);   
    HH(d, a, b, c, x[ 8], 11, 0x8771f681);   
    HH(c, d, a, b, x[11], 16, 0x6d9d6122);   
    HH(b, c, d, a, x[14], 23, 0xfde5380c);   
    HH(a, b, c, d, x[ 1], 4, 0xa4beea44);   
    HH(d, a, b, c, x[ 4], 11, 0x4bdecfa9);   
    HH(c, d, a, b, x[ 7], 16, 0xf6bb4b60);   
    HH(b, c, d, a, x[10], 23, 0xbebfbc70);   
    HH(a, b, c, d, x[13], 4, 0x289b7ec6);   
    HH(d, a, b, c, x[ 0], 11, 0xeaa127fa);   
    HH(c, d, a, b, x[ 3], 16, 0xd4ef3085);   
    HH(b, c, d, a, x[ 6], 23,  0x4881d05);   
    HH(a, b, c, d, x[ 9], 4, 0xd9d4d039);   
    HH(d, a, b, c, x[12], 11, 0xe6db99e5);   
    HH(c, d, a, b, x[15], 16, 0x1fa27cf8);   
    HH(b, c, d, a, x[ 2], 23, 0xc4ac5665);   
  
      
    II(a, b, c, d, x[ 0], 6, 0xf4292244);   
    II(d, a, b, c, x[ 7], 10, 0x432aff97);   
    II(c, d, a, b, x[14], 15, 0xab9423a7);   
    II(b, c, d, a, x[ 5], 21, 0xfc93a039);   
    II(a, b, c, d, x[12], 6, 0x655b59c3);   
    II(d, a, b, c, x[ 3], 10, 0x8f0ccc92);   
    II(c, d, a, b, x[10], 15, 0xffeff47d);   
    II(b, c, d, a, x[ 1], 21, 0x85845dd1);   
    II(a, b, c, d, x[ 8], 6, 0x6fa87e4f);   
    II(d, a, b, c, x[15], 10, 0xfe2ce6e0);   
    II(c, d, a, b, x[ 6], 15, 0xa3014314);   
    II(b, c, d, a, x[13], 21, 0x4e0811a1);   
    II(a, b, c, d, x[ 4], 6, 0xf7537e82);   
    II(d, a, b, c, x[11], 10, 0xbd3af235);   
    II(c, d, a, b, x[ 2], 15, 0x2ad7d2bb);   
    II(b, c, d, a, x[ 9], 21, 0xeb86d391);   
    state[0] += a;  
    state[1] += b;  
    state[2] += c;  
    state[3] += d;  
}  

void * original_md(void * _left) {
    type * left = c_car(_left);
    type * outcome = primitive_empty;
    char * source = NULL;
    char response[256] = "\0";

    char tmp[3]={'\0'};
    int i = 0;
    unsigned char decrypt[16];  
    char  debug_inf[256] = "\0";
    MD5_CTX md5;  

    if(left->em == VAR){
        source = left->u_data.s_data;
    }
    else if(left->em == STORAGE){
        source = left->u_data.a_storage + sizeof(int);
    }
    else{
        cross_strcpy(debug_inf, "1st should be STORAGE or VAR\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }

    MD5Init(&md5);           
    MD5Update(&md5, (unsigned char*)source, (unsigned int)cross_strlen(source));
    MD5Final(&md5, decrypt);   
    for(i = 0; i < 16; i++) {  
        cross_sprintf(tmp, "%02X", decrypt[i]); 
        strcat(response, tmp);
    }  

    outcome = new_storage(response, cross_strlen(response));
    return outcome;
}
/*(md5 system end)*/


/*(base64 system)*/
static char basis_64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
char base64_sep = '=';
#define CHAR64(c)  (((c) < 0 || (c) > 127) ? -1 : index_64[(c)])
static signed char index_64[128] = {    
    -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, 
    -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, 
    -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,62, -1,-1,-1,63, 
    52,53,54,55, 56,57,58,59, 60,61,-1,-1, -1,-1,-1,-1, 
    -1, 0, 1, 2,  3, 4, 5, 6,  7, 8, 9,10, 11,12,13,14, 
    15,16,17,18, 19,20,21,22, 23,24,25,-1, -1,-1,-1,-1, 
    -1,26,27,28, 29,30,31,32, 33,34,35,36, 37,38,39,40, 
    41,42,43,44, 45,46,47,48, 49,50,51,-1, -1,-1,-1,-1  
} ;
    
char *base64_encode(unsigned char *value, int vlen, char * result) {
    unsigned char oval = 0 ;    
    char *out = result; 
    while (vlen >= 3) { 
        *out++ = basis_64[value[0] >> 2];   
        *out++ = basis_64[((value[0] << 4) & 0x30) | (value[1] >> 4)];  
        *out++ = basis_64[((value[1] << 2) & 0x3C) | (value[2] >> 6)];  
        *out++ = basis_64[value[2] & 0x3F]; 
        value += 3; 
        vlen -= 3;  
    }
    if (vlen > 0) { 
        *out++ = basis_64[value[0] >> 2];   
        oval = (value[0] << 4) & 0x30 ; 
        if (vlen > 1) oval |= value[1] >> 4;    
        *out++ = basis_64[oval];    
        *out++ = (vlen < 2) ? base64_sep : basis_64[(value[1] << 2) & 0x3C];    
        *out++ = base64_sep;    
    }   
    *out = '\0';        
    return result;  
}   

unsigned char *base64_decode(char *value, int *rlen, unsigned char *result) 
{       
    int c1, c2, c3, c4;             
    unsigned char *out = result;    
    
    *rlen = 0;
    
    while (1) { 
        if (value[0]==0) {
            *out = '\0' ; 
            return result;  
        }
        c1 = value[0];  
        if (CHAR64(c1) == -1) goto base64_decode_error;
        c2 = value[1];  
        if (CHAR64(c2) == -1) goto base64_decode_error;
        c3 = value[2];  
        if ((c3 != base64_sep) && (CHAR64(c3) == -1)) goto base64_decode_error;
        c4 = value[3];  
        if ((c4 != base64_sep) && (CHAR64(c4) == -1)) goto base64_decode_error; 
        value += 4; 
        *out++ = (CHAR64(c1) << 2) | (CHAR64(c2) >> 4); 
        *rlen += 1; 
        if (c3 != base64_sep) { 
            *out++ = ((CHAR64(c2) << 4) & 0xf0) | (CHAR64(c3) >> 2);    
            *rlen += 1; 
            if (c4 != base64_sep) { 
                *out++ = ((CHAR64(c3) << 6) & 0xc0) | CHAR64(c4);   
                *rlen += 1; 
            }
        }   
    }   
 base64_decode_error:   
    *result = 0;    
    *rlen = 0;  
    return result;  
}

const char HEX2DEC[256] = 
    {
        /*       0  1  2  3   4  5  6  7   8  9  A  B   C  D  E  F */
        /* 0 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
        /* 1 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
        /* 2 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
        /* 3 */  0, 1, 2, 3,  4, 5, 6, 7,  8, 9,-1,-1, -1,-1,-1,-1,
    
        /* 4 */ -1,10,11,12, 13,14,15,-1, -1,-1,-1,-1, -1,-1,-1,-1,
        /* 5 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
        /* 6 */ -1,10,11,12, 13,14,15,-1, -1,-1,-1,-1, -1,-1,-1,-1,
        /* 7 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    
        /* 8 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
        /* 9 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
        /* A */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
        /* B */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    
        /* C */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
        /* D */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
        /* E */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
        /* F */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1
    };
    
void uri_decode(unsigned char * psrc, int len, char * pres, int * plen)
{
    /* Note from RFC1630:  "Sequences which start with a percent sign
       but are not followed by two hexadecimal characters (0-9, A-F) are reserved
       for future extension"*/
    int SRC_LEN = len ; 
    unsigned char *  SRC_END = psrc + SRC_LEN ;
    unsigned char *  SRC_LAST_DEC = SRC_END - 2;

    char * pstart = (char *)cross_malloc(SRC_LEN) ;
    char * pend = pstart ;

    while (psrc < SRC_LAST_DEC) {
        if (*psrc == '%') {
            signed char dec1, dec2;
            if ((-1 != (dec1 = HEX2DEC[*(psrc + 1)]))
                && (-1 != (dec2 = HEX2DEC[*(psrc + 2)])))  {
                *pend++ = (dec1 << 4) + dec2;
                psrc += 3;
                continue;
            }
        }
        *pend++ = *psrc++;
    }

    while (psrc < SRC_END) *pend++ = *psrc++;
    *plen = (pend - pstart); 
    cross_memcpy(pres, pstart, *plen); 
    cross_free(pstart);
}

char SAFE[256] =
    {
        /*      0 1 2 3  4 5 6 7  8 9 A B  C D E F */
        /* 0 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
        /* 1 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
        /* 2 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
        /* 3 */ 1,1,1,1, 1,1,1,1, 1,1,0,0, 0,0,0,0,
    
        /* 4 */ 0,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,
        /* 5 */ 1,1,1,1, 1,1,1,1, 1,1,1,0, 0,0,0,0,
        /* 6 */ 0,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,
        /* 7 */ 1,1,1,1, 1,1,1,1, 1,1,1,0, 0,0,0,0,
    
        /* 8 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
        /* 9 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
        /* A */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
        /* B */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    
        /* C */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
        /* D */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
        /* E */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
        /* F */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0
    };

void uri_encode(unsigned char * psrc, int len, char * pres, int * plen)
{
    
    char DEC2HEX[16 + 1] = "0123456789ABCDEF";
    int SRC_LEN = len ; 
    unsigned char * pstart = (unsigned char *)cross_malloc(SRC_LEN * 3) ;
    unsigned char * pend = pstart;
    unsigned char * SRC_END = psrc + SRC_LEN;
    for (; psrc < SRC_END; ++psrc) {
        if (SAFE[*psrc]) {
            *pend++ = *psrc;
        } 
        else {
            /* escape this char*/
            *pend++ = '%';
            *pend++ = DEC2HEX[*psrc >> 4];
            *pend++ = DEC2HEX[*psrc & 0x0F];
        }
    }
    *plen = pend - pstart; 
    cross_memcpy(pres, pstart, *plen) ; 
    cross_free(pstart);
}


void * original_encode(void * _left) {
    type * left = c_car(_left);
    type * outcome = primitive_empty;
    char * source = NULL;
    char * response = (char*) cross_calloc(BUF_SIZE, sizeof(char));
    int    source_size = 0;
    char * response_64= (char*) cross_calloc(BUF_SIZE, sizeof(char));
    int storage_size = 0;
    char debug_inf[256] = "\0";

    if(left->em == VAR){
        source = left->u_data.s_data;
        source_size = cross_strlen(source);
    }
    else if(left->em == STORAGE){
        source = left->u_data.a_storage + sizeof(int);
        source_size = *(int*)left->u_data.a_storage;
    }
    else{
        cross_free(response);
        cross_free(response_64);
        cross_strcpy(debug_inf, "1st Shoule Be VAR or STORAGE Type\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }
    uri_encode((unsigned char*)source, source_size, response, &storage_size); 
    base64_encode((unsigned char*)response, storage_size, response_64); 
    cross_free(response);

    outcome = new_storage(response_64, cross_strlen(response_64));
    cross_free(response_64);
    return outcome;
}

void * original_decode(void * _left) {
    type * left = c_car(_left);
    type * outcome = primitive_empty;
    char * source = NULL;
    char * response = (char*) cross_calloc(BUF_SIZE, sizeof(char));
    int    source_size = 0;
    char * response_64= (char*) cross_calloc(BUF_SIZE, sizeof(char));
    int storage_size = 0;
    char debug_inf[256] = "\0";

    if(left->em == VAR){
        source = left->u_data.s_data;
        source_size = cross_strlen(source);
    }
    else if(left->em == STORAGE){
        source = left->u_data.a_storage + sizeof(int);
        source_size = *(int*)left->u_data.a_storage;
    }
    else{
        cross_free(response);
        cross_free(response_64);
        cross_strcpy(debug_inf, "1st Shoule Be VAR or STORAGE Type\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }
    base64_decode(source, &source_size, (unsigned char*)response_64);
    uri_decode((unsigned char  *)response_64, source_size, response, &storage_size);
    cross_free(response_64);

    outcome = new_storage(response, cross_strlen(response));
    cross_free(response);
    return outcome;
}
/*(base64 system)*/

void* original_fflush(void *_left) {
    return primitive_empty;
}

void * original_itoa(void *_left) {
    type * left = c_car(_left);
    type * outcome = primitive_empty;
    int val = left->u_data.i_data;
    outcome = new_object();
    outcome->em = VAR;
    cross_sprintf(outcome->u_data.s_data, "%ld", val);
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
        cross_strcpy(debug_inf, "1st should be a VAR or STORAGE\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }

    outcome = long_type(value);
    return outcome;
}

void * original_len(void *_left) {
  return original_strlen(_left);
}

void * original_size(void *_left) {
    type * outcome = primitive_empty;
    type * left = c_car(_left);
    int val = left->obj_length;
    outcome = long_type(val);
    return outcome;
}

int matchhere(char *regexp, char *text, int label);
int grep_match(char *text, char *regexp, int label) { 
    if (regexp[0] == '^') 
        return matchhere(regexp + 1, text, label); 
    do {    /* must look even if string is empty */ 
        if (-1 != matchhere(regexp, text, label)) 
            return label; 
    } while (*text++ != '\0' && ++label); 
    return -1;
} 

int matchstar(int c, char *regexp, char *text, int label);
int matchhere(char *regexp, char *text, int label) { 
    if (regexp[0] == '\0') 
        return label;
    if (regexp[1] == '*') 
        return matchstar(regexp[0], regexp+2, text, label); 
    if (regexp[0] == '$' && regexp[1] == '\0') 
        if(*text == '\0'){
            return label;
        }
    if (*text!='\0' && (regexp[0]=='.' || regexp[0]==*text)) 
        return matchhere(regexp+1, text+1, label); 
    return -1;
} 

int matchstar(int c, char *regexp, char *text, int label) { 
    do {   /* a * matches zero or more instances */ 
        if (-1 != matchhere(regexp, text, label)) 
            return label; 
    } while (*text != '\0' && (*text++ == c || c == '.')); 
    return -1;
} 

void * original_grep(void *_left) {
    type * outcome = primitive_empty;
    type * left = c_car(_left);
    type * right = c_cadr(_left);
    char* source = NULL;
    char* pattern = NULL;
    int target = 0;
    char debug_inf[256] = "\0";

    if(left->em == VAR){
        source = left->u_data.s_data;
    }
    else if(left->em == STORAGE){
        source = left->u_data.a_storage + sizeof(int);
    }
    else {
        cross_strcpy(debug_inf, "1st should be a VAR or STORAGE\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }

    if(right->em == VAR){
        pattern = right->u_data.s_data;
    }
    else if(right->em == STORAGE){
        pattern = right->u_data.a_storage + sizeof(int);
    }
    else {
        cross_strcpy(debug_inf, "2st should be a VAR or STORAGE\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }

    target = grep_match(source, pattern, 0);
    if(target != -1) {
        outcome = long_type(target);
    }
    return outcome;
}

void * original_find(void *_left) {
    type * outcome = primitive_empty;
    type * left = c_car(_left);
    type * right = c_cadr(_left);
    char* source = NULL;
    char* pattern = NULL;
    char* target = NULL;
    char debug_inf[256] = "\0";

    if(left->em == VAR){
        source = left->u_data.s_data;
    }
    else if(left->em == STORAGE){
        source = left->u_data.a_storage + sizeof(int);
    }
    else {
        cross_strcpy(debug_inf, "1st should be a VAR or STORAGE\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }

    if(right->em == VAR){
        pattern = right->u_data.s_data;
    }
    else if(right->em == STORAGE){
        pattern = right->u_data.a_storage + sizeof(int);
    }
    else {
        cross_strcpy(debug_inf, "2st should be a VAR or STORAGE\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }

    target = cross_strstr(source, pattern);
    if(target != NULL){
        outcome = long_type(target - source);
    }
    return outcome;
}

void * original_strstr(void *_left) {
    type * outcome = primitive_empty;
    type * left = c_car(_left);
    type * right = c_cadr(_left);
    char* source = NULL;
    char* pattern = NULL;
    char* target = NULL;
    char debug_inf[256] = "\0";

    if(left->em == VAR){
        source = left->u_data.s_data;
    }
    else if(left->em == STORAGE){
        source = left->u_data.a_storage + sizeof(int);
    }
    else {
        cross_strcpy(debug_inf, "1st should be a VAR or STORAGE\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }

    if(right->em == VAR){
        pattern = right->u_data.s_data;
    }
    else if(right->em == STORAGE){
        pattern = right->u_data.a_storage + sizeof(int);
    }
    else {
        cross_strcpy(debug_inf, "2st should be a VAR or STORAGE\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }

    target = cross_strstr(source, pattern);
    if (target != NULL) {
      outcome = new_storage(target, cross_strlen(target));
    }
    return outcome;
}

void *list_operation(void *_left, int slice){
    type * left = c_car(_left);
    type * right = c_cdr(_left);

    if(slice == 0){
        return primitive_empty;
    }
    else{
        right = list_operation(right, slice - 1);
        return  c_cons(c_normal_copy(left), right);
    }
}

void * original_head(void *_left) {
    type * outcome = primitive_empty;
    type * left = c_car(_left);
    type * right = c_cadr(_left);
    char debug_inf[256] = "\0";
    unsigned long val = 0;

    if (left->em != LIST) {
        cross_strcpy(debug_inf, "1st should be a LIST\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }
    
    if (right->em != BYTES) {
        cross_strcpy(debug_inf, "2st should be a BYTES\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }
    val = right->u_data.i_data;
    outcome = list_operation(left, val); 
    return outcome;
}

void * original_tail(void *_left) {
    type * outcome = primitive_empty;
    type * left = c_car(_left);
    type * right = c_cadr(_left);
    char debug_inf[256] = "\0";
    unsigned long val = 0;

    if (left->em != LIST) {
        cross_strcpy(debug_inf, "1st should be a LIST\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }
        
    if (right->em != BYTES) {
        cross_strcpy(debug_inf, "2st should be a BYTES\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }
    val = right->u_data.i_data;
    while(1){
        if(val == 0)break;
        left = c_cdr(left);
        --val;
    }
    outcome = c_normal_copy(left);
    return outcome;
}

void * original_atoi(void *_left) {
    type * left = c_car(_left);
    type * outcome = primitive_empty;

    outcome = new_object();
    outcome->em = BYTES;
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
    return outcome;
}

void * varsort_helper(void *_left, int order) {
    type * left = c_car(_left);
    type * right = NULL, * head = NULL, ** contain = NULL;
    type * outcome = primitive_empty;
    char debug_inf[256] = "\0";
    int len = left->obj_length;
    int i = 0, j = 0, mode = 0, relation = 0;

    if (left->em != LIST) {
        cross_strcpy(debug_inf, "varsort_helper should be a list\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }

    contain = (type **)cross_calloc(sizeof(type*), len);
    head = left;
    while(1){
        if(head->em == EMPTY)break;
        contain[i++] = c_car(head);
        head = c_cdr(head);
    }

    if(left->em == VAR && right->em == VAR){
        mode = 0;
    }
    else{
        mode = 1;
    }

    for(i = 0; i < len; i++){
        for(j = i + 1; j < len; j++){
            left = contain[i];
            right = contain[j];
            if(mode == 1){
                relation = strcmp(left->u_data.a_storage + sizeof(int), right->u_data.a_storage + sizeof(int)) < 0;
                if(!order)relation = 1 - relation;
                if( relation){
                    contain[i] = right;
                    contain[j] = left;
                } 
            }
            else{
            }
        }
    }
    for(i = 0; i < len; i++) {
        outcome = c_append(outcome, c_cons(c_normal_copy(contain[i]),
                                           primitive_empty));
    }

    cross_free(contain);
    return outcome;
}

void * original_varsort(void *_left) {
    return varsort_helper(_left, 1);
}

void * original_varsortx(void *_left) {
    return varsort_helper(_left, 0);
}

void * concat_helper(void *_left, char *delim, int ignore_nil) {
    type * left = c_car(_left);
    type * present = NULL;
    type * outcome = primitive_empty;
    char * response = (char*) cross_calloc(BUF_SIZE, sizeof(char));
    int    response_length = 0;
    size_t delim_size = cross_strlen(delim);

    if(left->em == EMPTY){
        cross_free(response);
        return outcome;
    }
    else{
        while(1){
            if(left->em == EMPTY){
                break;
            }
            present = c_car(left);
            if(present->em == EMPTY){
                if(ignore_nil == 0){
                    cross_strcpy(response + response_length, delim);
                    response_length += delim_size;
                }
            }
            else{
                if(present->em == BYTES){
                    cross_sprintf(response + response_length, "%lu", present->u_data.i_data);
                    response_length = cross_strlen(response);
                }
                else if(present->em == VAR){
                    cross_sprintf(response + response_length, "%s", present->u_data.s_data);
                    response_length += cross_strlen(present->u_data.s_data);
                }
                else if(present->em == STORAGE && (*(int*)(present->u_data.a_storage) != 1)){
                    cross_memcpy(response + response_length, 
                                 present->u_data.a_storage + sizeof(int), 
                                 *(int*)(present->u_data.a_storage) - 1);
                    response_length += *(int*)(present->u_data.a_storage) - 1;
                }
                else{}
                cross_strcpy(response + response_length, delim);
                response_length += delim_size;
            }
            left = c_cdr(left);
        }
        if(response_length == 0){
            cross_free(response);
            return outcome;
        }
        else{
            response[response_length - delim_size] = '\0';
            outcome = new_storage(response, cross_strlen(response));
            cross_free(response);
            return outcome;
        }
    }
}

void * original_concat(void *_left) {
    type * left = _left;
    type * present = NULL;
    type * outcome = primitive_empty;
    char * response = (char*) cross_calloc(BUF_SIZE, sizeof(char));
    int    response_length = 0;

    if(left->em == EMPTY) {
        cross_free(response);
        return outcome;
    }
    else{
        while(1){
            if(left->em == EMPTY){
                break;
            }
            present = c_car(left);
            if(present->em == BYTES) {
                cross_sprintf(response + response_length, "%lu", present->u_data.i_data);
                response_length = cross_strlen(response);
            }
            else if(present->em == VAR) {
                cross_sprintf(response + response_length, "%s", present->u_data.s_data);
                response_length += cross_strlen(present->u_data.s_data);
            }
            else if((present->em == STORAGE) || (present->em == DEBUG)){
                cross_memcpy(response + response_length, 
                             present->u_data.a_storage + sizeof(int), 
                             *(int*)(present->u_data.a_storage) - 1);
                response_length += *(int*)(present->u_data.a_storage) - 1;
                /*  -1 is very important, because we use direct length, but it's contain tail \0 */
            }
            else{}
            left = c_cdr(left);
        }
        outcome = new_storage(response, response_length);
        cross_free(response);
        return outcome;
    }
}

void * original_tabconcat(void *_left) {
    return concat_helper(_left, "\t", 0);
}

void * original_andconcat(void *_left) {
    return concat_helper(_left, "&", 0);
}

void * original_spaceconcat(void *_left) {
    return concat_helper(_left, " ", 0);
}

void * original_dotconcat(void *_left) {
    return concat_helper(_left, ".", 0);
}

void * original_enterconcat(void *_left) {
    return concat_helper(_left, "\n", 1);
}

void * original_lineconcat(void *_left) {
    return concat_helper(_left, "\r\n", 0);
}

void * original_addconcat(void *_left) {
    return concat_helper(_left, "+", 0);
}

void * split_helper(void *_left, char * delim) {
    type * left = c_car(_left);
    type * outcome = primitive_empty;
    type * obj = primitive_empty;
    char * response = NULL;
    int chars_len = 0;
    char debug_inf[256] = "\0", *current = NULL, * next = NULL;
    if(left->em == EMPTY){
        return primitive_empty;
    }
    response = (char*)cross_calloc(BUF_SIZE, sizeof(char));
    /*follow code is import ,because strtok would change the source data*/
    if(left->em == VAR){
        chars_len = cross_strlen(left->u_data.s_data);
        cross_memcpy(response, left->u_data.s_data, chars_len);
    }
    else if(left->em == STORAGE){
        chars_len = *(int*)(left->u_data.a_storage);
        cross_memcpy(response, (left->u_data.a_storage + sizeof(int)), chars_len);
    }
    else {
        cross_strcpy(debug_inf, "split_helper 1st should be a VAR or STORAGE\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    } 

    current = response;
    if(current == NULL || cross_strlen(current) == 0 || (chars_len == 1)){
        cross_free(response);
        return primitive_empty;
    }

    next = cross_strstr(current, delim);
    if(next == NULL){
        outcome = new_storage(current, cross_strlen(current));
        obj = c_append(obj, c_cons(outcome, primitive_empty));
        cross_free(response);
        return obj;
    }
    outcome = new_storage(current, next - current);
    obj = c_append(obj, c_cons(outcome, primitive_empty));

    current = next + cross_strlen(delim);
    while((next = cross_strstr(current, delim)) ){
        outcome = new_storage(current, next - current);
        obj = c_append(obj, c_cons(outcome, primitive_empty));
        current = next + cross_strlen(delim);
    }

    outcome = new_storage(current, cross_strlen(current));
    obj = c_append(obj, c_cons(outcome, primitive_empty));
    current = next;
    cross_free(response);
    return obj;
}

void * original_split(void *_left) {
    type * left = c_cadr(_left);
    char debug_inf[256] = "\0";
    char* delim = NULL;

    if(left->em == VAR){
        delim = left->u_data.s_data;
    }
    else if(left->em == STORAGE){
        delim = left->u_data.a_storage + sizeof(int);
    }
    else {
        cross_strcpy(debug_inf, "2st should be a VAR or STORAGE\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }

    return split_helper(_left, delim);
}

void * original_tabsplit(void *_left) {
    return split_helper(_left, "\t");
}

void * original_colsplit(void *_left) {
    return split_helper(_left, "|");
}

void * original_spacesplit(void *_left) {
    return split_helper(_left, " ");
}

void * original_andsplit(void *_left) {
    return split_helper(_left, "&");
}

void * original_dotsplit(void *_left) {
    return split_helper(_left, ",");
}

void * original_eqlsplit(void *_left) {
    return split_helper(_left, "=");
}

void * original_linesplit(void *_left) {
    return split_helper(_left, "\r\n");
}

void * original_entersplit(void *_left) {
    return split_helper(_left, "\n");
}

void * original_strip(void *_left) {
    type * present = c_car(_left);
    char debug_inf[256] = "\0";
    int    response_length = 0;

    if(present->em == VAR){
        response_length = cross_strlen(present->u_data.s_data);
        if(present->u_data.s_data[response_length - 1 ] == '\n'){
            present->u_data.s_data[response_length - 1 ] = '\0';
        } 

        response_length = cross_strlen(present->u_data.s_data);
        if(present->u_data.s_data[response_length - 1 ] == '\r'){
            present->u_data.s_data[response_length - 1 ] = '\0';
        } 

    }
    else if(present->em == STORAGE){
        response_length = *(int*)present->u_data.a_storage + sizeof(int);
        if(present->u_data.a_storage[response_length - 2 ] == '\n'){
            present->u_data.a_storage[response_length - 2 ] = '\0';
            *(int*)present->u_data.a_storage = *(int*)present->u_data.a_storage - 1;
        } 

        response_length = *(int*)present->u_data.a_storage + sizeof(int);
        if(present->u_data.a_storage[response_length - 2 ] == '\r'){
            present->u_data.a_storage[response_length - 2 ] = '\0';
            *(int*)present->u_data.a_storage = *(int*)present->u_data.a_storage - 1;
        } 
    }
    else {
        cross_strcpy(debug_inf, "1st should be a VAR or STORAGE\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }
    return  c_normal_copy(present);
}

void * original_strdup(void *_left) {
    type * present = c_car(_left);
    type * start = c_cadr(_left);
    type * end = c_caddr(_left);
    type * outcome = primitive_empty;
    char * material = NULL;
    char * response = NULL;
    int i = 0, j = 0;
    char debug_inf[256] = "\0";
    int s = 0, d = 0;
  
    if(present->em == VAR){
        material = present->u_data.s_data;
    }
    else if(present->em == STORAGE){
        material = present->u_data.a_storage + sizeof(int);
    }
    else {
        cross_strcpy(debug_inf, "1st should be a VAR or STORAGE\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }

    if(start->em != BYTES) {
        cross_strcpy(debug_inf, "2st should be a BYTES\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }
    if(end->em != BYTES) {
        cross_strcpy(debug_inf, "3st should be a BYTES\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }

    s = start->u_data.i_data, d = end->u_data.i_data;
    if ((s > d) ||
        (d > (s + BUF_SIZE))) {
        cross_strcpy(debug_inf, "begin or end label error\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }

    response = (char*) cross_calloc(BUF_SIZE, sizeof(char));
    for(i = s; i < d; i++){
        *(response + j++) = *(material + i);
    }
    outcome = new_storage(response, d - s);
    cross_free(response);
    return  outcome;
}

void * original_tolower(void *_left) {
    type * present = c_car(_left);
    type * outcome = primitive_empty;
    char * material = NULL;
    char * response = NULL;
    int i = 0, j = 0, storage_size = 0, len = 0;
    char debug_inf[256] = "\0";
  
    if(present->em == VAR){
        material = present->u_data.s_data;
        len = cross_strlen(material);
    }
    else if(present->em == STORAGE){
        material = present->u_data.a_storage + sizeof(int);
        len = *(int*)present->u_data.a_storage - 1;
    }
    else {
        cross_strcpy(debug_inf, "1st should be a VAR or STORAGE\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }

    response = (char*)cross_calloc(BUF_SIZE, sizeof(char));
    for(i = 0; i < len; i++){
        *(response + sizeof(int) + j++) = cross_tolower(*(material + i));
    }

    outcome = new_storage(response + sizeof(int), len);
    cross_free(response);
    return  outcome;
}

void * original_toupper(void *_left) {
    type * present = c_car(_left);
    type * outcome = primitive_empty;
    char * material = NULL;
    char * response = NULL;
    int i = 0, j = 0, storage_size = 0, len = 0;
    char debug_inf[256] = "\0";
  
    if(present->em == VAR){
        material = present->u_data.s_data;
        len = cross_strlen(material);
    }
    else if(present->em == STORAGE){
        material = present->u_data.a_storage + sizeof(int);
        len = *(int*)present->u_data.a_storage - 1;
    }
    else {
        cross_strcpy(debug_inf, "1st should be a VAR or STORAGE\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }

    response = (char*)cross_calloc(BUF_SIZE, sizeof(char));
    for(i = 0; i < len; i++){
        *(response + sizeof(int) + j++) = cross_toupper(*(material + i));
    }

    outcome = new_storage(response + sizeof(int), len);
    cross_free(response);
    return  outcome;
}

void c_bindvars(void * _left, void * _right, void ** _env);
void c_unbindvars(void ** _env);

#if (defined MYSELECT) || (defined MYEPOLL)  
/*(network system)*/
int init_socket(void) {
    int init_flag = 0;
#ifdef _WIN32
    //Init Windows Socket
    WSADATA Ws;
    return WSAStartup(MAKEWORD(1,1), &Ws);
#endif
    return init_flag;
}

void cleanup_socket(void) {
#ifdef _WIN32
    WSACleanup();
#endif
}

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
    type * outcome = primitive_empty;
    unsigned char * source = NULL;
    char   response[1024] = "\0";

    if(left->em == VAR){
        source = left->u_data.s_data;
    }
    else if(left->em == STORAGE){
        source = left->u_data.a_storage + sizeof(int);
    }
    else{
        color_fprintf(stderr, COLOR_GREEN, "1st shoule be VAR or STORAGE");
        return primitive_empty;
    }
    strencode(response, source);
    outcome = new_storage(response, strlen(response));
    cross_free(response);
    return outcome;
}

int peek_mesg(int sockfd, int wait_time, int big_count, int r, int w) {
    // 0->nothing 1->canread 2->canwrite 3->error
    int status = 0;
    char a;
    fd_set rfdset;
    fd_set wfdset;
    fd_set efdset;        
    struct timeval tv; 
    int cycle_time = 1;
    int error = -1, slen = sizeof(int);

    while (1) {
        FD_ZERO(&rfdset);
        FD_SET(sockfd, &rfdset);
        FD_ZERO(&wfdset);
        FD_SET(sockfd, &wfdset);
        FD_ZERO(&efdset);
        FD_SET(sockfd, &efdset);        
        tv.tv_sec = 0;
        tv.tv_usec = wait_time * cycle_time++;
        if(cycle_time > big_count){
            status = 0;
            goto out;
        }        
        switch(select(sockfd + 1, &rfdset, &wfdset, &efdset, &tv)) {
        case -1:
            status = 0;            
            goto out;
        case 0:
            if(cycle_time > big_count){
                status = 0;                
                goto out;
            }
            break;
        default:
            getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, (socklen_t *)&slen);
            if (error != 0) {
                status = 3;
                goto out;
            }
            if (r && FD_ISSET(sockfd, &rfdset)) {
                status = 1;
                goto out;
            }
            if (w && FD_ISSET(sockfd, &wfdset)) {
                status = 2;
                goto out;               
            }
            if (FD_ISSET(sockfd, &efdset)) {
                status = 3;
                goto out;               
            }            
            else{
                break;
            }
        }
    }
 out:
    return status;
}

void * original_alive(void *_left) {
    type * left = c_car(_left);
    char debug_inf[256] = "\0";

    if(left->em != NET) {
        cross_snprintf(debug_inf, 256, "1st Shoule Be NET Type\r\n");
        return new_debug(debug_inf, strlen(debug_inf));
    }

    int new_fd = left->u_data.i_data;
    int status = peek_mesg(new_fd, 50000, 2, 1, 0);
    if (status == 1) {
        return long_type(1);
    }
    else {
        return long_type(0);
    }
}

void* recv_mesg(int sockfd, int wait_time, int retry) {
    type* outcome = primitive_empty;
    char* buffer = (char*) cross_calloc(BUF_SIZE, sizeof(char));
    char* file_buffer = (char*) cross_calloc(BUF_SIZE, sizeof(char));
    int file_buffer_count = 0;
    size_t batch_size = BUF_SIZE - 10;

    int status = peek_mesg(sockfd, wait_time, retry, 1, 0);
    if (status == 1) {
        while (1) {
            status = recv(sockfd, buffer, batch_size - file_buffer_count, 0);
            if (status == -1) {
                if(file_buffer_count == 0) {
                    outcome = new_storage("invalid", 7);
                    goto ok;
                }
                else {
                    goto out;
                }
            }
            else if (status == 0) {
                goto out;
            }
            else {
                memcpy(file_buffer + file_buffer_count, buffer, sizeof(char) * status);
                file_buffer_count += status;
                if (file_buffer_count >= batch_size) {
                    goto out;
                }
            }
        }
    }

 out:
    if (file_buffer_count == 0) {
        outcome = primitive_cont;
    }
    else {
        outcome = new_storage(file_buffer, file_buffer_count);
    }

 ok:
    cross_free(buffer);
    cross_free(file_buffer);
    return  outcome;
}

void * original_close(void *_left) {
    type * left = c_car(_left);
    char debug_inf[256] = "\0";

    if (left->em == EMPTY) {
        return primitive_empty;
    }

    if (left->em != NET) {
        cross_snprintf(debug_inf, 256, "1st Shoule Be NET Type\r\n");
        return new_debug(debug_inf, strlen(debug_inf));
    }

    int new_fd = left->u_data.i_data;
    /*cross_fprintf(stderr, "close %d\r\n", new_fd);*/
    close_tcp_socket(new_fd);
    return primitive_empty;
}

void * send_mesg(int sockfd, char* buf, int size){
    int status = peek_mesg(sockfd, 50000, 2, 0, 1);
    if (status == 2) {
        status = send(sockfd, buf, size, MSG_NOSIGNAL);
        if (status == -1) {
            goto out;
        }
        if (status > 0) {
            goto out;
        }
    }
 out:
    return primitive_empty;
}

void * original_send(void *_left) {
    type * outcome = primitive_empty;
    type * left = c_car(_left);
    char  int_buf[256] = "\0";
    char * buf = NULL;
    int file_size = 0;
    char debug_inf[256] = "\0";
    FILE * fp = NULL;

    if(left->em != NET) {
        /*
        cross_snprintf(debug_inf, 256, "1st Shoule Be NET Type\r\n");
        return new_debug(debug_inf, strlen(debug_inf));
        */
        cross_fprintf(stderr, "1st Shoule Be NET Type\r\n");        
        outcome = primitive_empty;
        return outcome;
    }

    int new_fd = left->u_data.i_data;
    type * right = c_cadr(_left);
    
    if(right->em == STORAGE) {
        buf = right->u_data.a_storage + sizeof(int);
        file_size = *(int*)right->u_data.a_storage - 1;
        send_mesg(new_fd, buf, file_size);
        outcome = c_normal_copy(left);    
    }
    else if(right->em == DEBUG) {
        buf = right->u_data.a_storage + sizeof(int);
        file_size = *(int*)right->u_data.a_storage - 1;
        //        outcome = send_mesg(new_fd, buf, file_size);
        fp = fdopen(new_fd, "w+");
        color_fprintf(fp, COLOR_RED, "%s", buf);
        //        fclose(fp);
        fflush(fp);
        outcome = primitive_empty;
    }
    else if(right->em == VAR) {
        buf = right->u_data.s_data;
        send_mesg(new_fd, buf, strlen(buf));
        outcome = c_normal_copy(left);            
    }
    else if(right->em == BYTES) {
        cross_snprintf(int_buf, 256, "%x", right->u_data.i_data);/* can't do this*/
        outcome = send_mesg(new_fd, (char*)&(right->u_data.i_data), sizeof(right->u_data.i_data));
        outcome = c_normal_copy(left);            
    }
    else if(right->em == EMPTY) {
    }    
    else {
        snprintf(debug_inf, 256, "2st INVALID\r\n");
        return new_debug(debug_inf, strlen(debug_inf));
    }
    return outcome;
}

void * original_fdtoint(void *_left) {
    type * left = c_car(_left);
    type * right = primitive_empty;
    type * outcome = primitive_empty;
    int new_fd = 0;
    char debug_inf[256] = "\0";

    if (left->em == EMPTY) {
        return primitive_empty;
    }
    if(left->em != NET) {
        snprintf(debug_inf, 256, "1st Shoule Be NET Type\r\n");
        return new_debug(debug_inf, strlen(debug_inf));
    }
    new_fd = left->u_data.i_data;
    return long_type(new_fd);
}

void * original_recv(void *_left) {
    type * left = c_car(_left);
    type * right = primitive_empty;
    type * outcome = primitive_empty;
    int new_fd = 0;
    int retry = 2;
    char debug_inf[256] = "\0";
    static int wait_time = 100000;
    static int last_not_ok = 0;

    if (left->em == EMPTY) {
        return primitive_empty;
    }
    if(left->em != NET) {
        snprintf(debug_inf, 256, "1st Shoule Be NET Type\r\n");
        return new_debug(debug_inf, strlen(debug_inf));
    }
    new_fd = left->u_data.i_data;

    right = c_cdr(_left);
    if (right->em == EMPTY) {
        if (last_not_ok < 2) {
            wait_time -= 1000 * power(2, last_not_ok);
        }
        else {
            wait_time += 1000 * power(2, last_not_ok);
        }

        if (wait_time < 100) {
            wait_time = 100; 
        }

        if (wait_time > 1000000) {
            wait_time = 1000000;
        }
    }
    else {
        wait_time = 500; 
        left = c_car(right);
        if (left->em == BYTES) {
            wait_time = left->u_data.i_data;
        }
        else {
            snprintf(debug_inf, 256, "2st Shoule Be BYTES Type\r\n");
            return new_debug(debug_inf, strlen(debug_inf));
        }
    }

    //    cross_fprintf(stderr, "not_ok is %d wait_time is %d\r\n", last_not_ok, wait_time);
    outcome = recv_mesg(new_fd, wait_time, retry);
    if (right->em == EMPTY) {
        if ((outcome->em == STORAGE) &&
                 strcmp(outcome->u_data.a_storage + sizeof(int), "invalid")) {
            if (last_not_ok > 0) {
                last_not_ok--; 
            }
        }
        else  {
            if (last_not_ok < 10) {
                last_not_ok++; 
            }
        }
    }

    return outcome;
}

void * original_accept(void *_left) {
    type * left = c_car(_left);
    type * outcome = primitive_cont;    
    int new_fd;
    char debug_inf[256] = "\0";
    char* peer_ip = NULL; 

    if (left->em != NET) {
        snprintf(debug_inf, 256, "1st Shoule Be NET Type\r\n");
        return new_debug(debug_inf, strlen(debug_inf));
    }

    new_fd = left->u_data.i_data;    
    int status = peek_mesg(new_fd, 50000, 2, 1, 0);
    if (status == 1) {
        struct sockaddr_in client_addr;
        int client_len = sizeof(client_addr);
        memset(&client_addr, 0, client_len);
        new_fd = accept(new_fd, (struct sockaddr *)(&client_addr), &client_len);
        cross_fprintf(stderr, "accept socket is %d\r\n", new_fd);
        if (new_fd == -1) {
            sleep(1);
            color_fprintf(stderr, COLOR_GREEN, "errno is %d, err is %s\r\n", errno, strerror(errno));
            outcome = primitive_cont;
        }
        else {
            outcome = new_object();
            outcome->em = NET;
            outcome->u_data.i_data = new_fd;

            color_fprintf(stderr, COLOR_YELLOW, "accept ip:%s\r\n", inet_ntoa(client_addr.sin_addr));
        }        
    }
    return outcome;
}

char* dns(char * host_name) {
    struct hostent *hostentobj = NULL;
    struct in_addr iaddr;
    hostentobj = gethostbyname(host_name);
    if (hostentobj == NULL) {
        return NULL;
    }
    iaddr = *((struct in_addr *)*hostentobj->h_addr_list);
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
        if (dns(host_name) != NULL) {
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
    struct timeval ti;
    ti.tv_sec = 0;
    ti.tv_usec = 10; 
    setsockopt(*new_socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&ti,
               sizeof(ti));
    int flags = fcntl(*new_socket, F_GETFL, 0);
    fcntl(*new_socket, F_SETFL,flags | O_NONBLOCK);
    int status = connect(*new_socket, (struct sockaddr*)&server, sizeof(struct sockaddr_in));
    if (status != 0) {
        if(errno != EINPROGRESS) {
            fcntl(*new_socket,F_SETFL,flags & ~O_NONBLOCK);
            cross_fprintf(stderr, "connect error :%s\n", strerror(errno));
            return 0;
        }
        if (*new_socket > 1024) {
            sleep(1);
            cross_fprintf(stderr, "socker is %d\r\n", *new_socket);            
        }
        status = peek_mesg(*new_socket, 50000, 5, 0, 1);
        fcntl(*new_socket,F_SETFL,flags & ~O_NONBLOCK);        
        return  status;
    }
    /*
    if (connect(*new_socket, (struct sockaddr *)&server, sizeof(server)) == -1) {
        close_tcp_socket(*new_socket);
        return -1;
    }
    return 1;
    */
}

void* handleurl(char* url, char* _host, int* _port, char* _file) {
    char* s;
    char host[256];
    int host_len;
    unsigned short port;
    char* file = 0;
    char debug_inf[256];

    for (s = url; *s != '\0' && *s != ':' && *s != '/'; ++s );

    host_len = s - url;
    strncpy(host, url, host_len);
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

void * original_dns(void *_left) {
    type * present = c_car(_left);
    char * material = NULL;
    char debug_inf[256] = "\0";
  
    if(present->em == VAR){
        material = present->u_data.s_data;
    }
    else if(present->em == STORAGE){
        material = present->u_data.a_storage + sizeof(int);
    }
    else {
        snprintf(debug_inf, 256, "1st should be a VAR or STORAGE\r\n");
        return new_debug(debug_inf, strlen(debug_inf));
    }

    snprintf(debug_inf, 256, "%s", dns(material));
    return new_storage(debug_inf, strlen(debug_inf));   /* not  new_debug, it's true value */
}

void * original_peer(void *_left) {
    type * left = c_car(_left);
    char debug_inf[256] = "\0";
  
    if (left->em == EMPTY) {
        return primitive_empty;
    }
    if (left->em != NET) {
        cross_snprintf(debug_inf, 256, "1st Shoule Be NET Type\r\n");
        return new_debug(debug_inf, strlen(debug_inf));
    }
    cross_snprintf(debug_inf, 256, (char*)left->next);
    return new_storage(debug_inf, strlen(debug_inf));   /* not  new_debug, it's true value */
}

void * original_htonl(void *_left) {
    type * present = c_car(_left);
    unsigned long material = 0;
    char debug_inf[256] = "\0";
  
    if (present->em == BYTES) {
        material = present->u_data.i_data;
    }
    else {
        snprintf(debug_inf, 256, "1st should be a BYTES\r\n");
        return new_debug(debug_inf, strlen(debug_inf));
    }

    return long_type(htonl(material));
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
    struct timeval ti;
    ti.tv_sec = 0;
    ti.tv_usec = 10;     
    setsockopt(*new_socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&ti,
               sizeof(ti));

    if (bind(*new_socket, (struct sockaddr *) &server, sizeof(server))
        == -1) {
        close_tcp_socket(*new_socket);
        return -1;
    }
    return 1;
}

void * original_connect(void *_left) {
    type * right = c_car(_left);
    type * outcome = primitive_empty;

    int sockfd;
    int result = -1;
    char* url = NULL;

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
        snprintf(debug_inf, 256, "url format error.\r\n");
        return new_debug(debug_inf, strlen(debug_inf));
    }

    outcome = handleurl(url, host, &port, file);
    if(outcome->em == DEBUG) {
        return outcome;
    } 
    result = connect_host(&sockfd, host, port);
    if (result == 2) {
        outcome = new_object();
        outcome->em = NET;
        outcome->u_data.i_data = sockfd;
        return outcome;        
    }
    else {
        return primitive_empty;
    }
}

void * original_bind(void *_left) {
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
        return new_debug(debug_inf, strlen(debug_inf));
    }
    ip = ip_type->u_data.a_storage + sizeof(int);

    if (!bind_host(&sockfd, ip, port)) {
        snprintf(debug_inf, 256, "bind host error.\r\n");
        return new_debug(debug_inf, strlen(debug_inf));
    }

    result = listen(sockfd, 100);
    if (result == -1) {
        snprintf(debug_inf, 256, "listening error:%d\r\n", port);
        close_tcp_socket(sockfd);
        return new_debug(debug_inf, strlen(debug_inf));
    }
    cross_fprintf(stderr, ">>>>>>>>>>!!!\r\n");
    /*follow  modify  by  rosslyn  ,2013.8.16*/
    outcome->em = NET;
    outcome->u_data.i_data = sockfd;
    return outcome;
}
/*(network system end)*/
#endif

char* this_now = NULL;
/*(callback system)*/
typedef void * (*original_callback_user)(void * _left, void *_right);
typedef struct  callback_event{
    int mask;
    original_callback_user proc;
    void *lambda;
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


void * iterate_address(void *_left, void * forth_code[], long forth_code_ipc);
void * execution_instance(void** _env, void * forth_code[], long begin, long end);
void * event_worker(void * _lambda, void *_right) {
    void** forth_code;
    char* this = this_now;
    int forth_code_ipc;

    type * lambda = NULL, * outcome = NULL,* para = NULL;
    char debug_inf[256] = "\0";
    entry result;
    int forth_find = 0;
    entry code_result, stream_result;

    type* m_env = primitive_empty;
    void** _env = (void**)&m_env;
    lambda = _lambda;

    result = lookup((char *)(lambda->u_data.s_data),
                    global_lambda);
    if (result.u_data._expr == NULL) {
        snprintf(debug_inf, 256, "first lambda can't found %s\r\n", lambda->u_data.s_data);
        return new_debug(debug_inf, strlen(debug_inf));
    } 

    *_env = result._env;
    forth_find = (long)result.u_data._expr;
    if (forth_find != 0) {
        code_result = lookup((char *)this,
                             global_code);
        if (code_result._env) {
            forth_code = (void**)code_result.u_data._expr;
        }

        stream_result = lookup((char *)this,
                               global_stream);
        if (stream_result._env){
            forth_code_ipc = (long)stream_result._env;
        }

        para = forth_code[forth_find];
        c_bindvars(para, _right, _env);
        outcome = execution_instance(_env, forth_code, forth_find + 1, forth_code_ipc);
        this_now = this;
        c_unbindvars(_env);
    }

    /*think why?*/
    return outcome ;  /*modify pjoin mean,so there need change according.  2014.5.31*/
}
/*(callback system end)*/
struct ae_event_loop;

typedef  int ae_create_api(struct ae_event_loop *eventLoop);
typedef  void ae_free_api(struct ae_event_loop *eventLoop);
typedef  int ae_add_api(struct ae_event_loop *eventLoop, int fd, int mask);
typedef  void ae_del_api(struct ae_event_loop *eventLoop, int fd, int mask);
typedef  int ae_poll_api(struct ae_event_loop *eventLoop, struct timeval *tvp);
typedef struct ae_worker {
    ae_create_api *create;
    ae_free_api *free;
    ae_add_api *add;
    ae_del_api *del;
    ae_poll_api *poll;
} ae_worker;


/* A fired event */
typedef struct ae_fire_event {
    int fd;
    int mask;
} ae_fire_event;

/* State of an event based program */
typedef struct ae_event_loop {
    int maxfd;
    ae_worker worker;  
    callback_event events[AE_SETSIZE]; /* Registered events */
    ae_fire_event fired[AE_SETSIZE]; /* Fired events */
    int stop;
    void *apidata; /* This is used for polling API specific data */
} ae_event_loop;

void aeDeleteEventLoop(ae_event_loop *eventLoop) {
    eventLoop->worker.free(eventLoop);
    cross_free(eventLoop);
}

#ifdef MYSELECT
/*(message system)*/
/*select, 2014.0729*/

typedef struct ae_state_api_select {
    fd_set rfds, wfds;
    /* We need to have a copy of the fd sets as it's not safe to reuse
     * FD sets after select(). */
    fd_set _rfds, _wfds;
} ae_state_api_select;

static int ae_select_create(ae_event_loop *eventLoop) {
    ae_state_api_select *state = cross_calloc(sizeof(ae_state_api_select), 1);

    if (!state) return -1;
    FD_ZERO(&state->rfds);
    FD_ZERO(&state->wfds);
    eventLoop->apidata = state;
    return 0;
}

static void ae_select_free(ae_event_loop *eventLoop) {
    cross_free(eventLoop->apidata);
}

static int ae_select_add(ae_event_loop *eventLoop, int fd, int mask) {
    ae_state_api_select *state = eventLoop->apidata;

    if (mask & AE_READABLE) FD_SET(fd, &state->rfds);
    if (mask & AE_WRITABLE) FD_SET(fd, &state->wfds);
    return 0;
}

static void ae_select_del(ae_event_loop *eventLoop, int fd, int mask) {
    ae_state_api_select* state = eventLoop->apidata;

    if (mask & AE_READABLE) FD_CLR(fd, &state->rfds);
    if (mask & AE_WRITABLE) FD_CLR(fd, &state->wfds);
}

static int ae_select_poll(ae_event_loop *eventLoop, struct timeval *tvp) {
    ae_state_api_select *state = eventLoop->apidata;
    int retval, j, numevents = 0;

    memcpy(&state->_rfds,&state->rfds,sizeof(fd_set));
    memcpy(&state->_wfds,&state->wfds,sizeof(fd_set));

    retval = select(eventLoop->maxfd + 1,
                    &state->_rfds, &state->_wfds, NULL, tvp);
    if (retval > 0) {
        for (j = 0; j <= eventLoop->maxfd; j++) {
            int mask = 0;
            callback_event *fe = &eventLoop->events[j];

            if (fe->mask == AE_NONE) continue;
            if (fe->mask & AE_READABLE) {
                if (FD_ISSET(j,&state->_rfds)) {
                    mask |= AE_READABLE;
                }
            }
            if (fe->mask & AE_WRITABLE) {
                if(FD_ISSET(j,&state->_wfds)) {
                    mask |= AE_WRITABLE;
                }
            }

            eventLoop->fired[numevents].fd = j;
            eventLoop->fired[numevents].mask = mask;
            numevents++;
        }
    }
    return numevents;
}
#endif

#ifdef MYEPOLL
typedef struct ae_state_api_epoll {
    int epfd;
    struct epoll_event events[AE_SETSIZE];
} ae_state_api_epoll;

static int ae_epoll_create(ae_event_loop *eventLoop) {
    ae_state_api_epoll *state = cross_calloc(sizeof(ae_state_api_epoll), 1);

    if (!state) return -1;
    state->epfd = epoll_create(1024);
    if (state->epfd == -1) return -1;
    eventLoop->apidata = state;
    return 0;
}

static void ae_epoll_free(ae_event_loop *eventLoop) {
    ae_state_api_epoll *state = eventLoop->apidata;

    close(state->epfd);
    cross_free(state);
}

static int ae_epoll_add(ae_event_loop *eventLoop, int fd, int mask) {
    ae_state_api_epoll *state = eventLoop->apidata;
    struct epoll_event ee;
    int op = eventLoop->events[fd].mask == AE_NONE ?
        EPOLL_CTL_ADD : EPOLL_CTL_MOD;

    ee.events = 0;
    mask |= eventLoop->events[fd].mask;
    if (mask & AE_READABLE) ee.events |= EPOLLIN;
    if (mask & AE_WRITABLE) ee.events |= EPOLLOUT;
    ee.data.u64 = 0;
    ee.data.fd = fd;
    if (epoll_ctl(state->epfd,op,fd,&ee) == -1) return -1;
    return 0;
}

static void ae_epoll_del(ae_event_loop *eventLoop, int fd, int delmask) {
    ae_state_api_epoll *state = eventLoop->apidata;
    struct epoll_event ee;
    int mask = eventLoop->events[fd].mask & (~delmask);

    ee.events = 0;
    if (mask & AE_READABLE) ee.events |= EPOLLIN;
    if (mask & AE_WRITABLE) ee.events |= EPOLLOUT;
    ee.data.u64 = 0;
    ee.data.fd = fd;
    if (mask != AE_NONE) {
        epoll_ctl(state->epfd,EPOLL_CTL_MOD,fd,&ee);
    } 
    else {
        epoll_ctl(state->epfd,EPOLL_CTL_DEL,fd,&ee);
    }
}

static int ae_epoll_poll(ae_event_loop *eventLoop, struct timeval *tvp) {
    ae_state_api_epoll *state = eventLoop->apidata;
    int retval, numevents = 0;

    retval = epoll_wait(state->epfd, state->events, AE_SETSIZE,
                        tvp ? (tvp->tv_sec*1000 + tvp->tv_usec/1000) : -1);
    if (retval > 0) {
        int j;

        numevents = retval;
        for (j = 0; j < numevents; j++) {
            int mask = 0;
            struct epoll_event * e = state->events + j;

            if (e->events & EPOLLIN) mask |= AE_READABLE;
            if (e->events & EPOLLOUT) mask |= AE_WRITABLE;
            if (e->events & EPOLLERR) mask |= AE_WRITABLE;
            if (e->events & EPOLLHUP) mask |= AE_WRITABLE;
            eventLoop->fired[j].fd = e->data.fd;
            eventLoop->fired[j].mask = mask;
        }
    }
    return numevents;
}
#endif

void * original_aedel(void *_left) {
    type * left = NULL, *right = NULL, *name = NULL;
    int sockfd, mask;
    type * outcome = primitive_empty;
    ae_event_loop * obj = NULL;
    callback_event *fe = NULL;
    char debug_inf[256] = "\0";
    entry result;

    name = c_car(_left);
    if(name->em == VAR) {
        if(!strcmp(name->u_data.s_data, "read")){
            mask = AE_READABLE;
        }
        else if(!strcmp(name->u_data.s_data, "write")){
            mask = AE_WRITABLE;
        }
        else {
            snprintf(debug_inf, 256, "1st should be a SYMBOL read or write\r\n");
            return new_debug(debug_inf, strlen(debug_inf));
        }
    }

    left = c_cadr(_left);
    if(left->em == EVENT){
        obj = (ae_event_loop *)left->u_data.i_data;
    }
    else {
        snprintf(debug_inf, 256, "2st should be a EVENT object\r\n");
        return new_debug(debug_inf, strlen(debug_inf));
    }

    right = c_caddr(_left);
    if (right->em == NET){
        sockfd = right->u_data.i_data;
    }
    else {
        snprintf(debug_inf, 256, "3st should be a NET object\r\n");
        return new_debug(debug_inf, strlen(debug_inf));
    }

    if (sockfd >= AE_SETSIZE) {
        snprintf(debug_inf, 256, "3st sockfd too big\r\n");
        return new_debug(debug_inf, strlen(debug_inf));
    }

    fe = &obj->events[sockfd];
    if (fe->mask == AE_NONE){
        snprintf(debug_inf, 256, "fe mask error, AE_NONE\r\n");
        return new_debug(debug_inf, strlen(debug_inf));
    }
    fe->mask = fe->mask & (~mask);

    if (sockfd == obj->maxfd) {
        int j;
        for (j = obj->maxfd - 1; j >= 0; j--) {
            if (obj->events[j].mask != AE_NONE) {
                break;
            }
        }
        obj->maxfd = j;
    }
    left = fe->lambda;
    result = lookup((char *)(left->u_data.s_data),
                    global_lambda);
    if (result.u_data._expr == NULL) {
        snprintf(debug_inf, 256, "lambda lambda can't be found %s\r\n", left->u_data.s_data);
        return new_debug(debug_inf, strlen(debug_inf));
    } 
    gc(result._env);

    global_lambda = freeentry(left->u_data.s_data, global_lambda);
    obj->worker.del(obj, sockfd, mask);
    cross_fprintf(stderr, "aedel socket is %d %s\r\n", sockfd, left->u_data.s_data);
    gc(left);

    outcome = new_object();
    outcome->em = EVENT;
    outcome->u_data.i_data = (long)obj;
    return outcome;
}

void * original_aedestroy(void *_left) {
    type * left = NULL;
    ae_event_loop * eventLoop = NULL;
    char debug_inf[256] = "\0";

    left = c_car(_left);

    if (left->em == EVENT) {
        eventLoop = (ae_event_loop *)left->u_data.i_data;
    }
    else {
        snprintf(debug_inf, 256, "2st should be a EVENT object\r\n");
        return new_debug(debug_inf, strlen(debug_inf));
    }

    eventLoop->worker.free(eventLoop);
    cross_free(eventLoop);
    return primitive_empty;
}

void * original_aeadd(void *_left) {
    type * left = NULL, *right = NULL, *name = NULL;
    int sockfd, mask;
    type * outcome = primitive_empty;
    ae_event_loop * obj = NULL;
    callback_event *fe = NULL;
    char debug_inf[256] = "\0";

    name = c_car(_left);
    if(name->em == VAR){
        if(!strcmp(name->u_data.s_data, "read")){
            mask = AE_READABLE;
        }
        else if(!strcmp(name->u_data.s_data, "write")){
            mask = AE_WRITABLE;
        }
        else {
            snprintf(debug_inf, 256, "1st should be a symbol read or write\r\n");
            return new_debug(debug_inf, strlen(debug_inf));
        }
    }

    left = c_caddr(_left);
    if(left->em == EVENT) {
        obj = (ae_event_loop *)left->u_data.i_data;
    }
    else {
        snprintf(debug_inf, 256, "2st should be a EVENT object\r\n");
        return new_debug(debug_inf, strlen(debug_inf));
    }

    left = c_cadr(c_cddr(_left));

    if(left->em == NET) {
        sockfd = left->u_data.i_data;
    }
    else if(left->em == EMPTY) {
        snprintf(debug_inf, 256, "socket value is empty\r\n");
        return new_debug(debug_inf, strlen(debug_inf));
    }
    else{
        snprintf(debug_inf, 256, "socket value is invalid\r\n");
        return new_debug(debug_inf, strlen(debug_inf));
        /*
        right = c_car(left);
        sockfd = right->u_data.i_data;
        */
    }

    if (sockfd >= AE_SETSIZE) {
        snprintf(debug_inf, 256, "third socket value too big\r\n");
        return new_debug(debug_inf, strlen(debug_inf));
    }

    if (obj->worker.add(obj, sockfd, mask) == -1) {
        color_fprintf(stderr, COLOR_GREEN, "poll msg error \r\n");
        return primitive_empty;
    }
    fe = &obj->events[sockfd];
    fe->mask |= mask;
    fe->proc = event_worker;
    fe->lambda = c_normal_copy(c_cadr(_left));

    if (sockfd > obj->maxfd) {
        obj->maxfd = sockfd;
    }

    outcome = new_object();
    outcome->em = EVENT;
    outcome->u_data.i_data = (long)obj;
    return outcome;
}

void * original_aepoll(void *_left) {
#define STEP 20000
    type * left = NULL, * right = NULL, *outcome = NULL;
    ae_event_loop * eventLoop = NULL;
    int numevents, j = 0, trigger_time = 0;
    struct timeval tv, *tvp;
    char debug_inf[256] = "\0";
    static int wait_time = STEP;
    static int last_not_ok = 1;

    left = c_car(_left);

    if (left->em == EVENT) {
        eventLoop = (ae_event_loop *)left->u_data.i_data;
    }
    else {
        snprintf(debug_inf, 256, "1st should be a EVENT object\r\n");
        return new_debug(debug_inf, strlen(debug_inf));
    }

    if (last_not_ok < 3) {
        wait_time -= 1000 * power(2, last_not_ok);
    }
    else {
        wait_time += 1000 * power(2, last_not_ok);
    }

    if (wait_time < STEP) {
        wait_time = STEP; 
    }

    if (wait_time > 5000000) {
        wait_time = 5000000;
    }

    tv.tv_sec = 0;
    tv.tv_usec = wait_time;
    tvp = &tv;

    numevents = eventLoop->worker.poll(eventLoop, tvp);
    right = c_cadr(_left);

    for (j = 0; j < numevents; j++) {
        if (eventLoop->fired[j].mask & AE_READABLE) { /* only monitor read  */
            callback_event* fe = &eventLoop->events[eventLoop->fired[j].fd];
            outcome = fe->proc(fe->lambda, right);
            if (outcome->em == DEBUG) {
                left_fprint(stderr, outcome);
            }
            gc(outcome);
            trigger_time++;
        }
    }

    cross_fprintf(stderr, "numevents is %d %d last_not_ok %d\r\n", trigger_time, wait_time, last_not_ok);
    if (trigger_time == 0) {
        if (last_not_ok < 8) {
            last_not_ok++; 
        }
    }
    else {
        if (last_not_ok > 0) {
            last_not_ok--; 
        }
    }

    outcome = c_normal_copy(left);
    return outcome;
}

void * original_aecreate(void *_left) {
    type * left = NULL;
    type * outcome = new_object();
    int i = 0;
    char mode[256] = "\0";
    ae_worker worker;
    ae_event_loop * obj = NULL;
    char debug_inf[256] = "\0";

    left = c_car(_left);
    sprintf(mode, "%s", left->u_data.s_data);
    if (!strcmp(mode, "select")) {
#ifdef MYSELECT
        worker.create = ae_select_create;
        worker.free = ae_select_free;
        worker.add = ae_select_add;
        worker.del = ae_select_del;
        worker.poll = ae_select_poll;
#endif
    }
    else if(!strcmp(mode, "epoll")){
#ifdef MYEPOLL
        worker.create = ae_epoll_create;
        worker.free = ae_epoll_free;
        worker.add = ae_epoll_add;
        worker.del = ae_epoll_del;
        worker.poll = ae_epoll_poll;
#endif
    }
    else {
        snprintf(debug_inf, 256, "1st should be symbol epoll or select\r\n");
        return new_debug(debug_inf, strlen(debug_inf));
    }

    obj = cross_calloc(sizeof(*obj), 1);
    obj->stop = 0;
    obj->maxfd = -1;
    obj->worker = worker;
    if (obj->worker.create(obj) == -1) {
        cross_free(obj);
        snprintf(debug_inf, 256, "create error\r\n");
        return new_debug(debug_inf, strlen(debug_inf));
    }

    for (i = 0; i < AE_SETSIZE; i++) {
        obj->events[i].mask = AE_NONE;
    }

    outcome->em = EVENT;
    outcome->u_data.i_data = (long)obj;
    return outcome;
}

void remove_lambda(void *_left) {
    type * left = _left;
    entry result;
    void * _env = NULL;

    if (cross_strlen(left->u_data.s_data) == ARABIC) {
        result = lookup((char *)(left->u_data.s_data),
                        global_lambda);
        if (result.u_data._expr != NULL) {
            _env = result._env;
            if (_env == NULL) {
            } 
            else {
                gc(_env);
            }
            global_lambda = freeentry(left->u_data.s_data, global_lambda);
        }
    }
}

typedef struct  callback_time{
    long long id;
    long when_sec;
    original_callback_user proc;
    void *lambda;
} callback_time;

typedef struct at_time_loop {
    int at_count;
    int at_capacity;
    callback_time** at_min_heap;
} at_time_loop;

void at_init(at_time_loop* obj, int capacity) {
    obj->at_count = 1;
    obj->at_capacity = capacity;
    obj->at_min_heap = (callback_time**)cross_calloc(sizeof(callback_time*), capacity);
}

int at_num(at_time_loop* obj) {
    return obj->at_count;
}

int at_insert(callback_time * newnode, at_time_loop* obj) {
    callback_time * present;
    int p, num;
    obj->at_min_heap[obj->at_count] = newnode;
    num = obj->at_count;
    while (num / 2) {
        p = num / 2;
        if (obj->at_min_heap[num]->when_sec < obj->at_min_heap[p]->when_sec) {
            present = obj->at_min_heap[num];
            obj->at_min_heap[num] = obj->at_min_heap[p];
            obj->at_min_heap[p] = present;
        } else
            break;
        num = p;
    }
    obj->at_count++;
    if (obj->at_capacity < obj->at_count) {
        if ((obj->at_capacity / 100 * 99) < obj->at_count) {        
            color_fprintf(stderr, COLOR_YELLOW, "at_object is dangerous %d %d\r\n",
                          obj->at_capacity, obj->at_count);
        }        
        return -1;
    }
    else {
        return 0;
    }
}

#define  at_left(i)  2*(i)
#define  at_right(i)  2*(i)+1

callback_time * at_top(at_time_loop* obj) {
    return obj->at_min_heap[1]; 
}

callback_time * at_pop(at_time_loop* obj) {
    callback_time* present, * result = obj->at_min_heap[1];
    int exchange;
    int label = 1;
    obj->at_min_heap[label] = obj->at_min_heap[obj->at_count - 1];
    while (at_left(label) <= obj->at_count - 1) {
        if (at_right(label) <= obj->at_count - 1) {
            if (obj->at_min_heap[at_left(label)]->when_sec < obj->at_min_heap[at_right(label)]->when_sec) {
                exchange = at_left(label);
            }
            else{
                exchange = at_right(label);
            }
        }
        else {
            exchange = at_left(label);
        }

        if (obj->at_min_heap[exchange]->when_sec <= obj->at_min_heap[label]->when_sec) {
            present = obj->at_min_heap[exchange];
            obj->at_min_heap[exchange] = obj->at_min_heap[label];
            obj->at_min_heap[label] = present;
        } 
        else{
            break;
        }
        label = exchange;
    }
    obj->at_count--;
    return result;
}

void atDeleteTimeLoop(at_time_loop *timeLoop) {
    cross_free(timeLoop);
}

void atGetTime(long* seconds)
{
    *seconds = time(NULL);
}

void * original_atwait(void *_left) {
    type * left =  _left, * outcome = primitive_empty;
    at_time_loop* timeLoop = NULL;
    callback_time* te = NULL;
    long now_sec = 0;
    int wait_sec = 1;
    char debug_inf[256] = "\0";

    if(left->em != LIST) {
        cross_strcpy(debug_inf, "at least have one parameter \r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }

    left = c_car(_left);
    if(left->em == TIME){
        timeLoop = (at_time_loop *)left->u_data.i_data;
    }
    else{
        cross_strcpy(debug_inf, "1st should be TIME type \r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }

    if (at_num(timeLoop) != 1) {
        te = at_top(timeLoop);
        atGetTime(&now_sec);
        wait_sec = te->when_sec - now_sec;
        if (wait_sec <= 0) {
            wait_sec = 1;
        }
        outcome = long_type(wait_sec);
    }
    else{
        outcome = long_type(1);
    }
    return outcome;
}

void * original_atpoll(void *_left) {
    type* left =  _left, * right = primitive_empty, * outcome = primitive_empty;
    at_time_loop* timeLoop = NULL;
    callback_time* te = NULL;
    long now_sec = 0;
    char debug_inf[256] = "\0";
    entry result;

    if (left->em != LIST) {
        cross_strcpy(debug_inf, "at least have one parameter \r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }

    left = c_car(_left);
    if (left->em == TIME) {
        timeLoop = (at_time_loop *)left->u_data.i_data;
    }
    else {
        cross_strcpy(debug_inf, "1st should be TIME type \r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }

    right = c_cadr(_left);
    while (1) {
        if (at_num(timeLoop) == 1) {
            break;
        }
        atGetTime(&now_sec);
        te = at_top(timeLoop);

        if (now_sec > te->when_sec || (now_sec == te->when_sec)) {
            te = at_pop(timeLoop);
            if (te->proc) {
                outcome = te->proc(te->lambda, right);
                if (outcome->em == DEBUG) {
                    left_fprint(stderr, outcome);
                }
                gc(outcome);
                gc(te->lambda);
            }
            cross_free(te);
        }
        else {
            break;
        }
    }

    outcome = new_object();
    outcome->em = TIME;
    outcome->u_data.i_data = (long)timeLoop;
    return outcome;
}

void * original_atadd(void *_left) {
    type * left =  _left, *outcome = primitive_empty;
    at_time_loop * obj = NULL;
    long seconds = 0;
    callback_time* te = NULL;
    char debug_inf[256] = "\0";

    if (left->em != LIST) {
        cross_strcpy(debug_inf, "at least have one parameter \r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }

    left = c_car(_left);
    if (left->em == TIME) {
        obj = (at_time_loop *)left->u_data.i_data;
    }
    else {
        cross_strcpy(debug_inf, "1st should be TIME type \r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }

    left = c_cadr(_left);
    seconds = left->u_data.i_data;

    te = (callback_time*)cross_malloc(sizeof(*te));
    if (te == NULL) {
        return primitive_empty;
    }

    te->when_sec = seconds;
    te->proc = event_worker;
    te->lambda = c_normal_copy(c_caddr(_left));
    at_insert(te, obj);

    outcome = new_object();
    outcome->em = TIME;
    outcome->u_data.i_data = (long)obj;
    return outcome;
}

void * original_atcreate(void *_left) {
    int contain_count = 1000;
    type * outcome = primitive_empty;
    type * left = primitive_empty;
    at_time_loop* obj = NULL;
    char debug_inf[256] = "\0";

    left = _left;
    if (left->em != EMPTY) {
        left = c_car(_left);
        if (left->em == BYTES) {
            contain_count = left->u_data.i_data;
        }
        else {
            snprintf(debug_inf, 256, "2st Shoule Be BYTES Type\r\n");
            return new_debug(debug_inf, strlen(debug_inf));
        }
    }

    outcome = new_object();
    obj = cross_calloc(sizeof(*obj), 1);
    at_init(obj, contain_count);
    outcome->em = TIME;
    outcome->u_data.i_data = (long)obj;
    return outcome;
}
/*(message system end)*/


/*ordinal operator*/
typedef struct fun_info {
    char name[ARABIC];
    original_callback address;
    unsigned char parameters_num;
} fun_info;

typedef struct type_info {
    char name[ARABIC];
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
    t = c_car(right);
    if (!strcmp(left->u_data.s_data, ((type *)c_car(t))->u_data.s_data)) {
        return c_cadr(t);
    } 
    else {
        return c_find_var_value_help(left, c_cdr(right));
    }
}

void * c_find_var_value(void * _left, void * _env) {
    type * left = _left, *env = _env, *outcome = NULL;
    type * mid_expr;
    entry result;
    while (env->em != EMPTY) {
        if ((outcome = c_find_var_value_help(left, c_car(env))))
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

    result = lookup((char *)(left->u_data.s_data), global_var);
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

    for (i = 0; i < ARABIC; i++) {
        name[i] = cross_rand() % ARABIC + 'a';
    }

    name[i] = '\0';
    return var_type(name);
}

void * original_randomname(void * _left) {
    type * outcome = random_name();
    return outcome;
}

/* for function declare */
void * original_pcreate(void * _left);
void * original_pjoin(void * _left);
void * original_eval(void *_left);
void * original_help(void * _left);

#define   ORIGINAL_PREFIX(PTR)    original_##PTR
#define   ORIGINAL_QUOTA(PTR)    #PTR
#define   ORIGINAL_CON(PTR, NUM)        {ORIGINAL_QUOTA(PTR), ORIGINAL_PREFIX(PTR), NUM},

    fun_info normal_fun[] = {
        /*(at declare)*/
        ORIGINAL_CON(atpoll, 2)
        ORIGINAL_CON(atwait, 1)
        ORIGINAL_CON(atadd, 3)
        ORIGINAL_CON(atcreate, 100)
        ORIGINAL_CON(timestring, 0)
        ORIGINAL_CON(timetostring, 1)
        ORIGINAL_CON(timetodata, 1)
        ORIGINAL_CON(mon, 0)
        ORIGINAL_CON(mday, 0)
        ORIGINAL_CON(hour, 0)
        ORIGINAL_CON(minute, 0)
        ORIGINAL_CON(unixtime, 0)
        ORIGINAL_CON(mktime, 1)
#if (defined MYSELECT) || (defined MYEPOLL)  
        ORIGINAL_CON(aedel, 3)
        ORIGINAL_CON(aepoll, 2)
        ORIGINAL_CON(aeadd, 4)
        ORIGINAL_CON(aecreate, 1)
        ORIGINAL_CON(aedestroy, 1)
        ORIGINAL_CON(close, 1)
        ORIGINAL_CON(urlencode, 1)
        ORIGINAL_CON(dns, 1)
        ORIGINAL_CON(peer, 1)
        ORIGINAL_CON(htonl, 1)
        ORIGINAL_CON(recv, 100)
        ORIGINAL_CON(fdtoint, 1)
        ORIGINAL_CON(send, 2)
        ORIGINAL_CON(alive, 1)
        ORIGINAL_CON(accept, 1)
        ORIGINAL_CON(connect, 1)
        ORIGINAL_CON(bind, 2)
#endif
        ORIGINAL_CON(fflush, 0)
        ORIGINAL_CON(itoa, 1)
        ORIGINAL_CON(atoi, 1)
        ORIGINAL_CON(varsortx, 1)
        ORIGINAL_CON(varsort, 1)
        ORIGINAL_CON(head, 2)
        ORIGINAL_CON(tail, 2)
        ORIGINAL_CON(strlen, 1)
        ORIGINAL_CON(len, 1)
        ORIGINAL_CON(size, 1)
        ORIGINAL_CON(strstr, 2)
        ORIGINAL_CON(find, 2)
        ORIGINAL_CON(grep, 2)
        ORIGINAL_CON(strdup, 3)
        ORIGINAL_CON(split, 2)
        ORIGINAL_CON(colsplit, 1)
        ORIGINAL_CON(spacesplit, 1)
        ORIGINAL_CON(andsplit, 1)
        ORIGINAL_CON(dotsplit, 1)
        ORIGINAL_CON(linesplit, 1)
        ORIGINAL_CON(entersplit, 1)
        ORIGINAL_CON(eqlsplit, 1)
        ORIGINAL_CON(tabsplit, 1)
        ORIGINAL_CON(concat, 100)
        ORIGINAL_CON(spaceconcat, 1)
        ORIGINAL_CON(dotconcat, 1)
        ORIGINAL_CON(enterconcat, 1)
        ORIGINAL_CON(lineconcat, 1)
        ORIGINAL_CON(addconcat, 1)
        ORIGINAL_CON(tabconcat, 1)
        ORIGINAL_CON(andconcat, 1)
        ORIGINAL_CON(strip, 1)
        ORIGINAL_CON(tolower, 1)
        ORIGINAL_CON(toupper, 1)
        ORIGINAL_CON(exact, 2)
        ORIGINAL_CON(decompressbytes, 1)
        ORIGINAL_CON(compressbytes, 1)
        /*(redis declare)*/
        ORIGINAL_CON(dcreate, 0)
        ORIGINAL_CON(dset, 4)
        ORIGINAL_CON(dget, 3)
        ORIGINAL_CON(dlen, 3)
        ORIGINAL_CON(ddel, 3)
        ORIGINAL_CON(jtos, 1)
        ORIGINAL_CON(jtosx, 1)
        ORIGINAL_CON(rpush, 3)
        ORIGINAL_CON(lrem, 3)
        ORIGINAL_CON(llen, 2)
        ORIGINAL_CON(lpush, 3)
        ORIGINAL_CON(lpop, 2)
        ORIGINAL_CON(rpop, 2)
        ORIGINAL_CON(lrange, 4)
        ORIGINAL_CON(zadd, 4)
        ORIGINAL_CON(zincr, 4)
        ORIGINAL_CON(zrange, 4)
        ORIGINAL_CON(zrevrange, 4)
        ORIGINAL_CON(zrank, 3)
        ORIGINAL_CON(zrevrank, 3)
        ORIGINAL_CON(zscore, 3)
        /*(json declare)*/
        ORIGINAL_CON(jaddarray, 2)
        ORIGINAL_CON(jaddobject, 3)
        ORIGINAL_CON(jupdateobject, 3)
        ORIGINAL_CON(jdeleteobject, 2)
        ORIGINAL_CON(jcreatearray, 0)
        ORIGINAL_CON(jcreatestring, 1)
        ORIGINAL_CON(jcreateint, 1)
        ORIGINAL_CON(jcreatetrue, 0)
        ORIGINAL_CON(jcreatefalse, 0)
        ORIGINAL_CON(jcreate, 0)
        ORIGINAL_CON(makejson, 1)
        ORIGINAL_CON(killjson, 1)
        ORIGINAL_CON(jgetarrayitem, 2)
        ORIGINAL_CON(jgetkeys, 1)
        ORIGINAL_CON(jgetarraysize, 1)
        ORIGINAL_CON(jgetint, 1)
        ORIGINAL_CON(jgetstring, 1)
        ORIGINAL_CON(jgetobject, 2)
        /*(md declare)*/
        ORIGINAL_CON(decode, 1)
        ORIGINAL_CON(encode, 1)
        ORIGINAL_CON(md, 1)
        /*(fake thread declare)*/
        ORIGINAL_CON(pjoin, 1)
        ORIGINAL_CON(pcreate, 100)
        /*(native declare)*/
        ORIGINAL_CON(eval, 100)
        ORIGINAL_CON(decrypt, 1)
        ORIGINAL_CON(encrypt, 1)
        ORIGINAL_CON(primitive, 100)
        ORIGINAL_CON(primitivep, 1)
        ORIGINAL_CON(lambdap, 1)
        ORIGINAL_CON(memory, 1)
        ORIGINAL_CON(cons, 2)
        ORIGINAL_CON(system, 1)
        ORIGINAL_CON(exe, 100)
        ORIGINAL_CON(fload, 1)
        ORIGINAL_CON(funload, 2)
        ORIGINAL_CON(fopen, 2)
        ORIGINAL_CON(ftell, 1)
        ORIGINAL_CON(fread, 2)
        ORIGINAL_CON(feof, 1)
        ORIGINAL_CON(fgets, 1)
        ORIGINAL_CON(fwrite, 2)
        ORIGINAL_CON(fclose, 1)
        ORIGINAL_CON(printchar, 1)
        ORIGINAL_CON(charp, 1)
        ORIGINAL_CON(storagep, 1)
        ORIGINAL_CON(debugp, 1)
        ORIGINAL_CON(digitp, 1)
        ORIGINAL_CON(display, 0)
        ORIGINAL_CON(help, 0)
        ORIGINAL_CON(random, 1)
        ORIGINAL_CON(randomname, 0)
        ORIGINAL_CON(print, 1)
        ORIGINAL_CON(printerr, 1)
        ORIGINAL_CON(minus, 100)
        ORIGINAL_CON(add, 100)
        ORIGINAL_CON(mul, 100)
        ORIGINAL_CON(div, 100)
        ORIGINAL_CON(mod, 100)
        ORIGINAL_CON(car, 1)
        ORIGINAL_CON(cdr, 1)
        ORIGINAL_CON(atom, 1)
        ORIGINAL_CON(list, 100)
        ORIGINAL_CON(and, 100)
        ORIGINAL_CON(or, 100)
        ORIGINAL_CON(power, 2)
        ORIGINAL_CON(eq, 2)
        ORIGINAL_CON(not, 1)
        ORIGINAL_CON(big, 2)
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
    {"iter", ITER}, 
    {"sleep", SLEEP}, 
    {"pget", PGET}, 
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
    {"join", JOIN}, 
    /*all forth special symbol*/
#ifdef FORTH
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

    return left_fprint(stderr, new_storage(debug_inf, cross_strlen(debug_inf)));
}

void init_primitive(void) {
    int sign = 0;
    while (1) {
        if (!strcmp("", normal_fun[sign].name)) {
            break;
        }
        if (lookup_primitive(normal_fun[sign].name, global_primitive) != NULL) {
            color_fprintf(stderr, COLOR_GREEN, "%s alread\r\n", normal_fun[sign].name);
        }

        assign_primitive(normal_fun[sign].name, normal_fun[sign].address,
                         &global_primitive);
        sign++;
    }

}

/*similar  to  the  macro  dispatch*/
void * c_set_global_var_value(void * _name, void * value) {
    type *name = _name;
    entry result = lookup((char *)(name->u_data.s_data), global_var);
    type * mid_expr = result.u_data._expr;
    if (mid_expr == NULL) {
    } 
    else {
        gc(mid_expr);
        global_var = freeentry((char *)(name->u_data.s_data), global_var); /*add by rosslyn  2015/6/27 doing flexhash*/
    }

    assign((char *)(name->u_data.s_data), value, NULL, &global_var);
    return c_normal_copy(name);
}

void * c_set_global_define_value(void * _name, void * value) {
    type * name = _name;
    assign((char *) (name->u_data.s_data), value, primitive_empty, &global_define);
    return c_normal_copy(name);
}

void * c_find_defmacro_arg(void * name, void * mem) {
    type * _env = mem;
    type * label;
    while (_env) {
        label = c_car(_env);
        if (!strcmp(((type *)c_car (label))->u_data.s_data,
                    ((type *)name)->u_data.s_data)) {
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
        label = c_car(_env);
        if (!strcmp(((type *)c_car (label))->u_data.s_data,
                    ((type *)name)->u_data.s_data)) {
            return c_caddr(label);
        }
        _env = c_cdr(_env);
    }
    return NULL;
}

int c_find_defun_arg(void * _name, void ** _arg) {
    type * mid_expr;
    entry result;
    type *name = _name;
    result = lookup((char *)(name->u_data.s_data), global_defun);
    mid_expr = result.u_data._expr;
    if (mid_expr == NULL) {
        result = lookup((char *)(name->u_data.s_data), global_lambda);
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
    result = lookup((char *)(name->u_data.s_data), global_defun);
    mid_expr = result.u_data._expr;
    if (mid_expr == NULL) {
        result = lookup((char *)(name->u_data.s_data), global_lambda);
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

 label: 
    outcome = 0;
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
void * iterate_address(void *_left, void * forth_code[], long forth_code_ipc) {
    long i = 0;
    type *left = _left, *right = NULL;
    for (i = 0; i < forth_code_ipc; i++) {
        right = forth_code[i];
        if(right && right->em == EFDEFUN){
            right = forth_code[i+1];
            if (right && right->em == VAR &&  !strcmp(left->u_data.s_data, right->u_data.s_data)) {
                return (void*)(i + 1);
            }
        }
    }
    return NULL;
}

typedef struct linear_contain {
    int num;
    void** contain;
}linear_contain;

void linear_init(linear_contain* obj) {
    obj->num = 0;
    obj->contain = (void**)cross_calloc(sizeof(void*), 1000);
}

void* linear_push(linear_contain* obj, void* material) {
    obj->contain[obj->num++] = material;
    return material;
}

void linear_free(linear_contain* obj) {
    int i = 0;
    for (i = 0; i < obj->num; i++) {
        gc(obj->contain[i]);
    }
}


void* fetch_code_skip_address(void *_left, native_operator em, linear_contain* skip_obj) {
    int i = 0;
    type *left = _left, *right = NULL;
    for (i = 0; i < skip_obj->num;) {
        right = skip_obj->contain[i];
        if(right && right->em == em){
            right = skip_obj->contain[i+1];
            if (right && right->em == VAR &&  !strcmp(left->u_data.s_data, right->u_data.s_data)) {
                return skip_obj->contain[i + 2];
            }
        }
        i += 3;
    }
    return NULL;
}

void fix_code_address(void *_left, int real_address, void* forth_code[], linear_contain* obj) {
    forth_code[((type*)_left)->u_data.i_data] = linear_push(obj, long_type(real_address));
}

void fix_code_relative_address(void *_left, int real_address, void* forth_code[], linear_contain* obj) {
    forth_code[((type*)_left)->u_data.i_data] = linear_push(obj, long_type(real_address - ((type*)_left)->u_data.i_data));
}

void fix_unfix_code(void *_left, int real_address, void* forth_code[], linear_contain* obj) {
    long i = 0;
    type *left = _left, *right = NULL;
    for (i = 0; i < forth_code_unfix_ipc;) {
        right = forth_code_unfix[i];
        if (!strcmp(left->u_data.s_data, right->u_data.s_data)) {
            forth_code[((type*)(forth_code_unfix[i + 1]))->u_data.i_data] = linear_push(obj, long_type(real_address));
        }
        i += 2;
    }
}

/*follow  code not use now ,it's different  is  different  calculate style  2013.10.10*/
at_time_loop global_timer_loop; /* add by boris 20160709 for bthread dispatch */
void * evaljit(char *buf, int socket);
void* dispatch_core(char* that);
void * original_eval(void *_left) {
    type* outcome = primitive_empty;
    type* left = _left;
    type* present = NULL;
    int size = 0, socket = -1;
    char* a_storage = NULL;
    char debug_inf[256] = "\0";
    char wrap_buf[1024 * 20] = {0};

    if (left->em == EMPTY) {
        cross_strcpy(debug_inf, "at least one parameter");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }

    left = c_car(_left);
    if (left->em != STORAGE) {
        cross_strcpy(debug_inf, "first parameter NOT a storage");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }

    a_storage  = left->u_data.a_storage + sizeof(int);
    size = *(int*)left->u_data.a_storage;
    if ((size == 0) || (size == 1)) {
        cross_strcpy(debug_inf, "first parameter length is 0");
        return new_debug(debug_inf, cross_strlen(debug_inf));
    }

    left = c_cdr(_left);
    if (left->em != EMPTY) {
        present = c_car(left);
        if(present->em == NET){
            socket = present->u_data.i_data;
        }
    }
    else {
        socket = 2;
    }

    cross_fprintf(stderr, "eval material is\r\n%s\r\n", a_storage);
    cross_sprintf(wrap_buf, "(progn %s)", a_storage);
    present = evaljit(wrap_buf, socket);
    if (present->em == DEBUG) {
        return present;
    }

    outcome = dispatch_core(present->u_data.s_data);
    gc(present);
    return outcome;
}

typedef struct dispatch_info{
    char* this;
    long priority;
    int socket;
    original_callback address;
    void* para; 
} dispatch_info;

#define TASK_VOLUME 2000
typedef struct task_contain_heap {
    int task_count;
    dispatch_info* task_min_heap[TASK_VOLUME];
} task_contain_heap;

task_contain_heap task_contain;

void bthread_init(task_contain_heap* obj) {
    obj->task_count = 1;
}

int bthread_num(task_contain_heap* obj) {
    return obj->task_count;
}

void bthread_insert(dispatch_info* newnode, task_contain_heap* obj) {
    dispatch_info* present = NULL;
    int p, num;
    obj->task_min_heap[obj->task_count] = newnode;
    num = obj->task_count;
    while (num / 2) {
        p = num / 2;
        if (obj->task_min_heap[num]->priority < obj->task_min_heap[p]->priority) {
            present = obj->task_min_heap[num];
            obj->task_min_heap[num] = obj->task_min_heap[p];
            obj->task_min_heap[p] = present;
        } else {
            break;
        }
        num = p;
    }
    if (obj->task_count > TASK_VOLUME) {
        color_fprintf(stderr, COLOR_YELLOW, "task_object is dangerous %d %d\r\n", obj->task_count, TASK_VOLUME);
    }
    obj->task_count++;
}

#define  task_left(i)  2*(i)
#define  task_right(i)  2*(i)+1

dispatch_info * task_top(task_contain_heap* obj) {
    return obj->task_min_heap[1]; 
}

dispatch_info * bthread_pop(task_contain_heap* obj) {
    dispatch_info* present, * result = obj->task_min_heap[1];
    int exchange;
    int label = 1;
    obj->task_min_heap[label] = obj->task_min_heap[obj->task_count - 1];
    while (task_left(label) <= obj->task_count - 1) {
        if (task_right(label) <= obj->task_count - 1) {
            if (obj->task_min_heap[task_left(label)]->priority < obj->task_min_heap[task_right(label)]->priority) {
                exchange = task_left(label);
            }
            else{
                exchange = task_right(label);
            }
        }
        else {
            exchange = task_left(label);
        }

        if (obj->task_min_heap[exchange]->priority <= obj->task_min_heap[label]->priority) {
            present = obj->task_min_heap[exchange];
            obj->task_min_heap[exchange] = obj->task_min_heap[label];
            obj->task_min_heap[label] = present;
        } 
        else{
            break;
        }
        label = exchange;
    }
    obj->task_min_heap[obj->task_count - 1] = NULL;
    obj->task_count--;
    return result;
}

void execution_install(char* this, void** _env, void* forth_code[], long begin, long end);
void execution_uninstall(char* this, original_callback address, void* para);
void * eval_impl(void* _left, void** _env, void* forth_code[], long* forth_code_ipc, linear_contain* recycle_obj, linear_contain* skip_obj);
void *localeval(void* _left, void** _env);

#define BIG_STEP 1000000000
void * execution(char* this, int _step) {
#define  STREAM_STOP 99999
    void** forth_code;
    void** _env;
    long begin = 0, end = 0, step = 0;

    long * forth_return_array_inner;
    long forth_return_ipc_inner;

    void * * forth_data_array_inner; 
    long forth_data_ipc_inner = 0;
    char * response = NULL;
    char debug_inf[256] = "\0";

    long i = 0, j = 0, eval_flag = 1;
    type * typology = NULL, *present = NULL, *left = NULL;

    long forth_return_ipc_inner_storage = 0;

    void** m_env = NULL;  /* only one use for yield stream , others use _env */

    linear_contain recycle_obj;
    linear_contain skip_obj;

    entry stream_result, stack_result, frame_result, code_result, result, lambda_result;

    stream_result = lookup((char *)this,
                           global_stream);

    stack_result = lookup((char *)this,
                          global_stack);

    frame_result = lookup((char *)this,
                          global_frame);

    code_result = lookup((char *)this,
                         global_code);

    if (stream_result._env){
        begin = (long)stream_result.u_data._expr;
        end = (long)stream_result._env;
    }

    if (code_result._env){
        forth_code = (void**)code_result.u_data._expr;
        _env = (void**)code_result._env;
    }

    if (stack_result._env){
        forth_return_ipc_inner = (long)stack_result.u_data._expr;
        forth_return_array_inner = stack_result._env;
    }

    if (frame_result._env){
        forth_data_ipc_inner = (long)frame_result.u_data._expr;
        forth_data_array_inner = frame_result._env;
    }

    for (i = begin; i < end; i++) {
        this_now = this;
        if (step++ == _step) {
            if (bthread_num(&task_contain) == 1) {
                step = 0;
            }
            else {
                typology = primitive_cont;

                assign(this, (void *)i,
                       (void*)end,
                       &global_stream);

                assign(this, (void *)forth_code,
                       _env,
                       &global_code);

                assign(this, (void *)forth_return_ipc_inner,
                       (void*)forth_return_array_inner,
                       &global_stack);

                assign(this, (void *)forth_data_ipc_inner,
                       (void*)forth_data_array_inner,
                       &global_frame);

                goto yield_out;
            }
        }
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
            forth_data_array_inner[forth_data_ipc_inner++] = c_normal_copy(forth_code[++i]);
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
            if(present == NULL) {
                cross_sprintf(debug_inf, "execution var %s value is None\r\n", typology->u_data.s_data);
                typology = new_debug(debug_inf, cross_strlen(debug_inf));
                /* maybe mem leak , as para before VAR now in stack */
                goto yield_out;
            }
            else if (present->ref_count < 1) {
                cross_sprintf(debug_inf, "object %s already dead\r\n", typology->u_data.s_data);
                typology = new_debug(debug_inf, cross_strlen(debug_inf));
                goto yield_out;
            }
            else{
                forth_data_array_inner[forth_data_ipc_inner++] = c_normal_copy(present);
            } 
            break;
        case STORAGE:
        case NOP:
        case EMPTY:
        case BYTES:
            forth_data_array_inner[forth_data_ipc_inner++] = c_normal_copy(typology);
            break;
        case EVARGSCOMBI:
            forth_data_array_inner[forth_data_ipc_inner - 2] = c_append(
                                                                        forth_data_array_inner[forth_data_ipc_inner - 2],
                                                                        c_cons(forth_data_array_inner[forth_data_ipc_inner - 1], primitive_empty));
            forth_data_ipc_inner--;
            break;
        case EFFUN:
            typology = forth_code[++i];
            present = typology->u_data.f_data(forth_data_array_inner[forth_data_ipc_inner - 1]);
            if ((present->em == DEBUG) && 
                (typology->u_data.f_data != original_eval)) {
                i = 0;
                while (1) {
                    if (!strcmp("", normal_fun[i].name)) {
                        cross_sprintf(debug_inf, "EFFUN find \r\n%s\r\n",
                                      present->u_data.a_storage + sizeof(int));                        
                        break;
                    }
                    if(normal_fun[i].address == typology->u_data.f_data){
                        cross_sprintf(debug_inf, "EFFUN find %s \r\n%s\r\n",
                                      normal_fun[i].name,
                                      present->u_data.a_storage + sizeof(int));                        
                        break;
                    }
                    i++;
                }
                gc(present);
                gc(forth_data_array_inner[forth_data_ipc_inner - 1]);
                typology = new_debug(debug_inf, cross_strlen(debug_inf));
                goto yield_out;
            }
            if (present->em == CONT) {
                i--;
                typology = present;

                assign(this, (void *)i,
                       (void*)end,
                       &global_stream);

                assign(this, (void *)forth_code,
                       _env,
                       &global_code);

                assign(this, (void *)forth_return_ipc_inner,
                       (void*)forth_return_array_inner,
                       &global_stack);

                assign(this, (void *)forth_data_ipc_inner,
                       (void*)forth_data_array_inner,
                       &global_frame);

                goto yield_out;
            }
            gc(forth_data_array_inner[forth_data_ipc_inner - 1]);
            forth_data_array_inner[forth_data_ipc_inner - 1] = present;
            if (forth_return_ipc_inner > (unsigned int)(0.99 * FORTH_WORD * 3)) {
                color_fprintf(stderr, COLOR_YELLOW, "fun call stack status  is  %ld  %ld  \r\n", forth_return_ipc_inner, forth_data_ipc_inner);
            }
            break;
        case EFSTOP:
            typology = forth_data_array_inner[forth_data_ipc_inner - 1];
            assign(this, (void *) STREAM_STOP,
                   (void *) STREAM_STOP,
                   &global_stream);
            c_unbindvars(_env);

            stack_result = lookup((char *) this,
                                  global_stack);
            frame_result = lookup((char *) this,
                                  global_frame);

            if (stack_result._env && frame_result._env){
                forth_return_ipc_inner = (long)stack_result.u_data._expr;
                forth_return_array_inner = stack_result._env;
                cross_free(forth_return_array_inner);
                global_stack = freeentry(this, global_stack);

                forth_data_ipc_inner = (long)frame_result.u_data._expr;
                forth_data_array_inner = frame_result._env;
                cross_free(forth_data_array_inner);
                global_frame = freeentry(this, global_frame);

                global_lambda = freeentry(this, global_lambda);
            }
            else{
                cross_fprintf(stderr, "exit efstop entry error \r\n");
            }
            /*can't delete the reference count, otherwise would core down nexttime.
              gc(typology);
            */
            goto yield_out;
        case EFYIELD:
            typology = forth_data_array_inner[forth_data_ipc_inner - 1];
            left = c_normal_copy(c_caar(typology));
            present = c_cadar(typology);

            if(present->u_data.i_data == 1) {
                assign(this, (void *) STREAM_STOP,
                       (void *) STREAM_STOP,
                       &global_stream);
                c_unbindvars(_env);

                stack_result = lookup((char *)this,
                                      global_stack);

                frame_result = lookup((char *)this,
                                      global_frame);

                lambda_result = lookup((char *)this,
                                      global_lambda);

                if (stack_result._env) {
                    forth_return_array_inner = stack_result._env;
                    cross_free(forth_return_array_inner);
                    global_stack = freeentry(this, global_stack);
                }

                if (frame_result._env) {
                    forth_data_array_inner = frame_result._env;
                    cross_free(forth_data_array_inner);
                    global_frame = freeentry(this, global_frame);
                }

                if (lambda_result._env) {
                    cross_free(lambda_result._env);
                    global_lambda = freeentry(this, global_lambda);
                }
                gc(typology);   /*never come back, so we need delete it. */
            }
            else {
                assign(this, (void *)(i + 1),
                       (void*)1000000,
                       &global_stream);

                assign(this, (void *)forth_code,
                       _env,
                       &global_code);

                assign(this, (void *)forth_return_ipc_inner,
                       (void*)forth_return_array_inner,
                       &global_stack);

                assign(this, (void *)forth_data_ipc_inner,
                       (void*)forth_data_array_inner,
                       &global_frame);
            }

            typology = left;
            goto  yield_out;
        case EFNEXT:
            typology = forth_data_array_inner[forth_data_ipc_inner - 1];
            present = c_car(typology);
            forth_data_array_inner[forth_data_ipc_inner - 1] = 
                execution(present->u_data.s_data, BIG_STEP);
            gc(typology);
            break;
        case EFEOFSTDIN:
            typology = forth_data_array_inner[forth_data_ipc_inner - 1];
            if( !feof(stdin)){
                present = long_type(0);
            }
            else{
                present = long_type(1);
            }
            forth_data_array_inner[forth_data_ipc_inner - 1] = present;

            gc(typology);
            break;
        case EFSTDIN:
            /*some input too long, so use BUF_SIZE instead of 1024*/
            typology = forth_data_array_inner[forth_data_ipc_inner - 1];
            response = (char*)cross_calloc(BUF_SIZE, sizeof(char));
            fgets(response, BUF_SIZE, stdin); 
            present = new_storage(response, cross_strlen(response));
            cross_free(response);
            forth_data_array_inner[forth_data_ipc_inner - 1] = present;
            gc(typology);
            break;
        case EFISSTOP:
            typology = forth_data_array_inner[forth_data_ipc_inner - 1];
            present = c_car(typology);
            stream_result = lookup((char *)(present->u_data.s_data),
                                   global_stream);

            if (!stream_result._env){
                forth_data_array_inner[forth_data_ipc_inner - 1] = long_type(0);
            }
            else{
                j = (long)stream_result._env;
                if(j == STREAM_STOP){
                    forth_data_array_inner[forth_data_ipc_inner - 1] = long_type(1);
                    global_stream = freeentry((char *)(present->u_data.s_data), global_stream);
                }
                else{
                    forth_data_array_inner[forth_data_ipc_inner - 1] = long_type(0);
                }
            }
            gc(typology);
            break;
        case EFSAPPLYX:
            forth_return_array_inner[forth_return_ipc_inner++] = i + 2;
            i = ((type*)forth_code[i + 1])->u_data.i_data;
            forth_return_array_inner[forth_return_ipc_inner++] = i;
            present = forth_code[++i];
            c_bindvars(present, forth_data_array_inner[forth_data_ipc_inner - 1], _env);
            gc(forth_data_array_inner[--forth_data_ipc_inner]);
            break;
        case EFSAPPLYXBEGIN:
            present = primitive_empty;
            while(1){
                i++;
                typology = forth_code[i];
                if(typology->em == EFSAPPLYX)break;
                present = c_append(present, c_cons(typology, primitive_empty));
            }
            forth_data_array_inner[forth_data_ipc_inner++] = present;
            i--;
            break;
        case EFSAPPLY:
            /*eliminate  tail-recurrsion*/
            typology = forth_code[i + 2];
            if ((typology != NULL) && (typology->em == EFDEFUNEND || typology->em == EFDEFUNTAIL)) {
                while (forth_return_ipc_inner >= 1) {
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
            c_bindvars(present, forth_data_array_inner[forth_data_ipc_inner - 1], _env);
            gc(forth_data_array_inner[--forth_data_ipc_inner]);
            break;
        case EFFUNCALL:
            break;
        case EFSELF:
            typology = forth_data_array_inner[--forth_data_ipc_inner];
            result = lookup((char *)this,
                            global_lambda);
            if (!result.u_data._expr) {
                color_fprintf(stderr, COLOR_YELLOW, "efself impossible efself \r\n");
            }
            else{
                i = (long)result.u_data._expr;
            }
            present = forth_code[i];
            c_unbindvars(_env);
            c_bindvars(present, typology, _env);
            gc(typology);
            break;
        case EFIF:
            typology = forth_data_array_inner[--forth_data_ipc_inner];
            if (typology->u_data.i_data == 0) {
                i += ((type*)forth_code[i + 1])->u_data.i_data + 1;
                i--;
            } 
            else {
                i++;
            }
            gc(typology);
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
            forth_data_array_inner[forth_data_ipc_inner++] = c_normal_copy( forth_code[i+1]);
            while(1){
                i++;
                typology = forth_code[i];
                if(typology->em == EFDEFUNEND)break;
            }
            break;
        case EFDEFMACRO:
            forth_data_array_inner[forth_data_ipc_inner++] = c_normal_copy( forth_code[i+1]);
            while(1){
                i++;
                typology = forth_code[i];
                if(typology->em == EFDEFUNEND)break;
            }
            break;
        case EFDEFUNTAIL:
        case EFDEFUNEND:
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
                        linear_init(&recycle_obj);
                        linear_init(&skip_obj);
                        eval_impl(present, _env, global_forth_code, &global_forth_code_ipc, &recycle_obj, &skip_obj);
                    }
                    else{
                        global_forth_code[global_forth_code_ipc++] = c_normal_copy(typology);
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
                    global_forth_code[global_forth_code_ipc++] = c_normal_copy(typology);
                }
                i++;
            }
            i = forth_return_array_inner[--forth_return_ipc_inner];
            execution_instance(_env, global_forth_code, i, global_forth_code_ipc);
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
        case EFJOIN:
            typology = forth_code[++i];
            result = lookup((char *) (typology->u_data.s_data), global_var);
            if(result.u_data._expr == NULL) {
                forth_data_array_inner[forth_data_ipc_inner++] = primitive_empty;
            }
            else {
                assign((char *)(typology->u_data.s_data),
                       c_append(result.u_data._expr,
                                c_cons(forth_data_array_inner[--forth_data_ipc_inner], primitive_empty)), NULL,
                       &global_var);
                forth_data_array_inner[forth_data_ipc_inner++] = primitive_empty;
            }
            break;
        case EFWHOLE:
            typology = forth_code[++i];
            result = lookup((char *) (typology->u_data.s_data), global_var);
            if (result.u_data._expr == NULL) {
                forth_data_array_inner[forth_data_ipc_inner++] = primitive_empty;
            }
            else{
                forth_data_array_inner[forth_data_ipc_inner++] = c_normal_copy(result.u_data._expr);
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
                          c_append(c_cadr(result.u_data._expr),
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
                forth_data_array_inner[forth_data_ipc_inner++] = c_normal_copy(
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
                cross_fprintf(stderr, "forth_data_ipc_inner  is  %ld  \r\n", forth_data_ipc_inner);
            } 
            else {
                if (present->em != VAR) {
                    cross_strcpy(debug_inf, "FUNCALL name is INVALID, shoule be VAR\r\n");
                    typology = new_debug(debug_inf, cross_strlen(debug_inf));
                    gc(present);
                    goto yield_out;
                }
                result = lookup((char *) (present->u_data.s_data),
                                global_lambda);
                if (result.u_data._expr == NULL) {
                    if (iterate_address(present, forth_code, end) == NULL) {
                        cross_sprintf(debug_inf, "FUNCALL name %s doesn't exist\r\n", present->u_data.s_data);
                        typology = new_debug(debug_inf, cross_strlen(debug_inf));
                        gc(present);
                        goto yield_out;
                    } 
                    else {
                        result.u_data._expr =
                            (void*) ((iterate_address(present, forth_code, end) + 1));
                    }
                } 
                else {
                    forth_return_array_inner[forth_return_ipc_inner++] = (long)*_env;
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
                c_bindvars(present, forth_data_array_inner[forth_data_ipc_inner - 1], _env);
                gc(forth_data_array_inner[--forth_data_ipc_inner]);
            }
            break;
        case EFLAMBDAEND: 
            /*modify  by rosslyn  for  dynamic calc,  20140810*/
            if (forth_return_ipc_inner == forth_return_ipc_inner_storage) {
                i = end;
            } 
            else{
                if (((type*)(*_env))->em == EMPTY) {
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
            if (((type*)(*_env))->em == EMPTY) {
                assign(present->u_data.s_data, (void *) (i + 2),
                       c_cons(primitive_empty, primitive_empty),
                       &global_lambda);
            } 
            else {
                /*
                  assign(present->u_data.s_data, (void *) (i + 2),
                  c_cons(c_normal_copy(c_car(*_env)), c_normal_copy(c_cdr(*_env))),
                  &global_lambda);
                */
                if (((type*)(c_cdr(*_env)))->em == EMPTY) {
                    assign(present->u_data.s_data, (void *) (i + 2),
                           c_list(c_normal_copy(c_car(*_env)), 0),
                           &global_lambda);
                }
                else {
                    assign(present->u_data.s_data, (void *) (i + 2),
                           c_list(c_normal_copy(c_car(*_env)), c_normal_copy(c_cadr(*_env)), 0),
                           &global_lambda);
                }
            }

            forth_data_array_inner[forth_data_ipc_inner++] = present;
            i = ((type*)forth_code[i + 1])->u_data.i_data;
            break;
        case EFPGET:
            forth_data_array_inner[forth_data_ipc_inner++] = var_type(this);
            break;
        case EFSLEEP:
            typology = forth_data_array_inner[forth_data_ipc_inner - 1];
            present = c_car(typology);
            if (present->em != BYTES) {
                cross_sprintf(debug_inf, "sleep value shoule be a integer\r\n");
                typology = new_debug(debug_inf, cross_strlen(debug_inf));
                gc(typology);
                goto yield_out;
            }
            else if ((bthread_num(&task_contain) == 1)) {
                sleep(present->u_data.i_data);
                forth_data_array_inner[forth_data_ipc_inner - 1] = c_normal_copy(present);
                gc(typology);
            }
            else {
                forth_data_array_inner[forth_data_ipc_inner - 1] = c_normal_copy(present);
                gc(typology);

                typology = primitive_cont;

                assign(this, (void *)i + 1,
                       (void*)end,
                       &global_stream);

                assign(this, (void *)forth_code,
                       _env,
                       &global_code);

                assign(this, (void *)forth_return_ipc_inner,
                       (void*)forth_return_array_inner,
                       &global_stack);

                assign(this, (void *)forth_data_ipc_inner,
                       (void*)forth_data_array_inner,
                       &global_frame);

                goto yield_out;
            }
            break;
        case EFITER:
            present = random_name();
            m_env = (void**) cross_calloc(sizeof(void*), 1);
            if (((type*) (*_env))->em == EMPTY) {
                *m_env = c_cons(primitive_empty, primitive_empty);
            } 
            else {
                *m_env = c_cons(c_normal_copy(c_car(*_env)), primitive_empty);
            }

            assign(present->u_data.s_data, (void *) (i + 2),
                   m_env, &global_lambda);

            execution_install(present->u_data.s_data, m_env, forth_code, i + 3, 100000);
            forth_data_array_inner[forth_data_ipc_inner++] = present;
            i = ((type*)forth_code[i + 1])->u_data.i_data;
            break;
        default:
            color_fprintf(stderr, COLOR_RED, "unhandled typology %d \r\n", typology->em);
            break;
        }
    }

    if (forth_data_ipc_inner != 1) {
        color_fprintf(stderr, COLOR_RED, "$$$$$$$$$$$$$$$   %ld   $$$$$$$$$$$$$$$$$$$$$$$$$\r\n", forth_data_ipc_inner);
        typology = primitive_empty;
    }
    else {
        typology = forth_data_array_inner[forth_data_ipc_inner - 1];
    }
 yield_out:
    return typology;
}

void dispatch_init(void) {
    bthread_init(&task_contain);
}

void* check_status(char* this) {
    type * typology = NULL;
    int status = 0;

    entry status_result;
    status_result = lookup((char *)this,
                           global_status);
    if (status_result.u_data._expr){
        typology = (type*)status_result.u_data._expr;
        status = (long)status_result._env;
        if (status == 1000) {
            return typology;
        }
        else {
            return NULL;
        }
    }
    else {
        return NULL;
    }
}

int global_in_un_install = 0;

void at_ok_pthread() {
    type * typology = NULL;
    callback_time* te = NULL;
    color_fprintf(stderr, COLOR_RED, "pthread num:%d\r\n", at_num(&global_timer_loop));
    color_fprintf(stderr, COLOR_RED, "global_in_un_install:%d\r\n", global_in_un_install);
   
    while (at_num(&global_timer_loop) > 1){
        te = at_top(&global_timer_loop);
        typology = check_status(te->lambda);
        if (typology == NULL) {
            te = at_pop(&global_timer_loop);
            cross_free(te);
        }
        else {
            break;
        }              
    }
}

void* dispatch_core(char* that) {
    type * typology = NULL;
    dispatch_info* task, * task_new;
    callback_time* te = NULL;
    char* this = NULL;
    void* para;
    int old_err, old_out, socket;
    int status = 0;
    int step = 0;
    long priority = 0;
    int thread_num = 1;

    while(1) {
        typology = check_status(that);
        if (typology) {
            freeentry(that, global_status);            
            return typology;
        }

        thread_num = bthread_num(&task_contain);
        if (thread_num == 1) {
            break;
        }
    
        task_new = bthread_pop(&task_contain);
        this = task_new->this;
        para = task_new->para;
        socket = task_new->socket;
        priority = task_new->priority;
        old_out = -1;
        old_err = -1;
    
        if (socket != 2) {
            old_out = dup(1);
            old_err = dup(2);
            dup2(socket, 2);
            dup2(socket, 1);
        }

        step = 500 / (thread_num + 1);
        if (step < 10) {
            step = 10;
        }

        typology = execution(this, step);
        if (old_err != -1) {
            dup2(old_out, 1);
            dup2(old_err, 2);
        }
        if (typology->em == DEBUG) {
            execution_uninstall(this, task_new->address, para);
            cross_free(task_new->this);
            cross_free(task_new);
            return typology;
        }

        if (typology->em == CONT) {
            task = (dispatch_info*)cross_malloc(sizeof(*task));
            task->priority = priority;
            task->this = (char*)cross_malloc(strlen(this) + 1);
            cross_memcpy(task->this, this, strlen(this));
            task->socket = socket;
            task->address = task_new->address;
            task->para = para;
            bthread_insert(task, &task_contain);
        }
        else {
            execution_uninstall(this, task_new->address, para);

            if (!strcmp(this, that)) {
                cross_free(task_new->this);
                cross_free(task_new);
                return typology;
            }
            /*means no need to put on the global_timer_loop */
            else if (typology->em == NOP) {
                at_ok_pthread();                
            }
            else {
                global_in_un_install = global_in_un_install + 1;
    
                at_ok_pthread();
                assign(this, (void *)typology,
                       (void*)1000,
                       &global_status);
        
                te = (callback_time*)cross_malloc(sizeof(*te));
                //                atGetTime(&te->when_sec);
                te->when_sec = global_in_un_install;
                te->lambda = this;
                status = at_insert(te, &global_timer_loop);
            }
        }
        cross_free(task_new->this);
        cross_free(task_new);
    }
    color_fprintf(stderr, COLOR_RED, "dispatch prb %s %d\r\n", that, priority);
    return primitive_cont;
}

void execution_install(char* this, void** _env, void* forth_code[], long begin, long end) {
    long * forth_return_array_inner;
    long forth_return_ipc_inner;

    void * * forth_data_array_inner; 
    long forth_data_ipc_inner = 0;

    forth_return_array_inner = (long*)cross_calloc(sizeof(long), 2000);    
    forth_return_ipc_inner = 0;

    forth_data_array_inner = (void **)cross_calloc(sizeof(void*), 2000);    
    forth_data_ipc_inner = 0;

    assign(this, (void *)begin,
           (void*)end,
           &global_stream);

    assign(this, (void *)forth_code,
           _env,
           &global_code);

    assign(this, (void *)forth_return_ipc_inner,
           (void*)forth_return_array_inner,
           &global_stack);

    assign(this, (void *)forth_data_ipc_inner,
           (void*)forth_data_array_inner,
           &global_frame);

    return;
}

void execution_uninstall(char* this, original_callback address, void* para) {
    entry stack_result, frame_result;

    if (address != NULL) {
        address(para);
    }
    else {
        color_fprintf(stderr, COLOR_RED, "unexpect happens\r\n");
    }

    stack_result = lookup((char *)this,
                          global_stack);

    frame_result = lookup((char *)this,
                          global_frame);

    if (stack_result._env){
        cross_free(stack_result._env);
    }

    if (frame_result._env){
        cross_free(frame_result._env);
    }

    freeentry(this, global_stack);
    freeentry(this, global_frame);

    freeentry(this, global_stream);
    freeentry(this, global_code);

    return;
}

void prepare_dispatch(char* this, 
                      void** _env, 
                      void* forth_code[], 
                      long begin, 
                      long end, 
                      int socket, 
                      original_callback address, 
                      void* para, 
                      long priority) {
    dispatch_info* task = NULL;
    execution_install(this, _env, forth_code, begin, end);

    //    task = (dispatch_info*)cross_malloc(sizeof(*task));
    task = (dispatch_info*)cross_malloc(BUF_SIZE);    
    //    task->this = (char*)cross_malloc(strlen(this) + 1);
    task->this = (char*)cross_malloc(BUF_SIZE);    
    cross_memcpy(task->this, this, strlen(this));
    task->socket = socket;
    task->address = address;
    task->para = para;
    task->priority = priority;
    bthread_insert(task, &task_contain);
    return;
}

void* execution_instance(void** _env, void* forth_code[], long begin, long end) {
    type* present = NULL;
    char this[256] = "\0";
    present = random_name();
    cross_sprintf(this, "%s%s", present->u_data.s_data, ":instance");
    gc(present);
    execution_install(this, _env, forth_code, begin, end);

    /* prepare_dispatch(this, _env, forth_code, begin, end, 0, NULL, NULL, prority);*/
    while (1) {
        present = execution(this, BIG_STEP);
        if (present->em != CONT) {
            break;
        }
    }

    execution_uninstall(this, 0, 0);
    return present;
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

void write_forth_code_helper(unsigned char* forth_code_serial, type* right, 
                             long* _forth_code_serial_ipc,
                             unsigned char* buffer_trans) {
    long forth_code_serial_ipc = *_forth_code_serial_ipc;
    type realvalue;
    int len = 0;
    type *left = NULL;
    unsigned char * special = NULL;
    unsigned long value = 0;

    if(!right) {
        color_fprintf(stderr, COLOR_RED, "exit global_forth_code  null occurs, offset is %d, please check \r\n");
        forth_code_serial[forth_code_serial_ipc++] = (unsigned  char )'$';
        /*it can cover the error problem*/
    }
    else if(right->em == LIST) {
        forth_code_serial[forth_code_serial_ipc++] = (unsigned char)EFLISTBEGIN;
        while(1) {
            if(right->em == EMPTY) {
                break;
            }
            left = c_car(right);
            write_forth_code_helper(forth_code_serial, left, &forth_code_serial_ipc, buffer_trans);
            right = c_cdr(right);
        }
        forth_code_serial[forth_code_serial_ipc++] = (unsigned char)EFLISTEND;
    }
    else if (right->em == FUN) {
        realvalue = *(type*)c_normal_copy(right);
        modify_fun_content(&realvalue);
        forth_code_serial[forth_code_serial_ipc++] = (unsigned  char )FUN;
        len = cross_strlen(realvalue.u_data.s_data);
        forth_code_serial[forth_code_serial_ipc++] = (unsigned  char )len;
        network_encryption((unsigned char *)realvalue.u_data.s_data, len, (unsigned char* )buffer_trans);
        cross_memcpy(forth_code_serial + forth_code_serial_ipc, buffer_trans, len);
        forth_code_serial_ipc += len;
    }
    else if (right->em == VAR) {
        forth_code_serial[forth_code_serial_ipc++] = (unsigned  char)VAR;
        len = cross_strlen(right->u_data.s_data);
        forth_code_serial[forth_code_serial_ipc++] = (unsigned  char)len;
        network_encryption((unsigned char *)right->u_data.s_data, len, (unsigned char*)buffer_trans);
        cross_memcpy(forth_code_serial + forth_code_serial_ipc, buffer_trans, len);
        forth_code_serial_ipc += len;
    }
    else if (right->em == STORAGE) {
        forth_code_serial[forth_code_serial_ipc++] = (unsigned  char)STORAGE;
        len = *(int*)(right->u_data.a_storage) - 1;

        *(int*)(forth_code_serial + forth_code_serial_ipc) = len;
        forth_code_serial_ipc += sizeof(int);
        special = (unsigned char*)cross_calloc(BUF_SIZE, sizeof(unsigned char));
        network_encryption((unsigned char *)(right->u_data.a_storage) + sizeof(int), len, (unsigned char *)special);
        cross_memcpy(forth_code_serial + forth_code_serial_ipc, special, len);
        cross_free(special);
        forth_code_serial_ipc += len;
    }
    else if (right->em == BYTES) {
        forth_code_serial[forth_code_serial_ipc++] = (unsigned char)BYTES;
        value = right->u_data.i_data;
        if (value < 256) {
            forth_code_serial[forth_code_serial_ipc++] = 1;
            forth_code_serial[forth_code_serial_ipc++] = value;
        }
        else if (value < 256 * 256) {
            forth_code_serial[forth_code_serial_ipc++] = 2;
            forth_code_serial[forth_code_serial_ipc++] = value / 256;
            forth_code_serial[forth_code_serial_ipc++] = value % 256;
        }
        else if (value < 256 * 256 * 256) {
            forth_code_serial[forth_code_serial_ipc++] = 3;
            forth_code_serial[forth_code_serial_ipc++] = value / (256 * 256);
            forth_code_serial[forth_code_serial_ipc++] = (value / 256 ) % 256;
            forth_code_serial[forth_code_serial_ipc++] = value % 256;
        }
        else {
            forth_code_serial[forth_code_serial_ipc++] = 4;
            forth_code_serial[forth_code_serial_ipc++] = value / (256 * 256 * 256);
            forth_code_serial[forth_code_serial_ipc++] = (value / (256 * 256)) % 256;
            forth_code_serial[forth_code_serial_ipc++] = (value / 256 ) % 256;
            forth_code_serial[forth_code_serial_ipc++] = value % 256;
        }
    }
    else{
        forth_code_serial[forth_code_serial_ipc++] = (unsigned char)(right->em);
    }

    *_forth_code_serial_ipc = forth_code_serial_ipc;
}

void write_forth_code(char * file_tmp_name) {
    unsigned char * forth_code_serial = (unsigned char *) cross_calloc(FORTH_WORD , 10);
    long forth_code_serial_ipc = 0;
    int i = 0;
    type realvalue , *right = NULL;
    int len = 0;
    unsigned char * buffer_trans = (unsigned char*) cross_calloc(256, sizeof(unsigned char));
    unsigned char * special = NULL;

    for (i = 0; i < global_forth_code_ipc; i++) {
        right = global_forth_code[i];
        write_forth_code_helper(forth_code_serial, right, &forth_code_serial_ipc, buffer_trans);
    }
    /*add  current ipc, genuis idea,2014.6.7*/
    cross_fwrite(file_tmp_name, (char*)forth_code_serial, sizeof(char), forth_code_serial_ipc);
    cross_free(buffer_trans);
    cross_free(forth_code_serial);
}

void *  read_forth_code_helper(unsigned char *forth_code_serial, int *_i, char * buffer_trans) {
    int i = *_i, len = 0;
    unsigned char realvalue;
    type *right = NULL;
    type *left = NULL;
    char * responsex = NULL;
    size_t storage_size = 0;
    unsigned long value = 0;

    left = primitive_empty;
    realvalue = forth_code_serial[i++];
    if(realvalue == (unsigned char)EFLISTBEGIN) {
        while (1) {
            realvalue = forth_code_serial[i];
            if(realvalue == (unsigned char)EFLISTEND) {
                i++; 
                break;
            }
            right = read_forth_code_helper(forth_code_serial, &i, buffer_trans);
            left = c_append(left, c_cons(right, primitive_empty));
        }
        right = left;
    }
    else {
        if(realvalue == (unsigned char)FUN) {
            right = new_object();
            right->em = FUN;
            len = forth_code_serial[i++];
            network_decryption((unsigned char *)forth_code_serial + i, len, (unsigned char *)buffer_trans);
            cross_memcpy(right->u_data.s_data, buffer_trans, len);
            i += len;
            modify_fun_content_x(right);
        }
        else if(realvalue == (unsigned char)BYTES) {
            right = new_object();
            right->em = BYTES;
            len = forth_code_serial[i++];
            value = 0;
            while (len--) {
                value = 256 * value + forth_code_serial[i++];
            }
            right->u_data.i_data = value;
        }
        else if(realvalue == (unsigned char)VAR) {
            right = new_object();
            right->em = VAR;
            len = forth_code_serial[i++];
            network_decryption((unsigned char *)forth_code_serial + i, len, (unsigned char* )buffer_trans);
            cross_memcpy(right->u_data.s_data, buffer_trans, len);
            i += len;
        }
        else if(realvalue == (unsigned char)STORAGE) {
            right = new_object();
            right->em = STORAGE;
            len = *(int*)(forth_code_serial + i);
            i += sizeof(int);
            storage_size = len + 1 + sizeof(int);
            responsex = (char*) cross_calloc(storage_size, sizeof(char));
            network_decryption((unsigned char *) forth_code_serial + i, len, (unsigned char* )responsex + sizeof(int));
            responsex[(int)storage_size - 1] = '\0';
            *(int*)responsex = storage_size - sizeof(int);
            right->u_data.a_storage = responsex;

            i += len;
        }
        else{
            right = new_object();
            right->em = (native_operator)realvalue;
        }
    }

    *_i = i;
    return right;
}

void read_forth_code(char * file_tmp_name) {
    unsigned char * forth_code_serial = (unsigned char *)cross_calloc(FORTH_WORD, 10);
    long forth_code_serial_ipc = 0;
    int i = 0;
    int  sign = 0;
    type* right = NULL;
    char * buffer_trans = (char*)cross_calloc(256, sizeof(unsigned char));
    size_t storage_size = 0;

    cross_read(file_tmp_name, &sign, (char *)forth_code_serial);
    if (sign == -1) {
        return;
    }
    forth_code_serial_ipc = sign / sizeof(char);
    global_forth_code_ipc = 0;
    i = 0;
  
    while (i < forth_code_serial_ipc) {
        right = read_forth_code_helper(forth_code_serial, &i, buffer_trans);
        global_forth_code[global_forth_code_ipc++] = right;
    }
    cross_free(buffer_trans);
    cross_free(forth_code_serial);
    return;
}

void * execution_strategy(void** _env, void *forth_code[], long begin, long end){
    char file_tmp_name[256] = "";

    if(cross_strlen(global_filename) == 0) {
        /*for  debug reason, normal change 0 to 1*/
        return execution_instance(_env, forth_code, begin, end);
    }
    else{
        cross_sprintf(file_tmp_name, "%sf", global_filename);
        write_forth_code(file_tmp_name);
        read_forth_code(file_tmp_name);
        return execution_instance(_env, forth_code, begin, end);
    }
}

#ifdef FORTH
void * execution_forth(void *forth_code[] , long begin , long end) {
    long forth_return_array[FORTH_WORD];
    int forth_return_ipc = 0;
    void * forth_data_array[FORTH_WORD];
    int forth_data_ipc = 0;
    long i = 0;
    type * typology = NULL, *present = NULL;

    for (i = begin; i < end; i++) {
        typology = forth_code[i];
        switch (typology->em) {
        case EMPTY:
        case VAR:
            /*special  for  display information,related to the ZFQUOTE*/
        case BYTES:
            forth_data_array[forth_data_ipc++] = c_normal_copy(typology);
            break;
        case EFFUN:
            typology = forth_code[++i];
            forth_data_array[forth_data_ipc - 1] = typology->u_data.f_data(
                                                                           forth_data_array[forth_data_ipc - 1]);
            break;
        case ZFEND:
            if (forth_return_ipc == 0) {
                i = end;
            } 
            else {
                i = forth_return_array[--forth_return_ipc];
                i--;
            }
            break;
        case ZFCOLON:
            left_fprint(stderr, forth_code[++i]);
            i = end;
            break;
        case ZFSAPPLY:
            forth_return_array[forth_return_ipc++] = i + 2;
            i = ((type*)forth_code[i + 1])->u_data.i_data;
            break;
        case ZFDUP:
            forth_data_array[forth_data_ipc + 1] = c_normal_copy(
                                                               forth_data_array[forth_data_ipc - 1]);
            forth_data_ipc++;
            break;
        case ZFEQ:
            typology = forth_data_array[forth_data_ipc - 1];
            present = forth_data_array[forth_data_ipc - 2];
            forth_data_array[forth_data_ipc - 2] =
                c_eq(typology, present) ?
                long_type(1) : long_type(0);
            forth_data_ipc--;
            gc(typology);
            gc(present);
            break;
        case ZFADD:
            forth_data_array[forth_data_ipc - 2] = original_add(
                                                                c_cons(forth_data_array[forth_data_ipc - 1],
                                                                       c_cons(forth_data_array[forth_data_ipc - 2],
                                                                              primitive_empty)));
            forth_data_ipc--;
            break;
        case ZFRANDOM:
            forth_data_array[forth_data_ipc - 1] = original_random(
                                                                   c_cons(forth_data_array[forth_data_ipc - 1],
                                                                          primitive_empty));
            break;
        case ZFDROP:
            gc(forth_data_array[--forth_data_ipc]);
            break;
        case ZFOVER:
            forth_data_array[forth_data_ipc + 1] = c_normal_copy(
                                                               forth_data_array[forth_data_ipc - 2]);
            forth_data_ipc++;
            break;
        case ZFBIG:
            forth_data_array[forth_data_ipc - 2] = original_big(
                                                                c_cons(forth_data_array[forth_data_ipc - 2],
                                                                       c_cons(forth_data_array[forth_data_ipc - 1],
                                                                              primitive_empty)));
            forth_data_ipc--;
            break;
        case ZFMOD:
            forth_data_array[forth_data_ipc - 2] = original_mod(
                                                                c_cons(forth_data_array[forth_data_ipc - 2],
                                                                       c_cons(forth_data_array[forth_data_ipc - 1],
                                                                              primitive_empty)));
            forth_data_ipc--;
            break;
        case ZFSWAP:
            present = forth_data_array[forth_data_ipc - 2];
            forth_data_array[forth_data_ipc - 2] =
                forth_data_array[forth_data_ipc - 1];
            forth_data_array[forth_data_ipc - 1] = present;
            break;
        case ZFMINUS:
            forth_data_array[forth_data_ipc - 2] = original_minus(
                                                                  c_cons(forth_data_array[forth_data_ipc - 2],
                                                                         c_cons(forth_data_array[forth_data_ipc - 1],
                                                                                primitive_empty)));
            forth_data_ipc--;
            break;
        case ZFDOT:
            cross_fprintf(stderr, "\n");
        case ZFDOTCHAR:
            gc(left_fprint(stderr, forth_data_array[--forth_data_ipc]));
            break;
        case ZFIF:
            typology = forth_data_array[--forth_data_ipc];
            if (typology->u_data.i_data == 0) {
                i = ((type*)forth_code[i + 1])->u_data.i_data;
                i--;
            } 
            else {
                i++;
            }
            gc(typology);
            break;
        case ZFELSEIF:
            i = ((type*)forth_code[i + 1])->u_data.i_data;
            i--;
            break;
        default:
            color_fprintf(stderr, COLOR_RED, "unhandled situation \r\n");
            break;
        }
    }
    return primitive_empty;
}

void eval_forth(void * _left, void ** _env, void *forth_code[], long *_forth_code_ipc) {
    type * present;
    type * left = _left;
    long forth_code_ipc = *_forth_code_ipc;
 label: switch (left->em) {
 case EMPTY:
     forth_code[forth_code_ipc++] = primitive_zfend;
     *_forth_code_ipc = forth_code_ipc;
     break;
 default:
     /*    assert(left->em == LIST);*/
     present = c_car (left );
     switch (present->em) {
     case ZFEQ:
     case ZFDUP:
     case ZFDOTCHAR:
     case ZFDOT:
     case ZFADD:
     case ZFRANDOM:
     case ZFDROP:
     case ZFOVER:
     case ZFBIG:
     case ZFMOD:
     case ZFSWAP:
     case ZFMINUS:
     case BYTES:
         forth_code[forth_code_ipc++] = present;
         left = c_cdr(left);
         goto label;
     case ZFIF:
         forth_code[forth_code_ipc++] = primitive_zfif;
         batch_write_circuit((void**) (forth_code_ipc),
                             binary_return_array + (binary_return_label++));
         forth_code_ipc++;
         left = c_cdr(left);
         goto label;
         break;
     case ZFELSEIF:
         fix_code_address(
                          batch_read_circuit(
                                             binary_return_array + --binary_return_label),
                          forth_code_ipc + 2, forth_code);

         forth_code[forth_code_ipc++] = primitive_zfelseif;
         batch_write_circuit((void**) (forth_code_ipc),
                             binary_return_array + (binary_return_label++));
         forth_code_ipc++;
         left = c_cdr(left);
         goto label;
         break;
     case ZFTHEN:
         fix_code_address(
                          batch_read_circuit(
                                             binary_return_array + --binary_return_label),
                          forth_code_ipc, forth_code);
         left = c_cdr(left);
         goto label;
         break;
     case VAR:
         forth_code[forth_code_ipc++] = primitive_zfsapply;
         if (fetch_code_skip_address(present, EFDEFUN) == NULL) {
             forth_code_unfix[forth_code_unfix_ipc++] = present;
             forth_code_unfix[forth_code_unfix_ipc++] =
                 long_type(forth_code_ipc++);
         } 
         else {
             forth_code[forth_code_ipc++] = fetch_code_skip_address(present, EFDEFUN);
         }
         left = c_cdr(left);
         goto label;
         break;
     case ZFCOLON:
         present = c_cdr(left);

         forth_code_skip[forth_code_skip_ipc++] = primitive_efdefun;
         forth_code_skip[forth_code_skip_ipc++] = c_car(present);
         forth_code_skip[forth_code_skip_ipc++] =
             long_type (forth_code_ipc + 1);
         fix_unfix_code(c_car(present), forth_code_ipc + 1, forth_code);

         forth_code[forth_code_ipc++] = primitive_zfcolon;
         forth_code[forth_code_ipc++] = c_copy_atom(c_car(present));
         left = c_cdr(present);
         goto label;
         break;
     case ZFFORTH:
         left = c_cdr(left);
         goto label;
         break;
     case ZFQUOTE:
         forth_code[forth_code_ipc++] = c_cadr(left);
         left = c_cddr(left);
         goto label;
         break;
     default:
         color_fprintf(stderr, COLOR_RED, "unhandled situation \r\n");
         break;
     }
 }
}
#endif

void * eval_impl(void * _left, void ** _env, void * forth_code[], long *_forth_code_ipc, linear_contain* recycle_obj, linear_contain* skip_obj) {
    type * present, *outcome = NULL;
    type * left = _left;
    int skip = 0;
    unsigned long forth_code_ipc = *_forth_code_ipc;
    char debug_inf[256] = "\0";

 label: 
    switch (left->em) {
    case NOP:
    case EMPTY:
        forth_code[forth_code_ipc++] = left;
        goto popjreturn;
        break;
    case STORAGE:
    case VAR:
        forth_code[forth_code_ipc++] = left;
        goto popjreturn;
        break;
    case BYTES:
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
            batch_write_circuit(c_cadr(left),
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
        case ITER:
            present = c_cdr(left);
            batch_write_circuit(long_type (forth_code_ipc + 1),
                                binary_return_array + (binary_return_label++));
            batch_write_circuit(primitive_eflambda,
                                binary_return_array + (binary_return_label++));

            forth_code[forth_code_ipc++] = primitive_efiter;
            forth_code_ipc++;
            /*for  skip*/
            forth_code[forth_code_ipc++] = c_car(present);
            left = c_cadr(present);
            goto label;
            break;
        case LAMBDA:
            present = c_cdr(left);
            batch_write_circuit(long_type (forth_code_ipc + 1),
                                binary_return_array + (binary_return_label++));
            batch_write_circuit(primitive_eflambda,
                                binary_return_array + (binary_return_label++));

            forth_code[forth_code_ipc++] = primitive_eflambda;
            forth_code_ipc++;
            /*for  skip*/
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
        case JOIN:
            /* follow code doesn't used  eval (left, ...) style , as the statement would copy  the used default, but we can used the essential code*/
            outcome = c_caddr(left);
            left = c_cadr(left);

            batch_write_circuit(left,
                                binary_return_array + (binary_return_label++));
            batch_write_circuit(primitive_evjoin,
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
        case BYTES:
            break;
        case LIST: /*for macro forth*/
            forth_code[forth_code_ipc++] = left;
            goto popjreturn;
            break;
        case PGET:
            forth_code[forth_code_ipc++] = primitive_efpget;
            goto popjreturn;
            break;
        case SLEEP:
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
            present = c_cdr(left);
            batch_write_circuit(primitive_efdefun,
                                binary_return_array + (binary_return_label++));

            forth_code[forth_code_ipc++] = primitive_efdefun;

            linear_push(skip_obj, primitive_efdefun);
            linear_push(skip_obj, c_car(present));
            linear_push(skip_obj, linear_push(recycle_obj, long_type( forth_code_ipc))); 
            fix_unfix_code(c_car(present), forth_code_ipc, forth_code, recycle_obj);

            /*  forth_code[forth_code_ipc++] = c_copy_atom(c_car(present));*/
            forth_code[forth_code_ipc++] = c_car(present);
            forth_code[forth_code_ipc++] = c_cadr(present);
            left = c_caddr(present);
            goto label;
            break;
        case VAR:
            if (1) {
                if (fetch_code_skip_address(present, EFDEFUN, skip_obj)) {
                    batch_write_circuit(c_car(left),
                                        binary_return_array + (binary_return_label++));
                    batch_write_circuit(c_cdr(left),
                                        binary_return_array + (binary_return_label++));
                    forth_code[forth_code_ipc++] = primitive_empty;
                    goto evargs;
                }
                else if (fetch_code_skip_address(present, EFDEFMACRO, skip_obj)) {
                    forth_code[forth_code_ipc++] = primitive_efsapplyxbegin;
                    outcome = c_cdr(left);
                    while(outcome->em != EMPTY){
                        forth_code[forth_code_ipc++] = c_car(outcome);
                        outcome = c_cdr(outcome);
                    }
                    forth_code[forth_code_ipc++] = primitive_efsapplyx;
                    if (fetch_code_skip_address(present, EFDEFMACRO, skip_obj) == NULL) {
                        forth_code_unfix[forth_code_unfix_ipc++] = c_car(left);
                        forth_code_unfix[forth_code_unfix_ipc++] = long_type(forth_code_ipc++);
                    } 
                    else {
                        forth_code[forth_code_ipc++] = fetch_code_skip_address(present, EFDEFMACRO, skip_obj);
                    }
                    goto popjreturn;
                } 
                else { 
                    /*un resolved symbol,fixit later*/
                    batch_write_circuit(c_car(left),
                                        binary_return_array + (binary_return_label++));
                    batch_write_circuit(c_cdr(left),
                                        binary_return_array + (binary_return_label++));
                    forth_code[forth_code_ipc++] = primitive_empty;
                    /* primitive_empty is parameter, use for args combinate*/
                    goto evargs;
                }
            }
            break;
        case DEFMACRO:
            left = c_normal_copy(left);
            present = c_cdr(left);
            batch_write_circuit(primitive_efdefmacro,
                                binary_return_array + (binary_return_label++));

            forth_code[forth_code_ipc++] = primitive_efdefmacro;

            linear_push(skip_obj, primitive_efdefmacro);
            linear_push(skip_obj, c_car(present));
            linear_push(skip_obj, long_type( forth_code_ipc));
            fix_unfix_code(c_car(present), forth_code_ipc, forth_code, recycle_obj);

            forth_code[forth_code_ipc++] = c_copy_atom(c_car(present));
            forth_code[forth_code_ipc++] = c_cadr(present);
            left = c_caddr(present);
            goto label;
        case QUOTE:
            forth_code[forth_code_ipc++] = primitive_efquote;
            forth_code[forth_code_ipc++] = c_cadr(left);
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
            color_fprintf(stderr, COLOR_GREEN, "maybe forth code\r\n");
            break;
        }
    } 
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
 evjoin: 
    left = batch_read_circuit(
                              binary_return_array + --binary_return_label);
    forth_code[forth_code_ipc++] = primitive_efjoin;
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
    present = linear_push(recycle_obj, long_type(forth_code_ipc++));
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
    case SLEEP:
        forth_code[forth_code_ipc++] = primitive_efsleep;
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
    if (fetch_code_skip_address(left, EFDEFUN, skip_obj) == NULL) {
        forth_code_unfix[forth_code_unfix_ipc++] = left;
        cross_sprintf(debug_inf, "function name %s doesn't exist\r\n", left->u_data.s_data);
        forth_code_unfix[forth_code_unfix_ipc++] = linear_push(recycle_obj, long_type(forth_code_ipc));
        forth_code[forth_code_ipc++] = new_debug(debug_inf, cross_strlen(debug_inf));
    } 
    else {
        forth_code[forth_code_ipc++] = fetch_code_skip_address(left, EFDEFUN, skip_obj);
    }
    goto popjreturn;
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
        cross_fprintf(stderr, "eval_impl result label is  %d\r\n", binary_data_label);
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
    case EVJOIN:
        goto evjoin;
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
    case EFITER:
        goto efiter;
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
                              forth_code_ipc, forth_code, recycle_obj);
    goto popjreturn;
 efelseif: 
    left = batch_read_circuit(
                              binary_return_array + --binary_return_label);

    fix_code_relative_address(
                              batch_read_circuit(binary_return_array + --binary_return_label),
                              forth_code_ipc + 2, forth_code, recycle_obj);

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
                forth_code[forth_code_ipc++] = primitive_efdefuntail; 
                /*if  go this way, efendif fix_code_address wouldn't be  nessary,but keep it also harmless*/
                break;
            }
            else if(present->em == EVPROGNEND){
                skip++;
            }
            else if(present->em == EFENDIF){
                skip += 2;
            }
            else{
                cross_fprintf(stderr, "tail elements never found, do it normal way \r\n");
                forth_code[forth_code_ipc++] = primitive_efelseif;
                break;
            }
        }
    }
    else {
        forth_code[forth_code_ipc++] = primitive_efelseif;
    }

    present = linear_push(recycle_obj, long_type(forth_code_ipc++));

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
 efiter: 
 eflambda: 
    fix_code_address(
                     batch_read_circuit(binary_return_array + --binary_return_label),
                     forth_code_ipc, forth_code, recycle_obj);
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
        if (!right) {
            cross_sprintf(debug_inf, "forth_code  null occurs, offset is %d, please check \r\n", (int)i);
            return new_debug(debug_inf, cross_strlen(debug_inf));
        }
        if (right->em == DEBUG) {
            return right;
        }
    }
    return primitive_empty;
}

void * localeval(void * _left, void ** _env) {
    type * status = NULL;
    linear_contain recycle_obj;
    linear_contain skip_obj;
    long  begin = local_forth_code_ipc;
    linear_init(&recycle_obj);
    linear_init(&skip_obj);
    eval_impl(_left, _env, local_forth_code, &local_forth_code_ipc, &recycle_obj, &skip_obj);
    status = analyse_forth(local_forth_code, begin, local_forth_code_ipc); 
    if(status->em == DEBUG){
        return status;
    }
    else{
        return execution_instance(_env, local_forth_code, begin, local_forth_code_ipc);    
    }
}

void* globaleval(void * _left, void ** _env){
    type * status = NULL;
    long  begin = global_forth_code_ipc;
    linear_contain recycle_obj;
    linear_contain skip_obj;
    linear_init(&recycle_obj);
    linear_init(&skip_obj);
    eval_impl(_left, _env, global_forth_code, &global_forth_code_ipc, &recycle_obj, &skip_obj);
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
            if(normal_fun[signx].parameters_num == 100 || now_num == normal_fun[signx].parameters_num){
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
    cross_fprintf(stderr, "defun");
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
    if ((left->em == EMPTY) || 
        (left->em == NOP)) {
        return primitive_empty;
    }
    else if(left->em == VAR || left->em == STORAGE) {
        return primitive_empty;
    }
    else if (left->em == BYTES && left->u_data.i_data == NULLVALUE)
        return primitive_empty;
    else if (left->em == BYTES)
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
    left_fprint(stderr, left);
    cross_fprintf(stderr, "\r\n");    
    head = c_car(left);
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

        present = analyse(c_cadr(left), _env);
        if(present->em == DEBUG) {
            cross_sprintf(debug_inf, "FUNCALL\r\n%s", present->u_data.a_storage + sizeof(int));
            gc_atom(present);
            outcome = new_debug(debug_inf, cross_strlen(debug_inf));
        }
        else{
            outcome = analyse_para(c_cddr(left), _env);
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
            cross_strcpy(debug_inf, "setq 1st should be VAR\r\n");
            outcome = new_debug(debug_inf, cross_strlen(debug_inf));
        } 
        else{
            present = c_cdr (left);
            if(present->em != LIST){
                cross_strcpy(debug_inf, "setq 2st should exist\r\n");
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
            cross_strcpy(debug_inf, "define 1st should be VAR\r\n");
            outcome = new_debug(debug_inf, cross_strlen(debug_inf));
        } 
        else{
            present = c_cdr (present);
            if(present->em != LIST){
                cross_strcpy(debug_inf, "define 2st should exist\r\n");
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
        present = analyse(c_cadr(left), _env);
        if (present->em == DEBUG) {
            cross_sprintf(debug_inf, "IF-CHOICE\r\n%s", present->u_data.a_storage + sizeof(int));
            gc_atom(present);
            return new_debug(debug_inf, cross_strlen(debug_inf));
        }
        present = c_cddr(left);
        if (present->em == EMPTY) {
            cross_strcpy(debug_inf, "IF shoule have then statement\r\n");
            return new_debug(debug_inf, cross_strlen(debug_inf));
        }
        present = c_cdr(present);
        if (present->em == EMPTY) {
            cross_strcpy(debug_inf, "IF shoule have else statement\r\n");
            return new_debug(debug_inf, cross_strlen(debug_inf));
        }
        present = analyse(c_car(present), _env);
        if(present->em == DEBUG){
            cross_sprintf(debug_inf, "IF-LAST\r\n%s", present->u_data.a_storage + sizeof(int));
            gc_atom(present);
            return new_debug(debug_inf, cross_strlen(debug_inf));
        }

        present = analyse(c_caddr(left), _env);
        if(present->em == DEBUG){
            cross_sprintf(debug_inf, "IF-FIRST\r\n%s", present->u_data.a_storage + sizeof(int));
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
            present = *_env;
            left_fprint(stderr, c_car(left));
            cross_fprintf(stderr, "\r\n");                
            right = analyse(c_car(left) , (void**)&present);
            if(right->em == DEBUG ){
                cross_sprintf(debug_inf, "PROGN-MID\r\n%s", right->u_data.a_storage + sizeof(int));
                gc_atom(right);
                return new_debug(debug_inf, cross_strlen(debug_inf));
            }
            left = c_cdr(left);
        }
        left = c_car(left);
        present = analyse(left, _env);
        if(present->em == DEBUG){
            cross_sprintf(debug_inf, "PROGN-LAST\r\n%s", present->u_data.a_storage + sizeof(int));
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
    case JOIN:
        present = c_cdr(left);
        if (present->em == EMPTY) {
            cross_strcpy(debug_inf, "join shoule follows a statement\r\n");
            return new_debug(debug_inf, cross_strlen(debug_inf));
        }

        present = c_cadr(left);
        if(present->em != VAR) {
            cross_strcpy(debug_inf, "join should be VAR\r\n");
            return new_debug(debug_inf, cross_strlen(debug_inf));
        } 
        right = c_caddr(left);
        return analyse(right, _env);
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
        return analyse(right, _env);
        break;
    case FUN:
        present = c_cdr(left);
        if(present->em == EMPTY){
            present = check_fun_parameters(head, 0);
        }
        else{
            present = check_fun_parameters(head, present->obj_length);
        }

        if(present != NULL) {
            return analyse_para(c_cdr(left), _env);
        }
        else{
            modify_fun_content(head);
            cross_sprintf(debug_inf, "%s original fun should have the same parameter num\r\n", head->u_data.s_data);
            return new_debug(debug_inf, cross_strlen(debug_inf));
        }
        break;
    case SLEEP:
        present = c_cdr(left);
        if (present->em == EMPTY) {
            cross_strcpy(debug_inf, "sleep shoule follows a statement\r\n");
            return new_debug(debug_inf, cross_strlen(debug_inf));
        }
        return primitive_empty;
        break;
    case PGET:
        present = c_cdr(left);
        if (present->em != EMPTY) {
            cross_strcpy(debug_inf, "pget shoule not follows a statement\r\n");
            return new_debug(debug_inf, cross_strlen(debug_inf));
        }
        return primitive_empty;
        break;
    case ITER:
    case LAMBDA:
        present = c_cdr(left);
        if (present->em == EMPTY) {
            cross_strcpy(debug_inf, "iter/lambda shoule have parameter list\r\n");
            return new_debug(debug_inf, cross_strlen(debug_inf));
        }
        mid_x = c_car(present);
        if (mid_x->em != LIST) {
            cross_strcpy(debug_inf, "iter/lambda 1st shoule be parameter list\r\n");
            return new_debug(debug_inf, cross_strlen(debug_inf));
        }
        mid_x = c_cdr(present);
        if (mid_x->em == EMPTY) {
            cross_strcpy(debug_inf, "iter/lambda shoule have statement implement\r\n");
            return new_debug(debug_inf, cross_strlen(debug_inf));
        }
        return analyse_para(c_cdr(present), _env);
        break;
    case DEFMACRO:
    case DEFUN:
        present = c_cdr(left);
        if (present->em == EMPTY) {
            cross_strcpy(debug_inf, "DEFUN/DEFMACRO shoule follows a statement\r\n");
            return new_debug(debug_inf, cross_strlen(debug_inf));
        }
        head = c_car(present);
        if (head->em != VAR) {
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

        mid_x = analyse_para(c_cddr(present), _env);
        if (mid_x != NULL) {
            assign((char *) (head->u_data.s_data), c_cdr(present), c_cddr(present), &global_defun);
        }
        return mid_x;
        break;
    case VAR:
        if ((c_find_defun_arg(head, (void ** )&presentarg) && presentarg)) {
            if (calc_length(presentarg) != calc_length(c_cdr(left))) {
                cross_sprintf(debug_inf, "%s user fun should have the same parameter num\r\n", head->u_data.s_data);
                return new_debug(debug_inf, cross_strlen(debug_inf));
            }
            else {
                return analyse_para(c_cdr(left), _env);
            }
        }
        else{
            cross_fprintf(stderr, "%s should be a function name, maybe error, leave for link detect \r\n", head->u_data.s_data);
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
    case BYTES:
        cross_strcpy(debug_inf, "list can't begin with BYTES\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
        break;
    default:
        cross_strcpy(debug_inf, "list begin with abnormal symbol\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf));
        break;
    }
    return outcome;
}

/*(fake thread system)*/
void* pcreate_closure(void * _left) {
    void** _env = (void**)c_car(_left);
    type* real_left = c_cadr(_left);

    c_unbindvars(_env);
    cross_free(_env);

    gc(real_left);
    gc_frame(_left);

    return NULL;
}

void * original_pcreate(void * _left) {
    void** forth_code;
    char* this = this_now;

    type* outcome = primitive_empty;
    type* worker = primitive_empty;
    type* para = primitive_empty;
    char   debug_inf[256] = "\0";
    int forth_find;
    int forth_code_ipc;
    long priority;
    void** _env = NULL;
    entry code_result, stream_result;

    worker = c_car(_left);
    if (worker->em != BYTES) {
        cross_snprintf(debug_inf, 256, "1st should be BYTES\r\n");
        return new_debug(debug_inf, strlen(debug_inf));
    }
    priority = worker->u_data.i_data;

    worker = c_cadr(_left);
    if (worker->em != VAR) {
        cross_snprintf(debug_inf, 256, "2st should be VAR as function name\r\n");
        return new_debug(debug_inf, strlen(debug_inf));
    }
    else {
        code_result = lookup((char *)this,
                             global_code);
        if (code_result._env) {
            forth_code = (void**)code_result.u_data._expr;
        }

        stream_result = lookup((char *)this,
                               global_stream);
        if (stream_result._env){
            forth_code_ipc = (long)stream_result._env;
        }

        forth_find = (long)iterate_address(worker, forth_code, forth_code_ipc);
        if (forth_find != 0) {
            para = forth_code[forth_find + 1];
            outcome = c_cddr(_left);
            if(calc_length(para) != calc_length(outcome)) {
                cross_snprintf(debug_inf, 256, "para key and value shoule be the same\r\n");
                return new_debug(debug_inf, strlen(debug_inf));
            }

            //            _env = (void**)cross_calloc(sizeof(void*), 1);
            _env = (void**)cross_calloc(sizeof(void*), BUF_SIZE/sizeof(void*));            
            *_env = primitive_empty;
            /*need not c_normal_copy outcome, think why*/            
            c_bindvars(para, outcome, _env);
            outcome = random_name();
            prepare_dispatch(outcome->u_data.s_data,
                             _env, forth_code, 
                             forth_find + 2, forth_code_ipc, 2,
                             pcreate_closure, c_list(_env, c_normal_copy(_left), 0), priority);

            
        }
        else  {
            cross_snprintf(debug_inf, 256, "2st not a function name\r\n");
            return new_debug(debug_inf, strlen(debug_inf));            
        }                  
    }      

    return outcome;  
    /*modify pjoin mean,so there need change according.  2014.5.31*/
}

void * original_pjoin(void * _left) {
    type * left = c_car(_left);
    char debug_inf[256] = "\0";

    if (left->em != VAR) {
        cross_snprintf(debug_inf, 256, "1st should be VAR\r\n");
        return new_debug(debug_inf, strlen(debug_inf));
    }

    return dispatch_core(left->u_data.s_data);
}

/*(fake thread system end)*/


void * c_bindvar_help(void * name, void * value) {
    return c_cons(c_copy_atom(name), c_cons(c_copy_atom(value), primitive_empty));
}

void * c_bindvar(void * _left, void * _right) {
    type * left = _left, *right = _right, *outcome;
    if (left->em == EMPTY || right->em == EMPTY) {
        /*2013.6.5 for c_car speed modify*/
        return primitive_empty;
    } 
    else {
        outcome = c_cons(c_bindvar_help(c_car(left), c_car(right)),
                         c_bindvar(c_cdr(left), c_cdr(right)));
        return outcome;
    }
}

void c_bindvars(void* _left, void* _right, void** _env) {
    type * left = _left;
    type * right = _right;
    /*solve  autotest  null-parameter cause memory leak*/
    if (left->em != EMPTY) {
        *_env = c_cons(c_bindvar(left, right), *_env);
    } 
}

void c_unbindvars(void ** _env) {
    type * right = c_cdr(*_env);

    gc(c_car(*_env));
    gc_atom(*_env);

    *_env = right;
}

void * analyse_para(void * _left, void ** _env) {
    type * present, *env = *_env;
    type * left = _left;
    char debug_inf[1024] = "\0";

    if (left->em == EMPTY) {
        return primitive_empty;
    }
    else {
        present = analyse(c_car(left), (void **)&env);
        if(present->em == DEBUG) {
            cross_sprintf(debug_inf, "analyse_para:\r\n%s", present->u_data.a_storage + sizeof(int));
            gc_atom(present);
            return new_debug(debug_inf, cross_strlen(debug_inf));
        }
        return analyse_para(c_cdr(left), _env);
    }
}

static int number; /*  if NUMBER: numerical value  */
static signed char arabic[ARABIC];
static char alpha_ex[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_!.:LC@#=&?*$/<{}";

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
    memset(arabic, 0, sizeof(arabic));

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
            arabic[sign++] = *bp++;

        if (sign >= ARABIC) {
            color_fprintf(stderr, COLOR_RED, "factor %s string too long\r\n", arabic);
            fflush(stderr);
            exit(-1);
        }
    } 
    else {
        token = VALUE;
        arabic[sign++] = *bp ? *bp++ : 0;
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

static void * factor(char **_now) {
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
        if ((is_quote == 0) && (outcome = fun_type(arabic))) {
            return outcome;
        } else if ((is_quote == 0) && (type = is_operate_type(arabic))) {
            return operate_type(type);
        } else if (!strcmp("nil", (char*)arabic)) {
            return primitive_empty;
        } else if (!strcmp("nop", (char*)arabic)) {
            return primitive_nop;
        } else if (!strcmp("LINE", (char*)arabic)) {
            return var_type("\r\n");
        } else if (!strcmp("ENTER", (char*)arabic)) {
            return var_type("\n");
        } else if (!strcmp("SPACE", (char*)arabic)) {
            return var_type(" ");
        } else if (!strcmp("TAB", (char*)arabic)) {
            return var_type("    ");
        } else if (!strcmp("BRACKETL", (char*)arabic)) {
            return var_type("[");
        } else if (!strcmp("BRACKETR", (char*)arabic)) {
            return var_type("]");
        } else if (!strcmp("BRACEL", (char*)arabic)) {
            return var_type("{");
        } else if (!strcmp("BRACER", (char*)arabic)) {
            return var_type("}");
        } else if (!strcmp("CURVEL", (char*)arabic)) {
            return var_type("(");
        } else if (!strcmp("CURVER", (char*)arabic)) {
            return var_type(")");
        } else if (!strcmp("HTML", (char*)arabic)) {
            return var_type("<html");
        } else if (!strcmp("DOT", (char*)arabic)) {
            return var_type(",");
        } else if (!strcmp("DOUBLE_QUOTE", (char*)arabic)) {
            return var_type("\"");
        } else if (!strcmp("t", (char*)arabic)) {
            return long_type(1);
        } 
        else {
            return var_type((char*)arabic);
        }
    case NUMBER:
        return long_type(number);
        break;
    case VALUE:
        switch (arabic[0]) {
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
                    left_fprint(stderr, ele_right);
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
                cross_fprintf(stderr, "storage is  %s  \r\n", responsex + sizeof(int));
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
                        ele_left = c_append(ele_left, c_cons(ele_right, primitive_empty));
                    } 
                    else {
                        break;
                    }

                }
                return ele_left;
            }
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
                    cross_strcpy(debug_inf, "' shoule follow with arabic\r\n");
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
                    cross_strcpy(debug_inf, "' shoule follow with arabic\r\n");
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
                cross_strcpy(debug_inf, "` shoule follow with arabic\r\n");
                return new_debug(debug_inf, cross_strlen(debug_inf));
            }
            if(ele_right->em == DEBUG) return ele_right;
            return c_list(operate_type(QUOTEX), ele_right, 0);
        case ',':
            ele_right = factor(_now);
            if( ele_right == NULL) {
                cross_strcpy(debug_inf, ", shoule follow with arabic\r\n");
                return new_debug(debug_inf, cross_strlen(debug_inf));
            }
            if(ele_right->em == DEBUG) return ele_right;
            return c_list(operate_type(EVAL), ele_right, 0);  /*modify 2014.6.30, for macro forth */
        default:
            if (arabic[0] == 0) {
                cross_strcpy(debug_inf, "shoule end with ), you can type (help) for help\r\n");
                return new_debug(debug_inf, cross_strlen(debug_inf));
            }
            if (arabic[0] < 0) {
                /*maybe some unexpected error*/
                return NULL;
            }
            cross_sprintf(debug_inf, "%s %c %u error fun arabic\r\n", arabic, arabic[0], arabic[0]);
            return new_debug(debug_inf, cross_strlen(debug_inf));
            break;
        }
    default:
        cross_strcpy(debug_inf, "unexpect character\r\n");
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
    time_t now;  
    int unixtime;

    /*follow code is special too. use calloc instead of cross_calloc*/
    mem_manager_unused = (wraptype *) cross_calloc(sizeof(wraptype), global_count);

    for (i = 0; i < global_count - 1; i++) {
        mem_manager_unused[i].mem_next = &mem_manager_unused[i + 1];
    }
    mem_manager_unused[global_count - 1].mem_next = NULL;

    unixtime = time(&now);  
    cross_fprintf(stderr, "initrand unixtime is %d\r\n", unixtime);
    srand(unixtime);
    rand_next = unixtime;

    global_frame = init_flexhash(7, 10, ARABIC, sizeof(entry), NULL, NULL);
    global_stack = init_flexhash(7, 10, ARABIC, sizeof(entry), NULL, NULL);
    global_stream = init_flexhash(7, 10, ARABIC, sizeof(entry), NULL, NULL);
    global_code = init_flexhash(7, 10, ARABIC, sizeof(entry), NULL, NULL);
    global_status = init_flexhash(7, 10, ARABIC, sizeof(entry), NULL, NULL);

    global_lambda = init_flexhash(7, 10, ARABIC, sizeof(entry), NULL, NULL);
    global_defun = init_flexhash(13, 10, ARABIC, sizeof(entry), NULL, NULL);
    global_var = init_flexhash(13, 10, ARABIC, sizeof(entry), NULL, NULL);
    global_define = init_flexhash(7, 10, ARABIC, sizeof(entry), NULL, NULL);
    global_primitive = init_flexhash(7, 10, ARABIC, sizeof(entry), NULL, NULL);

    binary_return_array = (void**)cross_calloc(sizeof(void*), FORTH_WORD);
    binary_data_array = (void**)cross_calloc(sizeof(void*), FORTH_WORD);

    global_forth_code = (void**)cross_calloc(sizeof(void*), FORTH_WORD);
    local_forth_code = (void**)cross_calloc(sizeof(void*), FORTH_WORD);

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
        current->content = (void*)calloc(BUF_SIZE, sizeof(char));
        raw_mem_manager_unused[i].mem_next = &raw_mem_manager_unused[i + 1];
    }

    raw_mem_manager_unused[raw_global_count - 1].mem_next = NULL;
}

#define   PRIMITIVE_PREFIX(PTR)    primitive_##PTR
#define   PRIMITIVE_INIT(PTR, TYPE)    PRIMITIVE_PREFIX(PTR) = new_object(); PRIMITIVE_PREFIX(PTR)->em = TYPE;
void init_primitive_var(void) {
    type *left;
    int i = 0;

    for(i = 0; i <= AMOUNT; i++){
        primitive_small[i] = new_object();
        primitive_small[i]->em = BYTES;
        primitive_small[i]->u_data.i_data = 0 - (AMOUNT - i);
        primitive_small[i]->ref_count = 1000;
    }

    for(i = AMOUNT + 1; i < 2 * AMOUNT + 1 + 1; i++){
        primitive_small[i] = new_object();
        primitive_small[i]->em = BYTES;
        primitive_small[i]->u_data.i_data = i - AMOUNT;
        primitive_small[i]->ref_count = 1000;
    }

    left = new_object();
    mem_manager_reserved = primitive_reserved = left->mother;

    left = new_object();
    mem_manager_used = primitive_used = left->mother;

    PRIMITIVE_INIT(empty, EMPTY)
        primitive_empty->u_data.i_data = NULLVALUE;

    PRIMITIVE_INIT(nop, NOP)
        primitive_nop->u_data.i_data = NULLVALUE;

    PRIMITIVE_INIT(cont, CONT)
        primitive_cont->u_data.i_data = NULLVALUE;

    PRIMITIVE_INIT(evif, EVIF)
        PRIMITIVE_INIT(evfuncall, EVFUNCALL)
        PRIMITIVE_INIT(evfuncallmid, EVFUNCALLMID)
        PRIMITIVE_INIT(evfuncallend, EVFUNCALLEND)
        PRIMITIVE_INIT(evmacro, EVMACRO)
        PRIMITIVE_INIT(evsetq, EVSETQ)
        PRIMITIVE_INIT(evdefine, EVDEFINE)
        PRIMITIVE_INIT(evargscombi, EVARGSCOMBI)
        PRIMITIVE_INIT(evargs, EVARGS)
        PRIMITIVE_INIT(evunbindvars, EVUNBINDVARS)
        PRIMITIVE_INIT(evprogn, EVPROGN)
        PRIMITIVE_INIT(evprognmid, EVPROGNMID)
        PRIMITIVE_INIT(evprognend, EVPROGNEND)
        PRIMITIVE_INIT(evpress, EVPRESS)
        PRIMITIVE_INIT(evjoin, EVJOIN)
        PRIMITIVE_INIT(effun, EFFUN)
        PRIMITIVE_INIT(efyield, EFYIELD)
        PRIMITIVE_INIT(efstop, EFSTOP)
        PRIMITIVE_INIT(efnext, EFNEXT)
        PRIMITIVE_INIT(efisstop, EFISSTOP)
        PRIMITIVE_INIT(efstdin, EFSTDIN)
        PRIMITIVE_INIT(efeofstdin, EFEOFSTDIN)
        PRIMITIVE_INIT(efdefun, EFDEFUN)
        PRIMITIVE_INIT(efdefunend, EFDEFUNEND)
        PRIMITIVE_INIT(efdefuntail, EFDEFUNTAIL)
        PRIMITIVE_INIT(efdefmacro, EFDEFMACRO)
        PRIMITIVE_INIT(efdefmacroend, EFDEFMACROEND)
        PRIMITIVE_INIT(efsapply, EFSAPPLY)
        PRIMITIVE_INIT(efsapplyx, EFSAPPLYX)
        PRIMITIVE_INIT(efsapplyxbegin, EFSAPPLYXBEGIN)
        PRIMITIVE_INIT(efif, EFIF)
        PRIMITIVE_INIT(efelseif, EFELSEIF)
        PRIMITIVE_INIT(efendif, EFENDIF)
        PRIMITIVE_INIT(efprognmid, EFPROGNMID)
        PRIMITIVE_INIT(efquote, EFQUOTE)
        PRIMITIVE_INIT(efquotex, EFQUOTEX)
        PRIMITIVE_INIT(efquotexend, EFQUOTEXEND)
        PRIMITIVE_INIT(efsetq, EFSETQ)
        PRIMITIVE_INIT(efdefine, EFDEFINE)
        PRIMITIVE_INIT(eflambda, EFLAMBDA)
        PRIMITIVE_INIT(efiter, EFITER)
        PRIMITIVE_INIT(efsleep, EFSLEEP)
        PRIMITIVE_INIT(efpget, EFPGET)
        PRIMITIVE_INIT(effuncall, EFFUNCALL)
        PRIMITIVE_INIT(efself, EFSELF)
        PRIMITIVE_INIT(eflambdaend, EFLAMBDAEND)
        PRIMITIVE_INIT(efiterend, EFITEREND)
        PRIMITIVE_INIT(effuncallend, EFFUNCALLEND)
        PRIMITIVE_INIT(efeject, EFEJECT)
        PRIMITIVE_INIT(eftop, EFTOP)
        PRIMITIVE_INIT(efcombi, EFCOMBI)
        PRIMITIVE_INIT(efexchange, EFEXCHANGE)
        PRIMITIVE_INIT(efwhole, EFWHOLE)
        PRIMITIVE_INIT(efjoin, EFJOIN)
        PRIMITIVE_INIT(efpress, EFPRESS)
        PRIMITIVE_INIT(eflistbegin, EFLISTBEGIN)
        PRIMITIVE_INIT(eflistend, EFLISTEND)

#ifdef FORTH
        PRIMITIVE_INIT(zfsapply, ZFSAPPLY)
        PRIMITIVE_INIT(zfcolon, ZFCOLON)
        PRIMITIVE_INIT(zfend, ZFEND)
        PRIMITIVE_INIT(zfif, ZFIF)
        PRIMITIVE_INIT(zfelseif, ZFELSEIF)
        PRIMITIVE_INIT(zfthen, ZFTHEN)
#endif
}

void * eval(void** _env, char *buf) {
    type * ele_left;
    type * ele_right, * present = primitive_empty;
    type * outcome = primitive_empty;
    enum tokens token;
    char *now = NULL;
    char debug_inf[256] = "\0";
    void* m_env = primitive_empty;

    now = buf;
    ele_left = factor(&now);
    if (ele_left->em == DEBUG) {
        return ele_left;
    }

    if (0 != *now) {
        gc(left_fprint(stderr, ele_left));
        cross_fprintf(stderr, "\r\n");
        cross_strcpy(debug_inf, "unexpected character occurs\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf)); 
    }

    present = analyse(ele_left, (void **)&m_env);
    if(present->em != DEBUG) {
        outcome = left_fprint(stderr, globaleval(ele_left, (void **)_env));
    }
    else{
        left_fprint(stderr, present);
        outcome = present;
    }
    cross_fprintf(stderr, "\r\n");
    return outcome;
}

void * evaljit_closure(void * _left) {
    type * symbol, * value;
    void** dis_forth_code = (void**)c_car(_left);
    void** _env = (void**)c_cadr(_left);
    linear_contain* recycle_obj = (linear_contain*)c_caddr(_left);
    linear_contain* skip_obj = (linear_contain*)c_cadr(c_cddr(_left));
    type* ele_right = c_caddr(c_cddr(_left));
    symbol = c_cadr(c_cddr(c_cddr(_left)));
    value = c_caddr(c_cddr(c_cddr(_left)));

    cross_free(dis_forth_code);

    gc(symbol);
    gc(value);

    c_unbindvars(_env);
    cross_free(_env);

    linear_free(recycle_obj);
    cross_free(recycle_obj);

    cross_free(skip_obj);

    gc(ele_right);

    gc_frame(_left);
    return primitive_empty;
}

void * evaljit(char *buf, int socket) {
    type * local_env = primitive_empty;
    type * ele_left;
    type * ele_right;
    type * symbol, * value;
    type * outcome = primitive_empty;

    long  begin = 0;
    long  end = 0;

    void** dis_forth_code = (void**)0;

    linear_contain* recycle_obj;
    linear_contain* skip_obj;

    void** _env = NULL;
    enum tokens token;
    char *now = NULL;
    char debug_inf[256] = "\0";

    now = buf;
    ele_left = factor(&now);
    if (ele_left->em == DEBUG) {
        return ele_left;
    }

    if (0 != *now) {
        gc(left_fprint(stderr, ele_left));
        cross_fprintf(stderr, "\r\n");
        cross_strcpy(debug_inf, "unexpected character occurs\r\n");
        return new_debug(debug_inf, cross_strlen(debug_inf)); 
    }

    ele_right = ele_left;
    outcome = analyse(ele_left, (void **) &local_env);
    if (outcome->em != DEBUG) {
        recycle_obj = (linear_contain*) cross_calloc(sizeof(linear_contain), 1);
        skip_obj = (linear_contain*) cross_calloc(sizeof(linear_contain), 1);
        linear_init(recycle_obj);
        linear_init(skip_obj);

        _env = (void**) cross_calloc(sizeof(void*), 1);
        *_env = primitive_empty;
        symbol = c_list(var_type("nothing"), 0);
        value = c_list(var_type("nothing"), 0);
        c_bindvars(symbol, value, _env);
        dis_forth_code = (void**)cross_calloc(sizeof(void*), FORTH_WORD);
        eval_impl(ele_left, _env, dis_forth_code, &end, recycle_obj, skip_obj);
        outcome = analyse_forth(dis_forth_code, begin, end);
        if(outcome->em != DEBUG) {
            outcome = random_name();
            prepare_dispatch(outcome->u_data.s_data, _env, 
                             dis_forth_code, begin, end, socket, evaljit_closure, 
                             c_list(dis_forth_code, _env, recycle_obj, skip_obj, ele_right, symbol, value, 0), 0);    
        }
        else {
            evaljit_closure(c_list(dis_forth_code, _env, recycle_obj, skip_obj, ele_right, symbol, value, 0));
        }
    }
    else {
        gc(ele_right);
    }

    gc(local_env);
    return outcome;
}

void init_meta() {
    init_raw_object();
    init_raw_var();

    init_object();
    init_primitive_var();
    init_primitive();
}

void high_tech(void** _env) {
    type * ele_left;
    /*advance  high-tech*/
    ele_left = c_list(operate_type(DEFMACRO), var_type("demo"),
                      c_list(var_type("expr"), 0),
                      c_list(fun_type("print"), var_type("expr"), 0), 0);
    globaleval(ele_left, (void **)_env);

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

    globaleval(ele_left, (void **)_env);
}

int main(int argc, char ** argv) {
    int sign;
    char buf[1024 * 20] = "(progn ";
    void** _env = NULL;
    type* present = NULL;
    type* para = NULL;

#if defined(_WIN32) && defined(MYSELECT)
    init_socket();
#endif

    dispatch_init();
    init_meta();

    _env = (void**) cross_calloc(sizeof(void*), 1);
    *_env = primitive_empty;
    high_tech(_env);
    at_init(&global_timer_loop, 5000);

    sign = 0;
    if (argc == 1) {
        gc(original_display(primitive_empty));
        cross_fprintf(stderr, "WELCOME!!\r\n");
        cross_fprintf(stderr, "arabic>");
        while (fgets(buf, 10240, stdin) != NULL) {
            para = new_storage(buf, strlen(buf));
            para = c_list(para, long_type(2), 0);
            present = original_eval(para);
            gc(para);
            left_fprint(stderr, present);
            cross_fprintf(stderr, "\r\n");
            gc(present);
            gc(original_display(primitive_empty));
            cross_fprintf(stderr, "\r\narabic>");
        }
    } 
    else {
        if (cross_strstr(argv[1], ".elf")) {
            /*milestone vm machine.*/
            char file_tmp_name[256] = "";
            cross_sprintf(file_tmp_name, "%s", argv[1]);
            read_forth_code(file_tmp_name);
            gc(left_fprint(stderr, execution_instance(_env, global_forth_code, 0, global_forth_code_ipc)));
        }
        else{
            cross_read(argv[1], &sign, buf + strlen(buf));
            strcat(buf, " )");
            if(cross_strstr(argv[1], ".elc")) {
#if defined(HUFFMAN)
                cross_strcpy(global_filename, argv[1]);
                global_filename[ cross_strlen(global_filename) - 1] = '\0'; 
                init_indicator();
                huffman_decryption((unsigned char  *)buf, sign);
                release_indicator();
                gc(left_fprint(stderr, eval(_env, buf)));
#endif
            }
            else{
                cross_strcpy(global_filename, argv[1]);
#if defined(HUFFMAN)
                init_indicator();
                wrap_process(buf, sign + 10, argv[1]); /*modify by rosslyn.2013.6.9*/
                release_indicator();
#endif
                gc(left_fprint(stderr, eval((void**)_env, buf)));
            }
        }
    }

#if defined(_WIN32) && defined(MYSELECT)
    cleanup_socket();
#endif

    fflush(stdout);
    return 0;
}
