#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <memory.h>
#include <errno.h>
// #include <zconf.h>
#include "common.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define DEST_PORT            "4950"
#define SERVER_IP_ADDRESS   "10.20.1.22"

test_struct_t client_data;
result_struct_t result;

void setup_udp_communication() {
    /*Step 1 : Initialization*/
    /*Socket handle*/
    int sockfd = 0, 
	sent_recv_bytes = 0;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	int numbytes;


	memset(&hints, 0, sizeof hints);
	
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;

	if ((rv = getaddrinfo(SERVER_IP_ADDRESS, DEST_PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return;
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
		return;
	}
	// freeaddrinfo(servinfo);

    while(1) {

	    printf("\nEnter name: ");
	    scanf("%s", &client_data.name);
	    printf("\nEnter  age: ");
	    scanf("%s", &client_data.age);
        printf("\nEnter BS number: ");
        scanf("%s", &client_data.bs_number);
	    printf("\n");
	    
	    if ((numbytes = sendto(sockfd, &client_data, sizeof(test_struct_t), 0,
				 p->ai_addr, p->ai_addrlen)) == -1) {
			perror("talker: sendto");
			exit(1);
		}
	    
	    printf("No of bytes sent = %d\n", numbytes);
		printf("Blocked on receive call\n");	 
	    // sent_recv_bytes =  recvfrom(sockfd, (char *)&result, sizeof(result_struct_t), 0,
		   //          (struct sockaddr *)&dest, &addr_len);
	    // sent_recv_bytes =  recvfrom(sockfd, (char *)&result, sizeof(result_struct_t), 0,
		   //           p->ai_addr, p->ai_addrlen);

	    printf("No of bytes received = %d\n", sent_recv_bytes);
	    
	    printf("Result received = %s\n", result.conc);
    }
}
    

int main(int argc, char **argv) {
    setup_udp_communication();
    return 0;
}

