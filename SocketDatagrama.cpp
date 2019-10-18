#include "SocketDatagrama.h"

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


    int longitudForanea = sizeof(direccionForanea);

    std::cout << "Enviando a: " << serverIpAdress << std::endl;

    bzero((char *)&direccionForanea, longitudForanea);
    direccionForanea.sin_family = AF_INET;
    direccionForanea.sin_addr.s_addr = inet_addr(serverIpAdress);
    direccionForanea.sin_port = htons(serverPort);



    if ((fd = open("test.png",O_RDONLY))==-1){
        perror("open fail");
        return EXIT_FAILURE;
    }

    usleep(5000000);
    n=read(fd,buf,BUFFERT);
    while(n){
        if(n==-1){
            perror("read fails");
            return EXIT_FAILURE;
        }

        m=sendto(s,buf,n,0,(struct sockaddr*)&direccionForanea,longitudForanea);

        if(m==-1){
            perror("send error");
            return EXIT_FAILURE;
        }
        count+=m;
        //fprintf(stdout,"----\n%s\n----\n",buf);
        bzero(buf,BUFFERT);
        n=read(fd,buf,BUFFERT);
    }
  //read vient de retourner 0 : fin de fichier
  
  //pour debloquer le serv
    m=sendto(s,buf,0,0,(struct sockaddr*)&direccionForanea,longitudForanea);

    std::cout << "NUmero de octetos a trasnferir: " << count << std::endl;

    close(fd);

    return 0;
}

int SocketDatagrama::recibeImagen(const char *serverIpAdress, int serverPort){

    int fd;
    char buf[BUFFERT];
    char filename[256];
    bzero(filename,256);
    sprintf(filename,"test2.png");
    printf("Creando la imagen : %s\n",filename);

    if((fd=open(filename,O_CREAT|O_WRONLY|O_TRUNC,0600))==-1){
        perror("open fail");
        return EXIT_FAILURE;
    }

    char *ip = inet_ntoa(direccionForanea.sin_addr);

    std::cout << "Recibiendo de: " << ip << std::endl;

    off_t count=0, n;

    unsigned int l=sizeof(struct sockaddr_in);
    //preparation de l'envoie
    bzero(&buf,BUFFERT);
    n = recvfrom(s, &buf,BUFFERT, 0, (struct sockaddr *)&direccionForanea, &l);
    while(n){
        std::cout << n << "de datos recibidos: " << std::endl;
        if(n==-1){
            n = 0;
            perror("read fails");
            return EXIT_FAILURE;
            //n = recvfrom(s, &buf,BUFFERT, 0, (struct sockaddr *)&direccionForanea, &l);
        }
        count+=n;
        write(fd,buf,n);
        bzero(buf,BUFFERT);
        n=recvfrom(s,&buf,BUFFERT,0,(struct sockaddr *)&direccionForanea,&l);
    }

    std::cout << "NUmero de octetos a trasnferir: " << count << std::endl;

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