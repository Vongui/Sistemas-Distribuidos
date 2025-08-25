#include <io.h>
#include <stdio.h>
#include <winsock2.h>
#include <string.h>
#pragma comment(lib,"ws2_32.lib")
#define PORT 8888
#define MESSAGESIZE 255
#define MAXSOCKETS 10

SOCKET new_socket[MAXSOCKETS];
int qtdsockets = 0;

void getdata(int pos) {
  int len, i;
  char message[MESSAGESIZE];
  while(1) {
    message[0]='\0';
    len=recv(new_socket[pos],message,MESSAGESIZE,0);
    if(len>0) {
      message[strlen(message)-1]='\0';
      printf("\n%s - (%d caracteres)",message,len);
      for(i=0;i<qtdsockets;i++)
        send(new_socket[i] , message , strlen(message)+1, 0);
    }
  }
}

int main(int argc , char *argv[]) {
  WSADATA wsa;
  SOCKET s;
  struct sockaddr_in server , client;
  int c, pos;


  printf("*** SERVER ***\n\nAguardando conexoes...\n\n");

  if (WSAStartup(MAKEWORD(2,2),&wsa) != 0) {
    printf("\nFalha na inicializacao da biblioteca Winsock: %d",WSAGetLastError());
    exit(EXIT_FAILURE);
  }

  if((s = socket(AF_INET , SOCK_STREAM , 0 )) == INVALID_SOCKET) {
    printf("\nNao e possivel inicializar o socket: %d" , WSAGetLastError());
    exit(EXIT_FAILURE);
  }

  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons( PORT );

  if( bind(s ,(struct sockaddr *)&server , sizeof(server)) == SOCKET_ERROR) {
    printf("\nNao e possivel construir o socket: %d" , WSAGetLastError());
    exit(EXIT_FAILURE);
  }

  listen(s,3);
  c = sizeof(struct sockaddr_in);

  while(1) {
    pos=qtdsockets;
    new_socket[qtdsockets++] = accept(s, (struct sockaddr *)&client, &c);
    if (new_socket[pos] == INVALID_SOCKET) {
      printf("\nConexao nao aceita. Codigo de erro: %d" , WSAGetLastError());
    }
    puts("\nConexao aceita.");
    printf("\nDados do cliente - IP: %s -  Porta: %d\n",inet_ntoa(client.sin_addr),htons(client.sin_port));
    _beginthread(getdata,NULL,pos);
  }
  closesocket(s);
  WSACleanup();
}
