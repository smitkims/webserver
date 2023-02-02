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
//Problem 1 of project 1:simple webserver with authentification
//Both Problem 1 and 2 was tested on WSL enviroments, Linux, and M1 mac
//But If you still have problems on running codes please mail us
//Most importantly please comment your code

//If you are using mac 
//You can install homebrew here :https://brew.sh
//And open terminal and type 
//sudo brew install gcc
//sudo brew install make
//Type make command to build server
//And server binary will be created
//Use ifconfig command to figure out your ip(usually start with 192. or 172.)
//run server with ./server and open browser and type 192.x.x.x:25000

//If you are using Linux or WSL
//You just need to run "make"(If you are using WSL you may need to install gcc and make with apt)
//And server binary will be created
//Use ifconfig command to figure out your ip(usually start with 192. or 172.)
//run server with ./server and open browser and type 192.x.x.x:25000

//It will be better if you run virtual machine or other device to run server
//But you can also test server with opening terminal and run it on local IP 

int main( int argc, char *argv[] ) {
  int sockfd, newsockfd, portno = PORT;
  socklen_t clilen;
  struct sockaddr_in serv_addr, cli_addr;
  clilen = sizeof(cli_addr);

  printf("encoding start \n");// We have implemented base64 encoding you just need to use this function
  char *token = base64_encode("2018-13749:password", strlen("2018-13749:password"));//you can change your userid
  printf("encoding end \n");

  //browser will repond with base64 encoded "userid:password" string 
  //You should parse authentification information from http 401 responese and compare it

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
  /* TODO : Now bind the host address using bind() call. 10% of score*/
    //it was mostly same as tutorial

    // bind socket to the specified IP and port
  if(bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1){
    perror("bind error");
    exit(1);
  }
  /* TODO : listen on socket you created  10% of score*/
  //listen for connections
  if(listen(sockfd, 10) == -1){
    perror("listen error");
    exit(1);
  }

  printf("Server is running on port %d\n", portno);
    
    //it was mostly same as tutorial
    //in the while loop every time request comes we respond with respond function if valid

    //TODO: authentication loop 40 % of score
    while(1){
      newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
      //TODO: accept connection
      //TODO: send 401 message(more information about 401 message : https://developer.mozilla.org/en-US/docs/Web/HTTP/Authentication) and authentificate user
      //close connection
      if (newsockfd == -1 ){
          perror("accept error");
          exit(1);
        }
        
        //browser will repond with base64 encoded "userid:password" string 
        //You should parse authentification information from http 401 responese and compare it
        //respond(newsockfd);
      }

  return 0;
}
//TODO: complete respond function 40% of score
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


