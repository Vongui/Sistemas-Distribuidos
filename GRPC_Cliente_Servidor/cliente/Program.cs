using System;
using System.Threading.Tasks;
using Grpc.Net.Client;
using Validador; 

public class Program
{
    public static async Task Main(string[] args)
    {
        AppContext.SetSwitch("System.Net.Http.SocketsHttpHandler.Http2UnencryptedSupport", true);

        //Configurando o servidor no cliente
        var serverAddress = "http://localhost:50051";
        using var canal = GrpcChannel.ForAddress(serverAddress);
        
        //Criando um cliente
        var cliente = new VerificarDocumento.VerificarDocumentoClient(canal);

        Console.WriteLine("---------------------------------------------");
        
        while (true)
        {
            Console.Write("Digite o tipo de documento (CPF ou CNPJ) ou 'sair': ");
            string? docTipo = Console.ReadLine()?.Trim();

            if (string.Equals(docTipo, "sair", StringComparison.OrdinalIgnoreCase))
            {
                break;
            }

            if (string.IsNullOrWhiteSpace(docTipo) || 
               (!docTipo.Equals("CPF", StringComparison.OrdinalIgnoreCase) && 
                !docTipo.Equals("CNPJ", StringComparison.OrdinalIgnoreCase)))
            {
                Console.WriteLine("Tipo inválido. Por favor, digite 'CPF' ou 'CNPJ'.");
            }

            Console.Write($"Digite o número do {docTipo?.ToUpper()}: ");
            string? docNumero = Console.ReadLine();

            if (string.IsNullOrWhiteSpace(docNumero))
            {
                Console.WriteLine("O número não pode ser vazio.");
                continue;
            }

            try
            {

                var requisicao = new Requisicao { Tipo = docTipo, Numero = docNumero };
                var resposta = await cliente.VerificaAsync(requisicao);

                //Frufuzinho de cor no terminal
                Console.ForegroundColor = resposta.Valido ? ConsoleColor.Green : ConsoleColor.Red;
                Console.WriteLine($"--> Resposta do Servidor: {resposta.Mensagem} (Válido: {resposta.Valido})");
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