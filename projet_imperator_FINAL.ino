/**************************************************************/
// Date de création du programme: 2 avril 2025
// Date de la dernière modification: à modifier
// Nom du programmeur principal: Votre nom
//
// But du programme: petit jeu dans lequel le joueur contrôle un
// petit vaiseau spatil à la droite de l'écran. Le vaisseau peut
// monter / descendre ou aller à gauche / droite. En appuyant sur
// la touche 'C' le vaisseau peut tirer des balles et détruire les
// vaisseau ennemis. Chaque fois que le jouer détruit un ennemi
// il gagne un point. Le joueur débute avec 5 vies et en perd une
// lorsqu'un vaisseau quitte l'écran vers la gaauche ou lorsqu'il
// y a une collision avec le joueur. Lorsque le joueur meurt son
// score est affiché et sauvegarder comme "high score" si son
// score est plus élevé que le précédent.
//
// Version: X.X
/**************************************************************/

/*██████*/
/* Lorsque vous voyez les caractères ci-desssus, cela signifie qu'il y a du code manquant
   à compléter.
*/

//*********************************************
//Inclusion des librairies nécessaire au projet
//*********************************************

#include <MOMO_RGB_Matrix.h> // Librairie pour contrôler la matrice
#include <Bounce2.h> // Librairie pour gérer le rebond et les fronts montants
#include <EEPROM.h> // Librairie pour enregistrer le high score dans la mémoire

MOMO_RGB_Matrix matrix;
//**************************************************************************************
//Création des DEFINE
//**************************************************************************************

#define DEBOUNCE_TIME 40 // Temps pour enlever le rebond des boutons
#define MAX_BALLES 5 // Nombre maximale de balles que le joueur peut tirer en même temps
#define VITESSE_BALLES 15 // Vitesse de déplacement des balles

#define LARGEUR_MATRICE 64 // Largeur en X de la matrice RGB
#define HAUTEUR_MATRICE 32 // Largeur en Y de la matrice RGB

#define HAUTEUR_INTERFACE 22 // Espace où débute l'interface du jeu

#define START_PLAYER_X 3 // Position en X de départ du joueur
#define START_PLAYER_Y 10 // Position en Y de départ du joueur
#define START_PLAYER_LIFE 5 // Nombre de vie avec lequel le joueur commence
#define MAX_X_PLAYER 20 // La coordonnée en X max où le joueur peut se déplacer. Sert à limiter le mouvement en X du joueur.

#define VITESSE_ENNEMIES 50 // Vitesse de mouvement des ennemis
#define MAX_ENNEMIES 15 // Nombre d'ennemis pouvant être présent en même temps à l'écran.
#define DEFAULT_SPAWN_SPEED 1000 // Temps entre chaque apparition d'ennemis
#define PLAYER_WIDTH 7 // Largeur du bitmap du joueur
#define PLAYER_HEIGHT 3 // Hauteur du bitmap du joueur
#define ENNEMY_WIDTH 6 // Largeur du bitmap du joueur
#define ENNEMY_HEIGHT 3 // Hauteur du bitmap du joueur

#define LEVEL_CHANGE 20 // Nombre de points requis pour changer de niveau
#define FACTEUR_AUGMENTATION 5 // Facteur d'augmentation du niveau de difficulté à chaque niveau

#define CHEATS_ON 1 // détermine si les codes de triches sont actifs.

#define BTN_UP 35 // bouton S1 (up) du contrôleur
Bounce btn_up = Bounce(); // Crée un objet Bounce

#define BTN_DOWN 30 // bouton S2 (down) du contrôleur
Bounce btn_down = Bounce(); // Crée un objet Bounce

#define BTN_LEFT 36 // bouton S3 (left) du contrôleur
Bounce btn_left = Bounce(); // Crée un objet Bounce

#define BTN_RIGHT 34 // bouton S4 (right) du contrôleur
Bounce btn_right = Bounce(); // Crée un objet Bounce

#define BTN_A 31 // bouton S6 (A) du contrôleur
Bounce btn_a = Bounce(); // Crée un objet Bounce

#define BTN_B 33 // bouton S7 (B) du contrôleur
Bounce btn_b = Bounce(); // Crée un objet Bounce

#define BTN_C 32 // bouton S8 (C) du contrôleur
Bounce btn_c = Bounce(); // Crée un objet Bounce

// Énumération des différents états possibles du jeu
enum Etat {
  ETAT_TITRE, // Affiche le titre du jeu et le high score
  ETAT_JEUX, // Affiche le jeu (ennemis, joueur, balles, score, vie, etc)
  ETAT_GAMEOVER, // Affiche la fin du jeu lorsque les vies du joueurs = 0. Affiche le level et le score du joueur
  ETAT_RESET, // Sert à remettre le jeu en état 0 pour pouvoir rejouer une partie après un game over
  ETAT_PRELUDE // État avant le Game Over pour afficher un message
};

// Structure pour représenter le joueur
struct Player
{
  int pos_x = 20; // Position en X du joueur
  int pos_y = 10; // Position en Y du joueur
  int vie = START_PLAYER_LIFE; // Nombre de vie du joueur
  int score = 0; // Score du joueur
  int level = 1; // Level auquel le joueur est rendu 0 = level 1
};

// Structure pour représenter les balles tirées par le joueur
struct Balles
{
  int pos_x = -1; // Position en X de la balle (initialisé hors écran)
  int pos_y = -1; // Position en Y de la balle (initialisé hors écran)
  bool actif = 0; // Si 1 la balle est visible à l'écran, 0 la balle est disponible et peut être tirée
};

// Structure pour représenter un ennemi
struct Ennemy
{
  int pos_x = 68; // Position de départ en X d'un ennemi. 68 est à l'extérieur de la matrice ce qui donne l'impression que l'ennemi
                  // arrive de l'extérieur de l'écran.
  int pos_y = 10; // Position de départ en Y de l'ennemi.
  bool actif = 0; // Si 1 l'ennemi est visible à l'écran, 0 l'ennemie est disponible et peut apparaître à l'écran
  unsigned long lastMoveTime = 0; // Sauvegarde le temps en ms où l'ennemi a bougé la dernière fois.
};

// Bitmap pour le joueur (représentation graphique)
bool bitmap_joueur[3][7] =
                            {1,1,1,0,1,1,1,
                             0,0,1,1,1,0,0,
                             1,1,1,0,1,1,1};

// Bitmap pour l'ennemi (représentation graphique)
bool bitmap_ennemy1[3][6] =
                             {0,0,1,0,1,1,
                              1,1,1,1,1,0,
                              0,0,1,0,1,1};

Player player; // Création de l'objet joueur
Ennemy ennemy[MAX_ENNEMIES]; // Création d'un tableau contenant les ennemies
Balles balles[MAX_BALLES]; // Création d'un tableau contenant les balles du joueurs
Etat etatActuel; // Variable pour stocker l'état actuel du jeu

// Fonction pour gérer les codes de triche
void cheatvie()
{
  static int CheatA; // Compteur pour le cheat de vie
  static int CheatB; // Compteur pour le cheat de reset du high score

  btn_a.update(); // Met à jour l'état du bouton A
  if(digitalRead(BTN_A) == LOW) // Si le bouton A est appuyé
  {
      CheatA++; // Incrémente le compteur
  }
  if(CheatA == 4) // Si le bouton A a été appuyé 4 fois rapidement
  {
    if(player.vie < 10) // Si la vie du joueur est inférieure à 10
    {
     CheatA = 0; // Réinitialise le compteur
     player.vie +=  10; // Ajoute 10 vies au joueur (cheat)
    }
  }
  btn_b.update(); // Met à jour l'état du bouton B
  if(digitalRead(BTN_B) == LOW) // Si le bouton B est appuyé
  {
    CheatB++; // Incrémente le compteur
  }
  if(CheatB == 4) // Si le bouton B a été appuyé 4 fois rapidement
  {
    matrix.setCursor(23, 25); // Positionne le curseur
    matrix.fillRect(23, 25, 36, 25, matrix.Color888(0, 0, 0)); // Efface la zone du high score
    resetHighScore(); // Réinitialise le high score
  }
}

// Fonction pour afficher un point du joueur à des coordonnées relatives
void affichage_point(int x, int y)
{
  matrix.drawPixel(player.pos_x + x, player.pos_y + y, matrix.Color888(128, 0, 128)); // Dessine un pixel violet
}

// Fonction pour afficher un point d'un ennemi à des coordonnées relatives
void affichage_point_ennemy(int x, int y, int num_ennemie)
{
  matrix.drawPixel(ennemy[num_ennemie].pos_x + x, ennemy[num_ennemie].pos_y + y, matrix.Color888(255, 128, 0)); // Dessine un pixel orange
}

// Fonction pour effacer un point du joueur à des coordonnées relatives (le met en noir)
void efface_point(int x, int y)
{
  matrix.drawPixel(player.pos_x + x, player.pos_y + y, matrix.Color888(0, 0, 0)); // Dessine un pixel noir
}

// Fonction pour effacer un point d'un ennemi à des coordonnées relatives (le met en noir)
void efface_point_ennemy(int x, int y, int num_ennemie)
{
  matrix.drawPixel(ennemy[num_ennemie].pos_x + x, ennemy[num_ennemie].pos_y + y, matrix.Color888(0, 0, 0)); // Dessine un pixel noir
}

// Fonction pour afficher le joueur à l'écran
void affiche_joueur()
{
  // Parcours la hauteur du joueur de 0 à PLAYER_HEIGHT (3)
  for(int i = 0; i < 3; i++)
  {
    // Parcours la largeur du joueur de 0 à PLAYER_WIDTH (7)
    for(int j = 0; j < 7; j++)
    {
      // Si le pixel dans le bitmap du joueur est à 1
      if(bitmap_joueur[i][j] == 1)
      {
        // On allume le pixel à la bonne position relative au joueur
       affichage_point(j,i);
      }
    }
  }
}

// Fonction pour effacer le joueur de l'écran (le met en noir)
void efface_joueur()
{
  // Parcours la hauteur du joueur de 0 à PLAYER_HEIGHT (3)
  for(int i = 0; i < 3; i++)
  {
    // Parcours la largeur du joueur de 0 à PLAYER_WIDTH (7)
    for(int j = 0; j < 7; j++)
    {
      // Si le pixel dans le bitmap du joueur est à 1
      if(bitmap_joueur[i][j] == 1)
      {
        // On éteint le pixel à la bonne position relative au joueur
        matrix.drawPixel(player.pos_x + j, player.pos_y + i, matrix.Color888(0, 0, 0));
      }
    }
  }
}

// Fonction pour afficher un ennemi à l'écran
void affiche_ennemies(int num_ennemie)
{
  // Parcours la hauteur de l'ennemi de 0 à ENNEMY_HEIGHT (3)
  for(int i = 0; i < 3; i++)
  {
    // Parcours la largeur de l'ennemi de 0 à ENNEMY_WIDTH (6)
    for(int j = 0; j < 6; j++)
    {
      // Si le pixel dans le bitmap de l'ennemi est à 1
      if(bitmap_ennemy1[i][j] == 1)
      {
        // On allume le pixel à la bonne position relative à l'ennemi
        affichage_point_ennemy(j, i, num_ennemie);
      }
    }
  }
}

// Fonction pour effacer un ennemi de l'écran (le met en noir)
void efface_ennemies(int num_ennemie)
{
  // Parcours la hauteur de l'ennemi de 0 à ENNEMY_HEIGHT (3)
  for(int i = 0; i < 3; i++)
  {
    // Parcours la largeur de l'ennemi de 0 à ENNEMY_WIDTH (6)
    for(int j = 0; j < 6; j++)
    {
      // Si le pixel dans le bitmap de l'ennemi est à 1
      if(bitmap_ennemy1[i][j] == 1)
      {
        // On éteint le pixel à la bonne position relative à l'ennemi
        efface_point_ennemy(j, i, num_ennemie);
      }
    }
  }
}

// Fonction pour faire bouger un ennemi
void bouge_ennemies(int num_ennemie)
{
  // Vérifie si suffisamment de temps s'est écoulé depuis le dernier mouvement de cet ennemi
  if((millis() - ennemy[num_ennemie].lastMoveTime) > VITESSE_ENNEMIES - check_level())
  {
    efface_ennemies(num_ennemie); // On efface l'ennemi à son ancienne position
    ennemy[num_ennemie].pos_x--; // On bouge l'ennemi vers la gauche de 1
    ennemy[num_ennemie].lastMoveTime = millis(); // On sauvegarde le temps actuel pour le prochain mouvement

    // Si l'ennemi dépasse l'écran par la gauche
    if(ennemy[num_ennemie].pos_x < 0 - ENNEMY_WIDTH)
    {
      ennemy[num_ennemie].actif = false; // L'ennemi est désactivé (considéré comme détruit)
      player.vie--; // Le joueur perd une vie.
    }
  }
}

// Fonction pour faire apparaître un nouvel ennemi
void spawn_ennemies(int num_ennemie)
{
  static unsigned long lastSpawn = 0; // Sauvegarde le temps du dernier spawn
  static int SpawnSpeed = DEFAULT_SPAWN_SPEED; // Vitesse pour l'apparition d'ennemies

  // Si le temps écoulé depuis le dernier spawn est supérieur à la vitesse de spawn avec un peu de hasard
  if((millis() - lastSpawn) > SpawnSpeed - random(10,31) * 10)
  {
    ennemy[num_ennemie].actif = true; // On active l'ennemi
    ennemy[num_ennemie].pos_x = 70; // Place l'ennemi hors écran à droite
    // On choisi une position en y au hasard dans la zone de spawn (au-dessus de l'interface)
    ennemy[num_ennemie].pos_y = random(0, HAUTEUR_MATRICE - (ENNEMY_HEIGHT + (HAUTEUR_MATRICE - HAUTEUR_INTERFACE)));
    affiche_ennemies(num_ennemie); // Affiche le nouvel ennemi
    lastSpawn = millis(); // Met à jour le temps du dernier spawn
  }
}

// Fonction principale pour gérer tous les ennemis
void gerer_ennemies()
{
  // On parcours le tableau des ennemis
  for(int i = 0; i < MAX_ENNEMIES; i++)
  {
    // Si l'ennemi est actif
    if(ennemy[i].actif == true)
    {
      bouge_ennemies(i); // Fait bouger l'ennemi
      check_kill(i); // Vérifie les collisions avec cet ennemi
      if(ennemy[i].actif == true) // Si l'ennemi n'a pas été tué par la collision
      {
        affiche_ennemies(i); // Affiche l'ennemi à sa nouvelle position
      }
    } else { // Si l'ennemi est inactif
      spawn_ennemies(i); // On essaye de faire apparaître un nouvel ennemi à cette position libre
    }
  }
}

// Fonction pour vérifier le niveau actuel et ajuster la difficulté
int check_level()
{
  static int oldScore = player.score; // Sauvegarde l'ancien score pour détecter un changement de niveau
  static bool changement_possible = true; // Indique si un changement de niveau est possible (évite les changements multiples pour le même score)

  // Si le score est un multiple de LEVEL_CHANGE, supérieur à 0, et qu'un changement est possible
  if(player.score % LEVEL_CHANGE == 0 && player.score > 0 && changement_possible == true)
  {
    player.level++; // Incrémente le niveau
    changement_possible = false; // Désactive le changement possible
    oldScore = player.score; // Met à jour l'ancien score
  }

  // Si le vieux score est différent du score actuel, on permet à nouveau le changement de niveau pour la prochaine fois
  if(oldScore != player.score)
  {
    changement_possible = true;
  }

  // On retourne un chiffre du niveau multiplier par le facteur d'augmentation
  // qui permettra d'accélérer la vitesse des ennemis.
  return player.level * FACTEUR_AUGMENTATION;
}

// Fonction pour vérifier les collisions entre les balles, les ennemis et le joueur
void check_kill(int num_ennemie)
{
  // Vérifie si une balle du joueur touche l'ennemi actuel
  for(int i = 0; i < ENNEMY_HEIGHT; i++) // Parcours la hauteur de l'ennemi
  {
    for(int j = 0; j < ENNEMY_WIDTH; j++) // Parcours la largeur de l'ennemi
    {
      if(bitmap_ennemy1[i][j] == 1) // Si le pixel de l'ennemi est actif
      {
        for(int k = 0; k < MAX_BALLES; k++) // Parcours toutes les balles
        {
          if(balles[k].actif == true) // Si la balle est active
          {
            // Vérifie si les coordonnées de la balle correspondent à celles du pixel de l'ennemi
            if(balles[k].pos_x == ennemy[num_ennemie].pos_x+j && balles[k].pos_y == ennemy[num_ennemie].pos_y+i)
            {
              matrix.drawPixel(balles[k].pos_x, balles[k].pos_y, matrix.Color888(0, 0, 0)); // Efface la balle
              balles[k].actif = false; // Désactive la balle

              efface_ennemies(num_ennemie); // Efface l'ennemi
              ennemy[num_ennemie].actif = false; // Désactive l'ennemi (il est détruit)
              player.score++; // Augmente le score du joueur

              // Si le score est un multiple de LEVEL_CHANGE, accorde une vie supplémentaire
              if(player.score % LEVEL_CHANGE == 0)
              {
                player.vie++;
              }

              return; // Sort de la fonction car l'ennemi a été touché
            }
          }
        }
      }
    }
  }

  // Vérifie si l'ennemi touche le joueur (collision entre les deux bitmaps)
  // C'est une vérification simple basée sur les boîtes englobantes des deux objets
  if(player.pos_x < ennemy[num_ennemie].pos_x + ENNEMY_WIDTH && player.pos_x + PLAYER_WIDTH > ennemy[num_ennemie].pos_x && player.pos_y
  < ennemy[num_ennemie].pos_y + ENNEMY_HEIGHT && player.pos_y + PLAYER_HEIGHT > ennemy[num_ennemie].pos_y)
  {
    efface_ennemies(num_ennemie); // Efface l'ennemi
    ennemy[num_ennemie].actif = false; // Désactive l'ennemi
    player.vie--; // Le joueur perd une vie
  }
}

// Fonction pour faire bouger le joueur (pour l'effet de titre)
void bouge_joueur()
{
  efface_joueur(); // Efface le joueur à son ancienne position
  player.pos_x++; // Déplace le joueur vers la droite
  affiche_joueur(); // Affiche le joueur à sa nouvelle position

  // Si le joueur sort de l'écran par la droite
  if(player.pos_x == 66)
  {
    player.pos_x = -9; // Le ramène à gauche de l'écran pour un effet de boucle
  }
}

// Fonction pour trouver l'index d'une balle inactive (libre)
int check_balle_libre()
{
  for(int i = 0; i < MAX_BALLES; i++)
  {
    if(balles[i].actif == 0) // Si la balle est inactive
    {
      return i; // Retourne l'index de cette balle
    }
  }
  return -1; // Aucune balle libre n'a été trouvée
}

// Fonction pour créer et faire bouger les balles du joueur
void affiche_tire_balle_joueur(bool create_balle)
{
  int balle_libre; // Variable pour stocker l'index d'une balle libre
  static unsigned long lastMoveTime = 0; // Sauvegarde le temps du dernier mouvement des balles

  // Si on veut créer une balle (appelé lorsque le joueur tire)
  if(create_balle == 1)
  {
    balle_libre = check_balle_libre(); // Trouve une balle libre

    // Si on a trouvé une balle libre
    if(balle_libre != -1)
    {
      // Calcule la position de départ de la balle (à la droite du joueur)
      balles[balle_libre].pos_x = player.pos_x + PLAYER_WIDTH;
      balles[balle_libre].pos_y = player.pos_y + 1; // Un peu au centre verticalement
      balles[balle_libre].actif = true; // Active la balle
      matrix.drawPixel(balles[balle_libre].pos_x, balles[balle_libre].pos_y , matrix.Color888(153, 0, 0)); // Affiche le pixel de la balle (rouge)
    }
  } else { // Sinon on bouge toutes les balles actives
    // Si suffisamment de temps s'est écoulé depuis le dernier mouvement des balles
    if((millis() - lastMoveTime) > VITESSE_BALLES)
    {
      for(int i = 0; i < MAX_BALLES; i++) // Parcours toutes les balles
      {
        // Si la balle est active
        if(balles[i].actif == true)
        {
          matrix.drawPixel(balles[i].pos_x, balles[i].pos_y, matrix.Color888(0, 0, 0)); // Efface la balle à son ancienne position
          balles[i].pos_x++; // Déplace la balle de 1 en x vers la droite
          matrix.drawPixel(balles[i].pos_x, balles[i].pos_y, matrix.Color888(153, 0, 0)); // Affiche la balle à sa nouvelle position

          // Si la balle quitte l'écran par la droite.
          if(balles[i].pos_x >= LARGEUR_MATRICE)
          {
            balles[i].actif = false; // Désactive la balle (elle disparaît)
          }
        }
      }
      lastMoveTime = millis(); // Met à jour le temps du dernier mouvement des balles
    }
  }
}

// Fonction pour gérer le tir de balle du joueur
void gere_balle()
{
  static bool bouton_precedent = HIGH; // État précédent du bouton C
  static bool tire = true; // Permet de tirer si vrai (pour éviter le tir continu en maintenant le bouton)

  bool bouton_actuel = digitalRead(BTN_C); // Lit l'état actuel du bouton C

  // Si le bouton est passé de non appuyé (HIGH) à appuyé (LOW) et que le tir est possible
  if (bouton_precedent == HIGH && bouton_actuel == LOW && tire) {
    affiche_tire_balle_joueur(true); // Crée une nouvelle balle
    tire = false; // Désactive la possibilité de tirer tant que le bouton est maintenu
  }

  affiche_tire_balle_joueur(false); // Fait bouger les balles actives

  // Si le bouton n'est plus appuyé, permet à nouveau de tirer
  if (bouton_actuel == HIGH) {
    tire = true;
  }
  bouton_precedent = bouton_actuel; // Met à jour l'état précédent du bouton
}

// Fonction pour afficher l'interface du jeu (vies et score)
void affiche_interface()
{
  static int oldLife = -1, oldScore = -1; // Sauvegarde les anciennes valeurs pour n'afficher que si elles changent

  if(player.vie != oldLife) // Si la vie a changé
  {
    matrix.drawLine(0,22,63,22, matrix.Color888(255, 255, 255)); // Dessine la ligne de séparation de l'interface
    matrix.setTextSize(1); // Taille du texte
    matrix.setTextWrap(false); // Pas de retour à la ligne automatique
    matrix.setCursor(5, 23); // Positionne le curseur pour la vie
    matrix.setTextColor(matrix.Color888(255, 215, 0)); // Couleur du texte (doré)
    matrix.fillRect(5, 23, 30, 8, matrix.Color888(0, 0, 0)); // Efface la zone de la vie
    matrix.setCursor(5, 23); // Repositionne le curseur
    matrix.print("V:"); // Affiche "V:" pour Vie
    matrix.setCursor(16,23); // Positionne le curseur pour le nombre de vies
    matrix.print(player.vie); // Affiche le nombre de vies
    matrix.setTextSize(1); // Remet la taille du texte (par précaution)
    matrix.setTextWrap(false); // Remet le retour à la ligne (par précaution)
    oldLife = player.vie; // Met à jour l'ancienne vie

    if(player.vie == 0) // Si la vie atteint 0
    {
      player.vie = 0; // S'assure qu'elle ne descende pas en dessous de 0 (utile pour l'affichage)
    }
  }

  // Si le score a changé
  if(player.score != oldScore)
  {
    matrix.fillRect(35, 23, 28, 8, matrix.Color888(0, 0, 0)); // Efface la zone du score
    matrix.setTextSize(1); // Taille du texte
    matrix.setCursor(35, 23); // Positionne le curseur pour le score
    matrix.setTextColor(matrix.Color888(255, 215, 0)); // Couleur du texte (doré)
    matrix.print("P:"); // Affiche "P:" pour Points
    matrix.print(player.score); // Affiche le score

    oldScore = player.score; // Met à jour l'ancien score
  }
}

// Fonction pour afficher l'écran titre
void titre()
{
  cheatvie(); // Vérifie les codes de triche (même sur l'écran titre)

  matrix.setTextSize(1); // Taille du texte
  matrix.setTextWrap(false); // Pas de retour à la ligne automatique
  matrix.setCursor(5, 1); // Positionne le curseur
  matrix.setTextColor(matrix.Color888(255, 128, 0)); // Couleur du texte (orange)
  matrix.println("Imperator"); // Affiche le titre du jeu

  // Dessine un cadre autour du titre
  matrix.drawLine(0,0,0,8, matrix.Color888(255, 255, 255));
  matrix.drawLine(0,14,0,32, matrix.Color888(255, 255, 255));
  matrix.drawLine(0,0,64,0, matrix.Color888(255, 255, 255));
  matrix.drawLine(0,31,63,31, matrix.Color888(255, 255, 255));
  matrix.drawLine(63,0,63,8, matrix.Color888(255, 255, 255));
  matrix.drawLine(63,14,63,31, matrix.Color888(255, 255, 255));
  matrix.drawLine(0, 9, 63, 9, matrix.Color888(255, 51, 51));
  matrix.drawLine(0, 13, 63, 13, matrix.Color888(255, 51, 51));

  matrix.setCursor(2, 15); // Positionne le curseur
  matrix.setTextColor(matrix.Color888(255, 215, 0)); // Couleur du texte (doré)
  matrix.println("High Score"); // Affiche "High Score"

  matrix.setCursor(29, 23); // Positionne le curseur pour le high score
  matrix.setTextColor(matrix.Color888(255, 51, 153)); // Couleur du texte (rose)
  matrix.println(lireHighScore()); // Affiche le high score enregistré

  bouge_joueur(); // Fait bouger le joueur pour l'effet d'animation sur l'écran titre

  // Si les boutons HAUT ou BAS sont appuyés, on passe à l'état de jeu
  if(digitalRead(BTN_UP) == LOW || digitalRead(BTN_DOWN) == LOW)
  {
    etatActuel = ETAT_JEUX; // Change l'état vers le jeu
    player.pos_x = START_PLAYER_X; // Réinitialise la position X du joueur
    player.pos_y = START_PLAYER_Y; // Réinitialise la position Y du joueur
    efface_ecran(); // Efface l'écran pour commencer le jeu
  }
}

// Fonction pour afficher l'écran de fin de partie (Game Over)
void gameOver()
{
    matrix.setTextSize(1); // Taille du texte
    matrix.setTextColor(matrix.Color888(255, 0, 0)); // Couleur du texte (rouge)
    matrix.setCursor(15, 0); // Positionne le curseur
    matrix.print("DEATH!"); // Affiche "DEATH!"

    matrix.setTextColor(matrix.Color888(255, 255, 255)); // Couleur du texte (blanc)
    matrix.setCursor(10, 8); // Positionne le curseur
    matrix.print("Level "); // Affiche "Level "
    matrix.print(player.level); // Affiche le niveau atteint par le joueur

    matrix.setTextColor(matrix.Color888(255, 255, 255)); // Couleur du texte (blanc)
    matrix.setCursor(10, 16); // Positionne le curseur
    matrix.print("Score:"); // Affiche "Score:"
    matrix.print(player.score); // Affiche le score du joueur

    // Vérifie si le score actuel est supérieur au high score enregistré
    if(player.score > lireHighScore())
    {
      ecrireHighScore(player.score); // Enregistre le nouveau high score
    }

    // Si les boutons A, B et C sont appuyés simultanément, réinitialise le jeu
    if(digitalRead(BTN_A) == LOW && digitalRead(BTN_B) == LOW && digitalRead(BTN_C) == LOW)
    {
      etatActuel = ETAT_RESET; // Passe à l'état de réinitialisation
      delay(100); // Petite pause pour éviter les doubles inputs
      resetGame(); // Réinitialise le jeu
    }
    delay(4000); // Attend 4 secondes sur l'écran Game Over
    prelude(); // Passe à l'état de prélude (affichage "MISSION FAILED")
}

// Fonction pour afficher le message "MISSION FAILED" avant le reset
void prelude()
{
    efface_ecran(); // Efface l'écran
    matrix.setTextSize(1); // Taille du texte
    matrix.setTextColor(matrix.Color888(255, 0, 0)); // Couleur du texte (rouge)
    matrix.setCursor(12, 5); // Positionne le curseur
    matrix.print("MISSION "); // Affiche "MISSION "
    matrix.setCursor(15, 15); // Positionne le curseur
    matrix.print("FAILED"); // Affiche "FAILED"
    delay(4000); // Attend 4 secondes
    resetGame(); // Réinitialise le jeu
}

// Fonction pour réinitialiser l'état du

_**Note:** Response was truncated due to length limits._
