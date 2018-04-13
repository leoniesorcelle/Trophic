//Initialisation des bibliothèques nécessaires au bon fonctionnement du code
#include "grman/grman.h"
#include <iostream>
#include "graph.h"

using namespace std;

int main()
{
    //A appeler avant d'instancier les objets graphiques etc...tous les éléments de l'interface
    grman::init();
    //Permet de retrouver le repertoire où se trouvent les images à charger
    grman::set_pictures_path("pics");
    //Initialisation des différentes variables nécessaires au fonctionnement de la boucle de jeu
    int what=0;
    int fetched1=0;
    int fetched2=0;
    int fetched3=0;
    //On initialise un exemple de menu en appelant la class
    Menu m;
    //On initialise un exemple de graphe en appelant la class
    Graph g;

    //Boucle de jeu
    while ( !key[KEY_ESC] )
    {
      //Le switch permet le fonctionnement de l'interface en prenant compte du choix de l'utilisateur dans le menu
      switch (what) {

        //La boucle de jeu entre directement dans la case 0
        case 0:
        //On appelle la fonction update de la class Menu pour afficher le menu sur l'interface
        what=m.update(g);
        grman::mettre_a_jour();
        break;

        case 1:
        //Si l'utilisateur a cliqué sur le premier réseau, on rentre dans un booléen nommé "fetched", c'est à dire récupération
        if (!fetched1){
            //Le booléen permet l'appelation de la fonction récupération de la class Graph pour le premier réseau
            //Elle récupère toutes les informations des widgets dans un fichier pour ensuite les afficher sur l'interface
            g.recuperation1();
            //Puis le booléen change de valeur pour que la boucle ne s'effectue qu'une seule fois
            fetched1=1;
        }
        //On appelle la fonction update de la class Graph qui permet l'affichage de la barre d'outils sur l'interface
        what=g.updateG1(what);
        g.reglage1();
        grman::mettre_a_jour();
        break;

        case 2:
        //De même pour le deuxième réseau
        if (!fetched2){
            g.recuperation2();
            fetched2=1;
        }
        what=g.updateG2(what);
        g.reglage2();
        grman::mettre_a_jour();
        break;

        case 3:
        //De même pour le troisième réseau
        if (!fetched3){
            g.recuperation3();
            fetched3=1;
        }
        what=g.updateG3(what);
        g.reglage3();
        grman::mettre_a_jour();
        break;

        case 4:
        //Si l'utilisateur clique sur le quatrième bouton, on quitte le menu
        exit(EXIT_FAILURE);
        break;
      }
    }

    //g.sauvegarde1();
    grman::fermer_allegro();

    return 0;
}
END_OF_MAIN();
