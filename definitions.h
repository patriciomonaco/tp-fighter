#ifndef _DEFINICIONES_H
#define _DEFINICIONES_H

#define LENGTH 1024
#define ENTITIES 9
#define HOST "localhost"
#define SERVICE "tp-fighter"

typedef enum {
	STATUS_NONE,
	STATUS_CONTINUE,
	STATUS_LOSE,
	STATUS_WIN
} Status;

typedef enum {
	ACTION_NONE,
	ACTION_ATTACK,
	ACTION_DEFEND,
	ACTION_HEAL
} Actions;

typedef enum {
	CHAR_NONE,
	CHAR_PALADIN,
	CHAR_ROGUE,
	CHAR_WARRIOR,
	CHAR_CAPYBARA,
	CHAR_PENGUIN,
	CHAR_GUANACO,
	CHAR_IORO,
	CHAR_TATU
} Characters;

#endif
