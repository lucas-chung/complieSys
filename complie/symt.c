//symt.c
#include <string.h>
#define DECL_SIZE 200
#define SYMT_SIZE 200
struct decl{
	int offset;
	int val;
	char* name;
	int pos;
}decl[DECL_SIZE];
//符号表定义
struct sym_table{
	struct decl syms[DECL_SIZE];
	char* f_name;
}g_symt, symt[SYMT_SIZE];// 前者为全局变量表，后者为局部变量表
int decl_position = 0;	
//decl是一个存储函数内声明的一个数组，decl_position用于指示当前将要写入的位置
int g_symt_position = 0;
//g_symt_position用于指示当前将要写入的全局变量表位置
int symt_position = 0;//局部变量符号表的位置
int stack_offset = 0;//局部变量在栈中的偏移量
char buffer[200], temp[150];//随意使用
int size[200];	//存储各函数所使用变量的个数
int s_counter = 0;	//当前该函数所使用变量的个数
//清空decl数组
void cl_decl()
{
	int i;
	for(i = 0; i < DECL_SIZE; i++)
	{ 
		decl[i].val = 0;
		strcpy(decl[i].name, "");
	}
	decl_position = 0;
}

//
void add_t_decl(char* temp_name)
{
	decl[decl_position].name = strdup(temp_name);
	decl_position++;
	size[symt_position] = decl_position;
}

//存储decl
void store_decl(struct decl input)
{
	decl[decl_position].val = input.val;
	decl[decl_position].name = strdup(input.name);
	decl_position++;
	size[symt_position] = decl_position;	
	//cl_decl();
	//decl_position = 0;
}

//设置全局变量符号表
void set_g_symt(struct decl input)
{
	g_symt.f_name = strdup("global");
	g_symt.syms[g_symt_position].val = input.val;
	g_symt.syms[g_symt_position].name = strdup(input.name);
	g_symt_position++;
}
//存储变量
void set_decl_c(struct decl* adecl, char* name)
{
	adecl->offset = 0;
	adecl->name = strdup(name);
}
//存储变量及数值
void set_decl_ci(struct decl* adecl, char* name, int val)
{
	adecl->offset = 0;
	adecl->name = strdup(name);
	adecl->val = val;
}
//设置局部变量表
void set_symt(char* f_name)
{
	int i;	
	symt[symt_position].f_name = strdup(f_name);	
	//printf("symt_postion = %d:\n", symt_position);
	
	for (i = 0;i < s_counter; i++)
	{
		symt[symt_position].syms[i].offset = -1*(i+2)*sizeof(int) ;
		symt[symt_position].syms[i].pos = 16;
	}

	
	size[symt_position] = s_counter;	
	symt_position++;
	s_counter = 0;	
	//decl_position = 0;

}
//清空当前函数的符号表
void cl_symt()
{
	int i;	
	symt[symt_position].f_name = strdup("");	
	for (i = 0;i < decl_position; i++)
	{
		symt[symt_position].syms[i].val = 0;
		symt[symt_position].syms[i].offset = 0;
		symt[symt_position].syms[i].name = strdup("");
	}
	size[symt_position] = 0;	
	symt_position--;
}

void decl_assign(struct decl *decla, struct decl declb)
{
	decla->offset = declb.offset;
	decla->val = declb.val;
	decla->name = strdup(declb.name);
}
//search for fun_position
int sc_fun_position(char* name)
{
	int i;
	for (i = symt_position - 1; i >= 0; i--)
	{
		if (strcmp(symt[i].f_name, name) == 0)
		{
			return i;		
		}
	}
	return -1;
}
//search for var'Initial val
int sc_var_val(char* name)
{
	int i;	
	for(i = 0; i < size[symt_position]; i++)
	{
		if (strcmp(symt[symt_position].syms[i].name, name) == 0)
		{
			return symt[symt_position].syms[i].val;
		}
	}
	if (i == decl_position)
	{
		for (i = 0; i < g_symt_position; i++)
		{
			if (strcmp(g_symt.syms[i].name, name) == 0)
				return 	g_symt.syms[i].val;		
		}
	}
	return 0;
}

//set the syms_table
void set_t(char* name, int val)
{
	int i;	
	for(i = 0; i < size[symt_position]; i++)
	{
		if (strcmp(symt[symt_position].syms[i].name, name) == 0)
		{
			symt[symt_position].syms[i].val = val;
		}
	}
	if (i == decl_position)
	{
		for (i = 0; i < g_symt_position; i++)
		{
			if (strcmp(g_symt.syms[i].name, name) == 0)
				g_symt.syms[i].val = val;		
		}
	}
}
