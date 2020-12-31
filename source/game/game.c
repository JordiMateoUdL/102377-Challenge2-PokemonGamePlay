#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>
#include <fcntl.h>
#include <time.h>
#include "game.h"
#include "ash.h"

int create_ash(char *ash_args[], char* msg){
    logger("INFO", msg);
    int pid_ash;
    switch (pid_ash = fork()){
        case -1:	perror("Error pokedex fork");
        case 0:	;
                execv(ash_args[0], ash_args);
                perror("Error exec");
                break; 
        }
    return pid_ash;
} 

/** @brief GamePlay process manager (entrypoint)
 *  @return Should return 0 with a normal execution.
 *  @return Should return -1 if there are any strange issue running the code.
 *  @return Should return -2 if the daemon is not running.
 *  @param 0 parameters represents normal mode.
 *  @param 1 parameters represents background mode. Does not matter which parameters is.
 *           Be carefoul: Only use this mode if you know what you are doing. 
 */
int main( int argc, char* argv[] ){

    /** 
     * @warning: Ignoring signals. This step is crucial, the children and grandchild's (synchronization) 
     * with signals must not affect this parent process.
     */
    signal(SIGUSR1, SIG_IGN);
    signal(SIGINT,  SIG_IGN);
    signal(SIGUSR2, SIG_IGN);
    
    int pid_daemon;
    int coderet;
    char msg [300];

    int fd=0;
    if (argc == 2 ){
        /** 
         * @warning: Background mode: Be sure of running cmd: mkfifo mypipe before launching.
         */
        fd = open("/root/tmp/myfifo", 0660);
        if (fd < -1){
            perror("Error. The pipe must be created first.");
            exit(1);
        }
    }

    printf("###### Pokemon GamePlay Process ###### \n###### OS-Challenge#2           ###### \n");

    FILE* f = fopen(LOCK_FILE, "r");

    if ( f == NULL ){
        perror("Error fopen[LOCK_FILE]: ");
        exit(1);
    }

    coderet = fscanf(f,"%d",&pid_daemon);

    if ( coderet < 0 ){
        perror("Error fscanf[LOCK_FILE]: ");
        exit(1);
    }
    
    fclose(f);

    /**
     * @note From kill manual: 
     * If sig is 0, then no signal is sent, but existence and permission
       checks are still performed; this can be used to check for the
       existence of a process ID or process group ID that the caller is
       permitted to signal.
    */
    if ( 0 == kill(pid_daemon, 0) ) {
        sprintf(msg,"[%d] %s [OK] %s %s and now is %s RUNNING %s \n", getpid(),KBLU,KNRM,DAEMON_MSG,KBLU,KNRM);
        logger("INFO", msg);
    } else {
        sprintf(msg,"[%d] %s [ERROR] %s %s and now is %s NOT RUNNING %s \n", getpid(),KRED,KNRM,DAEMON_MSG,KRED,KNRM);
        logger("INFO", msg);
        exit(2);
    }

    int selection=0;
    while ( selection != 4) {

        printf("###### 1.- Check Pokedex. \n");
        printf("###### 2.- Capture adventure.\n");
        printf("###### 3.- Simulate a battle. \n");
        printf("###### 4.- Exit \n");
        printf("###### Please, choose an option: ");
        fflush(stdout);
        
        char str[2];
        if (read(fd, str, 2) < -1){
            perror("Error. Reading user input.");
            exit(1);
        }

        selection = atoi(str); 
     
        if ( coderet < 0 ){
            perror("Error reading from input: ");
            exit(1);
        }

        switch(selection)
        {
            case 1: 

                sprintf(msg,"[%d] Launching pokedex...\n", getpid());
                logger("INFO", msg);

                int pid_ash1;
                switch (pid_ash1 = fork()){
                    case -1:	perror("Error pokedex fork");
                    case 0:	/**
                             * @brief: Launching a child-grandchild process to handle these actions. 
                             * Note: The child process is ash who is responsible to ask the user the input 
                             * (pokemonid) and to launch a grandchild pokedex responsible for checking in 
                             * the memory the pokemon information related to pokemonid. The inner-communications
                             * are done using pipes.	
                             */ 
                    ;
                    char *ash_p1_args[] = {"/home/pokemon-user/source/ash-p1", "/home/pokemon-user/source/ash-p1", NULL};
                    execv(ash_p1_args[0], ash_p1_args);
                    perror("Error exec");
                    break;  
                }
                waitpid(pid_ash1,0,0);
                break;
            case 2: ;
            sprintf(msg,"[%d] Launching capture adventure...\n", getpid());
                logger("INFO", msg);
    
                int pid_ash2;
                switch (pid_ash2 = fork()){
                    case -1:	perror("Error pokedex fork");
                    case 0: ;
                    char *ash_p2_args[] = {"/home/pokemon-user/source/ash-p2", "/home/pokemon-user/source/ash-p2", NULL};
                    execv(ash_p2_args[0], ash_p2_args);
                    perror("Error exec");
                    break;  
                }
                waitpid(pid_ash2,0,0);
                break; 
            case 3: 
                sprintf(msg,"[%d] Launching capture adventure...\n", getpid());
                char *ash_p3_args[] = {"/home/pokemon-user/source/ash-p3", "/home/pokemon-user/source/ash-p3", NULL};
                int pid_ash3 = create_ash(ash_p3_args,msg);
                waitpid(pid_ash3,0,0);
                break;
            case 4: break;
        }

    }

    exit(0);
}