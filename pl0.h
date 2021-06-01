/*
 * PL/0 complier program for win32 platform (implemented in C)
 *
 * The program has been test on Visual C++ 6.0,  Visual C++.NET and
 * Visual C++.NET 2003,  on Win98,  WinNT,  Win2000,  WinXP and Win2003
 *
 */

//typedef enum {
//    false,
//    true
//} bool;

#define txmax 100   /* 名字表容量 */
#define nmax 14     /* number的最大位数 */
#define al 10       /* 符号的最大长度 */
#define amax 2047   /* 地址上界*/
#define levmax 3    /* 最大允许过程嵌套声明层数 [0,  levmax]*/
#define cxmax 500   /* 最多的虚拟机代码数 */
#define strmax 100  /* 新增：字符串的最大长度 */

/* 符号 */ 
// 新增保留字：else, for, to, downto, return
// 新增运算符：+=, -=, *=, /=, ++, --, []
// 新增注释：/*...*/
#define norw 23     /* 保留字个数，从13个增加为23个 */
#define symnum 50   // 符号数从32增加到50
enum symbol {
    nul,         ident,     number,     plus,      minus,
    times,       slash,     oddsym,     eql,       neq,
    lss,         leq,       gtr,        geq,       lparen,
    rparen,      comma,     semicolon,  period,    becomes,
    beginsym,    endsym,    ifsym,      thensym,   whilesym,
    writesym,    readsym,   dosym,      callsym,   constsym,
    varsym,      procsym,
    // 新增 
	elsesym,	 breaksym,
	forsym,    	 tosym,		downtosym,	stepsym,
	stringsym,	 arraysym,	intsym,		floatsym,
	pluseq,		 minuseq,	timeseq,	slasheq,   plusplus,	minusminus,
    lbracket,	 rbracket,
};

const char* error_msg[] = {
	"",
	"把:=写成了=",//1 
	"常量声明=后应是数字",//2
    "常量说明标识后应是=",//3
    "const后应是标识",//4
    "漏掉了,或;",//5
    "过程名错误",//6
    "",//7
    "语句后跟符号错误",//8
    "程序末尾缺少.",//9
    "语句末尾缺少;",//10
    "标识符未声明",//11
    "非法的赋值类型",//12
    "没有检测到赋值符号:=、变量后++或变量后--运算",//13
    "call后缺少标识符",//14
    "call后标识符不是过程",//15
    "缺少then",//16
    "缺少end或;",//17
    "缺少do",//18
    "非变量使用了赋值或++、--运算",//19
    "条件处理语句中没有检测到逻辑运算符",//20
    "表达式中不能出现无返回值的过程",//21
    "缺少)",//22
    "因子后有非法符号",//23
    "表达式的开始符为非法符号",//24
    "字符串长度超出范围",//25
    "",//26
    "break未写在循环中！",//27
    "for语句缺少step或until!",//28
    "for语句缺少do",//29
    "整型变量超出范围",//30
    "整形常量超出范围",//31
    "超出最大嵌套层数",//32
    "read()或write()函数调用缺少)",//33
    "read()或write()函数调用缺少(",//34
    "read()函数中应是声明过的变量名",//35
    "变量声明不为int或float",//36
    "格式化输出不是标识符或数字或标识符或数字的数量不足",//37
    "格式化字符串中%后缺少d或f",//38
    "格式化输出未知类型",//39
    "格式化字符串中出现未知转义类型",//40
};

/* 名字表中的类型 */
// 新增类型：charvar, array
enum object {
    constant,
    variable,
    procedur,
    string,		// 新增 
    array		// 新增 
};

/* 虚拟机代码 */
#define fctnum 8
enum fct {
    lit,     opr,     lod,
    sto,     cal,     inte,
    jmp,     jpc,
};

/* 虚拟机代码结构 */
struct instruction
{
    enum fct f; /* 虚拟机代码指令 */
    int l;      /* 引用层与声明层的层次差 */
    int a;      /* 根据f的不同而不同 */
};

FILE* fas;  /* 输出名字表 */
FILE* fa;   /* 输出虚拟机代码 */
FILE* fa1;  /* 输出源文件及其各行对应的首地址 */
FILE* fa2;  /* 输出结果 */
bool listswitch;    /* 显示虚拟机代码与否 */
bool tableswitch;   /* 显示名字表与否 */
char ch;            /* 获取字符的缓冲区，getch 使用 */
enum symbol sym;    /* 当前的符号 */
char id[al+1];      /* 当前ident, 多出的一个字节用于存放0 */
float num;            /* 当前number 新增：支持浮点数 */
int cc, ll;         /* getch使用的计数器，cc表示当前字符(ch)的位置 */
int cx;             /* 虚拟机代码指针, 取值范围[0, cxmax-1]*/
char line[81];      /* 读取行缓冲区 */
char a[al+1];       /* 临时符号, 多出的一个字节用于存放0 */
struct instruction code[cxmax]; /* 存放虚拟机代码的数组 */
char word[norw][al];        /* 保留字 */
enum symbol wsym[norw];     /* 保留字对应的符号值 */
enum symbol ssym[256];      /* 单字符的符号值 */
char mnemonic[fctnum][5];   /* 虚拟机代码指令名称 */
bool declbegsys[symnum];    /* 表示声明开始的符号集合 */
bool statbegsys[symnum];    /* 表示语句开始的符号集合 */
bool facbegsys[symnum];     /* 表示因子开始的符号集合 */
char format_str[strmax]; 	/* 新增：当前的标准化输出字符串 */
char format_str_total[100][strmax]; 	/* 新增：所有标准化输出字符串 */
int format_str_size;        /* 新增：标准化输出字符串总数量 */
int cur_str_ptr;            /* 新增：当前标准化输出字符串的字符指针 */

/* 名字表结构 */
struct tablestruct
{
    char name[al];      /* 名字 */
    enum object kind;   /* 类型：const, var, procedure, array or string */
    float val;          /* 新增：整型或浮点型数值，仅const使用 */
    int level;          /* 所处层，仅const不使用 */
    int adr;            /* 地址，仅const不使用 */
    int size;           /* 需要分配的数据区空间, 仅procedure使用 */
    int type;			/* 新增：数值类型，整型数为 0，浮点数为 1 */ 
};

struct tablestruct table[txmax]; /* 名字表 */

FILE* fin;
FILE* fout;
char fname[al];
int err; /* 错误计数器 */

/* 当函数中会发生fatal error时，返回-1告知调用它的函数，最终退出程序 */
#define getsymdo                      if(-1 == getsym()) return -1
#define getchdo                       if(-1 == getch()) return -1
#define testdo(a, b, c)               if(-1 == test(a, b, c)) return -1
#define gendo(a, b, c)                if(-1 == gen(a, b, c)) return -1
#define expressiondo(a, b, c)         if(-1 == expression(a, b, c)) return -1
#define factordo(a, b, c)             if(-1 == factor(a, b, c)) return -1
#define termdo(a, b, c)               if(-1 == term(a, b, c)) return -1
#define conditiondo(a, b, c)          if(-1 == condition(a, b, c)) return -1
#define statementdo(a, b, c)          if(-1 == statement(a, b, c)) return -1
#define constdeclarationdo(a, b, c)   if(-1 == constdeclaration(a, b, c)) return -1
#define vardeclarationdo(a, b, c)     if(-1 == vardeclaration(a, b, c)) return -1
#define arraydeclarationdo(a, b, c)     if(-1 == arraydeclaration(a, b, c)) return -1		// 新增 

void error(int n);
int getsym();
int getch();
void init();
int gen(enum fct x, int y, float z);
int test(bool* s1, bool* s2, int n);
int inset(int e, bool* s);
int addset(bool* sr, bool* s1, bool* s2, int n);
int subset(bool* sr, bool* s1, bool* s2, int n);
int mulset(bool* sr, bool* s1, bool* s2, int n);
int block(int lev, int tx, bool* fsys);
void interpret();
int factor(bool* fsys, int* ptx, int lev);
int term(bool* fsys, int* ptx, int lev);
int condition(bool* fsys, int* ptx, int lev);
int expression(bool* fsys, int* ptx, int lev);
int statement(bool* fsys, int* ptx, int lev);
void listcode(int cx0);
int vardeclaration(int* ptx, int lev, int* pdx);
int constdeclaration(int* ptx, int lev, int* pdx);
int arraydeclaration(int* ptx, int lev, int* pdx);	// 新增 
int position(char* idt, int tx);
void enter(enum object k, int* ptx, int lev, int* pdx);
int base(int l, float* s, int b);
