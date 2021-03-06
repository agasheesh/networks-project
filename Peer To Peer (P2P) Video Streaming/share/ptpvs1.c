#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>

#define LEN 40

main(int argc, char *argv[])
{
  struct sockaddr_in cli_addr, serv_addr;
  struct hostent *server;
  int s, buf_len;
  char *buf, buf2[sizeof(int)];
  int num = 0;
  
  buf=(char *)malloc(LEN+1);
  if (argc != 3){
    fprintf (stderr, "Usage:%s <host> <port>\n",
            argv[0]);
    //exit(1);
  }
  
  if ((s=socket(AF_INET,SOCK_STREAM,0))== -1){
      perror("socket");
      //exit(1);
  }
  
  bzero(&cli_addr, sizeof(cli_addr));
  cli_addr.sin_family = AF_INET;
  cli_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  cli_addr.sin_port = htons(0);
  
  if (bind(s, (struct sockaddr *) &cli_addr, sizeof(cli_addr)) == -1) {
      perror("bind");
      //exit(1);
  }

  if ((server=gethostbyname(argv[1]))==NULL){
      perror("gethostbyname");
      //exit(1);
  }
  
  bzero (&serv_addr, sizeof (serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = *((ulong *)server->h_addr);
  serv_addr.sin_port = htons (atoi(argv[2]));
  
  if (connect(s,(struct sockaddr *)&serv_addr, sizeof (serv_addr)) == -1) {
     perror("connect");
     //exit(1);
  }
  printf("Connected to server...\nType 'exit' to quit...\n");
  do {
     num++;
//     fflush(stdin);
     gets(buf);
     send(s, buf, strlen(buf)+1,0);
     if(strcmp("lis", buf)==0) {
	int *i;
	memset(buf2, 0, sizeof(int));
       while((num=recv(s, buf2, sizeof(int), 0))>0) {
	 //buf[num]='\0';
	 //printf("%d ", num);
	i=(int)buf2;
	num=recv(s, buf, *i, 0);
	buf[num]='\0';
	 if(num==-1)
	   break;
	 printf("%s %d  \n", buf, *i);
       }
     }
     if(strcmp("getips", buf)==0) {
       gets(buf);
       send(s, buf, strlen(buf)+1,0);
	int *i;
	memset(buf2, 0, sizeof(int));
       while((num=recv(s, buf2, sizeof(int), 0))>0) {
	i=(int)buf2;
	num=recv(s, buf, *i, 0);
	buf[num]='\0';
	 if(num==-1)
	   break;
	printf("%s", buf);
       }
     }
  }while(strcmp(buf,"exit")!=0);
  
  free(buf);
  close(s);
}

