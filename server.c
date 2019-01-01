#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<sys/inotify.h>

#define EVENT_SIZE  ( sizeof (struct inotify_event) )
#define BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )

void main(int argc, char *argv[])
{
	int sockfd,newsockfd,portno,i,countfile=0,n,countdir=0,instance,fd,wd,z=0;
	int iSetOption = 1;
	char in,buff[10],command[100],c,filename[100],foldername[100],buffer[BUF_LEN];
	socklen_t clilen;
	struct sockaddr_in serv_addr,cli_addr;

	sprintf(command,"(cd \"%s\";find . -maxdepth 1 -type f -printf \"\\\"%s\\\"\\n\">../sysdirlist)",argv[1],"%f");
	system(command);
	sprintf(command,"(cd \"%s\";find . -maxdepth 1 -type d -printf \"\\\"%s\\\"\\n\">../folderlist)",argv[1],"%f");
	system(command);
	FILE *dir=fopen("sysdirlist","r");
	FILE *folder=fopen("folderlist","r");
	if(dir<0){
		printf("error opening sysdirlist");
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
	portno=65435;
	if(argc==2)
		instance=0;
	else
	if(argc==3){
		instance=atoi(argv[2]);
		sprintf(buff,"6500%d",instance);
		portno=atoi(buff);
	}
	else
		return;
	printf("port used-%d\ninstance-%d\n",portno,instance);
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
	printf("accepted connection\n");
	sprintf(command,"./binserver sysdirlist");
	system(command);
	sprintf(command,"./binserver folderlist");
	system(command);
	
	for (c = getc(dir); c != EOF; c = getc(dir))
    	{
        	if (c == '\n') 
        	    countfile++;
    	}
 	for (c = getc(folder); c != EOF; c = getc(folder))
    	{
        	if (c == '\n') 
        	    countdir++;
    	}
    	countdir--;
    	if(countdir<0)
    		countdir=0;
    	printf("%d file/s\n%d folder/s\n",countfile,countdir);
    	rewind(folder);
    	rewind(dir);
    	i=0;
    	while(i<countfile)
    	{
 		n=0;
 		while(1)
 		{
 			c=fgetc(dir);
 			if(c=='\0'||c=='\n'||c==EOF)
 			{
 				filename[n]='\0';
 				break;
 			}
 			if(c==32)
 			{
 				filename[n]=92;
 				filename[n+1]=32;
 				n++;
 				n++;
 				continue;
 			}
 			filename[n]=c;
 			n++;
 		}
 		
 		sprintf(command,"./binserver \"%s\"/\"%s\"",argv[1],filename); 
 		
 		system(command);
 		
 		i++;
 	}  		
	i=0;
	
 	while(1)
 	{
 		c=getc(folder);
 		if(c=='\n')
 			break;
 	}
 	while(i<countdir)
 	{
 		n=0;
 		while(1)
 		{
 			c=fgetc(folder);
 			if(c=='\0'||c=='\n'||c==EOF)
 			{
 				foldername[n]='\0';
 				break;
 			}
 			if(c==32)
 			{
 				foldername[n]=92;
 				foldername[n+1]=32;
 				n++;
 				n++;
 				continue;
 			}
 			foldername[n]=c;
 			n++;
 		}
 		printf("in folder %s\n",foldername);
 		sprintf(command,"cp -n server \"%s\"/",argv[1]);
 		system(command);
 		sprintf(command,"cp -n binserver \"%s\"/",argv[1]);
 		system(command);
 		sprintf(command,"cp -n binclient \"%s\"/",argv[1]);
 		system(command);
 		instance++;
 		sprintf(command,"(cd \"%s\";./server \"%s\" %d)",argv[1],foldername,instance);
 		system(command);
 		instance--;
 		sprintf(command,"rm \"%s\"/server",argv[1]);
    		system(command);
    		sprintf(command,"rm \"%s\"/binclient",argv[1]);
    		system(command);
    		sprintf(command,"rm \"%s\"/binserver",argv[1]);
    		system(command);
 		printf("sent dir %s\n",foldername);
 		i++;
 	}
	
	fclose(dir);
	fclose(folder);
	system("rm sysdirlist");
	system("rm folderlist");
	close(newsockfd);
	close(sockfd);
}
