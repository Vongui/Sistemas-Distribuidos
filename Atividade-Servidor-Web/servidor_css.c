#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <process.h>

#define PORT 8080
#define TAM_BUFFER 1024

char* tipo_arquivo(char *nome_arquivo)
{
    if (strstr(nome_arquivo, ".css"))
    {
        return "text/css";
    }
    else if (strstr(nome_arquivo, ".js"))
    {
        return "application/javascript";
    }
    else if (strstr(nome_arquivo, ".png"))
    {
        return "image/png";
    }
    else if (strstr(nome_arquivo, ".jpg") || strstr(nome_arquivo, ".jpeg"))
    {
        return "image/jpeg";
    }
    else if (strstr(nome_arquivo, ".gif"))
    {
        return "image/gif";
    }
    else if (strstr(nome_arquivo, ".txt"))
    {
        return "text/plain";
    }
    else if (strstr(nome_arquivo, ".pdf"))
    {
        return "application/pdf";
    }
    else if (strstr(nome_arquivo, ".zip"))
    {
        return "application/zip";
    }
    
    return "text/html";
}

//Monta Header
void enviar_arquivo(SOCKET cliente, FILE *f, char *status, char *tipo)
{
    fseek(f, 0, SEEK_END);
    long tamanho = ftell(f);
    fseek(f, 0, SEEK_SET);

    char header[200];
    sprintf(header, "HTTP/1.1 %s\r\nContent-Type: %s\r\nContent-Length: %ld\r\nConnection: close\r\n\r\n", status, tipo, tamanho);
    send(cliente, header, strlen(header), 0);

    printf("------------Resposta------------\n%s", header);

    char buf[TAM_BUFFER];
    int n;
    while ((n = fread(buf, 1, TAM_BUFFER, f)) > 0)
    {
        send(cliente, buf, n, 0);
    }
    
    printf("--------Fim de Resposta------------\n\n");
}

void resposta_cliente(void *cliente_socket)
{
    SOCKET sock = (SOCKET)cliente_socket;
    char buffer[TAM_BUFFER];
    int tam_recv = recv(sock, buffer, TAM_BUFFER - 1, 0);
    if (tam_recv <= 0)
    {
        closesocket(sock);
        return;
    }

    buffer[tam_recv] = '\0';
    printf("\nRequisicao recebida:\n%s\n", buffer);

    char metodo[10], caminho[100];
    sscanf(buffer, "%s %s", metodo, caminho);

    if (strcmp(metodo, "GET") != 0)
    {
        char nome_erro[200] = "./www/501.html";
        FILE *f = fopen(nome_erro, "rb");
        if (f != NULL)
        {
            enviar_arquivo(sock, f, "501 Not Implemented", "text/html");
            fclose(f);
        }
    }
    else
    {
        char nome_arquivo[200] = "./www";
        strcat(nome_arquivo, caminho);

        if (strcmp(caminho, "/") == 0)
        {
            strcpy(nome_arquivo, "./www/index.html");
        }

        FILE *f = fopen(nome_arquivo, "rb");
        if (f == NULL)
        {
            char nome_erro[200] = "./www/404.html";
            FILE *f404 = fopen(nome_erro, "rb");
            if (f404 != NULL)
            {
                enviar_arquivo(sock, f404, "404 Not Found", "text/html");
                fclose(f404);
            }
        }
        else
        {
            enviar_arquivo(sock, f, "200 OK", tipo_arquivo(nome_arquivo));
            fclose(f);
        }
    }
    closesocket(sock);
    _endthread();
}

int main()
{
    WSADATA wsa;
    SOCKET server, cliente;
    struct sockaddr_in server_addr, cliente_addr;
    int c;

    WSAStartup(MAKEWORD(2, 2), &wsa);

    //Configurcao do socket
    server = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    bind(server, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(server, 10);

    printf("Servidor rodando na porta %d...\n", PORT);

    //aceita conexoes e divide em threads
    c = sizeof(struct sockaddr_in);
    while ((cliente = accept(server, (struct sockaddr *)&cliente_addr, &c)) != INVALID_SOCKET)
    {
        printf("Nova conexao aceita\n");
        printf("Cliente conectado: %s:%d\n", inet_ntoa(cliente_addr.sin_addr), ntohs(cliente_addr.sin_port));
        _beginthread(resposta_cliente, 0, (void *)cliente);
    }

    closesocket(server);
    WSACleanup();
}
