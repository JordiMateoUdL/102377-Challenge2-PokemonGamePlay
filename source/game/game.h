#ifndef GAME_H_   
#define GAME_H_

// Define the colours
#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

// Define the boolean logic
#define TRUE 1
#define FALSE 0



// Messages
#define DAEMON_MSG "The daemon pokemond must be running to launch the processes."
#define ARG_ERR "The game process only accepts 1 numerical argument. This argument represents the input mode, so 0 means stdin in fd 0 other value means that stdin is a named pipe."


// Define daemon properties
#define RUNNING  1
#define FINISHED 0

#define RUNNING_DIR	"/root/tmp/" 
#define LOCK_FILE	RUNNING_DIR"pokemond.lock"
#define LOG_FILE	RUNNING_DIR"pokemond.log"

#endif 
