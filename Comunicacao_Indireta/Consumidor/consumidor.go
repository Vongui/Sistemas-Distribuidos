package main

import (
	"context"
	"fmt"
	"log"

	"github.com/segmentio/kafka-go"
)

func main() {
	p := kafka.NewReader(kafka.ReaderConfig{
		Brokers:     []string{"localhost:9092"},
		Topic:       "pedidos-criados",
		GroupID:     "pedido-consumer-group",
		StartOffset: kafka.FirstOffset,
	})

	l := kafka.NewReader(kafka.ReaderConfig{
		Brokers:     []string{"localhost:9092"},
		Topic:       "server-logs",
		GroupID:     "log-consumer-group",
		StartOffset: kafka.FirstOffset,
	})

	go func() {
		defer p.Close()
		fmt.Println("Iniciando consumidor de [PEDIDOS]...")
		for {
			msg, err := p.ReadMessage(context.Background())
			if err != nil {
				log.Printf("Erro ao ler [PEDIDO]: %v", err)
				break
			}

			fmt.Printf("\n[ESTOQUE] Pedido recebido!\n")
			fmt.Printf("  Tópico:    %s\n", msg.Topic)
			fmt.Printf("  Mensagem:  %s\n", string(msg.Value))
			fmt.Println("  Ação: Dando baixa no produto 'Teclado Mecânico'...")
		}
	}() // Os parênteses () no final executam a função anônima

	go func() {
		defer l.Close()
		fmt.Println("Iniciando consumidor de [LOGS]...")
		for {
			msg, err := l.ReadMessage(context.Background())
			if err != nil {
				log.Printf("Erro ao ler [LOG]: %v", err)
				break
			}

			fmt.Printf("\n[AUDITORIA] Log recebido!\n")
			fmt.Printf("  Tópico:    %s\n", msg.Topic)
			fmt.Printf("  Mensagem:  %s\n", string(msg.Value))
			fmt.Println("  Ação: Registrando log no sistema...")
		}
	}()

	fmt.Println("Rodando, escutando os tópicos 'pedidos-criados' e 'server-logs'...")
	fmt.Println("Pressione Ctrl+C para sair.")
	
	select {} // Bloqueia a 'main' para sempre
}