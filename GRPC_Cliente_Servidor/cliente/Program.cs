using System;
using System.Threading.Tasks;
using Grpc.Net.Client;
using Validador; 

public class Program
{
    public static async Task Main(string[] args)
    {
        AppContext.SetSwitch("System.Net.Http.SocketsHttpHandler.Http2UnencryptedSupport", true);

        var serverAddress = "http://localhost:50051";
        using var channel = GrpcChannel.ForAddress(serverAddress);
        // Criando o cliente para o novo serviço "ValidadorDocumento"
        var client = new ValidadorDocumento.ValidadorDocumentoClient(channel);

        Console.WriteLine("---------------------------------------------");
        
        while (true)
        {
            Console.Write("Digite o tipo de documento (CPF ou CNPJ) ou 'sair': ");
            string? docType = Console.ReadLine()?.Trim();

            if (string.Equals(docType, "sair", StringComparison.OrdinalIgnoreCase))
            {
                break;
            }

            if (string.IsNullOrWhiteSpace(docType) || 
               (!docType.Equals("CPF", StringComparison.OrdinalIgnoreCase) && 
                !docType.Equals("CNPJ", StringComparison.OrdinalIgnoreCase)))
            {
                Console.WriteLine("Tipo inválido. Por favor, digite 'CPF' ou 'CNPJ'.");
                continue;
            }

            Console.Write($"Digite o número do {docType.ToUpper()}: ");
            string? docNumber = Console.ReadLine();

            if (string.IsNullOrWhiteSpace(docNumber))
            {
                Console.WriteLine("O número não pode ser vazio.");
                continue;
            }

            try
            {
                // Criando o objeto Requisicao com os novos campos
                // Note que o gerador de código C# converte snake_case (tipo, numero) para PascalCase (Tipo, Numero)
                var request = new Requisicao { Tipo = docType, Numero = docNumber };

                // Chamando o novo método RPC "ValidaAsync"
                var reply = await client.ValidaAsync(request);

                Console.ForegroundColor = reply.Valido ? ConsoleColor.Green : ConsoleColor.Red;
                Console.WriteLine($"--> Resposta do Servidor: {reply.Mensagem} (Válido: {reply.Valido})");
                Console.ResetColor();
            }
            catch (Exception ex)
            {
                Console.ForegroundColor = ConsoleColor.DarkRed;
                Console.WriteLine($"Erro ao comunicar com o servidor: {ex.Message}");
                Console.ResetColor();
            }
            Console.WriteLine();
        }
    }
}