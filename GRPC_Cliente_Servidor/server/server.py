import grpc
import time
from concurrent import futures
import validador_pb2
import validador_pb2_grpc

def is_cpf_valid(cpf: str) -> bool:
    cpf = ''.join(filter(str.isdigit, cpf))
    if len(cpf) != 11 or len(set(cpf)) == 1: return False
    soma = sum(int(cpf[i]) * (10 - i) for i in range(9)); resto = (soma * 10) % 11
    if resto == 10: resto = 0
    if resto != int(cpf[9]): return False
    soma = sum(int(cpf[i]) * (11 - i) for i in range(10)); resto = (soma * 10) % 11
    if resto == 10: resto = 0
    if resto != int(cpf[10]): return False
    return True

def is_cnpj_valid(cnpj: str) -> bool:
    cnpj = ''.join(filter(str.isdigit, cnpj))
    if len(cnpj) != 14 or len(set(cnpj)) == 1: return False
    soma = sum(int(d) * w for d, w in zip(cnpj[:12], [5, 4, 3, 2, 9, 8, 7, 6, 5, 4, 3, 2])); resto = soma % 11
    digito1 = 0 if resto < 2 else 11 - resto
    if digito1 != int(cnpj[12]): return False
    soma = sum(int(d) * w for d, w in zip(cnpj[:13], [6, 5, 4, 3, 2, 9, 8, 7, 6, 5, 4, 3, 2])); resto = soma % 11
    digito2 = 0 if resto < 2 else 11 - resto
    if digito2 != int(cnpj[13]): return False
    return True


class ValidadorDocumentoServicer(validador_pb2_grpc.ValidadorDocumentoServicer):

    def Valida(self, request, context):
        doc_tipo = request.tipo.upper()
        doc_numero = request.numero
        
        print(f"Recebida requisição para validar {doc_tipo}: {doc_numero}")
        is_valid = False

        if doc_tipo == "CPF":
            is_valid = is_cpf_valid(doc_numero)
        elif doc_tipo == "CNPJ":
            is_valid = is_cnpj_valid(doc_numero)
        else:
            mensagem = f"Tipo de documento '{request.tipo}' desconhecido. Use 'CPF' ou 'CNPJ'."
            print(f"Resultado: Inválido (tipo desconhecido)")
            return validador_pb2.Resposta(valido=False, mensagem=mensagem)

        if is_valid:
            mensagem = f"O {doc_tipo} é válido."
            print(f"Resultado: Válido ({doc_tipo})")
        else:
            mensagem = f"O {doc_tipo} é inválido."
            print(f"Resultado: Inválido ({doc_tipo})")
            
        return validador_pb2.Resposta(valido=is_valid, mensagem=mensagem)

def main():
    server = grpc.server(futures.ThreadPoolExecutor(max_workers=10))
    validador_pb2_grpc.add_ValidadorDocumentoServicer_to_server(
        ValidadorDocumentoServicer(), server)
    
    port = '50051'
    server.add_insecure_port('[::]:' + port)
    server.start()
    print(f"Servidor iniciado na porta {port}.")
    server.wait_for_termination()

if __name__ == '__main__':
    main()