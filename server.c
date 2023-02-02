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

#define PORT 25006 // You can change port number here

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

  //address family = internet
  serv_addr.sin_family = AF_INET;
  //set ip address to localhost
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  //set port number using htons function to use proper byte order
  serv_addr.sin_port = htons(portno);

    //bind the address struct to the socket
    if(bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1){
      perror("bind error");
      exit(1);
    }

  //listen on the socket with 10 max connection requests queued
  if(listen(sockfd, 10) == -1){
    perror("listen error");
    exit(1);
  }


  printf("Server is running on port %d\n", portno);
    
    int authen = 0;
    while(1){
      newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

      //if accept has failed
      if(newsockfd == -1){
        perror("accept error");
        exit(1);
      }

      char buffer[9000];
      int bytes;
      bytes = 0;

      //receive the socket
      bytes = recv(newsockfd, buffer, 1500, 0);

      //send the 401 unauthorized message
      char message[] = "HTTP/1.1 401 Unauthorized\r\nWWW-authenticate: Basic realm='localhost'\r\nContent-Type: text/html;\r\n\r\n";
      
      //get the length of the message
      int length = strlen(message);

      //while the message exists, send it to the client
      while(length > 0){
        printf("send bytes : %d\n", bytes);
        bytes = send(newsockfd, message, length, 0);
        if(bytes==-1){
          continue;
        }
        length -= bytes;
      }

      //get the size of the token
      int tokensize = strlen(token);
      printf("Token: %s, tokensize : %d\n", token, tokensize);

      //receive a second time to see where it's stored in the buffer
      char* parse;
      bytes = 0;
      bytes = recv(newsockfd, buffer, 1500, 0);
      if(bytes == -1){
        printf("error receiving message");
        exit(1);
      }

      //parse the buffer at Authorization and at Basic
      parse = strtok(buffer, "\r\n");
      while(parse){
        parse = strtok(NULL, "\r\n");
        if(parse != NULL && strncmp(parse, "Authorization:", strlen("Authorization:" )) == 0){
          //print it out to see if it matches with token
          printf("%s\n", parse);
        }
      }

      //close the socket
      printf("close\n");
      shutdown(newsockfd, SHUT_RDWR);
      close(newsockfd);

      //this means that authorization was successful
      if(authen == 1){
        printf("success\n");
        break;
      }

      //this means it was not, continue to take in input until token and the inputted user/id match
      if(authen == 0){
        printf("Token: %s, tokensize : %d\n", token, tokensize);
        printf("not successful\n");
        if(strstr(buffer, token) != NULL){
          authen = 1;
        }
      }
    }

    //Respond loop
    while (1) {
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if ( newsockfd == -1 ){
          perror("accept error");
          exit(1);
        }
        printf("test");
        respond(newsockfd);
      }
  return 0;
}
//TODO: complete respond function 40% of score
int respond(int sock) {
  //fopen, strtok
  int offset, bytes;
  char buffer[9000];
  bzero(buffer, 9000);

  offset = 0;
  bytes = 0;
  do {
    // bytes < 0 : unexpected error
    // bytes == 0 : client closed connection
    bytes = recv(sock, buffer + offset, 1500, 0);
    offset += bytes;
    // this is end of http request
    if (strncmp(buffer + offset - 4, "\r\n\r\n", 4) == 0) break;
  } while (bytes > 0);

  if (bytes < 0) {
    printf("recv() error\n");
    return;
  } else if (bytes == 0) {
    printf("Client disconnected unexpectedly\n");
    return;
  }

  //opening the file
  FILE *fp = fopen("index.html", "r");
  if(fp == NULL){
    printf("error opening file");
    exit(1);
  }
  //read from the file
  ssize_t count = fread(sock, buffer, sizeof(buffer) - 1, fp);

  //if reading from the file has failed
  if(count < 0){
    printf("read error");
    exit(1);
  }
  if(count == 0){
    exit(1);
  }

  //print out the contents of index.html
  buffer[count] = '\0';
  printf("<%.s>\n", count, buffer);

  //close the socket
  printf("close\n");
  fclose(fp);
  shutdown(sock, SHUT_RDWR);
  close(sock);
    
  return 0;
}
