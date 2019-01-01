#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<netdb.h>
void main(int argc,char *argv[])
{
	int sockfd,newsockfd,portno,i=0,buffer[1024];
	int iSetOption = 1;
	char in,buff[10];
	socklen_t clilen;
	struct sockaddr_in serv_addr,cli_addr;
	FILE *fp=fopen(argv[1],"rb");
	if(fp==NULL)
	{
		printf("error:opening file\n");
		exit(1);
	}
	/*socket creation*/
	sockfd=socket(AF_INET,SOCK_STREAM,0);
	if(sockfd<0)
	{
		printf("socket creation error\n");
		exit(1);
	}
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char*)&iSetOption,sizeof(iSetOption));
	/*bind*/
	portno=1030;
	bzero((char *)&serv_addr,sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_addr.sin_port=htons(portno);
	bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
	
	/*listen*/
	listen(sockfd,1);
	clilen=sizeof(cli_addr);
	
	/*accept incoming connection*/
	newsockfd=accept(sockfd,(struct sockaddr *)&cli_addr,&clilen);
	if(newsockfd<0)
	{
		printf("error accept()");
		exit(1);
	}
	
	/*data handling*/
	while(1)
	{
		buffer[0]=getc(fp);
		if(buffer[0]==EOF)
		{
			buffer[0]=0;
			break;
		}
		write(newsockfd,&buffer,1);
	}
	
	/*closing sockets*/
	close(newsockfd);
	close(sockfd);
	fclose(fp);
	printf("sent %s\n",argv[1]);
}
	
