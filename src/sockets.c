#include "sockets.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int listentoport(int port) {
  // listen to the port while the app is running
  if (1 > port || 65535 < port) {
    return -1;
  }

  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (-1 == server_fd) {
    perror("sockets");
    return -1;
  }

  int optval = 1;
  int opt_r = setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
  if (-1 == opt_r) {
    perror("socket options");
    return -1;
  }

  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));

  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = htonl(INADDR_ANY);

  int bind_r = bind(server_fd, (struct sockaddr *)&addr, sizeof(addr));
  if (-1 == bind_r) {
    perror("binding");
    return -1;
  }

  int listen_r = listen(server_fd, 10);
  if (-1 == listen_r) {
    perror("listening");
    return -1;
  }

  while (true) {
    int client_fd = accept(server_fd, NULL, NULL);
    if (-1 == client_fd) {
      perror("connection failed");
      continue;
    }
    printf("Connecting with client and fd %d\n", client_fd);
    //parse http request
    close(client_fd);
  }

  close(server_fd);

  // send the parsed response to the router
  // return the response to the client
  return 0;
}
