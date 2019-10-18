#include "Solicitud.h"
#include <iostream>

using namespace std;

namespace std {
  template <typename _CharT, typename _Traits>
  inline basic_ostream<_CharT, _Traits> &
  tab(basic_ostream<_CharT, _Traits> &__os) {
    return __os.put(__os.widen('\t'));
  }
}

int main(int argc, char const *argv[]){

    //PC CENTRAL SERVIRA COMO CLIENTE UDP

    if (argc != 6){
        printf("Forma de ejecutar: ./%s [ID_RED] [SERVER PORT] [HOST INICIAL] [HOST FINAL] [CALIDAD]\n", argv[0]);
        exit(0);
    }

    // Creamos una solicitud
    Solicitud solicitud;

    char * id_inicial = (char*)argv[1];
    size_t size = strlen(id_inicial);
    char * id_red_clean = (char *) calloc(size-1,sizeof(char));
    strncpy(id_red_clean, id_inicial, size-1);

    register int i = 0;
    while(1){
        for (i = atoi(argv[3]); i <= atoi(argv[4]); ++i){
            //Empezamos con las iteraciones a las IP's
            char *id_host = (char *) calloc(size+3,sizeof(char));
            char *host = (char *) calloc(3,sizeof(char));
            sprintf(host, "%d", i);
            strncpy(id_host, id_red_clean, size+3);
            strcat(id_host, host);
            std::cout << "[ INFO ] " << std::tab << "Conectandose con direccion: " << id_host << std::endl;
            
            //Haremos una peticion para verificar si funciona como servidor la IP
            bool aviable = solicitud.ipDisponible(id_host, atoi(argv[2])); //Se pregunta si esta disponible
            if (aviable == true){
                std::cout << "[ SUCCESS ] " << std::tab  << "HOST DISPONIBLE " << std::endl;
                //Comenzamos a pedir la imagen
                solicitud.makeScreenshoot(id_host, atoi(argv[2]), atoi(argv[5]));
            }
            solicitud.cerrarSocket();
            free(id_red_clean);
            free(id_host);
            free(host);
        }
        usleep(atoi(argv[3]) * 1000000);
    }
    return 0;
}
