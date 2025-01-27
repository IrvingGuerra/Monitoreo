#ifndef SOCKETDATAGRAMA_H
#define SOCKETDATAGRAMA_H
#define BUFFERT 512

#include <arpa/inet.h>
#include <strings.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

#include <fcntl.h>

#include <iostream>

#include "PaqueteDatagrama.h"

class SocketDatagrama
{
public:
    SocketDatagrama(int puertoLocal);
    ~SocketDatagrama();
    //Recibe un paquete tipo datagrama proveniente de este socket
    int recibe(PaqueteDatagrama &p);
    //Envía un paquete tipo datagrama desde este socket
    int envia(PaqueteDatagrama &p);
    // Recibe un tiempo de caducidad.
    int recibeTimeout(PaqueteDatagrama & p, time_t segundos, suseconds_t microsegundos);
    
    int recibeImagen(const char *serverIpAdress, int serverPort);

    int enviaImagen(const char *serverIpAdress, int serverPort);

private:
    struct sockaddr_in direccionLocal;
    struct sockaddr_in direccionForanea;
    struct timeval timeout;
    int s; //ID socket
};

#endif