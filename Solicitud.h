#ifndef SOLICITUD_H
#define SOLICITUD_H

#include "SocketDatagrama.h"
#include "mensaje.h"

class Solicitud{
private:
	// Socket local para enviar y recibir paquetes por parte del cliente.
	SocketDatagrama *socketlocal;

public:
	// Constructor de una objeto que realiza solicitudes.
	Solicitud();
	// Operación para enviar petición. Retorna respuesta recibida.
	char *doOperation(const char *IpAddress, int puerto, int operationId, char *args, unsigned int argsLen);
	// Operacion para preguntar si la IP es disponible para servidor
	bool ipDisponible(const char *IpAddress, int puerto);
	bool makeScreenshoot(const char *IpAddress, int puerto, int calidad);
};
#endif