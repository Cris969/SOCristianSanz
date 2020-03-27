#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>

float Convertir_celcius(float fahrenheit){
	float celcius = ((fahrenheit-32)*5)/9;
	return celcius;
}
float Convertir_fahrenheit(float celcius){
	float fahrenheit = (celcius*1.8)+32;
	return fahrenheit;
}
int main(int argc, char *argv[])
{
	int sock_conn, sock_listen, ret;
	struct sockaddr_in serv_adr;
	char peticion[512];
	char respuesta[512];
	
	// INICIALIZACIONES
	//Abrimos el socket
	if ((sock_listen = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		printf("Error creando socket");
	// Hacemos el bind en el port
	memset(&serv_adr, 0, sizeof(serv_adr));// inicializa a cero serv_addr
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY); /* Lo mete en IP local */
	serv_adr.sin_port = htons(9000);
	if (bind(sock_listen, (struct sockaddr *) &serv_adr, sizeof(serv_adr)) < 0)
		printf("Error en el bind");
		// Limitamos el numero de conexiones pendientes
	if (listen(sock_listen, 10) < 0)
		printf("Error en el listen");
	for(;;){
		printf("Escuchando\n");
		sock_conn = accept(sock_listen, NULL, NULL);
		//sock_conn es el socket que utilizaremos para el cliente
		//Servicio
		//read hace 2 cosas , guarda lo que llega en el socket en el buffer peticion
		//y devuelve el numero de Bytes del mensaje , es decir su tamaÃ±o
		//el tamaño de un char es de 1 Byte , o lo que es lo mismo 8 bits
		ret=read(sock_conn,peticion, sizeof(peticion));
		printf("Vamos bien\n");
		//Añadimos el caracter de fin de string para que no escriba lo que hay despues en el buffer
		peticion[ret] = '\0';
		//Vamos a ver que quieren(analizar la peticion)
		char *p = strtok(peticion,"/");
		int codigo = atoi(p);
		switch(codigo){
			//Convertir a Celcius
			case 1:
				p = strtok(NULL,"/");
				int fahrenheit = atof(p);
				sprintf(respuesta,"%f",Convertir_celcius(fahrenheit));
				write(sock_conn,respuesta,strlen(respuesta));
				break;
			//Convertir a Fahrenheit
			case 2:
				p = strtok(NULL,"/");
				int celcius = atof(p);
				sprintf(respuesta,"%f",Convertir_fahrenheit(celcius));
				write(sock_conn,respuesta,strlen(respuesta));
				break;
			default:
				printf("Error en el codigo");
				break;
		}
		close(sock_conn); /* Necessari per a que el client detecti EOF */
	}	
}	
