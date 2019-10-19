#include "Respuesta.h"

using namespace std;

namespace std {
  template <typename _CharT, typename _Traits>
  inline basic_ostream<_CharT, _Traits> &
  tab(basic_ostream<_CharT, _Traits> &__os) {
    return __os.put(__os.widen('\t'));
  }
}


int main(int argc, char const *argv[]){
    if (argc != 2){
        printf("Forma de ejecutar: ./%s  [LISTENING PORT]\n", argv[0]);
    }

    while(true){
        // Preparamos respuesta.
        Respuesta respuesta(atoi(argv[1]));
        // Recibe mensaje.
        mensaje *solicitud = respuesta.getRequest();
        //std::cout << "[ SUCCESS ] " << std::tab << "Mensaje recibido " <<std::endl;
        // Valida mensaje.
        if (solicitud->messageType != REQUEST){
            std::cout << "[ ERROR ] " << std::tab << "El mensaje recibido no es una peticion " <<std::endl;
            exit(EXIT_FAILURE);
        }

        char *resultado = (char *) calloc(3,sizeof(char));
        char *CMD = (char *) calloc(30,sizeof(char));
        if (respuesta.newRequest){
            switch (solicitud->operationID){
                case AVAIABLE:{
                    sprintf(resultado, "%s", "OK");
                    //std::cout << "[ INFO ] " << std::tab << "Enviando mensaje: " << resultado <<std::endl;
                    break;
                }
                case CAPTURA:
                {   
                    std::cout << "[ INFO ] " << std::tab << "Tomar captura con calidad: " << solicitud->args <<std::endl;
                    sprintf(CMD, "scrot screenshoot.png -q ");
                    strcat(CMD, solicitud->args);
                    system(CMD);
                    break;
                }
            }

        }else{
            std::cout << "[ INFO ] " << std::tab << "Reenviando resultado: " << resultado <<std::endl;
        }
        // Enviamos resultado.
        if (solicitud->operationID == CAPTURA){
            respuesta.sendScreenshoot();
        }else{
            respuesta.sendReply(resultado, sizeof(resultado));
        }
        
        respuesta.cerrarSocket();
        free(resultado);
        free(CMD);

    }
    return 0;
}
