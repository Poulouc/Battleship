/*
Role : Fichier du serveur TCP du Battleship
Etudiants : Hugo Babin et Romain Guillon
*/

#include <stdio.h>
#include <errno.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>     // Pour exit() et atoi()
#include <arpa/inet.h>  // Pour inet_aton() et inet_ntoa()
#include <unistd.h>     // Pour read(), write(), close()
#include <time.h>       //pour random
#include <stdbool.h>

// Paramètres du serveur
char* id = 0;   // Identifiant du serveur
short port = 0; // Port du serveur
int sock = 0;   // Socket de communication principale
int nb_reply = 0; // Compteur de réponses envoyées

// Paramètres du jeu
#define GRID_SIZE 20 // Taile de la grille
#define AIRCRAFT_CARRIER 5 // Longueur du porte-avion
#define FRIGATE 3 // Longueur de la frégate
#define MAX_PLAYS 50   // Nombre d'échanges entre le server et le client
bool IsAircraftCarrierSunk = false; // Si le porte avion est coulé
bool IsFrigateSunk = false; // Si la fregate est coulé

// Position du porte-avion sur la grille
int positionAircraftCarrier[5][2] = {
    {-1, -1},
    {-1, -1},
    {-1, -1},
    {-1, -1},
    {-1, -1}
};

// Position de la fregate sur la grille
int positionFrigate[3][2] = {
    {-1, -1},
    {-1, -1},
    {-1, -1}
};

// Fonction qui regarde si la position des bateaux est correct sur la grille
bool isPositionCorrect(int size, int positionBoat[][2]) {
    for (int i = 0; i < size; i++) {
        if(size == FRIGATE && positionBoat[i][0] == positionFrigate[i][0] && positionBoat[i][1] == positionFrigate[i][1]){
            return false;
        }
        else if(size == AIRCRAFT_CARRIER && positionBoat[i][0] == positionAircraftCarrier[i][0] && positionAircraftCarrier[i][1] == positionFrigate[i][1]){
            return false;
        }
    }
    return true;
}

// Place les coordonnées d'un bateau(sans grille)
void placeBoat(int size, int positionBoat[][2]) {
    int x, y;
    if (rand() % 2 == 0) {  // Si 0, le bateau sera placé horizontalement
        do{
            x = rand() % GRID_SIZE;
            y = rand() % (GRID_SIZE - size);  // Assure que le bateau rentre dans la grille
            for (int i = 0; i < size; i++) {
                positionBoat[i][0] = x;    // Coordonnée x
                positionBoat[i][1] = y + i;  // Coordonnée y (incrémentation horizontale)
            }
        }while(isPositionCorrect(size, positionBoat)); //empeche les bateau d'etre l'un sur l'autre
    } 
    else {  // Sinon verticalement
        do{
            x = rand() % (GRID_SIZE - size);
            y = rand() % GRID_SIZE;
            for (int i = 0; i < size; i++) {
                positionBoat[i][0] = x + i;  // Coordonnée x (incrémentation verticale)
                positionBoat[i][1] = y;    // Coordonnée y
            }
        }while(isPositionCorrect(size, positionBoat));
    }
}

// Vérifie le résultat du tir
char* checkShot(int i, int j) {
    for (int k = 0; k < AIRCRAFT_CARRIER; k++) {
        if (!IsAircraftCarrierSunk && positionAircraftCarrier[k][0] == i && positionAircraftCarrier[k][1] == j) { 
            positionAircraftCarrier[k][0] = -1; 
            positionAircraftCarrier[k][1] = -1;
            return "touche";
        }
    }
    for (int k = 0; k < FRIGATE; k++) {
        if (!IsFrigateSunk && positionFrigate[k][0] == i && positionFrigate[k][1] == j) { 
            positionFrigate[k][0] = -1; 
            positionFrigate[k][1] = -1;
            return "touche";
        }
    }
    return "rate";
}

// Vérifie si un bateau est coulé
bool isSunk(int size, bool* isSunk, int positionBoat[][2]) {
    int nbSectionTouched = 0;
    for (int i = 0; i < size; i++) {
        if (positionBoat[i][0] == -1 && positionBoat[i][1] == -1) {
            nbSectionTouched++;
        }
    }
    if(nbSectionTouched == size){
        *isSunk = true;
        return true;
    }
    else{
        return false;
    }
}

int main(int argc, char** argv) {
    struct sockaddr_in server; // creation de la strucuture du socket
    if (argc != 3) {
        fprintf(stderr,"usage: %s id port\n",argv[0]);
        exit(1);
    }
    id = argv[1];
    port = atoi(argv[2]);

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) { // creation de la socket en IPv4 "AF_INET" avec pour type "SOCK_STREAM"(tcp)
        fprintf(stderr,"%s: socket %s\n", argv[0], strerror(errno));
        exit(1);
    }

    server.sin_family = AF_INET; // type d'adressage IP
    server.sin_port = htons(port); // port d'ecoute
    server.sin_addr.s_addr = INADDR_ANY; // attachement à n'importe qu'elle adresse
    if (bind(sock, (struct sockaddr *)&server, sizeof(server)) < 0) { //le server associe le socket à une adresse IP et un port spécifique
        fprintf(stderr, "%s: bind %s\n", argv[0], strerror(errno));
        exit(1);
    }

    if (listen(sock, 5) != 0) { // le socket est mis en ecoute
        fprintf(stderr, "%s: listen %s\n", argv[0], strerror(errno));
        exit(1);
    }

    while (1) {
        struct sockaddr_in client;
        int len = sizeof(client);
        int sock_pipe;
        sock_pipe = accept(sock, (struct sockaddr *)&client, &len);
        // Crée un nouveau processus pour gérer chaque client
        int numPID = fork();

        // Processus enfant - Gérer la connexion et le jeu pour un client
        if(numPID == 0){
            int ret, nb_plays = 0;
            srand(time(NULL));
            placeBoat(FRIGATE, positionFrigate);
            placeBoat(AIRCRAFT_CARRIER, positionAircraftCarrier);
            do {
                nb_plays++;
                char buf_read[256], buf_write[256];
                ret = read(sock_pipe, buf_read, 256);
                if (ret <= 0) {
                    printf("%s: read=%d: %s\n", argv[0], ret, strerror(errno));
                    break;
                }

                int i, j;
                char *dash = strchr(buf_read, '-');
                if (dash != NULL) {
                    *dash = '\0';
                    i = atoi(buf_read);
                    j = atoi(dash + 1);
                }

                char* res = checkShot(i, j);
                if (IsAircraftCarrierSunk && IsFrigateSunk) {
                    res = "gagne";
                } else if (!IsAircraftCarrierSunk && isSunk(AIRCRAFT_CARRIER, &IsAircraftCarrierSunk, positionAircraftCarrier)) {
                    res = "porte-avion coule";
                    if (IsAircraftCarrierSunk && IsFrigateSunk) {
                        res = "gagne";
                    }
                } else if (!IsFrigateSunk && isSunk(FRIGATE, &IsFrigateSunk, positionFrigate)) {
                    res = "fregate coule";
                    if (IsAircraftCarrierSunk && IsFrigateSunk) {
                        res = "gagne";
                    }
                } else if (nb_plays == MAX_PLAYS) {
                    res = "perdu";
                }

                // Affichage des positions des bateaux et du résultat du tir au niveau du serveur
                printf("Porte-avion[i,j]: [%d, %d], [%d, %d], [%d, %d], [%d, %d], [%d, %d]\n"
                        "Fregate[i,j]: [%d, %d], [%d, %d], [%d, %d]\n",
                        positionAircraftCarrier[0][0], positionAircraftCarrier[0][1],
                        positionAircraftCarrier[1][0], positionAircraftCarrier[1][1],
                        positionAircraftCarrier[2][0], positionAircraftCarrier[2][1],
                        positionAircraftCarrier[3][0], positionAircraftCarrier[3][1],
                        positionAircraftCarrier[4][0], positionAircraftCarrier[4][1],
                        positionFrigate[0][0], positionFrigate[0][1],
                        positionFrigate[1][0], positionFrigate[1][1],
                        positionFrigate[2][0], positionFrigate[2][1]);

                sprintf(buf_write, "%s", res);
                ret = write(sock_pipe, buf_write, 256);
                if (ret <= 0) {
                    printf("%s: write=%d: %s\n", argv[0], ret, strerror(errno));
                    break;
                }
            } while (nb_plays < MAX_PLAYS && (!IsFrigateSunk || !IsAircraftCarrierSunk));

            close(sock_pipe);
            return 0;
        }
        else {
            // Processus parent - Continue d'accepter d'autres connexions
            close(sock_pipe); // Ferme la socket de dialogue pour ce client dans le parent
        }
    }
}