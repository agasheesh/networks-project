int send_file(char *fileN,int sock_cli,char *dir)
{
  FILE *file;
  int size;
  const int bufferSize = 1024;
  char data[bufferSize];
  int count;
  int toSend = bufferSize;
  int send_len;
  char fileName[bufferSize];
  strcpy(fileName,dir);
  strcat(fileName,fileN);
  
  file = fopen(fileName,"r");
  if(file==NULL)
  {
    send(sock_cli,"block",6,0);
    perror("Error");
    return 0;
    //exit(0);
  }
  
  send(sock_cli,"allow",6,0);
  
  struct stat buf;
  stat(fileName,&buf);
  size = buf.st_size;
  //sprintf(fileSize,"%li",size);
  //printf("\nFile Size : %d %d\n\n",size, sizeof(size));
  printf("\n\nSending file....\n");
  
  if(send(sock_cli,fileN,bufferSize,0)==-1)
  {
      perror("Error sending file name");
      return 0;
      //exit(0);
  }
  
  if(send(sock_cli,&size,sizeof(size),0)==-1)
  {
      perror("Error sending file size");
      return 0;
      //exit(0);
  }
  
  
  /*while(!feof(file) || size!=0)
  {
    if(size<toSend)
      toSend=size;
    memset(data,0,toSend);
    count = fread(data,sizeof(char),toSend,file);
    if((send_len=send(sock_cli,data,count,0))==-1)
    {
      perror("Error sending");
      return 0;
      //exit(0);
    }
    size-=send_len;
       
  }*/
  
  while(!feof(file))
  {
    memset(data,0,bufferSize);
    count = fread(data,sizeof(char),bufferSize,file);
    if(send(sock_cli,data,count,0)==-1)
    {
      perror("Error sending");
      exit(0);
    }
       
  }
  printf("\nFile sent....\n");
  
  fclose(file);
  return 1;
  
}


int recv_file(int sock_conn,char *directory)
{
  const int bufferSize = 1024;
  char fileName[bufferSize];
  char data[bufferSize];
  char dir[bufferSize];
  strcpy(dir,directory);
  int toRead = bufferSize;
  int *fileSize;
  FILE *file;
  int count;
  int recv_len;
  char size[5];
  char proceed[6];
  
  if(recv(sock_conn,proceed,6,0)==-1)
  {
    perror("Error receiving proceed status");
    return 0;
    //exit(0);
  }
  
  if(strcmp(proceed,"block")==0)
  {
    char buf[6];
    printf("\n\nFile not found...\nEnter any letter to continue...");
    scanf("%s",buf);
    return 0;
  }
  
  memset(fileName,0,bufferSize);
  if(recv(sock_conn,fileName,bufferSize,0)==-1)
  {
    perror("Error receiving file name");
    return 0;
    //exit(0);
  }
  memset(size,0,sizeof(size));
  if(recv(sock_conn,size,4,0)==-1)
  {
    perror("Error receiving file size");
    return 0;
    //exit(0);
  }
  
  fileSize = (int *)size;
  printf("Receiving file %s......\n\n",fileName);
  printf("Size of file : %d %d\n",*fileSize, sizeof(fileSize));
  
  //fileName = strcat(fileName,"new");
  //printf("%s\n",fileName);
  //file = fopen(strcat(".\\received\\","hi.avi"),"w+");
  
  file = fopen(strcat(dir,fileName),"w");
  if(file==NULL)
  {
    perror("Error\n");
    return 0;
    //exit(0);
  }
  
  if(*fileSize==0)
    return 1;
  
  do
  {
    if(bufferSize>*fileSize)
      toRead = *fileSize;
    memset(data,0,toRead);
    recv_len = recv(sock_conn,data,toRead,0);
    if(recv_len==-1)
    {
      perror("Error receiving");
      exit(0);
    }
    //printf("\nreceived : %d %d\n",recv_len,*fileSize);
    count = fwrite(data,sizeof(char),recv_len,file);
    *fileSize-=recv_len;
  }while(*fileSize>0 && recv_len>0);
  
  fclose(file);
  return 1;
  
}