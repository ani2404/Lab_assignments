/*  main.c  - main */

#include <xinu.h>

pid32 p1,p2,p3;
sid32 s1,s2,s3;

process process1(void)
{
	while(1)
	{
		umsg32 msg = receiveMsg();
		kprintf("Message Received is %d \n",msg);
	}	
}

process process4(void)
{
	uint32 msg_count = 1;
	umsg32 msg[MAX_MSGS]={0};
	while(1)
	{
		int i=0;
		receiveMsgs(msg,msg_count);
		while(i < msg_count){
			kprintf("Message Received is %d \n",msg[i++]);
		}
		msg_count++;
		msg_count %= MAX_MSGS;
	}	
}

process process2(void)
{
	wait(s1);
	uint32 msg_count = 1;
	while(msg_count < MAX_MSGS)
	umsg32 msg = (umsg32)"A";
	kprintf("Sending Message "%d" to Process[%d] \n",msg,p1);
	int32 result = sendMsg(p1,msg);
	if(result)
		kprintf("Message "%d" successfully sent to Process[%d] \n",msg,p1);
	else
		kprintf("Unable to send message as the receiver process[%d] queue is full \n",p1);
	
	
	umsg32 msg = (umsg32)"A";
	kprintf("Sending Message "%d" to Process[%d] \n",msg,p1);
	int32 result = sendMsgs(p1,msg);
	if(result)
		kprintf("Message "%d" successfully sent to Process[%d] \n",msg,p1);
	else
		kprintf("Unable to send message as the receiver process[%d] queue is full \n",p1)
	
	signal(s2);
}

process process3(void)
{
	wait(s2);
	umsg32 msg = (umsg32)"N";
	kprintf("Sending Message "%d" to Process[%d] \n",msg,p1);
	int32 result = sendMsg(p1,msg);
	if(result)
		kprintf("Message "%d" successfully sent to Process[%d] \n",msg,p1);
	else
		kprintf("Unable to send message as the receiver process[%d] queue is full \n",p1);
	signal(s1);
	
}

process	main(void)
{

	p1 = create(process1, 8192, 51, "process1", 0);
	p2 = create(process2, 8192, 50, "process2", 0);
	p3 = create(process3, 8192, 50, "process3", 0);
	p4 = create(process4, 8192, 51, "process4", 0);

	if(isbadpid(p1) || isbadpid(p2) || isbadpid(p32) || isbadpid(p4))
	{
		kprintf("Unable to create process in main \n");
		return SYSERR;
	}
	
	s1 = sem_create(1);
	s2 = sem_create(0);
	
	if((s1 == (sid32)SYSERR) || (s1 == (sid32)SYSERR))
	{
		kprintf("Unable to create semaphore in main \n");
		return SYSERR;
	}
	
	
	resume(p1);
	resume(p2);
	resume(p3);
	resume(p4);

	
	return OK;
    
}

