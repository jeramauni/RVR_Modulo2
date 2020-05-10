#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

#include <thread>
#include <vector>
#include <iostream>

/*
./tcp 0.0.0.0 7777
*/

class TCPConexion
{
private:
    int sd;
    int id;

public:
    TCPConexion(int sd_, int id_){sd = sd_; id = id_;}

    void haz_conexion()
    {

        // ---------------------------------------------------------------------- //
        // GESTION DE LA CONEXION CLIENTE //
        // ---------------------------------------------------------------------- //
        char buffer[80];
        while(true) //Hasta que se pierda la conexion
        {
            memset(&buffer, 0, sizeof(buffer));

            ssize_t bytes = recv(sd, (void *) buffer, sizeof(char)*79, 0);

            if ( bytes <= 0 )
            {               
                std::cout << '[' << id << "] disconnected... \n";
                close(sd);
                return;
            }

            std::cout << '[' << id << ']' << " MENSAJE: " << buffer << '\n';

            send(sd, (void *) buffer, bytes, 0);
        }
    };
};

int main(int argc, char **argv)
{
    struct addrinfo hints;
    struct addrinfo * res;

    // ---------------------------------------------------------------------- //
    // INICIALIZACIÓN SOCKET & BIND //
    // ---------------------------------------------------------------------- //

    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int rc = getaddrinfo(argv[1], argv[2], &hints, &res);

    if ( rc != 0 )
    {
        std::cerr << "getaddrinfo: " << gai_strerror(rc) << std::endl;
        return -1;
    }

    // res contiene la representación como sockaddr de dirección + puerto

    int sd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

    if ( bind(sd, res->ai_addr, res->ai_addrlen) != 0 )
    {
        std::cerr << "bind: " << std::endl;
        return -1;
    }

    freeaddrinfo(res);

    // ---------------------------------------------------------------------- //
    // PUBLICAR EL SERVIDOR (LISTEN) //
    // ---------------------------------------------------------------------- //
    listen(sd, 16);

    // ---------------------------------------------------------------------- //
    // GESTION DE LAS CONEXIONES AL SERVIDOR //
    // ---------------------------------------------------------------------- //


    char host[NI_MAXHOST];
    char service[NI_MAXSERV];
    struct sockaddr client_addr;
    socklen_t client_len = sizeof(struct sockaddr);

    int id = 1;

    while(true)
    {       
        int sd_client = accept(sd, &client_addr, &client_len);

        getnameinfo(&client_addr, client_len, host, NI_MAXHOST, service,
        NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);

        std::cout << "CONEXION DESDE IP: " << host << " PUERTO: " << service << '\n';

        TCPConexion* th = new TCPConexion(sd_client, id);  id++;
        std::thread([&th]()
        {
            th->haz_conexion();
        }).detach();
    }
    return 0;
}