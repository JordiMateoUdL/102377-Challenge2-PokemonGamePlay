#include "ash.h"

/**
 * @brief This function is the handler of signal SIGUSR1. 
 * When ash-px receives this signals it means that the process must change 
 * it status and make progress. First time, ash process will receives SIGUSR1 
 * when pokedex is ready. It let ash to check information in the Pokedex. 
 * The state ready_mode indicates if ash makes an infinite loop (challenge1 requirement)
 * or not get block and just use the Pokedex when it needs. 
 * The other states are needed to control and sync the pokemon battles (challenge 3).
 * */
void changeStatus(){

    if(current_state==WaitingPokedex){
         current_state=ready_mode;
    }
    if(current_state==WaitingPokemon && pokemonsReady<2){
         pokemonsReady=pokemonsReady + 1;
    }
    if(current_state==WaitingPokemon && pokemonsReady==2){
         current_state=Fighting;
    }
    if(current_state==Fighting){
        current_state=EndFight;
    }
}

void logger(const char* tag, const char* message) {
   time_t now;
   time(&now);
   printf("%s [%s]: %s", ctime(&now), tag, message);
   fflush(stdout);
}

void printPokemon( struct pokemon pokemon){
    char s[1000];
    sprintf(s,"%s@@@@@@@@@@@@@@@@@@@@@@@\n %s (%d) \n +++ Type1: %s, Type2: %s\n +++ Total: %d, Hp: %d, \n +++ Attack: %d, Defense: %d\n +++ SpAttack: %d, SpDefense: %d, Speed: %d\n +++ Gen: %d Legendary: %d\n@@@@@@@@@@@@@@@@@@@@@@@%s\n", 
    KCYN,pokemon.name,pokemon.id,
    pokemon.type[0],pokemon.type[1],pokemon.total,pokemon.hp,pokemon.attack,pokemon.defense,
    pokemon.spAttack,pokemon.spDefense,pokemon.speed,pokemon.generation,pokemon.legendary , KNRM
    );
    write(1,&s,strlen(s));
}

/**
 * @brief This function handles a proper end to ash-px processes. It always ensures:
 *  waiting and finishing pokedex (child) and pipes associated. Then depending on the
 *  game mode (2-capture o 3-battle) it ends and waits the childs and closes pipes associated.
 * */
void endAsh(){

    if (ready_mode==WaitingPokemon){
        endPokemon();
    }

    if (ready_mode==EndFight){
        endCombat();
    }

    endPokedex();

    exit(0);
}

void endCombat(){

    char msg [200];

    for(int p=0; p<2; p++){
        waitpid(pidPokemon[p],0,0);
        sprintf(msg,"[%d] Received the signal SIGCHLD... pokemon [%d] ends properly\n", getpid(),pidPokemon[p]);
        logger("INFO", msg);
    }

    close(pokemon1PipeWR[1]);
    close(pokemon1PipeWR[0]);
    close(pokemon2PipeWR[1]);
    close(pokemon2PipeWR[0]); 
    close(gamePipeRW[1]);
    close(gamePipeRW[0]);  
    close(gamePipeWR[1]);
    close(gamePipeWR[0]); 
    
}
    
void endPokemon(){

    char msg [200];
  
    waitpid(pokemonPid,0,0);
    close(pokemonPipe[0]);
    close(pokemonPipe[1]);
    
    sprintf(msg,"[%d] Received the signal SIGCHLD... pokemon [%d] ends properly\n", getpid(),pokemonPid);
    logger("INFO", msg);
    
}

void endPokedex(){

    char msg [300];
    sprintf(msg,"[%d] Received the signal SIGINT... so we send SIGUSR1 to pokedex [%d]\n", getpid(),pokedexPid);
    logger("INFO", msg);

    kill(pokedexPid,SIGUSR1);
    waitpid(pokedexPid,0,0);

    sprintf(msg,"[%d] Received the signal SIGCHLD... so Pokedex[%d] ends properly\n", getpid(),pokedexPid);
    logger("INFO", msg);

    close(pokedexPipe1[0]);
    close(pokedexPipe2[0]);
    close(pokedexPipe1[1]);
    close(pokedexPipe2[1]);

}

void launchPokedex(){

    signal(SIGUSR1, changeStatus);
    signal(SIGINT, endAsh);
    signal(SIGUSR2,SIG_IGN);

    if (pipe(pokedexPipe1) < 0) perror("Error. Opening pokedexPipe1.");
    if (pipe(pokedexPipe2) < 0) perror("Error. Opening pokedexPipe2.");

    switch (pokedexPid = fork()){
        case -1:	perror("Error pokedex fork");
        case 0:		/*Fill*/
              if (close(pokedexPipe1[0]) < 0) perror("Error in pokedexPipe1 -> close pokedex [0]");
              if (dup2(pokedexPipe1[1], 3) < 0) perror("Error in pokedexPipe1 -> dup2 pokedex");
              if (close(pokedexPipe1[1]) < 0) perror("Error in pokedexPipe1 -> close pokedex [1]");

              if (close(pokedexPipe2[1]) < 0) perror("Error in pokedexPipe2 -> close pokedex [0]");
              if (dup2(pokedexPipe2[0], 0) < 0) perror("Error in pokedexPipe2 -> dup2 pokedex");
              if (close(pokedexPipe2[0]) < 0) perror("Error in pokedexPipe2 -> close pokedex [1]");

              char *pokedex_args[] = {"/home/pokemon-user/source/pokedex", "/home/pokemon-user/source/pokedex", NULL};
              execv(pokedex_args[0], pokedex_args);
              
              perror("Error exec");
              break;
     }

    current_state=WaitingPokedex;
    while( current_state == WaitingPokedex){}

    char msg [100];
    sprintf(msg,"[%d] Pokedex is ready to search info...\n", getpid());
    logger("INFO", msg);

    
    while( current_state == Ready){
        int pokemonId=0;
        /**
         * @warning: This is a first-generation Pokedex, so the valid ids are [1-151].
         * */
        while (pokemonId<1 || pokemonId>151){
            printf("Enter a pokemonId:  ");
	        scanf("%d",&pokemonId);
        }
       
        struct pokemon p;
        if (write(pokedexPipe2[1],&pokemonId,sizeof(int)) < 0) perror("Error. Writing pokemonId in pokedex2 pipe");
        if (read(pokedexPipe1[0],&p,sizeof(struct pokemon)) < 0) perror("Error. Reading pokemon from pokedex1 pipe");;
        printPokemon(p);
 
    }
    
}

void adventure(){

    int endFlag = 1;
    int endTurn = 1;
    int validTurn=1;
    int st;
    char msg [300];

    while (endFlag == 1) { 

        char s[100];
        char choice;
        char turnOption;
        
        sprintf(s, "################\n# E. Explore \n# Q. Quit\n################\n");
		if (write(1, s, strlen(s)) < 0) perror("Error. Writting the adventure menu.");

        scanf(" %c", &choice);

        switch (choice) { 
            case 'Q':
                endFlag=0;
                raise(SIGINT);
                break; 
            case 'E':
                if (pipe(pokemonPipe) < 0) perror("Error. Opening pokemonPipe");
                switch (pokemonPid = fork()) 
                {
                    case -1:	perror("Error adventure fork");

                    case 0:	;
                            if (close(pokemonPipe[0]) < 0) perror("Error in pokemonPipe. Closing pokemonPipe[0]");
                            if (dup2(pokemonPipe[1], 3) < 0) perror("Error in pokemonPipe. dup2(pokemonPipe[1],3)");
                            if (close(pokemonPipe[1]) < 0) perror("Error in pokemonPipe. Closing pokemonPipe[1]");

                            char *pokemon_args[] = {"/home/pokemon-user/source/pokemon", "/home/pokemon-user/source/pokemon", NULL};
                            execv(pokemon_args[0], pokemon_args);
                            perror("Error exec");

                    default:
                            sprintf(s, "%s[%d] pid=%d creat%s\n", KBLU, getpid(), pokemonPid, KNRM);
                            
                            int pokemonId;
                            if (read(pokemonPipe[0], &pokemonId, sizeof(int)) < 0) perror("Error read pipe");
                            
                            sprintf(s, "Ash:[%d] --> %sWild pokemon appeared [%d]%s\n", getpid(),KBLU,pokemonPid, KNRM);
                            
                            if (write(1, s, strlen(s)) < 0) perror("Error write 'Creació fill'"); 
                            if (write(pokedexPipe2[1], &pokemonId, sizeof(int)) < 0) perror("Error write pipe");
                            
                            struct pokemon p;
                            if (read(pokedexPipe1[0], &p, sizeof(struct pokemon)) < 0) perror("Error write pipe"); 
                            printPokemon(p);
                             
                            endTurn=1;
                            while(endTurn){

                                sprintf(msg, "# P. Throw pokeball \n# R. Run \n");
                                if (write(1, msg, strlen(msg)) < 0) perror("Error writting the capture menu");
                                scanf(" %c", &turnOption);

                                switch(turnOption) {
                                    case 'P':
                                        kill(pokemonPid, SIGUSR1);
                                        validTurn=0;
                                        break;
                                    case 'R':
                                        kill(pokemonPid, SIGUSR2);
                                        validTurn=0;
                                        break;
                                    default:
                                            sprintf(s, "%s!!!!Invalid option. Try again. %s\n", KRED, KNRM);
                                            if (write(1, s, strlen(s)) < 0) perror("Error writting invalid option");
                                            validTurn=1;
                                            break;
                                }
                                if(validTurn==0){
                                    waitpid(-1, &st, WUNTRACED);
                                    int status = WEXITSTATUS(st);
                                    if(status==PokemonEscaped){
                                        sprintf(s, "%sThe pokemon escaped already%s\n", KYEL,KNRM);
                                        if (write(1, s, strlen(s)) < 0) perror("Error writting the menu");
                                        endTurn=0;
                                    } else if (status==PokemonCaught) {
                                        sprintf(s, "%sGotcha!The pokemon was caught.%s\n", KGRN,KNRM);
                                        if (write(1, s, strlen(s)) < 0) perror("Error writting the menu");
                                        endTurn=0;
                                    } else if (status==AshEscaped) {
                                        sprintf(s, "%sYou have escaped safely.%s\n", KRED,KNRM);
                                        if (write(1, s, strlen(s)) < 0) perror("Error writting the menu");
                                        endTurn=0;
                                    }  
                                    else {
                                        sprintf(s, "%sOh no!The pokemon broke free.%s\n", KBLU,KNRM);
                                        if (write(1, s, strlen(s)) < 0) perror("Error writting the menu");
                                        kill(pokemonPid, SIGCONT);
                                    }
                               }
                            }

                            }
                break;
            default: 
                sprintf(s, "%s!!!!Invalid option. Try again. %s\n", KRED, KNRM);
                if (write(1, s, strlen(s)) < 0) perror("Error writting invalid option");
     } 
} 

}

void battle(){

    pokemonsReady=0;
    current_state=WaitingPokemon;
    char msg [200];
    

    if (pipe(pokemon1PipeWR) < 0) perror("Error. Opening pokemon1PipeWR");
    if (pipe(pokemon2PipeWR) < 0) perror("Error. Opening pokemon2PipeWR");
    if (pipe(gamePipeRW) < 0) perror("Error. Opening pokemonPipeWR");
    if (pipe(gamePipeWR) < 0) perror("Error. Opening pokemonPipeWR");

    for(int p=1; p<=2; p++){
        switch (pidPokemon[p] = fork()){
            case -1:	perror("Error. Forking a new pokemon to battle.");
            case 0:	
                // gamePipeWR: Ash -> Pokemon (Pipe)
                if (close(gamePipeWR[1]) < 0) perror("Error. Closing gamePipeWR[1]");
                if (dup2(gamePipeWR[0], 0) < 0) perror("Error. Dup2 gamePipeWR <- 0");
                if (close(gamePipeWR[0]) < 0) perror("Error. Closing gamePipeWR[0]");

                // pokedexPipe1: Pokemon -> Pokedex (Pipe)
                if (close(pokedexPipe1[1]) < 0) perror("Error. Close pokedexPipeWR [1]");
                if (dup2(pokedexPipe1[0], 2) < 0) perror("Error. Dup2 pokedexPipeWR <- 3");
                if (close(pokedexPipe1[0]) < 0) perror("Error. Close pokedexPipeWR [0]");

                // pokedexPipe1: Pokedex -> Pokemon (Pipe)
                if (close(pokedexPipe2[0]) < 0) perror("Error. Close pokedexPipeRW [0]");
                if (dup2(pokedexPipe2[1],3) < 0) perror("Error. Dup2 pokedexPipeRW <- 4");
                if (close(pokedexPipe2[1]) < 0) perror("Error. Close pokedexPipeRW[1]");

                // pokemon1PipeWR: Pokemon -> Pokemon (Pipe) 
                if (dup2(pokemon1PipeWR[0], 4) < 0) perror("Error. Dup2 pokemon1PipeWR <- 5");
                if (close(pokemon1PipeWR[0]) < 0) perror("Error. Close pokemon1PipeWR [0]");
                if (dup2(pokemon1PipeWR[1], 5) < 0) perror("Error. Dup2 pokemon1PipeWR <- 6");
                if (close(pokemon1PipeWR[1]) < 0) perror("Error. Close pokemon1PipeWR [1]");

                // pokemon2PipeWR: Pokemon -> Pokemon (Pipe) 
                if (dup2(pokemon2PipeWR[0], 6) < 0) perror("Error. Dup2 pokemon2PipeWR <- 7");
                if (close(pokemon2PipeWR[0]) < 0) perror("Error. Close pokemon2PipeWR [0]");
                if (dup2(pokemon2PipeWR[1], 7) < 0) perror("Error. Dup2 pokemon2PipeWR <-8");
                if (close(pokemon2PipeWR[1]) < 0) perror("Error. Close pokemon2PipeWR [1]");

                // arguments
                char initialFightMode[32];
                char color[32];
                sprintf(initialFightMode,"%d", 4+p-1);
                sprintf(color,"%d", p-1);

                char *pokemon_fight[] = {"pokemon-fight",initialFightMode,color, NULL};
                execv(pokemon_fight[0], pokemon_fight);
        }
        printf("p:%d pokemons:%d\n",p,pokemonsReady); fflush(stdout);
        while(pokemonsReady<p){}
    }

    sprintf(msg,"[%d] The pokemons are ready to fight...\n", getpid());
    logger("INFO", msg);
   
    struct info i;
    
    /**
     * @warning: Be careful, we can not know beforehand which process (pokemon) 
     * is going to get the CPU first. Thus, we do not know who will read first from 
     * the shared gamePipe.
     * @note: It does not matter which process reads first, because it do not matter
     * which file descriptors we assign to each process. If we are consistent. 
     * @definition: Pipes are unidirectionals.
     * @example: fd=4 reprsents reading from pipe1, and fd=7 represents writting from pipe2.
     * fd=5 reprsents writting from pipe1, and fd=6 represents reading from pipe2.
     **/

    i.fdR=4; i.fdW=7;
    if (write(gamePipeWR[1],&i, sizeof(struct info)) < 0) perror("Error writting in the pipe info1");

    i.fdR=6;i.fdW=5;
    if (write(gamePipeWR[1],&i, sizeof(struct info)) < 0) perror("Error writting in the pipe info1"); 

    current_state=Fighting;

    sprintf(msg,"[%d] The pokemons start fighting...\n", getpid());
    logger("INFO", msg);

    while(current_state==Fighting){}

    sprintf(msg,"%s[%d] The pokemons end the fight...\n", KNRM,getpid());
    logger("INFO", msg);

    endAsh();
}