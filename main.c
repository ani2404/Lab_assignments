/*  main.c  - main */

#include <xinu.h>

pid32 p1,p2,p3,p4;
sid32 s1,s2;

process process1(void)
{
	while(1)
	{
		umsg32 msg = receiveMsg();
		kprintf("Receiver[1]: Message Received is %d \n",msg);
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
			kprintf("Receiver[2]: Message Received is %d \n",msg[i++]);
		}
		msg_count++;
		msg_count %= MAX_MSGS;
	}	
}

process process2(void)
{
	uint32 msg_count = 1;
	umsg32 msg_buff[MAX_MSGS]={0};
	while(msg_count < MAX_MSGS)
	{
		//wait(s1);
		umsg32 msg = 21;
		kprintf("Sender[1]: Sending Message '%d' to Receiver[1] \n",msg);
		uint32 result = sendMsg(p1,msg);
		if(result)
			kprintf("Sender[1]: Message '%d' successfully sent to Receiver[1] \n",msg);
		else
			kprintf("Sender[1]: Unable to send message as the Receiver[1] queue is full \n");
	
	
	    	//umsg32 msg_buff[msg_count] = {0};
		uint32 i = 0;
		while(i < msg_count){
			msg_buff[i] = i;
			kprintf("Sender[1]: Sending Message '%d' to Receiver[1] \n",msg_buff[i++]);
		}
		
		result = sendMsgs(p1,msg_buff,msg_count);
		
		kprintf("Sender[1]: Successfuly sent %u messages of %u messages to Receiver[1] \n",result,msg_count);
		msg_count++;	
		//signal(s2);
	}
   	return OK;
}

process process3(void)
{
	uint32 msg_count = 1;
	umsg32 msg_buff[MAX_MSGS]={0};
	while(msg_count < MAX_MSGS)
	{
		wait(s2);
		umsg32 msg = 12;
		kprintf("Sender[2]: Sending Message '%d' to Receiver[2] \n",msg);
		uint32 result = sendMsg(p2,msg);
		if(result)
			kprintf("Sender[2]: Message '%d' successfully sent to Receiver[2] \n",msg);
		else
			kprintf("Sender[2]: Unable to send message as the Receiver[2] queue is full \n");
	
	
	    	//umsg32 msg_buff[msg_count] = {0};
		uint32 i = 0;
		while(i < msg_count){
			msg_buff[i] = i;
			kprintf("Sender[2]: Sending Message '%d' to Receiver[1] \n",msg_buff[i++]);
		}
		
		result = sendMsgs(p1,msg_buff,msg_count);
		
		kprintf("Sender[2]: Successfuly sent %u messages of %u messages to Receiver[1] \n",result,msg_count);
		msg_count++;	
		signal(s1);
	}
	
	kill(p1);
	kill(p4);
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
	resume(p3);
	resume(p4);

	
	return OK;
    
}

