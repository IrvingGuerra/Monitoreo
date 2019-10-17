#include "Respuesta.h"

int main(int argc, char const *argv[]){
    if (argc != 2){
        printf("Forma de ejecutar: ./%s  [LISTENING PORT]\n", argv[0]);
    }

    Respuesta respuesta(atoi(argv[1]));
    // Preparamos respuesta.

/*
    while(true){
        // Recibe mensaje.
        mensaje *solicitud = respuesta.getRequest();
        printf("Mensaje recibido.\n");
        // Valida mensaje.
        if (solicitud->messageType != REQUEST)
        {
            perror("El mensaje recibido no es una petición.");
            exit(EXIT_FAILURE);
        }

        char *resultado = (char *) calloc(2,sizeof(char));
        if (respuesta.newRequest){
            switch (solicitud->operationID){
                case AVAIABLE:{
                    sprintf(resultado, "%s", "OK");
                    break;
                }
                case CAPTURA:
                {
                    
                    break;
                }
            }
            printf("Enviando resultado: %s.\n", resultado);
        }else{
            printf("Reenviando resultado: %s\n", resultado);
        }
        // Enviamos resultado.
        respuesta.sendReply(resultado, sizeof(resultado));

    }

*/
    
    int resultado;
    int nbd = 0;

    while (true){
        // Recibe mensaje.
        mensaje *solicitud = respuesta.getRequest();
        printf("Mensaje recibido.\n");
        // Valida mensaje.
        if (solicitud->messageType != REQUEST)
        {
            perror("El mensaje recibido no es una petición.");
            exit(EXIT_FAILURE);
        }

        if (respuesta.newRequest)
        {
            switch (solicitud->operationID)
            {
            case ADD:
            {
                resultado = ((int *)solicitud->args)[0] + ((int *)solicitud->args)[1];
                break;
            }
            case SUB:
            {
                resultado = ((int *)solicitud->args)[0] - ((int *)solicitud->args)[1];
                break;
            }
            case DEPOSITO:
                nbd += *(int *)solicitud->args;
                resultado = nbd;
            }
            printf("Enviando resultado: %d.\n", resultado);
        } else {
            printf("Reenviando resultado: %d\n", resultado);
        }

        // Enviamos resultado.
        respuesta.sendReply((char *)&resultado, sizeof(resultado));
    }
    

    return 0;
}
