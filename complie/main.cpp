#include <stdio.h>
#include "gen.cpp"

int main(int argc, char** argv)
{
	extern FILE *yyin, *yyout;	
	argc--; argv++;		
	if (argc > 0 && **argv == '-' && (*argv)[1] == 'S')
	{
		argc--;argv++;
		if (argc > 0 && **argv == '-' && (*argv)[1] == 'o')
		{
			argc--;argv++;	
			fpWrite = fopen(*argv, "w+");
			if (argc > 0)
			{
				fpWrite = fopen(*argv, "w+");
							
			}
			
		}
		else
		{
			fpWrite = fopen("a.s", "w+");
			
		}
		argc--;argv++;
		if (argc > 0)
		{
			yyin = fopen(*argv, "r");
		}
		
		
	}
	
		
	InitList_L(begin_entry);
	InitList_L(true_list);
	InitList_L(false_list);
	InitList_L(begin_list);
	InitList_L(bk_list);
	InitList_L(ct_list);

	InitList_L(true_entry);
	InitList_L(false_entry);
	InitList_L(next_entry);
	InitList_L(M_list);	
	
	yyout = fopen("ir.txt", "w+");
	
	yyparse();
	/*
	//check symt_table
	int i1, i2;
	for (i1 = 0; i1 < symt_position; i1++)
	{
		printf("%s:\n", symt[i1].f_name);		
		for (i2 = 0; i2 < size[i1]; i2++)
		{
			printf("%s %d\n", symt[i1].syms[i2].name, symt[i1].syms[i2].offset);		
		}
	}
	
	*/
	fclose(yyout);
	fpRead = fopen("ir.txt", "r");
	
	prepoccessor();
	define_g_var(g_symt);
	
	int i = 0;
	int j;
		    	
	while(i < line)
	{
		j = 0;
		int isLabel = 0;
		while(c_buffer[i][j] != '\0')
		{
			if(c_buffer[i][j] == '.' && j == 0)
			{
				//如果开头为.  则为跳转标签
				int k = j;
				while(c_buffer[i][k] != '\n'){
				fprintf(fpWrite,"%c",c_buffer[i][k]);
				k++;
				}
				fprintf(fpWrite,":\n");
				isLabel = 1;
				break;
			}
			 if(c_buffer[i][j] == ':')
			{
				//如果有:，则为函数名标签
				end = j-1;
				char *temp = getIden(c_buffer[i],begin,end);
				int t = sc_fun_position(temp);
				sym = symt[t].syms;
				sym_size = size[t];
				t++;
				
				fprintf(fpWrite,"%s\n",".text");
				
				fprintf(fpWrite,"%s\n",".align 2");
				
				fprintf(fpWrite,".global %s\n",temp);
				fprintf(fpWrite,"%s\n",".syntax unified");
				fprintf(fpWrite,"%s\n",".arm");
				fprintf(fpWrite,"%s\n",".fpu vfp");
				fprintf(fpWrite,".type %s,%%function\n",temp);
				fprintf(fpWrite,"%s:\n",temp);
				
				isLabel = 1;
				
				break;
			}
			j++;
		}
		
		if(isLabel == 0)
		//当不是标签的时候才进行代码生成
		{			
			genAss(c_buffer[i],r,sym);
		}
		
		i++;
    	}
	print_gsymt(g_symt);
	
	
	return 0;
}
