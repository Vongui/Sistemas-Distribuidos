package cliente_servidor;

import com.sun.source.tree.ReturnTree;

import java.io.*;
import java.net.*;
import java.nio.charset.Charset;
import java.util.*;
import java.util.concurrent.atomic.AtomicInteger;

/**
 * @Author Vongui
 */
public class Servidor {

    private static final int MAX_CONNECTIONS = 5; // Máximo de conexões simultâneas
    private static AtomicInteger conexoesAtuais = new AtomicInteger(0);
    private static final List<Socket> conexoesAtivas = Collections.synchronizedList(new ArrayList<>());
    
    public static void main(String[] args) {

//        System.out.println("Informe a porta para o servidor:");
        int port = 8080;

        try (ServerSocket serverSocket = new ServerSocket(port)) {
            System.out.println("Servidor iniciado na porta " + port);

            while (true) {
                if (conexoesAtuais.get() >= MAX_CONNECTIONS) {
                    System.out.println("Limite de conexoes atingido. Aguardando liberação de espaço...");
                }
                else {
                    Socket clientSocket = serverSocket.accept();
                    conexoesAtivas.add(clientSocket);
                    conexoesAtuais.incrementAndGet();
                    System.out.println("Nova conexao recebida. Conexoes ativas: " + conexoesAtuais.get());
                    System.out.println("\nCliente: " + clientSocket.getInetAddress());
                    new ClientHandler(clientSocket, conexoesAtuais, conexoesAtivas).start();
                }
            }
        } catch (IOException e) {
            System.out.println("Erro ao iniciar o servidor: " + e.getMessage());
        }
    }

}
