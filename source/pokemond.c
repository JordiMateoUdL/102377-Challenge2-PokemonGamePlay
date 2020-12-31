#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include "game/game.h"

int status = 0;

void log_message(char *filename, char *message)
{
    FILE *logfile;
    time_t curtime; 
    time(&curtime); 

    char *t = ctime(&curtime);
    if (t[strlen(t)-1] == '\n') t[strlen(t)-1] = '\0';
      
	logfile=fopen(filename,"a");
	if(!logfile) return;
	fprintf(logfile,"[%s] %s \n",t, message);
	fclose(logfile);
}

void signal_handler(sig)
int sig;
{
	switch(sig) {
	case SIGTERM:
        log_message(LOG_FILE,  "Rebuda la señal per aturar el dimoni pokemon \n");
		status = FINISHED;
	}
    
}


static void daemonize()
{
    pid_t pid;
    int lfp;
    char str[10];

    /* Fork off the parent process */
    pid = fork();

    /* An error occurred */
    if (pid < 0)
        exit(EXIT_FAILURE);

    /* Success: Let the parent terminate */
    if (pid > 0)
        exit(EXIT_SUCCESS);

    /* On success: The child process becomes session leader */
    if (setsid() < 0)
        exit(EXIT_FAILURE);

    /* Catch, ignore and handle signals */
    signal(SIGCHLD,SIG_IGN); /* ignore child */
	signal(SIGTSTP,SIG_IGN); /* ignore tty signals */
	signal(SIGTTOU,SIG_IGN);
	signal(SIGTTIN,SIG_IGN);
	signal(SIGHUP,SIG_IGN); /* catch hangup signal */
	signal(SIGTERM,signal_handler); 

    /* Fork off for the second time*/
    pid = fork();

    /* An error occurred */
    if (pid < 0)
        exit(EXIT_FAILURE);

    /* Success: Let the parent terminate */
    if (pid > 0)
        exit(EXIT_SUCCESS);

    /* Set new file permissions */
    umask(0);

    /* Change the working directory to the root directory */
    /* or another appropriated directory */
    chdir(RUNNING_DIR);

    /* Close all open file descriptors */
    int x;
    for (x = sysconf(_SC_OPEN_MAX); x>=0; x--)
    {
        close (x);
    }

    lfp=open(LOCK_FILE,O_RDWR|O_CREAT,0640);
	if (lfp<0) exit(1); /* can not open */
	if (lockf(lfp,F_TLOCK,0)<0) exit(0); /* can not lock */
	/* first instance continues */
	sprintf(str,"%d\n",getpid());
	write(lfp,str,strlen(str)); /* record pid to lockfile */

    status = RUNNING;
}




int main()
{

    daemonize();

    while ( status == RUNNING )
    {
        char * msg =  KNRM "The pokemond is " KBLU " RUNNING " KNRM " \n";
        log_message(LOG_FILE,  msg);
        sleep (60);
    }

    char * msg =  KNRM "The pokemond is  " KRED " NOT RUNNING " KNRM " \n";
    log_message(LOG_FILE, msg);

    return EXIT_SUCCESS;
}