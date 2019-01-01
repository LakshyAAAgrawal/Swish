#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 

void main(int argc, char *argv[])
{
    int sockfd = 0, n = 0,portno,i=0,recvBuff[10],buff[10],x;
    char o;
    int iSetOption = 1;
    struct sockaddr_in serv_addr; 
    FILE *fp=fopen(argv[2],"wb");
    if(fp==NULL)
    {
    	printf("error:creating file\n");
    	exit(1);
    }

    if(argc != 3)
    {
        printf("\n Usage: %s <ip of server> <file name>\n",argv[0]);
        exit(1);
    } 

    memset(recvBuff, '0',sizeof(recvBuff));
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Error : Could not create socket \n");
        exit(1);
    } 
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char*)&iSetOption,sizeof(iSetOption));
    memset(&serv_addr, '0', sizeof(serv_addr)); 

    portno=1030;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno); 

    if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0)
    {
        printf("\n inet_pton error occured\n");
        exit(1);
    } 
	x=connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
       	while(x < 0){
       		printf("try\n");
       		sleep(1);
       		x=connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    	}
 
    /*data handling*/
    while(1)
    {
    	n=read(sockfd, &recvBuff, 1);
    	if(n<0||n==0)
    	{
    		break;
    	}
    	i=recvBuff[0];
    	if(recvBuff[0]==EOF)
    	{
    		break;
    	}
    	fputc(recvBuff[0],fp);
    }
    close(sockfd);
    fclose(fp);
    printf("received %s\n",argv[2]); 
}
