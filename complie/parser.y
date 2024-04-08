%require "3.0.4"

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "exp_tree.c"
void yyerror(char *s, ...);
int yylex();
extern FILE* yyout;


int i;
int label;
LinkList s;
%}

%debug

%locations

%union {
    char chval;
    int intval;
    char *strval;
    int subtok;
    struct decl decl;
    struct str_int_val{
	char* name;
	int val;
   }str_int_val;
    Node* node;
}
%token <intval>IntConst
%token <strval>Ident
%token COMMA
%token LSBK
%token RSBK
%token LRBK
%token RRBK
%token SMC
%token LCB
%token RCB
%token VOID
%token INT
%token BREAK
%token CONST
%token CONTINUE
%token ELSE
%token WHILE
%token IF
%token RETURN
%token <strval> STRING
%left NEQ
%left EQ
%left NL
%left NG
%left OR
%left AND
%left ASSIGN
%left '!'
%left '+' '-'
%left '*' '/' '%'
%left GT
%left LS
%start CompUnit
%type <intval> InitVal Number M ConstInitVal
%type <decl> Decl ConstDecl ConstDef VarDecl VarDef
%type <strval> LVal
%type <node> Exp UnaryExp MulExp AddExp PrimaryExp RelExp EqExp	LAndExp LOrExp	ConstExp
%type <chval>	UnaryOp 

%%
CompUnit	:CompUnit Decl	{
	 				set_g_symt($2);
					cl_t_buffer();
				}
			|CompUnit FuncDef
			|Decl	{
				set_g_symt($1);
				cl_t_buffer();

				}
			|FuncDef
			;
Decl		:ConstDecl		{
      				$$ = $1;
					}
			|VarDecl	{
						$$ = $1;
					}
			;
ConstDecl	:CONST BType ConstDef ConstDefs SMC	{decl_assign(&$$, $3);}
			;
BType		:INT
		;
ConstDefs	:
			|ConstDefs COMMA ConstDef	
			|COMMA ConstDef
			;
ConstDef	:Ident ConstExps ASSIGN ConstInitVal	
		{
			set_decl_ci(&$$,$1, $4);		
		}
		;
ConstExps	:
		|ConstExps LSBK ConstExp RSBK	
		|LSBK ConstExp RSBK
		;
ConstInitVal:		ConstExp				
							{
								store_I($1);
								if (isDigit(t_buffer[0]))
								{
									$$ = atoi(t_buffer[0]);
								}
							}
			|LCB ConstInitVal ConstInitVals RCB
			;
ConstInitVals:
			|ConstInitVals COMMA ConstInitVal
			;
VarDecl		:BType	VarDef VarDefs SMC	
				{
					decl_assign(&$$, $2);
				}
			;
VarDef		:Ident ConstExps	{
				set_decl_ci(&$$, $1, 0);
				}
			|Ident ConstExps ASSIGN InitVal
				{
				set_decl_ci(&$$,$1, $4);
				}
			;
VarDefs		:
			|VarDefs COMMA VarDef
			|COMMA VarDef
			;
InitVal		:Exp	{
				store_I($1);
				if (isDigit(t_buffer[0]))
				{
					$$ = atoi(t_buffer[0]);
				}
				else
				{
					$$ = 0;
				}

				}
			|LCB InitVals RCB
			;
InitVals	:
			|InitVals COMMA InitVal
			|COMMA InitVal
			;
FuncDef		:BType Ident LRBK RRBK Block
	 {
		snprintf(buffer, sizeof(buffer), "%s:\n", $2);
		fputs(buffer, yyout);
                snprintf(buffer, sizeof(buffer), "ent #%d\n", s_counter);
		size[symt_position] = s_counter;		
                fputs(buffer, yyout);
		set_symt($2);
		enQueue("ret \n");
		while (!q_isEmpty())
		{
			fputs(deQueue(), yyout);
		}
		cl_queue();

		}
		|BType Ident LRBK FuncFParams RRBK Block

		 {
			snprintf(buffer, sizeof(buffer), "%s:\n", $2);
			fputs(buffer, yyout);
		        snprintf(buffer, sizeof(buffer), "ent #%d\n", s_counter);
			size[symt_position] = s_counter;		
		        fputs(buffer, yyout);
			set_symt($2);
			enQueue("ret\n");
			while (!q_isEmpty())
			{
				fputs(deQueue(), yyout);
			}
			cl_queue();		
		}
		|VOID Ident LRBK RRBK Block

	{	
		snprintf(buffer, sizeof(buffer), "%s:\n", $2);
		fputs(buffer, yyout);
	        snprintf(buffer, sizeof(buffer), "ent #%d\n", s_counter);
		size[symt_position] = s_counter;		
	        fputs(buffer, yyout);
		set_symt($2);
		
		while (!q_isEmpty())
		{
			fputs(deQueue(), yyout);
		}
		cl_queue();
	}
		|VOID Ident LRBK FuncFParams RRBK Block
	{
		snprintf(buffer, sizeof(buffer), "%s:\n", $2);
		fputs(buffer, yyout);
	        snprintf(buffer, sizeof(buffer), "ent #%d\n", s_counter);
		size[symt_position] = s_counter;		
	        fputs(buffer, yyout);
		set_symt($2);
		
		while (!q_isEmpty())
		{
			fputs(deQueue(), yyout);
		}
		cl_queue();
	}

		;
FuncFParam	:BType Ident
		|VOID Ident LRBK FuncFParams RRBK Block

		;
FuncFParam	:BType Ident

		;
FuncFParam	:BType Ident
			|BType Ident LSBK RSBK Exps
			;
Exps		:
		|Exps LSBK Exp RSBK
            	|LSBK Exp RSBK
			;
FuncFParams	:FuncFParam FParams
			;
FParams		:
			|FParams COMMA FuncFParam
			|COMMA FuncFParam
			;
Block		:LCB BlockItems RCB
			;
BlockItem	:Decl	{

	  		symt[symt_position].syms[s_counter].val = $1.val;
			symt[symt_position].syms[s_counter].name = strdup($1.name);
			s_counter++;
			if (t_buffer[0] == NULL)
			{
				t_buffer[0] = strdup("#0");
			}
			if (strlen(t_buffer[0]) <= 0)
			{
				strcpy(t_buffer[0], "#0");
			}
			addNS(t_buffer[0]);
			snprintf(buffer, sizeof(buffer), "init %s,%s\n", $1.name, t_buffer[0]);
			enQueue(buffer);
			strcpy(t_buffer[0], "0");
			}
			|Stmt
			;
BlockItems	:
	  	|BlockItems BlockItem
		;
Stmt		:LVal ASSIGN Exp SMC	{
      	         store_I($3);
		addNS(t_buffer[0]);
		snprintf(buffer, sizeof(buffer), "mov %s,%s\n", $1, t_buffer[0]);
		enQueue(buffer);
		t_buffer_counter = 0;
		strcpy(t_buffer[0], "0");
			}

			|Exp SMC	{}
			|SMC		{}
			|Block		{}
			|N IF LRBK Cond M RRBK M Stmt M M
{
    /*
        当整个产生式规约完成时完成以下的语义动作:
        1.获得需要回填真出口的中间代码的位置
        2.获得真出口的标签
        3.入队

        1.删掉begin_list的元素
        2.删掉begin_entry的元素


        1.获得此产生式代码块后面的代码的标签(if这里没有用,只是为了统一if ifelse while)
        2.仅调用delete_L删除,不入队

        1.获得需要回填假出口的中间代码的位置
        2.获得假出口的标签
        3.入队

        1.最后将M_list的头结点出栈恢复上一次的times
    */	
		
	
	
	i = delete_L(true_list);
	label = delete_L(false_entry);
		
	snprintf(buffers[i], sizeof(buffers[i]), "beq .LC%d\n",label);

    	i = delete_L(begin_list);
	label = delete_L(begin_entry);


	i = delete_L(false_list);
	label = delete_L(next_entry);
	snprintf(buffers[i], sizeof(buffers[i]), "b .LC%d\n",label);

	times = delete_L(M_list);
}
			|N IF LRBK Cond M RRBK M Stmt M ELSE Stmt M
{
    /*
            当整个产生式规约完成时完成以下的语义动作:
            1.获得需要回填真出口的中间代码的位置
            2.获得真出口的标签
            3.入队

            1.删掉begin_list的元素
            2.删掉begin_entry的元素

            1.获得真出口的代码执行完后跳过else代码块的标签
            2.入队(b .LC 在规约M2的时候已经入队)

            1.获得需要回填假出口的中间代码的位置
            2.获得假出口的标签
            3.入队

            1.最后将M_list的头结点出栈恢复上一次的times
        */
	

	i = delete_L(true_list);
	label = delete_L(false_entry);
	snprintf(buffers[i], sizeof(buffers[i]), "beq .LC%d\n",label);

    	i = delete_L(begin_list);
	label = delete_L(begin_entry);

	i = delete_L(false_list);
	label = delete_L(next_entry);
	snprintf(buffers[i], sizeof(buffers[i]), "b .LC%d\n",label);



	times = delete_L(M_list);
}
			|N WHILE LRBK Cond M RRBK M Stmt M M
{
    /*
                当整个产生式规约完成时完成以下的语义动作:
                1.获得需要回填真出口的中间代码的位置
                2.获得真出口的标签
                3.入队

                1.while和if不同,需要用begin_list链表获得cond语句的开始
                2.获得一个标签回填
                3.同时在代码块末尾写下这个标签

                1.获得真出口的代码执行完后跳过else代码块的标签(此处不需要,只将其出队)
                2.入队(b .LC 在规约M2的时候已经入队)


                1.获得需要回填假出口的中间代码的位置
                2.获得假出口的标签
                3.入队

                1.最后将M_list的头结点出栈恢复上一次的times
    */
	int bg, ed;
	bg = begin_entry->next->label;
	ed = false_entry->next->label;	
	while ((bk_list)->next != NULL)
	{
		i = delete_L(bk_list);
		snprintf(buffers[i], sizeof(buffers[i]), "b .LC%d\n",ed);
	}
	while ((ct_list->next) != NULL)
	{
		i = delete_L(ct_list);
		snprintf(buffers[i], sizeof(buffers[i]), "b .LC%d\n",bg);
	}

	i = delete_L(begin_list);
	label = delete_L(begin_entry);
	snprintf(buffers[i], sizeof(buffers[i]), ".LC%d\n",label);
	
	i = delete_L(false_list);
	delete_L(next_entry);
	snprintf(buffers[i], sizeof(buffers[i]), "b .LC%d\n",label);

	i = delete_L(true_list);
	label = delete_L(false_entry);
	snprintf(buffers[i], sizeof(buffers[i]), "beq .LC%d\n",label);

	

	times = delete_L(M_list);
	}
			|BREAK SMC		{
							enQueue("");
							s = new LNode;
							s -> label = rear-1;
							Insert_L(s, bk_list);
						}
			|CONTINUE SMC		{
							enQueue("");
							s = new LNode;
							s -> label = rear-1;
							Insert_L(s, ct_list);
						}
			|RETURN SMC
	{
        snprintf(buffer, sizeof(buffer), "mov r0,#%s\n", "0");
        enQueue(buffer);
	}
			|RETURN Exp SMC
	{
	store_I($2);
	addNS(t_buffer[0]);
	snprintf(buffer, sizeof(temp), "mov r0,%s\n", t_buffer[0]);
	enQueue(buffer);
	}
			;
M			:
{
	if(times == 0){
	    /*规约第一个M的时候进行的动作*/		
		s = new LNode;
		s->label = label_num;
		Insert_L(s,begin_entry);
		label_num++;
		times++;
	}
	else if(times == 1){
	    /*规约第二个M的时候进行的动作*/
		s = new LNode;
		s->label = label_num;
		Insert_L(s,true_entry);
		

		enQueue("");
		
		s = new LNode;
		s ->label = rear-1;
		Insert_L(s, true_list);
	

		//snprintf(buffer, sizeof(buffer), ".LC%d\n", label_num);
		//enQueue(buffer);
		//label_num++;
		times++;
	}
	else if(times == 2){
	    /*规约第三个M的时候进行的动作*/
		enQueue("");
		
		s = new LNode;
		s ->label = rear-1;
		Insert_L(s, false_list);		
	
		s = new LNode;
		s->label = label_num;
		Insert_L(s,false_entry);
		snprintf(buffer, sizeof(buffer), ".LC%d\n",label_num);
		enQueue(buffer);
		
		
	
		//snprintf(buffers[i], sizeof(buffers[i]), "b .LC%d\n",label_num);
		label_num++;
		times++;
	}
	else if(times == 3){
	    /*规约第四个M的时候进行的动作*/
		s = new LNode;
		s->label = label_num;
		Insert_L(s,next_entry);
		snprintf(buffer, sizeof(buffer), ".LC%d\n", label_num);
		enQueue(buffer);
		label_num++;
		times=0;
	}
	
}
;

N			:{
				if (times != 0)
				{
					s = new LNode;
					s -> label = times;
					Insert_L(s, M_list);
					times = 0;
				}		
			}
			;
Exp			:AddExp	{$$ = $1; 
				}
			;
Cond		:LOrExp		{
				enQueue("");
				s = new LNode;
				s -> label = rear-1;
				Insert_L(s, begin_list);

				
				store_I($1);
				addNS(t_buffer[0]);
				snprintf(buffer, sizeof(buffer), "cmp %s,#0\n", t_buffer[0]);
				enQueue(buffer);
				t_buffer_counter = 0;
					}
			;
LVal		:Ident Arr	{$$ = strdup($1);}
			;
Arr			:
			|Arr LSBK Exp RSBK
			|LSBK Exp RSBK
			;
PrimaryExp	:LRBK Exp RRBK	{
	   			$$ = $2;
				}
			|LVal	{
		 		$$ = newNode_c($1);
					}
			|Number	{
				$$ = newNode_i($1);
				}
			;
Number		:IntConst	{$$ = $1;}
			;
UnaryExp	:PrimaryExp	{$$ = $1;}
			|Ident LRBK RRBK	{
							snprintf(buffer, sizeof(buffer), "%s()", $1);
							$$ = newNode_c(buffer);
						}
			|Ident LRBK FuncRParams RRBK
			|UnaryOp UnaryExp		{
								if ($1 == '!')
								{
									$$ = link("!", $2, NULL);
								}
								if ($1 == '-')
								{
									$$ = link("*", $2, newNode_i(-1));
								}
								if ($1 == '+')
								{
									$$ = $2;
								}
							}	
			;
UnaryOp		:'!'		{$$ = '!';}
		|'+'		{$$ = '+';}
		|'-'		{$$ = '-';}
		;
FuncRParams	:Exp Params
			;
Params		:
			|COMMA Exp
			|Params COMMA Exp
			;
MulExp		:UnaryExp			{$$ = $1;}
			|MulExp '*' UnaryExp	{$$ = link("*", $1, $3);}
			|MulExp '/' UnaryExp	{$$ = link("/", $1, $3);}
			|MulExp '%' UnaryExp	{$$ = link("%", $1, $3);}
			;
AddExp		:MulExp				{$$ = $1;}
			|AddExp '+' MulExp	{$$ = link("+", $1, $3);}
			|AddExp '-' MulExp	{$$ = link("-", $1, $3);}
			;
RelExp		:AddExp				{$$ = $1;}
			|RelExp GT AddExp	{$$ = link(">", $1, $3);}
			|RelExp LS AddExp	{$$ = link("<", $1, $3);}
			|RelExp NL AddExp	{$$ = link(">=", $1, $3);}
			|RelExp NG AddExp	{$$ = link("<=", $1, $3);}
			;
EqExp		:RelExp				{$$ = $1;}
		|EqExp EQ RelExp		{$$ = link("==", $1, $3);}
		|EqExp NEQ RelExp		{$$ = link("!=", $1, $3);}
		;
LAndExp		:EqExp				{$$ = $1;}
		|LAndExp AND EqExp		{$$ = link("&&", $1, $3);}
		;	
LOrExp		:LAndExp			{$$ = $1;}
		|LOrExp OR LAndExp		{$$ = link("||", $1, $3);}
		;
ConstExp	:AddExp				{$$ = $1;}
			;
%%
#include "lex.yy.c"
#include <string.h>
/*
int main(int argc, const char *args[])
{	
	extern FILE *yyin, *yyout;

	InitList_L(begin_entry);
	InitList_L(true_list);
	InitList_L(false_list);
	InitList_L(begin_list);

	InitList_L(true_entry);
	InitList_L(false_entry);
	InitList_L(next_entry);
	InitList_L(M_list);	

	yyout = fopen("ir.txt", "w+");
	if (argc >= 1)
	{
		yyin = fopen(args[1], "r");	
	}
	yyparse();
		//check g_symt
	printf("\nchecking g_symt\n");
	for(i = 0; i < g_symt_position; i++)
	{
		printf("%s %d\n", g_symt.syms[i].name, g_symt.syms[i].val);	
	}	
	

	//check symt
	//int i;
	printf("\nchecking symt\n");
	int j;
	for (i = 0; i < symt_position; i++)
	{
		printf("%s:\n", symt[i].f_name);		
		for (j = 0; j < size[i];j++)
		{
			printf("%s %d\n", symt[i].syms[j].name, symt[i].syms[j].offset);	
		}
	}	
	

	return 0;


    return 0;
}
*/

void yyerror(char *s, ...)
{
    extern int yylineno;

    va_list ap;
    va_start(ap, s);

    fprintf(stderr, "%d: error: ", yylineno);
    vfprintf(stderr, s, ap);
    fprintf(stderr, "\n");
}
