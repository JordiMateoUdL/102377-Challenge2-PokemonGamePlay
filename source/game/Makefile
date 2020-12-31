CC = gcc
SOURCES = $(wildcard *.c)
OBJECTS = $(SOURCES:%.c=%.o)
CFLAGS = -c -Wall
EXECUTABLE=game

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	@rm -rf *.o $(TARGET)








	


