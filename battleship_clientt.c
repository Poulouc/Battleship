/*
Role : Fichier du client TCP du Battleship
Etudiants : Hugo Babin et Romain Guillon
*/

#include <stdio.h>
#include <errno.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdbool.h>
 
#define NBCHANGE 3   // Nombre d'échanges entre le client et le server
#define GRID_SIZE 20 // Taille de la grille

char* id=0;  // Identifiant du client
short sport=0;  // Port du server
int sock=0;	// Socket de communication
char grid[GRID_SIZE][GRID_SIZE]; // Grille du jeu

// Génération d'une grille
void generateGrid(char grid[GRID_SIZE][GRID_SIZE]){
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            grid[i][j] = '~'; // '~' = eau
        }
    }
}

// Actualisation de la grille en fonction de l'action
void gridMarking(char grid[GRID_SIZE][GRID_SIZE], int i, int j, char* isTouched){
    if (!strcmp(isTouched, "touche") || !strcmp(isTouched, "porte-avion coule") || !strcmp(isTouched, "fregate coule") && grid[i][j] == '~'){
        grid[i][j] = '*';  // Marque comme touché
    }
    else{
        grid[i][j] = 'X';  // Marque comme manqué
    }
}

// Fonction qui regarde si la partie est fini
bool endOfGame(char* isEnding)
{ 
    if(!strcmp(isEnding, "gagne"))
    { 
        printf("Vous avez gagné\n");
        return true;
    }
    else if(!strcmp(isEnding, "perdu"))
    {
        printf("Vous avez perdu\n");
        return true;
    }
    return false;
}

// Fonction d'affichage de la grille
void displayGrid(char grid[GRID_SIZE][GRID_SIZE]){
    printf("\n\n   ");
    for(int i = 0; i < GRID_SIZE; i++) {
        printf("%2d ", i);
    }
    printf("\n");
    for(int i = 0; i < GRID_SIZE; i++) {
        printf("%2d ", i);
        for(int j = 0; j < GRID_SIZE; j++) {
            printf(" %c ", grid[i][j]);
        }
        printf("\n");   
    }
}

int main(int argc, char** argv) {
    struct sockaddr_in me;    /* SAP du client (adresse du client) */
    struct sockaddr_in server; /* SAP du serveur (adresse du serveur) */
    int nb_question = 0;       /* Compteur du nombre de questions échangées */
    int ret, len;

    // Vérifier que le nombre d'arguments est correct (id, adresse du server et port)
    if (argc != 4) {
        fprintf(stderr, "usage: %s id serveur port\n", argv[0]);
        exit(1);
    }

    id = argv[1];               // Récupère l'ID du client
    sport = atoi(argv[3]);       // Convertit le port fourni en argument de chaîne en entier

    // Création de la socket pour la connexion TCP
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        fprintf(stderr, "%s: socket %s\n", argv[0], strerror(errno));
        exit(1);
    }

    // Configuration de l'adresse du serveur
    server.sin_family = AF_INET;        // Utilise IPv4
    server.sin_port = htons(sport);     // Définit le port du serveur (conversion en "network byte order")
    inet_aton(argv[2], &server.sin_addr); // Convertit l'adresse IP du server de la chaîne de caractères vers le format binaire

    // Connexion au serveur
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        fprintf(stderr, "%s: connect %s\n", argv[0], strerror(errno));
        perror("bind");
        exit(1);
    }

    // Récupération de la SAP locale (adresse du client après la connexion)
    len = sizeof(me);
    getsockname(sock, (struct sockaddr *)&me, &len);

    // Boucle principale de la partie
    int end = 1;
    char buf_read[256], buf_write[256]; 
    int i = -1, j = -1;
    generateGrid(grid);
    do
    {
        displayGrid(grid);

        if(i != -1 || j != -1){
            printf("%s\n", buf_read);
        }

        // Lecture des coordonnées i et j
        printf("Ecrivez i (0-%d) : ", GRID_SIZE-1);
        scanf("%d", &i);
        printf("Ecrivez j (0-%d) : ", GRID_SIZE-1);
        scanf("%d", &j);

        // Formatage du message à envoyer (i-j)
        snprintf(buf_write, sizeof(buf_write), "%d-%d", i, j);

        // Envoi des coordonnées au serveur
        send(sock, buf_write, 256, 0);

        // Réception de la réponse du serveur
        recv(sock, buf_read, 256, 0);

        // Marquer le tir sur la grille
        gridMarking(grid, i, j, buf_read);
    }while(!endOfGame(buf_read));

    // Fermeture de la socket après la fin des échanges
    close(sock);
    return 0;
}