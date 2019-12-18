#ifndef _FUNCTIONS_H
#define _FUNCTIONS_H

int openInetConnection(char *Host_Servidor, char *Servicio);
int acceptClientConnection(int Descriptor);
int openInetSocket(char *Servicio);

int writeSocket(int socket, char *chain, int length);
int readSocket(int socket, char *chain, int length);

int aleatorio(int min, int max);

#endif