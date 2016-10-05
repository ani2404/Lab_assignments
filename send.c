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
	uint32 head,tail;
	
	mask = disable();
	if(isbadpid(pid)) {
		restore(mask);
		return SYSERR;
	}
	
	prptr = &proctab[pid];
	head = prptr->qptr->head;
	tail = prptr->qptr->tail;
	
	if(head == tail){
		restore(mask);
		return SYSERR;
	}
	
	//Queue is not full		
	prptr->qptr->msgq[tail] = msg; // Queue the message		
		
	if(head == MAX_MSGS)
	{
		
		// Empty queue is now filled
		//Update the head index to point to the tail
		head = tail;
	}
	tail++;

	tail %= MAX_MSGS; // Update the tail index
	
	prptr->qptr->head = head;
	prptr->qptr->tail = tail;
	
	if(prptr->prstate == PR_RECQ && ((tail==head) || (((tail - head + MAX_MSGS)%MAX_MSGS) >= prptr->qptr->count)))
	{ 
 		// Ready the process as expected count is acheived
		prptr->qptr->count = 0;
		ready(pid);
	}
	restore(mask);
	return OK;
}


uint32 sendMsgs(pid32 pid, umsg32* msgs, uint32 msg_count)
{
	intmask mask;
	struct procent *prptr;
	uint32 head,tail,loop_index=0;
	
	mask = disable();
	if(isbadpid(pid)) {
		restore(mask);
		return SYSERR;
	}
	
	prptr = &proctab[pid];
	tail = prptr->qptr->tail;
	head = prptr->qptr->head;
	for(;(loop_index < msg_count) && (loop_index < MAX_MSGS); loop_index++)
	{ 
		if(head != tail)
		{
			//Queue is not full
			prptr->qptr->msgq[tail++] = msgs[loop_index]; // Queue the message
			tail %= MAX_MSGS;
		}
		else
		{
			break;
		}
	}
	
	if(head == MAX_MSGS)
	{
		// Empty queue is now filled
		//Update the head index to point to the tail
		head = prptr->qptr->tail;
	}
	
	prptr->qptr->head = head;
	prptr->qptr->tail = tail; // Update the tail index
	
	if(prptr->prstate == PR_RECQ && ((tail == head) || (((tail - head + MAX_MSGS)%MAX_MSGS) >= prptr->qptr->count)))
	{ 
		// Ready the process as expected count is acheived
		prptr->qptr->count = 0;
		ready(pid);
	}
	
	restore(mask);
	return loop_index == 0 ? SYSERR : loop_index;
	
}


uint32 sendnMsg(uint32 pid_count, pid32* pids, umsg32 msg)
{
	intmask mask;
	struct procent *prptr;
	uint32 head,tail,success = 0,loop_index=0;
	pid32 pid;
	
	mask = disable();
	
	resched_cntl(DEFER_START);
	for(;(loop_index < pid_count) && (loop_index < NPROC); loop_index++)
	{
		pid = pids[loop_index];
		if(!isbadpid(pid))
		{
			prptr = &proctab[pid];
			tail = prptr->qptr->tail;
			head = prptr->qptr->head;
			if(head != tail)
			{
				//Queue is not full
				prptr->qptr->msgq[tail] = msg; // Queue the message
				if(head == MAX_MSGS) {
					// Empty queue is now filled
					//Update the head index to point to the tail
					head = tail;
				}
				tail++;
				tail %= MAX_MSGS;
				
				prptr->qptr->head = head;
				prptr->qptr->tail = tail; // Update the tail index
				
				if(prptr->prstate == PR_RECQ && ((tail==head) || (((tail - head + MAX_MSGS)%MAX_MSGS) >= prptr->qptr->count)))
				{ 
					// Ready the process as expected count is acheived
					prptr->qptr->count = 0;
					ready(pid);
				}	
				success++;
			}
			else
			{
				kprintf("Process[%d]: Unable to send messages as Process[%d] queue is full \n",getpid(),pid);
			}
		}
		else
		{
			kprintf("Process[%d]: Invalid Receiver process id: %d \n",getpid(),pid);	
		}
	}
	
	resched_cntl(DEFER_STOP);
	restore(mask);
	return success == 0 ? SYSERR : success;
}
