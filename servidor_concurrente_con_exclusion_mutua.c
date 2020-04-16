#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <pthread.h>

int contador;

//Estructura necesaria para acceso excluyente
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct{
	//Nombre del usuario
	char nombre[20];
	//Socket asignado al usuario
	int socket;
	
}Tusuario;

typedef struct{
	//Lista de usuarios conectados en el servidor
	Tusuario conectados[50];
	//Numero de usuarios conectados
	int num;
	
}Tlistaconectados;

//Variables globales
	Tlistaconectados lista;
	
//Eliminar jugador de la lista de conectados
int eliminar_jugador(int socket,Tlistaconectados *lista){
	
	int resultado = 0;
	int i = 0;
		
	//Busqueda
	while(i<lista->num && resultado == 0){
		if(socket == lista->conectados[i].socket){
			//Implica que el jugador esta conectado
			resultado = 1;
		}
		else{
			i++;
		}
	}
	if(resultado != 0){
		for(i;i<lista->num-1;i++){
			strcpy(lista->conectados[i].nombre,lista->conectados[i+1].nombre);
			lista->conectados[i].socket = lista->conectados[i+1].socket;
		}
		lista->num--;
		//Jugador eliminado, devuelve 1
		return resultado;
	}
	else{
		//Jugador no conectado o que no existe, por tanto no se puede eliminar, devuelve 0
		return resultado;
	}
}
//Añadir el nombre del usuario conectado
int add_nombre(char nombre[20],int socket,Tlistaconectados * lista){
	
	int resultado = 0;
	int i = 0;
	
	while((i<lista->num) && (resultado == 0)){
		if(socket == lista->conectados[i].socket){
			resultado = 1;
			strcpy(lista->conectados[i].nombre,nombre);
		}
		else{
			i++;
		}
	}
	return resultado;
	
}
void *AtenderCliente (void *socket)
{
	int sock_conn;
	int *s;
	s= (int *) socket;
	sock_conn= *s;
	
	//int socket_conn = * (int *) socket;
	
	char peticion[512];
	char respuesta[512];
	int ret;
	
	
	int terminar =0;
	// Entramos en un bucle para atender todas las peticiones de este cliente
	//hasta que se desconecte
	while (terminar ==0)
	{
		// Ahora recibimos la petici?n
		ret=read(sock_conn,peticion, sizeof(peticion));
		printf ("Recibido\n");
		
		// Tenemos que a?adirle la marca de fin de string 
		// para que no escriba lo que hay despues en el buffer
		peticion[ret]='\0';
		
		
		printf ("Peticion: %s\n",peticion);
		
		int resultado;
		// vamos a ver que quieren
		char *p = strtok( peticion, "/");
		int codigo =  atoi (p);
		// Ya tenemos el c?digo de la petici?n
		char nombre[20];
		
		if ((codigo !=0)&&(codigo!=4))
		{
			p = strtok( NULL, "/");
			
			strcpy (nombre, p);
			// Ya tenemos el nombre
			printf ("Codigo: %d, Nombre: %s\n", codigo, nombre);
			resultado = add_nombre(nombre,sock_conn,&lista);
			if(resultado == 1){
				printf("Nombre añadido\n");
			}
			else{
				printf("Error al añadir nombre");
			}
		}
		if (codigo ==0){ //petici?n de desconexi?n
			pthread_mutex_lock( &mutex ); //No me interrumpas ahora
			resultado = eliminar_jugador(sock_conn,&lista);
		    if(resultado !=0){
				printf("Se ha desconectado un Usuario\n");
				terminar=1;
			}
			else{
				printf("Error\n");
			}
			pthread_mutex_unlock( &mutex); //ya puedes interrumpirme
		}
		else if (codigo ==4)
			sprintf (respuesta,"%d",contador);
		else if (codigo ==1) //piden la longitd del nombre
			sprintf (respuesta,"%d",strlen (nombre));
		else if (codigo ==2)
			// quieren saber si el nombre es bonito
			if((nombre[0]=='M') || (nombre[0]=='S'))
			strcpy (respuesta,"SI");
			else
				strcpy (respuesta,"NO");
			else //quiere saber si es alto
			{
				p = strtok( NULL, "/");
				float altura =  atof (p);
				if (altura > 1.70)
					sprintf (respuesta, "%s: eres alto",nombre);
				else
					sprintf (respuesta, "%s: eresbajo",nombre);
			}
			
			if (codigo !=0)
			{
				
				printf ("Respuesta: %s\n", respuesta);
				// Enviamos respuesta
				write (sock_conn,respuesta, strlen(respuesta));
			}
			if ((codigo ==1)||(codigo==2)|| (codigo==3))
			{
				pthread_mutex_lock( &mutex ); //No me interrumpas ahora
				contador = contador +1;
				pthread_mutex_unlock( &mutex); //ya puedes interrumpirme
			}
			
	}
	// Se acabo el servicio para este cliente
	close(sock_conn); 
	
}


int main(int argc, char *argv[])
{
	
	int sock_conn, sock_listen;
	struct sockaddr_in serv_adr;
	
	// INICIALITZACIONS
	// Obrim el socket
	if ((sock_listen = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		printf("Error creant socket");
	// Fem el bind al port
	
	
	memset(&serv_adr, 0, sizeof(serv_adr));// inicialitza a zero serv_addr
	serv_adr.sin_family = AF_INET;
	
	// asocia el socket a cualquiera de las IP de la m?quina. 
	//htonl formatea el numero que recibe al formato necesario
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	// establecemos el puerto de escucha
	serv_adr.sin_port = htons(9301);
	if (bind(sock_listen, (struct sockaddr *) &serv_adr, sizeof(serv_adr)) < 0)
		printf ("Error al bind");
	
	if (listen(sock_listen, 3) < 0)
		printf("Error en el Listen");
	
	contador =0;
	int i;
	//int sockets[100];
	pthread_t thread;
	// Bucle para atender a 50 clientes
	i = 0;
	for (;;){
		printf ("Escuchando\n");
		
		sock_conn = accept(sock_listen, NULL, NULL);
		printf ("He recibido conexion\n");
		
		//sock_conn es el socket que usaremos para este cliente
		lista.conectados[i].socket =sock_conn;
		lista.num++;
		printf("Se ha conectado un Usuario\n");
		
		// Crear thead y decirle lo que tiene que hacer
		
		pthread_create (&thread, NULL, AtenderCliente,&lista.conectados[i].socket);
		i=i+1;
		
	}
	
	//for (i=0; i<5; i++)
	//pthread_join (thread[i], NULL);
	
	
	
}
