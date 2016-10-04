/*  main.c  - main */

#include <xinu.h>

pid32 p1,p2,p3,p4;
sid32 s1,s2;

process process1(void)
{
	while(1)
	{
		wait(s1);
		umsg32 msg = receiveMsg();
		kprintf("Receiver[1]: Message Received is %d \n",msg);
		signal(s2);
	}	
}

process process4(void)
{
	uint32 msg_count = 7,i=0;
	umsg32 msg[MAX_MSGS]={0};
	while(1)
	{
		i=0;
		wait(s1);
		receiveMsgs(msg,msg_count);
		while(i < msg_count){
			kprintf("Receiver[2]: Message Received is %d \n",msg[i++]);
		}
		signal(s2);

	}	
}

process process2(void)
{
	uint32 msg_count = 1,i=0,result;
	umsg32 msg_buff[MAX_MSGS]={0},msg;
	
	msg = 21;
	kprintf("Sender[1]: Sending Message '%d' to Receiver[1] \n",msg);
	result = sendMsg(p1,msg);
	if(result)
		kprintf("Sender[1]: Message '%d' successfully sent to Receiver[1] \n",msg);
	else
		kprintf("Sender[1]: Unable to send message as the Receiver[1] queue is full \n");
	
	wait(s2);
	
	msg_count = 10;
	i = 0;
	while(i < msg_count){
		msg_buff[i] = i;
		kprintf("Sender[1]: Sending Message '%d' to Receiver[1] \n",msg_buff[i++]);
	}
		
	result = sendMsgs(p1,msg_buff,msg_count);
		
	kprintf("Sender[1]: Successfuly sent %u messages of %u messages to Receiver[1] \n",result,msg_count);
	
	signal(s1);
	wait(s2);
	
	msg_count = 2;
	i = 0;
	while(i < msg_count){
		msg_buff[i] = i;
		kprintf("Sender[1]: Sending Message '%d' to Receiver[1] \n",msg_buff[i++]);
	}
		
	result = sendMsgs(p1,msg_buff,msg_count);
		
	kprintf("Sender[1]: Successfuly sent %u messages of %u messages to Receiver[1] \n",result,msg_count);
	
	msg = 12;
	kprintf("Sender[1]: Sending Message '%d' to Receiver[1] \n",msg);
	result = sendMsg(p1,msg);
	if(result)
		kprintf("Sender[1]: Message '%d' successfully sent to Receiver[1] \n",msg);
	else
		kprintf("Sender[1]: Unable to send message as the Receiver[1] queue is full \n");
	
	kill(p1);
	semreset(s1,1);
	semreset(s2,0);
	resume(p3);
	resume(p4);
	
   	return OK;
}

process process3(void)
{
	uint32 msg_count = 1,i=0,result;
	umsg32 msg_buff[MAX_MSGS]={0},msg,pid_buff[2];
	
	msg = 21;
	kprintf("Sender[2]: Sending Message '%d' to Receiver[2] \n",msg);
	result = sendMsg(p2,msg);
	if(result)
		kprintf("Sender[2]: Message '%d' successfully sent to Receiver[2] \n",msg);
	else
		kprintf("Sender[2]: Unable to send message as the Receiver[2] queue is full \n");
	
	
	msg_count = 6;
	i = 0;
	while(i < msg_count){
		msg_buff[i] = i;
		kprintf("Sender[2]: Sending Message '%d' to Receiver[2] \n",msg_buff[i++]);
	}
		
	result = sendMsgs(p2,msg_buff,msg_count);
		
	kprintf("Sender[2]: Successfuly sent %u messages of %u messages to Receiver[2] \n",result,msg_count);
	
	wait(s2);
	
	msg_count = 6;
	i = 0;
	while(i < msg_count){
		msg_buff[i] = i;
		kprintf("Sender[2]: Sending Message '%d' to Receiver[2] \n",msg_buff[i++]);
	}
		
	result = sendMsgs(p2,msg_buff,msg_count);
		
	kprintf("Sender[2]: Successfuly sent %u messages of %u messages to Receiver[2] \n",result,msg_count);
	
	signal(s1);
	
	msg = 12;
	kprintf("Sender[2]: Sending Message '%d' to Receiver[2] \n",msg);
	result = sendMsg(p2,msg);
	if(result)
		kprintf("Sender[2]: Message '%d' successfully sent to Receiver[2] \n",msg);
	else
		kprintf("Sender[2]: Unable to send message as the Receiver[2] queue is full \n");
	
	wait(s2);
	
	pid_buff[0] = p4;
	pid_buff[1]= -1;
	
	result = sendnMsg(2,pid_buff,55);
	
	kprintf("Sender[2]: Successfuly sent message of %u processes of total %u processes \n",result,2);
	
	
	kill(p4);
	semdelete(s1);
	semdelete(s2);
	return OK;
	
}

process	main(void)
{

	p1 = create(process1, 8192, 51, "process1", 0);
	p2 = create(process2, 8192, 50, "process2", 0);
	p3 = create(process3, 8192, 50, "process3", 0);
	p4 = create(process4, 8192, 51, "process4", 0);

	if(isbadpid(p1) || isbadpid(p2) || isbadpid(p3) || isbadpid(p4))
	{
		kprintf("Unable to create process in main \n");
		return SYSERR;
	}
	
	s1 = semcreate(1);
	s2 = semcreate(0);
	
	if((s1 == (sid32)SYSERR) || (s2 == (sid32)SYSERR))
	{
		kprintf("Unable to create semaphore in main \n");
		return SYSERR;
	}
	
	
	resume(p1);
	resume(p2);
	
	return OK;
    
}

