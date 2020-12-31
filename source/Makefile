CC = gcc
SOURCES = $(wildcard *.c)
TARGET = $(SOURCES:%.c=%)
OBJECTS = game/ash.o
CFLAGS = -g -Wall
all: $(TARGET)
%.o: %.c
%: %.c
	$(CC) $(CFLAGS) $(OBJECTS) -o $@ $<

run_%: %
	./$< 
test_%: % pokemond
	chmod +x ./test/test_game.sh 
	./test/test_game.sh
clean:
	@rm -rf *.o $(TARGET)








	


