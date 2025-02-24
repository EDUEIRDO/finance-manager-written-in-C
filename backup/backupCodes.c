#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mongoose.h"

static const char *s_listen_on = "http://0.0.0.0:8000";

// Connection event handler function
static void fn(struct mg_connection *c, int ev, void *ev_data)
{
  if (ev == MG_EV_HTTP_MSG) // New http request received
  {
    struct mg_http_message *hm = (struct mg_http_message *) ev_data; // Parsed HTTP request
  //  if (mg_match(hm->uri, mg_str("/api/hello"), NULL)) // REST API call

  // Headers to allow CORS
      mg_http_reply(c, 200,
        "Content-Type: text/plain\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n"
        "Access-Control-Allow-headers: Content-Type\r\n",
        "Hello, Mongoose with CORS\n"); // Respond JSON
  }
};

int main()
{
  struct mg_mgr mgr; // Mongoose event manager. HOlds all connections
  mg_mgr_init(&mgr); // initialize event manager
  mg_http_listen(&mgr, s_listen_on, fn, &mgr); // Setup listener
  printf("Server running in %s\n", s_listen_on );
  while (true) // Infinite event loop
  {
    mg_mgr_poll(&mgr, 1000);
  }
  mg_mgr_free(&mgr);
  return 0;
}
