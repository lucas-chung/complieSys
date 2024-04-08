#include <stdio.h>
#include <string.h>
char buffers[200][150];
int front = 0;
int rear = 0;
void enQueue(char* data){
    strcpy(buffers[rear], data);
    rear++;
} 
char* deQueue(){
	if (front != rear) 
	{
		return buffers[front++];
	  }  
	  else
	  {
	  	return NULL;
	  }
}
void cl_queue()
{
	front = 0;
	rear = 0;
}
int q_isEmpty()
{
	return (front == rear);
}
