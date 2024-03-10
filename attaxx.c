#include<MLV/MLV_all.h>
#include <stdio.h>
#include <stdio_ext.h>
#include <stdlib.h>
#include <time.h>

#define TAILLE_PLATEAU 7
#define TAILLE_MAX_NOM 20
#define TAILLE_FENETRE 720 // doit etre un multiple de 9 pour eviter un decalage
#define CASE TAILLE_FENETRE / (TAILLE_PLATEAU +2)

typedef struct {
    char nom[TAILLE_MAX_NOM];
    char symbole;
    int score;  
}Joueur;

typedef struct{
    char plateau[TAILLE_PLATEAU + 2 ][TAILLE_PLATEAU + 2];
    int pixel_plateau_x[TAILLE_PLATEAU];
    int pixel_plateau_y[TAILLE_PLATEAU];
    Joueur * joueurs[2];
} Plateau;

int estValide(char a){
    /*
    La fonction prend en argument un caractère et elle verifie s'il fait parti des options valides 
    pour lancer le programme. 
    Elle retourne 0 si l'argument n'existe pas et 1 sinon.
    */
    return (a == 'a' || a == 'g' || a == 'h' || a == 'o' || a == 'p' || a == 's');
}
int argumentsValide(int argc, char* argv[], char arg[]){
    /*
    La fonction prend en argument un entier qui correspond au nombre d'argument qui
    sont dans le tableau de chaine de caractère argv. Le dernier argument est un tableau 
    de caractère qui contiendra les arguments s'ils sont valide.
    La fonction retourne 0 si les arguments donné ne correspondent pas a des versions valide
    du jeu 1 sinon.
    */
    if (argc < 3 || argc > 4){
        return 0;
    }

    for(int i=1; i< argc; i++){
        if (argv[i][0] == '-'){
            if (estValide(argv[i][1])){
                arg[i-1] = argv[i][1];
            }else{
                return 0;
            }
        }
    }
    return 1;
}
int versionValide(char arg[2]){
    /*
    La fonction verifie si les commandes qui ont été donnée peuvent
    fonctionner ensemble.
    Elle prend en argument un tableau de caractère, et renvoie 1 si la
    version est valide 0 sinon.
    */
    char a1, a2;
    
    if (arg[0] > arg[1]){ // si les arguments ont ete donné dans le mauvais sens 
        a1 = arg[0];
        arg[0] = arg[1];
        arg[1] = a1;
    }

    a1 = arg[0];
    a2 = arg[1];
    if(a1 == 'a' && (a2 == 'h' || a2 == 'o')){
        return 1;
    }
    if(a1 == 'g' && (a2 == 'h' || a2 == 'o')){
        return 1;
    }
    return 0;
}
int versionJeu(char arg[], int nb_arg, char* argv[]){
    /*
    La fonction verifie la version du jeu.
    Elle prend en argument un tableau de caractère qui stoquera les arguments
    un nombre d'argument donne et un tableau les contenant.
    Elle retourne 0 si la version n'est pas valide, sinon elle renvoie:
    1 si la version est ASCII avec 2 joueurs
    2 si la version est ASCII avec un joueur
    3 si la version est graphique avec 2 joueurs
    4 si la version est graphique avec un joueur.
    */
    if(! argumentsValide(nb_arg, argv, arg)){
        return 0;
    }
    if (! versionValide(arg)){
        return 0;
    }

    if (arg[0] == 'a'){
        if (arg[1] == 'h'){
            return 1;
        }
        return 2;
    }
    if (arg[1] == 'h'){
        return 3;
    }
    return 4;
}
void initialiseJoueur(Joueur* j, int version, int num){
    /*
    La fonction innitialise les joueurs. 
    Elle prend l'adresse d'un joueur le numero de la version dans laquelle il joue
    et son numéro (1 ou 2).
    Elle ne retourne rien.
    */
    if (version == 1 || version == 3){
        if (num == 1){
            printf("Quel est le nom du premier joueur (symbole o) : "); 
            scanf("%s", j->nom);
            j->symbole = 'o';
        }else{
            printf("Quel est le nom du second joueur (symbole x) : ");
            scanf("%s", j->nom);
            j->symbole ='x';
        } 
    }else{
        char r[11] = {'o', 'r', 'd', 'i', 'n', 'a', 't', 'e', 'u', 'r', '\0'};
        for(int i=0; i<11; i++){
            j->nom[i] = r[i];
        }
        j->symbole ='x';
    }

    j->score = 2;
}
void initialiseTab(Plateau* p, int zones){
    /*
    La fonction prend en argument un plateau et l'initialise
    elle place des etoiles sur le tour et les premiers pions.
    Si l'option avec les cases pièges est activée, la fonction
    place des # pour les cases pièges.
    Elle ne retourne rien.
    */
    int zones_aleatoires[5] = {7,11,13,17,19}; // liste nombre premiers pour alétoire  + pertinents 
    int cpt = zones_aleatoires[random()%4]; 
    for (int i=0; i<TAILLE_PLATEAU+2; i++){
        for(int j =0; j<TAILLE_PLATEAU+2; j++){
            cpt++;
            if (i==0 || i == TAILLE_PLATEAU+1){
                p->plateau[i][j] = '*';
            }else if(j== 0 || j == TAILLE_PLATEAU+1){
                p->plateau[i][j] = '*';
            }else if(zones ==  1 && cpt % 7 == 0){
                p->plateau[i][j] = '#';
            }
            else{
                p->plateau[i][j] = '.';
            }
        }
    }

    p->plateau[1][1] = 'x';
    p->plateau[TAILLE_PLATEAU][TAILLE_PLATEAU] = 'x';
    p->plateau[1][TAILLE_PLATEAU] = 'o';
    p->plateau[TAILLE_PLATEAU][1] = 'o';
}

void affichePlateau(Plateau* p){
    /*
    La fonction affiche le plateau donné en parametre dans le terminal.
    Elle ne renvoie rien.
    */
    for(int i =0; i<TAILLE_PLATEAU+2; i++){
        for(int j = 0; j< TAILLE_PLATEAU+2; j++){
            printf("%c ", p->plateau[i][j]);
        }
        printf("\n");
    }   
}
void initialisation(Plateau* p, Joueur* j1, Joueur* j2, int version, int zones){
    /*
    La fonction initialise le jeu. Elle prend l'adresse d'un plateau, de deux joueurs
    et un numero de version.
    Elle ne retourne rien.
    */
    initialiseJoueur(j1, 1, 1);
    initialiseJoueur(j2, version, 2);
    p->joueurs[0] = j1;
    p->joueurs[1] = j2;

    initialiseTab(p, zones);
}
int mouvValide(Plateau* p, int num, int x, int y){
    /*
    La fonction prend en argument l'adresse d'un plateau, le numéro du joueur
    qui joue et les coordonnées de la position ou il veut jouer.
    Elle retourne 1 si le mouvement est possible et 0 sinon.
    */
    char symb;

    if (p->joueurs[num]->symbole == 'o'){
        symb = 'x';
    } else {
         symb = 'o';
    }

    if (p->plateau[x][y] == '#'){
        return 0;
    }
    if (p->plateau[x][y] != '.'){
        return 0;
    }else{
        if (p->plateau[x-1][y-1] == symb){
            return 1;
        }
        if (p->plateau[x-1][y] == symb){
            return 1;
        }
        if (p->plateau[x-1][y+1] == symb){
            return 1;
        }
        if (p->plateau[x][y-1] == symb){
            return 1;
        }
        if (p->plateau[x][y+1] == symb){
            return 1;
        }
        if (p->plateau[x+1][y-1] == symb){
            return 1;
        }
        if (p->plateau[x+1][y] == symb){
            return 1;
        }
        if (p->plateau[x+1][y+1] == symb){
            return 1;
        }
        return 0;
    }
}
int retourne(){
    /*
    La fonction ne prend pas d'argument.
    Elle renvoie 1 si la on doit retourner le pion
    0 sinon.
    */
    int max = 100;
    int proba= 75;
    int resultat;
    resultat = random() % max;
    if (resultat <= proba){
        return 1;
    }
    return 0;
}
void ajouteMouv(Plateau* p, int num, int x, int y, int proba){
    /*
    La fonction prend en argument l'adresse d'un plateau, le numero du joueur, les coordonnées 
    auxquelles il veut jouer et un entier proba (0 ou 1) pour savoir s'il joue avec les probabilités
    Elle ajoute son mouvement au plateau de jeu et met ajour les scores.
    Elle ne retourne rien.
    */
    int tx, ty, cmpt1=1;
    int cmpt2, num2;
    char symb1, symb2;
    char voisines[8][2];

    voisines[0][0] = x-1;
    voisines[0][1] = y-1;
    
    voisines[1][0] = x-1;
    voisines[1][1] = y;

    voisines[2][0] = x-1;
    voisines[2][1] = y+1;

    voisines[3][0] = x;
    voisines[3][1] = y-1;

    voisines[4][0] = x;
    voisines[4][1] = y+1;

    voisines[5][0] = x+1;
    voisines[5][1] = y-1;

    voisines[6][0] = x+1;
    voisines[6][1] = y;

    voisines[7][0] = x+1;
    voisines[7][1] = y+1;

    symb1 = p->joueurs[num]->symbole;

    if (symb1 == 'o'){
        symb2 = 'x';
        cmpt2 = p->joueurs[1]->score;
        num2 =1;
    }else {
        symb2 = 'o';
        cmpt2 = p->joueurs[0]->score;
        num2=0;
    }

    p->plateau[x][y] = symb1;
    if (!proba){ // on ne joue pas avec les probabilités
        for (int i =0; i< 8; i++){
        tx = voisines[i][0];
        ty = voisines[i][1];
            if (p->plateau[tx][ty] == symb2){
                p->plateau[tx][ty] = symb1; 
                cmpt1++;
                cmpt2--;
            }
        }
    }else{
        for (int i =0; i< 8; i++){
        tx = voisines[i][0];
        ty = voisines[i][1];
            if (p->plateau[tx][ty] == symb2){
                if (retourne()){
                    p->plateau[tx][ty] = symb1;
                    cmpt1++;
                    cmpt2--;
                }
            }
        }
    }
    p->joueurs[num]->score = p->joueurs[num]->score + cmpt1;
    p->joueurs[num2]->score = cmpt2;
}
void saisieMouv(Plateau* p, int num, int proba){
    /*
    La fonction prend en paramètre l'adresse d'un plateau et le numéro d'un joueur.
    Elle fait une saisie securisé des coordonné auquelles le joueur veut placer son pion.
    Elle ne retourne rien.
    */
    int x, y, secureScan;

    while(1){
        printf("%s(%c) veuillez saisir les coordonnées où jouer (entre 1 et %d) : ", p->joueurs[num]->nom, p->joueurs[num]->symbole, TAILLE_PLATEAU);
        secureScan = scanf("%d %d", &x, &y);
        if (secureScan < 2){
        __fpurge(stdin);/*Nettoie le flu d'entrée dans le cas ou l'utilisateur rentre un caractère au lieu d'un entier*/
        } 
        else{
            if (mouvValide(p, num, x, y)){
                ajouteMouv(p, num, x, y, proba);
                break;
            }
        }
    }
}
int caseVoisines(Plateau* p, int x, int y){
    /*
    La fonction prend en paramètre l'adresse d'un plateau, et les coordonnées d'une case.
    Elle compte les cases voisines a un 'x' (elle sert pour l'ordinateur qui est toujours joueur 2).
    Elle renvoie le nombre de voisines 'o' qu'il y a autour de la case de coordonnées (x, y).
    */
    char voisines[8][2];
    int cmpt=0;
    voisines[0][0] = x-1;
    voisines[0][1] = y-1;
    
    voisines[1][0] = x-1;
    voisines[1][1] = y;

    voisines[2][0] = x-1;
    voisines[2][1] = y+1;

    voisines[3][0] = x;
    voisines[3][1] = y-1;

    voisines[4][0] = x;
    voisines[4][1] = y+1;

    voisines[5][0] = x+1;
    voisines[5][1] = y-1;

    voisines[6][0] = x+1;
    voisines[6][1] = y;

    voisines[7][0] = x+1;
    voisines[7][1] = y+1;
    

    for (int i=1; i<8; i++){
        x = voisines[i][0];
        y = voisines[i][1];
        if (p->plateau[x][y] == 'o'){ // symbole de l'ordinateur toujours x
            cmpt++;
        }
    }
    return cmpt;
}
void ordiMouv(Plateau* p, int mouv[]){
    /*
    La fonction prend en paramètre l'adresse d'un plateau et un tableau d'entier.
    La fonction place dans mouv les coordonnées du meilleur coup pour l'ordinateur.
    Elle ne retourne rien.
    */
    int cmpt=0, temp;
    for(int i=1; i<TAILLE_PLATEAU+1; i++){
        for (int j=1; j<TAILLE_PLATEAU+1; j++){
            if (p->plateau[i][j] == '.'){
                temp = caseVoisines(p, i, j);// compte le nombre de points que l'ordinateur va gagner
                if(temp > cmpt){
                    cmpt = temp;
                    mouv[0] = i;
                    mouv[1] = j;
                }
            }
        }
    }   
}
void bougeOrdi(Plateau* p, int proba){
    /*
    La fonction prend en paramètre l'adresse d'un plateau et un entier (0 ou 1) en
    fonction de s'il jou avec les probabilités.
    Elle va faire "bouger" le robot a la position ou il gagnera le plus de points.
    Elle ne retourne rien.
    */
    int mouv[2];
    ordiMouv(p, mouv);
    ajouteMouv(p, 1, mouv[0], mouv[1], proba);
}
void init_Pixel_vers_Case(Plateau* p){
    /*
    La fonction prend en paramètre l'adresse d'un plateau.
    Elle initialise les tableaux de pixels dans le plateau.
    Elle ne retourne rien.
    */
    for(int i = 0; i <TAILLE_PLATEAU; i++){
        // place les coordonnées des cases en pixel dans les tableaux
        p->pixel_plateau_x[i] = CASE + i*CASE;
        p->pixel_plateau_y[i] = CASE + i*CASE;
    }
}
void creeFenetre(){
    /*
    La fonction ne prend pas de paramètre et ne retourne rien.
    Elle cree la fenetre.
    */
    MLV_create_window("dm","dm", TAILLE_FENETRE, TAILLE_FENETRE);
    MLV_clear_window(MLV_COLOR_WHITE);
    MLV_actualise_window();
}
void dessineGrille(){
    /*
    La fonction ne prend pas de parametres et ne renvoie rien.
    Elle dessines les lignes qui forment le plateau de jeu.
    */
    for(int i=CASE; i<TAILLE_FENETRE - CASE + 10; i += CASE){
        MLV_draw_line(i, CASE, i, CASE * (TAILLE_PLATEAU + 1), MLV_COLOR_BLACK);
        MLV_draw_line(CASE, i, CASE * (TAILLE_PLATEAU + 1), i, MLV_COLOR_BLACK); 
    }
}
int graphic_saisie_point(Plateau* p, int num, int point[]){
    /*
    La fonction prend en paramètre l'adresse d'un plateau, le numero d'un joueur
    et un tableau d'entien.
    La fonction place dans le tableau les coordonnées de la case dans laquelle le joueur numero 
    veut jouer.
    Elle retourne 1 s'il peut jouer et 0 sinon.
    */
    int a,b;
    MLV_wait_mouse(&b, &a);
    a /= CASE;
    b /= CASE;
    if (mouvValide(p, num, a, b)){
        point[0] = a;
        point[1] = b;
        return 1;
    }
    return 0;
}
void dessinePion(int x, int y, char couleur){
    /*
    La fonction prend en paramètre les coordonées d'une case et un charactère qui correspond 
    a la couleur du joueur.
    Elle dessine le cercle qui represente le pion du joueur.
    Elle ne retourne rien.
    */
    int rayon = CASE / 2;
    if (couleur == 'b'){//blanc
        MLV_draw_circle(x, y,rayon, MLV_COLOR_BLACK);
    }else{
        MLV_draw_filled_circle(x, y,rayon, MLV_COLOR_BLACK);
    }
    
}
void dessinePiege(int x, int y){
    /*
    La fonction prend en paramètre les coordonées d'une case.
    Elle dessine une croix de couleur rouge qui représente une case "trou".
    Le joueur ne pourra donc pas jouer sur cette case.
    Elle ne retourne rien
    */
    MLV_draw_line(x , y, x + CASE, y + CASE, MLV_COLOR_RED);
    MLV_draw_line(x, y + CASE, x + CASE, y, MLV_COLOR_RED);
}
void graphic_plateau(Plateau* p){
    /*
    La fonction prend en argument l'adresse d'un plateau et ne renvoie rien.
    Elle permet de dessiner les pions sur le plateau de jeu.
    */
    int a,b;
    for(int i = 1 ; i <= TAILLE_PLATEAU; i++){
        for(int j = 1; j <= TAILLE_PLATEAU; j++){
            a = p->pixel_plateau_x[j-1];
            b = p->pixel_plateau_y[i-1];
            if (p->plateau[i][j] == 'x'){
                dessinePion(a + CASE/2, b + CASE/2,'n');
            }else if (p->plateau[i][j] =='o'){
                dessinePion(a + CASE/2, b + CASE/2,'b');
            }else if (p->plateau[i][j] == '#'){
                dessinePiege(a,b);
            }
        }
    }
}
void graphic_saisie_mouv(Plateau* p, int num, int proba){
    /*
    La fonction prend en paramètre l'adresse d'un plateau, un entier qui correspond au numero du joueur
    un entier (0 ou 1) en fonction de s'il jou avec les probabilités.
    Elle ne renvoie rien.
    Elle permet la saisie d'un mouvement dans la fenetre.
    */
    int points[2];
    while(1){
        if (graphic_saisie_point(p, num, points)){
            ajouteMouv(p, num, points[0], points[1], proba);
            break;
        }
    }
}
void afficheScore(Plateau* p){
    /*
    La fonction affiche le score dans la fenetre, elle prend en paramètre l'adresse d'un plateeau et 
    ne retourne rien.
    */
    char score[TAILLE_MAX_NOM*3];
    int taille_boite = 300;
    int a = TAILLE_FENETRE/2 - taille_boite/2;


    sprintf(score, "%s : %d - %d : %s", p->joueurs[0]->nom, p->joueurs[0]->score, p->joueurs[1]->score, p->joueurs[1]->nom);
    MLV_draw_text_box(a, 10, taille_boite, 20, score,
                     10,MLV_COLOR_WHITE, MLV_COLOR_BLACK, MLV_COLOR_WHITE, MLV_TEXT_LEFT ,MLV_HORIZONTAL_CENTER, MLV_VERTICAL_CENTER);

}
void actualiseFenetre(Plateau* p){
    /*
    La fonction prend en paramètre l'adresse d'un plateau et ne retourne rien.
    Elle met a jour la fenetre.
    */
    MLV_clear_window(MLV_COLOR_WHITE);
    dessineGrille();
    graphic_plateau(p);
    afficheScore(p); 
    MLV_actualise_window();
}
int fin(Plateau* p){
    /*
    La fonction prend en paramètre l'adresse d'un plateau et renvoie 0 si la partie n'est pas terminé, 
    et 1 sinon.
    */
    if (p->joueurs[0]->score == 0 || p->joueurs[1]->score == 0){
        return 1;
    }
    for (int i =1; i<TAILLE_PLATEAU+1; i++){
        for(int j =1; j < TAILLE_PLATEAU+1; j++){
            if (p->plateau[i][j] == '.'){
                return 0;
            }
        }
    }
    return 1;
}
void boucle1(Plateau* p, int scores[], int proba){
    /*
    La fonction prend en paramètre l'adresse d'un plateau, un tableau d'entier et un entier (0 ou 1) en
    fonction de s'il jou avec les probabilités. Elle ne renvoie rien.
    C'est la boucle de jeu pour la première version (ASCII avec 2 joueurs).
    */
    int tour = 0;
    affichePlateau(p);
    while(1){
        saisieMouv(p, tour%2, proba);
        affichePlateau(p);
        
        printf("Score actuel : %s : %d, %s : %d\n\n", p->joueurs[0]->nom, p->joueurs[0]->score, p->joueurs[1]->nom, p->joueurs[1]->score);
        tour++;
        if (fin(p)){
            scores[0] = p->joueurs[0]->score;
            scores[1] = p->joueurs[1]->score;
            affichePlateau(p);
            break;
        }
    }
}
void boucle2(Plateau* p, int scores[], int proba){
    /*
    La fonction prend en paramètre l'adresse d'un plateau, un tableau d'entier et un entier (0 ou 1) en
    fonction de s'il jou avec les probabilités. Elle ne renvoie rien.
    C'est la boucle de jeu pour la deuxieme version (ASCII avec l'ordinateur).
    */
    int tour = 0;
    affichePlateau(p);
    while(1){
        affichePlateau(p);
        if (tour%2 == 0){
            saisieMouv(p, 0, proba);
        }else{
            bougeOrdi(p, proba);
        }
        affichePlateau(p);
        printf("Score actuel : %s(%c) %d - %s(%c) %d\n\n", p->joueurs[0]->nom, p->joueurs[0]->symbole, p->joueurs[0]->score, p->joueurs[1]->nom, p->joueurs[1]->symbole, p->joueurs[1]->score);
        tour++;
        if (fin(p)){
            scores[0] = p->joueurs[0]->score;
            scores[1] = p->joueurs[1]->score;
            affichePlateau(p);
            break;
        }
    }
}
void boucle3(Plateau*p, int scores[], int proba){
     /*
    La fonction prend en paramètre l'adresse d'un plateau, un tableau d'entier et un entier (0 ou 1) en
    fonction de s'il jou avec les probabilités. Elle ne renvoie rien.
    C'est la boucle de jeu pour la troisieme version (graphique avec 2 joueurs).
    */
    int tour = 0;
    init_Pixel_vers_Case(p);
    creeFenetre();
    while(1){
        actualiseFenetre(p);
        graphic_saisie_mouv(p, tour%2, proba);
        tour++;
        if (fin(p)){
            scores[0] = p->joueurs[0]->score;
            scores[1] = p->joueurs[1]->score;
            actualiseFenetre(p);
            MLV_wait_seconds(1);
            break;
        }
    }
}
void boucle4(Plateau* p, int scores[], int proba){
    /*
    La fonction prend en paramètre l'adresse d'un plateau, un tableau d'entier et un entier (0 ou 1) en
    fonction de s'il jou avec les probabilités. Elle ne renvoie rien.
    C'est la boucle de jeu pour la quatrième version (graphique avec l'ordinateur).
    */
    int tour=0;
    init_Pixel_vers_Case(p);
    creeFenetre();
    while(1){
        actualiseFenetre(p);
        if (tour%2 == 0){
            graphic_saisie_mouv(p, tour%2, proba);
        }else{
            MLV_wait_milliseconds(300);
            bougeOrdi(p, proba);
        }
        tour++;
        if (fin(p)){
            scores[0] = p->joueurs[0]->score;
            scores[1] = p->joueurs[1]->score;
            actualiseFenetre(p);
            MLV_wait_seconds(1);
            break;
        }
    }
}
int victoire(int un, int deux){
    /*
    La fonction prend en paramètre deuc entier et renvoie
    0 si c'est le premier score qui est le plus grand
    1 si c'est le deuxième score qui est plus grand
    3 s'il sont egaux.
    */
    if (un > deux){
        return 0;
    }
    if (deux > un){
        return 1;
    }
    return 3;
}
void vainqueur(Plateau* p, int scores[], int robot){
    /*
    La fonction prend en parametre l'adresse d'un plateaun, un tableau d'entier et un entier (0 ou 1).
    Elle affiche dans le terminal le vainqueur.
    Elle ne retourne rien
    */
    int resultat;
    resultat = victoire(scores[0], scores[1]); // score du joueur 1 et score du joueur 2
    if (resultat == 0){ // joueur 1 qui a gagné
        printf("Bravo %s, vous avez gagné %d à %d.\n", p->joueurs[resultat]->nom, p->joueurs[resultat]->score, p->joueurs[1]->score);
    }else if(resultat == 1){ // joueur 2 vainqueur
        if (robot){ // s'il joue contre le robot
            printf("Désolé %s, vous avez perdu %d à %d.\n", p->joueurs[0]->nom, p->joueurs[0]->score, p->joueurs[1]->score);
        }else{
            printf("Bravo %s, vous avez gagné %d à %d.\n", p->joueurs[resultat]->nom, p->joueurs[resultat]->score, p->joueurs[0]->score);
        }
        
    }else{
        printf("Égalité, %d à %d.\n", p->joueurs[0]->score, p->joueurs[1]->score);
    }
}
void affiche_fin(char message[]){
    /*
    La fonction prend en paramètre un tableau de caractères.
    Elle ecrit dans la fenetre le message de victoire.
    Elle ne renvoie rien.
    */
    MLV_clear_window(MLV_COLOR_WHITE);
    MLV_draw_text(TAILLE_FENETRE/4, TAILLE_FENETRE/2, message, MLV_COLOR_BLACK);
    MLV_actualise_window();
    MLV_wait_seconds(3);
    MLV_free_window();
}
void graphic_fin(Plateau* p, int scores[], int robot){
    /*
    La fonction prend en paramètre l'adresse d'un plateau, un tableau d'entier et un entier (0 ou 1).
    Elle permet d'afficher la fin dans la fenetre.
    Elle ne renvoie rien.
    */
    int resultat;
    resultat = victoire(scores[0], scores[1]);
    char message[TAILLE_MAX_NOM+100];

    if (resultat == 0){
        sprintf(message, "Bravo %s, vous avez gagné %d à %d.", p->joueurs[resultat]->nom, p->joueurs[resultat]->score, p->joueurs[1]->score);
    }else if(resultat == 1){
        if (robot){
            sprintf(message, "Désolé %s, vous avez perdu %d à %d.", p->joueurs[0]->nom, p->joueurs[0]->score, p->joueurs[1]->score);
        }else{
            sprintf(message, "Bravo %s, vous avez gagné %d à %d.", p->joueurs[resultat]->nom, p->joueurs[resultat]->score, p->joueurs[0]->score);
        }
    }else{
        printf(message, "Égalité, %d à %d.", p->joueurs[0]->score, p->joueurs[1]->score);
    }
    affiche_fin(message);
}

int partie(int argc, char* argv[]){
    /*
    La fonction prend en parametre un entier qui est le nombre d'argument dans le tableau de charactère
    argv.
    Elle renvoie 0 si la partie n'est pas valide et 1 si le joueur a choisit un mode valide.
    Elle premet l'execution des boucles.
    */
    Plateau p;
    Joueur j1, j2;
    char arg[3];
    int version, proba=0, zones=0;
    int scores[2];


    version = versionJeu(arg, argc, argv);
    if(!version){
        return 0;// n'a pas d'autre utilité que de sortir de la fonction la valeur ne sert a rien
    }
    if(argc == 4){
        proba = (arg[2] == 'p');// 1 si oui 0 sinon
        zones = (arg[2] == 's');
    }
    initialisation(&p, &j1, &j2, version, zones);
    switch(version){
        case 1: boucle1(&p, scores, proba); vainqueur(&p, scores, 0); break;
        case 2: boucle2(&p, scores, proba); vainqueur(&p, scores, 1); break;
        case 3: boucle3(&p, scores, proba); graphic_fin(&p, scores, 0); break;
        case 4: boucle4(&p, scores, proba); graphic_fin(&p, scores, 1); break;
    }
    return 1;// n'a pas d'autre utilité que de sortir de la fonction la valeur ne sert a rien
}


int main(int argc, char* argv[]){
    srandom(time(NULL));
    partie(argc, argv);
    return 0;
}