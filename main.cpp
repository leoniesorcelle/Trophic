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
    Menu m;
    Graph g;
    //g.menu();
    //g.rajout1();
    //g.supprimer_Sommet();

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
        what=g.update();
        g.recuperation1();
        grman::mettre_a_jour();
        break;
        case 2:
        what=g.update();
        g.recuperation2();
        grman::mettre_a_jour();
        break;
        case 3:
        what=g.update();
        g.recuperation3();
        grman::mettre_a_jour();
        case 4:
        exit(EXIT_FAILURE);
      }
        /// Il faut appeler les m�thodes d'update des objets qui comportent des widgets
        //g.update();
        /// Mise � jour g�n�rale (clavier/souris/buffer etc...)
    }

    g.sauvegarde1();
    grman::fermer_allegro();

    return 0;
}
END_OF_MAIN();
