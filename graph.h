#ifndef GRAPH_H_INCLUDED
#define GRAPH_H_INCLUDED

#include <vector>
#include <map>
#include <string>
#include <memory>

#include <iostream>
#include <stack>
#include "grman/grman.h"

/***************************************************
                    VERTEX
****************************************************/
//Ces class permettent l'affichage des sommets
class VertexInterface
{
    //On met ces trois autres class en friend pour leur permetrre d'accéder directement à ses attributs privés
    friend class Vertex;
    friend class EdgeInterface;
    friend class Graph;

    //Ici, on va déclarer les widgets de l'interface nécessaires pour les sommets
    private :
        //Déclaration de la boite qui contient toute l'interface d'un sommet
        grman::WidgetBox m_top_box;
        //Déclaration du slider de visualisation/modification de la valeur du sommet : jauge
        grman::WidgetVSlider m_slider_value;
        //Déclaration du label de visualisation de la valeur du sommet : nombre affiché en bas à gauche qui se modifie en fonction du slider
        grman::WidgetText m_label_value;
        //Déclaration de l'image de "remplissage" du sommet
        grman::WidgetImage m_img;
        //Déclaration du label indiquant l'index du sommet : nombre affiché en bas à droite, le numéro du sommet
        grman::WidgetText m_label_idx;
        //Déclaration de la boite pour le label précédent : boite avec le numéro du sommet dedans
        grman::WidgetText m_box_label_idx;

        int vertex_id;

    public :
        //Le constructeur met en place les éléments de l'interface
        VertexInterface(int idx, int x, int y, std::string pic_name="", int pic_idx=0);
};

class Vertex
{
    friend class Graph;
    friend class VertexInterface;
    friend class Edge;
    friend class EdgeInterface;

    private :
        //Liste des indices des arcs arrivant au sommet : accès aux prédécesseurs
        std::vector<int> m_in; //plusieurs flèches qui arrivent
        //Liste des indices des arcs partant du sommet : accès aux successeurs
        std::vector<int> m_out; //plusieurs flèches qui partent
        //Exemple de donnée associée à l'arc
        double m_value; //poids

        int m_id; //identifiant
        int m_x,m_Y; //coordonnées
        //Le POINTEUR sur l'interface associée, nullptr -> pas d'interface
        std::shared_ptr<VertexInterface> m_interface = nullptr;

    public :
        //On ne donne qu'un seul constructeur qui peut utiliser une interface
        Vertex (double _value=0,VertexInterface *interface=nullptr) :
            m_value(_value),m_interface(interface)  {  }

        //Vertex étant géré par Graph, ce sera la méthode update de graph qui appellera
        //le pre_update et post_update de Vertex (pas directement la boucle de jeu)
        void pre_update();
        void post_update();
};

/***************************************************
                    EDGE
****************************************************/
//Ces class permettent l'affichage des arcs
class EdgeInterface
{
    friend class Edge;
    friend class Graph;

    private :
        //Le WidgetEdge qui "contient" toute l'interface d'un arc
        grman::WidgetEdge m_top_edge;
        //Une boite pour englober les widgets de réglage associés càd les jauges
        grman::WidgetBox m_box_edge;
        //Un slider de visualisation/modification du poids valeur de l'arc : jauge pour la valeur d'un arc
        grman::WidgetVSlider m_slider_weight;
        // Un label de visualisation du poids de l'arc : nombre affiché en dessous de la jauge
        grman::WidgetText m_label_weight;

    public :
        // Le constructeur met en place les éléments de l'interface
        EdgeInterface(Vertex& from, Vertex& to);
};

class Edge
{
    friend class Graph;
    friend class EdgeInterface;

    private :
        //Indice du sommet de départ de l'arc
        int m_from;
        //Indice du sommet d'arrivée de l'arc
        int m_to;
        //Exemple de donnée associée à l'arc
        double m_weight; //poids
        int m_i; //identifiant
        //Le POINTEUR sur l'interface associée, nullptr -> pas d'interface
        std::shared_ptr<EdgeInterface> m_interface = nullptr;

    public :
        //Ici on ne donne qu'un seul constructeur qui peut utiliser une interface
        Edge (double weight=0, EdgeInterface *interface=nullptr) :
            m_weight(weight), m_interface(interface)  {  }
        //Edge étant géré par Graph, ce sera la méthode update de graph qui appellera
        //le pre_update et post_update de Edge (pas directement la boucle de jeu)
        void pre_update();
        void post_update();
};

/***************************************************
                    GRAPH
****************************************************/
//Cette class va permettre l'affichage de toute la barre d'outils qui vont aider à l'étude des réseaux
class GraphInterface
{
    friend class Graph;

    private :
        //La boite qui contient toute l'interface d'un graphe
        grman::WidgetBox m_top_box;
        //Dans cette boite seront ajoutés les (interfaces des) sommets et des arcs...
        grman::WidgetBox m_main_box;
        //Dans cette boite seront ajoutés des boutons de contrôle etc...
        grman::WidgetBox m_tool_box;
        //Widget de d'édition de la barre d'outils

        grman::WidgetButton m_boutonAjout; //Permet l'ajout de sommet sur le graph
        grman::WidgetImage m_boutonAjout_image; //Permet l'ajout de l'image sur le bouton d'ajout
        grman::WidgetButton m_boutonSupp; //Permet la suppression d'un sommet sur le graph
        grman::WidgetImage m_boutonSupp_image;
        grman::WidgetButton m_boutonCharger; //Permet de charger un des trois graph
        grman::WidgetImage m_boutonCharger_image;
        grman::WidgetButton m_boutonSauvegarder; //Permet de sauvegarder le graph affiché
        grman::WidgetImage m_boutonSauvegarder_image;
        grman::WidgetButton m_boutonQuitter; //Permet de quitter le graph affiché
        grman::WidgetImage m_boutonQuitter_image;
        grman::WidgetButton m_boutonAlgoChoix; //Permet de lancer l'algorithme de connexité
        grman::WidgetImage m_boutonAlgoChoix_image;
        grman::WidgetText m_algo; //Affichage de texte sur la barre d'outils
        grman::WidgetText m_algo2_1;
        grman::WidgetText m_algo2_2;
        grman::WidgetText m_algo2_3;
        grman::WidgetText m_algo2_4;

    public :
        // Le constructeur met en place les éléments de l'interface
        GraphInterface(int x, int y, int w, int h);
};

class Graph
{
    public :
        //La "liste" des arêtes
        std::map<int, Edge> m_edges; //indice et arete
        //La liste des sommets
        std::map<int, Vertex> m_vertices;

        int m_ordre;
        int m_arete;

        //Le POINTEUR sur l'interface associée, nullptr -> pas d'interface
        std::shared_ptr<GraphInterface> m_interface = nullptr;

        //Ici on ne donne qu'un seul constructeur qui peut utiliser une interface
        Graph (GraphInterface *interface=nullptr) :
            m_interface(interface){  }

        //Méthode spéciale qui construit un graphe arbitraire
        void add_interfaced_vertex(int idx, double value, int x, int y, std::string pic_name="");
        void add_interfaced_edge(int idx, int vert1, int vert2, double weight=0);

        //La méthode update à appeler dans la boucle de jeu pour les graphes avec interface
        int update(int what);
        //Cette fonction va permettre de mettre en avant les sommets fortement connexes
        void connexite();
        //Cette fonction permet de récupérer les informations des widgets à afficher dans un fichier pour les trois différents graphs
        void recuperation1();
        void recuperation2();
        void recuperation3();
        //Cette fonction permet la sauvegarder de chaque graph lorsqu'on clique le bouton
        void sauvegarde1();
        void sauvegarde2();
        void sauvegarde3();
        //Cette fonction permet d'ajouter un sommet au graph affiché
        void rajout1();
        void rajout2();
        void rajout3();

        void supprimer_edge(int edx);
        void supprimer_Sommet(int idx);
};

class Menu
{
    friend class Graph;

    public :
        //La boite qui contient toute l'interface d'un graphe
        grman::WidgetBox m_top_box;
        //Dans cette boite seront ajoutés les (interfaces des) sommets et des arcs...
        grman::WidgetBox m_main_box;
        //Dans cette boite seront ajoutés des boutons de contrôle etc...
        grman::WidgetBox m_tool_box;

        grman::WidgetBox m_Menu; //Boite qui englobe le visuel du menu
        grman::WidgetImage m_Menu_image; //Image ajoutée sur le visuel du menu
        grman::WidgetButton m_boutonReseau1; //Premier bouton qui permet l'affichage de l'interface du premier réseau
        grman::WidgetButton m_boutonReseau2; //De même pour les deuxième et troisième réseaux
        grman::WidgetButton m_boutonReseau3;
        grman::WidgetButton m_boutonMenuQuitter; //Bouton qui permet de quitter

        Menu(); //Constructeur de la class menu qui va permettre de mettre en place les éléments de l'interface du menu
        //Cette méthode permet le déroulement du code lorsqu'on clique sur un des boutons initialisé avant
        int update(Graph g);
};

#endif // GRAPH_H_INCLUDED
