package cliente_servidor;

import java.io.*;
import java.net.*;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.List;
import java.util.regex.*;

public class ClientHandler extends Thread {
    private Socket clientSocket;
    private static AtomicInteger conexaoAtual;
    private static List<Socket> conexaoAtiva;

    // Construtor que recebe o socket do cliente
    public ClientHandler(Socket socket, AtomicInteger conexaoAtual, List<Socket> conexaoAtiva) {
        this.clientSocket = socket;
        ClientHandler.conexaoAtual = conexaoAtual;
        ClientHandler.conexaoAtiva = conexaoAtiva;
    }

    @Override
    public void run() {
        try (BufferedReader reader = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
             PrintWriter writer = new PrintWriter(clientSocket.getOutputStream(), true)) {

            String requisicao = reader.readLine();
            System.out.println("========================Requisição do cliente======================\n"
                    + requisicao + "\n========================Fim da Requisição==========================\n");

            if (requisicao != null) {
                // Expressão regular para capturar os parâmetros da requisição
                String regex = "GM/1\\.1 TIPO=(\\S+) NUMERO=(\\S+) DATA=(\\S+)";
                Pattern pattern = Pattern.compile(regex);
                Matcher matcher = pattern.matcher(requisicao);

                if (matcher.matches()) {
                    String tipo = matcher.group(1); // Captura o tipo (CPF ou CNPJ)
                    String numero = matcher.group(2); // Captura o número (CPF ou CNPJ)
                    String data = matcher.group(3); // Captura a data

                    boolean isValid = false;
                    if ("cpf".equalsIgnoreCase(tipo)) {
                        isValid = validarCPF(numero);
                    } else if ("cnpj".equalsIgnoreCase(tipo)) {
                        isValid = validarCNPJ(numero);
                    }

                    // Resposta: Data e hora junto com o status de validade
                    String resposta = "Data e Hora: " + new SimpleDateFormat("dd/MM/yyyy HH:mm:ss").format(new Date()) +
                            " | Validação: " + (isValid ? "Válido" : "Inválido");
                    System.out.println("==========================Resposta ao Cliente======================");
                    System.out.println(resposta);
                    System.out.println("==========================Fim da Resposta==========================");
                    writer.println(resposta);
                } else {
                    String error = "Erro: Formato inválido. A requisição deve ser no formato 'GM/1.1 TIPO=<tipo> NUMERO=<numero> DATA=<data>'";
                    writer.println(error);
                    System.out.println(error);
                }

            } else {
                System.out.println("Requisição nula recebida");
            }

        } catch (IOException e) {
            System.out.println("Erro na comunicação com o cliente: " + e.getMessage());
        } finally {
            try {
                clientSocket.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
            conexaoAtual.decrementAndGet();
            conexaoAtiva.remove(clientSocket);
            System.out.println("Conexão encerrada. Conexões ativas: " + conexaoAtual.get());
        }
    }

    private boolean validarCPF(String cpf) {
        if (cpf.length() != 11) return false;
        int soma = 0, digito;
        for (int i = 0; i < 9; i++) soma += (10 - i) * (cpf.charAt(i) - '0');
        digito = 11 - (soma % 11);
        if (digito == 10 || digito == 11) digito = 0;
        if (digito != (cpf.charAt(9) - '0')) return false;
        soma = 0;
        for (int i = 0; i < 10; i++) soma += (11 - i) * (cpf.charAt(i) - '0');
        digito = 11 - (soma % 11);
        if (digito == 10 || digito == 11) digito = 0;
        return digito == (cpf.charAt(10) - '0');
    }

    private boolean validarCNPJ(String cnpj) {
        if (cnpj.length() != 14) return false;
        int soma = 0;
        int[] peso1 = {5, 4, 3, 2, 9, 8, 7, 6, 5, 4, 3, 2};
        for (int i = 0; i < 12; i++) soma += (cnpj.charAt(i) - '0') * peso1[i];
        int digito1 = 11 - (soma % 11);
        if (digito1 == 10 || digito1 == 11) digito1 = 0;
        if (digito1 != (cnpj.charAt(12) - '0')) return false;
        soma = 0;
        int[] peso2 = {6, 5, 4, 3, 2, 9, 8, 7, 6, 5, 4, 3, 2};
        for (int i = 0; i < 13; i++) soma += (cnpj.charAt(i) - '0') * peso2[i];
        int digito2 = 11 - (soma % 11);
        if (digito2 == 10 || digito2 == 11) digito2 = 0;
        return digito2 == (cnpj.charAt(13) - '0');
    }
}
