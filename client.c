#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<netinet/in.h>
#include<sys/stat.h>
#include<dirent.h>
#include <stdlib.h>


int main()
{
	int sfd;
	struct sockaddr_in saddr;
	char buff[100];
	int ch;
	char data[100];
    char mail_id[16];
    char mail_subject[500];
    char mail_body[2000];

	sfd= socket(AF_INET,SOCK_STREAM,0);
	if(sfd<0)
	{
		perror("Error in socket \n");
		return 1;
	}
	saddr.sin_family=AF_INET;
	saddr.sin_port=htons(587);
	saddr.sin_addr.s_addr=inet_addr("192.168.57.50");

	int c=connect(sfd,(const struct sockaddr*)&saddr,sizeof(saddr));
	if(c<0)
	{
		perror("error in connect \n");
		return 1;
	} 
	printf("\nconnected to server \n");
	while(1)
	{
		memset(&buff,'\0',sizeof(buff));
		recv(sfd,buff,sizeof(buff),0);
		printf("%s",buff);

		scanf("%s",data);
		int ch= atoi(data);
		
		//scanf("%d",&ch);
		// int ch= atoi(data);

		if(ch==1)
		{
		
			memset(&buff,'\0',sizeof(buff));
			recv(sfd,buff,sizeof(buff),0);
			printf("%s",buff);
			scanf("%[^\n]%*c",mail_id);
		  	send(sfd,mail_id,sizeof(mail_id),0);
		  			
		  	memset(&buff,'\0',sizeof(buff));
			recv(sfd,buff,sizeof(buff),0);
			printf("%s",buff);
			scanf("%[^\n]%*c",mail_subject);
			send(sfd,mail_subject,sizeof(mail_subject),0);
							
			memset(&buff,'\0',sizeof(buff));
			recv(sfd,buff,sizeof(buff),0);
			printf("%s",buff);
			scanf("%[^\n]%*c",mail_body);
			send(sfd,mail_body,sizeof(mail_body),0);				
			
		}
		else if(ch==3)
		{
			close(sfd);
			printf("Disconected from server : ");
			exit(1);				
		}
	}

	return 0;
}