# Validador de Documentos SOAP (Python + Go)

Este projeto é um exemplo de um Web Service SOAP para validar CPFs e CNPJs, demonstrando a interoperabilidade entre um servidor Python (usando `spyne`) e um cliente Go (usando `gowsdl`).

O servidor Python expõe as operações `valida_cpf` e `valida_cnpj` via SOAP/WSDL, e o cliente Go consome o WSDL para gerar código de cliente automaticamente.

---

## 🐍 Servidor (Python)

O servidor é responsável por expor as operações de validação de documentos brasileiros (CPF e CNPJ).

### 1. Setup

É recomendado usar um ambiente virtual (`venv`).

```bash
# Crie um ambiente virtual
python -m venv venv

# Ative o ambiente
# Windows:
.\venv\Scripts\activate
# macOS/Linux:
source venv/bin/activate

# Instale as dependências
pip install -r requirements.txt
```

Ou manualmente:
```bash
pip install spyne lxml
```

### 2. Rodando o Servidor

Com o ambiente ativado, execute:

```bash
# No diretório servidor/
python validator_service.py
```

O servidor estará disponível em:
- **Serviço SOAP:** `http://localhost:8000`
- **WSDL:** `http://localhost:8000/?wsdl`

Você deve ver a mensagem:
```
Servidor SOAP rodando em http://localhost:8000
WSDL disponível em: http://localhost:8000/?wsdl
```

---

## ☕ Cliente (Go)

O cliente Go é uma aplicação de console interativa que consome o serviço SOAP.

### 1. Pré-requisitos

Instale o Go (versão 1.16+) de https://golang.org

### 2. Setup (Apenas uma vez)

Instale a ferramenta `gowsdl` que gera o código do cliente a partir do WSDL:

```bash
go install github.com/hooklift/gowsdl/cmd/gowsdl@latest
```

### 3. Gerando o Código do Cliente (Apenas uma vez)

**Importante:** O servidor Python deve estar rodando neste momento.

No diretório `cliente/main/`, execute:

```bash
gowsdl -p main -o client_generated.go http://localhost:8000/?wsdl
```

Este comando irá gerar o arquivo `client_generated.go` com as estruturas e métodos SOAP.

### 4. Rodando o Cliente

No diretório `cliente/main/`, execute:

```bash
# Baixa as dependências (primeira vez)
go mod tidy

# Compila e roda o projeto
go run .
```

O programa iniciará e pedirá interativamente:

```
Deseja validar 'cpf' ou 'cnpj' (ou 'sair' para terminar)? cpf
Digite o cpf: 111.444.777-35
CPF válido

Deseja validar 'cpf' ou 'cnpj' (ou 'sair' para terminar)? sair
Encerrando.
```

---

## 📁 Estrutura do Projeto

```
Servidor_XMP_SOAP/
├── servidor/
│   ├── validator_service.py      # Servidor SOAP
│   ├── valida_schema.xsd          # Schema de validação XML
│   ├── requirements.txt           # Dependências Python
│   └── .idea/                     # IDE config (IDE)
│
├── cliente/
│   ├── main/
│   │   ├── main.go                # Código principal do cliente
│   │   ├── client_generated.go    # Gerado pelo gowsdl (não editar)
│   │   ├── go.mod                 # Dependências Go
│   │   └── go.sum                 # Hash das dependências
│   │
│   └── cliente.go                 # Cliente alternativo (manual)
│
└── README.md                       # Este arquivo
```

---

## 🧪 Testes Rápidos

### CPF Válido
```
111.444.777-35  → Válido
11144477735     → Válido
```

### CPF Inválido
```
123.456.789-01  → Inválido
12345678901     → Inválido
```

### CNPJ Válido
Gere um CNPJ válido ou use: `11.222.333/0001-81`

### CNPJ Inválido
```
12.345.678/0001-99  → Inválido
```

---

## 🔧 Troubleshooting

### Erro: "SOAP Fault"
**Causa:** Servidor não está rodando ou nomes de operações não batem.
**Solução:** 
1. Certifique-se de que o servidor Python está em execução
2. Regenere o client_generated.go: `gowsdl -p main -o client_generated.go http://localhost:8000/?wsdl`

### Erro: "Connection refused"
**Causa:** Servidor não está disponível em `localhost:8000`.
**Solução:** Verifique se o servidor Python está rodando e escutando na porta correta.

### Erro: "gowsdl: command not found"
**Causa:** `gowsdl` não foi instalado ou não está no PATH.
**Solução:** Execute `go install github.com/hooklift/gowsdl/cmd/gowsdl@latest` novamente.

---

## 📝 Tecnologias Utilizadas

- **Servidor:** Python 3.x com [Spyne](https://spyne.io/) (SOAP framework)
- **Cliente:** Go 1.16+ com [gowsdl](https://github.com/hooklift/gowsdl)
- **Protocolo:** SOAP 1.1
- **Schema:** XSD (XML Schema Definition)

---

