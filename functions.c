#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

//Conecta con un servidor remoto a traves de socket INET
int openInetConnection(char *Host_Servidor, char *Servicio) {
	struct sockaddr_in Direccion;
	struct servent *Puerto;
	struct hostent *Host;
	int Descriptor;

	Puerto = getservbyname (Servicio, "tcp");
	if (Puerto == NULL) {
		return -1;
	}

	Host = gethostbyname (Host_Servidor);
	if (Host == NULL) {
		return -1;
	}

	Direccion.sin_family = AF_INET;
	Direccion.sin_addr.s_addr = ((struct in_addr *)(Host->h_addr))->s_addr;
	Direccion.sin_port = Puerto->s_port;
	
	Descriptor = socket (AF_INET, SOCK_STREAM, 0);
	if (Descriptor == -1) {
		return -1;
	}

	if (connect(Descriptor, (struct sockaddr *)&Direccion,sizeof (Direccion)) == -1) {
		return -1;
	}

	return Descriptor;
}

//Se le pasa un socket de servidor y acepta en el una conexion de cliente
int acceptClientConnection(int Descriptor) {
	socklen_t Longitud_Cliente;
	struct sockaddr Cliente;
	int Hijo;

	Longitud_Cliente = sizeof(Cliente);
	Hijo = accept(Descriptor, &Cliente, &Longitud_Cliente);
	if(Hijo == -1) {
		return -1;
	}

	return Hijo;
}

//Abre un socket servidor de tipo AF_INET
int openInetSocket(char *Servicio) {
	struct sockaddr_in Direccion;
	struct sockaddr Cliente;
	socklen_t Longitud_Cliente;
	struct servent *Puerto;
	int Descriptor;

	//Se abre el socket
	Descriptor = socket(AF_INET, SOCK_STREAM, 0);
	if(Descriptor == -1) {
		return -1;
	}

	//Se obtiene el servicio del fichero /etc/services
	Puerto = getservbyname (Servicio, "tcp");
	if(Puerto == NULL) {
		return -1;
	}

	Direccion.sin_family = AF_INET;
	Direccion.sin_port = Puerto->s_port;
	Direccion.sin_addr.s_addr =INADDR_ANY;
	if(bind(Descriptor, (struct sockaddr *)&Direccion, sizeof (Direccion)) == -1) {
		close (Descriptor);
		return -1;
	}

	//Se avisa al sistema que comience a atender llamadas de clientes
	if(listen(Descriptor, 1) == -1)	{
		close (Descriptor);
		return -1;
	}

	return Descriptor;
}

int writeSocket(int socket, char *chain, int length) {
	int written = 0;
	int aux = 0;

	//Parametros correctos
	if((socket == -1) || (chain == NULL) || (length < 1)) {
		return -1;
	}

	//Escribo la cadena
	while (written < length) {
		aux = write(socket, chain + written, length - written);
		if (aux > 0) {
			written += aux;
		} else {
			if(aux == 0) {
				return written;
			} else {
				return -1;
			}
		}
	}

	return written;
}

int readSocket(int socket, char *chain, int length) {
	int redd = 0;
	int aux = 0;

	//Parametros correctos
	if((socket == -1) || (chain == NULL) || (length < 1)) {
		return -1;
	}

	//Leo la cadena
	while(redd < length) {
		aux = read(socket, chain + redd, length - redd);
		if(aux > 0) {
			redd += aux;
		} else {
			if(aux == 0) { 
				return redd;
			}
			if(aux == -1) {
				//Tipo de error
				switch(errno) {
					case EINTR:
					case EAGAIN:
						printf("ERROR\n");
						usleep (100);
						break;
					default:
						return -1;
				}
			}
		}
	}

	return redd;
}

int aleatorio(int min, int max) {
	int resultado;
	resultado = rand() % (max - min + 1) + min;
	return resultado;
}