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
    Graph g;
   // g.menu();
    g.recuperation1();

   // g.rajout1();
 //g.supprimer_Sommet(4);

    /// Vous gardez la main sur la "boucle de jeu"
    /// ( contrairement � des frameworks plus avanc�s )
    while ( !key[KEY_ESC] )
    {
        /// Il faut appeler les m�thodes d'update des objets qui comportent des widgets
        g.update();
        ///1kg = 10 1h=0,1
/*
        g.reglage_multi_moins(1,0,0.008);//sommet 0
        g.reglage_div_p(2,7,1,2,40);//sommet 1
        g.reglage_plus_moins(5,1,2,0.8,2);//sommet 2
        g.reglage_2plus_1moins_p(1,2,4,3,12,9,1);//sommet 3
        g.reglage_moins_plus(5,3,4,0.016,1);// sommet 4
        g.reglage_2plus_2moins(6,4,7,2,5,0.8,0.016,0.5,0.8); // sommet 5
        g.reglage_2moins_1plus(8,1,5,7,20.0,40.0,0.5); // reglage sommet 7
        g.reglage_div(7,8,20.0); // reglage sommet 8
*/

        /// Mise � jour g�n�rale (clavier/souris/buffer etc...)
        grman::mettre_a_jour();

    }

g.sauvegarde3();
    grman::fermer_allegro();

    return 0;
}
END_OF_MAIN();
