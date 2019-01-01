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
#include<signal.h>
#include<errno.h>


#define EVENT_SIZE  ( sizeof (struct inotify_event) )
#define EVENT_BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )

void signal_callback_handler(int signum)

{
 		exit(0);
}


void main(int argc, char *argv[])
{
	
	char monitor[100],command[100],ip[INET_ADDRSTRLEN],buffer[100],buffer2[EVENT_BUF_LEN],c,filename[40];
	int arg=argc,sockfd,newsockfd,portno,iSetOption = 1,err,x,fd,wd,length,i,read1,n,a=0;
	pid_t pid;
	socklen_t clilen,len=sizeof(err);
	fd_set sock,writesock;
	struct sockaddr_in serv_addr,cli_addr;
	signal(SIGKILL, signal_callback_handler);
	signal(SIGHUP,signal_callback_handler);
	if(arg==1)
	{
		printf("use swish as\n1)server\n2)client\n");
		scanf("%s",monitor);
		if (strcmp(monitor, "server") == 0)
		{
			printf("folder to be swished:-\n");
			scanf("%s",monitor);
			arg=2;
		}
		else
		if (strcmp(monitor, "client") == 0)
		{
			printf("folder to be swished:-\n");
			scanf("%s",monitor);
			printf("IP of Swish server:-\n");
			scanf("%s",ip);
			arg=3;
		}
		else
			return;
	}
	if(argc==2)
	{
		sprintf(monitor,"%s",argv[1]);
		printf("%s\n",monitor);
	}
	if(arg==2)//server
	{
		sprintf(command,"./server \"%s\"",monitor);
		printf("2");
		system(command);
		
		/*socket creation*/
	sockfd=socket(AF_INET,SOCK_STREAM,0);
	if(sockfd<0)
	{
		printf("socket creation error\n");
		exit(1);
	}
	err=setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char*)&iSetOption,sizeof(iSetOption));
	if (err!=0)
    		printf("error setsock\n%s\n",strerror(err));
	/*bind*/
	portno=48888;
	printf("port used-%d\n",portno);
	bzero((char *)&serv_addr,sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_addr.sin_port=htons(portno);
	bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
	
	/*listen*/
	listen(sockfd,100);
	clilen=sizeof(cli_addr);
	
	/*accept incoming connection*/
	newsockfd=accept(sockfd,(struct sockaddr *)&cli_addr,&clilen);
	if(newsockfd<0)
	{
		printf("error accept()");
		exit(1);
	}
	printf("accepted connection\n");
	
	
		
		pid=fork();
		if(pid>0)//child
		{
			fd = inotify_init();
		if ( fd < 0 ) {
			printf("failed to initialise inotify\n%s\n",strerror(errno));
    			return;
  		}
  		wd = inotify_add_watch( fd, monitor, IN_CREATE );	
  		if(wd==-1){
  			printf("error add watch\n%s\n",strerror(errno));
  			exit(0);
  			}
  			FD_ZERO(&sock);
			FD_SET(fd,&sock);
			FD_ZERO(&writesock);
			FD_SET(newsockfd,&writesock);
  			while(1)
  			{
  				
  				err=select(FD_SETSIZE,&sock,NULL,NULL,NULL);
  				if(err==-1)
  					printf("error select\n");
  				length = read( fd, buffer2, EVENT_BUF_LEN );
  				if ( length < 0 ) {
    					printf( "read error" );
    					return;
  				}
  				i=0;
  				while ( i < length ) {     
  				struct inotify_event *event = ( struct inotify_event * ) &buffer2[ i ];     
  					if ( event->len ) {
      						if ( event->mask & IN_CREATE ) 
      						{
        						if ( event->mask & IN_ISDIR ) {
          							printf( "New directory %s created.\n", event->name );
          							
          							err=select(FD_SETSIZE,NULL,&writesock,NULL,NULL);
          							if(err==-1)
  					printf("error select\n");
  					
          							err=write(newsockfd,"dircreated",sizeof(buffer));
          							if(err==-1)
          								printf("write error\n");
          							sprintf(buffer,"\"%s\"",event->name);
          							
          							n=0;
          							a=0;
 				while(1)
 				{
 					c=buffer[a];
 					if(c=='\0'||c=='\n'||c==EOF)
 					{
 						filename[n]='\0';
 						break;
 					}
 					
 					filename[n]=c;
 					n++;
 					a++;
 				}
          							
          							err=select(FD_SETSIZE,NULL,&writesock,NULL,NULL);
          							if(err==-1)
  					printf("error select\n");
          							err=write(newsockfd,filename,sizeof(filename));
          							
          							printf("written %s\n",filename);
          							
        						}
        						else {
          							printf( "New file %s created.\n", event->name );
          							
        						}
      						}
    					}
    				i += EVENT_SIZE + event->len;
  				}
			}
}
			
		
		else
		if(pid==0)//parent
		{
			
			printf("waiting read\n");
			while(1){
			FD_ZERO(&sock);
			FD_SET(newsockfd,&sock);
			err=select(FD_SETSIZE,&sock,NULL,NULL,NULL);
			if(err==-1)
  					printf("error select\n");
			x=read(newsockfd,buffer,sizeof(buffer));
			if(x==-1)
				printf("read error\n");
			if(strcmp(buffer,"dircreated") == 0)
			{
				
				printf("directory created on the client\n");
				err=select(FD_SETSIZE,&sock,NULL,NULL,NULL);
				if(err==-1)
  					printf("error select\n");
				x=read(newsockfd,filename,sizeof(filename));
				if(x==-1)
				printf("read error\n");
				sprintf(command,"(cd \"%s\";mkdir -p %s)",monitor,filename);		
				printf("%s\n",command);
				printf("3");
				system(command);
				
				
			}
			err=0;
				if(getsockopt (newsockfd, SOL_SOCKET, SO_ERROR, &err, &len ))
				{
					printf("connection on client closed\n");
					exit(0);
				}
			}
		}
		else
			printf("error fork()\n");
		/*exit*/
		printf("out\n");
		close(newsockfd);
		close(sockfd);
		exit(0);
	}
	if(argc==3)
	{
		sprintf(monitor,"%s",argv[2]);
		sprintf(ip,"%s",argv[1]);
	}
	if(arg==3)//client
	{
		sprintf(command,"./client %s \"%s\"",ip,monitor);
		printf("44\n");
		system(command);
		printf("system runned 1");
		portno=48889;
		printf("system runned 12");
		// printf("port used- %d \n",portno);
		printf("system runned 13");
		printf("system runned 30");
	    	serv_addr.sin_family = AF_INET;
	    	printf("system runned 20");
	    	serv_addr.sin_port = htons(portno);
	    	printf("system runned 10");
	    	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	    	{
			printf("\n Error : Could not create socket \n");
			exit(1);
	    	}
	    	printf("system runned 2"); 
	err=setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char*)&iSetOption,sizeof(iSetOption));
    	if (err!=0)
    		printf("error setsock\n%s\n",strerror(err));
    	if((err=inet_pton(AF_INET, argv[1], &serv_addr.sin_addr))<=0)
   	 {
       	 	if(err==-1)
       	 		printf("\n inet_pton error occured\n");
       	 	else
       	 		printf("invalid ip\n");
       	 	exit(1);
  	 }
  	 printf("system runned 3"); 
	x=connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
	printf("system runned 4");
       	while(x < 0){
       		printf("try\n");
       		sleep(1);
       		x=connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    	}
    	printf("connected\n");
    	printf("%s\n",strerror(x));
    
    	pid=fork();
		if(pid>0)//child
		{
			fd = inotify_init();
		if ( fd < 0 ) {
			printf("failed to initialise inotify\n%s\n",strerror(errno));
    			return;
  		}
  		wd = inotify_add_watch( fd, monitor, IN_CREATE );	
  		if(wd==-1){
  			printf("error add watch\n%s\n",strerror(errno));
  			exit(0);
  			}
  			FD_ZERO(&sock);
			FD_SET(fd,&sock);
			FD_ZERO(&writesock);
			FD_SET(sockfd,&writesock);
  			while(1)
  			{
  			
  				err=select(FD_SETSIZE,&sock,NULL,NULL,NULL);
  				if(err==-1)
  					printf("error select\n");
  				length = read( fd, buffer2, EVENT_BUF_LEN );
  				if ( length < 0 ) {
    					printf( "read error" );
    					return;
  				}
  				i=0;
  				while ( i < length ) {     
  				struct inotify_event *event = ( struct inotify_event * ) &buffer2[ i ];     
  					if ( event->len ) {
      						if ( event->mask & IN_CREATE ) 
      						{
        						if ( event->mask & IN_ISDIR ) {
          							printf( "New directory %s created.\n", event->name );
          							
          							err=select(FD_SETSIZE,NULL,&writesock,NULL,NULL);
          							if(err==-1)
  					printf("error select\n");
          							err=write(sockfd,"dircreated",sizeof(buffer));
          							if(err==-1)
          								printf("error write\n");
          							sprintf(buffer,"\"%s\"",event->name);
          						
          							n=0;
          							a=0;
 				while(1)
 				{
 					c=buffer[a];
 					if(c=='\0'||c=='\n'||c==EOF)
 					{
 						filename[n]='\0';
 						break;
 					}
 					
 					filename[n]=c;
 					n++;
 					a++;
 				}
          							
          							err=select(FD_SETSIZE,NULL,&writesock,NULL,NULL);
          							if(err==-1)
  					printf("error select\n");
          							err=write(sockfd,filename,sizeof(filename));
          							if(err==-1)
          								printf("error write\n");
          							printf("written %s\n",filename);
          							
        						}
        						else {
          							printf( "New file %s created.\n", event->name );
          							
        						}
      						}
    					}
    				i += EVENT_SIZE + event->len;
  				}
			}
}
			
		
		else
		if(pid==0)//parent
		{
			printf("waiting read\n");
			FD_ZERO(&sock);
			FD_SET(sockfd,&sock);
			while(1){
			
			err=select(FD_SETSIZE,&sock,NULL,NULL,NULL);
			if(err==-1)
  					printf("error select\n");
			x=read(sockfd,buffer,sizeof(buffer));
			if(x==-1)
				printf("error read\n");
			if(strcmp(buffer,"dircreated") == 0)
			{
				
				printf("dir created on server\n");
				err=select(FD_SETSIZE,&sock,NULL,NULL,NULL);
				if(err==-1)
  					printf("error select\n");
				x=read(sockfd,filename,sizeof(filename));
				if(x==-1)
				printf("error read\n");
				sprintf(command,"(cd \"%s\";mkdir -p %s)",monitor,filename);		
				printf("%s\n",command);
				printf("1");
				system(command);
			}
			err=0;
				if(getsockopt (sockfd, SOL_SOCKET, SO_ERROR, &err, &len ))
				{
					printf("connection on server closed\n");
					exit(0);
				}	
			}
			
		}
		else
			printf("error fork()\n");
    	
}
}
