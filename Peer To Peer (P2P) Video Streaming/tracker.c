#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

#define MAX_LENGTH 40

int main(int argc, char *argv[]) {
  struct sockaddr_in cli_addr, serv_addr;
  int s, c, c_len, buf_len, pid;
  char buf[MAX_LENGTH], buf2[sizeof(int)], ip[17];
    
  if ((s=socket(AF_INET,SOCK_STREAM,0))== -1){
      perror("socket");
      exit(1);
  }
  
  bzero (&serv_addr, sizeof (serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin_port = htons (atoi(argv[1]));
  
  if (bind(s, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1) {
      perror("bind");
      exit(1);
  }

  if(listen(s, 5)== -1) {
    perror("listen");
    //exit(1);
  }
    
  while(1)
  {
    
    if ((c=accept(s,(struct sockaddr *)&cli_addr, &c_len)) == -1) {
     perror("accept");
     //exit(1);
    }
     
    pid = fork();
    
    if(pid!=0)
      printf("Connected\n");
    else {
      recv(c, buf2, sizeof(int), 0);
      int *i;
      i=(int)buf2;
      buf_len=recv(c, buf, *i, 0);
      printf("%d\n",buf_len);
      
      if(buf_len!=-1) {
	if(strcmp("enter", buf)==0) {
	  strcpy(ip, (char *)inet_ntoa(cli_addr.sin_addr));
	  printf("peer address from accept() is %s\n",ip);
	  enterIP(ip, c);
	  printf("done\n");
	}
	else if(strcmp("exit", buf)==0) {
	  strcpy(ip, (char *)inet_ntoa(cli_addr.sin_addr));
	  printf("peer address from accept() is %s\n",ip);
	  exitIP(ip);
	}
	else if(strcmp("list", buf)==0) {
	  sendlist(c);
	}
	else if(strcmp("getips", buf)==0) {
	  getips(c);
	}
	else
	  printf("%s",buf);
      }
      close(c);
      return 0;
    }
  }
  
  close(c);
  close(s);
}


//called when a new client boots up... gets the list of files shared and stores them
int enterIP(char *ip, int sock) {
  exitIP(ip);
  FILE *fp, *fp2;
  char buf[MAX_LENGTH];
  int *i, *j;
  
  fp=fopen("lists/iplist","a");
  
  if(fp==NULL)
    return -1;
  
  if(findinfile(ip, "lists/iplist")==1)
    exitIP(ip);
  
  fprintf(fp, ip);
  fprintf(fp, "\n");
  fclose(fp);
  
  fp = fopen("lists/filelist", "a");
  if(fp==NULL)
    return -1;
  fp2 = fopen("lists/mainlist", "a");
  if(fp2==NULL)
    return -1;
  
  if(recv(sock, buf, sizeof(int), 0) == -1)
    return -1;
  i=(int)buf;
  
  for(;*i>0;*i--) {
    if(recv(sock, buf, sizeof(int), 0) == -1)
      return -1;
    j=(int)buf;
    if(recv(sock, buf, *j, 0) == -1)
      return -1;
    if(findinfile(buf, "lists/filelist")!=1) {
      fprintf(fp, buf);
      fprintf(fp, "\n");
    }
    fprintf(fp2, ip);
    fprintf(fp2, "\n");
    fprintf(fp2, buf);
    fprintf(fp2, "\n");
  }
  fclose(fp);
  fclose(fp2);
  
  return 0;
}

//is called when a client exits... would be called in case a client is found to be disconnected...
int exitIP(char *ip) {
  FILE *fp, *fp2;
  char iptmp[17], name[MAX_LENGTH];
  
  fp=fopen("lists/iplist","r");
  if(fp==NULL)
    return -1;
  fp2=fopen("lists/tempiplist","w");
  if(fp2==NULL)
    return -1;
  
  while (fscanf(fp, "%s\n", iptmp) != EOF) {
    if(strcmp(iptmp,ip)!=0) {
      fprintf(fp2, iptmp);
      fprintf(fp2, "\n");
    }
  }
  fclose(fp);
  fclose(fp2);
  unlink("lists/iplist");
  rename("lists/tempiplist","lists/iplist");
  
  
  fp=fopen("lists/mainlist","r");
  if(fp==NULL)
    return -1;
  fp2=fopen("lists/tempmainlist","w");
  if(fp2==NULL)
    return -1;
  
  while (fscanf(fp, "%s\n%s\n", iptmp, name) != EOF) {
    if(strcmp(iptmp,ip)!=0) {
      fprintf(fp2, iptmp);
      fprintf(fp2, "\n");
      fprintf(fp2, name);
      fprintf(fp2, "\n");
    }
  }
  fclose(fp);
  fclose(fp2);
  unlink("lists/mainlist");
  rename("lists/tempmainlist","lists/mainlist");
  
  return cleanfilelist();
}


//cleans the filelist synchronizing it with the mainlist
int cleanfilelist() {
  FILE *fp, *fp2;
  char tmp[MAX_LENGTH];
  
  fp=fopen("lists/filelist","r");
  if(fp==NULL)
    return -1;
  fp2=fopen("lists/tempfilelist","w");
  if(fp2==NULL)
    return -1;
  
  while (fscanf(fp, "%s\n", tmp) != EOF) {
    if(findinfile(tmp, "lists/mainlist")==1) {
      fprintf(fp2, tmp);
      fprintf(fp2, "\n");
    }
  }
  fclose(fp);
  fclose(fp2);
  unlink("lists/filelist");
  rename("lists/tempfilelist","lists/filelist");
  return 0;
}

//finds a line in a file
int findinfile(char *str, char *file) {
  FILE *fp;
  char tmp[MAX_LENGTH];
  
  fp=fopen(file,"r");
  if(fp==NULL)
    return -1;
  
  while (fscanf(fp, "%s\n", tmp) != EOF) {
    if(strcmp(tmp,str)==0) {
      fclose(fp);
      return 1;
    }
  }
  fclose(fp);
  return 0;
}

//sends back the list of files
int sendlist(int sock) {
  FILE *fp;
  char file[MAX_LENGTH];
  int counter=0;
  fp=fopen("lists/filelist","r");
  
  if(fp==NULL)
    return -1;
  while (fscanf(fp, "%s\n", file) != EOF) {
    counter++;
  }
  send(sock, &counter, sizeof(int), 0);
  fseek(fp, 0, SEEK_SET);
  
  while (fscanf(fp, "%s\n", file) != EOF) {
    //strcat(file, "\n");
    int i=strlen(file)+1;
    send(sock, &i, sizeof(int), 0);
    send(sock, file, strlen(file)+1, 0);
    printf("%s",file);
  }
  fclose(fp);
  return 0;
}

//read a file name and get all ip addresses who have that filelist
int getips(int sock) {
  char file[MAX_LENGTH], iptmp[17], name[MAX_LENGTH], buf2[sizeof(int)];
  FILE *fp;
  
  recv(sock, buf2, sizeof(int), 0);
  int *len;
  len=(int)buf2;
  
  if(recv(sock, file, *len, 0)==-1)
    return -1;
  if(findinfile(file, "lists/filelist")!=1)
    return 0;
  
  fp=fopen("lists/mainlist", "r");
  
  while (fscanf(fp, "%s\n%s\n", iptmp, name) != EOF) {
    if(strcmp(file, name)==0) {
      //strcat(iptmp, "\n");
      int i=strlen(iptmp)+1;
      send(sock, &i, sizeof(int), 0);
      send(sock, iptmp, strlen(iptmp)+1, 0);
      printf("%s",iptmp);
    }
  }
  
  strcpy(iptmp, "<end>");
  int i=strlen(iptmp)+1;
  send(sock, &i, sizeof(int), 0);
  send(sock, iptmp, strlen(iptmp)+1, 0);
  
  fclose(fp);
  return 0;
}
