#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>

int getRandom (int val)
{
  return (rand () % val);
}

void init ()
{
   srand (getpid ());
   int pokemonId = getRandom(151);
   if (write(3, &pokemonId, sizeof(int)) < 0) perror("Error write 'Fi del joc'");
}

void action(){
    int option=getRandom(10);
    if(option==7){
        exit(2);
    } else if (option%2==0){
        exit(3);
    }else{
        raise(SIGSTOP);
    }
}

void action2(){
   exit(4); 
}

int main ()
{
  if (signal(SIGINT, SIG_IGN) == (void (*)(int))-1) perror ("Error. Pokemon - SIGINT");
  if (signal(SIGUSR1, action) == (void (*)(int))-1) perror ("Error. Pokemon - SIGUSR1");
  if (signal(SIGUSR2, action2) == (void (*)(int))-1) perror ("Error. Pokemon - SIGUSR2");
  init();
  while (1){}
}