#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "definitions.h"
#include "functions.h"

typedef struct character {
	char* name[20];
	int maxHealthPoints;
	int currentHealthPoints;
	int attackPower;
	int defensePower;
	int healingPower;
	int speed;
} statList;

int main(int argc, char *argv[]) {
	int i;
	int serverSocket;
	int clientSocket;
	char cadena[LENGTH];
	char message[LENGTH];
	char *token;
	int counter;
	int player;
	int enemy;
	int playerAction;
	int playerMod;
	int enemyAction;
	int enemyMod;
	int damage;
	int healing;
	int health;
	int randomStats;
	int fightStatus = STATUS_CONTINUE;
	FILE *entitiesData = NULL;
	statList entities[ENTITIES];

	//Verifico que la cantidad de parametros sea la correcta
	if(argc > 1) {
		randomStats = atoi(argv[1]);
		if(randomStats != 0 && randomStats != 1) {
			printf("Error en la modalidad de juego.\n");
			exit(-1);
		}
	} else {
		printf("Error en el numero de parametros.\n");
		exit(-1);
	}

	//Inicio la semilla random
	srand(time(NULL));

	//Abro el servidor
	serverSocket = openInetSocket(SERVICE);
	if(serverSocket == -1) {
		printf("No se puede abrir socket servidor\n");
		exit(-1);
	}

	//Espero y conecto al cliente
	clientSocket = acceptClientConnection(serverSocket);
	if(serverSocket == -1) {
		printf("No se puede abrir socket de cliente\n");
		exit(-1);
	}

	//Leo la clase
	readSocket(clientSocket, cadena, LENGTH);

	token = NULL;
	counter = 0;
	token = strtok(cadena, "|");
	while(token != NULL){
		switch(counter) {
			case 0:
				player = atoi(token);
				break;
			case 1:
				sprintf(message, token);
				break;
		}
		token = strtok(NULL, "|");
		counter++;
	}

	printf("clase(%d):%s\n", player, message);
	sprintf(message,"");

	//Cargo las stats para poder inicializar
	entitiesData = fopen("entitiesStats.dat", "r");
	if(randomStats == 0) {
		//Stats base
		printf("Stats base.\n");
		for(i = 0; i < ENTITIES; i++) {
			fscanf(entitiesData, "%s %d %d %d %d %d %d\n", entities[i].name, &entities[i].maxHealthPoints, &entities[i].currentHealthPoints, 
				&entities[i].attackPower, &entities[i].defensePower, &entities[i].healingPower, &entities[i].speed);
		}
	} else {
		//Stats randomizadas
		printf("Stats randomizadas.\n");
		for(i = 0; i <  ENTITIES; i++) {
			fscanf(entitiesData, "%s %d %d %d %d %d %d\n", entities[i].name, &entities[i].maxHealthPoints, &entities[i].currentHealthPoints, 
				&entities[i].attackPower, &entities[i].defensePower, &entities[i].healingPower, &entities[i].speed);
			//Asigno los modificadores random
			entities[i].maxHealthPoints += aleatorio(5,10);
			entities[i].currentHealthPoints = entities[i].maxHealthPoints;
			entities[i].attackPower += aleatorio(-3,3);
			entities[i].defensePower += aleatorio(-3,3);
			entities[i].healingPower += aleatorio(-3,3);
			entities[i].speed += aleatorio(-3,3);
		}
	}
	
	fclose(entitiesData);
	
	//Randomizo el enemigo
	enemy = aleatorio(CHAR_CAPYBARA, CHAR_TATU);
	printf("Enemigo:%s\n", entities[enemy].name);
	printf("Player Stats: HP:%d ATK:%d, DEF:%d HEL:%d SPD:%d\n", entities[player].maxHealthPoints, entities[player].attackPower, entities[player].defensePower,
		entities[player].healingPower, entities[enemy].speed);
	printf("Enemy Stats: HP:%d ATK:%d, DEF:%d SPD:%d\n", entities[enemy].maxHealthPoints, entities[enemy].attackPower, entities[enemy].defensePower,
		entities[enemy].speed);
	//GAMELOGIC
	do {
		sprintf(cadena, "%d|%sEstas peleando contra un %s! Tiene %d de vida.\nEres un %s con %d/%d de vida. Que deseas hacer?\n1.Atacar 2.Defender 3.Curar\n", fightStatus, message, entities[enemy].name, entities[enemy].currentHealthPoints, entities[player].name, entities[player].currentHealthPoints, entities[player].maxHealthPoints);
		writeSocket(clientSocket, cadena, strlen(cadena) + 1);

		//Leo la accion que manda el cliente (En conjunto con el modificador)
		readSocket(clientSocket, cadena, LENGTH);
		token = NULL;
		counter = 0;
		token = strtok(cadena, "|");
		while(token != NULL) {
			switch(counter) {
				case 0:
					playerAction = atoi(token);
					break;
				case 1:
					playerMod = atoi(token);
					break;
			}
			token = strtok(NULL, "|");
			counter++;
		}

		//Randomizo la accion del enemigo
		enemyAction = aleatorio(ACTION_ATTACK, ACTION_DEFEND);
		enemyMod = aleatorio(-1, 1);

		//Mensaje de consola
		printf("Player: Act:%d, Mod:%d \tEnemy: Act:%d, Mod:%d\n", playerAction, playerMod, enemyAction, enemyMod);
		//RESUELVO EL COMBATE
		if(entities[player].speed >= entities[enemy].speed) {
			switch(playerAction) {
				case ACTION_ATTACK:
					if(enemyAction == ACTION_DEFEND) {
						damage = (entities[player].attackPower + playerMod) - (entities[enemy].defensePower + enemyMod);
						//El damage no puede ser negativo si no, estaria curando al enemigo
						if(damage < 0) {
							damage = 0;
						}
						sprintf(message, "Has atacado al %s, pero se ha defendido! Realizaste %d de damage.\n", entities[enemy].name, damage);
						//Mensaje de consola
						printf("El enemigo recibe %d de damage.\n", damage);
						entities[enemy].currentHealthPoints -= damage;
						//Compruebo si el enemigo murio
						if(entities[enemy].currentHealthPoints <= 0) {
							//Mensaje de consola
							printf("Enemigo muerto.\n");
							fightStatus = STATUS_WIN;
						}
					} else {
						//Ambos atacan
						damage = entities[player].attackPower + playerMod;
						//El damage no puede ser negativo si no, estaria curando al enemigo
						if(damage < 0) {
							damage = 0;
						}
						entities[enemy].currentHealthPoints -= damage;
						//Mensaje de consola
						printf("El enemigo recibe %d de damage.\n", damage);
						sprintf(message, "Has realizado %d de damage.\n", damage);
						//Compruebo si el enemigo murio
						if(entities[enemy].currentHealthPoints <= 0) {
							//Mensaje de consola
							printf("Enemigo muerto.\n");
							fightStatus = STATUS_WIN;
						} else {
							//El enemigo no murio, ataca
							damage = entities[enemy].attackPower + enemyMod;
							//El damage no puede ser negativo si no, estaria curando al enemigo
							if(damage < 0) {
								damage = 0;
							}
							printf("El player recibe %d de damage.\n", damage);
							sprintf(message, "%sEl %s te ha realizado %d de damage.\n", message, entities[enemy].name, damage);
							entities[player].currentHealthPoints -= damage;
							//Compruebo si el player murio
							if(entities[player].currentHealthPoints <= 0) {
								//Mensaje de consola
								printf("player muerto.\n");
								fightStatus = STATUS_LOSE;
							}
						}
					}
					break;
				case ACTION_DEFEND:
					//Veo si el enemigo ataca
					if(enemyAction == ACTION_ATTACK) {
						damage = (entities[enemy].attackPower + enemyMod) - (entities[player].defensePower + playerMod);
						//El damage no puede ser negativo si no, estaria curando
						if(damage < 0) {
							damage = 0;
						}
						sprintf(message, "El %s te ha atacado, pero te has defendido! Recibes %d de damage.\n", entities[enemy].name, damage);
						//Mensaje de consola
						printf("El player recibe %d de damage.\n", damage);
						entities[player].currentHealthPoints -= damage;
						//Compruebo si el player murio
						if(entities[player].currentHealthPoints <= 0) {
							//Mensaje de consola
							printf("player muerto.\n");
							fightStatus = STATUS_LOSE;
						}
					} else {
						sprintf(message, "Ambos se han defendido. No pasa nada.\n");
					}
					break;
				case ACTION_HEAL:
					//Curo al personaje
					healing = entities[player].healingPower + playerMod;
					//messages de consola
					printf("El player se curo por %d puntos.\n", healing);
					sprintf(message, "Te has curado por %d puntos.\n", healing);
					entities[player].currentHealthPoints += healing;
					//No puede tener mas vida que la maxima
					if(entities[player].currentHealthPoints > entities[player].maxHealthPoints) {
						entities[player].currentHealthPoints = entities[player].maxHealthPoints;
					}
					//Verifico si el enemigo ataca (Si no lo hace, no hago nada)
					if(enemyAction == ACTION_ATTACK) {
						damage = entities[enemy].attackPower + enemyMod;
						//El damage no puede ser negativo si no, estaria curando al enemigo
						if(damage < 0) {
							damage = 0;
						}
						printf("El player recibe %d de damage.\n", damage);
						sprintf(message, "%sEl %s te ha realizado %d de damage.\n", message, entities[enemy].name, damage);
						entities[player].currentHealthPoints -= damage;
						//Compruebo si el player murio
						if(entities[player].currentHealthPoints <= 0) {
							//Mensaje de consola
							printf("player muerto.\n");
							fightStatus = STATUS_LOSE;
						}
					} else {
						sprintf(message, "%sEl %s se ha defendido.\n", message, entities[enemy].name);
					}
					break;
			}
		//El enemigo es mas veloz
		} else {
			switch(enemyAction) {
				case ACTION_ATTACK:
					if(playerAction == ACTION_DEFEND) {
						damage = (entities[enemy].attackPower + enemyMod) - (entities[player].defensePower + playerMod);
						//El damage no puede ser negativo si no, estaria curando
						if(damage < 0) {
							damage = 0;
						}
						sprintf(message, "El %s te ha atacado, pero te has defendido! Recibes %d de damage.\n", entities[enemy].name, damage);
						//Mensaje de consola
						printf("El player recibe %d de damage.\n", damage);
						entities[player].currentHealthPoints -= damage;
						//Compruebo si el player murio
						if(entities[player].currentHealthPoints <= 0) {
							//Mensaje de consola
							printf("player muerto.\n");
							fightStatus = STATUS_LOSE;
						}
					} else {
						//Me cure o ataque, primero ataca el
						damage = entities[enemy].attackPower + enemyMod;
						//El damage no puede ser negativo si no, estaria curando al enemigo
						if(damage < 0) {
							damage = 0;
						}
						printf("El player recibe %d de damage.\n", damage);
						entities[player].currentHealthPoints -= damage;
						sprintf(message, "El %s te ha realizado %d de damage.\n", entities[enemy].name, damage);
						//Compruebo si el player murio
						if(entities[player].currentHealthPoints <= 0) {
							//Mensaje de consola
							printf("player muerto.\n");
							fightStatus = STATUS_LOSE;
						} else if(playerAction == ACTION_ATTACK) {
							damage = entities[player].attackPower + playerMod;
							entities[enemy].currentHealthPoints -= damage;
							//Mensaje de consola
							printf("El enemigo recibe %d de damage.\n", damage);
							sprintf(message, "%sHas realizado %d de damage.\n", message, damage);
							//Compruebo si el enemigo murio
							if(entities[enemy].currentHealthPoints <= 0) {
								//Mensaje de consola
								printf("Enemigo muerto.\n");
								fightStatus = STATUS_WIN;
							}
						} else {
							//Curo al personaje
							healing = entities[player].healingPower + playerMod;
							entities[player].currentHealthPoints += healing;
							sprintf(message, "%sTe has curado por %d puntos.\n", message, healing);
							//No puede tener mas vida que la maxima
							if(entities[player].currentHealthPoints > entities[player].maxHealthPoints) {
								entities[player].currentHealthPoints = entities[player].maxHealthPoints;
							}
						}
					}
					break;
				case ACTION_DEFEND:
					//Veo si ataca el personaje
					if(playerAction == ACTION_ATTACK){
						damage = (entities[player].attackPower + playerMod) - (entities[enemy].defensePower + enemyMod);
						//El damage no puede ser negativo si no, estaria curando al enemigo
						if(damage < 0) {
							damage = 0;
						}
						sprintf(message, "Has atacado al %s, pero se ha defendido! Realizaste %d de damage.\n", entities[enemy].name, damage);
						//Mensaje de consola
						printf("El enemigo recibe %d de damage.\n", damage);
						entities[enemy].currentHealthPoints -= damage;
						//Compruebo si el enemigo murio
						if(entities[enemy].currentHealthPoints <= 0) {
							//Mensaje de consola
							printf("Enemigo muerto.\n");
							fightStatus = STATUS_WIN;
						}
					} else if (playerAction == ACTION_HEAL) {
						//Curo al personaje
						healing = entities[player].healingPower + playerMod;
						entities[player].currentHealthPoints += healing;
						sprintf(message, "Te has curado por %d puntos.\n", healing);
						//No puede tener mas vida que la maxima
						if(entities[player].currentHealthPoints > entities[player].maxHealthPoints) {
							entities[player].currentHealthPoints = entities[player].maxHealthPoints;
						}
					} else {
						sprintf(message, "Ambos se han defendido. No sucede nada.\n");				
					}
					break;
			}
		}
	printf("Status: %d\n", fightStatus);
	} while(fightStatus == STATUS_CONTINUE);

	if(fightStatus == STATUS_WIN) {
		sprintf(cadena, "%d|%sGanaste! Has derrotado a un %s. Felicitaciones!\n", fightStatus, message, entities[enemy].name);
	} else {
		sprintf(cadena, "%d|%sPerdiste! Te ha derrotado un %s. Mejor suerte para la proxima!\n", fightStatus, message, entities[enemy].name);
	}

	writeSocket(clientSocket, cadena, strlen(cadena) + 1);
	//Cierro Socket
	close (clientSocket);
	close (serverSocket);
	printf("Sockets cerrados.\n");

	return 0;
}
