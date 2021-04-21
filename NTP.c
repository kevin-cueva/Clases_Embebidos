#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

unsigned long long NTP = 2208988800ull; //marca de tiempo NTP 1 Jan 1970o, dicho de otra manera, 2,208,988,800 unsigned long longsegundos.

//SOCK_STREAM -> TCP
//SOCK_DGRAM -> UDP


int main(){
    int conection = 0;  //conectivilidad
    int socket_id = 0; // Identificador de socket devuelto por la función socket()
    int full = 0;     //envio y resepcion de datos 

  //socket(int dominio, int tipo, int protocolo)
    socket_id = socket(PF_INET, SOCK_DGRAM, 0);
        
      typedef struct //Estructura 
  { 

    uint8_t li_vn_mode;      // Eight bits. li, vn, and mode.
                             // li.   Two bits.   Leap indicator.
                             // vn.   Three bits. Version number of the protocol.
                             // mode. Three bits. Client will pick mode 3 for client.

    uint8_t stratum;         // Ocho bits. Nivel de estrato del reloj local.
    uint8_t poll;            // Ocho bits. Intervalo máximo entre mensajes sucesivos.
    uint8_t precision;       // Eight bits. Precision of the local clock.

    uint32_t rootDelay;      // 32 bits. Tiempo total de retardo de ida y vuelta
    uint32_t rootDispersion; // 32 bits. Error máximo en voz alta de la fuente de reloj principal.
    uint32_t refId;          // 32 bits. Reference clock identifier.

    uint32_t refTm_s;        // 32 bits. Reference time-stamp seconds.
    uint32_t refTm_f;        // 32 bits. Reference time-stamp fraction of a second.

    uint32_t origTm_s;       // 32 bits. Originate time-stamp seconds.
    uint32_t origTm_f;       // 32 bits. Originate time-stamp fraction of a second.

    uint32_t rxTm_s;         // 32 bits. Received time-stamp seconds.
    uint32_t rxTm_f;         // 32 bits. Received time-stamp fraction of a second.

    uint32_t txTm_s;         // 32 bits and the most important field the client cares about. Transmit time-stamp seconds.
    uint32_t txTm_f;         // 32 bits. Transmit time-stamp fraction of a second.

  } ntp_packet;              // Total: 384 bits or 48 bytes.

  ntp_packet packet = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; //Limpiamos la estructura
  memset( &packet, 0, sizeof( ntp_packet ) );       //Reservamos memoria

  //Establezca los bits del primer byte en 00,011,011 para li = 0, vn = 3 y mode = 3. El resto se dejará en cero.
  *( ( char * ) &packet + 0 ) = 0x1b; // Represents 27 in base 10 or 00011011 in base 2.

    /* La estructura TCP/IP es la sockaddr_in, equivalente a la estructura sockaddr, pero que permite
     referenciar a sus elementos de forma más sencilla.*/
    
    struct sockaddr_in server; //nombre de structura

    server.sin_family = AF_INET; //Dominio donde se realiza la conexion
    server.sin_addr.s_addr = inet_addr("162.159.200.1");
    server.sin_port =  htons(123); //el puerto de internet

/*connect () se usa para conectarse a una dirección de [servidor] remota
                        identificador, puntero a una estructura sockaddr,  tamaño de la estructura*/
    conection = connect(socket_id, (struct sockaddr*)&server, sizeof(server)); 
     if(conection < 0){
        printf("Error al realizar la conexión\r\n");
        return(-1);
    }

     printf("Conexión realizada con éxito\r\n");
          

    full = write( socket_id, ( char* ) &packet, sizeof( ntp_packet ) ); // envia o escribe al servidor

     if ( full < 0 )   // Si hay algun error marca full = -1
        perror( "ERROR al escribir al servidor" );
   
   full = read( socket_id, ( char* ) &packet, sizeof( ntp_packet ) );  // escucha o lee

    if ( full < 0 )   // envia error en caso de que no lea nada full=-1
        perror( "ERROR al leer  el servidor" );

    packet.txTm_s = ntohl( packet.txTm_s ); 
    packet.txTm_f = ntohl( packet.txTm_f ); 
 
    time_t txTm = ( time_t ) ( packet.txTm_s - NTP );

    printf( "Time: %s", ctime( ( const time_t* ) &txTm ) );
    return(0);  
}