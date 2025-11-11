from spyne import Application, rpc, ServiceBase, Unicode, Boolean
from spyne.protocol.soap import Soap11
from spyne.server.wsgi import WsgiApplication
from wsgiref.simple_server import make_server
from lxml import etree
from xml.sax.saxutils import escape
import os

XSD_PATH = os.path.join(os.path.dirname(__file__), 'valida_schema.xsd')

# carrega schema uma vez
try:
    _SCHEMA = etree.XMLSchema(etree.parse(XSD_PATH))
except Exception as e:
    print("Erro ao carregar XSD:", e)
    _SCHEMA = None


def validar_doc(root_name: str, inner_xml: str) -> (bool, str):
    if _SCHEMA is None:
        return True, ""
    xml = f"<{root_name} xmlns='http://validator.example.com/'>{inner_xml}</{root_name}>"
    try:
        doc = etree.fromstring(xml.encode('utf-8'))
        ok = _SCHEMA.validate(doc)
        if ok:
            return True, ""
        return False, str(_SCHEMA.error_log)
    except Exception as e:
        return False, str(e)


def is_cpf_valid(cpf: str) -> bool:
    cpf = ''.join(filter(str.isdigit, cpf))
    if len(cpf) != 11 or len(set(cpf)) == 1:
        return False
    soma = sum(int(cpf[i]) * (10 - i) for i in range(9))
    resto = (soma * 10) % 11
    if resto == 10:
        resto = 0
    if resto != int(cpf[9]):
        return False
    soma = sum(int(cpf[i]) * (11 - i) for i in range(10))
    resto = (soma * 10) % 11
    if resto == 10:
        resto = 0
    if resto != int(cpf[10]):
        return False
    return True


def is_cnpj_valid(cnpj: str) -> bool:
    cnpj = ''.join(filter(str.isdigit, cnpj))
    if len(cnpj) != 14 or len(set(cnpj)) == 1:
        return False
    soma = sum(int(d) * w for d, w in zip(cnpj[:12], [5, 4, 3, 2, 9, 8, 7, 6, 5, 4, 3, 2]))
    resto = soma % 11
    digito1 = 0 if resto < 2 else 11 - resto
    if digito1 != int(cnpj[12]):
        return False
    soma = sum(int(d) * w for d, w in zip(cnpj[:13], [6, 5, 4, 3, 2, 9, 8, 7, 6, 5, 4, 3, 2]))
    resto = soma % 11
    digito2 = 0 if resto < 2 else 11 - resto
    if digito2 != int(cnpj[13]):
        return False
    return True


class DocumentValidator(ServiceBase):
    @rpc(Unicode, _returns=Boolean)
    def valida_cpf(ctx, cpf):
        # valida XSD do fragmento
        inner = f"<cpf>{escape(cpf)}</cpf>"
        ok, err = validar_doc('valida_cpf', inner)
        if not ok:
            print("XSD invalido - valida_cpf:", err)
            return False
        result = is_cpf_valid(cpf)
        print("valida_cpf:", cpf, "->", result)
        return result

    @rpc(Unicode, _returns=Boolean)
    def valida_cnpj(ctx, cnpj):
        inner = f"<cnpj>{escape(cnpj)}</cnpj>"
        ok, err = validar_doc('valida_cnpj', inner)
        if not ok:
            print("XSD invalido valida_cnpj:", err)
            return False
        result = is_cnpj_valid(cnpj)
        print("valida_cnpj:", cnpj, "->", result)
        return result


def create_app_with_validation():
    return Application(
        [DocumentValidator],
        tns='http://validator.example.com/',
        in_protocol=Soap11(),
        out_protocol=Soap11()
    )


if __name__ == '__main__':
    wsgi_app = WsgiApplication(create_app_with_validation())
    server = make_server('0.0.0.0', 8000, wsgi_app)
    print("Servidor SOAP rodando em http://localhost:8000")
    print("WSDL disponível em: http://localhost:8000/?wsdl")
    server.serve_forever()
