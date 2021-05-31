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

#define norw 23     /* 保留字个数，从13个增加为23个 */
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
#define symnum 50   // 符号数从32增加到50

const char* error_msg[] = {
	"",
	"把:=写成了=",//1 
	"常量声明=后应是一个数字",//2
    "常量说明标识后应是=",//3
    "const后应是标识",//4
    "漏掉了,或;",//5
    "过程名错误！",//6
    "需要声明",//7
    "声明后边是一个不正确的符号",//8
    "少了'.'，程序无法正常结束",//9
    "少了';'",//10
    "发现未声明的标识符！",//11
    "非法赋值",//12
    "少了':='",//13
    "call之后缺少标识符！",//14
    "call之后标识符不是过程！",//15
    "少了then",//16
    "缺少';'或者end",//17
    "少了do",//18
    "符号错误",//19
    "条件语句中未发现操作符（“#，>”等）",//20
    "不能把过程的标识符放在表达式里！",//21
    "单引号后未跟单引号，词法分析出错！",//22
    "符号后面不能跟着<因子>",//23
    "符号不能作为<表达式>的开始！",//24
    "数组声明有误",//25
    "write里面不是表达式或字符串！",//26
    "break未写在循环中！",//27
    "for语句缺少step或until!",//28
    "for 语句循环变量类型错误！",//29
    "数字过大！",//30
    "常量超过可定义的最大值！",//31
    "超过允许的最大嵌套层数，层数太多啦！",//32
    "格式错误，应是右括号')'",//33
    "格式错误，应是左括号'('",//34
    "read里不是标识符ID,或该标识符未声明",//35
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
enum fct {
    lit,     opr,     lod,
    sto,     cal,     inte,
    jmp,     jpc,
};
#define fctnum 8

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
char str[strmax];			/* 新增：字符串 */ 

/* 名字表结构 */
struct tablestruct
{
    char name[al];      /* 名字 */
    enum object kind;   /* 类型：const, var, procedure, array or string */
    int val;            /* 整型数值，仅const使用 */
    float fval;			/* 新增：浮点型数值，仅const使用 */ 
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
#define chardeclarationdo(a, b, c)    if(-1 == chardeclaration(a, b, c)) return -1		// 新增 

void error(int n);
int getsym();
int getch();
void init();
int gen(enum fct x, int y, int z);
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
