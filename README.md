
<img align="right" width="90" height="40" src="https://user-images.githubusercontent.com/61190134/103407124-a0fe6380-4b5d-11eb-9692-a7a372f816a2.png">

# 102377-Challenge2-PokemonGamePlay

## About ##

|            |                                              |  
| ---------- | -------------------------                    |  
| Title:     | Challenge 2: Pokemon GamePay Processes       |  
| Instructor and Author:| [Jordi Mateo Fornés](http:jordimateofornes.com)                           |  
| *Fall*:      | 2020-2021                                   |  
| *Course*:    |    102377 - Operating Systems - [Grau en Tècniques d'Interacció Digital i de Computació](http://www.grauinteraccioicomputacio.udl.cat/ca/index.html) |
| University:     | [University of Lleida](http://www.udl.cat/ca/) - [Campus Igualada](http://www.campusigualada.udl.cat/ca/) - [Escola Politècnica Superior](http://www.eps.udl.cat/ca/)       |  
| Copyright: | Copyright © 2019-2020 Jordi Mateo Fornés     |  
| Version:   | 1.0

## Scope

A UNIX process or job is the result of executing a UNIX command. Processes are created by UNIX commands (including the commands that open windows in X), program executions (including GCC (1), mail
(1) and programs you write and compile), and the C-shell command interpreter itself. At any moment,
a process may be either running or stopped. The UNIX operating system provides many ways to control
these processes, such as suspending, resuming and terminating.

All the tasks can be found in this document: [OS-2021-Challenge2.pdf](https://github.com/JordiMateoUdL/102377-Challenge2-PokemonGamePlay/files/5757129/OS-2021-Challenge2.pdf)

## Organization

- **data/**: This folder contains data required for running the code.
  - *pokedex.csv*: A csv file with information related to the first generation pokemons.
  
- **docker/**: This folder contains the docker image used to run the code.
  - *Dockerfile*: Docker image based on ubuntu.
- *create-pokemon-container*: Bash script to create and sanitize the docker container.

- **source/**: This folder contains the implementation.
  - *Makefile*: To compile all the *.c in this folder. Please, note that this makefile allows to compile multiple main functions files. It is important, because we are making execv calls and this require several main function for different processes ash, pokedex, pokemons...
  - *pokemond.c*: Simulates a very simple unix daemon process.
  - *pokemon.h*: Defines the pokemon struct using the csv fields (headers).
  - *pokedex.c*: Reads the csv into memory and generates and instance of pokemon struct. It reads the pokemon id from the file descriptor 0 and write into the file descriptor 3.
  - *ash-p1.c*: Ash wants to check information using the pokedex device. First of all, the code creates the pokemon process, once the pokedex is ready (all csv is)
  - *ash-p2.c*: Solve the second activity in the challenge. Situation depitcted in image 1.
  - *pokemon.c*: Represents a wild pokemon that appears to be caught.
  - *ash-p3.c*: Solve the third activity in the challenge. Situation depitcted in image 1.
  - *pokemon-fight: Represents a pokemon, it is used to simulate pokemon battles.
  - *packet.h*: Struct used to exchange multiple information between pipes. Ash uses an instance of this struct to tell pokemon-fight which file descriptors represent the pipes to read and write.
  - **game**:  This folder contains the common information and the orchestrator process.
    - *ash.h*: Common definitions for all ash-px.
    - *ash.c*: Common implementations for all ash-px.
    - *game.h*: Common definitions for ash-px, pokedex, pokemon and pokemon-fight.
    - *game.c*: Implementation of orchestrator process. The main goal is to handle the overall execution of (ash-px).
    - *Makefile*: This makefile is used to compile and link game and ash implementation and definitions.
  - **test**: This folder contains the tests and the test suite.
- *Makefile*: This makefile is the **orchestrator** make. It manages all the makefiles stored in the subfolders.
  

## How to use the code 

### Using without docker

There is no constraint or reason to use the code in a docker container. This code can be executed in any virtual or real Linux operating system. The main reason to exchange the container is to pack, ship, and provide a lightweight, portable, and self-sufficient tool to replicate and execute the code.

```bash
# To execute the code (automatic way)
make run
# To launch the provide test suite (automatic way)
make test
# To execute manually
make all
#  1. Initiate the daemon
cd source
make run_pokemond
#  2. Initiate the orchestrator
./game/game
```

### Using containers

> :warning: Make sure you have docker installed in your host machine.

1. Create the container to run the executions

- Connect your shell to the container and execute the same cmd we describe in the previous section.
   
```bash
docker run -it -v $(PWD):/home/pokemon-user/source  --entrypoint /bin/bash pokemon-service
```

- Execute and run automatic in a container
  
```bash
docker run --rm  pokemon-service make run
docker run --rm  pokemon-service make test
```

![logo-eng](https://user-images.githubusercontent.com/61190134/103407749-0eab8f00-4b60-11eb-8b96-c26a90b7cdc6.png)