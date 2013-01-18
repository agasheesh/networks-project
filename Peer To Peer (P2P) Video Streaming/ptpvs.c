#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>

#define LEN 30

int port=1235;

main(int argc, char *argv[])
{
  if (argc != 3){
    fprintf (stderr, "Usage:%s <host> <port>\n",
            argv[0]);
  }
  
  int pid = fork();
  
  if(pid!=0) {
    enter(argv);
    
    char b[60];
  
    while(1) {
      system("clear");
      printf("1. Get list of files\n2. Request video\n3. Exit\n");
      gets(b);
      if(strcmp("1", b)==0) {
	displaylist(argv);
      }
      else if(strcmp("2", b)==0) {
	printf("Enter the name of the file to request\n");
	gets(b);
	printf("Getting the list of IPs\n");
	getips(argv, b);
      }
      else if(strcmp("3", b)==0) {
	break;
      }
      else {
	printf("Invalid arguement.\nPress any key to continue...");
	gets(b);
      }
    }
    stopstreamserver();
    exit1(argv);
  }
  else {
    startstreamserver();
  }
}

int enter(char *argv[]) {
  struct sockaddr_in cli_addr, serv_addr;
  struct hostent *server;
  int s, buf_len;
  char buf[LEN], buf2[sizeof(int)];
  
  if ((s=socket(AF_INET,SOCK_STREAM,0))== -1){
      perror("socket");
      return -1;
  }
  
  bzero(&cli_addr, sizeof(cli_addr));
  cli_addr.sin_family = AF_INET;
  cli_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  cli_addr.sin_port = htons(0);
  
  if (bind(s, (struct sockaddr *) &cli_addr, sizeof(cli_addr)) == -1) {
      perror("bind");
      return -1;
  }

  if ((server=gethostbyname(argv[1]))==NULL){
      perror("gethostbyname");
      return -1;
  }
  
  bzero (&serv_addr, sizeof (serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = *((ulong *)server->h_addr);
  serv_addr.sin_port = htons (atoi(argv[2]));
  
  if (connect(s,(struct sockaddr *)&serv_addr, sizeof (serv_addr)) == -1) {
     perror("connect");
     return -1;
  }
  
  strcpy(buf, "enter");
  int len=strlen(buf)+1;
  send(s, &len, sizeof(int), 0);
  send(s, buf, strlen(buf)+1,0);
  
  system("ls share > templist.send");
  FILE *fp;
  int counter = 0;
  fp=fopen("templist.send", "r");
  while(fscanf(fp, "%s", buf)!=EOF) {
    counter++;
  }
  
  send(s, &counter, sizeof(int), 0);
  fseek(fp, 0, SEEK_SET);
   
  while(fscanf(fp, "%s", buf)!=EOF) {
    counter=strlen(buf)+1;
    send(s, &counter, sizeof(int), 0);
    send(s, buf, strlen(buf)+1, 0);
    printf("%s\n", buf);
  }
  fclose(fp);
  unlink("templist.send");
  close(s);
}

int exit1(char *argv[]) {
  struct sockaddr_in cli_addr, serv_addr;
  struct hostent *server;
  int s, buf_len;
  char buf[LEN];
  
  if ((s=socket(AF_INET,SOCK_STREAM,0))== -1){
      perror("socket");
      return -1;
  }
  
  bzero(&cli_addr, sizeof(cli_addr));
  cli_addr.sin_family = AF_INET;
  cli_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  cli_addr.sin_port = htons(0);
  
  if (bind(s, (struct sockaddr *) &cli_addr, sizeof(cli_addr)) == -1) {
      perror("bind");
      return -1;
  }

  if ((server=gethostbyname(argv[1]))==NULL){
      perror("gethostbyname");
      return -1;
  }
  
  bzero (&serv_addr, sizeof (serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = *((ulong *)server->h_addr);
  serv_addr.sin_port = htons (atoi(argv[2]));
  
  if (connect(s,(struct sockaddr *)&serv_addr, sizeof (serv_addr)) == -1) {
     perror("connect");
     return -1;
  }
  
  strcpy(buf, "exit");
  int len=strlen(buf)+1;
  send(s, &len, sizeof(int), 0);
  send(s, buf, strlen(buf)+1,0);
  
  close(s);
}

int displaylist(char *argv[]) {
  struct sockaddr_in cli_addr, serv_addr;
  struct hostent *server;
  int s, buf_len;
  char buf[LEN], buf2[sizeof(int)];
  
  if ((s=socket(AF_INET,SOCK_STREAM,0))== -1){
      perror("socket");
      return -1;
  }
  
  bzero(&cli_addr, sizeof(cli_addr));
  cli_addr.sin_family = AF_INET;
  cli_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  cli_addr.sin_port = htons(0);
  
  if (bind(s, (struct sockaddr *) &cli_addr, sizeof(cli_addr)) == -1) {
      perror("bind");
      return -1;
  }

  if ((server=gethostbyname(argv[1]))==NULL){
      perror("gethostbyname");
      return -1;
  }
  
  bzero (&serv_addr, sizeof (serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = *((ulong *)server->h_addr);
  serv_addr.sin_port = htons (atoi(argv[2]));
  
  if (connect(s,(struct sockaddr *)&serv_addr, sizeof (serv_addr)) == -1) {
     perror("connect");
     return -1;
  }
  
  strcpy(buf, "list");
  int len=strlen(buf)+1;
  send(s, &len, sizeof(int), 0);
  send(s, buf, strlen(buf)+1,0);
  
  int *i, num, count;
  memset(buf2, 0, sizeof(int));
  recv(s, buf2, sizeof(int), 0);
  i=(int)buf2;
  count=*i;
  while(count-->0) {
    num=recv(s, buf2, sizeof(int), 0);
    i=(int)buf2;
    num=recv(s, buf, *i, 0);
    buf[num]='\0';
    if(num==-1)
      break;
    printf("%s  ", buf, *i);
  }
  gets(buf);
  close(s);
}

int getips(char *argv[], char *file) {
  struct sockaddr_in cli_addr, serv_addr;
  struct hostent *server;
  int s, buf_len;
  char buf[LEN], buf2[sizeof(int)];
  
  if ((s=socket(AF_INET,SOCK_STREAM,0))== -1){
      perror("socket");
      return -1;
  }
  
  bzero(&cli_addr, sizeof(cli_addr));
  cli_addr.sin_family = AF_INET;
  cli_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  cli_addr.sin_port = htons(0);
  
  if (bind(s, (struct sockaddr *) &cli_addr, sizeof(cli_addr)) == -1) {
      perror("bind");
      return -1;
  }

  if ((server=gethostbyname(argv[1]))==NULL){
      perror("gethostbyname");
      return -1;
  }
  
  bzero (&serv_addr, sizeof (serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = *((ulong *)server->h_addr);
  serv_addr.sin_port = htons (atoi(argv[2]));
  
  if (connect(s,(struct sockaddr *)&serv_addr, sizeof (serv_addr)) == -1) {
     perror("connect");
     return -1;
  }
  
  strcpy(buf, "getips");
  int len=strlen(buf)+1;
  send(s, &len, sizeof(int), 0);
  send(s, buf, strlen(buf)+1,0);
  
  len=strlen(file)+1;
  send(s, &len, sizeof(int), 0);
  send(s, file, strlen(file)+1,0);
  int *i, num;
  memset(buf2, 0, sizeof(int));
  while(1) {
    num=recv(s, buf2, sizeof(int), 0);
    if(num<=0)
      break;
    i=(int)buf2;
    num=recv(s, buf, *i, 0);
    buf[num]='\0';
    if(num==-1)
      break;
    if(strcmp(buf, "<end>")==0)
      break;
    else {
      requestvid(buf, file);
      break;
    }
    printf("%s\n", buf, *i);
  }
  gets(buf);
  close(s);
}

int requestvid(char *add, char *file) {
  struct sockaddr_in cli_addr, serv_addr;
  struct hostent *server;
  int s;
  char buf[LEN], command[LEN];
  
  if ((s=socket(AF_INET,SOCK_STREAM,0))== -1){
      perror("socket");
      return -1;
  }
  
  bzero(&cli_addr, sizeof(cli_addr));
  cli_addr.sin_family = AF_INET;
  cli_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  cli_addr.sin_port = htons(0);
  
  if (bind(s, (struct sockaddr *) &cli_addr, sizeof(cli_addr)) == -1) {
      perror("bind");
      return -1;
  }

  if ((server=gethostbyname(add))==NULL){
      perror("gethostbyname");
      return -1;
  }
  
  bzero (&serv_addr, sizeof (serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = *((ulong *)server->h_addr);
  serv_addr.sin_port = htons (port);
  
  if (connect(s,(struct sockaddr *)&serv_addr, sizeof (serv_addr)) == -1) {
     perror("connect");
     return -1;
  }
  
  strcpy(buf, "stream");
  int len=strlen(buf)+1;
  send(s, &len, sizeof(int), 0);
  send(s, buf, strlen(buf)+1,0);
  
  strcpy(buf, file);
  len=strlen(buf)+1;
  send(s, &len, sizeof(int), 0);
  send(s, buf, strlen(buf)+1,0);
  
  int *length;
  char buf2[sizeof(int)];
  recv(s, buf2, sizeof(int), 0);
  length=(int)buf2;
  recv(s, buf, *length, 0);
  
  strcpy(command, "vlc -q udp://@");
  strcat(command, buf);
  strcat(command, ":1234 &");
  
  system(command);
  
  close(s);
}

int stopstreamserver() {
  struct sockaddr_in cli_addr, serv_addr;
  struct hostent *server;
  int s, buf_len;
  char buf[LEN];
  
  if ((s=socket(AF_INET,SOCK_STREAM,0))== -1){
      perror("socket");
      return -1;
  }
  
  bzero(&cli_addr, sizeof(cli_addr));
  cli_addr.sin_family = AF_INET;
  cli_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  cli_addr.sin_port = htons(0);
  
  if (bind(s, (struct sockaddr *) &cli_addr, sizeof(cli_addr)) == -1) {
      perror("bind");
      return -1;
  }

  if ((server=gethostbyname("127.0.0.1"))==NULL){
      perror("gethostbyname");
      return -1;
  }
  
  bzero (&serv_addr, sizeof (serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = *((ulong *)server->h_addr);
  serv_addr.sin_port = htons (port);
  
  if (connect(s,(struct sockaddr *)&serv_addr, sizeof (serv_addr)) == -1) {
     perror("connect");
     return -1;
  }
  
  strcpy(buf, "stop");
  int len=strlen(buf)+1;
  send(s, &len, sizeof(int), 0);
  send(s, buf, strlen(buf)+1,0);
  
  close(s);
}

int startstreamserver() {
  system("./streamserver.o &");
}