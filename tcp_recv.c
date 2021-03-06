#include <stdio.h>  

#include <stdlib.h>  

//#include <conio.h>  

#include <string.h>  
#include <memory.h> 
//#include <winsock2.h>  

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>   
#include <netinet/in.h>   
#include <netdb.h>   
#include <arpa/inet.h> 
#include <pthread.h>

#define filename  "/home/liuxun/tcp_recv.264"
const int local_port=6000;
const int remote_port=8000;
const int buf_size=1400*100;
int flag = 0;
#define  remote_ip   "127.0.0.1"

typedef struct 
{
	char buf[4];
	unsigned long frame_num;
;		
} FRAME_INFO;
FRAME_INFO *fra;

// 客户端，接收服务器端发送的tcp流，同时发送flag，来控制服务器的发送层数
// ./a.out 

void *recv_264file_thread()
{
	int local_sockfd,remote_sockfd;
	int len,sin_size,i,count,t;
	unsigned char buf[buf_size],*temp;
	
	FILE *fp_out ;    //文件数组
	fp_out = fopen(filename,"wb");
	//printf("file write sucess\n");
	struct sockaddr_in local_addr,remote_addr;
	memset(&local_addr,0,sizeof(local_addr)); //数据初始化--清零
	local_addr.sin_family=AF_INET; //设置为IP通信
	local_addr.sin_addr.s_addr=INADDR_ANY;//服务器IP地址--允许连接到所有本地地址上
	local_addr.sin_port=htons(local_port); //服务器端口号
	if((local_sockfd=socket(PF_INET,SOCK_STREAM,0))<0)
	{
		perror("socket");
		
	}
	if(bind(local_sockfd,(struct sockaddr *)&local_addr,sizeof(struct sockaddr))<0)
	{
		perror("bind");
		
	}
	listen(local_sockfd,5);
	flag = 1;
	sin_size=sizeof(struct sockaddr_in);
	if((remote_sockfd=accept(local_sockfd,(struct sockaddr *)&remote_addr,&sin_size))<0)
	{
		perror("accept");
		
	}
	while((len=recv(remote_sockfd,buf,buf_size,0))>0)
	{
		
		unsigned char *buffer;
		buffer = (unsigned char*)malloc(len*sizeof(char));
		
		memcpy(buffer,buf,len);
		int j ,count = 0;
		for(j=0;j<len;j++)
		{
		
			if(buffer[j]==0x00 && buffer[j+1]==0x00 && buffer[j+2]==0x00 && buffer[j+3]==0x01 && buffer[j+4]==0xff && buffer[j+5]==0xff && buffer[j+6]==0xff && buffer[j+7]==0xff)
			{
				fra = (FRAME_INFO*)&buffer[j+4];
				//if(last < fra->frame_num)
				//{
				count=fra->frame_num;
				//last = fra->frame_num;
				//printf("count = %d\n",count);
				//}						
			}
		}
		/*
		for(i=0;i<len;i++)
		{
			if(buffer[i]==0xff && buffer[i+1]==0xff &&buffer[i+1]==0xff)
			{
				
			}
		}
		*/
		fwrite(buffer,1,len,fp_out);
		/*
		unsigned char *buffer;
		buffer = (unsigned char*)malloc(len*sizeof(char));
		memcpy(buffer,buf,len);
		
		
	
		fwrite(buffer,1,len,fp_out);
		//sscanf(buf,"%s",buffer);
		
		for(i=0;i<len;i++)
		{
			t = (int)buf[i];
			printf("%x ",t);
			//printf("%s ",temp);

		}
*/
	}
	
	printf("thread1 :主函数在等我完成任务\n");
	close(local_sockfd);
	close(remote_sockfd);
        pthread_exit(NULL);
}

void *send_flag_thread()
{
	int remote_sockfd;
	struct sockaddr_in remote_addr;
	memset(&remote_addr,0,sizeof(remote_addr));
	remote_addr.sin_family=AF_INET; //设置为IP通信
	remote_addr.sin_addr.s_addr=inet_addr(remote_ip);//服务器IP地址--允许连接到所有本地地址上
	remote_addr.sin_port=htons(remote_port); //服务器端口号
	while(1)
	{  
	    if(flag == 1)
	    {
		if((remote_sockfd=socket(PF_INET,SOCK_STREAM,0))<0)
		{
			perror("socket");
			
		}
	
	
		if(connect(remote_sockfd,(struct sockaddr *)&remote_addr,sizeof(struct sockaddr))<0)
		{
			perror("connect");
			
		}
	
		char buf=0x02;
		int a = (int)(buf);
		printf("send buf =%d\n",a);
		send(remote_sockfd,&buf,1,0);
		close(remote_sockfd);
		printf("thread2 :主函数在等我完成任务\n");
		pthread_exit(NULL);
	    }	
	}

//	*/

}

int main(int argc,char *argv[])
{
	
	//线程	
	pthread_t thread[2];
	int temp;
        memset(&thread, 0, sizeof(thread));          //comment1
        /*创建线程*/
        if((temp = pthread_create(&thread[0], NULL, recv_264file_thread, NULL)) != 0)       //comment2
                printf("线程1创建失败!\n");
        else
               printf("线程1被创建\n");
	if((temp = pthread_create(&thread[1], NULL, send_flag_thread, NULL)) != 0)       //comment2
                printf("线程2创建失败!\n");
        else
               printf("线程2被创建\n");
	/*
	printf("continue");
	int remote_sockfd;
	struct sockaddr_in remote_addr;
	memset(&remote_addr,0,sizeof(remote_addr));
	remote_addr.sin_family=AF_INET; //设置为IP通信
	remote_addr.sin_addr.s_addr=inet_addr(remote_ip);//服务器IP地址--允许连接到所有本地地址上
	remote_addr.sin_port=htons(remote_port); //服务器端口号
	if(flag == 1)
	{
		if((remote_sockfd=socket(PF_INET,SOCK_STREAM,0))<0)
		{
			perror("socket");
			return 1;
		}
	
	
		if(connect(remote_sockfd,(struct sockaddr *)&remote_addr,sizeof(struct sockaddr))<0)
		{
			perror("connect");
			return 1;
		}
	
		char buf="0x02";
		printf("send buf =%d\n",(int)buf);
		send(remote_sockfd,&buf,1,0);
	}

	close(remote_sockfd);
	*/	
	
	
	
	
	/*等待线程结束*/
	if(thread[0]!=0)
	{
		pthread_join(thread[0],NULL);
                printf("线程1已经结束\n");
	}
	if(thread[1]!=0)
	{
		pthread_join(thread[1],NULL);
                printf("线程2已经结束\n");
	}
	
	
	
	
		
}
