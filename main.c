#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mongoose.h"

static const char *s_listen_on = "http://0.0.0.0:8000";

static void fn(struct mg_connection *c, int ev, void *ev_data)
{
  if (ev == MG_EV_HTTP_MSG)
  {
    strcut mg_http_message *hm = (struct mg_http_message *) ev_data;

    // Verify if is a OPTIONS request
    if (mg_strcmp(hm->method, mg_str("OPTIONS")) == 0)
    {
      mg_http_reply(c, 200,
          "Access-Control-Allow-Origin: *\r\n"
          "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n"
          "Access-Control-Allow-Headers: Content-Type\r\n"
          "Content-Length: 0\r\n"
          "\r\n",  // Empty body if necessary
          "");
          return
    }

    // Manual URI comparison
    if (mg_strcmp(hm->uri, mg_str("/calcular")) == 0)
    {
        char op[10];
        char num1_str[20], num2_str[20];
        double num1, num2, resultado = 0;

        // Extract parameters from the request body (form-urlencoded)
        mg_http_get_var(&hm->body, "num1", num1_str, sizeof(num1_str));
        mg_http_get_var(&hm->body, "num2", num2_str, sizeof(num2_str));
        mg_http_get_var(&hm->body, "operacao", op, sizeof(op));

        // Convert number
        num1 = atof(num1_str);
        num2 = atof(num2_str);

        // Process the operation
        if (strcmp(op, "soma") == 0)
        {
            resultado = num1 + num2;
        } else if (strcmp(op, "subtracao") == 0)
        {
            resultado = num1 - num2;
        } else if (strcmp(op, "multiplicacao") == 0)
        {
            resultado = num1 * num2;
        } else if (strcmp(op, "divisao") == 0 && num2 != 0)
        {
            resultado = num1 / num2;
        } else
        {
            mg_http_reply(c, 400,
                "Access-Control-Allow-Origin: *\r\n"
                "Content-Type: text/plain\r\n",
                "Erro: operação inválida ou divisão por zero!");
            return;
        }

        // Returns JSON with the result and CORS headers
        mg_http_reply(c, 200,
            "Content-Type: application/json\r\n"
            "Access-Control-Allow-Origin: *\r\n",
            "{ \"num1\": %g, \"num2\": %g, \"operacao\": \"%s\", \"resultado\": %g }",
            num1, num2, op, resultado);
    } else
    {
        // If the route is not /calculate, it responds with error and CORS
        mg_http_reply(c, 404,
            "Access-Control-Allow-Origin: *\r\n"
            "Content-Type: text/plain\r\n",
            "Rota não encontrada\n");
    }
  }
}

int main()
{
  struct mg_mgr mgr;
  mg_mgr_init(&mgr);
  mg_http_listen(&mgr, s_listen_on, fn, &mgr);

  printf("Server running in %s\n", s_listen_on);
  while (1)
  {
    mg_mgr_poll(&mgr, 1000);
  }
  mg_mgre_free(&mgr);
  return 0;
}
