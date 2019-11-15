#include "SocketDatagrama.h"

using namespace std;

namespace std {
  template <typename _CharT, typename _Traits>
  inline basic_ostream<_CharT, _Traits> &
  tab(basic_ostream<_CharT, _Traits> &__os) {
    return __os.put(__os.widen('\t'));
  }
}


SocketDatagrama::SocketDatagrama(int puertoLocal)
{
    s = socket(AF_INET, SOCK_DGRAM, 0);

    int longitudLocal = sizeof(direccionLocal);

    bzero(&direccionLocal, longitudLocal);
    direccionLocal.sin_family = AF_INET;
    direccionLocal.sin_addr.s_addr = INADDR_ANY;
    direccionLocal.sin_port = htons(puertoLocal);

    bind(s, (sockaddr *)&direccionLocal, longitudLocal);
}

int SocketDatagrama::envia(PaqueteDatagrama &p)
{
    int longitudForanea = sizeof(direccionForanea);

    bzero((char *)&direccionForanea, longitudForanea);
    direccionForanea.sin_family = AF_INET;
    direccionForanea.sin_addr.s_addr = inet_addr(p.obtieneDireccion());
    direccionForanea.sin_port = htons(p.obtienePuerto());

    return sendto(s, p.obtieneDatos(), p.obtieneLongitud(), 0, (struct sockaddr *)&direccionForanea, longitudForanea);
}

int SocketDatagrama::recibe(PaqueteDatagrama &p)
{
    // Recibe datos.
    int longitudForanea = sizeof(direccionForanea);
    int recibidos = recvfrom(s, p.obtieneDatos(), p.obtieneLongitud(), 0, (struct sockaddr *)&direccionForanea, (socklen_t *)&longitudForanea);

    // Guarda direccción fuente.
    uint32_t direccionFuente = ntohl(direccionForanea.sin_addr.s_addr);
    char direccionFuenteCadena[16];
    sprintf(direccionFuenteCadena, "%u.%u.%u.%u",
            direccionFuente >> 24 & 0xff,
            direccionFuente >> 16 & 0xff,
            direccionFuente >> 8 & 0xff,
            direccionFuente & 0xff);
    p.inicializaIp(direccionFuenteCadena);

    // Guarda puerto fuente.
    in_port_t puertoFuente = ntohs(direccionForanea.sin_port);
    p.inicializaPuerto(puertoFuente);

    return recibidos;
}


int SocketDatagrama::enviaImagen(const char *serverIpAdress, int serverPort){

    int fd;
    char buf[BUFFERT];
    off_t count=0, m;//long
    bzero(&buf,BUFFERT);
    long int n;

    unsigned int longitudForanea = sizeof(direccionForanea);

    //std::cout << "[ INFO ] " << std::tab  << "Enviando imagen a: " << serverIpAdress << std::endl;

    bzero((char *)&direccionForanea, longitudForanea);
    direccionForanea.sin_family = AF_INET;
    direccionForanea.sin_addr.s_addr = inet_addr(serverIpAdress);
    direccionForanea.sin_port = htons(serverPort);

    if ((fd = open("screenshoot.png",O_RDONLY))==-1){
        perror("Error al abrir la imagen");
        return EXIT_FAILURE;
    }

    n = read(fd,buf,BUFFERT);

    char temp_buf [1];

    while(n){

        if(n==-1){
            perror("Error al leer la transmicion");
            return EXIT_FAILURE;
        }
        m = sendto(s,buf,n,0,(struct sockaddr*)&direccionForanea,longitudForanea);

        if( m == -1 ){
            perror("Error al enviar el archivo");
            return EXIT_FAILURE;
        }

        //Verificamos que el paquete le llego

        recvfrom(s,&temp_buf,1,0,(struct sockaddr *)&direccionForanea,&longitudForanea);

        if (temp_buf[0] == 't'){
            //Si se envio el paquete correctamente, se envia el sig pedazo
            count += m;
            bzero(buf,BUFFERT);
            n = read(fd,buf,BUFFERT);
        }else{
            std::cout << "[ FAIL ] " << std::tab  << "Reenviando pedazo: " << count << std::endl;
        }
        
    }
  //read vient de retourner 0 : fin de fichier
  
  //pour debloquer le serv
    m=sendto(s,buf,0,0,(struct sockaddr*)&direccionForanea,longitudForanea);

    std::cout << "[ SUCCESSS ] " << std::tab  << "Tamaño de imagen transferida: " << count << std::endl;

    close(fd);

    return 0;
}

int SocketDatagrama::recibeImagen(const char *serverIpAdress, int serverPort){

    int fd;
    char buf[BUFFERT];
    char filename[256];
    bzero(filename,256);
    sprintf(filename,"%s","www/images/");
    strcat(filename, serverIpAdress);

    char *extension = (char *) calloc(4,sizeof(char));
    sprintf(extension, ".png");
    strcat(filename, extension);

    std::cout << "[ SUCCESSS ] " << std::tab  << "Creando la imagen: " << filename << std::endl;


    if((fd=open(filename,O_CREAT|O_WRONLY|O_TRUNC,0600))==-1){
        perror("Error al abrir la imagen creada");
        return EXIT_FAILURE;
    }

    //char *ip = inet_ntoa(direccionForanea.sin_addr);

    //std::cout << "[ INFO ] " << std::tab  << "Recibiendo datos de: " << ip << std::endl;

    off_t count=0, n;

    unsigned int l=sizeof(struct sockaddr_in);
    //preparation de l'envoie
    bzero(&buf,BUFFERT);
    n = recvfrom(s, &buf,BUFFERT, 0, (struct sockaddr *)&direccionForanea, &l);

    char temp_buf [1];


    while(n){
        //std::cout << "[ INFO ] " << std::tab << n << " bits de datos recibidos " << " del pedazo " << count <<std::endl;
        if(n==-1){
            perror("Error al leer");
            temp_buf [0] = 'f';
            std::cout << "[ FAIL ] " << std::tab  << "Pidiendo pedazo: " << count << std::endl;
            //return -1;
        }else{
            temp_buf [0] = 't';
            count += n;
            write(fd,buf,n);
            bzero(buf,BUFFERT);
        }

        sendto(s,temp_buf,1,0,(struct sockaddr*)&direccionForanea,l);

        n = recvfrom(s,&buf,BUFFERT,0,(struct sockaddr *)&direccionForanea,&l);
    }

    std::cout << "[ SUCCESSS ] " << std::tab  << "Tamaño de imagen recibido: " << count << std::endl;

    free(extension);

    return 1;
}

int SocketDatagrama::recibeTimeout(PaqueteDatagrama &p, time_t segundos, suseconds_t microsegundos)
{
    timeout.tv_sec = segundos;
    timeout.tv_usec = microsegundos;
    setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));
    //---------------
    // Recibe datos.
    int longitudForanea = sizeof(direccionForanea);
    int recibidos = recvfrom(s, p.obtieneDatos(), p.obtieneLongitud(), 0, (struct sockaddr *)&direccionForanea, (socklen_t *)&longitudForanea);

    // Guarda direccción fuente.
    uint32_t direccionFuente = ntohl(direccionForanea.sin_addr.s_addr);
    char direccionFuenteCadena[16];
    sprintf(direccionFuenteCadena, "%u.%u.%u.%u",
            direccionFuente >> 24 & 0xff,
            direccionFuente >> 16 & 0xff,
            direccionFuente >> 8 & 0xff,
            direccionFuente & 0xff);
    p.inicializaIp(direccionFuenteCadena);

    // Guarda puerto fuente.
    in_port_t puertoFuente = ntohs(direccionForanea.sin_port);
    p.inicializaPuerto(puertoFuente);
    //--------------
    if (recibidos < 0)
    {
        if (errno == EWOULDBLOCK)
        {
            fprintf(stderr, "Tiempo de recepción transcurrido\n");
        }
        else
        {
            fprintf(stderr, "Error en recvfrom\n");
        }
    }
    return recibidos;
}

SocketDatagrama::~SocketDatagrama()
{
    close(s);
}