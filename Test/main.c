#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mongoose.h"

static const char *s_listen_on = "http://0.0.0.0:8000";

// Função para processar requisições
static void fn(struct mg_connection *c, int ev, void *ev_data) {
    if (ev == MG_EV_HTTP_MSG) {
        struct mg_http_message *hm = (struct mg_http_message *) ev_data;

        // Verifica se é uma requisição OPTIONS (pré-voo CORS)
        if (mg_strcmp(hm->method, mg_str("OPTIONS")) == 0) {  // Corrigido
            mg_http_reply(c, 200,
                "Access-Control-Allow-Origin: *\r\n"
                "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n"
                "Access-Control-Allow-Headers: Content-Type\r\n"
                "Content-Length: 0\r\n"
                "\r\n",  // Corrigido: corpo vazio necessário
                "");
            return;
        }

        // Comparação manual da URI
        if (mg_strcmp(hm->uri, mg_str("/calcular")) == 0) {
            char op[10];
            char num1_str[20], num2_str[20];
            double num1, num2, resultado = 0;

            // Extrai parâmetros do corpo da requisição (form-urlencoded)
            mg_http_get_var(&hm->body, "num1", num1_str, sizeof(num1_str));
            mg_http_get_var(&hm->body, "num2", num2_str, sizeof(num2_str));
            mg_http_get_var(&hm->body, "operacao", op, sizeof(op));

            // Converte números
            num1 = atof(num1_str);
            num2 = atof(num2_str);

            // Processa a operação
            if (strcmp(op, "soma") == 0) {
                resultado = num1 + num2;
            } else if (strcmp(op, "subtracao") == 0) {
                resultado = num1 - num2;
            } else if (strcmp(op, "multiplicacao") == 0) {
                resultado = num1 * num2;
            } else if (strcmp(op, "divisao") == 0 && num2 != 0) {
                resultado = num1 / num2;
            } else {
                mg_http_reply(c, 400,
                    "Access-Control-Allow-Origin: *\r\n"
                    "Content-Type: text/plain\r\n",
                    "Erro: operação inválida ou divisão por zero!");
                return;
            }

            // Retorna JSON com o resultado e cabeçalhos CORS
            mg_http_reply(c, 200,
                "Content-Type: application/json\r\n"
                "Access-Control-Allow-Origin: *\r\n",
                "{ \"num1\": %g, \"num2\": %g, \"operacao\": \"%s\", \"resultado\": %g }",
                num1, num2, op, resultado);
        } else {
            // Se a rota não for /calcular, responde com erro e CORS
            mg_http_reply(c, 404,
                "Access-Control-Allow-Origin: *\r\n"
                "Content-Type: text/plain\r\n",
                "Rota não encontrada\n");
        }
    }
}

int main() {
    struct mg_mgr mgr;
    mg_mgr_init(&mgr);
    mg_http_listen(&mgr, s_listen_on, fn, &mgr);

    printf("Servidor rodando em %s\n", s_listen_on);
    while (1) {
        mg_mgr_poll(&mgr, 1000);
    }

    mg_mgr_free(&mgr);
    return 0;
}
