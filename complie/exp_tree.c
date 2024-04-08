#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <ctype.h>
#include "queue.c"
#include "symt.c"

int times = 0;/*该变量用于记录此时规约到第几个M*/
int label_num = 0;/*该变量记录此时已经用了几个标签*/
int is_end = 0;
char t_temp[200];
enum operators{ADD = 1, SUB, MUL, DIV, MOD, NOT, SOR, SAND, SEQ, SNEQ, SGE, SLE, SGT, SLS}; 
typedef struct LNode{
	int label;
	struct LNode *next;
}LNode,*LinkList;

LinkList true_list;
LinkList false_list;
LinkList begin_list;
LinkList M_list;
LinkList bk_list;
LinkList ct_list;

LinkList true_entry;
LinkList false_entry;
LinkList next_entry;
LinkList begin_entry;


int InitList_L(LinkList &L){
    //单链表的初始化,每个链表都有一个头结点不保存任何内容	
	if (L == NULL)
	{	
		L = (LinkList)malloc(sizeof(LNode));
		L->next=NULL;
		return 1;
	}
	else
		return 0;
}

int Insert_LI(int val, LinkList &L)
{	
	LinkList t = (LinkList)malloc(sizeof(LNode));
	t -> label = val;
	t->next = L->next;
	L->next = t;
}

int Insert_L(LinkList &new_node,LinkList &L){
    /*尾插法插入新节点*/
    new_node->next = L->next;
	L->next = new_node;
    return 1;
}

int delete_L(LinkList &L){
    /*删除头结点后的第一个节点,并且返回该节点的label值,可能是中间代码的行数,也可能是标签值*/	
	int i;
	LinkList temp;
	temp = L->next;
	if(!temp){return -1;}
	i = temp->label;
	L->next= temp->next;
	free(temp);
	return i;
}

char *t_buffer[200];
int t_buffer_counter = 0;
int t_counter = 0;
int isOperator(char* c)
{
	int i;
	char symbol[][3] = {"", "+", "-", "*" , "/", "%", "!", "||", "&&", "==", "!=", ">=", "<=", ">", "<"};
	for (i = 1; i< sizeof symbol; i++)
	{
		if (strcmp(c, symbol[i]) == 0) 
		{
			return i;
		} 
	}
	return 0;
}
int isFunC(char* c)
{
	int p1 = 0, p2 = 0;
	
	
	p1 = strchr(c, '(') - c;
	p2 = strrchr(c, ')') - c;
	if (p1 < p2)
	{
		return p1;
	}
	else
	{
		return 0;
	}
}
int isDigit(char* c_ptr)
{
	int i = 0;
	int len = strlen(c_ptr);
	if (c_ptr[0] == '-')
	{
		i++;
	}
	for (; i < len; i++)
	{
		if (!isdigit(c_ptr[i]))
		{
			return 0;
		}
	}
	return 1;
}
int isUOperator(char* c)			//unary operator 
{
	int i;
	char symbol[][3] = {"", "+", "-", "!"};
	for (i = 1; i< sizeof symbol; i++)
	{
		if (strcmp(c, symbol[i]) == 0) 
		{
			return 1;
		} 
	}
	return 0;
}
 void addNS(char *str)
{
	char* temp;
	temp = strdup(str);
	if (isDigit(str))
	{
		snprintf(str, sizeof(str), "#%s", temp);
	}
}
typedef struct Node
{
	char op[200];
	struct Node* op1;
	struct Node* op2;
 }Node;

Node* newNode_i(int i)
 {
 	Node* anode;
	anode = (Node *)malloc(sizeof(Node));
 	snprintf(anode->op, sizeof(anode->op), "%d", i);
	anode->op1 = NULL;
 	anode->op2 = NULL;
	return anode;
 }

 Node* newNode_c(char* name)
 {
 	Node* anode = (Node *)malloc(sizeof(Node));
	//anode->op = strdup(name);
	strcpy(anode->op, name);
	anode->op1 = NULL;
 	anode->op2 = NULL;
	return anode;
 }

 Node* link(char* c, Node* op1, Node* op2)
 {
 	Node* anode = (Node *)malloc(sizeof(Node));
 	strcpy(anode->op, c);
	anode->op1 = op1;
 	anode->op2 = op2;
	return anode;
 }

 void rp_t(int c_position, int t_counter)
 {
 	int i;
	snprintf(t_buffer[c_position-2], sizeof(t_buffer[c_position]), "T%d", t_counter);
 	for(i = c_position+1; i < t_buffer_counter; i++)
	 {
	 	strcpy(t_buffer[i-2], t_buffer[i]);
	  }
	  t_buffer_counter -= 2;
	  t_buffer[i-2] = strdup ("");
 }

void cl_t_buffer()
{
	int i;
	for (i = 0; i < t_buffer_counter; i++)
	{
		strcpy(t_buffer[i], "");
	}
	t_buffer_counter = 0;
}
  void rp_t1(int c_position, int t_counter)
 {
 	int i;
	snprintf(t_buffer[c_position-1], sizeof(t_buffer[c_position]), "T%d", t_counter);
 	for(i = c_position+1; i < t_buffer_counter; i++)
	 {
	 	strcpy(t_buffer[i-1], t_buffer[i]);
	  } 
	  t_buffer_counter -= 1;
	  t_buffer[i-1] = strdup ("");
 }
 void rp_i(int c_position, int temp)
 {
 	int i;
	 snprintf(t_buffer[c_position-2], sizeof(t_buffer[c_position-2]), "%d", temp);
 	for (i = c_position+1; i < t_buffer_counter; i++)
	 {
		strcpy(t_buffer[i-2], t_buffer[i]);
	  }
	  t_buffer_counter -= 2;
 }
void rp_i1(int c_position, int temp)
 {
 	int i;
	 snprintf(t_buffer[c_position-1], sizeof(t_buffer[c_position-1]), "%d", temp);
 	for (i = c_position+1; i < t_buffer_counter; i++)
	 {
		strcpy(t_buffer[i-1], t_buffer[i]);
	  } 
	  t_buffer_counter -= 1;
 }
  void simp_t_buffer(char** t_buffer)
 {
 	int i;
 	int temp;
 	for (i = 0; i < t_buffer_counter; i++)
 	{
 		if (isOperator(t_buffer[i]))
 		{
 			if (isDigit(t_buffer[i-1]) && (strcmp(t_buffer[i], "!") == 0)) 
 			{
 				temp = atoi(t_buffer[i-1]);
 				rp_i1(i, temp);
			 }
			 if (isDigit(t_buffer[i-1]) && isDigit(t_buffer[i-2]))
 			{
 				switch(isOperator(t_buffer[i]))
 				{
 					case ADD:	temp = atoi(t_buffer[i-2]) + atoi(t_buffer[i-1]) ;
 								rp_i(i, temp);
								 break;
 					case SUB:	temp = atoi(t_buffer[i-2]) - atoi(t_buffer[i-1]) ;
 								rp_i(i, temp);
 								break;
 					case MUL:	temp = atoi(t_buffer[i-2]) * atoi(t_buffer[i-1]) ;
 								rp_i(i, temp);
								 break;
 					case DIV:	temp = atoi(t_buffer[i-2]) / atoi(t_buffer[i-1]) ;
 								rp_i(i, temp);
								 break;
					case MOD:	temp = atoi(t_buffer[i-2]) % atoi(t_buffer[i-1]) ;
 								rp_i(i, temp);
								 break;
					case SOR:	temp = atoi(t_buffer[i-2]) || atoi(t_buffer[i-1]) ;
 								rp_i(i, temp);
								 break;
					case SAND:	temp = atoi(t_buffer[i-2]) && atoi(t_buffer[i-1]) ;
 								rp_i(i, temp);
								 break;
					case SEQ:	temp = atoi(t_buffer[i-2]) == atoi(t_buffer[i-1]) ;
 								rp_i(i, temp);
								 break;
					case SNEQ:	temp = atoi(t_buffer[i-2]) != atoi(t_buffer[i-1]) ;
 								rp_i(i, temp);
								 break;
					case SGE:	temp = atoi(t_buffer[i-2]) >= atoi(t_buffer[i-1]) ;
 								rp_i(i, temp);
								 break;
					case SLE:	temp = atoi(t_buffer[i-2]) <= atoi(t_buffer[i-1]) ;
 								rp_i(i, temp);
								 break;
					case SGT:	temp = atoi(t_buffer[i-2]) > atoi(t_buffer[i-1]) ;
 								rp_i(i, temp);
								 break;
					case SLS:	temp = atoi(t_buffer[i-2]) < atoi(t_buffer[i-1]) ;
 								rp_i(i, temp);
								 break;			 			 			 			 
				 }
				 i = 0;
				 
			 }
		 }
	 }
	 //printf("%s!!!\n", t_buffer[0]);
 }

 void print(Node* root)
 {
 	if (root != NULL)
 	{
 		
 		print(root->op1);

 		print(root->op2);
		printf("%s ", root->op);

	 }
 }
void tree_to_s(Node* root)
 {
	 if(root != NULL)
 	{
 		tree_to_s(root->op1);
 		tree_to_s(root->op2);
 		t_buffer[t_buffer_counter++] =  strdup(root->op);
		 //strcpy(t_buffer[t_buffer_counter++], strdup(root->op));
	 }
 }

 void s_to_ins(char** buffer)
 {
 	int i;
 	int tmp_len;
 	char I_buffer[200];
 	simp_t_buffer(buffer);
 	for (i = 0; i < t_buffer_counter; i++)
 	{
		if ((tmp_len = isFunC(buffer[i])))
		{
			t_counter++;
			buffer[i][tmp_len] = '\0';
			snprintf(I_buffer, sizeof(I_buffer), "bl %s\nmov T%d,r0\n", buffer[i], t_counter);
			enQueue(I_buffer);
			snprintf(buffer[i], sizeof(buffer[i]), "T%d", t_counter);
			
			i = 0;
			snprintf(t_temp, sizeof(t_temp), "T%d", t_counter);
			symt[symt_position].syms[s_counter].name = strdup(t_temp);
			s_counter++;
			 continue;
		}

		 if (isOperator(buffer[i]))
 		{
			
			if (isDigit(buffer[i-1]))
			{
				snprintf(I_buffer, sizeof(I_buffer), "#%s", buffer[i-1]);
				strcpy(buffer[i-1], I_buffer);
				//printf("%s!\n", buffer[i-1]);
			}
			if (strcmp(buffer[i], "!") != 0 && isDigit(buffer[i-2]))
			{
				snprintf(I_buffer, sizeof(I_buffer), "#%s", buffer[i-2]);
				strcpy(buffer[i-2], I_buffer);
			}			
			 switch(isOperator(buffer[i]))
 			{
				//printf("%s %s!\n", buffer[i-1], buffer[i-2]);
				
				case ADD: 		
							t_counter++;
				 			snprintf(I_buffer, sizeof(I_buffer), "add T%d,%s,%s\n",t_counter, buffer[i-2], buffer[i-1]);
 							rp_t(i, t_counter);
 							i = 0;
							enQueue(I_buffer);
 							break;
 				case SUB:		t_counter++;
				 			snprintf(I_buffer, sizeof(I_buffer), "sub T%d,%s,%s\n",t_counter, buffer[i-2], buffer[i-1]);
 							rp_t(i, t_counter);
 							i = 0;
							enQueue(I_buffer);
 							break;
 				case MUL: 		t_counter++;
				 			snprintf(I_buffer, sizeof(I_buffer), "mul T%d,%s,%s\n",t_counter, buffer[i-2], buffer[i-1]);
 							rp_t(i, t_counter);
 							i = 0;
							enQueue(I_buffer);
 							break;
 				case DIV: 		t_counter++;
				 			snprintf(I_buffer, sizeof(I_buffer), "div T%d,%s,%s\n",t_counter, buffer[i-2], buffer[i-1]);
 							rp_t(i, t_counter);
 							i = 0;
							enQueue(I_buffer);
 							break;
 				case MOD: 		t_counter++;
				 			snprintf(I_buffer, sizeof(I_buffer), "mod T%d,%s,%s\n",t_counter, buffer[i-2], buffer[i-1]);
 							rp_t(i, t_counter);
 							i = 0;
							enQueue(I_buffer);
 							break;
				case NOT:		t_counter++;
							snprintf(I_buffer, sizeof(I_buffer), "cmp %s,#0\nmoveq T%d,#1\nmovne T%d,#0\n", buffer[i-1], t_counter, t_counter);			
 							rp_t1(i, t_counter);
 							i = 0;
							enQueue(I_buffer);
 							break;
				case SOR:		t_counter++;
				 			snprintf(I_buffer, sizeof(I_buffer), "or T%d %s %s\n",t_counter, buffer[i-2], buffer[i-1]);
 							rp_t(i, t_counter);
 							i = 0;
							enQueue(I_buffer);
 							break;
				case SAND:		t_counter++;
				 			snprintf(I_buffer, sizeof(I_buffer), "and T%d,%s,%s\n",t_counter, buffer[i-2], buffer[i-1]);
 							rp_t(i, t_counter);
 							i = 0;
							enQueue(I_buffer);
 							break;
 				case SEQ:		t_counter++;
				 			snprintf(I_buffer, sizeof(I_buffer), "cmp %s,%s\nmoveq T%d,#1\nmovne T%d,#0\n", buffer[i-2], buffer[i-1], t_counter, t_counter);
 							rp_t(i, t_counter);
 							i = 0;
							enQueue(I_buffer);
 							break;	
				case SNEQ:		t_counter++;
				 			snprintf(I_buffer, sizeof(I_buffer), "CMP %s,%s\nmovne T%d,#1\nmoveq T%d,#0\n", buffer[i-2], buffer[i-1], t_counter, t_counter);
 							rp_t(i, t_counter);
 							i = 0;
							enQueue(I_buffer);
 							break;
				case SGE:		t_counter++;
				 			snprintf(I_buffer, sizeof(I_buffer), "cmp %s,%s\nmovge T%d,#1\nmovlt T%d,#0\n", buffer[i-2], buffer[i-1], t_counter, t_counter);
 							rp_t(i, t_counter);
 							i = 0;
							enQueue(I_buffer);
 							break;
				case SLE:		t_counter++;
				 			snprintf(I_buffer, sizeof(I_buffer), "cmp %s,%s\nmovle T%d,#1\nmovgt T%d,#0\n", buffer[i-2], buffer[i-1], t_counter, t_counter);
 							rp_t(i, t_counter);
 							i = 0;
							enQueue(I_buffer);
 							break;;
				case SGT:		t_counter++;
				 			snprintf(I_buffer, sizeof(I_buffer), "cmp %s,%s\nmovgt T%d,#1\nmovle T%d,#0\n", buffer[i-2], buffer[i-1], t_counter, t_counter);
 							rp_t(i, t_counter);
 							i = 0;
							enQueue(I_buffer);
 							break;
				case SLS:		t_counter++;
				 			snprintf(I_buffer, sizeof(I_buffer), "cmp %s,%s\nmovlt T%d,#1\nmovge T%d,#0\n", buffer[i-2], buffer[i-1], t_counter, t_counter);
 							rp_t(i, t_counter);
 							i = 0;
							enQueue(I_buffer);
 							break;
			 }
			snprintf(t_temp, sizeof(t_temp), "T%d", t_counter);
			symt[symt_position].syms[s_counter].name = strdup(t_temp);
			s_counter++;
			 continue;
		 }

	 }
		
 }

void store_I(Node* anode)
{
	cl_t_buffer();
	tree_to_s(anode);
	s_to_ins(t_buffer);
}

