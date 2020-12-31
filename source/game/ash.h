#ifndef ASH_H_   
#define ASH_H_

#include "game.h"
#include "../pokemon.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include "../packet.h"

// Define the status
enum State {WaitingPokedex=1, Ready=2, WaitingPokemon=3, Fighting=4, EndFight=5, EndPokedex=6}; 
int current_state;
int ready_mode;

// Define the boolean logic
#define TRUE 1
#define FALSE 0

// Define functions
void launchPokedex();
void endPokedex();
void endPokemon();
void endAsh();
void endCombat();
void handlerSIG1();
void adventure();
void battle();
void logger(const char* tag, const char* message);
void printPokemon( struct pokemon pokemon);

// Define variables
int pokedexPid;
int pokemonPid;
int pokedexPipe1[2];
int pokedexPipe2[2];
int pokemonPipe[2];
int pokemon1PipeWR[2];
int pokemon2PipeWR[2];
int gamePipeRW[2];
int gamePipeWR[2];
int pidPokemon[2];
int pokemonsReady;

#endif 
