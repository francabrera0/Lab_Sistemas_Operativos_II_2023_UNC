#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cjson/cJSON.h>

#include "client.h"
#include "ipv4Client.h"
#include "ipv6Client.h"
#include "unixClient.h"

int main( int argc, char *argv[] ) {

	if(argc < 3) {
		perror("Invalid arguments, please specify <Client Type> <host> <port> in IPv4 and IPv6 clients or <Client Type> <file UnixIpc> in unix client");
		exit(EXIT_FAILURE);
	}

	char* type = argv[1];
	if((!strcmp(type, "ipv4")) && (argc == 4)) {
		createIpv4ClientSocket(argv[2], argv[3]);
	}

	if((!strcmp(type, "ipv6")) && (argc == 4)) {
		createIpv6ClientSocket(argv[2], argv[3]);
	}

	if((!strcmp(type, "unix")) && (argc == 3)) {
		createUnixClientSocket(argv[2]);
	}

	else {
		perror("Invalid client type. Type ipv4 -> IPv4, Type ipv6 -> IPv6, Type unix -> Unix");
		exit(EXIT_FAILURE);
	}

} 