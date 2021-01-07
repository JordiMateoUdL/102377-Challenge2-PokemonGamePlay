#!/bin/bash
all: 
	cd source/game && make;
	cd source && make;
clean: 
	cd source && make clean;
	cd source/game && make clean;
run: all
	cd source && make run_pokemond;
	trap "" INT;
	sleep 3;
	cd source && ./game/game;
test: all
	cd source && make test_game
