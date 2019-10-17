#include "Solicitud.h"
#include <iostream>

int main(int argc, char const *argv[]){

    //PC CENTRAL SERVIRA COMO CLIENTE UDP

    if (argc != 5){
        printf("Forma de ejecutar: ./%s [ID_RED] [SERVER PORT] [HOST INICIAL] [HOST FINAL]\n", argv[0]);
        exit(0);
    }

    // Creamos una solicitud
    Solicitud solicitud;

    /*
    char * id_inicial = (char*)argv[1];
    size_t size = strlen(id_inicial);
    char * id_red_clean = (char *) calloc(size-1,sizeof(char));
    strncpy(id_red_clean, id_inicial, size-1);

    register int i = 0;

    for (i = atoi(argv[3]); i <= atoi(argv[4]); ++i){
        char *id_host = (char *) calloc(size+3,sizeof(char));
        char *host = (char *) calloc(3,sizeof(char));
        sprintf(host, "%d", i);
        strncpy(id_host, id_red_clean, size+3);
        strcat(id_host, host);
        std::cout << "[ INFO ] " << "Conectandose con direccion: " << id_host << std::endl;
        //Haremos una peticion para verificar si funciona como servidor la IP
        bool aviable = solicitud.ipDisponible(id_host, atoi(argv[2])); //Se pregunta si esta disponible
        std::cout << "[ INFO ] " << "Disponible: " << aviable << std::endl;
    }
    */

    int reqs = atoi(argv[3]);
    register int i = 0, resultado;
    int valorEsperado = 0;
    for (i = 0; i < reqs; i++)
    {
        int valorDeposito = rand() % 8 + 1;
        resultado = *(int *)solicitud.doOperation(argv[1], atoi(argv[2]), DEPOSITO, (char *)&valorDeposito, sizeof(valorDeposito));
        valorEsperado += valorDeposito;
        if (valorEsperado == resultado) {
            printf("Depósito de $%d realizado. Ahora se tiene $%d en la cuenta.\n", valorDeposito, resultado);
        } else {
            printf("Fallo en depósito %d de $%d. Se esperaban $%d y se obtuvieron $%d\n", i, valorDeposito,  valorEsperado, resultado);
            exit(EXIT_FAILURE);
        }
    }
    

    return 0;
}
