import re
import socket
from datetime import datetime


def retorna_tipo(numero):
    if len(numero) == 14:
        return "CNPJ"
    elif len(numero) == 11:
        return "CPF"
    else:
        return -1

def remove_pontuacao(numero):
    return re.sub(r'\D', '', numero)

def main():
    host = 'localhost'  # Endereço do servidor
    port = 8080  # Porta do servidor

    while True:
        entrada = input("Digite o número (cpf ou cnpj): ").strip()
        numero = remove_pontuacao(entrada)
        tipo = retorna_tipo(numero)
        data_atual = datetime.now().strftime("%Y-%m-%d")

        mensagem = f"GM/1.1 TIPO={tipo} NUMERO={numero} DATA={data_atual}\n"

        try:
            with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
                s.connect((host, port))
                s.sendall(mensagem.encode())

                resposta = s.recv(1024).decode()
                print(f"==========================Resposta do servidor=====================\n\n"
                      f"{resposta}"
                      f"\n==========================Fim da Resposta==========================\n")

        except Exception as e:
            print(f"Erro ao se conectar com o servidor: {e}")


if __name__ == "__main__":
    main()
