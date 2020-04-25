#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

#include <iostream>

/*
argv[0] ---> ./ejercicio1
argv[1] --->  IPv4/nombre de dominio/nombre en /etc/hosts (char *)

ej.1
./ejercicio1 www.google.com
ej.2
./ejercicio1 147.96.1.9
*/

void OutputInfo (struct addrinfo* res, const char * host)
{
    std::cout << host << "      " << res->ai_family << "        " << res->ai_socktype << '\n';
}

int main(int argc, char **argv)
{
    const char * HOST_ = argv[1];
    const char * SERVICE_ = NULL;
    
    //Filter
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = 0;

    struct addrinfo * res;

    int err_addrinfo = getaddrinfo(HOST_, SERVICE_, &hints, &res);

    if (err_addrinfo != 0) {
        std::cerr << "ERROR (getaddrinfo): " << gai_strerror(err_addrinfo) << std::endl;
        return -1;
    }
    
    char host[NI_MAXHOST];
    char service[NI_MAXSERV];

    for(struct addrinfo * i = res; i != NULL; i = i->ai_next)
    {
        int err_getnameinfo = getnameinfo(i->ai_addr, i->ai_addrlen, host, NI_MAXHOST, service, NI_MAXSERV, NI_NUMERICHOST);
        
        if (err_getnameinfo != 0) {
        std::cerr << "ERROR (getnameinfo): " << gai_strerror(err_getnameinfo) << std::endl;
        return -1;
        }

        OutputInfo(i, host);
    }

    freeaddrinfo(res);
    
    return 0;
}