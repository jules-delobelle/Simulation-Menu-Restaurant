CC = gcc
CFLAGS = -Wall -Wextra -g
SRC_COM = fonctions_communication.c
OBJ_COM = fonctions_communication.o

TARGETS = client routeur dataserveur

all: $(TARGETS)

$(OBJ_COM): $(SRC_COM) fonctions_communication.h
	$(CC) $(CFLAGS) -c $(SRC_COM) -o $(OBJ_COM)

client: main.c client.c $(OBJ_COM)
	$(CC) $(CFLAGS) -DCLIENT main.c client.c $(OBJ_COM) -o client -lpthread

routeur: main.c routeur.c $(OBJ_COM)
	$(CC) $(CFLAGS) -DROUTEUR main.c routeur.c $(OBJ_COM) -o routeur -lpthread -lrt

dataserveur: main.c dataserveur.c $(OBJ_COM)
	$(CC) $(CFLAGS) -DDATASERVEUR main.c dataserveur.c $(OBJ_COM) -o dataserveur -lpthread

clean:
	rm -f $(OBJ_COM) $(TARGETS)

cleanall: clean
	rm -f /tmp/dataserveur_*.fifo /tmp/routeur_*.fifo \
		/tmp/from_routeur_to_client \
		/tmp/from_dataserveur_to_routeur
	sem_unlink /verrou_routeur 2>/dev/null || true

.PHONY: all clean cleanall