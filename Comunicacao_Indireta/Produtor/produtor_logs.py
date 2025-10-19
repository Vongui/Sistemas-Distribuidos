# producer_logs.py
import json
import time
from confluent_kafka import Producer
from datetime import datetime

# Configuração do Producer
config = {
    'bootstrap.servers': 'localhost:9092'
}

producer = Producer(config)
print("Produtor de [LOGS] iniciado.")

log_id = 1

def delivery_report(err, msg):
    if err is not None:
        print(f'[LOGS] Falha ao entregar: {err}')
    else:
        print(f'[LOGS] Mensagem entregue ao tópico {msg.topic()} [{msg.partition()}]')


try:
    while True:
        novo_log = {
            'id_log': f'log_{log_id}',
            'timestamp': datetime.now().isoformat(),
            'servidor': 'webserver-01',
            'mensagem': 'Acesso a pagina /login'
        }
        log_json = json.dumps(novo_log).encode('utf-8')

        producer.produce('server-logs', value=log_json, callback=delivery_report)
        print(f"[LOGS] Enviando log: {novo_log['id_log']}")

        log_id += 1
        producer.poll(2)
        time.sleep(20)

except KeyboardInterrupt:
    print("\nEncerrando o produtor de [LOGS]...")
finally:
    producer.flush()
    print("Produtor de [LOGS] encerrado.")