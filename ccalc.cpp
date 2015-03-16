#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "ccalc.h"

int to_int(int val) { return val; }
double to_float(double val) { return val; }

int prime(int n) { 
    if (n <= 3) { 
	return n;
    }
    n |= 1; 
    while (true) {
	int m = (int)sqrt((double)n) + 1;
	int k = 3;
	while (true) { 
	    if (k > m) { 
		return n;
	    }
	    if (n % k == 0) break;
	    k += 2;
	}
	n += 2;
    }
}

const char * help = "\
Input for this calculator are normal C expressions containing operators, float\n\
or integer constants, variables and and references to previous results ($n).\n\
Precedence and semantic of operators is the same as in C language. There are\n\
two extra binary operators: >>> unsigned shift right and ** raising to power.\n\
Calculator supports standard set of functions from C mathematics library and\n\
also defines function prime(n), which returns smallest prime number >= n.\n\
Operators:\n\
	++ -- ! ~ unary + -\n\
	**\n\
	* / %\n\
	+ -\n\
	<< >> >>>\n\
	< <= > >= \n\
	== != \n\
	&\n\
	^\n\
	|\n\
	= += -= *= /= %= <<= >>= >>>= &= |= ^= **= \n\
Functions:\n\
	abs	atan	cosh	float	prime	sqrt\n\
	acos	atan2	exp	log	sin	tan\n\
	asin 	cos	int	log10	sinh	tanh\n\
Type \"exit\" or \"quit\" to terminate program, \"help\" or \"?\" to show this help\n";

/*
int main(int argc, char* argv[]) 
{ 
    calculator ccalc;
    symbol* sp;

    symbol::add(FFUNC1, "abs", (void*)fabs);
    symbol::add(FFUNC1, "acos", (void*)acos);
    symbol::add(FFUNC1, "asin", (void*)asin);
    symbol::add(FFUNC1, "atan", (void*)atan);
    symbol::add(FFUNC2, "atan2", (void*)atan2);
    symbol::add(FFUNC1, "cos", (void*)cos);
    symbol::add(FFUNC1, "cosh", (void*)cosh);
    symbol::add(FFUNC1, "exp", (void*)exp);
    symbol::add(FFUNC1, "log", (void*)log);
    symbol::add(FFUNC1, "log10", (void*)log10);
    symbol::add(FFUNC1, "sin", (void*)sin);
    symbol::add(FFUNC1, "sinh", (void*)sinh);
    symbol::add(FFUNC1, "tan", (void*)tan);
    symbol::add(FFUNC1, "tanh", (void*)tanh);
    symbol::add(FFUNC1, "sqrt", (void*)sqrt);

    symbol::add(FFUNC1, "float", (void*)to_float);
    symbol::add(IFUNC1, "int", (void*)to_int);

    symbol::add(IFUNC1, "prime", (void*)prime);

    sp = symbol::add(VARIABLE, "pi");
    sp->val.tag = FLOAT;
    sp->val.fval = 3.1415926535897932385E0;
    sp = symbol::add(VARIABLE, "e");
    sp->val.tag = FLOAT;
    sp->val.fval = 2.7182818284590452354E0;

    if (argc > 1) 
	{
		for (int i = 1; i < argc; i++) 
		{ 
		    ccalc.evaluate(argv[i]);
		}
    } 
	else 
	{ 
		char buf[max_expression_length];
		printf("C expression command line calculator.  Author: http://www.ispras.ru/~knizhnik\nType \"help\" for more information\n\n");
		printf(prompt);
		while (fgets(buf, sizeof buf, stdin) != NULL) 
		{ 
	    	if (strncmp(buf, "help", 4) == 0 || strncmp(buf, "HELP", 4) == 0 || *buf == '?')
	    	{
				printf(help);
		    } 
			else 
			if  (strncmp(buf,"exit",4)==0 || strncmp(buf,"EXIT",4)==0 ||
		       		strncmp(buf,"quit",4)==0 || strncmp(buf,"QUIT",4)==0)
            {
				return 0;
	    	} 
			else 
			{ 	
				ccalc.evaluate(buf);
	    	}
		    printf(prompt);
		}
    }
    return 0;
}
*/

// -------------------------------------------------------------------
inline unsigned string_hash_function(char * p) 
{ 
    unsigned h = 0, g;
    while(*p) 
	{ 
		h = (h << 4) + *p++;
		if ((g = h & 0xF0000000) != 0) 
		{ 
		    h ^= g >> 24;
		}
		h &= ~g;
    }
    return h;
}

symbol* symbol::hash_table[hash_table_size];

// -------------------------------------------------------------------
symbol * 
symbol::add(t_symbol tag, char * name, void * func) 
{ 
    unsigned h = string_hash_function(name) % hash_table_size;
    symbol* sp;
    for (sp = hash_table[h]; sp != NULL; sp = sp->next) 
	{ 
		if (strcmp(sp->name, name) == 0) 
		{ 
	    	return sp;
		}
    }
    sp = new symbol;
    sp->tag = tag;
    sp->func = func;
    sp->name = strdup(name);
    sp->val.tag = INT;
    sp->val.ival = 0;
    sp->next = hash_table[h];
    hash_table[h] = sp;
    return sp;
}
// -------------------------------------------------------------------
void 
calculator::error(int pos, const char * msg) 
{ 
    pos += strlen(prompt);
    if (pos < 80) 
	{ 
		while (--pos >= 0) putc('.', stderr);
		fprintf(stderr, "^\n");
    }
    fprintf(stderr, "Error: %s\n\n", msg);
}
// -------------------------------------------------------------------
t_operator 
calculator::scan(bool operand)
{
    char name[max_expression_length], *np;

    while (isspace(buf[pos])) pos += 1;
    switch (buf[pos++]) 
	{ 
    	case '\0':
			return END;
      	
		case '(':
			return LPAR;
      	
		case ')':
			return RPAR;
		
		case '+':
			if (buf[pos] == '+') 
			{ 
	    		pos += 1;
	    		return operand ? PREINC : POSTINC;
			} 
			else 
			if (buf[pos] == '=') 
			{ 
	    		pos += 1;
	    		return SETADD;
			}
			return operand ? PLUS : ADD;
      
	  	case '-':
			if (buf[pos] == '-') 
			{ 
	    		pos += 1;
	    		return operand ? PREDEC : POSTDEC;
			} 
			else 
			if (buf[pos] == '=') 
			{ 
			    pos += 1;
	    		return SETSUB;
			}
			return operand ? MINUS : SUB;

		case '!':
			if (buf[pos] == '=') 
			{ 
	    		pos += 1;
	    		return NE;
			}
			return NOT;

		case '~':
			return COM;
      		
		case '*':
			if (buf[pos] == '*') 
			{ 
	    		if (buf[pos+1] == '=') 
				{ 
					pos += 2;
					return SETPOW;
	    		}
	    		pos += 1;
	    		return POW;
			} 
			else 
			if (buf[pos] == '=') 
			{ 
	    		pos += 1;
	    		return SETMUL;
			} 
			return MUL;

		case '/':
			if (buf[pos] == '=') 
			{ 
	    		pos += 1;
	    		return SETDIV;
			}
			return DIV;

		case '%':
			if (buf[pos] == '=') 
			{ 
			    pos += 1;
			    return SETMOD;
			}
			return MOD;

		case '<':
			if (buf[pos] == '<') 
			{ 
				if (buf[pos+1] == '=') 
				{ 
					pos += 2;
					return SETASL;
				} 
				else 
				{ 
					pos += 1;
					return ASL;
			    }
			} 
			else 
			if (buf[pos] == '=') 
			{ 
			    pos += 1;
	    		return LE;
			} 
			return LT;

		case '>':
			if (buf[pos] == '>') 
			{ 
	    		if (buf[pos+1] == '>') 
				{ 
					if (buf[pos+2] == '=') 
					{ 
					    pos += 3;
		    			return SETLSR;
					}
					pos += 2;
					return LSR;
	    		} 
				else 
				if (buf[pos+1] == '=') 
				{ 
					pos += 2;
					return SETASR;
	    		} 
				else 
				{ 
					pos += 1;
					return ASR;
	    		}
			} 
			else 
			if (buf[pos] == '=') 
			{ 
			    pos += 1;
			    return GE;
			} 
			return GT;
      
		case '=':
			if (buf[pos] == '=') 
			{ 
	    		pos += 1;
	    		return EQ;
			} 
			return SET;

		case '&':
			if (buf[pos] == '&') 
			{ 
			    pos += 1;
	    		return AND;
			} 
			else 
			if (buf[pos] == '=') 
			{ 
			    pos += 1;
	    		return SETAND;
			}
			return AND;

		case '|':
			if (buf[pos] == '|') 
			{ 
			    pos += 1;
	    		return OR;
			} 
			else 
			if (buf[pos] == '=') 
			{ 
			    pos += 1;
	    		return SETOR;
			}
			return OR;

		case '^':
			if (buf[pos] == '=') 
			{ 
	    		pos += 1;
	    		return SETXOR;
			}
			return XOR;

		case ',':
			return COMMA;

		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
		{
	    	int ival, ipos = 0, fpos = 0;
		    double fval;
	    
		    sscanf(buf+pos-1, "%i%n", &ival, &ipos);
		    sscanf(buf+pos-1, "%lf%n", &fval, &fpos);
		    if (ipos == 0 && fpos == 0)
		    {
				error("bad numeric constant");
				return ERROR;
		    }
		
		    if (v_sp == max_stack_size) 
			{ 
				error("stack overflow");
				return ERROR;
		    }
	
		    if (ipos >= fpos) 
			{ 
				v_stack[v_sp].tag = INT;
				v_stack[v_sp].ival = ival;
				pos += ipos - 1;
		    } 
			else 
			{ 
				v_stack[v_sp].tag = FLOAT;
				v_stack[v_sp].fval = fval;
				pos += fpos - 1;
		    } 
		    v_stack[v_sp].pos = pos;
		    v_stack[v_sp++].var = NULL;
		    return OPERAND;
		}

		default:
			pos -= 1;
			np = name;
			while (isalnum(buf[pos]) || buf[pos] == '$' || buf[pos] == '_') 
			{
			    *np++ = buf[pos++];
			}

			if (np == buf) 
			{ 
			    error("Bad character");
	    		return ERROR;
			}
			*np = '\0';
			symbol* sym = symbol::add(VARIABLE, name);
			
			if (v_sp == max_stack_size) 
			{ 
			    error("stack overflow");
			    return ERROR;
			}
		
			v_stack[v_sp] = sym->val;
			v_stack[v_sp].pos = pos;
			v_stack[v_sp++].var = sym;
			return (sym->tag == VARIABLE) ? OPERAND : FUNC;
	}
}

// -------------------------------------------------------------------
static int lpr[TERMINALS] = 
{
    2, 0, 0, 0,       // BEGIN, OPERAND, ERROR, END, 
    4, 4,             // LPAR, RPAR 
    5, 98, 98,        // FUNC, POSTINC, POSTDEC,
    98, 98, 98, 98, 98, 98, // PREINC, PREDEC, PLUS, MINUS, NOT, COM,
    90,               // POW,
    80, 80, 80,       // MUL, DIV, MOD,
    70, 70,           // ADD, SUB, 
    60, 60, 60,       // ASL, ASR, LSR, 
    50, 50, 50, 50,   // GT, GE, LT, LE,     
    40, 40,           // EQ, NE, 
    38,               // AND,
    36,               // XOR,
    34,               // OR,
    20, 20, 20, 20, 20, 20, 20, //SET, SETADD, SETSUB, SETMUL, SETDIV, SETMOD, 
    20, 20, 20, 20, 20, 20, // SETASL, SETASR, SETLSR, SETAND, SETXOR, SETOR,
    10               // COMMA
};
// -------------------------------------------------------------------
static int rpr[TERMINALS] = 
{
    0, 0, 0, 1,       // BEGIN, OPERAND, ERROR, END, 
    110, 3,           // LPAR, RPAR 
    120, 99, 99,      // FUNC, POSTINC, POSTDEC
    99, 99, 99, 99, 99, 99, // PREINC, PREDEC, PLUS, MINUS, NOT, COM,
    95,               // POW,
    80, 80, 80,       // MUL, DIV, MOD,
    70, 70,           // ADD, SUB, 
    60, 60, 60,       // ASL, ASR, LSR, 
    50, 50, 50, 50,   // GT, GE, LT, LE,     
    40, 40,           // EQ, NE, 
    38,               // AND,
    36,               // XOR,
    34,               // OR,
    25, 25, 25, 25, 25, 25, 25, //SET, SETADD, SETSUB, SETMUL, SETDIV, SETMOD, 
    25, 25, 25, 25, 25, 25, // SETASL, SETASR, SETLSR, SETAND, SETXOR, SETOR,
    15               // COMMA
};
// -------------------------------------------------------------------
bool calculator::assign() 
{ 
    value& v = v_stack[v_sp-1];
    if (v.var == NULL) 
	{ 
		error(v.pos, "variable expected");
		return false;
    } 
	else 
	{ 
		v.var->val = v;
		return true;
    }
}
// -------------------------------------------------------------------
void 
calculator::evaluate(char * expression) 
{ 
    char var_name[16];
    buf = expression;
    v_sp = 0;
    o_sp = 0;
    pos = 0;
    o_stack[o_sp++] = BEGIN;
    bool operand = true;
    int n_args = 0;

    while (true) 
	{ 
    	next_token:
		int oper = scan(operand);
		if (oper == ERROR) 
		{
	    	return;
		}

		if (!operand) 
		{ 
	    	if (!BINARY(oper) && oper != END && oper != POSTINC 
				&& oper != POSTDEC && oper != RPAR) 
	    	{ 
				error("operator expected");
				return;
	    	}

		    if (oper != POSTINC && oper != POSTDEC && oper != RPAR) 
			{ 
				operand = true;
		    }
		} 
		else 
		{ 
	    	if (oper == OPERAND) 
			{ 
				operand = false;
				n_args += 1;
				continue;
	    	}
			
	    	if (BINARY(oper) || oper == RPAR) 
			{
				error("operand expected");
				return;
	    	}
		}

		int n_args = 1;
		while (lpr[o_stack[o_sp-1]] >= rpr[oper]) 
		{ 
	    	int cop = o_stack[--o_sp]; 
		    switch (cop) 
			{ 
				case BEGIN:
					if (oper == RPAR) 
					{ 
		    			error("Unmatched ')'");
		    			return;
					}

					if (oper != END) 
					{ 
		    			error("Unexpected end of input");
					}

					if (v_sp == 1) 
					{
					    sprintf(var_name, "$%d", ++tmp_var_count);
					    printf("%s = ", var_name);
					    symbol::add(VARIABLE, var_name)->val = v_stack[0];
					    if (v_stack[0].tag == INT) 
						{ 
							printf("%d [%#x %#o]\n", v_stack[0].ival,
							       v_stack[0].ival, v_stack[0].ival);
		    			} 
						else 
						{ 
							printf("%.10g\n", v_stack[0].fval);
					    }
					} 
					else 
					if (v_sp != 0) 
					{ 
					    error("Unexpected end of expression");
					}
					return;

				case COMMA:
					n_args += 1;
					continue;

				case ADD:
			    case SETADD:

					if (v_stack[v_sp-1].tag == INT && v_stack[v_sp-2].tag == INT) 
					{
				    	v_stack[v_sp-2].ival += v_stack[v_sp-1].ival;
					} 
					else 
					{ 
				    	v_stack[v_sp-2].fval = 
							v_stack[v_sp-2].get() + v_stack[v_sp-1].get();
					    v_stack[v_sp-2].tag = FLOAT;
					}
					v_sp -= 1;
		
					if (cop == SETADD) 
					{ 
				    	if (!assign()) return;
					}

					v_stack[v_sp-1].var = NULL;
				break;

				case SUB:
				case SETSUB:
					if (v_stack[v_sp-1].tag == INT && v_stack[v_sp-2].tag == INT) 
					{
					    v_stack[v_sp-2].ival -= v_stack[v_sp-1].ival;
					} 
					else 
					{ 
					    v_stack[v_sp-2].fval = 
							v_stack[v_sp-2].get() - v_stack[v_sp-1].get();
					    v_stack[v_sp-2].tag = FLOAT;
					}
					v_sp -= 1;
					if (cop == SETSUB) 
					{ 
					    if (!assign()) return;
					}
					v_stack[v_sp-1].var = NULL;
				break;

				case MUL:
				case SETMUL:
					if (v_stack[v_sp-1].tag == INT && v_stack[v_sp-2].tag == INT) 
					{
					    v_stack[v_sp-2].ival *= v_stack[v_sp-1].ival;
					} 
					else 
					{ 
					    v_stack[v_sp-2].fval = 
						v_stack[v_sp-2].get() * v_stack[v_sp-1].get();
					    v_stack[v_sp-2].tag = FLOAT;
					}
					v_sp -= 1;
			
					if (cop == SETMUL) 
					{ 
					    if (!assign()) return;
					}
					v_stack[v_sp-1].var = NULL;
				break;
	    
				case DIV:
				case SETDIV:
					if (v_stack[v_sp-1].get() == 0.0) 
					{
					    error(v_stack[v_sp-2].pos, "Division by zero");
		    			return;
					}

					if (v_stack[v_sp-1].tag == INT && v_stack[v_sp-2].tag == INT) 
					{
					    v_stack[v_sp-2].ival /= v_stack[v_sp-1].ival;
					} 
					else 
					{ 
					    v_stack[v_sp-2].fval = 
							v_stack[v_sp-2].get() / v_stack[v_sp-1].get();
					    v_stack[v_sp-2].tag = FLOAT;
					}
					v_sp -= 1;
			
					if (cop == SETDIV) 
					{ 
					    if (!assign()) return;
					}
					v_stack[v_sp-1].var = NULL;
				break;

				case MOD:
		      	case SETMOD:
					if (v_stack[v_sp-1].get() == 0.0) 
					{
		    			error(v_stack[v_sp-2].pos, "Division by zero");
		    			return;
					}
					
					if (v_stack[v_sp-1].tag == INT && v_stack[v_sp-2].tag == INT) 
					{
		    			v_stack[v_sp-2].ival %= v_stack[v_sp-1].ival;
					} 
					else 
					{ 
					    v_stack[v_sp-2].fval = 
						fmod(v_stack[v_sp-2].get(), v_stack[v_sp-1].get());
		    			v_stack[v_sp-2].tag = FLOAT;
					}
					v_sp -= 1;
					
					if (cop == SETMOD) 
					{ 
					    if (!assign()) return;
					}
					v_stack[v_sp-1].var = NULL;
				break;
	    
				case POW:
	      		case SETPOW:
					if (v_stack[v_sp-1].tag == INT && v_stack[v_sp-2].tag == INT) 
					{
		    			v_stack[v_sp-2].ival = 
							(int)pow((double)v_stack[v_sp-2].ival, 
				 				(double)v_stack[v_sp-1].ival);
					} 
					else 
					{ 
					    v_stack[v_sp-2].fval = 
							pow(v_stack[v_sp-2].get(), v_stack[v_sp-1].get());
					    v_stack[v_sp-2].tag = FLOAT;
					}
					v_sp -= 1;
			
					if (cop == SETPOW) 
					{ 
					    if (!assign()) return;
					}
					v_stack[v_sp-1].var = NULL;
				break;

				case AND:
				case SETAND:
					if (v_stack[v_sp-1].tag == INT && v_stack[v_sp-2].tag == INT) 
					{
					    v_stack[v_sp-2].ival &= v_stack[v_sp-1].ival;
					} 
					else 
					{ 
					    v_stack[v_sp-2].ival = 
							v_stack[v_sp-2].get_int() & v_stack[v_sp-1].get_int();
					    v_stack[v_sp-2].tag = INT;
					}
					v_sp -= 1;
					if (cop == SETAND) 
					{ 
					    if (!assign()) return;
					}
					v_stack[v_sp-1].var = NULL;
				break;

				case OR:
				case SETOR:
					if (v_stack[v_sp-1].tag == INT && v_stack[v_sp-2].tag == INT) 
					{
		    			v_stack[v_sp-2].ival |= v_stack[v_sp-1].ival;
					} 
					else 
					{ 
		    			v_stack[v_sp-2].ival = 
							v_stack[v_sp-2].get_int() | v_stack[v_sp-1].get_int();
		    			v_stack[v_sp-2].tag = INT;
					}
					v_sp -= 1;
					if (cop == SETOR) 
					{ 
		    			if (!assign()) return;
					}
					v_stack[v_sp-1].var = NULL;
				break;
	    
		  		case XOR:
	      		case SETXOR:
					if (v_stack[v_sp-1].tag == INT && v_stack[v_sp-2].tag == INT) 
					{
		    			v_stack[v_sp-2].ival ^= v_stack[v_sp-1].ival;
					} 
					else 
					{ 
		    			v_stack[v_sp-2].ival = 
							v_stack[v_sp-2].get_int() ^ v_stack[v_sp-1].get_int();
					    v_stack[v_sp-2].tag = INT;
					}
					v_sp -= 1;
					if (cop == SETXOR) 
					{ 
		    			if (!assign()) return;
					}
					v_stack[v_sp-1].var = NULL;	
				break;
				
			    case ASL:
	      		case SETASL:
					if (v_stack[v_sp-1].tag == INT && v_stack[v_sp-2].tag == INT) 
					{
		    			v_stack[v_sp-2].ival <<= v_stack[v_sp-1].ival;
					} 
					else 
					{ 
		    			v_stack[v_sp-2].ival = 
						v_stack[v_sp-2].get_int() << v_stack[v_sp-1].get_int();
		    			v_stack[v_sp-2].tag = INT;
					}
					v_sp -= 1;
					if (cop == SETASL) 
					{ 
					    if (!assign()) return;
					}
					v_stack[v_sp-1].var = NULL;
				break;

	      		case ASR:
	      		case SETASR:
					if (v_stack[v_sp-1].tag == INT && v_stack[v_sp-2].tag == INT) 
					{
		    			v_stack[v_sp-2].ival >>= v_stack[v_sp-1].ival;
					} 
					else 
					{ 
		    			v_stack[v_sp-2].ival = 
							v_stack[v_sp-2].get_int() >> v_stack[v_sp-1].get_int();
		    			v_stack[v_sp-2].tag = INT;
					}
					v_sp -= 1;
					if (cop == SETASR) 
					{ 
		    			if (!assign()) return;
					}
					v_stack[v_sp-1].var = NULL;
				break;

	      		case LSR:
	      		case SETLSR:
					if (v_stack[v_sp-1].tag == INT && v_stack[v_sp-2].tag == INT) 
					{
		    			v_stack[v_sp-2].ival = 
							(unsigned)v_stack[v_sp-2].ival >> v_stack[v_sp-1].ival;
					} 
					else 
					{ 
		    			v_stack[v_sp-2].ival = (unsigned)v_stack[v_sp-2].get_int()
							>> v_stack[v_sp-1].get_int();
		    			v_stack[v_sp-2].tag = INT;
					}
					v_sp -= 1;
					if (cop == SETLSR) 
					{ 
		    			if (!assign()) return;
					}
					v_stack[v_sp-1].var = NULL;
				break;

				case EQ:
					if (v_stack[v_sp-1].tag == INT && v_stack[v_sp-2].tag == INT) 
					{
		    			v_stack[v_sp-2].ival = 
							v_stack[v_sp-2].ival == v_stack[v_sp-1].ival;
					} 
					else 
					{ 
					    v_stack[v_sp-2].ival = 
							v_stack[v_sp-2].get() == v_stack[v_sp-1].get();
					    v_stack[v_sp-2].tag = INT;
					}
					v_sp -= 1;
					v_stack[v_sp-1].var = NULL;
				break;

				case NE:
					if (v_stack[v_sp-1].tag == INT && v_stack[v_sp-2].tag == INT) 
					{
		    			v_stack[v_sp-2].ival = 
							v_stack[v_sp-2].ival != v_stack[v_sp-1].ival;
					} 
					else 
					{ 
					    v_stack[v_sp-2].ival = 
							v_stack[v_sp-2].get() != v_stack[v_sp-1].get();
					    v_stack[v_sp-2].tag = INT;
					}
					v_sp -= 1;
					v_stack[v_sp-1].var = NULL;
				break;		
	    
				case GT:
					if (v_stack[v_sp-1].tag == INT && v_stack[v_sp-2].tag == INT) 
					{
					    v_stack[v_sp-2].ival = 
							v_stack[v_sp-2].ival > v_stack[v_sp-1].ival;
					} 
					else 
					{ 
					    v_stack[v_sp-2].ival = 
							v_stack[v_sp-2].get() > v_stack[v_sp-1].get();
					    v_stack[v_sp-2].tag = INT;
					}
					v_sp -= 1;
					v_stack[v_sp-1].var = NULL;
				break;		
	    
				case GE:
					if (v_stack[v_sp-1].tag == INT && v_stack[v_sp-2].tag == INT) 
					{
					    v_stack[v_sp-2].ival = 
							v_stack[v_sp-2].ival >= v_stack[v_sp-1].ival;
					} 
					else 
					{ 
					    v_stack[v_sp-2].ival = 
							v_stack[v_sp-2].get() >= v_stack[v_sp-1].get();
					    v_stack[v_sp-2].tag = INT;
					}
					v_sp -= 1;
					v_stack[v_sp-1].var = NULL;
				break;
				
				case LT:
					if (v_stack[v_sp-1].tag == INT && v_stack[v_sp-2].tag == INT) 
					{
					    v_stack[v_sp-2].ival = 
							v_stack[v_sp-2].ival < v_stack[v_sp-1].ival;
					} 
					else 
					{ 
					    v_stack[v_sp-2].ival = 
							v_stack[v_sp-2].get() < v_stack[v_sp-1].get();
					    v_stack[v_sp-2].tag = INT;
					}
					v_sp -= 1;
					v_stack[v_sp-1].var = NULL;
				break;		

				case LE:
					if (v_stack[v_sp-1].tag == INT && v_stack[v_sp-2].tag == INT) 
					{
					    v_stack[v_sp-2].ival = 
							v_stack[v_sp-2].ival <= v_stack[v_sp-1].ival;
					} 
					else 
					{ 
					    v_stack[v_sp-2].ival = 
							v_stack[v_sp-2].get() <= v_stack[v_sp-1].get();
					    v_stack[v_sp-2].tag = INT;
					}
					v_sp -= 1;
					v_stack[v_sp-1].var = NULL;
				break;		

				case PREINC:
					if (v_stack[v_sp-1].tag == INT) 
					{ 
					    v_stack[v_sp-1].ival += 1;
					} 
					else 
					{ 
					    v_stack[v_sp-1].fval += 1;
					} 
					if (!assign()) return;
					v_stack[v_sp-1].var = NULL;
				break;
				
				case PREDEC:
					if (v_stack[v_sp-1].tag == INT) 
					{ 
					    v_stack[v_sp-1].ival -= 1;
					} 
					else 
					{ 
					    v_stack[v_sp-1].fval -= 1;
					} 
					if (!assign()) return;
					v_stack[v_sp-1].var = NULL;
				break;
				
				case POSTINC:
					if (v_stack[v_sp-1].var == NULL) 
					{ 
			    		error(v_stack[v_sp-1].pos, "Varaibale expected");
			    		return;
					} 
					if (v_stack[v_sp-1].var->val.tag == INT) 
					{ 
						v_stack[v_sp-1].var->val.ival += 1;
					} 
					else 
					{ 
					    v_stack[v_sp-1].var->val.fval += 1;
					} 
					v_stack[v_sp-1].var = NULL;
				break;

				case POSTDEC:
					if (v_stack[v_sp-1].var == NULL) 
					{ 
					    error(v_stack[v_sp-1].pos, "Varaibale expected");	
					    return;
					} 

					if (v_stack[v_sp-1].var->val.tag == INT) 
					{ 
					    v_stack[v_sp-1].var->val.ival -= 1;
					} 
					else 
					{ 
					    v_stack[v_sp-1].var->val.fval -= 1;
					} 
					v_stack[v_sp-1].var = NULL;
				break;

				case SET:
					if (v_stack[v_sp-2].var == NULL) 
					{ 
					    error(v_stack[v_sp-2].pos, "Variabale expected");
		    			return;
					} 
					else 
					{ 
					    v_stack[v_sp-2]=v_stack[v_sp-2].var->val=v_stack[v_sp-1];
					}		    
					v_sp -= 1;
					v_stack[v_sp-1].var = NULL;
				break;
				
				case NOT:
					if (v_stack[v_sp-1].tag == INT) 
					{ 
					    v_stack[v_sp-1].ival = !v_stack[v_sp-1].ival;
					} 
					else 
					{ 
					    v_stack[v_sp-1].ival = !v_stack[v_sp-1].fval;
		    			v_stack[v_sp-1].tag = INT;
					}
					v_stack[v_sp-1].var = NULL;
				break;
	    		
				case MINUS:
					if (v_stack[v_sp-1].tag == INT) 
					{ 
					    v_stack[v_sp-1].ival = -v_stack[v_sp-1].ival;
					} 
					else 
					{ 
					    v_stack[v_sp-1].fval = -v_stack[v_sp-1].fval;
					}
					// no break

				case PLUS:
					v_stack[v_sp-1].var = NULL;
				break;
	    
				case COM:
					if (v_stack[v_sp-1].tag == INT) 
					{ 
					    v_stack[v_sp-1].ival = ~v_stack[v_sp-1].ival;
					} 
					else 
					{ 
					    v_stack[v_sp-1].ival = ~(int)v_stack[v_sp-1].fval;
		    			v_stack[v_sp-1].tag = INT;
					}
					v_stack[v_sp-1].var = NULL;
				break;		
	    
				case RPAR:
					error("mismatched ')'");
					return;

				case FUNC:
					error("'(' expected");
				return;
	    
				case LPAR:
					if (oper != RPAR) 
					{ 
					    error("')' expected");
		    			return;
					}

					if (o_stack[o_sp-1] == FUNC) 
					{ 
					    symbol* sym = v_stack[v_sp-n_args-1].var;
					    if (sym->tag == IFUNC1) 
						{ 
							if (n_args != 1) 
							{ 
			    				error(v_stack[v_sp-n_args-1].pos, 
				  					"Function should take one argument");
			    				return;
							}
							v_stack[v_sp-2].ival = 
			  					(*(int(*)(int))sym->func)(v_stack[v_sp-1].get_int());
							v_stack[v_sp-2].tag = INT;
							v_sp -= 1;
		    			} 
						else 
						if (sym->tag == FFUNC1) 
						{ 
							if (n_args != 1) 
							{ 
							    error(v_stack[v_sp-n_args-1].pos, 
								  "Function should take one argument");
			    				return;
							}
							v_stack[v_sp-2].fval = 
							  (*(double(*)(double))sym->func)(v_stack[v_sp-1].get());
							v_stack[v_sp-2].tag = FLOAT;
							v_sp -= 1;
		    			} 
						else 
						if (sym->tag == FFUNC2) 
						{ 
							if (n_args != 2) 
							{ 
			    				error(v_stack[v_sp-n_args-1].pos, 
				  					"Function should take two arguments");
			    				return;
							}
							v_stack[v_sp-3].fval = 
							    (*(double(*)(double, double))sym->func)
			    			(v_stack[v_sp-2].get(), v_stack[v_sp-1].get());
							v_stack[v_sp-3].tag = FLOAT;
							v_sp -= 2;
					    } 
						else 
						{ 
							error("Invalid expression");
					    }
		    			v_stack[v_sp-1].var = NULL; 
					    o_sp -= 1;
		    			n_args = 1;
						} 
						else 
						if (n_args != 1) 
						{ 
						    error("Function call expected");
						    return;
						}
						goto next_token;
				
				default:
					error("synctax error");
	    	}
		}

		if (o_sp == max_stack_size) 
		{ 
	    	error("operator stack overflow");
	    	return;
		}
		o_stack[o_sp++] = oper;
    }
}

// -------------------------------------------------------------------
