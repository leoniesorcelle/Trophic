#include "grman/grman.h"
#include <iostream>
#include "graph.h"

using namespace std;

int main()
{
    /// A appeler en 1er avant d'instancier des objets graphiques etc...
    grman::init();
    /// Le nom du r�pertoire o� se trouvent les images � charger
    grman::set_pictures_path("pics");
    /// Un exemple de graphe
    int what=0;
    int fetched=0;
    int recup2=0;
    int recup3=0;
    Menu m;
    Graph g;


    /// Vous gardez la main sur la "boucle de jeu"
    /// ( contrairement � des frameworks plus avanc�s )
    while ( !key[KEY_ESC] )
    {
      switch (what) {
        case 0:
        what=m.update(g);
        grman::mettre_a_jour();
        break;
        case 1:
        if (!fetched){
            g.recuperation1();
            fetched=1;
        }
        what=g.update(what);
        grman::mettre_a_jour();
        break;
        case 2:
        if (!recup2){
            g.recuperation2();
            recup2=1;
        }
        what=g.update(what);
        grman::mettre_a_jour();
        break;
        case 3:
        if (!recup3){
            g.recuperation3();
            recup3=1;
        }
        what=g.update(what);
        grman::mettre_a_jour();
        break;
        case 4:
        exit(EXIT_FAILURE);
        break;
      }

    }

    g.sauvegarde1();
    grman::fermer_allegro();

    return 0;
}
END_OF_MAIN();
