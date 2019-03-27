//Taken from Abhishek Sagar

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <memory.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/*Server process is running on this port no. Client has to send data to this port no*/
// #define SERVER_PORT     2000 
// #define SERVER_IP_ADDRESS   "10.20.1.22"
#define FILENAME   "file.txt"
char SERVER_IP_ADDRESS[16] = "10.20.1.22";
int SERVER_PORT = 2000;


char data_buffer[1024];
char filename[100] = FILENAME;

int compute_number_of_words(char* filename){
  FILE * fp = fopen(filename, "r");
  if (fp == NULL) return 1;
  char c;
  int count = 0;
  while((c = fgetc(fp)) != EOF){
    if(c == ' ' || c == '\n'){
      ++count;
    }
  }
  fclose(fp);
  return (count + 1);
}

char** words_from_file_to_array(char* filename){
  int size = compute_number_of_words(filename);
  FILE * fp = fopen(filename, "r");
  char **data = (char **)calloc(size, sizeof(char *)); 
  for (int i=0; i<size; i++)  data[i] = (char *)calloc(100, sizeof(int));
  if (fp == NULL) return (char **)1;
  char c;
  int count = 0;
  int i = -1;
  while((c = fgetc(fp)) != EOF){
    if(c == ' ' || c == '\n'){
      data[count][++i] = '\0';
      count++;
      i = -1;
    }else 
      data[count][++i] = c;
  }
  fclose(fp);
  return data;
}

void server_add_peer_to_database(char *new_peer_name, char* ip, int port){
  char buffer[100];
  strcpy(buffer, new_peer_name);
  FILE* fp = fopen("peer_database.txt", "a");
  char int_string[10];
  strcat(buffer, ":");
  strcat(buffer, ip);
  snprintf(int_string, 10, "%d", port);
  strcat(buffer, ":");
  strcat(buffer, int_string);
  strcat(buffer, " ");
  fputs(buffer, fp);
  fclose(fp);
}


void client_request_file(int sockfd, char *filename){
  int sent_recv_bytes = 0;
  // SEND FILENAME TO SERVER
  char send_msg[100];
  printf("check if server has file\n");
  strcpy(send_msg, filename);
  sent_recv_bytes = send(sockfd, &send_msg, sizeof(send_msg), 0);
  // GET THE ANSWER FROM SERVER IF IT HAS FILE & # of words in file
  char result2[10];
  sent_recv_bytes =  recv(sockfd, (char *)&result2, sizeof(char)*10, 0);
  printf("No of bytes received = %d: %s\n", sent_recv_bytes, result2);
  char if_file_exist[4] = "none";
  int number_of_words = 10;
  sscanf(result2, "%s %d", if_file_exist, &number_of_words);
  printf("%s %d\n", if_file_exist, number_of_words);

  if (strcmp(if_file_exist, "yes") == 0){
    printf("file exist, request for download, #ofWords: %d\n", number_of_words);
    FILE* fp = fopen(filename, "w");
    if(fp == NULL) {printf("Unable to create file"); exit(-3);}
    int i = 1;
    char result3[50] = "string";
    // REQUEST i'th WORD FROM FILE FROM SERVER
    while (i <= number_of_words){
      // CONVERT i TO STRING msg
      snprintf(send_msg, 3, "%d", i);
        
      // SEND i TO SERVER
      printf("request %s'th word\n", send_msg);
      sent_recv_bytes = send(sockfd, &send_msg, sizeof(char)*3, 0);

      // RECEIVE THE i'th WORD ROM SERVER
      // -! size of receive buffer should be greater than sent buffer of server
      sent_recv_bytes =  recv(sockfd, (char *)&result3, sizeof(char)*50, 0);
      printf("client got: %s\n", result3);
      if (i > 1) fputs(" ", fp);
      fputs(result3, fp);
      i++;
    }
    fclose(fp);
  } else printf("server has no file\n");
}

int server_receive_option(int comm_socket_fd){
  int sent_recv_bytes = 0;
  sent_recv_bytes = recv(comm_socket_fd, (char *)data_buffer, sizeof(data_buffer), 0);
  // printf("Server got: %s\n", data_buffer);
  if(data_buffer[0] == '0'){
    // printf("will send file\n");
    return 0;
  }
  else if (data_buffer[0] = '1'){
    // printf("will synchronise\n");  
    return 1;
  } else {
    return -1;
    printf("Wrong option. connection refused\n");
  } 
}

int client_choose_option(){
  int option = -1;
  printf("Synchronise or download file?(1|0)\n");
  // char buffer[100];
  // fgets(buffer, 99, stdin);
  // sscanf(buffer, "%d", &option);
  scanf(" %d", &option);
  if(option == 1){
    return 1;
  }else if (option == 0){
    return 0;
  } else return -1;
}

void client_send_option(int sockfd, int option){
  int sent_recv_bytes = 0;
  char send_msg[2];
  snprintf(send_msg, 2, "%d", option);
  printf("Client ---------> server: %s\n", send_msg);
  sent_recv_bytes = send(sockfd, &send_msg, sizeof(send_msg), 0);
  
}

void server_send_file(int comm_socket_fd){
  int sent_recv_bytes = 0;
  sent_recv_bytes = recv(comm_socket_fd, (char *)data_buffer, sizeof(data_buffer), 0);
  char result[10] = "no";
  printf("Server got: %s\n", data_buffer);
  int fd = open(data_buffer, 2);
  if (fd > 0){
    strcpy(result, "yes ");
    int number_of_words = compute_number_of_words(data_buffer);
    char number_of_words_str[3] = "str";
    snprintf(number_of_words_str, 3, "%d", number_of_words);
    strcat(result, number_of_words_str);
  }        
  // SEND RESPONSE WITH yes/no & # of Words IN FILE
  sent_recv_bytes = send(comm_socket_fd, (char *)&result, sizeof(char)*10, 0);
  printf("Server ---------> client: '%s'\n", result);
  char **data = words_from_file_to_array(data_buffer);
  int i;
  while(1){
  // TODO: this loop should be executed only when (fd > 0)
  //    & # of iterations no more then words in file
  // RECEIVE # OF WORD TO SEND
    sent_recv_bytes = recv(comm_socket_fd, (char *)data_buffer, sizeof(data_buffer), 0);
    if (sent_recv_bytes == 0) break;
    sscanf(data_buffer, "%d", &i);
    printf("Server got:%s\n", data_buffer);
    // SEND CURRENT FILE WORD TO CLIENT
    char current_word[30];
    strcpy(current_word, data[i-1]);
    sent_recv_bytes = send(comm_socket_fd, (char *)&current_word, sizeof(char)*50, 0);
    printf("Server -----------> client: %s\n", current_word);
  }
}

void setup_tcp_server_communication(){
  int master_sock_tcp_fd = 0, 
       sent_recv_bytes = 0, 
       addr_len = 0, 
       opt = 1;
   int comm_socket_fd = 0;
   fd_set readfds;             
   struct sockaddr_in server_addr, 
                      client_addr;
   if ((master_sock_tcp_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP )) == -1){
       printf("socket creation failed\n");
       exit(1);
   }
   server_addr.sin_family = AF_INET;
   server_addr.sin_port = SERVER_PORT;
   server_addr.sin_addr.s_addr = INADDR_ANY; 
   addr_len = sizeof(struct sockaddr);
   if (bind(master_sock_tcp_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1){
       printf("socket bind failed\n");
       return;
   }
  server_add_peer_to_database("first_peer_server", SERVER_IP_ADDRESS, SERVER_PORT);

  struct sockaddr_in sin;
  socklen_t len = sizeof(sin);
  if (getsockname(master_sock_tcp_fd, (struct sockaddr *)&sin, &len) == -1)
      perror("getsockname");
  else
      // printf("port number %d\n", ntohs(sin.sin_port));

  if (listen(master_sock_tcp_fd, 5)<0) {
      printf("listen failed\n");
      return;
  }
  int i = 0;
	while(1){
    i++;
		FD_ZERO(&readfds);                     
    FD_SET(master_sock_tcp_fd, &readfds);  
    // printf("blocked on select System call...\n");
    select(master_sock_tcp_fd + 1, &readfds, NULL, NULL, NULL); 
    if (FD_ISSET(master_sock_tcp_fd, &readfds)){ 
      // Connection run
      printf("New connection recieved recvd, accept the connection. Client and Server completes TCP-3 way handshake at this point\n");
      comm_socket_fd = accept(master_sock_tcp_fd, (struct sockaddr *)&client_addr, &addr_len);
      if(comm_socket_fd < 0){ printf("accept error : errno = %d\n", errno); exit(0); }        
      // SERVER ESTABLISHED CONNECTION WITH NEW NODE
      char int_string[10], new_peer_name[10] = "name_";

      snprintf(int_string, 3, "%d", i);
      strcat(new_peer_name, int_string);
      printf("Got new node! %s:%s:%u\n ", new_peer_name,inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
      server_add_peer_to_database(new_peer_name, inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));
      // while(1){
        printf("Wait for an option from client:\n");
        int option = 1;
        option = server_receive_option(comm_socket_fd);
        
        if(option == 1){
          server_send_file(comm_socket_fd);
        }else if (option == 0){
          server_send_file(comm_socket_fd);
        } else printf("Server got wrong option\n");
        
        
          
        // }
    }
  }
}

void setup_tcp_client_communication(/*char *SERVER_IP_ADDRESS, int SERVER_PORT*/) {
  printf("%s:%d\n", SERVER_IP_ADDRESS, SERVER_PORT);
  int sockfd = 0, 
  sent_recv_bytes = 0;
  int addr_len = 0;
  addr_len = sizeof(struct sockaddr);
  struct sockaddr_in dest;
  dest.sin_family = AF_INET;
  dest.sin_port = SERVER_PORT;
  struct hostent *host = (struct hostent *)gethostbyname(SERVER_IP_ADDRESS);
  dest.sin_addr = *((struct in_addr *)host->h_addr);
  sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	// CONNECT TO THE SERVER
  connect(sockfd, (struct sockaddr *)&dest,sizeof(struct sockaddr));

  //  'CLIENT' PEER LOGIC
  int option = client_choose_option();
  client_send_option(sockfd, option);
  if (option == 1){
    client_request_file(sockfd, "peer_database.txt");
  } else if (option == 0) {
    printf("debug1\n");
    client_request_file(sockfd, "file.txt");
    
  } else printf("Error with choosing an option\n");
  
  
}



int main(int argc, char **argv){
  char option = 'd';
  while(1){
    printf("Run as a client or as a server?(c|s)\n");
    scanf(" %c", &option);
    if(option == 's'){
      setup_tcp_server_communication();
    } else if (option = 'c'){
      
      printf("type IP and PORT of peer to connect\n");
      scanf("%s %d", SERVER_IP_ADDRESS, &SERVER_PORT);
      // scanf(" %d\n", &SERVER_PORT);
      
      setup_tcp_client_communication();
    } else {
      printf("Wrong option. (type only one character)\n");
      return -1;
    }
  }
  
  return 0;
}