# Guia rápido de comandos

Observação: a linha fornecida parece ter os dois comandos juntos. Aqui estão os comandos corretos e separados para Windows.

## 1) Rodar Kafka - Terminais Diferentes
```
.\bin\windows\zookeeper-server-start.bat .\config\zookeeper.properties

.\bin\windows\kafka-server-start.bat .\config\server.properties

.\bin\windows\kafka-topics.bat --create --topic pedidos-criados --bootstrap-server localhost:9092 --partitions 1 --replication-factor 1

.bin\windows\kafka-topics.bat --list --bootstrap-server localhost:9092
```

## 2) Instalar a bibliotecas
```
pip install confluent-kafka

go mod init consumidor
go get github.com/segmentio/kafka-go
```

## 3) Ir para a pasta do Consumidor
```powershell
cd "C:\Projects\Sistemas-Distribuidos\Comunicacao_Indireta\Consumidor"
```

## 4) Executar
```
go run consumidor.go
python produtor.py
```

### Obs: Configurar Kafka Windows
Edite C:\kafka\config\zookeeper.properties:

    Encontre: dataDir=/tmp/zookeeper

    Mude para: dataDir=C:/kafka/zookeeper-data

Edite C:\kafka\config\server.properties:

    Encontre: log.dirs=/tmp/kafka-logs

    Mude para: log.dirs=C:/kafka/kafka-logs