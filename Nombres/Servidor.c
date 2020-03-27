#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>

void Convertir_mayusculas(char nombre[20]){
	int i = 0;
	
	while (nombre[i] != '\0') 
	{
		if (nombre[i] >= 'a' && nombre[i] <= 'z') {
			nombre[i] = nombre[i] - 32;
		}
		i++;
	}
}
int Palindromo(char nombre[20]){
	
	int encontrado = 0;
	int i=0;

	while((i<strlen(nombre)/2) && encontrado == 0){
		if(nombre[i] != nombre[strlen(nombre)-1-i]){
			//No es un nombre Palindromo
			encontrado = 1;
		}
		i++;
	}
	if(encontrado == 1){
		return -1;
	}
	else{
		return 1;
	}
}
int main(int argc, char *argv[])
{
	int sock_conn, sock_listen, ret;
	struct sockaddr_in serv_adr;
	char buff[512];
	char buff2[512];
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
	// escucharemos en el port 9050
	serv_adr.sin_port = htons(9000);
	if (bind(sock_listen, (struct sockaddr *) &serv_adr, sizeof(serv_adr)) < 0)
		printf ("Error al bind");
	//La cola de peticiones pendientes no podr? ser superior a 4
	if (listen(sock_listen, 2) < 0)
		printf("Error en el Listen");
	
	int i;
	// Atenderemos solo 5 peticione
	for(i=0;i<7;i++){
		printf ("Escuchando\n");
		
		sock_conn = accept(sock_listen, NULL, NULL);
		printf ("He recibido conexi?n\n");
		//sock_conn es el socket que usaremos para este cliente
		
		// Ahora recibimos su nombre, que dejamos en buff
		ret=read(sock_conn,buff, sizeof(buff));
		printf ("Recibido\n");
		
		// Tenemos que a?adirle la marca de fin de string 
		// para que no escriba lo que hay despues en el buffer
		buff[ret]='\0';
		
		//Escribimos el nombre en la consola
		
		printf ("Se ha conectado: %s\n",buff);
		
		
		char *p = strtok( buff, "/");
		int codigo =  atoi (p);
		p = strtok( NULL, "/");
		char nombre[20];
		strcpy (nombre, p);
		printf ("Codigo: %d, Nombre: %s\n", codigo, nombre);
		
		if (codigo ==1) //piden la longitd del nombre
			sprintf (buff2,"%d,",strlen (nombre));
		else if(codigo == 2){
			// quieren saber si el nombre es bonito
			if((nombre[0]=='M') || (nombre[0]=='S'))
				strcpy (buff2,"SI,");
			else
				strcpy (buff2,"NO,");
		}
		else if(codigo == 3){
			//Palindromo
			//Primero lo pasamos todo a mayusculas
			Convertir_mayusculas(nombre);
			int resultado = Palindromo(nombre);
			sprintf(buff2,"%d",resultado);
		}
		else if(codigo == 4){
			//Nombre en mayusculas
			Convertir_mayusculas(nombre);
			strcpy(buff2,nombre);
		}
	
		printf ("%s\n", buff2);
		// Y lo enviamos
		write (sock_conn,buff2, strlen(buff2));
		
		// Se acabo el servicio para este cliente
		close(sock_conn); 
	}
}
