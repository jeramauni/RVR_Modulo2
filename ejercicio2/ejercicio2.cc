#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <iostream>

//Comprueba si char command es un comando registrado
bool isViable (char command)
{
    if (command == 't' || command == 'd' || command == 'q') {
        return true;
    }
    return false;
}

int main(int argc, char **argv)
{
    struct addrinfo hints;
    struct addrinfo * res;

    // ---------------------------------------------------------------------- //
    // INICIALIZACIÓN SOCKET & BIND //
    // ---------------------------------------------------------------------- //

    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    int rc = getaddrinfo(argv[1], argv[2], &hints, &res);

    if ( rc != 0 )
    {
    std::cerr << "getaddrinfo error: " << gai_strerror(rc) << std::endl;
    return -1;
    }

    // res contiene la representación como sockaddr de dirección + puerto

    int sd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

    if ( bind(sd, res->ai_addr, res->ai_addrlen) != 0 )
    {
    std::cerr << "bind error " << std::endl;
    return -1;
    }

    freeaddrinfo(res);

    // ---------------------------------------------------------------------- //
    // RECEPCIÓN MENSAJE DE CLIENTE //
    // ---------------------------------------------------------------------- //
    char buffer [80];
    bool run = true;
    char host[NI_MAXHOST];
    char service[NI_MAXSERV];
    struct sockaddr client_addr;
    socklen_t client_len = sizeof(struct sockaddr);

    while(run)
    {
        //Evitar basura en el buffer-------------
        memset(&buffer, 0, sizeof(buffer));
        ssize_t bytes = recvfrom(sd, buffer, 79 * sizeof(char) , 0, &client_addr, &client_len);
        
        //Detecta si se introduce un comando válido
        while (bytes != 2 || !isViable(buffer[0])) 
        {
            if ( bytes == -1)
            {
                std::cerr << "recvfrom error " << std::endl;
                return -1;
            }

            //Output de comando incorrecto/ no soportado         
            std::cout << "Comando incorrecto..." << buffer;                   
            std::cout <<"Los comandos aceptados son \'t\'(hora) / \'d\'(fecha) / \'q\'(terminar proceso) \n";
            //Evitar basura en el buffer-------------
            memset(&buffer, 0, sizeof(buffer));
            bytes = recvfrom(sd, buffer, 79 * sizeof(char) , 0, &client_addr, &client_len);
        }

        //Si llegó hasta aquí significa que se ha introducido un comando correcto
        
        // ---------------------------------------------------------------------- //
        // PROCESAMIENTO DEL COMANDO//
        // ---------------------------------------------------------------------- //
        
        char respuesta [80];
        
        getnameinfo(&client_addr, client_len, host, NI_MAXHOST, service,
        NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);

        std::cout << bytes << " bytes   de  " << host << ":" << service << '\n';

        //Procesamiento de fecha y hora y selección de respuesta
        time_t current_time;
        struct tm * timeinfo;
        current_time = time(NULL);
        timeinfo = localtime(&current_time);

        switch (buffer[0])
        {
            case 't':   //hora
                strftime(respuesta,80, "%r", timeinfo);
                break;
            case 'd':   //fecha
                strftime(respuesta,80, "%F", timeinfo);
                break;
            case 'q':   //quit
                std::cout << "saliendo... \n";
                run = false;
                break;
            default:
                break;
        }
               
        // ---------------------------------------------------------------------- //
        // RESPUESTA AL CLIENTE //
        // ---------------------------------------------------------------------- //
        sendto(sd, respuesta, strlen(respuesta), 0, &client_addr, client_len);
        memset(&respuesta, 0, sizeof(respuesta));
    }
    return 0;
}