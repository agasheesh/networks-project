#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/stat.h>
#include "protocol.h"

int search_for_login_details(char *name,char *pwd)
{
  FILE *file;
  file = fopen("database.txt","r");
  char readname[21];
  char readpwd[21];
  //printf("\nusername: %s pwd: %send\n",name,pwd);
  do {
    fscanf(file,"username:%s\n",readname);
    fscanf(file,"password:%s\n",readpwd);
    if(strcmp(name,readname)==0 && strcmp(pwd,readpwd)==0)
    {
      fclose(file);
      return 1;
    }
  }while(!feof(file));
  fclose(file);
  return 0;
}

int search_for_username(char *name)
{
  FILE *file;
  file = fopen("database.txt","r");
  char readname[21];
  char readpwd[21];
  //printf("\nusername: %s pwd: %send\n",name,pwd);
  do {
    fscanf(file,"username:%s\n",readname);
    fscanf(file,"password:%s\n",readpwd);
    if(strcmp(name,readname)==0)
    {
      fclose(file);
      return 1;
    }
  }while(!feof(file));
  fclose(file);
  return 0;
}

void login(int sock_conn,char *name,char *pwd)
{
  int res;
  do {
    recv(sock_conn,name,21,0);
    recv(sock_conn,pwd,21,0);
    res = search_for_login_details(name,pwd);
    if(res==1)
    {
      send(sock_conn,"allow",6,0);
    }
    else
    {
      send(sock_conn,"block",6,0);
    }
  }while(res!=1);
}

void regis(int sock_conn,char *name,char *pwd)
{
  int res;
  do {
    recv(sock_conn,name,21,0);
    recv(sock_conn,pwd,21,0);
    res = search_for_username(name);
    if(res==0)
    {
      FILE *file;
      file = fopen("database.txt","a");
      fputs("username:",file);
      fputs(name,file);
      fputs("\n",file);
      fputs("password:",file);
      fputs(pwd,file);
      fputs("\n",file);
      fclose(file);
      char command[1030];
      strcpy(command,"mkdir ");
      strcat(command,name);
      system(command);
      send(sock_conn,"allow",6,0);
    }
    else
    {
      send(sock_conn,"block",6,0);
    }
  }while(res!=0);  
}

int send_file_list(int sock_conn,char *dir)
{
  char command[1030];
  char filepath[1044];
  strcpy(command,"ls ");
  strcat(command,dir);
  strcat(command," > ");
  strcpy(filepath,dir);
  strcat(filepath,".l_i_s_t.txt");
  strcat(command,filepath);
  system(command);
  //file = fopen("l_i_s_t.txt","r");
  if(send_file(".l_i_s_t.txt",sock_conn,dir)==1)
  {
    printf("File List Uploaded successfully");
  }
  else
  {
    printf("Error Uploading File List");	    
    return 0;
  }

  strcpy(command,"unlink ");
  strcat(command,filepath);
  printf("\nCommand:%s\n",command);
  fflush(stdout);
  system(command);
  return 1;
}

int main(int argc,char *arg[])
{
 
  int sock_server;
  struct sockaddr_in address;
  int addrlen;
  int sock_conn,sock_conn1;
  const int bufferSize = 1024;
  char data[bufferSize];
  char fileName[bufferSize];
  char size[5];
  int *fileSize;
  FILE *file;
  int count;
  size_t msglen = 0;
  long bytesRead = 0L;
  int toRead = bufferSize; 
  int recv_len;
  pid_t pid;
  char choice[2];
  char name[21];
  char pwd[21];
  char dir[22];  

  if (argc != 2) 	// Checking argument count
  {
    perror("Incorrect arguments");
    exit(EXIT_FAILURE);
  }
  
  // assigning port number, address and family
 
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(atoi(arg[1]));
  addrlen=sizeof(address);

  // Creating server socket
  
  if ((sock_server=socket(AF_INET,SOCK_STREAM,0))==0)
  {
    perror("Create socket");
    exit(EXIT_FAILURE);
  }
  
  // Binding the server socket with address
  
  if (bind(sock_server,(struct sockaddr *)&address,sizeof(address))<0)
  {
    perror("bind");
    exit(EXIT_FAILURE);
  }
 
    printf("\nWaiting for connections...\n\n");
  
    // Listening for connections
    
  if (listen(sock_server,5)<0)
  {
    perror("listen");
    exit(EXIT_FAILURE);
  }
  
  // Accepting connection with client
  
  do{
    
    if ((sock_conn=accept(sock_server,(struct sockaddr *)&address,&addrlen))<0)
    {
      perror("accept");
      exit(EXIT_FAILURE);
    }
    
    pid = fork();
    
    if(pid!=0) {
      printf("Connected to client...\n\n");
    }
    else if(pid==0) {
      
      // Login and Register
      
      
      
      int cont = 1;
      do {
	recv(sock_conn,choice,2,0);
	//printf("\nchoice : %s\n",choice);
	if(strcmp(choice,"1")==0)
	{
	  login(sock_conn,name,pwd);
	  cont=0;
	}
	else if(strcmp(choice,"2")==0)
	{
	  regis(sock_conn,name,pwd);
	  cont=0;
	}		
	else
	  printf("\nInvalid arguments\n");
      }while(cont);
      
      
      
      strcpy(dir,name);
      strcat(dir,"/");
      cont = 1;
      do {
	recv(sock_conn,choice,2,0);
	printf("\nchoice:%s\n",choice);
	if(strcmp(choice,"1")==0)
	{ 
	  recv_file(sock_conn,dir);
	  printf("File Received Successfully");
	  fflush(stdout);
	}
	else if(strcmp(choice,"2")==0)
	{ 
	  printf("\nchoice:%s\n",choice);
	  char filepath[bufferSize];
	  strcpy(filepath,dir);
	  if(recv(sock_conn,fileName,bufferSize,0)==-1)
	  {
	    perror("Error receiving file name");
	  }
	  printf("\nSending file %s\n",fileName);
	  strcat(filepath,fileName);
	  //printf("\nFile path:%s\n",filepath);
	  if(send_file(fileName,sock_conn,dir)==1)
	  {
	    printf("File Uploaded successfully");
	  }
	  else
	  {
	    printf("Error Uploading File");	    
	  }
	  fflush(stdout);

	}
	else if(strcmp(choice,"3")==0)
	{ 
	  /*if(send_file("filelist.txt",sock_conn,dir)==1)
	  {
	    printf("File List Uploaded successfully");
	  }
	  else
	  {
	    printf("Error Uploading File List");	    
	  }*/
	  printf("\nchoice:%s\n",choice);
	  send_file_list(sock_conn,dir);
	}
	else if(strcmp(choice,"4")==0)
	{ 
  	  printf("\nchoice:%s\n",choice);
	  if(recv(sock_conn,fileName,bufferSize,0)==-1)
	  {
	    perror("Error receiving file name");
	  }
	  
	  char filepath[bufferSize];
	  strcpy(filepath,dir);
	  strcat(filepath,fileName);
	  
	  file = fopen(filepath,"r");
	  if(file==NULL)
	  {
	    send(sock_conn,"block",6,0);
	    continue;
	  }
	  fclose(file);
	  send(sock_conn,"allow",6,0);
	  
	  char command[bufferSize];
	  strcpy(command,"rm ");
	  strcat(command,filepath);
	  system(command);
	}
	else if(strcmp(choice,"5")==0)
	{ 
	  cont = 0;
	}
      }while(cont);
    }
    
  }while(pid!=0);
  
    
  //fflush(file);
  // Closing connections
  close(sock_conn);
  if(pid!=0)
    close(sock_server);
}
