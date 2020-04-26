#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <iostream>

int main(int argc, char **argv)
{
    struct addrinfo hints;
    struct addrinfo * res;

    // ---------------------------------------------------------------------- //
    // INICIALIZACIÓN SOCKET//
    // ---------------------------------------------------------------------- //

    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;


    int rc = getaddrinfo(argv[1], argv[2], &hints, &res);


    if(rc != 0)
    {
        std::cerr << "getaddrinfo error: " << gai_strerror(rc) << std::endl;
        return -1;
    }

    // res contiene la representación como sockaddr de dirección + puerto

    int sd = socket (res->ai_family, res->ai_socktype, res->ai_protocol);

    freeaddrinfo(res);

    char buffer[80]; 
    char host[NI_MAXHOST];
    char service[NI_MAXSERV];

    //Evitar basura en el buffer-------------
    memset(&buffer, 0, sizeof(buffer));

    getnameinfo(res->ai_addr, res->ai_addrlen, host, NI_MAXHOST, service, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);


    sendto(sd, argv[3], strlen(argv[3]), 0, res->ai_addr, res->ai_addrlen);
    recvfrom(sd, buffer, sizeof(buffer), 0,res->ai_addr, &res->ai_addrlen);

    std::cout << buffer << std::endl;

    return 0;
}