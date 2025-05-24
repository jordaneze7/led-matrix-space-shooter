
//*********************************************
//Inclusion des librairies nécessaire au projet
//*********************************************

#include <MOMO_RGB_Matrix.h> //Librairie pour contrôler la matrice
#include <Bounce2.h> //Librairie pour gérer le rebond et les fronts montants
#include <EEPROM.h> //Librairie pour enregistrer le high score dans la mémoire

MOMO_RGB_Matrix matrix;
//**************************************************************************************
//Création des DEFINE
//**************************************************************************************

#define DEBOUNCE_TIME 40 //Temps pour enlever le rebond des boutons
#define MAX_BALLES 5 //Nombre maximale de balles que le joueur peut tirer en même temps
#define VITESSE_BALLES 15 //Vitesse de déplacement des balles

#define LARGEUR_MATRICE 64 //Largeur en X de la matrice RGB
#define HAUTEUR_MATRICE 32 //Largeur en Y de la matrice RGB

#define HAUTEUR_INTERFACE 22 //Espace où débute l'interface du jeu

#define START_PLAYER_X 3 //Position en X de départ du joueur
#define START_PLAYER_Y 10 //Position en Y de départ du joueur
#define START_PLAYER_LIFE 5 //Nombre de vie avec lequel le joueur commence
#define MAX_X_PLAYER 20 //La coordonnée en X max où le joueur peut se déplacer. Sert à limiter le mouvement en X du joueur.

#define VITESSE_ENNEMIES 50 //Vitesse de mouvement des ennemis
#define MAX_ENNEMIES 15 //Nombre d'ennemis pouvant être présent en même temps à l'écran.
#define DEFAULT_SPAWN_SPEED 1000 //Temps entre chaque apparition d'ennemis
#define PLAYER_WIDTH 7 //Largeur du bitmap du joueur
#define PLAYER_HEIGHT 3 //Hauteur du bitmap du joueur
#define ENNEMY_WIDTH 6 //Largeur du bitmap du joueur
#define ENNEMY_HEIGHT 3 //Hauteur du bitmap du joueur

#define LEVEL_CHANGE 20 //Nombre de points requis pour changer de niveau
#define FACTEUR_AUGMENTATION 5 //Facteur d'augmentation du niveau de difficulté à chaque niveau

#define CHEATS_ON 1 //détermine si les codes de triches sont actifs.

#define BTN_UP 35 //bouton S1 (up) du contrôleur
Bounce btn_up = Bounce(); // Crée un objet Bounce

#define BTN_DOWN 30 //bouton S2 (down) du contrôleur
Bounce btn_down = Bounce(); // Crée un objet Bounce

#define BTN_LEFT 36 //bouton S3 (left) du contrôleur
Bounce btn_left = Bounce(); // Crée un objet Bounce

#define BTN_RIGHT 34 //bouton S4 (right) du contrôleur
Bounce btn_right = Bounce(); // Crée un objet Bounce

#define BTN_A 31 //bouton S6 (A) du contrôleur
Bounce btn_a = Bounce(); // Crée un objet Bounce

#define BTN_B 33 //bouton S7 (B) du contrôleur
Bounce btn_b = Bounce(); // Crée un objet Bounce

#define BTN_C 32 //bouton S8 (C) du contrôleur
Bounce btn_c = Bounce(); // Crée un objet Bounce

enum Etat {
  ETAT_TITRE, //Affiche le titre du jeu et le high score
  ETAT_JEUX, //Affiche le jeu (ennemis, joueur, balles, score, vie, etc)
  ETAT_GAMEOVER, //Affiche la fin du jeu lorsque les vies du joueurs = 0. Affiche le level et le score du joueur
  ETAT_RESET, //Sert à remettre le jeu en état 0 pour pouvoir rejouer une partie après un game over
  ETAT_PRELUDE
};

struct Player 
{
  int pos_x = 20; //Position en X du joueur
  int pos_y = 10; //Position en Y du joueur
  int vie = START_PLAYER_LIFE; //Nombre de vie du joueur
  int score = 0; //Score du joueur
  int level = 1; //Level auquel le joueur est rendu 0 = level 1
};

struct Balles
{
  int pos_x = -1; //Position en X de la balle
  int pos_y = -1; //Position en Y de la balle
  bool actif = 0; //Si 1 la balle est visible à l'écran, 0 la balle est disponible et peut être tirée
};

struct Ennemy 
{
  int pos_x = 68; //Position de départ en X d'un ennemi. 68 est à l'extérieur de la matrice ce qui donne l'impression que l'ennemi 
                  //arrive de l'extérieur de l'écran.
  int pos_y = 10; //Position de départ en Y de l'ennemi.
  bool actif = 0; //Si 1 l'ennemi est visible à l'écran, 0 l'ennemie est disponible et peut apparaître à l'écran
  unsigned long lastMoveTime = 0; //Sauvegarde le temps en ms où l'ennemi a bougé la dernière fois.
};



bool bitmap_joueur[3][7] =  
                            {1,1,1,0,1,1,1,
                             0,0,1,1,1,0,0,
                             1,1,1,0,1,1,1};

bool bitmap_ennemy1[3][6] =             
                             {0,0,1,0,1,1,
                              1,1,1,1,1,0,
                              0,0,1,0,1,1};

Player player;
Ennemy ennemy[MAX_ENNEMIES]; //Création d'un tableau contenant les ennemies 
Balles balles[MAX_BALLES]; //Création d'un tableau contenant les balles du joueurs
Etat etatActuel;

void cheatvie()
{

  static int CheatA;
  static int CheatB;

  btn_a.update();
  if(digitalRead(BTN_A) == LOW)
  {
  
      CheatA++;
  }
  if(CheatA == 4)
  {
    if(player.vie<10)
    {
     CheatA = 0;
     player.vie +=  10;
    }
  }
  btn_b.update();
  if(digitalRead(BTN_B) == LOW)
  {
    CheatB++;
  }
  if(CheatB == 4)
  {
    matrix.setCursor(23, 25);
    matrix.fillRect(23, 25, 36, 25, matrix.Color888(0, 0, 0));
    resetHighScore();
  }
}

void affichage_point(int x, int y)
{
  matrix.drawPixel(player.pos_x + x, player.pos_y + y, matrix.Color888(128, 0, 128));
}

void affichage_point_ennemy(int x, int y, int num_ennemie)
{
  matrix.drawPixel(ennemy[num_ennemie].pos_x + x, ennemy[num_ennemie].pos_y + y, matrix.Color888(255, 128, 0));
}

void efface_point(int x, int y)
{
  matrix.drawPixel(player.pos_x + x, player.pos_y + y, matrix.Color888(0, 0, 0)); 
}

void efface_point_ennemy(int x, int y, int num_ennemie)
{
  matrix.drawPixel(ennemy[num_ennemie].pos_x + x, ennemy[num_ennemie].pos_y + y, matrix.Color888(0, 0, 0)); 
}



void affiche_joueur()
{
  //Parcours la hauteur du joueur de 0 à PLAYER_HEIGHT (3)
  for(int i = 0; i < 3; i++)
  {
    //Parcours la largeur du joueur de 0 à PLAYER_WIDTH (3)
    for(int j = 0; j < 7; j++)
    {
      //Si on doit allumer le pixel
      if(bitmap_joueur[i][j] == 1)
      {
        //On allume le pixel à la bonne position.
       affichage_point(j,i);
      }
    }   
  }
}

void efface_joueur()
{
  //Parcours la hauteur du joueur de 0 à PLAYER_HEIGHT (3)
  for(int i = 0; i < 3; i++)
  {
    //Parcours la largeur du joueur de 0 à PLAYER_WIDTH (3)
    for(int j = 0; j < 7; j++)
    {
      //Si on doit éteindre le pixel
      if(bitmap_joueur[i][j] == 1)
      {
        //On éteint le pixel à la bonne position.
        matrix.drawPixel(player.pos_x + j, player.pos_y + i, matrix.Color888(0, 0, 0));
      }
    }   
  }
}

void affiche_ennemies(int num_ennemie)
{
  for(int i = 0; i < 3; i++)
  {
    for(int j = 0; j < 6; j++)
    {
      if(bitmap_ennemy1[i][j] == 1)
      {
        affichage_point_ennemy(j, i, num_ennemie); 
      }
    }   
  }
}

void efface_ennemies(int num_ennemie)
{
  for(int i = 0; i < 3; i++)
  {
    for(int j = 0; j < 6; j++)
    {
      if(bitmap_ennemy1[i][j] == 1)
      {
        efface_point_ennemy(j, i, num_ennemie); 
      }
    }   
  }
}

void bouge_ennemies(int num_ennemie)
{
     
  if((millis() - ennemy[num_ennemie].lastMoveTime) > VITESSE_ENNEMIES - check_level())
  {
    efface_ennemies(num_ennemie); //On efface l'ennemi
    ennemy[num_ennemie].pos_x--; //On bouge l'ennemi vers la droite de 1
    ennemy[num_ennemie].lastMoveTime = millis(); //On sauvegarde le temps pour éviter de bouger trop vite.

    //Si l'ennemie dépasse l'écran par la gauche
    if(ennemy[num_ennemie].pos_x < 0 - ENNEMY_WIDTH)
    {
      ennemy[num_ennemie].actif = false; //L'ennemi est tuer
      player.vie--; //Le joueur perd une vie.
    }
  }
}

void spawn_ennemies(int num_ennemie)
{
  static unsigned long lastSpawn = 0;
  static int SpawnSpeed = DEFAULT_SPAWN_SPEED; //Vitesse pour l'apparition d'ennemies

  //Si le temps entre le dernier spawn et vitesse de spawn avec une petit touche de hasard 
  if((millis() - lastSpawn) > SpawnSpeed - random(10,31) * 10)
  {
    ennemy[num_ennemie].actif = true; //On active un ennemie
    ennemy[num_ennemie].pos_x = 70; //affiche l'ennemi hors écran
    //On choisi une position en y au hasard dans la zone de spawn
    ennemy[num_ennemie].pos_y = random(0, HAUTEUR_MATRICE - (ENNEMY_HEIGHT + (HAUTEUR_MATRICE - HAUTEUR_INTERFACE)));
    affiche_ennemies(num_ennemie); //affiche l'ennemi
                                  //on bouge l'ennemi
    lastSpawn = millis();
  }
}

void gerer_ennemies()
{
  //On parcours le tableau des ennemis 
  for(int i = 0; i < MAX_ENNEMIES; i++)
  {
    //Si l'ennemi est actif
    if(ennemy[i].actif == true)
    {
      bouge_ennemies(i); //bouge l'ennemi
      check_kill(i); //verifie les collisions
      if(ennemy[i].actif == true)
      {
      affiche_ennemies(i); //affiche l'ennemi
      }
     //vérifie si l'ennemi est tuer
    } else { //Si l'ennemi est inactif
      spawn_ennemies(i); //On fait apparaître un ennemi.
    }
  }
}

int check_level()
{
  static int oldScore = player.score; //Sert à enregistrer le score pour changer de level seulement lorsqu'il y a un changement de niveau.
  static bool changement_possible = true; //Vrai = changement de niveau possible
  
  //Si le score est un multiple de LEVEL_CHANGE et que le changement est possible
  if(player.score % LEVEL_CHANGE == 0 && player.score > 0 && changement_possible == true)
  {
    player.level++;
    changement_possible = false;
    oldScore = player.score;
  }

  //Si le vieux score est différent du score actuel on permet a nouveau le changement de niveau.
  if(oldScore != player.score)
  {
    changement_possible = true;
  }

  //On retourne un chiffre du niveau multiplier par le facteur d'augmentation 
  //qui permettra d'accéléré la vitesse des ennemis.
  return player.level * FACTEUR_AUGMENTATION;
}

void check_kill(int num_ennemie)
{
  //Vérifie si une balle du joueur touche un ennemi
  for(int i = 0; i < ENNEMY_HEIGHT; i++)
  {
    for(int j = 0; j < ENNEMY_WIDTH; j++)
    {
        if(bitmap_ennemy1[i][j] == 1) //Si le pixel vérifié contient un 1
        {
          for(int k = 0; k < MAX_BALLES; k++)
          {
          if(balles[k].actif == true) //Si la balle est active
          {
            if(balles[k].pos_x == ennemy[num_ennemie].pos_x+j && balles[k].pos_y == ennemy[num_ennemie].pos_y+i)
            {
               matrix.drawPixel(balles[k].pos_x, balles[k].pos_y, matrix.Color888(0, 0, 0));
               balles[k].actif = false; //On efface la balle si elle touche l'ennemi

               efface_ennemies(num_ennemie); //On efface l'ennemi
              ennemy[num_ennemie].actif = false; //On tue l'ennemi
               player.score++; //On augmente le score de 1
              //Si le score est un multiple de LEVEL_CHANGE on accorde une vie au joueur.
              if(player.score % LEVEL_CHANGE == 0)
              {
                player.vie++;
              }
              
              return;
            }
          }
        }
        }
    }
  }  
  //Vérifie si un ennemi touche le joueur
  if(player.pos_x < ennemy[num_ennemie].pos_x + ENNEMY_WIDTH && player.pos_x + PLAYER_WIDTH > ennemy[num_ennemie].pos_x && player.pos_y
  < ennemy[num_ennemie].pos_y + ENNEMY_HEIGHT && player.pos_y + PLAYER_HEIGHT > ennemy[num_ennemie].pos_y)
{
      efface_ennemies(num_ennemie);
      ennemy[num_ennemie].actif = false;
      player.vie--;
    }
  }

  
void bouge_joueur()
{
  efface_joueur();
  player.pos_x++;
  affiche_joueur();
    if(player.pos_x == 66)
  {
    player.pos_x = -9;
  }
}

int check_balle_libre()
{
  for(int i = 0; i < MAX_BALLES; i++)
  {
    if(balles[i].actif == 0) //Si la balle est inactive
    {
      return i;
    }
  }
  return -1;
}

void affiche_tire_balle_joueur(bool create_balle)
{
  int balle_libre;
  static unsigned long lastMoveTime = 0;    
  //Si on veut créer une balle
  if(create_balle == 1)
  {
    balle_libre = check_balle_libre(); //On trouve quel balle est libre

    //Si on a trouver une balle libre
    if(balle_libre != -1)
    {
      //On trouve le centre du joueur pour que la balle apparaisse au centre du modèle du joueur.
      balles[balle_libre].pos_x = player.pos_x + PLAYER_WIDTH;
      balles[balle_libre].pos_y = player.pos_y + 1;
      balles[balle_libre].actif = true; //On active la balle
      matrix.drawPixel(balles[balle_libre].pos_x, balles[balle_libre].pos_y , matrix.Color888(153, 0, 0)); //On affiche le pixel de la balle au bon emplacement
    } 
  } else { //Sinon on bouge toute les balles
    if((millis() - lastMoveTime) > VITESSE_BALLES)
    {
      for(int i = 0; i < MAX_BALLES; i++)
      {
        //Si la balle est active
        if(balles[i].actif == true)
        {
          matrix.drawPixel(balles[i].pos_x, balles[i].pos_y, matrix.Color888(0, 0, 0)); //On efface la balle
          balles[i].pos_x++; //On bouge la balle de 1 en x
          matrix.drawPixel(balles[i].pos_x, balles[i].pos_y, matrix.Color888(153, 0, 0)); //On affiche le pixel de la balle au bon emplacement

          //Si la balle quitte l'écran par la droite.
          if(balles[i].pos_x >= LARGEUR_MATRICE)
          {
            balles[i].actif = false; //On désactive la balle
          }
        }
      }
      lastMoveTime = millis();
      
    }
  } 
}

void gere_balle()
{
  static bool bouton_precedent = HIGH; 
  static bool tire = true; 

  bool bouton_actuel = digitalRead(BTN_C);

  if (bouton_precedent == HIGH && bouton_actuel == LOW && tire) {
    affiche_tire_balle_joueur(true); 
    tire = false;  
  }

  
  affiche_tire_balle_joueur(false);

  if (bouton_actuel == HIGH) {
    tire = true;  
  }
  bouton_precedent = bouton_actuel;

}

void affiche_interface()
{
  static int oldLife = -1, oldScore = -1;
  if(player.vie != oldLife)//Si la vie a changé
{
  matrix.drawLine(0,22,63,22, matrix.Color888(255, 255, 255));
  matrix.setTextSize(1);
  matrix.setTextWrap(false);
  matrix.setCursor(5, 23);
  matrix.setTextColor(matrix.Color888(255, 215, 0));
  matrix.fillRect(5, 23, 30, 8, matrix.Color888(0, 0, 0));
  matrix.setCursor(5, 23);
  matrix.print("V:");
  matrix.setCursor(16,23);
  matrix.print(player.vie);
  matrix.setTextSize(1);
  matrix.setTextWrap(false);
  oldLife = player.vie;
  if(player.vie == 0)
  {
    player.vie = 0;
  }
  }


  //Si la vie ou le score ont changé
  if(player.score != oldScore)
{
matrix.fillRect(35, 23, 28, 8, matrix.Color888(0, 0, 0));
matrix.setTextSize(1);
matrix.setCursor(35, 23);
matrix.setTextColor(matrix.Color888(255, 215, 0));
matrix.print("P:");
matrix.print(player.score);


  oldScore = player.score;
  
}

}



void titre()
{
  cheatvie();
  matrix.setTextSize(1);
  matrix.setTextWrap(false);
  matrix.setCursor(5, 1);
  matrix.setTextColor(matrix.Color888(255, 128, 0));
  matrix.println("Imperator");
  matrix.drawLine(0,0,0,8, matrix.Color888(255, 255, 255));
  matrix.drawLine(0,14,0,32, matrix.Color888(255, 255, 255));
  matrix.drawLine(0,0,64,0, matrix.Color888(255, 255, 255));
  matrix.drawLine(0,31,63,31, matrix.Color888(255, 255, 255));
  matrix.drawLine(63,0,63,8, matrix.Color888(255, 255, 255));
  matrix.drawLine(63,14,63,31, matrix.Color888(255, 255, 255));
  matrix.drawLine(0, 9, 63, 9, matrix.Color888(255, 51, 51));
  matrix.drawLine(0, 13, 63, 13, matrix.Color888(255, 51, 51));
  matrix.setCursor(2, 15);
  matrix.setTextColor(matrix.Color888(255, 215, 0));
  matrix.println("High Score");
  matrix.setCursor(29, 23);
  matrix.setTextColor(matrix.Color888(255, 51, 153));
  matrix.println(lireHighScore());

   bouge_joueur();

   if(digitalRead(BTN_UP) == LOW || digitalRead(BTN_DOWN) == LOW)
   {
    etatActuel = ETAT_JEUX;
    player.pos_x = START_PLAYER_X;
    player.pos_y = START_PLAYER_Y;
    efface_ecran();
   }
}

void gameOver()
{
    matrix.setTextSize(1);
    matrix.setTextColor(matrix.Color888(255, 0, 0));
    matrix.setCursor(15, 0);
    matrix.print("DEATH!");
    matrix.setTextColor(matrix.Color888(255, 255, 255));
    matrix.setCursor(10, 8);
    matrix.print("Level ");
    matrix.print(player.level);
    matrix.setTextColor(matrix.Color888(255, 255, 255));
    matrix.setCursor(10, 16);
    matrix.print("Score:");
    matrix.print(player.score);
    if(player.score > lireHighScore())
    {
      ecrireHighScore(player.score);
    }
    if(digitalRead(BTN_A) == LOW && digitalRead(BTN_B) == LOW && digitalRead(BTN_C) == LOW)
    {
      etatActuel = ETAT_RESET;
      delay(100);
      resetGame();
    }
    delay(4000);
    prelude();
}

void prelude()
{
    efface_ecran();
    matrix.setTextSize(1);
    matrix.setTextColor(matrix.Color888(255, 0, 0));
    matrix.setCursor(12, 5);
    matrix.print("MISSION ");
    matrix.setCursor(15, 15);
    matrix.print("FAILED");
    delay(4000);
    resetGame();
}


void resetGame()
{
  for(int i = 0; i < MAX_ENNEMIES; i++)
  {
    efface_ennemies(i);
    ennemy[i].actif = false;
    ennemy[i].pos_x = LARGEUR_MATRICE;
    ennemy[i].pos_y = random(0, HAUTEUR_MATRICE - ENNEMY_HEIGHT);
    ennemy[i].lastMoveTime = millis();
  }
  for(int i = 0; i < MAX_BALLES; i++)
  {
    balles[i].actif = false;
  }
  efface_ecran();
  player.pos_x = 5;
  player.pos_y = HAUTEUR_MATRICE / 3, 2 - PLAYER_HEIGHT / 2;
  player.vie = 5;
  player.score = 0;
  player.level = 1;
  etatActuel = ETAT_TITRE;
}

void ecrireHighScore(int score) 
{
  //Rappel un int est égale à deux octets. Chaque case mémoire EEPROM
  //ne peut stocker qu'un octet. On doit donc diviser en deux le int 
  //pour pouvoir le stocker.
  EEPROM.write(0, highByte(score)); //stocke l'octet haut dans la case 0.
  EEPROM.write(1, lowByte(score)); //stocke l'octet bas dans la case 1.
}

int lireHighScore() {
  //Comme chaque case de la mémoire ne contient qu'un octet, on doit
  //recomposer l'entier à partir de deux octets.
  int highScore = EEPROM.read(0) << 8;
  highScore += EEPROM.read(1);
  return highScore;
}

void resetHighScore()
{
  ecrireHighScore(0);
}

//***************************************************************
// Fonction qui d'effacer l'ensemble de l'écran. Mets tout l'écran
// en noire. ATTENTION cette fonction prends beaucoup de temps et
// peut grandement ralentir le jeu si elle est trop souvent utilisée.
//
// Paramètres: aucun
// Retour: aucun
//***************************************************************
void efface_ecran()
{
  matrix.fillScreen(matrix.Color888(0,0,0));
}
void game()
{
  gere_balle();
  check_button();
  affiche_ennemies(MAX_ENNEMIES);
  efface_ennemies(MAX_ENNEMIES);

  affiche_joueur();
  gerer_ennemies(); //gère les ennemies
  affiche_interface(); //affiche l'interface

  //Si après avoir bougé, tirer, bouger les ennemis et vérifier les collisions 
  //on vérifie si le joueur est toujours en vie.
  if(player.vie <= 0)
  {
    etatActuel = ETAT_GAMEOVER;
    efface_ecran();
  }
  

  }



// Fonction qui permet de vérifier si un bouton à été appuyé et 
// de réagir en conséquence.
//
// Paramètres: aucun
// Retour: aucun
//***************************************************************
void check_button()
{ 
  btn_up.update();
  btn_down.update();
  btn_left.update();
  btn_right.update();
  btn_c.update();
  static unsigned long lastPressed = 0;

  if (digitalRead(BTN_UP) == LOW) { // Bouton appuyé
    //Vérifie si le bouton est encore appuyé, évite de bouger trop vite
    if(millis() - lastPressed > 50) {
      lastPressed = millis(); 
      efface_joueur();
      player.pos_y--; //Bouge le joueur de 1 en y négatif
      //player.pos_y = constrain(player.pos_y, 0, HAUTEUR_MATRICE - (PLAYER_HEIGHT + (HAUTEUR_MATRICE - HAUTEUR_INTERFACE))); //Limite le mouvement du joueur à l'écran
      if(player.pos_y < 0)
    {
      player.pos_y = HAUTEUR_INTERFACE - PLAYER_HEIGHT;
    }
    affiche_joueur();

    }
  }
  if (digitalRead(BTN_DOWN) == LOW) { // Bouton appuyé
    //Vérifie si le bouton est encore appuyé, évite de bouger trop vite
    if(millis() - lastPressed > 50) {
      lastPressed = millis(); 
      efface_joueur();
      player.pos_y++; //Bouge le joueur de 1 en y négatif
      //player.pos_y = constrain(player.pos_y, 0, HAUTEUR_MATRICE - (PLAYER_HEIGHT + (HAUTEUR_MATRICE - HAUTEUR_INTERFACE))); //Limite le mouvement du joueur à l'écran
      if(player.pos_y >= HAUTEUR_INTERFACE - PLAYER_HEIGHT + 1)
      {
        // On le replace en haut de l'écran
        player.pos_y = 0;
      }
      affiche_joueur();
    }
  }
  if (digitalRead(BTN_LEFT) == LOW) { // Bouton appuyé
    //Vérifie si le bouton est encore appuyé, évite de bouger trop vite
    if(millis() - lastPressed > 50) {
      lastPressed = millis(); 
      efface_joueur();
      player.pos_x--; //Bouge le joueur de 1 en y négatif
      player.pos_x = constrain(player.pos_x, 0, MAX_X_PLAYER - PLAYER_WIDTH); //Limite le mouvement du joueur à l'écran

    }
  }
  if (digitalRead(BTN_RIGHT) == LOW) { // Bouton appuyé
    //Vérifie si le bouton est encore appuyé, évite de bouger trop vite
    if(millis() - lastPressed > 50) {
      lastPressed = millis(); 
      efface_joueur();
      player.pos_x++; //Bouge le joueur de 1 en y négatif
      player.pos_x = constrain(player.pos_x, 0, MAX_X_PLAYER - PLAYER_WIDTH); //Limite le mouvement du joueur à l'écran

    }
    
  }
  affiche_joueur();
  }

void setup() {
  Serial.begin(115200);
  randomSeed(A0);

  pinMode(BTN_UP, INPUT_PULLUP);
  btn_up.attach(BTN_UP); // Attache le bouton à l'objet Bounce
  btn_up.interval(DEBOUNCE_TIME); // Définit le délai anti-rebond à 50 millisecondes

  pinMode(BTN_DOWN, INPUT_PULLUP);
  btn_down.attach(BTN_DOWN); // Attache le bouton à l'objet Bounce
  btn_down.interval(DEBOUNCE_TIME); // Définit le délai anti-rebond à 50 millisecondes

  pinMode(BTN_LEFT, INPUT_PULLUP);
  btn_left.attach(BTN_LEFT); // Attache le bouton à l'objet Bounce
  btn_left.interval(DEBOUNCE_TIME); // Définit le délai anti-rebond à 50 millisecondes

  pinMode(BTN_RIGHT, INPUT_PULLUP);
  btn_right.attach(BTN_RIGHT); // Attache le bouton à l'objet Bounce
  btn_right.interval(DEBOUNCE_TIME); // Définit le délai anti-rebond à 50 millisecondes

  pinMode(BTN_A, INPUT_PULLUP);
  btn_a.attach(BTN_A); // Attache le bouton à l'objet Bounce
  btn_a.interval(DEBOUNCE_TIME); // Définit le délai anti-rebond à 50 millisecondes

  pinMode(BTN_B, INPUT_PULLUP);
  btn_b.attach(BTN_B); // Attache le bouton à l'objet Bounce
  btn_b.interval(DEBOUNCE_TIME); // Définit le délai anti-rebond à 50 millisecondes

  pinMode(BTN_C, INPUT_PULLUP);
  btn_c.attach(BTN_C); // Attache le bouton à l'objet Bounce
  btn_c.interval(DEBOUNCE_TIME); // Définit le délai anti-rebond à 50 millisecondes
  
  matrix.begin(); //démarrer la matrice
  matrix.fillScreen(matrix.Color888(0, 0, 0));

  for (int i = 0; i < MAX_ENNEMIES; i++) {
    ennemy[i].pos_x = 68;   // Set the initial X position outside the screen
    ennemy[i].pos_y = 10;   // Set a default Y position
    ennemy[i].actif = 0;    // Mark the enemy as inactive
    ennemy[i].lastMoveTime = 0; // Initialize the last move time
  }

  etatActuel = ETAT_TITRE;
  delay(1000);

}

void loop() {
  //Gestion de la machine à état
  switch(etatActuel)
  {
    case ETAT_TITRE:
      titre();
      break;
    case ETAT_JEUX:
      game();
      break;
    case ETAT_GAMEOVER:
      gameOver();
      break;
    case ETAT_PRELUDE:
      prelude();
    case ETAT_RESET:
      resetGame();
      break;
  }


}