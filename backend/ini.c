#include <stdio.h>
#include <microhttpd.h>
#include <string.h>
#include <stdlib.h>

#define PORT 8080

// Function response handler
enum MHD_Result answer_to_connection(void *cls, struct MHD_Connection *connection,
                                     const char *url, const char *method,
                                     const char *version, const char *upload_data,
                                     size_t *upload_data_size, void **con_cls) {
  
  const char *response_text = "Hello, world!";
  struct MHD_Response *response = MHD_create_response_from_buffer(strlen(response_text), (void *)response_text, MHD_RESPMEM_PERSISTENT);
  
// Send response
  enum MHD_Result ret = MHD_queue_response(connection, MHD_HTTP_NOT_FOUND, response);

// Free response object
  MHD_destroy_response(response);
  return ret;
}

int main() {
  struct MHD_Daemon *daemon;

  // Start the server
  daemon = MHD_start_daemon(MHD_USE_THREAD_PER_CONNECTION, PORT, NULL, NULL, &answer_to_connection, NULL, MHD_OPTION_END);

  if (!daemon) {
    fprintf(stderr, "Error when starting the server\n");
    return 1;
  }

  printf("Server running at http://localhost:%d\n", PORT);
  getchar(); // Press enter to quit

  MHD_stop_daemon(daemon);
  return 0;
}
