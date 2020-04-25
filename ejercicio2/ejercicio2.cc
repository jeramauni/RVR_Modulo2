#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <time.h>

/*
argv[0] ---> nombre del programa
argv[1] ---> primer argumento (char *)

./addrinfo www.ucm.es 80
argv[0] = "./addrinfo"
argv[1] = "www.ucm.es"
argv[2] = "80"
|
|
V
res->ai_addr ---> (socket + bind)
|
|
V
host (numeric)

./addrinfo 127.0.0.1 80
./addrinfo www.ucm.es http
*/

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
// RECEPCIÓN MENSAJE DE CLIENTE //
// ---------------------------------------------------------------------- //
char buffer [80];
char host[NI_MAXHOST];
char service[NI_MAXSERV];
struct sockaddr client_addr;
socklen_t client_len = sizeof(struct sockaddr);

ssize_t bytes = recvfrom(sd, buffer, 79 * sizeof(char) , 0, &client_addr, &client_len);

while(true)
{

    //Detecta si se introduce un comando válido
    while (bytes != 2 || !isViable(buffer[0])) 
    {
        if ( bytes == -1)
        {
            std::cerr << "recvfrom error " << std::endl;
            return -1;
        }
        std::cout << "Comando incorrecto... \n Los comandos aceptados son \'t\'(hora) / \'d\'(fecha) / \'q\'(terminar proceso) \n";
        bytes = recvfrom(sd, buffer, 79 * sizeof(char) , 0, &client_addr, &client_len);
    }

    //Si llegó hasta aquí significa que se ha introducido un comando correcto
    //Evitar basura en el buffer-------------
    char exactBuff [bytes];
    memcpy(exactBuff, buffer, bytes);
    //---------------------------------------

    switch (exactBuff[0])
    {
        case 't':   //hora
        std::cout << "hora! \n";
            break;
        case 'd':   //fecha
        std::cout << "fecha! \n";
            break;
        case 'q':   //quit
        std::cout << "quit! \n";
        return 0;
            break;
        default:
            break;
    }
    time_t t;
   
}
/*getnameinfo(&client_addr, client_len, host, NI_MAXHOST, service,
NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);

std::cout << "IP: " << host << " PUERTO: " << service
<< " MENSAJE: " ;

for(size_t i = 0; i < bytes; i++)
{
    std::cout << exactBuff[i];
}
std::cout << '\n';
// ---------------------------------------------------------------------- //
// PROCESAMIENTO DEL COMANDO//
// ---------------------------------------------------------------------- //
  
    
// ---------------------------------------------------------------------- //
// RESPUESTA AL CLIENTE //
// ---------------------------------------------------------------------- //
sendto(sd, exactBuff, bytes, 0, &client_addr, client_len);*/

return 0;
}