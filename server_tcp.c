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

// 服务器端，以nalu为buffer长度，用tcp协议发送出去，同时接收客户端tcp_recv发来的flag，调整发的层数，flag为2就是发最高层
//  ./a.out input.264         本程序先启动，客户端程序tcp_recv再启动

const int buf_size = 1;
const int local_port = 8000;
const int remote_port = 6000;
#define  remote_ip   "127.0.0.1"
int recv_flag = 2,flag=0;

unsigned char* get_buf(int file_size,FILE *fp)
{
	unsigned char *buf;
	buf = (unsigned char*)malloc(file_size*sizeof(char));

	fread(buf,sizeof(unsigned char),file_size,fp);
	return buf;
}

int get_file_size(FILE *fp)
{
	int size_file=0;
	fseek(fp,0,SEEK_END);
	size_file = ftell(fp);
	rewind(fp);
	printf("###############%d\n",size_file);	
	return size_file;
}


void *thread1()
{
	int local_sockfd,remote_sockfd;//服务器端套接字
	int sin_size,len;
	char buf[5];
	struct sockaddr_in local_addr,remote_addr; //服务器网络地址结构体
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
	sin_size=sizeof(struct sockaddr_in);
	if((remote_sockfd=accept(local_sockfd,(struct sockaddr *)&remote_addr,&sin_size))<0)
	{
		perror("accept");
		
	}
	//printf("accept sucess\n");
	
	while((len=recv(remote_sockfd,buf,buf_size,0))>0)
	{
		int a = (int)(*buf);
		if(a==0)
		{recv_flag=0;}
		else if(a==1)
		{recv_flag=1;}
		else
		{recv_flag=2;}
		printf("a=%d\n",a);
		printf("recv_flag=%d\n",recv_flag);
		flag = 1;
		memset(buf,0,1);
	}
	close(local_sockfd);
	close(remote_sockfd);
	
}

int main(int argc,char *argv[])
{
	int size_file,i=0,start_i=0;
	int flag1=0,flag2=0,flag3=0;
	
//文件	
	unsigned char *buf_data,*buf;
	FILE *fp;    //文件数组
	fp = fopen(argv[1],"rb");
	size_file = get_file_size(fp);
	buf = get_buf(size_file,fp);
//线程	
	pthread_t thread;
	int temp;
        memset(&thread, 0, sizeof(thread));          //comment1
        /*创建线程*/
        if((temp = pthread_create(&thread, NULL, thread1, NULL)) != 0)       //comment2
                printf("线程1创建失败!\n");
        else
                printf("线程1被创建\n");
	
//socket
	//printf("continue");	
//	int local_sockfd;//服务器端套接字
	int remote_sockfd;//客户端套接字
	int len;
//	struct sockaddr_in local_addr; //服务器网络地址结构体
	struct sockaddr_in remote_addr; //客户端网络地址结构体
	int sin_size;
	
//	memset(&local_addr,0,sizeof(local_addr)); //数据初始化--清零
//	local_addr.sin_family=AF_INET; //设置为IP通信
//	local_addr.sin_addr.s_addr=INADDR_ANY;//服务器IP地址--允许连接到所有本地地址上
//	local_addr.sin_port=htons(local_port); //服务器端口号
	
	memset(&remote_addr,0,sizeof(remote_addr));
	remote_addr.sin_family=AF_INET; //设置为IP通信
	remote_addr.sin_addr.s_addr=inet_addr(remote_ip);//服务器IP地址
	remote_addr.sin_port=htons(remote_port); //服务器端口号 
  while(1)
  {
    if(flag==1)
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

//遍历文件，发送buf	
	for(i=0;i<size_file;i++)
	{
		if(buf[i]==0 && buf[i+1]==0 && buf[i+2]==0 && buf[i+3]==0x01 )
		{
			if((((buf[start_i+12] & 0x0f)==0x01) || ((buf[start_i+12] & 0x0f)==0x05))  )
			{
				//if()
				/*
				if(flag1 == 0)  //关键地方，此条件必须放在这里，使得帧号加1,并且每一个nalu都能遍历到
				{
					flag1 = 1;
					flag2 = 0;
					buf_data=(unsigned char *)malloc((i-start_i-8)*sizeof(char));
					memcpy(buf_data,&buf[start_i],i-start_i-8);
					send(remote_sockfd,buf_data,i-start_i-8,0);
					//printf("send_base_layer=%d\n",i-start_i);
					//printf("send_base_layer=%d\n",strlen(buf_data));
				
					free(buf_data);
				}
				else
				{
				*/
					buf_data=(unsigned char *)malloc((i-start_i)*sizeof(char));
					memcpy(buf_data,&buf[start_i],i-start_i);
					send(remote_sockfd,buf_data,i-start_i,0);
					usleep(2500);
					//printf("send_base_layer=%d\n",i-start_i);
					//printf("send_base_layer=%d\n",strlen(buf_data));
				
					free(buf_data);
				//}
			}
			else if((buf[start_i+12] & 0x1f)==0x14 && buf[start_i+14]==0x10)
			{
				if(recv_flag== 2 || recv_flag== 1 )
				{
				/*
					if(flag2 == 0)  //关键地方，此条件必须放在这里，使得帧号加1,并且每一个nalu都能遍历到
					{
						flag2 = 1;
						flag3 = 0;
						buf_data=(unsigned char *)malloc((i-start_i-8)*sizeof(char));
						memcpy(buf_data,&buf[start_i],i-start_i-8);
						send(remote_sockfd,buf_data,i-start_i-8,0);
						//printf("send_base_layer=%d\n",i-start_i);
						//printf("send_base_layer=%d\n",strlen(buf_data));
				
						free(buf_data);
					}
					else
					{
					*/
						buf_data=(unsigned char *)malloc((i-start_i)*sizeof(char));
						memcpy(buf_data,&buf[start_i],i-start_i);
						send(remote_sockfd,buf_data,i-start_i,0);
						usleep(2500);
						//printf("send_en1_layer=%d\n",i-start_i);
					
						free(buf_data);
					//}
				}
			}
			else if((buf[start_i+12] & 0x1f)==0x14 && buf[start_i+14]==0x20 )
			{
				if(recv_flag== 2)
				{
				/*
					if(flag3 == 0)  //关键地方，此条件必须放在这里，使得帧号加1,并且每一个nalu都能遍历到
					{
						flag3 = 1;
						flag1 = 0;
						buf_data=(unsigned char *)malloc((i-start_i-8)*sizeof(char));
						memcpy(buf_data,&buf[start_i],i-start_i-8);
						send(remote_sockfd,buf_data,i-start_i-8,0);
						//printf("send_base_layer=%d\n",i-start_i);
						//printf("send_base_layer=%d\n",strlen(buf_data));
				
						free(buf_data);
					}
					else
					{
					*/
						buf_data=(unsigned char *)malloc((i-start_i)*sizeof(char));
						memcpy(buf_data,&buf[start_i],i-start_i);
						send(remote_sockfd,buf_data,i-start_i,0);
						usleep(2500);
						//printf("send_en2_layer=%d\n",i-start_i);
						//printf("send_en2_layer=%d\n",strlen(buf_data));
					
						free(buf_data);
					//}
				}
			}
			else
			{
				//if(recv_flag==2)
				//{
				buf_data=(unsigned char *)malloc((i-start_i)*sizeof(char));
				memcpy(buf_data,&buf[start_i],i-start_i);
				send(remote_sockfd,buf_data,i-start_i,0);
				usleep(2500);
				//printf("send_para=%d\n",strlen(buf_data));
				free(buf_data);
				//}
			}
			start_i = i;
		}
	}
	break;
    }
	
  }
	close(remote_sockfd);
}


//unsigned char get_nalu()
/*
int get_frame_count(int file_size,unsigned char *buf,FILE *fp_out,FRAME_INFO *fra_info)
{
	int flag1=0,flag2=0,flag3=0,frame_count=0,i,start_i=0,match_count=0,total_match_count=0,final_frame_count=0;
	unsigned char *buffer;


	
	for(i=0;i<file_size;i++)
	{
		if(buf[i]==0 && buf[i+1]==0 && buf[i+2]==0 && buf[i+3]==0x01 )
		{
			
			//fra_info=(FRAME_INFO*)&buffer[0];
			fra_info->buf[1] = 0xff;
			fra_info->buf[2] = 0xff;
			fra_info->buf[3] = 0xff;
			fra_info->buf[0] = 0xff;
			fra_info->frame_num=frame_count;

			if((((buf[start_i+4] & 0x0f)==0x01) || ((buf[start_i+4] & 0x0f)==0x05))  )
			{
				//buffer=(unsigned char *)malloc(i-start_i+sizeof(FRAME_INFO));  
    				//memset(buffer,0,i-start_i+sizeof(FRAME_INFO));
    				 
				if(flag1 == 0)
				{fra_info->frame_num=frame_count+1;}
				fwrite(fra_info,sizeof(FRAME_INFO),1,fp_out);
				
				//memcpy(buffer+sizeof(FRAME_INFO),&buf[start_i],i-start_i);
				
				fwrite(&buf[start_i],1,i-start_i,fp_out);
				
				if(flag1 == 0)  //关键地方，此条件必须放在这里，使得帧号加1,并且每一个nalu都能遍历到
				{
					flag1 = 1;
					flag2 = 0;
					frame_count++;
				}
				//printf("11111111111111111\n");

				//free(buffer);
				//continue;
			}
			else if((buf[start_i+4] & 0x1f)==0x14 && buf[start_i+6]==0x10)
			{
				//buffer=(unsigned char *)malloc(i-start_i+sizeof(FRAME_INFO));  
    				//memset(buffer,0,i-start_i+8);
				fwrite(fra_info,sizeof(FRAME_INFO),1,fp_out);
				
				//memcpy(buffer+sizeof(FRAME_INFO),&buf[start_i],i-start_i);
				
				fwrite(&buf[start_i],1,i-start_i,fp_out);
				if(flag2 == 0)
				{
			            flag2 = 1;
				    flag3 = 0;
				}			
				
				//free(buffer);
			}
			else if((buf[start_i+4] & 0x1f)==0x14 && buf[start_i+6]==0x20 )
			{
				//buffer=(unsigned char *)malloc(i-start_i+sizeof(FRAME_INFO));  
    				//memset(buffer,0,i-start_i+sizeof(FRAME_INFO));
				fwrite(fra_info,sizeof(FRAME_INFO),1,fp_out);
				
				//memcpy(buffer+sizeof(FRAME_INFO),&buf[start_i],i-start_i);
				
				fwrite(&buf[start_i],1,i-start_i,fp_out);
				if(flag3==0)
				{
				    flag3 = 1;
				    flag1 = 0;
				}   
				//free(buffer);
			}
			else
			{	
				//buffer=(unsigned char *)malloc(i-start_i);  
    				//memset(buffer,0,i-start_i);
				//fwrite(fra_info,sizeof(FRAME_INFO),1,fp_out);
				
				//memcpy(buffer+sizeof(FRAME_INFO),&buf[start_i],i-start_i);
				
				fwrite(&buf[start_i],1,i-start_i,fp_out);	
				//free(buffer);
			}			
			start_i = i;
		}
		
	} 
	
	return frame_count;
}

int main(int argc,char *argv[])
{
	int frame_num,i,m=0;
	
	FRAME_INFO *fra_info;
	fra_info = (FRAME_INFO *)malloc(sizeof(FRAME_INFO));
	printf("sizeof(FRAME_INFO)=%d",sizeof(FRAME_INFO)); 
	FILE *fp,*fp_out;    //文件数组
	fp = fopen(argv[1],"rb");
	fp_out = fopen(argv[2],"wb");
	int size_file,argc_count=0;
	argc_count = argc -2;
	unsigned char *buf_data;
	//int argv_data[argc_count];
	//argv_data = (char*)malloc(argc_count*2*sizeof(char));
	/*
	for(i=0;i<argc_count;i++)
	{
		char *a;
		a=argv[i+2];
		argv_data[i]=*a;//argv[]括号里存的是字符串的地址
		sscanf(a,"%x",&m);
		argv_data[i]=(int)m;
		//printf("arv = %d\n",atoi(argv_data+i));
		//printf("a = %x\n",argv_data[i]);
	}
	
	size_file = get_file_size(fp);
	buf_data = get_buf(size_file,fp);
	printf("arg = %d\n",argc_count);
	
	frame_num = get_frame_count(size_file,buf_data,fp_out,fra_info);
	printf("total_frame_num = %d\n",frame_num);
	free(buf_data);
	fclose(fp);
	
	return 0;
}

*/

