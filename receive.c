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
	
	mask = disable();
	prptr = &proctab[currpid];

	if (prptr->qptr->head == MAX_MSGS) {
		// Queue is Empty
		prptr->prstate = PR_RECQ;
		prptr->qptr->count = 1;
		resched();		/* Block until message arrives	*/
	}
	
	
	msg = prptr->qptr->msgq[prptr->qptr->head++];		/* Retrieve message		*/
	prptr->qptr->head %= MAX_MSGS;
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

	if ((prptr->qptr->head == MAX_MSGS) || ((prptr->qptr->head != prptr->qptr->tail) &&
						(((prptr->qptr->head - prptr->qptr->tail + MAX_MSGS)%MAX_MSGS) < msg_count))) {
		// Queue is Empty or not sufficient to conusme [msg_count] messgs
		prptr->prstate = PR_RECQ;
		prptr->qptr->count = msg_count;
		resched();		/* Block until message arrives	*/
	}
	
	for(;loop_index < msg_count; loop_index++)
	{
		msgs[loop_index] = prptr->qptr->msgq[prptr->qptr->head++];		/* Retrieve message		*/
		prptr->qptr->head %= MAX_MSGS;
	}
	
	if(prptr->qptr->head == prptr->qptr->tail){
		// Queue is now empty
		prptr->qptr->head = MAX_MSGS;
	}
	
	restore(mask);
	return OK;
	
}
