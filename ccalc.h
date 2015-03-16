#ifndef ccalc_h
#define ccalc_h

class symbol;


enum t_value 
{ 
    INT, 
    FLOAT
};

class value 
{ 
public:
    t_value   tag;
    symbol *  var;
    int       pos;
    
	union 
	{ 
		int    ival;
		double fval;
    };
    
    double get() 
	{ 
		return tag == INT ? (double)ival : fval;
    }
    
	int get_int() 
	{ 
		return tag == INT ? ival : (int)fval;
    }
};

	
enum t_operator 
{ 
    BEGIN, 
	OPERAND, 
	ERROR, 
	END, 
    LPAR, 
	RPAR, 
	FUNC, 
	POSTINC, 
	POSTDEC,
    PREINC, 
	PREDEC, 
	PLUS, 
	MINUS, 
	NOT, 
	COM,
    POW,
    MUL, 
	DIV, 
	MOD,
    ADD, 
	SUB, 
    ASL, 
	ASR, 
	LSR, 
    GT, 
	GE, 
	LT, 
	LE,     
    EQ, 
	NE, 
    AND,
    XOR,
    OR,
    SET, 
	SETADD, 
	SETSUB, 
	SETMUL, 
	SETDIV, 
	SETMOD, 
	SETASL, 
	SETASR, 
	SETLSR, 
    SETAND, 
	SETXOR, 
	SETOR, 
	SETPOW,
    COMMA,
    TERMINALS
};

#define BINARY(opd) (opd >= POW)

enum t_symbol 
{ 
    VARIABLE, 
    IFUNC1, 
    FFUNC1, 
    FFUNC2 
};

const char * prompt = "ccalc> ";

const int max_stack_size = 256;
const int max_expression_length = 1024;

const int hash_table_size = 1013;

class symbol 
{ 
public:
    t_symbol tag;
    void*    func;
    value    val;
    char*    name;
    symbol*  next;

    static symbol* hash_table[hash_table_size];

    static symbol* add(t_symbol tag, char* name, void* func = NULL);
};
	       
class calculator 
{ 
public:
    value v_stack[max_stack_size];
    int   v_sp;
    int   o_stack[max_stack_size];
    int   o_sp;
    char* buf;
    int   pos;
    int   tmp_var_count;
    
    void      evaluate(char * expr);
    t_operator scan(bool operand);
    void      error(int pos, const char * msg);
    void      error(const char * msg) { error(pos-1, msg); }
    bool      assign();

    calculator() { tmp_var_count = 0; }
};

#endif
