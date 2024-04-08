#include <math.h>
#include "parser.tab.c"
#define INF 65536
#define MEMO 16

int sym_size = 0;
int t = 0;

struct decl *sym;

typedef struct Rvalue{
	char *varName;
    	int num;
}Rvalue;/*寄存器使用情况数组*/

Rvalue r[8] = {{"null",0},{"null",0},{"null",0},{"null",0},{"null",0},{"null",0},{"null",0},{"null",0}};

char c_buffer[1024][50];/*存放中间代码的缓冲区*/
int begin,end;/*起始和终点指针*/

FILE *fpWrite;
FILE *fpRead;
int line = 0 ;

int cond_times;
int reg_seq = 0;

char* getIden(char *buffer,int begin,int end){
	/*截取字符串*/
	char *temp = new char[end-begin+1];
	int i;
	for(i = 0;i < end-begin+1;i++){
		temp[i] = buffer[begin+i];
	}
	temp[i] = '\0';
	return temp;
}

int getNum(char *buffer,int begin,int end){
	/*将字符串转换为十进制数*/
	int sum = 0;
	for(int i = 0;i < end-begin+1;i++){
		sum = (buffer[begin+i] - 48) + sum * 10;
	}
	return sum;
}

int getOffset(struct decl* sym,char* varName){
	/*获得某一变量的偏移量*/
	int i = 0;
    while(sym[i].name != NULL){
        if(strcmp(sym[i].name,varName)==0)return sym[i].offset;
		i++;
    }
    /*while(glo != NULL){
            if(strcmp(glo++->varNme,name)==0)return glo->;
    }*/  /*此处为全局变量的符号表，但是还没给出，所以暂时不用*/
    return INF;/*找不到返回INF*/
}

int get_Global_var(struct sym_table g_symt,char* varName){
	int i = 0;
    while(i < g_symt_position){
         if(strcmp(g_symt.syms[i].name,varName)==0)return i;
	 i++;
    }
	return INF;/*找不到返回INF*/
}

int getReg(Rvalue* r,struct decl *sym,char* varName){
	/*寄存器分配算法*/
	int i = 0;

	for(i = 1;i < 8;i++)
		if(strcmp(r[i].varName,"null")==0) return i;

	int replace_r = reg_seq % 8;
	if(replace_r==0){replace_r = 1;reg_seq+=2;}
	else reg_seq++;

	return replace_r;
}

void define_g_var(struct sym_table g_symt){
            fprintf(fpWrite,".text\n");
            fprintf(fpWrite,".data\n");
    for(int i =  0;i < g_symt_position;i++){
            fprintf(fpWrite,".global %s\n",g_symt.syms[i].name);
            fprintf(fpWrite,".align	2\n");
            fprintf(fpWrite,".type	%s, %%object\n",g_symt.syms[i].name);
            fprintf(fpWrite,".size	%s, 4\n",g_symt.syms[i].name);
            fprintf(fpWrite,"%s:\n",g_symt.syms[i].name);
            fprintf(fpWrite,".word  %d\n",g_symt.syms[i].val);
        }
}

void print_gsymt(struct sym_table g_symt){
    fprintf(fpWrite,".L4:\n");
    fprintf(fpWrite,".align 2\n");
    fprintf(fpWrite,".L3:\n");
    for(int i =  0;i < g_symt_position;i++){
        fprintf(fpWrite,".word %s\n",g_symt.syms[i].name);
    }
}

void set_MEMO(struct decl* sym,Rvalue *r){
	for(int i = 0;i < sym_size;i++){
		sym[i].pos = MEMO;
	}
	for(i = 0;i < 8;i++){
		r[i].varName = "null";
	}
}

void genAss(char *ch,Rvalue *r,struct decl *sym){
	/*代码生成算法*/
	int begin = 0;
	int end = 0;
	int i = 0;
    while(ch[i] != '\0'){
		if(ch[i] == ' '){
		end = i-1;
		char* temp = getIden(ch,begin,end);
			if(strcmp(temp,"ent") == 0){
				fprintf(fpWrite,"%s","push {fp,lr}\n");
				fprintf(fpWrite,"%s","add fp,sp,#4\n");
				fprintf(fpWrite,"sub sp,sp,#%d\n",sym_size * sizeof(int) +4);
				return;
			}
			if (strcmp(temp,"ret") == 0){
				fprintf(fpWrite,"sub sp,fp,#4\n");
				fprintf(fpWrite,"pop {fp,pc}\n");
				return;
			}
			if (strcmp(temp,"ite") == 0){
				fprintf(fpWrite,"%s\n",ch);
				return;
			}
			if (strcmp(temp,"beq") == 0){
				fprintf(fpWrite,"%s\n",ch);
				return;
			}
			if (strcmp(temp,"bne") == 0){
				fprintf(fpWrite,"%s\n",ch);
				return;
			}
			if (strcmp(temp,"bge") == 0){
				fprintf(fpWrite,"%s\n",ch);
				return;
			}
			if (strcmp(temp,"blt") == 0){
				fprintf(fpWrite,"%s\n",ch);
				return;
			}
			if (strcmp(temp,"bgt") == 0){
				fprintf(fpWrite,"%s\n",ch);
				return;
			}
			if (strcmp(temp,"ble") == 0){
				fprintf(fpWrite,"%s\n",ch);
				return;
			}
			if (strcmp(temp,"bal") == 0){
				fprintf(fpWrite,"%s\n",ch);
				return;
			}
			if (strcmp(temp,"bl") == 0){
				set_MEMO(sym,r);
				fprintf(fpWrite,"%s\n",ch);
				return;
			}
			if (strcmp(temp,"b") == 0){
				fprintf(fpWrite,"%s\n",ch);
				return;
			}
			if (strcmp(temp,"orr") == 0){
				int isNumber1 = 0;
				int isNumber2 = 0;
				int begin1;
				int end1;
				int begin2;
				int end2;
				i++;
				begin = i;
				while(ch[i] != ',')i++;
				end = i-1;
				char* varName1 = getIden(ch,begin,end);
				i++;
				begin = i;
				if(ch[i] == '#'){isNumber1 = 1;begin1 = i+1;}
				while(ch[i] != ',')i++;
				end1 = i-1;
				end  = i-1;
				char* varName2 = getIden(ch,begin,end);
				i++;
				begin = i;
				if(ch[i] == '#'){isNumber2 = 1;begin2 = i+1;}
				while(ch[i] != '\n')i++;
				end2  = i-1;
				end  = i-1;
				char* varName3 = getIden(ch,begin,end);

				int x1 = getReg(r,sym,varName1);
				int offset1 = getOffset(sym,varName1);
				int offset2 = getOffset(sym,varName2);

				int j = 0;
				int change = -1;

				int sign1;
				int sign2;
				int sign3;

				if(offset1 == INF){
					sign1 = get_Global_var(g_symt,varName1);
					fprintf(fpWrite,"ldr r%d, .L3+%d\n",x1,4*sign1);
					r[x1].varName = varName1;
				}

				int x2 = getReg(r,sym,varName2);

				if(isNumber1 == 1){
					sign2 = getNum(ch,begin1,end1);
					if(isNumber2 == 1){
						sign3 = getNum(ch,begin2,end2);
						if(offset1 == INF){
						fprintf(fpWrite,"mov r%d, #%d\n",x2,sign3|sign2);
						fprintf(fpWrite,"str r%d, [r%d]\n",x2,x1);
						if(offset1 == INF)r[x1].varName = "null";
						return;
						}
						if(offset1 != INF){
						fprintf(fpWrite,"mov r%d, #%d\n",x1,sign3|sign2);
						fprintf(fpWrite,"str r%d, [fp,#%d]\n",x1,offset1);
						return;
						}
					}
					else{
						fprintf(fpWrite,"mov r%d, #%d\n",x2,sign2);
					}
				}
                else{
                    j = 0;
		    r[x2].varName = varName2;
                    if(offset2 == INF){
                        sign2 = get_Global_var(g_symt,varName2);
                        fprintf(fpWrite,"ldr r%d, .L3+%d\n",x2,4*sign2);
                        fprintf(fpWrite,"ldr r%d, [r%d]\n",x2,x2);
                    }
                    else{
                    while(1){
                        if(strcmp(sym[j].name,varName2) == 0){
                                fprintf(fpWrite,"ldr r%d,[fp,#%d]\n",x2,offset2);
                                break;
                        }
                        j++;
                    }
                    }
                 }
                int x3 = getReg(r,sym,varName3);
		int offset3;
		r[x1].varName = "null";
		r[x2].varName = "null";

                if(isNumber2 == 1){
						sign3 = getNum(ch,begin2,end2);
						if(offset1 == INF){
						fprintf(fpWrite,"mov r%d, #%d\n",x3,sign3);
						fprintf(fpWrite,"orr r%d, r%d, r%d\n",x2,x2,x3);
						fprintf(fpWrite,"str r%d, [r%d]\n",x2,x1);
						return;
						}
						if(offset1 != INF){
						fprintf(fpWrite,"mov r%d, #%d\n",x3,sign3);
                        			fprintf(fpWrite,"orr r%d, r%d, r%d\n",x2,x2,x3);
						fprintf(fpWrite,"str r%d, [fp,#%d]\n",x2,offset1);
						return;
						}
				}

				j=0;
				offset3 = getOffset(sym,varName3);
				if(offset3 == INF){
					sign3 = get_Global_var(g_symt,varName3);
					if(offset1 == INF){
					fprintf(fpWrite,"ldr r%d, .L3+%d\n",x3,4*sign3);
					fprintf(fpWrite,"ldr r%d, [r%d]\n",x3,x3);
					fprintf(fpWrite,"orr r%d, r%d, r%d\n",x2,x2,x3);
					fprintf(fpWrite,"str r%d, [r%d]\n",x2,x1);
					return;
					}
					if(offset1 != INF){
					fprintf(fpWrite,"ldr r%d, .L3+%d\n",x3,4*sign3);
					fprintf(fpWrite,"ldr r%d, [r%d]\n",x3,x3);
					fprintf(fpWrite,"orr r%d, r%d, r%d\n",x2,x2,x3);
					fprintf(fpWrite,"str r%d, [fp,#%d]\n",x2,offset1);
					r[x2].varName = varName1;
					return;
					}
				}
				else{
				while(1){
					if(strcmp(sym[j].name,varName3) == 0){
						fprintf(fpWrite,"ldr r%d,[fp,#%d]\n",x3,offset3);
						fprintf(fpWrite,"orr r%d, r%d, r%d\n",x2,x2,x3);
						if(offset1 == INF){
						fprintf(fpWrite,"str r%d, [r%d]\n",x2,x1);
						return;
						}
						if(offset1 != INF){
						fprintf(fpWrite,"str r%d, [fp,#%d]\n",x2,offset1);
						return;
						}
					}
					j++;
				}
				}
				return;
			}
			if (strcmp(temp,"and") == 0){
				int isNumber1 = 0;
				int isNumber2 = 0;
				int begin1;
				int end1;
				int begin2;
				int end2;
				i++;
				begin = i;
				while(ch[i] != ',')i++;
				end = i-1;
				char* varName1 = getIden(ch,begin,end);
				i++;
				begin = i;
				if(ch[i] == '#'){isNumber1 = 1;begin1 = i+1;}
				while(ch[i] != ',')i++;
				end1 = i-1;
				end  = i-1;
				char* varName2 = getIden(ch,begin,end);
				i++;
				begin = i;
				if(ch[i] == '#'){isNumber2 = 1;begin2 = i+1;}
				while(ch[i] != '\n')i++;
				end2  = i-1;
				end  = i-1;
				char* varName3 = getIden(ch,begin,end);

				int x1 = getReg(r,sym,varName1);
				int offset1 = getOffset(sym,varName1);
				int offset2 = getOffset(sym,varName2);

				int j = 0;
				int change = -1;

				int sign1;
				int sign2;
				int sign3;

				if(offset1 == INF){
					sign1 = get_Global_var(g_symt,varName1);
					fprintf(fpWrite,"ldr r%d, .L3+%d\n",x1,4*sign1);
					r[x1].varName = varName1;
				}

				int x2 = getReg(r,sym,varName2);

				if(isNumber1 == 1){
					sign2 = getNum(ch,begin1,end1);
					if(isNumber2 == 1){
						sign3 = getNum(ch,begin2,end2);
						if(offset1 == INF){
						fprintf(fpWrite,"mov r%d, #%d\n",x2,sign3&sign2);
						fprintf(fpWrite,"str r%d, [r%d]\n",x2,x1);
						if(offset1 == INF)r[x1].varName = "null";
						return;
						}
						if(offset1 != INF){
						fprintf(fpWrite,"mov r%d, #%d\n",x1,sign3&sign2);
						fprintf(fpWrite,"str r%d, [fp,#%d]\n",x1,offset1);
						return;
						}
					}
					else{
						fprintf(fpWrite,"mov r%d, #%d\n",x2,sign2);
					}
				}
                else{
                    j = 0;
		    r[x2].varName = varName2;
                    if(offset2 == INF){
                        sign2 = get_Global_var(g_symt,varName2);
                        fprintf(fpWrite,"ldr r%d, .L3+%d\n",x2,4*sign2);
                        fprintf(fpWrite,"ldr r%d, [r%d]\n",x2,x2);
                    }
                    else{
                    while(1){
                        if(strcmp(sym[j].name,varName2) == 0){
                                fprintf(fpWrite,"ldr r%d,[fp,#%d]\n",x2,offset2);
                                break;
                        }
                        j++;
                    }
                    }
                 }
                int x3 = getReg(r,sym,varName3);
		int offset3;
		r[x1].varName = "null";
		r[x2].varName = "null";

                if(isNumber2 == 1){
						sign3 = getNum(ch,begin2,end2);
						if(offset1 == INF){
						fprintf(fpWrite,"mov r%d, #%d\n",x3,sign3);
						fprintf(fpWrite,"and r%d, r%d, r%d\n",x2,x2,x3);
						fprintf(fpWrite,"str r%d, [r%d]\n",x2,x1);
						return;
						}
						if(offset1 != INF){
						fprintf(fpWrite,"mov r%d, #%d\n",x3,sign3);
                        			fprintf(fpWrite,"and r%d, r%d, r%d\n",x2,x2,x3);
						fprintf(fpWrite,"str r%d, [fp,#%d]\n",x2,offset1);
						return;
						}
				}

				j=0;
				offset3 = getOffset(sym,varName3);
				if(offset3 == INF){
					sign3 = get_Global_var(g_symt,varName3);
					if(offset1 == INF){
					fprintf(fpWrite,"ldr r%d, .L3+%d\n",x3,4*sign3);
					fprintf(fpWrite,"ldr r%d, [r%d]\n",x3,x3);
					fprintf(fpWrite,"and r%d, r%d, r%d\n",x2,x2,x3);
					fprintf(fpWrite,"str r%d, [r%d]\n",x2,x1);
					return;
					}
					if(offset1 != INF){
					fprintf(fpWrite,"ldr r%d, .L3+%d\n",x3,4*sign3);
					fprintf(fpWrite,"ldr r%d, [r%d]\n",x3,x3);
					fprintf(fpWrite,"and r%d, r%d, r%d\n",x2,x2,x3);
					fprintf(fpWrite,"str r%d, [fp,#%d]\n",x2,offset1);
					r[x2].varName = varName1;
					return;
					}
				}
				else{
				while(1){
					if(strcmp(sym[j].name,varName3) == 0){
						fprintf(fpWrite,"ldr r%d,[fp,#%d]\n",x3,offset3);
						fprintf(fpWrite,"and r%d, r%d, r%d\n",x2,x2,x3);
						if(offset1 == INF){
						fprintf(fpWrite,"str r%d, [r%d]\n",x2,x1);
						return;
						}
						if(offset1 != INF){
						fprintf(fpWrite,"str r%d, [fp,#%d]\n",x2,offset1);
						return;
						}
					}
					j++;
				}
				}
				return;
			}
			if (strcmp(temp,"add") == 0){
				int isNumber1 = 0;
				int isNumber2 = 0;
				int begin1;
				int end1;
				int begin2;
				int end2;
				i++;
				begin = i;
				while(ch[i] != ',')i++;
				end = i-1;
				char* varName1 = getIden(ch,begin,end);
				i++;
				begin = i;
				if(ch[i] == '#'){isNumber1 = 1;begin1 = i+1;}
				while(ch[i] != ',')i++;
				end1 = i-1;
				end  = i-1;
				char* varName2 = getIden(ch,begin,end);
				i++;
				begin = i;
				if(ch[i] == '#'){isNumber2 = 1;begin2 = i+1;}
				while(ch[i] != '\n')i++;
				end2  = i-1;
				end  = i-1;
				char* varName3 = getIden(ch,begin,end);

				int x1 = getReg(r,sym,varName1);
				int offset1 = getOffset(sym,varName1);
				int offset2 = getOffset(sym,varName2);

				int j = 0;
				int change = -1;

				int sign1;
				int sign2;
				int sign3;

				if(offset1 == INF){
					sign1 = get_Global_var(g_symt,varName1);
					fprintf(fpWrite,"ldr r%d, .L3+%d\n",x1,4*sign1);
					r[x1].varName = varName1;
				}

				int x2 = getReg(r,sym,varName2);

				if(isNumber1 == 1){
					sign2 = getNum(ch,begin1,end1);
					if(isNumber2 == 1){
						sign3 = getNum(ch,begin2,end2);
						if(offset1 == INF){
						fprintf(fpWrite,"mov r%d, #%d\n",x2,sign3+sign2);
						fprintf(fpWrite,"str r%d, [r%d]\n",x2,x1);
						if(offset1 == INF)r[x1].varName = "null";
						return;
						}
						if(offset1 != INF){
						fprintf(fpWrite,"mov r%d, #%d\n",x1,sign3+sign2);
						fprintf(fpWrite,"str r%d, [fp,#%d]\n",x1,offset1);
						return;
						}
					}
					else{
						fprintf(fpWrite,"mov r%d, #%d\n",x2,sign2);
					}
				}
                else{
                    j = 0;
		    r[x2].varName = varName2;
                    if(offset2 == INF){
                        sign2 = get_Global_var(g_symt,varName2);
                        fprintf(fpWrite,"ldr r%d, .L3+%d\n",x2,4*sign2);
                        fprintf(fpWrite,"ldr r%d, [r%d]\n",x2,x2);
                    }
                    else{
                    while(1){
                        if(strcmp(sym[j].name,varName2) == 0){
                                fprintf(fpWrite,"ldr r%d,[fp,#%d]\n",x2,offset2);
                                break;
                        }
                        j++;
                    }
                    }
                 }
                int x3 = getReg(r,sym,varName3);
		int offset3;
		r[x1].varName = "null";
		r[x2].varName = "null";

                if(isNumber2 == 1){
						sign3 = getNum(ch,begin2,end2);
						if(offset1 == INF){
						fprintf(fpWrite,"mov r%d, #%d\n",x3,sign3);
						fprintf(fpWrite,"add r%d, r%d, r%d\n",x2,x2,x3);
						fprintf(fpWrite,"str r%d, [r%d]\n",x2,x1);
						return;
						}
						if(offset1 != INF){
						fprintf(fpWrite,"mov r%d, #%d\n",x3,sign3);
                        			fprintf(fpWrite,"add r%d, r%d, r%d\n",x2,x2,x3);
						fprintf(fpWrite,"str r%d, [fp,#%d]\n",x2,offset1);
						return;
						}
				}

				j=0;
				offset3 = getOffset(sym,varName3);
				if(offset3 == INF){
					sign3 = get_Global_var(g_symt,varName3);
					if(offset1 == INF){
					fprintf(fpWrite,"ldr r%d, .L3+%d\n",x3,4*sign3);
					fprintf(fpWrite,"ldr r%d, [r%d]\n",x3,x3);
					fprintf(fpWrite,"add r%d, r%d, r%d\n",x2,x2,x3);
					fprintf(fpWrite,"str r%d, [r%d]\n",x2,x1);
					return;
					}
					if(offset1 != INF){
					fprintf(fpWrite,"ldr r%d, .L3+%d\n",x3,4*sign3);
					fprintf(fpWrite,"ldr r%d, [r%d]\n",x3,x3);
					fprintf(fpWrite,"add r%d, r%d, r%d\n",x2,x2,x3);
					fprintf(fpWrite,"str r%d, [fp,#%d]\n",x2,offset1);
					r[x2].varName = varName1;
					return;
					}
				}
				else{
				while(1){
					if(strcmp(sym[j].name,varName3) == 0){
						fprintf(fpWrite,"ldr r%d,[fp,#%d]\n",x3,offset3);
						fprintf(fpWrite,"add r%d, r%d, r%d\n",x2,x2,x3);
						if(offset1 == INF){
						fprintf(fpWrite,"str r%d, [r%d]\n",x2,x1);
						return;
						}
						if(offset1 != INF){
						fprintf(fpWrite,"str r%d, [fp,#%d]\n",x2,offset1);
						return;
						}
					}
					j++;
				}
				}
				return;
			}
			if (strcmp(temp,"sub") == 0){
				int isNumber1 = 0;
				int isNumber2 = 0;
				int begin1;
				int end1;
				int begin2;
				int end2;
				i++;
				begin = i;
				while(ch[i] != ',')i++;
				end = i-1;
				char* varName1 = getIden(ch,begin,end);
				i++;
				begin = i;
				if(ch[i] == '#'){isNumber1 = 1;begin1 = i+1;}
				while(ch[i] != ',')i++;
				end1 = i-1;
				end  = i-1;
				char* varName2 = getIden(ch,begin,end);
				i++;
				begin = i;
				if(ch[i] == '#'){isNumber2 = 1;begin2 = i+1;}
				while(ch[i] != '\n')i++;
				end2  = i-1;
				end  = i-1;
				char* varName3 = getIden(ch,begin,end);

				int x1 = getReg(r,sym,varName1);
				int offset1 = getOffset(sym,varName1);
				int offset2 = getOffset(sym,varName2);

				int j = 0;
				int change = -1;

				int sign1;
				int sign2;
				int sign3;

				if(offset1 == INF){
					sign1 = get_Global_var(g_symt,varName1);
					fprintf(fpWrite,"ldr r%d, .L3+%d\n",x1,4*sign1);
					r[x1].varName = varName1;
				}

				int x2 = getReg(r,sym,varName2);

				if(isNumber1 == 1){
					sign2 = getNum(ch,begin1,end1);
					if(isNumber2 == 1){
						sign3 = getNum(ch,begin2,end2);
						if(offset1 == INF){
						fprintf(fpWrite,"mov r%d, #%d\n",x2,sign3-sign2);
						fprintf(fpWrite,"str r%d, [r%d]\n",x2,x1);
						if(offset1 == INF)r[x1].varName = "null";
						return;
						}
						if(offset1 != INF){
						fprintf(fpWrite,"mov r%d, #%d\n",x1,sign3-sign2);
						fprintf(fpWrite,"str r%d, [fp,#%d]\n",x1,offset1);
						return;
						}
					}
					else{
						fprintf(fpWrite,"mov r%d, #%d\n",x2,sign2);
					}
				}
                else{
                    j = 0;
		    r[x2].varName = varName2;
                    if(offset2 == INF){
                        sign2 = get_Global_var(g_symt,varName2);
                        fprintf(fpWrite,"ldr r%d, .L3+%d\n",x2,4*sign2);
                        fprintf(fpWrite,"ldr r%d, [r%d]\n",x2,x2);
                    }
                    else{
                    while(1){
                        if(strcmp(sym[j].name,varName2) == 0){
                                fprintf(fpWrite,"ldr r%d,[fp,#%d]\n",x2,offset2);
                                break;
                        }
                        j++;
                    }
                    }
                 }
                int x3 = getReg(r,sym,varName3);
		int offset3;
		r[x1].varName = "null";
		r[x2].varName = "null";

                if(isNumber2 == 1){
						sign3 = getNum(ch,begin2,end2);
						if(offset1 == INF){
						fprintf(fpWrite,"mov r%d, #%d\n",x3,sign3);
						fprintf(fpWrite,"sub r%d, r%d, r%d\n",x2,x2,x3);
						fprintf(fpWrite,"str r%d, [r%d]\n",x2,x1);
						return;
						}
						if(offset1 != INF){
						fprintf(fpWrite,"mov r%d, #%d\n",x3,sign3);
                        			fprintf(fpWrite,"sub r%d, r%d, r%d\n",x2,x2,x3);
						fprintf(fpWrite,"str r%d, [fp,#%d]\n",x2,offset1);
						return;
						}
				}

				j=0;
				offset3 = getOffset(sym,varName3);
				if(offset3 == INF){
					sign3 = get_Global_var(g_symt,varName3);
					if(offset1 == INF){
					fprintf(fpWrite,"ldr r%d, .L3+%d\n",x3,4*sign3);
					fprintf(fpWrite,"ldr r%d, [r%d]\n",x3,x3);
					fprintf(fpWrite,"sub r%d, r%d, r%d\n",x2,x2,x3);
					fprintf(fpWrite,"str r%d, [r%d]\n",x2,x1);
					return;
					}
					if(offset1 != INF){
					fprintf(fpWrite,"ldr r%d, .L3+%d\n",x3,4*sign3);
					fprintf(fpWrite,"ldr r%d, [r%d]\n",x3,x3);
					fprintf(fpWrite,"sub r%d, r%d, r%d\n",x2,x2,x3);
					fprintf(fpWrite,"str r%d, [fp,#%d]\n",x2,offset1);
					r[x2].varName = varName1;
					return;
					}
				}
				else{
				while(1){
					if(strcmp(sym[j].name,varName3) == 0){
						fprintf(fpWrite,"ldr r%d,[fp,#%d]\n",x3,offset3);
						fprintf(fpWrite,"sub r%d, r%d, r%d\n",x2,x2,x3);
						if(offset1 == INF){
						fprintf(fpWrite,"str r%d, [r%d]\n",x2,x1);
						return;
						}
						if(offset1 != INF){
						fprintf(fpWrite,"str r%d, [fp,#%d]\n",x2,offset1);
						return;
						}
					}
					j++;
				}
				}
				return;
			}
			if (strcmp(temp,"mul") == 0){
				int isNumber1 = 0;
				int isNumber2 = 0;
				int begin1;
				int end1;
				int begin2;
				int end2;
				i++;
				begin = i;
				while(ch[i] != ',')i++;
				end = i-1;
				char* varName1 = getIden(ch,begin,end);
				i++;
				begin = i;
				if(ch[i] == '#'){isNumber1 = 1;begin1 = i+1;}
				while(ch[i] != ',')i++;
				end1 = i-1;
				end  = i-1;
				char* varName2 = getIden(ch,begin,end);
				i++;
				begin = i;
				if(ch[i] == '#'){isNumber2 = 1;begin2 = i+1;}
				while(ch[i] != '\n')i++;
				end2  = i-1;
				end  = i-1;
				char* varName3 = getIden(ch,begin,end);

				int x1 = getReg(r,sym,varName1);
				int offset1 = getOffset(sym,varName1);
				int offset2 = getOffset(sym,varName2);

				int j = 0;
				int change = -1;

				int sign1;
				int sign2;
				int sign3;

				if(offset1 == INF){
					sign1 = get_Global_var(g_symt,varName1);
					fprintf(fpWrite,"ldr r%d, .L3+%d\n",x1,4*sign1);
					r[x1].varName = varName1;
				}

				int x2 = getReg(r,sym,varName2);

				if(isNumber1 == 1){
					sign2 = getNum(ch,begin1,end1);
					if(isNumber2 == 1){
						sign3 = getNum(ch,begin2,end2);
						if(offset1 == INF){
						fprintf(fpWrite,"mov r%d, #%d\n",x2,sign3*sign2);
						fprintf(fpWrite,"str r%d, [r%d]\n",x2,x1);
						if(offset1 == INF)r[x1].varName = "null";
						return;
						}
						if(offset1 != INF){
						fprintf(fpWrite,"mov r%d, #%d\n",x1,sign3*sign2);
						fprintf(fpWrite,"str r%d, [fp,#%d]\n",x1,offset1);
						return;
						}
					}
					else{
						fprintf(fpWrite,"mov r%d, #%d\n",x2,sign2);
					}
				}
                else{
                    j = 0;
		    r[x2].varName = varName2;
                    if(offset2 == INF){
                        sign2 = get_Global_var(g_symt,varName2);
                        fprintf(fpWrite,"ldr r%d, .L3+%d\n",x2,4*sign2);
                        fprintf(fpWrite,"ldr r%d, [r%d]\n",x2,x2);
                    }
                    else{
                    while(1){
                        if(strcmp(sym[j].name,varName2) == 0){
                                fprintf(fpWrite,"ldr r%d,[fp,#%d]\n",x2,offset2);
                                break;
                        }
                        j++;
                    }
                    }
                 }
                int x3 = getReg(r,sym,varName3);
		int offset3;
		r[x1].varName = "null";
		r[x2].varName = "null";

                if(isNumber2 == 1){
						sign3 = getNum(ch,begin2,end2);
						if(offset1 == INF){
						fprintf(fpWrite,"mov r%d, #%d\n",x3,sign3);
						fprintf(fpWrite,"mul r%d, r%d, r%d\n",x2,x2,x3);
						fprintf(fpWrite,"str r%d, [r%d]\n",x2,x1);
						return;
						}
						if(offset1 != INF){
						fprintf(fpWrite,"mov r%d, #%d\n",x3,sign3);
                        			fprintf(fpWrite,"mul r%d, r%d, r%d\n",x2,x2,x3);
						fprintf(fpWrite,"str r%d, [fp,#%d]\n",x2,offset1);
						return;
						}
				}

				j=0;
				offset3 = getOffset(sym,varName3);
				if(offset3 == INF){
					sign3 = get_Global_var(g_symt,varName3);
					if(offset1 == INF){
					fprintf(fpWrite,"ldr r%d, .L3+%d\n",x3,4*sign3);
					fprintf(fpWrite,"ldr r%d, [r%d]\n",x3,x3);
					fprintf(fpWrite,"mul r%d, r%d, r%d\n",x2,x2,x3);
					fprintf(fpWrite,"str r%d, [r%d]\n",x2,x1);
					return;
					}
					if(offset1 != INF){
					fprintf(fpWrite,"ldr r%d, .L3+%d\n",x3,4*sign3);
					fprintf(fpWrite,"ldr r%d, [r%d]\n",x3,x3);
					fprintf(fpWrite,"mul r%d, r%d, r%d\n",x2,x2,x3);
					fprintf(fpWrite,"str r%d, [fp,#%d]\n",x2,offset1);
					r[x2].varName = varName1;
					return;
					}
				}
				else{
				while(1){
					if(strcmp(sym[j].name,varName3) == 0){
						fprintf(fpWrite,"ldr r%d,[fp,#%d]\n",x3,offset3);
						fprintf(fpWrite,"mul r%d, r%d, r%d\n",x2,x2,x3);
						if(offset1 == INF){
						fprintf(fpWrite,"str r%d, [r%d]\n",x2,x1);
						return;
						}
						if(offset1 != INF){
						fprintf(fpWrite,"str r%d, [fp,#%d]\n",x2,offset1);
						return;
						}
					}
					j++;
				}
				}
				return;
			}
			if (strcmp(temp,"div") == 0){
				set_MEMO(sym,r);
				int isNumber1 = 0;
                				int begin1;
                				int end1;
                				int isNumber2 = 0;
                				int begin2;
                				int end2;
                				i++;
                				begin = i;
                				while(ch[i] != ',')i++;
                				end = i-1;
                				char* varName1 = getIden(ch,begin,end);
                				i++;
                				begin = i;
                				if(ch[i] == '#'){isNumber1 = 1;begin1 = i+1;}
                				while(ch[i] != ',')i++;
                				end1 = i-1;
                				end  = i-1;
                				char* varName2 = getIden(ch,begin,end);
                				i++;
                				begin = i;
                				if(ch[i] == '#'){isNumber2 = 1;begin2 = i+1;}
                				while(ch[i] != '\n')i++;
                				end2 = i-1;
                				end  = i-1;

                				int sign1;
                				int sign2;
						int sign3;

                				char* varName3 = getIden(ch,begin,end);
                				int x1 = getReg(r,sym,varName1);

                				int offset1 = getOffset(sym,varName1);

                				int j = 0;
                				int change = 0;
						if(offset1 == INF){
							r[x1].varName = varName1;
							g_symt.syms[sign1].pos = x1;
						}


						int x2 = getReg(r,sym,varName2);
                				int offset2 = getOffset(sym,varName2);
                				j = 0;
                				if(isNumber1 == 1){
                                			sign2 = getNum(ch,begin1,end1);
							if(isNumber2 == 1){
								sign3 = getNum(ch,begin2,end2);	
								if(offset1 == INF){
									sign1 = get_Global_var(g_symt,varName1);
									fprintf(fpWrite,"mov r%d, #%d\n",x2,sign2/sign3);
									fprintf(fpWrite,"ldr r%d, .L3+%d\n",x1,sign1*4);
									fprintf(fpWrite,"str r%d, [r%d]\n",x2,x1);
									return;
								}
								else if(offset1 != INF){
									fprintf(fpWrite,"mov r%d, #%d\n",x1,sign2/sign3);
									fprintf(fpWrite,"str r%d, [fp,#%d]\n",x1,offset1);
									return;
								}
							}
							else{
								fprintf(fpWrite,"mov r0, %s\n",varName2);
							}
                                		}
                				else {
						if(offset2 == INF){
							r[x2].varName = varName2;
							sign2 = get_Global_var(g_symt,varName2);
							fprintf(fpWrite,"ldr r%d, .L3+%d\n",x1,sign2*4);
							fprintf(fpWrite,"ldr r0, [r%d]\n",x1);
						}
						else{

                				while(1){
                					if(strcmp(sym[j].name,varName2) == 0){
                							fprintf(fpWrite,"ldr r0,[fp,#%d]\n",offset2);
                							break;
                						}
                					j++;
                					}
						}
                				}

                				if(isNumber2 == 1){
                					fprintf(fpWrite,"mov r1,%s\n",varName3);
                					fprintf(fpWrite,"bl __aeabi_idiv\n");
							if(offset1 == INF){
								fprintf(fpWrite,"ldr r%d, .L3+%d\n",x1);
								fprintf(fpWrite,"str r0, [r%d]\n",x1);
							}
							else if(offset1 != INF){
								fprintf(fpWrite,"mov r%d, r0\n",x1);
                						fprintf(fpWrite,"str r%d,[fp,#%d]\n",x1,offset1);
							}
                					return ;
                				}

                				j=0;
                				int offset3 = getOffset(sym,varName3);
						if(offset1 == INF)r[x1].varName = "null";
						if(offset2 == INF)r[x2].varName = "null";

						if(offset3 == INF){
							sign3 = get_Global_var(g_symt,varName3);
							fprintf(fpWrite,"ldr r%d, .L3+%d\n",x1,sign3*4);
							fprintf(fpWrite,"ldr r1, [r%d]\n",x1,x1);
							fprintf(fpWrite,"bl __aeabi_idiv\n");
							if(offset1 == INF){
								fprintf(fpWrite,"ldr r%d, .L3+%d\n",x1);
								fprintf(fpWrite,"str r0,[r%d]\n",x1);
							}
							else if(offset1 !=INF){
								fprintf(fpWrite,"mov r%d,r1\n",x1);
                						fprintf(fpWrite,"str r%d,[fp,#%d]\n",x1,offset1);
							}
							return;
						}
                				while(1){
                					if(strcmp(sym[j].name,varName3) == 0){
                						fprintf(fpWrite,"ldr r0,[fp,#%d]\n",offset3);
                						fprintf(fpWrite,"bl __aeabi_idiv\n",varName2);
                						if(offset1 == INF){
								fprintf(fpWrite,"ldr r%d, .L3+%d\n",x1);
								fprintf(fpWrite,"str r0,[r%d]\n",x1);
							}
								else if(offset1 !=INF){
								fprintf(fpWrite,"mov r%d,r0\n",x1);
                						fprintf(fpWrite,"str r%d,[fp,#%d]\n",x1,offset1);
							}
                						break;
                					}
                					j++;
                				}
                				return;
			}
			if (strcmp(temp,"mod") == 0){
				set_MEMO(sym,r);
				int isNumber1 = 0;
                				int begin1;
                				int end1;
                				int isNumber2 = 0;
                				int begin2;
                				int end2;
                				i++;
                				begin = i;
                				while(ch[i] != ',')i++;
                				end = i-1;
                				char* varName1 = getIden(ch,begin,end);
                				i++;
                				begin = i;
                				if(ch[i] == '#'){isNumber1 = 1;begin1 = i+1;}
                				while(ch[i] != ',')i++;
                				end1 = i-1;
                				end  = i-1;
                				char* varName2 = getIden(ch,begin,end);
                				i++;
                				begin = i;
                				if(ch[i] == '#'){isNumber2 = 1;begin2 = i+1;}
                				while(ch[i] != '\n')i++;
                				end2 = i-1;
                				end  = i-1;

                				int sign1;
                				int sign2;
						int sign3;

                				char* varName3 = getIden(ch,begin,end);
                				int x1 = getReg(r,sym,varName1);

                				int offset1 = getOffset(sym,varName1);

                				int j = 0;
                				int change = 0;
						if(offset1 == INF){
							r[x1].varName = varName1;
							g_symt.syms[sign1].pos = x1;
						}


						int x2 = getReg(r,sym,varName2);
                				int offset2 = getOffset(sym,varName2);
                				j = 0;
                				if(isNumber1 == 1){
                                			sign2 = getNum(ch,begin1,end1);
							if(isNumber2 == 1){
								sign3 = getNum(ch,begin2,end2);	
								if(offset1 == INF){
									sign1 = get_Global_var(g_symt,varName1);
									fprintf(fpWrite,"mov r%d, #%d\n",x2,sign2%sign3);
									fprintf(fpWrite,"ldr r%d, .L3+%d\n",x1,sign1*4);
									fprintf(fpWrite,"str r%d, [r%d]\n",x2,x1);
									return;
								}
								else if(offset1 != INF){
									fprintf(fpWrite,"mov r%d, #%d\n",x1,sign2%sign3);
									fprintf(fpWrite,"str r%d, [fp,#%d]\n",x1,offset1);
									return;
								}
							}
							else{
								fprintf(fpWrite,"mov r0, %s\n",varName2);
							}
                                		}
                				else {
						if(offset2 == INF){
							r[x2].varName = varName2;
							sign2 = get_Global_var(g_symt,varName2);
							fprintf(fpWrite,"ldr r%d, .L3+%d\n",x1,sign2*4);
							fprintf(fpWrite,"ldr r0, [r%d]\n",x1);
						}
						else{

                				while(1){
                					if(strcmp(sym[j].name,varName2) == 0){
                							fprintf(fpWrite,"ldr r0,[fp,#%d]\n",offset2);
                							break;
                						}
                					j++;
                					}
						}
                				}

                				if(isNumber2 == 1){
                					fprintf(fpWrite,"mov r1,%s\n",varName3);
                					fprintf(fpWrite,"bl __aeabi_idivmod\n");
							if(offset1 == INF){
								fprintf(fpWrite,"ldr r%d, .L3+%d\n",x1);
								fprintf(fpWrite,"str r0, [r%d]\n",x1);
							}
							else if(offset1 != INF){
								fprintf(fpWrite,"mov r%d, r0\n",x1);
                						fprintf(fpWrite,"str r%d,[fp,#%d]\n",x1,offset1);
							}
                					return ;
                				}

                				j=0;
                				int offset3 = getOffset(sym,varName3);
						if(offset1 == INF)r[x1].varName = "null";
						if(offset2 == INF)r[x2].varName = "null";

						if(offset3 == INF){
							sign3 = get_Global_var(g_symt,varName3);
							fprintf(fpWrite,"ldr r%d, .L3+%d\n",x1,sign3*4);
							fprintf(fpWrite,"ldr r1, [r%d]\n",x1,x1);
							fprintf(fpWrite,"bl __aeabi_idivmod\n");
							if(offset1 == INF){
								fprintf(fpWrite,"ldr r%d, .L3+%d\n",x1);
								fprintf(fpWrite,"str r0,[r%d]\n",x1);
							}
							else if(offset1 !=INF){
								fprintf(fpWrite,"mov r%d,r1\n",x1);
                						fprintf(fpWrite,"str r%d,[fp,#%d]\n",x1,offset1);
							}
							return;
						}
                				while(1){
                					if(strcmp(sym[j].name,varName3) == 0){
                						fprintf(fpWrite,"ldr r0,[fp,#%d]\n",offset3);
                						fprintf(fpWrite,"bl __aeabi_idivmod\n",varName2);
                						if(offset1 == INF){
								fprintf(fpWrite,"ldr r%d, .L3+%d\n",x1);
								fprintf(fpWrite,"str r0,[r%d]\n",x1);
							}
								else if(offset1 !=INF){
								fprintf(fpWrite,"mov r%d,r0\n",x1);
                						fprintf(fpWrite,"str r%d,[fp,#%d]\n",x1,offset1);
							}
                						break;
                					}
                					j++;
                				}
                				return;
			}
			if (strcmp(temp,"mov") == 0){
				int isNumber = 0;
				int isReg1 = 0;
				int isReg2 = 0;
				int begin1;
				int end1;
				i++;
				begin = i;
				if(ch[begin] == 'r')isReg1=1;
				while(ch[i] != ',')i++;
				end = i-1;
				char* varName1 = getIden(ch,begin,end);
				i++;
				begin = i;
				if(ch[i] == '#'){isNumber = 1;begin1 = i+1;}
				if(ch[begin] == 'r')isReg2=1;
				while(ch[i] != '\n')i++;
				end1 = i-1;
				end = i-1;
				char* varName2 = getIden(ch,begin,end);
				int x1 = getReg(r,sym,varName1);
				int offset1 = getOffset(sym,varName1);
				int j = 0;
				int sign1;
				int sign2;

				if(offset1 == INF && isReg1 != 1){
					sign1 = get_Global_var(g_symt,varName1);
					r[x1].varName = varName1;
					fprintf(fpWrite,"ldr r%d, .L3+%d\n",x1,4*sign1);
				}

				int x2 = getReg(r,sym,varName2);

				if(isNumber == 1){
					sign2 = getNum(ch,begin1,end1);
					if(isReg1 == 1){
					fprintf(fpWrite,"mov %s, #%d\n",varName1,sign2);
					return;
				}
					else if(offset1 == INF){
					fprintf(fpWrite,"mov r%d, #%d\n",x2,sign2);
					fprintf(fpWrite,"str r%d, [r%d]\n",x2,x1);
					r[x1].varName = "null";
					return;
				}
					else if(offset1 != INF){
					fprintf(fpWrite,"mov r%d, #%d\n",x1,sign2);
					fprintf(fpWrite,"str r%d, [fp,#%d]\n",x1,offset1);
					return;
				}
				}
				if(isReg2 == 1){
					if(isReg1 == 1){
					fprintf(fpWrite,"mov %s, #s\n",varName1,varName2);
					return;
				}
					else if(offset1 == INF){
					fprintf(fpWrite,"str %s, [r%d]\n",varName2,x1);
					r[x1].varName = "null";
					return;
				}
					else if(offset1 !=INF){
					fprintf(fpWrite,"mov r%d, %s\n",x1,varName2);
					fprintf(fpWrite,"str r%d, [fp,#%d]\n",x1,offset1);
					return;
				}
				}

				j=0;
				int offset2 = getOffset(sym,varName2);
				r[x1].varName = "null";

				if(offset2 == INF){
					sign2 = get_Global_var(g_symt,varName2);
					fprintf(fpWrite,"ldr r%d, .L3+%d\n",x2,sign2*4);
					fprintf(fpWrite,"ldr r%d, [r%d]\n",x2,x2);

					if(isReg1 == 1){
					fprintf(fpWrite,"mov %s, r%d\n",varName1,x2);
					return;
				}
					else if(offset1 == INF){
					fprintf(fpWrite,"str r%d, [r%d]\n",x2,x1);
					r[x1].varName = "null";
					return;
				}
					else if(offset1 !=INF){
					fprintf(fpWrite,"mov r%d, %s\n",x1,varName2);
					fprintf(fpWrite,"str r%d, [fp,#%d]\n",x1,offset1);
					return;
				}
				}
				else{
					while(1){
					if(strcmp(sym[j].name,varName2) == 0){
							fprintf(fpWrite,"ldr r%d, [fp,#%d]\n",x2,offset2);
							if(isReg1 == 1){
							fprintf(fpWrite,"mov %s, r%d\n",varName1,x2);
							return;
						}
							else if(offset1 == INF){
							fprintf(fpWrite,"str r%d, [r%d]\n",x2,x1);
							return;
						}
							else if(offset1 != INF){
							fprintf(fpWrite,"str r%d, [fp,#%d]\n",x2,offset1);
							return;
						}

							break;
					}
					j++;
				}
			}
				return;

			}
			if (strcmp(temp,"movgt") == 0 || strcmp(temp,"movle") == 0){
				cond_times++;
				int isNumber = 0;
				int begin1;
				int end1;
				i++;
				begin = i;
				while(ch[i] != ',')i++;
				end = i-1;
				char* varName1 = getIden(ch,begin,end);
				i++;
				begin = i;
				if(ch[i] == '#'){isNumber = 1;begin1 = i+1;}
				while(ch[i] != '\n')i++;
				end1 = i-1;
				end = i-1;
				char* varName2 = getIden(ch,begin,end);
				int x1 = getReg(r,sym,varName1);
				int offset1 = getOffset(sym,varName1);
				int j = 0;

				while(1){
						if(strcmp(sym[j].name,varName1)==0){
								fprintf(fpWrite,"%s r%d,",temp,x1);
								break;
						}
						j++;
				}
				if(isNumber == 1){
					fprintf(fpWrite,"%s\n",varName2);
					if(cond_times == 2)
						fprintf(fpWrite,"str r%d,[fp, #%d]\n",x1,offset1);
					return;
				}
				return;
			}
			if (strcmp(temp,"movge") == 0 || strcmp(temp,"movlt") == 0){
				cond_times++;
				int isNumber = 0;
				int begin1;
				int end1;
				i++;
				begin = i;
				while(ch[i] != ',')i++;
				end = i-1;
				char* varName1 = getIden(ch,begin,end);
				i++;
				begin = i;
				if(ch[i] == '#'){isNumber = 1;begin1 = i+1;}
				while(ch[i] != '\n')i++;
				end1 = i-1;
				end = i-1;
				char* varName2 = getIden(ch,begin,end);
				int x1 = getReg(r,sym,varName1);
				int offset1 = getOffset(sym,varName1);
				int j = 0;

				while(1){
						if(strcmp(sym[j].name,varName1)==0){
								fprintf(fpWrite,"%s r%d,",temp,x1);
								break;
						}
						j++;
				}
				if(isNumber == 1){
					fprintf(fpWrite,"%s\n",varName2);
					if(cond_times == 2)
						fprintf(fpWrite,"str r%d,[fp, #%d]\n",x1,offset1);
					return;
				}
				return;
			}
			if (strcmp(temp,"moveq") == 0 || strcmp(temp,"movne") == 0){
				cond_times++;
				int isNumber = 0;
				int begin1;
				int end1;
				i++;
				begin = i;
				while(ch[i] != ',')i++;
				end = i-1;
				char* varName1 = getIden(ch,begin,end);
				i++;
				begin = i;
				if(ch[i] == '#'){isNumber = 1;begin1 = i+1;}
				while(ch[i] != '\n')i++;
				end1 = i-1;
				end = i-1;
				char* varName2 = getIden(ch,begin,end);
				int x1 = getReg(r,sym,varName1);
				int offset1 = getOffset(sym,varName1);
				int j = 0;

				while(1){
						if(strcmp(sym[j].name,varName1)==0){
								fprintf(fpWrite,"%s r%d,",temp,x1);
								break;
						}
						j++;
				}
				if(isNumber == 1){
					fprintf(fpWrite,"%s\n",varName2);
					if(cond_times == 2)
						fprintf(fpWrite,"str r%d,[fp, #%d]\n",x1,offset1);
					return;
				}
				return;
			}
			if (strcmp(temp,"init") == 0){
				int isNumber = 0;
				int isReg = 0;
				int begin1;
				int end1;
				i++;
				begin = i;
				while(ch[i] != ',')i++;
				end = i-1;
				char* varName1 = getIden(ch,begin,end);
				i++;
				begin = i;
				if(ch[i] == '#'){isNumber = 1;begin1 = i+1;}
				if(ch[begin] == 'r')isReg=1;
				while(ch[i] != '\n')i++;
				end1 = i-1;
				end = i-1;
				char* varName2 = getIden(ch,begin,end);
				int x1 = getReg(r,sym,varName1);
				
				int offset1 = getOffset(sym,varName1);
				int sign;
				int j = 0;

				while(1){
						if(strcmp(sym[j].name,varName1)==0){
							if(isNumber == 1){
							fprintf(fpWrite,"mov r%d, %s\n",x1,varName2);
							fprintf(fpWrite,"str r%d,[fp,#%d]\n",x1,offset1);
							return;
						}
							if(isReg == 1){
							fprintf(fpWrite,"mov r%d, %s\n",x1,varName2);
							fprintf(fpWrite,"str r%d,[fp,#%d]\n",x1,offset1);
							return;
						}
							r[x1].varName = varName1;
							break;
						}
						j++;
				}

				int x2 = getReg(r,sym,varName2);
				int offset2 = getOffset(sym,varName2);
				r[x1].varName = "null";
				j = 0;
				if(offset2 == INF){
					sign = get_Global_var(g_symt,varName2);
					fprintf(fpWrite,"ldr r%d, .L3+%d\n",x2,sign*4);
					fprintf(fpWrite,"ldr r%d, [r%d]\n",x2,x2);
					fprintf(fpWrite,"mov r%d, r%d\n",x1,x2);
					fprintf(fpWrite,"str r%d, [fp,#%d]\n",x1,offset1);
					return;
					
				}
				while(1){
						if(strcmp(sym[j].name,varName2)==0){
							fprintf(fpWrite,"ldr r%d, [fp,#%d]\n",x1,offset2);
							fprintf(fpWrite,"str r%d,[fp,#%d]\n",x1,offset1);
							break;
						}
						j++;
				}
				return;
			}
			if (strcmp(temp,"cmp") == 0){
				cond_times = 0;
				int isNumber1 = 0;
				int isNumber2 = 0;
				int begin1;
				int end1;
				int begin2;
				int end2;
				i++;
				begin = i;
				if(ch[i] == '#'){isNumber1 = 1;begin1 = i+1;}
				while(ch[i] != ',')i++;
				end1 = i-1;
				end = i-1;
				char* varName1 = getIden(ch,begin,end);
				i++;
				begin = i;
				if(ch[i] == '#'){isNumber2 = 1;begin2 = i+1;}
				while(ch[i] != '\n')i++;
				end2 = i-1;
				end  = i-1;
				char* varName2 = getIden(ch,begin,end);

				int offset1;
				int x1 = getReg(r,sym,varName1);
				int j = 0;
				
				int sign1;
				int sign2;

				if(isNumber1 == 1){
					sign1 = getNum(ch,begin1,end1);
					fprintf(fpWrite,"mov r%d, %s\n",x1,varName1);
					if(isNumber2 == 1){
						sign2 = getNum(ch,begin2,end2);
						fprintf(fpWrite,"cmp #%d, #%d\n",sign1,sign2);
						return;
					}
				}
				else{
				offset1 = getOffset(sym,varName1);
				r[x1].varName = varName1;
				if(offset1 == INF){
					sign1 = get_Global_var(g_symt,varName1);
					fprintf(fpWrite,"ldr r%d, .L3+%d\n",x1,sign1*4);
					fprintf(fpWrite,"ldr r%d, [r%d]\n",x1,x1);
				}
				else{
				while(1){
						if(strcmp(sym[j].name,varName1)==0){
								fprintf(fpWrite,"ldr r%d,[fp,#%d]\n",x1,offset1);
								break;
						}
						j++;
				}
				}
				}

				if(isNumber2 == 1){
					fprintf(fpWrite,"cmp r%d, %s\n",x1,varName2);
					if(offset1 == INF)r[x1].varName = "null";
					r[x1].varName = "null";
					return;
				}

				j = 0;
				int x2 = getReg(r,sym,varName2);
				r[x1].varName = "null";

				int offset2 = getOffset(sym,varName2);
				if(offset2 == INF){
					sign2 = get_Global_var(g_symt,varName2);
					fprintf(fpWrite,"ldr r%d, .L3+%d\n",x2,sign2*4);
					fprintf(fpWrite,"ldr r%d, [r%d]\n",x2,x2);
					fprintf(fpWrite,"cmp r%d, r%d\n",x1,x2);
					return;
				}
				while(1){
						if(strcmp(sym[j].name,varName2)==0){
							fprintf(fpWrite,"ldr r%d,[fp,#%d]\n",x2,offset2);
							fprintf(fpWrite,"cmp r%d,r%d\n",x1,x2);
							break;
						}
						j++;
				}
				return;
			}
		}
		i++;
    }
    return;
}

void prepoccessor(){
    /*预处理子程序*/
	while(!feof(fpRead)) {

		fgets(c_buffer[line],sizeof(c_buffer[line]), fpRead);

		//puts(c_buffer[line]);    //输出字符二维数组的每一行

		line++;
	}
	strcat(c_buffer[line-1], "\n");
};
