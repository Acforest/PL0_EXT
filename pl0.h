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

#define txmax 100   /* ���ֱ����� */
#define nmax 14     /* number�����λ�� */
#define al 10       /* ���ŵ���󳤶� */
#define amax 2047   /* ��ַ�Ͻ�*/
#define levmax 3    /* ����������Ƕ���������� [0,  levmax]*/
#define cxmax 500   /* ��������������� */
#define strmax 100  /* �������ַ�������󳤶� */

/* ���� */ 
// ���������֣�else, for, to, downto, return
// �����������+=, -=, *=, /=, ++, --, []
// ����ע�ͣ�/*...*/
#define norw 23     /* �����ָ�������13������Ϊ23�� */
#define symnum 50   // ��������32���ӵ�50
enum symbol {
    nul,         ident,     number,     plus,      minus,
    times,       slash,     oddsym,     eql,       neq,
    lss,         leq,       gtr,        geq,       lparen,
    rparen,      comma,     semicolon,  period,    becomes,
    beginsym,    endsym,    ifsym,      thensym,   whilesym,
    writesym,    readsym,   dosym,      callsym,   constsym,
    varsym,      procsym,
    // ���� 
	elsesym,	 breaksym,
	forsym,    	 tosym,		downtosym,	stepsym,
	stringsym,	 arraysym,	intsym,		floatsym,
	pluseq,		 minuseq,	timeseq,	slasheq,   plusplus,	minusminus,
    lbracket,	 rbracket,
};

const char* error_msg[] = {
	"",
	"��:=д����=",//1 
	"��������=��Ӧ������",//2
    "����˵����ʶ��Ӧ��=",//3
    "const��Ӧ�Ǳ�ʶ",//4
    "©����,��;",//5
    "����������",//6
    "",//7
    "��������Ŵ���",//8
    "����ĩβȱ��.",//9
    "���ĩβȱ��;",//10
    "��ʶ��δ����",//11
    "�Ƿ��ĸ�ֵ����",//12
    "û�м�⵽��ֵ����:=��������++�������--����",//13
    "call��ȱ�ٱ�ʶ��",//14
    "call���ʶ�����ǹ���",//15
    "ȱ��then",//16
    "ȱ��end��;",//17
    "ȱ��do",//18
    "�Ǳ���ʹ���˸�ֵ��++��--����",//19
    "�������������û�м�⵽�߼������",//20
    "���ʽ�в��ܳ����޷���ֵ�Ĺ���",//21
    "ȱ��)",//22
    "���Ӻ��зǷ�����",//23
    "���ʽ�Ŀ�ʼ��Ϊ�Ƿ�����",//24
    "�ַ������ȳ�����Χ",//25
    "",//26
    "breakδд��ѭ���У�",//27
    "for���ȱ��step��until!",//28
    "for���ȱ��do",//29
    "���ͱ���������Χ",//30
    "���γ���������Χ",//31
    "�������Ƕ�ײ���",//32
    "read()��write()��������ȱ��)",//33
    "read()��write()��������ȱ��(",//34
    "read()������Ӧ���������ı�����",//35
    "����������Ϊint��float",//36
    "��ʽ��������Ǳ�ʶ�������ֻ��ʶ�������ֵ���������",//37
    "��ʽ���ַ�����%��ȱ��d��f",//38
    "��ʽ�����δ֪����",//39
    "��ʽ���ַ����г���δ֪ת������",//40
};

/* ���ֱ��е����� */
// �������ͣ�charvar, array
enum object {
    constant,
    variable,
    procedur,
    string,		// ���� 
    array		// ���� 
};

/* ��������� */
#define fctnum 8
enum fct {
    lit,     opr,     lod,
    sto,     cal,     inte,
    jmp,     jpc,
};

/* ���������ṹ */
struct instruction
{
    enum fct f; /* ���������ָ�� */
    int l;      /* ���ò���������Ĳ�β� */
    int a;      /* ����f�Ĳ�ͬ����ͬ */
};

FILE* fas;  /* ������ֱ� */
FILE* fa;   /* ������������ */
FILE* fa1;  /* ���Դ�ļ�������ж�Ӧ���׵�ַ */
FILE* fa2;  /* ������ */
bool listswitch;    /* ��ʾ������������ */
bool tableswitch;   /* ��ʾ���ֱ���� */
char ch;            /* ��ȡ�ַ��Ļ�������getch ʹ�� */
enum symbol sym;    /* ��ǰ�ķ��� */
char id[al+1];      /* ��ǰident, �����һ���ֽ����ڴ��0 */
float num;            /* ��ǰnumber ������֧�ָ����� */
int cc, ll;         /* getchʹ�õļ�������cc��ʾ��ǰ�ַ�(ch)��λ�� */
int cx;             /* ���������ָ��, ȡֵ��Χ[0, cxmax-1]*/
char line[81];      /* ��ȡ�л����� */
char a[al+1];       /* ��ʱ����, �����һ���ֽ����ڴ��0 */
struct instruction code[cxmax]; /* ����������������� */
char word[norw][al];        /* ������ */
enum symbol wsym[norw];     /* �����ֶ�Ӧ�ķ���ֵ */
enum symbol ssym[256];      /* ���ַ��ķ���ֵ */
char mnemonic[fctnum][5];   /* ���������ָ������ */
bool declbegsys[symnum];    /* ��ʾ������ʼ�ķ��ż��� */
bool statbegsys[symnum];    /* ��ʾ��俪ʼ�ķ��ż��� */
bool facbegsys[symnum];     /* ��ʾ���ӿ�ʼ�ķ��ż��� */
char format_str[strmax]; 	/* ��������ǰ�ı�׼������ַ��� */
char format_str_total[100][strmax]; 	/* ���������б�׼������ַ��� */
int format_str_size;        /* ��������׼������ַ��������� */
int cur_str_ptr;            /* ��������ǰ��׼������ַ������ַ�ָ�� */

/* ���ֱ�ṹ */
struct tablestruct
{
    char name[al];      /* ���� */
    enum object kind;   /* ���ͣ�const, var, procedure, array or string */
    float val;          /* ���������ͻ򸡵�����ֵ����constʹ�� */
    int level;          /* �����㣬��const��ʹ�� */
    int adr;            /* ��ַ����const��ʹ�� */
    int size;           /* ��Ҫ������������ռ�, ��procedureʹ�� */
    int type;			/* ��������ֵ���ͣ�������Ϊ 0��������Ϊ 1 */ 
};

struct tablestruct table[txmax]; /* ���ֱ� */

FILE* fin;
FILE* fout;
char fname[al];
int err; /* ��������� */

/* �������лᷢ��fatal errorʱ������-1��֪�������ĺ����������˳����� */
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
#define arraydeclarationdo(a, b, c)     if(-1 == arraydeclaration(a, b, c)) return -1		// ���� 

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
int arraydeclaration(int* ptx, int lev, int* pdx);	// ���� 
int position(char* idt, int tx);
void enter(enum object k, int* ptx, int lev, int* pdx);
int base(int l, float* s, int b);
