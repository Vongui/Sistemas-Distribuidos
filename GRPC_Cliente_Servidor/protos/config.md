# Guia de Execução

## C#
Instalação das bibliotecas (executar um de cada vez)  
dotnet add package Grpc.Net.Client  
dotnet add package Google.Protobuf  
dotnet add package Grpc.Tools

## python

Geração dos esqueletos do Grpc Server -> python -m grpc_tools.protoc -I../protos --python_out=. --pyi_out=. --grpc_python_out=. ../protos/validador.proto

Instalação da biblioteca - > pip install grpcio grpcio-tools 

## Execução -> Rodar em terminais diferentes
 cd ./cliente -> dotnet run  
 cd ./server -> python server.py
