package main

import (
	"bufio"
	"bytes"
	"encoding/xml"
	"fmt"
	"io"
	"net/http"
	"os"
	"strings"
)

// Envelope/Body com namespace SOAP
type SOAPEnvelope struct {
	XMLName xml.Name `xml:"http://schemas.xmlsoap.org/soap/envelope/ Envelope"`
	Body    SOAPBody `xml:"http://schemas.xmlsoap.org/soap/envelope/ Body"`
}

type SOAPBody struct {
	XMLName xml.Name    `xml:"http://schemas.xmlsoap.org/soap/envelope/ Body"`
	Content interface{} `xml:",any"`
}

const svcNS = "http://validator.example.com/"

type ValidateCPFRequest struct {
	XMLName xml.Name `xml:"http://validator.example.com/ valida_cpf"`
	CPF     string   `xml:"http://validator.example.com/ cpf"`
}

type ValidateCPFResponse struct {
	XMLName xml.Name `xml:"http://validator.example.com/ valida_cpfResponse"`
	Result  bool     `xml:"http://validator.example.com/ valida_cpfResult"`
}

type ValidateCNPJRequest struct {
	XMLName xml.Name `xml:"http://validator.example.com/ valida_cnpj"`
	CNPJ    string   `xml:"http://validator.example.com/ cnpj"`
}

type ValidateCNPJResponse struct {
	XMLName xml.Name `xml:"http://validator.example.com/ valida_cnpjResponse"`
	Result  bool     `xml:"http://validator.example.com/ valida_cnpjResult"`
}

type SOAPClient struct {
	URL string
}

func NewSOAPClient(url string) *SOAPClient { return &SOAPClient{URL: url} }

func (c *SOAPClient) Call(request interface{}, response interface{}) error {
	envelope := SOAPEnvelope{Body: SOAPBody{Content: request}}

	xmlBytes, err := xml.MarshalIndent(envelope, "", "  ")
	if err != nil {
		return err
	}
	xmlRequest := []byte(xml.Header + string(xmlBytes))
	// fmt.Println("REQUEST:\n", string(xmlRequest))

	httpResp, err := http.Post(c.URL, "text/xml; charset=utf-8", bytes.NewBuffer(xmlRequest))
	if err != nil {
		return err
	}
	defer httpResp.Body.Close()

	respBody, err := io.ReadAll(httpResp.Body)
	if err != nil {
		return err
	}
	// fmt.Println("RESPONSE:\n", string(respBody))

	// preparar envelope de resposta para o unmarshal
	var respEnvelope SOAPEnvelope
	respEnvelope.Body.Content = response
	if err := xml.Unmarshal(respBody, &respEnvelope); err != nil {
		return err
	}
	return nil
}

func (c *SOAPClient) ValidateCPF(cpf string) (bool, error) {
	req := &ValidateCPFRequest{CPF: cpf}
	resp := &ValidateCPFResponse{}
	if err := c.Call(req, resp); err != nil {
		return false, err
	}
	return resp.Result, nil
}

func (c *SOAPClient) ValidateCNPJ(cnpj string) (bool, error) {
	req := &ValidateCNPJRequest{CNPJ: cnpj}
	resp := &ValidateCNPJResponse{}
	if err := c.Call(req, resp); err != nil {
		return false, err
	}
	return resp.Result, nil
}

func main() {
	client := NewSOAPClient("http://localhost:8000")
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
			valid, err := client.ValidateCPF(val)
			if err != nil {
				fmt.Println("Erro:", err)
			} else {
				if valid {
					fmt.Println("CPF válido")
				} else {
					fmt.Println("CPF inválido")
				}
			}
		} else { // cnpj
			valid, err := client.ValidateCNPJ(val)
			if err != nil {
				fmt.Println("Erro:", err)
			} else {
				if valid {
					fmt.Println("CNPJ válido")
				} else {
					fmt.Println("CNPJ inválido")
				}
			}
		}

		fmt.Println()
	}
}
