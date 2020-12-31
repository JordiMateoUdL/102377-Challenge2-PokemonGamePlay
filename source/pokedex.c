#include "game/ash.h"

struct pokemon pokedex[151];
char msg [300];

void end(){
  sprintf(msg,"[%d] Received the signal SIGUSR1 from ash process[%d] ENDING...\n", getpid(), getppid());
  logger("INFO", msg);
  exit(0);
}

int main(int argc, char** argv) {
  FILE* f = fopen("/home/pokemon-user/data/pokedex.csv", "r");

  signal(SIGUSR1,end);
  signal(SIGINT,SIG_IGN);

  if(!f) {
    perror("fopen");
    exit(1);
  }

  char *buf = malloc(151);
  char *info;
  int i = 0;

  sprintf(msg,"[%d] The pokedex starts READING ...\n", getpid());
  logger("INFO", msg);

  while (fgets(buf, 151, f) != NULL) {
    if ((strlen(buf)>0) && (buf[strlen (buf) - 1] == '\n'))
        buf[strlen (buf) - 1] = '\0';

    struct pokemon p;
    p.seen=0;
    p.captured=0;

    //printf("%s\n",buf);
    info = strtok(buf, ",");
    int id  = atoi(info);
    p.id = id;
    
    info = strtok(NULL, ",");
    strcpy(p.name, info);

    info = strtok(NULL, ",");
    strcpy(p.type[0], info);

    info = strtok(NULL, ",");
    strcpy(p.type[1], info);

    info = strtok(NULL, ",");
    p.total = atoi(info);

    info = strtok(NULL, ",");
    p.hp = atoi(info); 

    info = strtok(NULL, ",");
    p.attack = atoi(info);

    info = strtok(NULL, ",");
    p.defense = atoi(info);

    info = strtok(NULL, ",");
    p.spAttack = atoi(info);

    info = strtok(NULL, ",");
    p.spDefense = atoi(info);

    info = strtok(NULL, ",");
    p.speed = atoi(info);

    info = strtok(NULL, ",");
    p.generation = atoi(info);

    info = strtok(NULL, ",");
    p.legendary = atoi(info);

    pokedex[i]=p;
  
    i++;

  }
  fclose(f);

  sprintf(msg,"[%d] The pokedex ends READING ... and sends SIGUSR1 to ash [%d]\n", getpid(),getppid());
  logger("INFO", msg);

   kill(getppid(),SIGUSR1);
    while(1){
       int pokemonId;
       if (read(0, &pokemonId, sizeof(int)) < 0) perror("Error[Pokedex] reading pipe");
       sprintf(msg,"[%d] Pokedex receives from the pipe pokemonid: %d \n", getpid(),pokemonId);
       logger("INFO", msg);
       struct pokemon p;
       p = pokedex[pokemonId - 1];
       write(3,&p,sizeof(struct pokemon));
       sprintf(msg,"[%d] Pokedex writes in the pipe pokemon with name: %s \n", getpid(),p.name);
       logger("INFO", msg);
    };
}
