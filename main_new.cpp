/*  main.c  - main */

#include <xinu.h>

pid32 p1,p2,p3,p4;
sid32 s1,s2s3;

process process2(void)
{
    umsg32 msg;
	while(1)
	{
		wait(s2);
		msg = receiveMsg();
		kprintf("Process[%d]: Message Received is %d \n",getpid(),msg);
		signal(s1);
	}	
}

process process3(void)
{
	uint32 msg_count = 10,i=0;
	umsg32 msg[MAX_MSGS]={0};
	while(1)
	{
		i=0;
		wait(s3);
		receiveMsgs(msg,msg_count);
		while(i < msg_count){
			kprintf("Process[%d]: Message Received is %d \n",getpid(),msg[i++]);
		}
		signal(s1);

	}	
}

process process1(void)
{
	uint32 msg_count = 1,i=0,result;
	umsg32 msg_buff[MAX_MSGS]={0},msg;
	pid32 pid_buff[3];
	static int counter = 0;
	
	wait(s1);
	
	msg_count = 10;
	i = 0;
	while(i < msg_count){
		msg_buff[i] = ++counter;
		kprintf("Process[%d]: Sending Message '%d' to Process[%d] \n",getpid(),msg_buff[i++],p3);
	}
	
	// Receiver 2 Buffer is filled
	result = sendMsgs(p3,msg_buff,msg_count);
	
	if(result)
	{
		kprintf("Process[%d]: Successfuly sent %u messages of %u messages to Process[%d] \n",getpid(),result,msg_count,p3);		
	}
	else
		kprintf("Process[%d]: Unable to send message as the Process[%d] queue is full \n",getpid(),p3);
	
	//SendnMsg to empty buffer, full buffer and invalid pid
	
	pid_buff[0] = p2; // empty buffer
	pid_buff[1] = p3; // full buffer
	pid_buff[1]= -1; // invalid pid
	
	kprintf("Process[%d]: Sending message '55'  to Process[%d], Process[%d], Process[%d] \n",getpid(),p2,p3,-1);
	result = sendnMsg(3,pid_buff,55);
	
	kprintf("Process[%d]: Successfuly sent message to %d processes of total %u processes \n",getpid(),result,3);

    signal(s2); // Receiver 1 consumes message
    signal(s3); // Receiver 2 consumes message

	// Both receivers should consume
	wait(s1);	
	wait(s1);
	
	
	//Send message using sendMsg, sendMsgs
	
	
	msg = ++counter;
	kprintf("Process[%d]: Sending Message '%d' to Process[%d] \n",getppid(),msg,p2);
	result = sendMsg(p2,msg);
	if(result)
		kprintf("Process[%d]: Message '%d' successfully sent to Process[%d] \n",getpid(),msg,p2);
	else
		kprintf("Process[%d]: Unable to send message as the Process[%d] queue is full \n",getpid(),p2);
	
	signal(s2);
	wait(s1);
	
	msg_count = 10;
	i = 0;
	while(i < msg_count){
		msg_buff[i] = ++counter;
		kprintf("Process[%d]: Sending Message '%d' to Process[%d] \n",getpid(),msg_buff[i++],p3);
	}
		
	result = sendMsgs(p3,msg_buff,msg_count);
		
	if(result)
	{
		kprintf("Process[%d]: Successfuly sent %u messages of %u messages to Process[%d] \n",getpid(),result,msg_count,p3);		
	}
	else
		kprintf("Process[%d]: Unable to send message as the Process[%d] queue is full \n",getpid(),p3);
	
	
	msg = ++counter;
	kprintf("Process[%d]: Sending Message '%d' to Process[%d] \n",getppid(),msg,p2);
	result = sendMsg(p2,msg);
	if(result)
		kprintf("Process[%d]: Message '%d' successfully sent to Process[%d] \n",getpid(),msg,p2);
	else
		kprintf("Process[%d]: Unable to send message as the Process[%d] queue is full \n",getpid(),p2);
	
	
	msg_count = 1;
	i = 0;
	while(i < msg_count){
		msg_buff[i] = ++counter;
		kprintf("Process[%d]: Sending Message '%d' to Process[%d] \n",getpid(),msg_buff[i++],p3);
	}
		
	result = sendMsgs(p3,msg_buff,msg_count);
		
	if(result)
	{
		kprintf("Process[%d]: Successfuly sent %u messages of %u messages to Process[%d] \n",getpid(),result,msg_count,p3);		
	}
	else
		kprintf("Process[%d]: Unable to send message as the Process[%d] queue is full \n",getpid(),p3);
	
	signal(s2);
	signal(s3);
	
	wait(s1);
	wait(s1);
	
	
	kill(p2);
	kill(p3)
	
   	return OK;
}

process	main(void)
{

	p1 = create(process1, 8192, 50, "sender1", 0);
	//p2 = create(process2, 8192, 50, "sender2", 0);
	p2 = create(process2, 8192, 50, "receiver1", 0);
	p3 = create(process3, 8192, 50, "receiver2", 0);

	if(isbadpid(p1) || isbadpid(p2) || isbadpid(p3))
	{
		kprintf("Unable to create process in main \n");
		return SYSERR;
	}
	
	s1 = semcreate(1);
	s2 = semcreate(0);
	s3 = sem_create(0);
	
	if((s1 == (sid32)SYSERR) || (s2 == (sid32)SYSERR) || (s3 == (sid32)SYSERR))
	{
		kprintf("Unable to create semaphore in main \n");
		return SYSERR;
	}
	
	
	resume(p1);
	resume(p2);
	resume(p3)
	
	return OK;
    
}

