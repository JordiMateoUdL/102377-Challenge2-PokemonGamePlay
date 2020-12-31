#ifndef _POKEMON_H_
#define _POKEMON_H_

#define MAXLEN  32

struct pokemon {
  int  id;
  char name[MAXLEN];
  char type[2][MAXLEN];
  int total;
  int hp;
  int attack;
  int defense;
  int spAttack;
  int spDefense;
  int speed;
  int generation;
  int legendary;
  int seen;
  int captured;
};

#endif