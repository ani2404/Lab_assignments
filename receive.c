/* receive.c - receive */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  receive  -  Wait for a message and return the message to the caller
 *------------------------------------------------------------------------
 */
umsg32	receive(void)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	procent *prptr;		/* Ptr to process's table entry	*/
	umsg32	msg;			/* Message to return		*/

	mask = disable();
	prptr = &proctab[currpid];
	if (prptr->prhasmsg == FALSE) {
		prptr->prstate = PR_RECV;
		resched();		/* Block until message arrives	*/
	}
	msg = prptr->prmsg;		/* Retrieve message		*/
	prptr->prhasmsg = FALSE;	/* Reset message flag		*/
	restore(mask);
	return msg;
}


umsg32 receiveMsg(void)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	procent *prptr;		/* Ptr to process's table entry	*/
	umsg32	msg;			/* Message to return		*/
	uint32 head;
	
	mask = disable();
	prptr = &proctab[currpid];
	
	if (prptr->qptr->head == MAX_MSGS) {
		// Queue is Empty
		prptr->prstate = PR_RECQ;
		prptr->qptr->count = 1;
		resched();		/* Block until message arrives	*/
	}
	
	head = prptr->qptr->head;
	msg = prptr->qptr->msgq[head];		/* Retrieve message		*/
	prptr->qptr->head = (++head % MAX_MSGS);
	if(prptr->qptr->head == prptr->qptr->tail){
		// Queue is now empty
		prptr->qptr->head = MAX_MSGS;
	}
	restore(mask);
	return msg;
	
	
}


syscall receiveMsgs(umsg32* msgs, uint32 msg_count)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	procent *prptr;		/* Ptr to process's table entry	*/
	uint32	loop_index = 0;
	
	mask = disable();
	prptr = &proctab[currpid];
	uint32 &head = prptr->qptr->head;
	uint32 &tail = prptr->qptr->tail;
	if ((head == MAX_MSGS) || ((head != tail) && (((tail - head + MAX_MSGS)%MAX_MSGS) < msg_count))) {
		// Queue is Empty or not sufficient to conusme [msg_count] messgs
		prptr->prstate = PR_RECQ;
		prptr->qptr->count = msg_count;
		resched();		/* Block until message arrives	*/
	}
	
	//head = prptr->qptr->head;	
	for(;loop_index < msg_count; loop_index++)
	{
		msgs[loop_index] = prptr->qptr->msgq[head++];		/* Retrieve message		*/
		head %= MAX_MSGS;
	}
	
	//prptr->qptr->head = head;
	
	if(head == prptr->qptr->tail){
		// Queue is now empty
		prptr->qptr->head = MAX_MSGS;
	}
	
	restore(mask);
	return OK;
	
}
