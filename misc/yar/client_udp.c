#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <memory.h>
#include "common.h"

#define DEST_PORT            "4950"
#define SERVER_IP_ADDRESS   "10.20.1.22"

test_struct_t client_data;
result_struct_t result;

void setup_udp_communication() {
    /*Step 1 : Initialization*/
    /*Socket handle*/
    int sockfd = 0,
        sent_recv_bytes = 0;
    int rv;
    int numbytes;


    struct addrinfo hints, *servinfo, *p;

    memset(&hints, 0, sizeof hints);

    hints.ai_family = AF_UNSPEC;
  	hints.ai_socktype = SOCK_DGRAM;

    if ((rv = getaddrinfo(SERVER_IP_ADDRESS, DEST_PORT, &hints, &servinfo)) != 0) {
      fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
      return 1;
    }

    // loop through all the results and make a socket
  	for(p = servinfo; p != NULL; p = p->ai_next) {
  		if ((sockfd = socket(p->ai_family, p->ai_socktype,
  				p->ai_protocol)) == -1) {
  			perror("talker: socket");
  			continue;
  		}

  		break;
  	}

    if (p == NULL) {
      fprintf(stderr, "talker: failed to create socket\n");
      return 2;
    }

    while(1) {

	    printf("Enter name: ?\n");
	    scanf("%s", &client_data.name);
	    printf("Enter age : ?\n");
	    scanf("%s", &client_data.age);
      printf("Enter group: ?\n");
	    scanf("%s", &client_data.group);

      if ((numbytes = sendto(sockfd, &client_data, sizeof(test_struct_t), 0,
    			 p->ai_addr, p->ai_addrlen)) == -1) {
    		perror("talker: sendto");
    		exit(1);
    	}

	    printf("No of bytes sent = %d\n", numbytes);

	    sent_recv_bytes =  recvfrom(sockfd, (char *)&result, sizeof(result_struct_t), 0,
		             p->ai_addr, p->ai_addrlen);

	    printf("No of bytes received = %d\n", sent_recv_bytes);

	    printf("Result received = %s\n", result.c);
    }
}




int main(int argc, char **argv) {
    setup_udp_communication();
    printf("application quits\n");
    return 0;
}
