#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <string.h>

#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "base64.c"

#define PORT 25000 // You can change port number here

int respond (int sock);

char usrname[] = "username";
char password[] = "password";
#include <stdint.h>
#include <stdlib.h>

int main( int argc, char *argv[] ) {
  int sockfd, newsockfd, portno = PORT;
  socklen_t clilen;
  struct sockaddr_in serv_addr, cli_addr;
  clilen = sizeof(cli_addr);

  printf("encoding start \n");
  char *token = base64_encode("2018-13749:password", strlen("2018-13749:password"));//you can change your userid
  printf("encoding end \n");

  /* First call to socket() function */
  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  if (sockfd < 0) {
    perror("ERROR opening socket");
    exit(1);
  }

  // port reusable
  int tr = 1;
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &tr, sizeof(int)) == -1) {
    perror("setsockopt");
    exit(1);
  }

  /* Initialize socket structure */
  bzero((char *) &serv_addr, sizeof(serv_addr));

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);

    // bind socket to the specified IP and port
  if(bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1){
    perror("bind error");
    exit(1);
  }
  //listen for connections
  if(listen(sockfd, 10) == -1){
    perror("listen error");
    exit(1);
  }

  printf("Server is running on port %d\n", portno);

    while(1){
      newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
      //close connection
      if (newsockfd == -1 ){
          perror("accept error");
          exit(1);
        }
        respond(newsockfd);
      }

  return 0;
}
// fopen, strtok
int respond(int sock) {
  int offset, bytes;
  char buffer[9000];
  char message[] = "GET / HTTP/1.1\r\n\r\n";

  // opening the file
        FILE *fp = fopen("index.html", "r");
        if (fp == NULL){
          printf("error opening file");
          exit(1);
        }
        char file_data[9000];
        char data[9000] = {0};
        size_t nbytes = 0;

        while((nbytes = fread(data, sizeof(char), 9000, fp)) > 0){
          if(send(sock, data, nbytes, 0) == -1){
            perror("error in sending file");
            exit(1);
          }
        }

  //int length = strlen(message);
  /*while(length > 0){
    printf("send bytes : %d\n", bytes);
    bytes = send(sock, message, length, 0);
    length = length - bytes;
  }*/
  printf("close\n");
  shutdown(sock, SHUT_RDWR);
  close(sock);
}


