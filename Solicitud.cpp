#include "Solicitud.h"
#include <iostream>

Solicitud::Solicitud(){
    socketlocal = new SocketDatagrama(0);
}

bool Solicitud::ipDisponible(const char *serverIpAdress, int serverPort){
    //Creamos el cuerpo del mensaje
    mensaje _request;
    unsigned int requestID = rand();
    _request.messageType = REQUEST;
    _request.requestID = requestID;
    _request.operationID = AVAIABLE;
    memcpy(_request.args, "DISPONIBILIDAD", 2);

    PaqueteDatagrama saliente((char *)&_request, sizeof(_request), serverIpAdress, serverPort);
    PaqueteDatagrama entrante(MAX_DATA_SIZE + 12);

    //Intentamos enviar el dato 2 veces
    register int i = 0;
    for (i = 0; i < 2; i++) {
        socketlocal->envia(saliente);
        if (socketlocal->recibeTimeout(entrante, 1, 0) != -1) {
            break;
        }
    }
    if (i == 2) {
        perror("El servidor no está dsponible. Intente más tarde.");
        return false;
    }else{
        return true;
    }

}

bool Solicitud::makeScreenshoot(const char *serverIpAdress, int serverPort, int calidad){
    mensaje _request;
    unsigned int requestID = rand();
    _request.messageType = REQUEST;
    _request.requestID = requestID;
    _request.operationID = CAPTURA;

    char *quality = (char *) calloc(3,sizeof(char));
    sprintf(quality, "%d", calidad);

    memcpy(_request.args, quality, 3*sizeof(quality));

    PaqueteDatagrama saliente((char *)&_request, sizeof(_request), serverIpAdress, serverPort);
    socketlocal->envia(saliente);

/*
    register int i = 0;
    for (i = 0; i < 5; i++) {
        if (socketlocal->recibeImagen(serverIpAdress, serverPort) != -1) {
            break;
        }
    }

    */

    socketlocal->recibeImagen(serverIpAdress, serverPort);

    free(quality);

    return true;

}

void Solicitud::cerrarSocket(){
    delete socketlocal;
}