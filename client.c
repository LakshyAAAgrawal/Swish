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

int main(int argc, char *argv[])
{
	
    	int sockfd = 0, n = 0,portno,i=0,buff[10],err,countfile=0,countdir=0,rs,instance,x=1;
    	int iSetOption = 1;
    	char o,recvBuff[10],command[100],c,filename[100],foldername[100];
    	struct sockaddr_in serv_addr; 
	FILE *sysdir;
	FILE *folder;
	portno=65435;
	if(argc==3)
		instance=0;
	else
	if(argc==4){
		instance=atoi(argv[3]);
		sprintf(recvBuff,"6500%d",instance);
		portno=atoi(recvBuff);
    	}
    	printf("port used-%d\n",portno);
    	serv_addr.sin_family = AF_INET;
    	serv_addr.sin_port = htons(portno); 
    	memset(recvBuff, '0',sizeof(recvBuff));
    	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    	{
        	printf("\n Error : Could not create socket \n");
        	exit(1);
    	} 
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char*)&iSetOption,sizeof(iSetOption));
    	if((err=inet_pton(AF_INET, argv[1], &serv_addr.sin_addr))<=0)
   	 {
       	 	if(err==-1)
       	 		printf("\n inet_pton error occured\n");
       	 	else
       	 		printf("invalid ip\n");
       	 	exit(1);
  	 } 
	x=connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
       	while(x < 0){
       		printf("try\n");
       		sleep(1);
       		x=connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    	}
    	
    	
    	sprintf(command,"./binclient %s clisysdirlist",argv[1]);
    	system(command);
    	sprintf(command,"./binclient %s servfolderlist",argv[1]);
    	system(command);
    	sysdir=fopen("clisysdirlist","r");
    	folder=fopen("servfolderlist","r");
    	if(sysdir<0||folder<0){
    		printf("error opening clisysdirlist.txt");
    		exit(1);
    	}
    	
    	for (c = getc(sysdir); c != EOF; c = getc(sysdir))
    	{
        	if (c == '\n') // Increment count if this character is newline
        	    countfile = countfile + 1;
    	}
    	for (c = getc(folder); c != EOF; c = getc(folder))
    	{
        	if (c == '\n') // Increment count if this character is newline
        	    countdir = countdir + 1;
    	}
    	countdir--;
    	if(countdir<0)
    		countdir=0;
    	printf("%d file/s\n",countfile);
    	printf("%d folder/s\n",countdir);
    	rewind(sysdir);
    	rewind(folder);
    	i=0;
    	while(i<countfile)
    	{
    		n=0;
 		while(1)
 		{
 			c=fgetc(sysdir);
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
 		sprintf(command,"./binclient %s \"%s\"/\"%s\"",argv[1],argv[2],filename); 
 		
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
 		sprintf(command,"cp -n client \"%s\"/",argv[2]);
 		system(command);
 		sprintf(command,"cp -n binserver \"%s\"/",argv[2]);
 		system(command);
 		sprintf(command,"cp -n binclient \"%s\"/",argv[2]);
 		system(command);
 		sprintf(command,"(cd \"%s\";mkdir -p \"%s\")",argv[2],foldername);
 		system(command);
 		instance++;
 		sprintf(command,"(cd \"%s\";./client %s \"%s\" %d)",argv[2],argv[1],foldername,instance);
 		
 		rs=0;
 		rs=system(command);
 		while(rs==2)
 		{
 			rs=system(command);
 		}	
 		instance--;
 		sprintf(command,"rm \"%s\"/client",argv[2]);
    		system(command);
    		sprintf(command,"rm \"%s\"/binclient",argv[2]);
    		system(command);
    		sprintf(command,"rm \"%s\"/binserver",argv[2]);
    		system(command);
 		printf("made dir %s\n",foldername);
 		i++;
 	}
    	/*exit*/
    	
    	close(sockfd);
    	fclose(sysdir);
    	fclose(folder);
    	system("rm clisysdirlist");
    	system("rm servfolderlist");
    	return(1);
}
