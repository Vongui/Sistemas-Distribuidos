#include <stdio.h>
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <string.h>
#include <process.h>
#include <windows.h>

#define BUFLEN 512

int portaservidor;

void escuta(void *arg) {
    char buf[BUFLEN];
    int recebido;
    struct sockaddr_in servidor;
    SOCKET s;

    // cria socket
    s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s == INVALID_SOCKET) {
        printf("Erro ao criar socket de escuta. Código: %d\n", WSAGetLastError());
        return;
    }

    // configura endereço local
    servidor.sin_family = AF_INET;
    servidor.sin_addr.s_addr = INADDR_ANY;
    servidor.sin_port = htons(portaservidor);

    // faz bind
    if (bind(s, (struct sockaddr *)&servidor, sizeof(servidor)) == SOCKET_ERROR) {
        printf("Erro no bind. Código: %d\n", WSAGetLastError());
        closesocket(s);
        return;
    }

    printf("\nEscutando na porta %d...\n", portaservidor);

    memset(buf, 0, BUFLEN);
    while (1) {
        recebido = recvfrom(s, buf, BUFLEN, 0, NULL, NULL);
        if (recebido == SOCKET_ERROR) {
            printf("Erro ao receber mensagem. Código: %d\n", WSAGetLastError());
            break;
        }
        if (recebido > 0) {
            buf[recebido] = '\0'; // garante fim da string
            printf("\nAmigo: %s\n", buf);
        }
        memset(buf, 0, BUFLEN);
    }

    closesocket(s);
}

int main() {
    int portacliente;
    struct sockaddr_in cliente;
    char msg[BUFLEN], ip[17];
    WSADATA wsa;

    // inicializa Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Falha no WSAStartup. Código: %d\n", WSAGetLastError());
        return 1;
    }

    SOCKET c = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (c == INVALID_SOCKET) {
        printf("Erro ao criar socket de envio. Código: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // configuração inicial
    printf("Escutar na porta: ");
    scanf("%d", &portaservidor);

    getchar(); // consome o ENTER deixado pelo scanf
    printf("IP do parceiro: ");
    fgets(ip, sizeof(ip), stdin);
    ip[strcspn(ip, "\n")] = 0; // remove '\n'

    printf("Porta do parceiro: ");
    scanf("%d", &portacliente);

    // inicia thread de escuta
    _beginthread(escuta, 0, NULL);

    // configura destino
    cliente.sin_family = AF_INET;
    cliente.sin_port = htons(portacliente);
    cliente.sin_addr.S_un.S_addr = inet_addr(ip);

    // loop de envio
    while (1) {
        getchar(); // limpa buffer
        printf("\nMensagem: ");
        fgets(msg, BUFLEN, stdin);
        msg[strcspn(msg, "\n")] = 0; // remove ENTER

        if (strlen(msg) == 0) continue; // ignora vazio

        if (sendto(c, msg, strlen(msg), 0, (struct sockaddr*)&cliente, sizeof(cliente)) == SOCKET_ERROR) {
            printf("Erro ao enviar mensagem. Código: %d\n", WSAGetLastError());
        } else {
            printf("Voce: %s\n", msg);
        }
    }

    closesocket(c);
    WSACleanup();
    return 0;
}
