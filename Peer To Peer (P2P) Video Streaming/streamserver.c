#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <vlc/vlc.h>

#define LEN 5

int port=1235;

static void quit_on_exception (libvlc_exception_t *excp) {
   if (libvlc_exception_raised (excp)) {
      fprintf(stderr, "error: %s\n", libvlc_exception_get_message(excp));
      exit(-1);
   }
}

int main(int argc, char *argv[]) {
  struct sockaddr_in cli_addr, serv_addr;
  int s, c, c_len;
  char buf[LEN], buf2[sizeof(int)], temp[LEN];
    
  if ((s=socket(AF_INET,SOCK_STREAM,0))== -1){
    perror("socket");
    exit(1);
  }
  
  bzero (&serv_addr, sizeof (serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin_port = htons (port);
  
  if (bind(s, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1) {
    perror("bind");
    exit(1);
  }

  if(listen(s, 5)== -1) {
    perror("listen");
    //exit(1);
  }
  
  while(1) {
    if ((c=accept(s,(struct sockaddr *)&cli_addr, &c_len)) == -1) {
      perror("accept");
      //exit(1);
    }
    
    //printf("Connected to %s\n", (char *)inet_ntoa(cli_addr.sin_addr));
    
    int *len;
    recv(c, buf2, sizeof(int), 0);
    len=(int)buf2;
    recv(c, buf, *len, 0);
  
    if(strcmp("stream", buf)==0) {
      recv(c, buf2, sizeof(int), 0);
      len=(int)buf2;
      recv(c, buf, *len, 0);
      
      int l = strlen((char *)inet_ntoa(cli_addr.sin_addr))+1;
      send(c, &l, sizeof(int), 0);
      send(c, inet_ntoa(cli_addr.sin_addr), l, 0);
      
      strcpy(temp, "share/");
      strcat(temp, buf);
      
      libvlc_exception_t excp;
      libvlc_instance_t *inst;
      int item, argc2=3;
      char *filename = temp, *argv2[3];
      argv2[0]=malloc(50);
      argv2[1]=malloc(50);
      argv2[2]=malloc(50);
            
      strcpy(argv2[0], "vlc");
      strcpy(argv2[1], "--sout");
      strcpy(argv2[2], "#std{access=udp,mux=ts,dst=");
      strcat(argv2[2], (char *)inet_ntoa(cli_addr.sin_addr));
      strcat(argv2[2], ":1234}");
            
      libvlc_exception_init (&excp);
      inst = libvlc_new (argc2, argv2, &excp);
      quit_on_exception (&excp);
      item = libvlc_playlist_add (inst, filename, NULL, &excp); 
      quit_on_exception (&excp);
      libvlc_playlist_play (inst, item, 0, NULL, &excp); 
      quit_on_exception (&excp);
      free(argv2[0]);
      free(argv2[1]);
      free(argv2[2]);
    }
    else if(strcmp("stop", buf)==0)
      break;
  }
  close(c);
  close(s);
}
