# producer_pedidos.py
import json
import time
from confluent_kafka import Producer

# Configuração do Producer
config = {
    'bootstrap.servers': 'localhost:9092'
}

producer = Producer(config)
print("Produtor de [PEDIDOS] iniciado.")

pedido_id = 1


def delivery_report(err, msg):
    if err is not None:
        print(f'[PEDIDOS] Falha ao entregar: {err}')
    else:
        print(f'[PEDIDOS] Mensagem entregue ao tópico {msg.topic()} [{msg.partition()}]')


try:
    while True:
        novo_pedido = {
            'id_pedido': f'pedido_{pedido_id}',
            'id_cliente': 'cliente_775'
        }
        pedido_json = json.dumps(novo_pedido).encode('utf-8')

        producer.produce('pedidos-criados', value=pedido_json, callback=delivery_report)
        print(f"[PEDIDOS] Enviando pedido: {novo_pedido['id_pedido']}")

        pedido_id += 1
        producer.poll(5)
        time.sleep(20)

except KeyboardInterrupt:
    print("\nEncerrando o produtor de [PEDIDOS]...")
finally:
    producer.flush()
    print("Produtor de [PEDIDOS] encerrado.")