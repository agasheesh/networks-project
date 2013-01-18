#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "protocol.h"

#define DIR "receivedClient/"

void print_menu(char *name)
{
  system("clear");
  printf("Welcome %s!!!\n1 - Upload file to server\n2 - Download files from server\n3 - Get file list\n4 - Delete files\n5 - Exit\nEnter choice : ",name);
}

void print_welcome_menu()
{
  system("clear");
  printf("Welcome !!!\n1 - Login\n2 - Register\nEnter choice : ");
}

void print_file_list(int sock_cli)
{
  FILE *file;
  char filename[1024];
  char filepath[1044];
  char command[1030];
  int size;
  
  if(recv_file(sock_cli,DIR)==1)
    printf("\nFile List Downloaded successfully\n");
  
  strcpy(filepath,DIR);
  strcat(filepath,".l_i_s_t.txt");
  file = fopen(filepath,"r");
  system("clear");
  printf("*****List Of Files On Server*****\n\n");
  //printf("\nusername: %s pwd: %send\n",name,pwd);
  
  struct stat fileDes;
  stat(filepath,&fileDes);
  size = fileDes.st_size;
  if(size!=0)
  {
    while(!feof(file)) 
    {
      fscanf(file,"%s\n",filename);
      printf("%s\n",filename);
    }
  }
  fclose(file);
  strcpy(command,"unlink ");
  strcat(command,filepath);
  //printf("\n%s\n",command);
  system(command);
  fflush(stdout);
  char *buf;
  printf("\nPress any letter to continue...");
  scanf("%s",buf);
}

void login_and_register(int sock_cli,char *choice)
{
  int cont = 1;
  do {
    
    print_welcome_menu();
    scanf("%s",choice);
    printf("\nchoice : %s\n",choice);
    fflush(stdin);
    if(strcmp(choice,"1")==0)
    {
      send(sock_cli,choice,2,0);
      cont=0;
    }
    else if(strcmp(choice,"2")==0)
    {
      send(sock_cli,choice,2,0);
      cont=0;
    }		
    else
      printf("\nInvalid arguments\n");
  }while(cont);  
}

void enter_details(int sock_cli,char *name,char *pwd,char *str)
{
  char authresult[6];
  do {
    system("clear");
    printf("%s\n\n",str);
    printf("Username : ");
    scanf("%s",name);
    printf("\nPassword : ");
    scanf("%s",pwd);
    send(sock_cli,name,21,0);
    send(sock_cli,pwd,21,0);
    recv(sock_cli,authresult,6,0);
  }while(strcmp(authresult,"allow")!=0);
  
}

void getFileName(char *fileName)
{
  printf("\nEnter file name :");
  scanf("%s",fileName);
}

int main(int argc,char *arg[])
{
  int sock_cli;		// Stores file descriptor of client socket
  struct sockaddr_in address;
  struct in_addr ipaddr;
  size_t msglen = 0;
  FILE *file;
  int size;
  const int bufferSize = 1024;
  char fileSize[bufferSize];
  char data[bufferSize];
  int count;
  char fileName[bufferSize];
  char choice[2];
  char name[21];
  char pwd[21];
  
  /* Stores length of message received from server
     Useful to determine abnormal loss of connection 
     with server e.g. Ctrl-C on server */
  
  int recv_len;		
  
  if (argc != 3) 	// Checking argument count
  {
    perror("Incorrect arguments");
    exit(EXIT_FAILURE);
  }

  // Storing IP of server in ipaddr

  if (inet_aton(arg[1],&ipaddr) == 0)  
  {
    perror("Incorrect address");
    exit(EXIT_FAILURE);
  }

  // Creating client socket

  if ((sock_cli=socket(AF_INET,SOCK_STREAM,0))==0)
  {
    perror("Error creating socket");
    exit(EXIT_FAILURE);
  }

  // assigning port number, address and family

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = ipaddr.s_addr;
  address.sin_port = htons(atoi(arg[2]));
  
  // Connecting to server
  
  if(connect(sock_cli,(struct sockaddr *)&address,sizeof(address))<0)
  {
    perror("Error connecting with server");
    exit(EXIT_FAILURE);
  }
  
  printf("Connected to server...\n\n");
  
  // Login and Register
  
  login_and_register(sock_cli,choice);
  if(strcmp(choice,"1")==0)
    enter_details(sock_cli,name,pwd,"*****Authentication*****");
  else if(strcmp(choice,"2")==0)
    enter_details(sock_cli,name,pwd,"*****Register*****");
  
  
  
  int cont = 1;
  do {
    
    print_menu(name);
    //fgets(choice,2,stdin);
    scanf("%s",choice);
    //printf("\nchoice : %s\n",choice);
    fflush(stdin);
    if(strcmp(choice,"1")==0)
    {
      getFileName(fileName);
      if(fopen(fileName,"r")==NULL)
      {
	char buf[2];
	printf("\n\nFile not found...\nEnter any letter to continue...");
	scanf("%s",buf);
      }
      else{
	send(sock_cli,choice,2,0);
	if(send_file(fileName,sock_cli,"")==1)
	  printf("File Uploaded successfully");
	else
	  printf("Error Uploading File");
	fflush(stdout);
      }
    }
    else if(strcmp(choice,"2")==0)
    {
      send(sock_cli,choice,2,0);
      getFileName(fileName);
      printf("\nRequested for file %s\n",fileName);
      send(sock_cli,fileName,bufferSize,0);
      if(recv_file(sock_cli,DIR)==1)
	printf("File Downloaded successfully");
      fflush(stdout);
    }
    else if(strcmp(choice,"3")==0)
    {
      send(sock_cli,choice,2,0);
      print_file_list(sock_cli);
    }
    else if(strcmp(choice,"4")==0)
    {	
      char buf[6];
      send(sock_cli,choice,2,0);
      getFileName(fileName);
      send(sock_cli,fileName,bufferSize,0);
      recv(sock_cli,buf,6,0);
      if(strcmp(buf,"block")==0)
      {
	printf("\n\nFile not found on server...\nEnter any letter to continue...");
	scanf("%s",buf);
      }
      else if(strcmp(buf,"allow")==0)
      {
	printf("\n\nFile deleted...\nEnter any letter to continue...");
	scanf("%s",buf);
      }
      
    }
    else if(strcmp(choice,"5")==0)
    {	
      send(sock_cli,choice,2,0);
      cont=0;
    }
    else
      printf("\nInvalid arguments\n");
  }while(cont);
  
  close(sock_cli);		// Shutting down client socket
}
