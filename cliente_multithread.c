#include<stdio.h>
#include<winsock2.h>
#include<string.h>
#pragma comment(lib,"ws2_32.lib")
#define MESSAGESIZE 255
#define NICKSIZE 20

SOCKET s;
char nick[MESSAGESIZE];

void senddata(void) {
  char msg[MESSAGESIZE], phrase[MESSAGESIZE];
  while(1) {
    msg[0]='\0';
    fgets(phrase,MESSAGESIZE-NICKSIZE,stdin);
    strncat(msg,nick,MESSAGESIZE);
    strncat(msg,": ",MESSAGESIZE);
    strncat(msg,phrase,MESSAGESIZE);
    if(send(s, msg, strlen(msg)+1, 0) < 0) {
      puts("\nEnvio falhou.");
    }
  }
}

int main(int argc , char *argv[]) {
  WSADATA wsa;

  struct sockaddr_in server;
  char message[MESSAGESIZE], server_reply[MESSAGESIZE];
  int recv_size;

  printf("*** CLIENT ***\n\n");

  if (WSAStartup(MAKEWORD(2,2),&wsa) != 0) {
    printf("\nFalha na inicializacao da biblioteca Winsock: %d",WSAGetLastError());
    exit(EXIT_FAILURE);
  }

  if((s = socket(AF_INET , SOCK_STREAM , 0 )) == INVALID_SOCKET) {
    printf("\nNao e possivel criar o socket: %d" , WSAGetLastError());
    exit(EXIT_FAILURE);
  }

  server.sin_addr.s_addr = inet_addr("127.0.0.1");
  server.sin_family = AF_INET;
  server.sin_port = htons( 8888 );

  if (connect(s , (struct sockaddr *)&server , sizeof(server)) < 0) {
    printf("\nErro de conex�o.");
    exit(EXIT_FAILURE);
  }

  printf("Nickname [MAX 20 characters]: ");
  fgets(nick,NICKSIZE+1,stdin);
  nick[strlen(nick)-1]='\0';

  _beginthread(senddata,NULL,NULL);

  while(1) {
    if((recv_size = recv(s ,server_reply, MESSAGESIZE, 0)) == SOCKET_ERROR) {
      puts("\nRecep��o falhou.");
    }
    if(recv_size > 0)
      puts(server_reply);
  }

  system("pause");
  return 0;
}
