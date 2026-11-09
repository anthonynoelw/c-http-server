#include "sockets.h"
#include <stdlib.h>
#include <sys/socket.h>


int main(int argc, char* argv[]){
    if(0 == argc){
      free(argv);
    }

    int list_r = listen_to_port(8080);
    if(-1 == list_r){
        return -1;
    }

    return 0;
}
