#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "definitions.h"
#include "functions.h"

//Prototipo
char* initial();

int main() {
	int serverSocket;
	char cadena[LENGTH];
	char message[LENGTH];
	char* token;
	int count;
	int fightStatus;
	int heroMod;
	int choice;

	//Inicializo la semilla Random
	srand(time(NULL));

	// Conexion con el servidor
	serverSocket = openInetConnection(HOST, SERVICE);
	if(serverSocket == 1) {
		printf ("No puedo establecer conexion con el servidor\n");
		exit(-1);
	}

	//Escojo la clase y se la envio al servidor
	sprintf(cadena, initial());
	writeSocket(serverSocket, cadena, strlen(cadena) + 1);
	
	do {
		readSocket(serverSocket, cadena, LENGTH);
		token = NULL;
		token = strtok(cadena, "|");
		count = 0;
		while(token != NULL) {
			switch(count) {
				case 0:
					fightStatus = atoi(token);
					break;
				case 1:
					sprintf(message, token);
					break;
			}
			token = strtok(NULL, "|");
			count++;
		}
		printf("%s", message);
		if(fightStatus == STATUS_CONTINUE) {
			scanf("%d", &choice);
			while(choice < 1 || choice > 3) {
				printf("Eleccion invalida.\nEscoge tu accion:\n");
				printf("1.Atacar 2.Defender 3.Curar\n");
				scanf("%d", &choice);	
			}
			heroMod = aleatorio(-2,2);
			sprintf(cadena, "%d|%d", choice, heroMod);
			writeSocket(serverSocket, cadena, strlen(cadena) + 1);
		}
	//Imprime el status de la pelea
	//printf("Status: %d\n", fightStatus);
	} while(fightStatus == STATUS_CONTINUE);

	//Cierro conexion y programa
	close (serverSocket);
	printf("Conexion cerrada.\n");
	return 0;
}

char* initial() {
	char class[10];
	static char chain[10];
	int choice;
	printf("Escoge tu clase:\n");
	printf("1.Paladin\n2.Rogue\n3.Warrior\n");
	scanf("%d", &choice);

	do {
        switch(choice) {
            case(1):
                sprintf(class, "PALADIN");
                break;
            case(2):
                sprintf(class, "ROGUE");
                break;
            case(3):
                sprintf(class, "WARRIOR");
                break;
            default:
                printf("Eleccion invalida.\n");
                printf("Escoge tu clase:\n");
                printf("1.Paladin\n2.Rogue\n3.Warrior\n");
                scanf("%d", &choice);
                break;
        }
	} while(class == NULL);
	sprintf(chain, "%d|%s", choice, class);
	return chain;
}
