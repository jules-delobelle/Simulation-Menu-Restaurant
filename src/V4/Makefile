CC = gcc
CFLAGS = -Wall -Wextra -g
SRC_COM = fonctions_communication.c
OBJ_COM = fonctions_communication.o

TARGETS = client lecteur_relais lecteur_final

all: $(TARGETS)

$(OBJ_COM): $(SRC_COM) fonctions_communication.h
	$(CC) $(CFLAGS) -c $(SRC_COM) -o $(OBJ_COM)

client: client.c $(OBJ_COM)
	$(CC) $(CFLAGS) client.c $(OBJ_COM) -o client

lecteur_relais: lecteur_relais.c $(OBJ_COM)
	$(CC) $(CFLAGS) lecteur_relais.c $(OBJ_COM) -o lecteur_relais

lecteur_final: lecteur_final.c $(OBJ_COM)
	$(CC) $(CFLAGS) lecteur_final.c $(OBJ_COM) -o lecteur_final

clean:
	rm -f $(OBJ_COM) $(TARGETS)

cleanall: clean
	rm -f /tmp/client_*.fifo /tmp/relais_*.fifo \
		/tmp/from_relais_to_client \
		/tmp/from_lecteur_final_to_relais

.PHONY: all clean cleanall