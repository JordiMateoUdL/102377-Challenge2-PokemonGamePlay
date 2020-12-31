
#include "pokemon.h"
#include <time.h>
#include "game/ash.h"

#define pokedexPipeRWfd 2
#define pokedexPipeWRfd 3
#define START_ATTACKING 4
#define START_DEFENDING 5

enum Fight {NotReady=0, Winner=2, Looser=3, Attack=4,Defense=5};
int currentStatus;

char * rivalColour;
char * myColour;
int fighting = TRUE;


void getColour(int colour){
    if (colour == 1){
            myColour = KMAG;
            rivalColour = KCYN;
    }else{
            myColour = KCYN;
            rivalColour = KMAG;
    }
}

/**
 * @brief pokemon-fight process communication with pokedex process.
 *        We assume that pokedex pipe filedescriptors are 2 and 3. 
 *        Ash needs to prepare it, before launching the process.
 * @param id is a integer between 1-151.
 * @return pokemon struct
 */
struct pokemon getPokemon(int id){
   struct pokemon p;
   if (write(pokedexPipeWRfd, &id, sizeof(int)) < 0) perror("Error. Writting id into pokedex pipe");
   if (read(pokedexPipeRWfd, &p, sizeof(struct pokemon)) < 0)  perror("Error. Reading pokemon from pokedex pipe");
   return p;
}

int getRandomInt(int start, int end){
    srand (time (NULL)+getpid());
    return (start + rand () % (end-start));
}

void changeFightMode(){
    if (currentStatus==Attack){
        currentStatus=Defense;
    }else{
        currentStatus=Attack;
    }
}

void endFight(){
    currentStatus=Winner;
    exit(currentStatus);
}

void sendAttack(int damage, int fd){
    char attack_send[250]; 
    sprintf(attack_send,"%d",damage);
    if (write(fd, attack_send, strlen(attack_send)) < 0) perror("Error write 'Creació fill'"); 
}

int receiveAttack(int fd){
   char attack_received[250];
   if (read(fd, &attack_received, sizeof(char)) < 0) perror("Error read pipe");
   return atoi(attack_received); 
}

int main(int argc, char *argv[])
{
    char msg[300];

    // Signals
    signal(SIGUSR1,changeFightMode);
    signal(SIGUSR2,endFight);

    // ctrl+c can stop fighting, so we 

    /**
     * @param argv1 Defines the fighting mode. One pokemon must start attacking (4) 
     *             and the other defending (5).
     * @param argv2 Defines the stdout color. The are two colors predefined. KMAG (0) and KCYN (1). 
     * */

    if ( argc != 3 || 
       (atoi(argv[1]) != START_ATTACKING && atoi(argv[1]) != START_DEFENDING ) || 
       (atoi(argv[2]) != 0 && atoi(argv[2]) != 1 )) {
        sprintf(msg,"[%d] Wrong arguments... ending! \n", getpid());
        logger("ERROR", msg);   
        exit(1);
    } 

    int initialFightMode=atoi(argv[1]); 
    getColour(atoi(argv[2])); 

    currentStatus=NotReady;
    struct pokemon p = getPokemon(getRandomInt(0,151));
    int round=0;
    int life = getRandomInt(10,20);
    int currentLife = life;

    currentStatus=initialFightMode;
    kill(getppid(),SIGUSR1);
    sprintf(msg,"[%d] %s is ready to fight, sends SIGUSR1 to ash process [%d]! \n", getpid(),p.name,getppid());
    logger("INFO", msg);

    /**
     * @warning: This point the pokemon process is ready to fight, but needs information about the rival and the 
     *           inner file descriptors that it is going to use to attack and defend from its rival. Ash process 
     *           is going to send to standard input (gamePipe) an struct with (fdW, fdR). Where fdW 
     *           represents the file descriptor to attack and fdR the file descriptor to receive damage.
     * */
    struct info i;
    if (read(0, &i, sizeof(struct info)) < 0) perror("Error read pipe - reciving pid rival");
    int pid = getpid();
    int pidRival;
    struct pokemon pokemonRival;
    int fdR = i.fdR;
    int fdW = i.fdW;

    
    while(fighting == TRUE){
        printf("%s[%d] ########%s[%d/%d]\n", myColour, getpid(), p.name, currentLife,life);
        if (currentStatus==Attack){

            /**
             * @note: It is important, before figthing be polite and say hello! ^^ 
             **/
            if(round==0){
              if (write(fdW, &p, sizeof(struct pokemon)) < 0) perror("Error sending my info to rival");
              if (read(fdR, &pokemonRival, sizeof(struct pokemon) ));
              if (write(fdW, &pid, sizeof(int)) < 0) perror("Error sending my info to rival");
              if (read(fdR, &pidRival, sizeof(int) ));
              round = round +1;
            }

            int attack_damage = getRandomInt(0,6);
            printf("%s[%d] ########%s %s---(Send attack with damage(%d))---> %s%s[%d]%s\n", myColour, getpid(),
                    p.name, KGRN ,attack_damage,rivalColour,pokemonRival.name,pidRival,myColour);
            sendAttack(attack_damage, fdW);
            
            sleep(3);
            raise(SIGUSR1);

        }else if(currentStatus==Defense){

            /**
             * @note: It is important, before figthing be polite and say hello! ^^ 
             **/
            if(round==0){
              if (read(fdR, &pokemonRival, sizeof(struct pokemon) ));
              if (write(fdW, &p, sizeof(struct pokemon)) < 0) perror("Error sending my info to rival");
              if (read(fdR, &pidRival, sizeof(int) ));
              if (write(fdW, &pid, sizeof(int)) < 0) perror("Error sending my info to rival");
              round = round +1;
            }

            int received_damage = receiveAttack(fdR);
            printf("%s[%d] ########%s %s<---(Receives attack with damage(%d))--- %s%s[%d]%s\n", myColour, getpid(),
             p.name, KRED, received_damage,rivalColour,pokemonRival.name,pidRival,myColour);
            currentLife = currentLife - received_damage;
            
            if(currentLife<=0) {
                currentLife=0;
                currentStatus=Looser;
                printf("%s El combat ha acabat. %s%s[%d] a derrotat %s%s[%d]%s\n",KNRM,
                rivalColour,pokemonRival.name,pidRival, myColour,p.name,getpid(),KNRM);
                kill(pidRival,SIGUSR2);
                kill(getppid(),SIGUSR1);
            }else{
            sleep(3);
            raise(SIGUSR1);
            }
        }else{
            exit(currentStatus);
        }
    }
}