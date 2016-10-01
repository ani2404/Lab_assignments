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
	
	if(prptr->prmsgsptr[HEAD] != prptr->prmsgsptr[TAIL])
	{
		//Queue is not full
		uint32 tail = prptr->prmsgsptr[TAIL];		
		prptr->prmsgsptr[QUEUE_START + tail] = msg; // Queue the message		
		
		if(prptr->prmsgsptr[HEAD] == MAX_MSGS)
		{
			// Empty queue is now filled
			//Update the head index to point to the tail
			prptr->prmsgsptr[HEAD] = tail;
		}
		prptr->prmsgsptr[TAIL] = (++tail % MAX_MSGS); // Update the tail index
	}
	else
	{
		kprintf("Process[%d]: Unable to send message as Receiver[%d] queue is full \n",getpid(),pid);
		restore(mask);
		return SYSERR;
	}
		
	
	if(prptr->prstate == PR_RECQ)
	{
		ready(pid);
	}
	restore(mask);
	return OK;
}


uint32 sendMsgs(pid32, umsg32*, uint32)
{
	
}


uint32 sendnMsg(uint32, pid32*, umsg32)
{
	
}
