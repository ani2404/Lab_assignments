/*  main.c  - main */

#include <xinu.h>

process	main(void)
{

	kprintf("Size of process entry table is %d \n",sizeof(struct procent));

	
	return OK;
    
}
