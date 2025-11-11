package main

import (
	"bufio"
	"fmt"
	"os"
	"strings"

	"github.com/hooklift/gowsdl/soap"
)

func main() {
	soapClient := soap.NewClient("http://localhost:8000")
	client := NewApplication(soapClient)
	reader := bufio.NewReader(os.Stdin)

	for {
		fmt.Print("Deseja validar 'cpf' ou 'cnpj' (ou 'sair' para terminar)? ")
		kindRaw, _ := reader.ReadString('\n')
		kind := strings.ToLower(strings.TrimSpace(kindRaw))
		if kind == "sair" || kind == "exit" {
			fmt.Println("Encerrando.")
			return
		}
		if kind != "cpf" && kind != "cnpj" {
			fmt.Println("Opção inválida. Digite 'cpf' ou 'cnpj'.")
			continue
		}

		fmt.Printf("Digite o %s: ", kind)
		valRaw, _ := reader.ReadString('\n')
		val := strings.TrimSpace(valRaw)

		if kind == "cpf" {
			req := &Valida_cpf{Cpf: &val}
			resp, err := client.Valida_cpf(req)

			if err != nil {
				fmt.Println("Erro:", err)
			} else if resp.Valida_cpfResult == nil {
				fmt.Println("Erro: Resposta do servidor foi nula.")
			} else {
				if *resp.Valida_cpfResult {
					fmt.Println("CPF válido")
				} else {
					fmt.Println("CPF inválido")
				}
			}
		} else {
			req := &Valida_cnpj{Cnpj: &val}
			resp, err := client.Valida_cnpj(req)

			if err != nil {
				fmt.Println("Erro:", err)
			} else if resp.Valida_cnpjResult == nil {
				fmt.Println("Erro: Resposta do servidor foi nula.")
			} else {
				if *resp.Valida_cnpjResult {
					fmt.Println("CNPJ válido")
				} else {
					fmt.Println("CNPJ inválido")
				}
			}
		}

		fmt.Println()
	}
}
