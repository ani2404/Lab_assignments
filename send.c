/* send.c - send */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  send  -  Pass a message to a process and start recipient if waiting
 *------------------------------------------------------------------------
 */
syscall	send(
	  pid32		pid,		/* ID of recipient process	*/
	  umsg32	msg		/* Contents of message		*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	procent *prptr;		/* Ptr to process's table entry	*/

	mask = disable();
	if (isbadpid(pid)) {
		restore(mask);
		return SYSERR;
	}

	prptr = &proctab[pid];
	if (prptr->prhasmsg) {
		restore(mask);
		return SYSERR;
	}
	prptr->prmsg = msg;		/* Deliver message		*/
	prptr->prhasmsg = TRUE;		/* Indicate message is waiting	*/

	/* If recipient waiting or in timed-wait make it ready */

	if (prptr->prstate == PR_RECV) {
		ready(pid);
	} else if (prptr->prstate == PR_RECTIM) {
		unsleep(pid);
		ready(pid);
	}
	restore(mask);		/* Restore interrupts */
	return OK;
}

syscall sendMsg(pid32 pid, umsg32 msg)
{
	intmask mask;
	struct procent *prptr;
	
	mask = disable();
	if(isbadpid(pid)) {
		restore(mask);
		return SYSERR;
	}
	
	prptr = &proctab[pid];
	
	if(prptr->qptr->head != prptr->qptr->tail)
	{
		//Queue is not full
		uint32 tail = prptr->qptr->tail;		
		prptr->qptr->msgq[tail] = msg; // Queue the message		
		
		if(prptr->qptr->head == MAX_MSGS)
		{
			// Empty queue is now filled
			//Update the head index to point to the tail
			prptr->qptr->head = tail;
		}
		prptr->qptr->tail = (++tail % MAX_MSGS); // Update the tail index
	}
	else
	{
		kprintf("Process[%d]: Unable to send message as Receiver[%d] queue is full \n",getpid(),pid);
		restore(mask);
		return SYSERR;
	}
		
	
	if(prptr->qptr->count > 0)
	{
		// Receiver Process is waiting for messages to be queued 
		prptr->qptr->count -= 1; // Decrement by 1 as one message is queued
		if(prptr->qptr->count == 0)
			ready(pid); // Ready the process as expected count is acheived
	}
	restore(mask);
	return OK;
}


uint32 sendMsgs(pid32 pid, umsg32* msgs, uint32 msg_count)
{
	intmask mask;
	struct procent *prptr;
	int loop_index=0;
	uint32 tail;
	
	mask = disable();
	if(isbadpid(pid)) {
		restore(mask);
		return SYSERR;
	}
	
	prptr = &proctab[pid];
	tail = prptr->qptr->tail;
	for(;(loop_index < msg_count) && (loop_index < MAX_MSGS); loop_index++)
	{ 
		if(prptr->qptr->head != tail)
		{
			//Queue is not full
			prptr->qptr->msgq[tail] = msgs[loop_index]; // Queue the message
			tail = ((++tail)%MAX_MSGS);
		}
		else
		{
			//kprintf("Process[%d]: Unable to send messages as Receiver[%d] queue is full \n",getpid(),pid);
			break;
		}
	}
	
	if(prptr->qptr->head == MAX_MSGS)
	{
		// Empty queue is now filled
		//Update the head index to point to the tail
		prptr->qptr->head = prptr->qptr->tail;
	}
	
	prptr->qptr->tail = tail; // Update the tail index
	
	if(prptr->qptr->count > 0)
	{
		// Receiver Process is waiting for messages to be queued 
		prptr->qptr->count -= loop_index; // Decrement by successfully queued messages
		if(((int)prptr->qptr->count) <= 0){
			prptr->qptr->count = 0;
			ready(pid); // Ready the process as expected count is acheived
		}		
	}
	restore(mask);
	return loop_index;
	
}


uint32 sendnMsg(uint32 pid_count, pid32* pids, umsg32 msg)
{
	intmask mask;
	struct procent *prptr;
	int loop_index=0;
	uint32 success = 0;
	pid32 pid;
	
	mask = disable();
	
	resched_cntl(DEFER_START);
	for(;(loop_index < pid_count) && (loop_index < N_PROC); loop_index++)
	{
		pid = pids[loop_index];
		if(!isbadpid(pid))
		{
			prptr = &proctab[pid];
			uint32 tail = prptr->qptr->tail;
			if(prptr->qptr->head != tail)
			{
				//Queue is not full
				prptr->qptr->msgq[tail] = msg; // Queue the message
				if(prptr->qptr->head == MAX_MSGS) {
					// Empty queue is now filled
					//Update the head index to point to the tail
					prptr->qptr->head = prptr->qptr->tail;
				}
				prptr->qptr->tail = ((++tail)%MAX_MSGS);
				
				if(prptr->qptr->count > 0){
					// Receiver Process is waiting for messages to be queued 
					prptr->qptr->count -= 1; // Decrement by 1 as one message is queued
					if(prptr->qptr->count == 0)
						ready(pid); // Ready the process as expected count is acheived
				}	
				success++;
			}
			//else
			//{
			//kprintf("Process[%d]: Unable to send messages as Receiver[%d] queue is full \n",getpid(),pid);
			//}
		}
	}
	
	resched_cntl(DEFER_STOP);
	restore(mask);
	return success;	
}
