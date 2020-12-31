#!/bin/bash

# Test suite
RED="\e[31m"
GREEN="\e[32m"
NRM="\e[0m"

## Clean 
kill -n 9 $(pidof pokemond)
rm -rf $HOME/tmp/* 

## Test 1
echo "## Test 1. Ensure that game process is not launch when pokemond is not running..."
./game/game > $HOME/tmp/stdout.txt 2>&1
retcode=$?

if [ "$retcode" != "1" ]; then
    echo -e "    "$RED"[FAILURE]"$NRM". The status code of game process is $retcode and must be 1."
fi
    echo -e "    "$GREEN"[PASSED]"$NRM".  The status code of game process is $retcode and must be 1."

## Test 2
echo "## Test 2. Ensure that the pid of the daemon is properly stored in pokemond.lock file..."
./pokemond
sleep 1 
pid=$(pidof pokemond)
if [ "$pid" != $(cat $HOME/tmp/pokemond.lock) ]; then
    echo -e "    "$RED"[FAILURE]"$NRM". Pid:$pid is different from Pid:$(cat $HOME/tmp/pokemond.lock)"
fi
    echo -e "    "$GREEN"[PASSED]"$NRM". Pid:$pid is the same stored in pokemond.lock: $(cat $HOME/tmp/pokemond.lock)"

## Test 3
echo "## Test 3. Ensure that now with pokemond launched, the game is launched properly..."
mkfifo $HOME/tmp/myfifo
./game/game 1 > $HOME/tmp/stdout.txt 2>&1 &
sleep 3
game_pid=$(pidof game)

if [ $game_pid > 0 ]; then
  echo -e "    "$GREEN"[PASSED]"$NRM". The game is launched and the pid is $game_pid"
else
  echo -e "    "$RED"[FAILURE]"$NRM". The game is not launched and the pid is $game_pid"
fi

## Test 4
# echo "## Test 4. Ensure user input 1,2 or 3 not ends the process"
# echo 1 > "$HOME/tmp/myfifo"
# echo 2 > "$HOME/tmp/myfifo"
# echo 3 > "$HOME/tmp/myfifo"
# game_pid=$(pidof game)
# if [ $game_pid > 0 ]; then
#   echo -e "    "$GREEN"[PASSED]"$NRM". With user input 1,2 and 3."
# else
#   echo -e "    "$RED"[FAILURE]"$NRM". With user input 1,2 and 3."
# fi

## Test 5
echo "## Test 5. Ensure user input 4 ends the process"
echo 4 > "$HOME/tmp/myfifo"
game_pid=$(pidof game)
if [ $game_pid > 0 ]; then
  echo -e "    "$RED"[FAILURE]"$NRM". With user input 4, the process must end, but its pid=$game_pid."
else
  echo -e "    "$GREEN"[PASSED]"$NRM". With user input 4, the process ends properly."
fi

## Test 6
echo "## Test 6. Ensure that once we stop the daemon the game process not runs again..."
kill -TERM $pid
./game/game > $HOME/tmp/stdout.txt 2>&1 
retcode=$?

if [ "$retcode" != "2" ]; then
    echo -e "    "$RED"[FAILURE]"$NRM". The status code of game process is $retcode and must be 2."
else
    echo -e "    "$GREEN"[PASSED]"$NRM".  The status code of game process is $retcode and must be 2."
fi

#### Clean
rm -rf $HOME/tmp/*



