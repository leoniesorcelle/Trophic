#include "graph.h"
#include "grman/grman.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

/***************************************************
                    VERTEX
****************************************************/
//Le constructeur met en place les éléments de l'interface
VertexInterface::VertexInterface(int idx, int x, int y, std::string pic_name, int pic_idx)
{
    //On affiche la totalité de l'écran dans une top box
    m_top_box.set_pos(x, y);
    m_top_box.set_dim(130, 110);
    m_top_box.set_moveable();

    //On affiche le slider de réglage de valeur
    m_top_box.add_child( m_slider_value );
    m_slider_value.set_range(0.0 , 1000.0);
    m_slider_value.set_dim(20,90);
    m_slider_value.set_gravity_xy(grman::GravityX::Left, grman::GravityY::Up);

    //On affiche le label de visualisation de valeur : valeur du slider
    m_top_box.add_child( m_label_value );
    m_label_value.set_gravity_xy(grman::GravityX::Left, grman::GravityY::Down);

    //On affiche une illustration
    if (pic_name!="")
    {
        m_top_box.add_child( m_img );
        m_img.set_pic_name(pic_name);
        m_img.set_pic_idx(pic_idx);
        m_img.set_gravity_x(grman::GravityX::Right);
    }

    //On affiche le label de visualisation d'index du sommet dans une boite pour savoir comment est situé le sommet par rapport aux autres
    m_top_box.add_child( m_box_label_idx );
    m_box_label_idx.set_gravity_xy(grman::GravityX::Right, grman::GravityY::Down);
    m_box_label_idx.set_dim(20,12);
    m_box_label_idx.set_bg_color(BLANC);

    m_box_label_idx.add_child( m_label_idx );
    m_label_idx.set_message( std::to_string(idx) );
}

//Gestion du Vertex avant l'appel à l'interface
void Vertex::pre_update()
{
    if (!m_interface)
        return;
    //Copier la valeur locale de la donnée m_value vers le slider associé
    m_interface->m_slider_value.set_value(m_value);
    //Copier la valeur locale de la donnée m_value vers le label sous le slider
    m_interface->m_label_value.set_message( std::to_string( (int)m_value) );
}

//Gestion du Vertex après l'appel à l'interface
void Vertex::post_update()
{
    if (!m_interface)
        return;
    //Reprendre la valeur du slider dans la donnée m_value locale
    m_value = m_interface->m_slider_value.get_value();
}

/***************************************************
                    EDGE
****************************************************/
//Le constructeur met en place les éléments de l'interface
EdgeInterface::EdgeInterface(Vertex& from, Vertex& to)
{
    //Le WidgetEdge de l'interface de l'arc
    if ( !(from.m_interface && to.m_interface) )
    {
        std::cerr << "Error creating EdgeInterface between vertices having no interface" << std::endl;
        throw "Bad EdgeInterface instanciation";
    }
    m_top_edge.attach_from(from.m_interface->m_top_box);
    m_top_edge.attach_to(to.m_interface->m_top_box);
    m_top_edge.reset_arrow_with_bullet();


    //Une boite pour englober les widgets de réglage associés
    m_top_edge.add_child(m_box_edge);
    m_box_edge.set_dim(20,20);
    m_box_edge.set_bg_color(BLANCBLEU);

    //Le slider de réglage de valeur
   // m_box_edge.add_child( m_slider_weight );
    m_slider_weight.set_range(0.0 , 100.0);
    m_slider_weight.set_dim(16,40);
    m_slider_weight.set_gravity_y(grman::GravityY::Up);

    //Label de visualisation de valeur
    m_box_edge.add_child( m_label_weight );
    m_label_weight.set_gravity_y(grman::GravityY::Down);
}

//Gestion du Edge avant l'appel à l'interface
void Edge::pre_update()
{
    if (!m_interface)
        return;
    //Copier la valeur locale de la donnée m_weight vers le slider associé
    m_interface->m_slider_weight.set_value(m_weight);
    //Copier la valeur locale de la donnée m_weight vers le label sous le slider
    m_interface->m_label_weight.set_message( std::to_string( (int)m_weight ) );
}

//Gestion du Edge après l'appel à l'interface
void Edge::post_update()
{
    if (!m_interface)
        return;
    //Reprendre la valeur du slider dans la donnée m_weight locale
    m_weight = m_interface->m_slider_weight.get_value();
}

/***************************************************
                    GRAPH
****************************************************/
//Ce constructeur se contente de préparer un cadre d'accueil des éléments qui seront ensuite ajoutés lors de la mise ne place du Graphe
GraphInterface::GraphInterface(int x, int y, int w, int h)
{
    //La top box prend les dimensions de l'écran où on va tout afficher
    m_top_box.set_dim(1000,740);
    m_top_box.set_gravity_xy(grman::GravityX::Right, grman::GravityY::Up);
    //On affiche la tool box sur la top box sur la gauche de l'écran pour ensuite afficher la barre d'outils dessus
    m_top_box.add_child(m_tool_box);
    m_tool_box.set_dim(200,720);
    m_tool_box.set_gravity_xy(grman::GravityX::Left, grman::GravityY::Up);
    m_tool_box.set_bg_color(BLANCBLEU);
    //On affiche la main box sur la top box sur la droite de l'écran
    m_top_box.add_child(m_main_box);
    m_main_box.set_dim(800,720);
    m_main_box.set_gravity_xy(grman::GravityX::Right, grman::GravityY::Up);
    m_main_box.set_bg_color(BLANCBLEU);

    //Déclaration des différents boutons sur la barre d'outils
    //Cadre et bouton pour ajouter un sommet
    m_tool_box.add_child(m_boutonAjout);
    m_boutonAjout.set_dim(70,50);
    m_boutonAjout.set_pos(10, 150);
    m_boutonAjout.set_bg_color(NOIR);
    m_boutonAjout.add_child(m_boutonAjout_image);
    m_boutonAjout_image.set_pic_name("Ajout_sommet.jpg");
    //Cadre et bouton pour supprimer un ou plusieurs sommets
    m_tool_box.add_child(m_boutonSupp);
    m_boutonSupp.set_dim(70,50);
    m_boutonSupp.set_pos(110,150);
    m_boutonSupp.set_bg_color(NOIR);
    m_boutonSupp.add_child(m_boutonSupp_image);
    m_boutonSupp_image.set_pic_name("Supp_sommet.jpg");
    //Cadre et bouton pour charger un nouveau réseau
    m_tool_box.add_child(m_boutonCharger);
    m_boutonCharger.set_dim(140,100);
    m_boutonCharger.set_pos(30,215);
    m_boutonCharger.set_bg_color(NOIR);
    m_boutonCharger.add_child(m_boutonCharger_image);
    m_boutonCharger_image.set_pic_name("Charger.jpg");
    //Cadre et bouton pour sauvegarder le réseau
    m_tool_box.add_child(m_boutonSauvegarder);
    m_boutonSauvegarder.set_dim(140,100);
    m_boutonSauvegarder.set_pos(30,330);
    m_boutonSauvegarder.set_bg_color(NOIR);
    m_boutonSauvegarder.add_child(m_boutonSauvegarder_image);
    m_boutonSauvegarder_image.set_pic_name("Sauvegarder.jpg");
    //Cadre et bouton pour quitter le réseau et retourner au menu
    m_tool_box.add_child(m_boutonQuitter);
    m_boutonQuitter.set_dim(140,100);
    m_boutonQuitter.set_pos(30,445);
    m_boutonQuitter.set_bg_color(NOIR);
    m_boutonQuitter.add_child(m_boutonQuitter_image);
    m_boutonQuitter_image.set_pic_name("Quitter.jpg");

    //Affichage des algorithmes
    m_tool_box.add_child(m_algo);
    m_algo.set_message("  A L G O R I T H M E :");
    m_algo.set_pos(0,560);
    //Cadre et bouton pour calculer k, réseau : robuste ou non
    m_tool_box.add_child(m_boutonAlgoChoix);
    m_boutonAlgoChoix.set_dim(180,80);
    m_boutonAlgoChoix.set_pos(5,570);
    m_boutonAlgoChoix.set_bg_color(NOIR);
    m_boutonAlgoChoix.add_child(m_boutonAlgoChoix_image);
    m_boutonAlgoChoix_image.set_pic_name("Algo_choix.jpg");
    //Affichage de l'algorithme quantitatif
    m_tool_box.add_child(m_algo2_1);
    m_algo2_1.set_message("Modifier les valeurs ");
    m_algo2_1.set_pos(0,665);
    m_tool_box.add_child(m_algo2_2);
    m_algo2_2.set_message("quantitatives a l'aide ");
    m_algo2_2.set_pos(0,675);
    m_tool_box.add_child(m_algo2_3);
    m_algo2_3.set_message("de chaque jauge pour ");
    m_algo2_3.set_pos(0,685);
    m_tool_box.add_child(m_algo2_4);
    m_algo2_4.set_message("etudier les changements");
    m_algo2_4.set_pos(0,695);
}

//Ce constructeur va permettre l'affchage du menu
Menu::Menu()
{
    //La déclaration de la tox box est la même que précédemment
    m_top_box.set_dim(1000,740);
    m_top_box.set_gravity_xy(grman::GravityX::Right, grman::GravityY::Up);

    //On affiche la box du menu sur la top box avec les mêmes dimensions
    m_top_box.add_child(m_Menu);
    m_Menu.set_dim(1000,740);
    m_Menu.set_gravity_xy(grman::GravityX::Left, grman::GravityY::Up);
    m_Menu.set_bg_color(BLANC);
    //On affiche sur le box du menu son image qui prend la totalité des dimensions
    m_Menu.add_child(m_Menu_image);
    m_Menu_image.set_pic_name("Menu.jpg");

    //Ici, on déclare les boutons pour les différentes actions que peut faire l'utilisateur dans le menu
    m_Menu.add_child(m_boutonReseau1);
    m_boutonReseau1.set_dim(380, 110);
    m_boutonReseau1.set_pos(567, 200);

    m_Menu.add_child(m_boutonReseau2);
    m_boutonReseau2.set_dim(380, 110);
    m_boutonReseau2.set_pos(567, 342);

    m_Menu.add_child(m_boutonReseau3);
    m_boutonReseau3.set_dim(380, 110);
    m_boutonReseau3.set_pos(567, 484);

    m_Menu.add_child(m_boutonMenuQuitter);
    m_boutonMenuQuitter.set_dim(240, 58);
    m_boutonMenuQuitter.set_pos(707, 626);
}

//On initialise la fonction qui va permettre de procéder aux différents choix de l'utilisateur
int Menu::update(Graph g)
{
    //On affiche la top box du menu donc les boutons sur l'image du menu comme déclarer dans le constructeur
    m_top_box.update();
    //Quand on clique sur le premier bouton
    if (m_boutonReseau1.clicked())
    {
        //On affiche le choix de l'utilisateur
        std::cout<<"Reseau numero 1" << std::endl;
        //On retourne la valeur 1 pour pouvoir afficher le premier réseau et son interface
        return 1;
    }

    //De même pour le deuxième bouton
    if (m_boutonReseau2.clicked())
    {
        std::cout<<"Reseau numero 2" << std::endl;
        return 2;
    }

    if (m_boutonReseau3.clicked())
    {
        std::cout<<"Reseau numero 3" << std::endl;
        return 3;
    }

    //Si on clique sur le dernier bouton, on quitte le menu
    if (m_boutonMenuQuitter.clicked())
    {
        std::cout<<"Demande de quitter l'interface!" << std::endl;
        return 4;
    }

    else
    //Sinon on retourne rien et la boucle ne continue pas, on reste afficher sur le menu
    return 0;
}

//La méhode update à appeler dans la boucle de jeu pour les graphes avec interface
int Graph::updateG1(int what)
{
    if (!m_interface)
    return what;

    //Si on clique sur les différents boutons de la barre d'outils de l'interface
    if (m_interface->m_boutonAjout.clicked())
    {
        std::cout<<"Demande d'ajout de sommet !" << std::endl;
        rajout1();
    }

    if (m_interface->m_boutonSupp.clicked())
    {   int idx;
        std::cout<<"Demande de suppression de sommet!" << std::endl;
        cout<<"Quel sommet voulez vous supprimer"<<endl;
        cin>>idx;
        supprimer_Sommet(idx);

    }

    if (m_interface->m_boutonCharger.clicked())
    {
        std::cout<<"Demande de chargement d'un reseau!" << std::endl;
        Null();
        recuperation1();
        return 0; // Retour au menu
    }

    if (m_interface->m_boutonSauvegarder.clicked())
    {
        std::cout<<"Demande de sauvegarde!" << std::endl;
        sauvegarde1();
        Null();
        recuperation1();
    }

    if (m_interface->m_boutonQuitter.clicked())
    {
        std::cout<<"Demande de quitter le reseau!" << std::endl;
        exit(EXIT_FAILURE);
    }

    if (m_interface->m_boutonAlgoChoix.clicked())
    {
        std::cout<<"Demande d'algorithme 2!" << std::endl;
    }

    for (auto &elt : m_vertices)
        elt.second.pre_update();
    for (auto &elt : m_edges)
        elt.second.pre_update();

    m_interface->m_top_box.update();

    for (auto &elt : m_vertices)
        elt.second.post_update();
    for (auto &elt : m_edges)
        elt.second.post_update();

    return what;
}

int Graph::updateG2(int what)
{
    if (!m_interface)
    return what;

    //Si on clique sur les différents boutons de la barre d'outils de l'interface
    if (m_interface->m_boutonAjout.clicked())
    {
        std::cout<<"Demande d'ajout de sommet !" << std::endl;
        rajout2();
    }

    if (m_interface->m_boutonSupp.clicked())
    {   int idx;
        std::cout<<"Demande de suppression de sommet!" << std::endl;
        cout<<"Quel sommet voulez vous supprimer"<<endl;
        cin>>idx;
        supprimer_Sommet(idx);
    }

    if (m_interface->m_boutonCharger.clicked())
    {
        std::cout<<"Demande de chargement d'un reseau!" << std::endl;
        Null();
        recuperation2();
        return 0;

    }

    if (m_interface->m_boutonSauvegarder.clicked())
    {
        std::cout<<"Demande de sauvegarde!" << std::endl;
        sauvegarde2();
        Null();
        recuperation2();
    }

    if (m_interface->m_boutonQuitter.clicked())
    {
        std::cout<<"Demande de quitter le reseau!" << std::endl;
        exit(EXIT_FAILURE);
    }

    if (m_interface->m_boutonAlgoChoix.clicked())
    {
        std::cout<<"Demande d'algorithme 2!" << std::endl;
    }

    for (auto &elt : m_vertices)
        elt.second.pre_update();
    for (auto &elt : m_edges)
        elt.second.pre_update();

    m_interface->m_top_box.update();

    for (auto &elt : m_vertices)
        elt.second.post_update();
    for (auto &elt : m_edges)
        elt.second.post_update();

    return what;
}

int Graph::updateG3(int what)
{
    if (!m_interface)
    return what;

    //Si on clique sur les différents boutons de la barre d'outils de l'interface
    if (m_interface->m_boutonAjout.clicked())
    {
        std::cout<<"Demande d'ajout de sommet !" << std::endl;
        rajout3();
    }

    if (m_interface->m_boutonSupp.clicked())
    {   int idx;
        std::cout<<"Demande de suppression de sommet!" << std::endl;
        cout<<"Quel sommet voulez vous supprimer"<<endl;
        cin>>idx;
        supprimer_Sommet(idx);
    }

    if (m_interface->m_boutonCharger.clicked())
    {
        std::cout<<"Demande de chargement d'un reseau!" << std::endl;
        Null();
        recuperation3();
        return 0;
    }

    if (m_interface->m_boutonSauvegarder.clicked())
    {
        std::cout<<"Demande de sauvegarde!" << std::endl;
        sauvegarde3();
        Null();
        recuperation3();
    }

    if (m_interface->m_boutonQuitter.clicked())
    {
        std::cout<<"Demande de quitter le reseau!" << std::endl;
        exit(EXIT_FAILURE);
    }

    if (m_interface->m_boutonAlgoChoix.clicked())
    {
        std::cout<<"Demande d'algorithme 2!" << std::endl;
    }

    for (auto &elt : m_vertices)
        elt.second.pre_update();
    for (auto &elt : m_edges)
        elt.second.pre_update();

    m_interface->m_top_box.update();

    for (auto &elt : m_vertices)
        elt.second.post_update();
    for (auto &elt : m_edges)
        elt.second.post_update();

    return what;
}

//Aide à l'ajout de sommets interfacés
void Graph::add_interfaced_vertex(int idx, double value, int x, int y, std::string pic_name)
{
    //m_interface = std::make_shared<GraphInterface>(50, 0, 750, 600);
    if ( m_vertices.find(idx)!=m_vertices.end() )
    {
        std::cerr << "Error adding vertex at idx=" << idx << " already used..." << std::endl;
        throw "Error adding vertex";
    }
    //Création d'une interface de sommet
    VertexInterface *vi = new VertexInterface(idx, x, y, pic_name);
    //Ajout de la top box de l'interface de sommet
    m_interface->m_main_box.add_child(vi->m_top_box);
    //On peut ajouter directement des vertices dans la map avec la notation crochet
    m_vertices[idx] = Vertex(value, vi);
}

//Aide à l'ajout d'arcs interfacés
void Graph::add_interfaced_edge(int idx, int id_vert1, int id_vert2, double weight)
{
    if ( m_edges.find(idx)!=m_edges.end() )
    {
        std::cerr << "Error adding edge at idx=" << idx << " already used..." << std::endl;
        throw "Error adding edge";
    }

    if ( m_vertices.find(id_vert1)==m_vertices.end() || m_vertices.find(id_vert2)==m_vertices.end() )
    {
        std::cerr << "Error adding edge idx=" << idx << " between vertices " << id_vert1 << " and " << id_vert2 << " not in m_vertices" << std::endl;
        throw "Error adding edge";
    }

    EdgeInterface *ei = new EdgeInterface(m_vertices[id_vert1], m_vertices[id_vert2]);
    m_interface->m_main_box.add_child(ei->m_top_edge);
    m_edges[idx] = Edge(weight, ei);
    m_edges[idx].m_from = id_vert1;
    m_edges[idx].m_to = id_vert2;

    m_vertices[id_vert1].m_out.push_back(idx);
    m_vertices[id_vert2].m_in.push_back(idx);
}

/// Fonction pour vider le graphe
void Graph::Null()
{
    m_vertices.clear();
    m_edges.clear();
    m_interface.reset();
    m_ordre=0;
    m_arete=0;

}

int Graph::RecupNull(int fetched)
{
    return 0;
}

/// Recuperer les informations de notre fichier
void Graph::recuperation1()
{
    //Ouvrir le fichier
    m_interface = std::make_shared<GraphInterface>(50, 0, 750, 600);
    cout<<m_interface<<endl;
    double value; //poids du sommet
    int m_x,m_y; //coordonnées du sommet
    string nom; //nom du sommet
    double poids; // poids de l'arete
    int ar,de; //numéro de sommet d'arrivée et de départ
    std::ifstream fichierA("fiche1.txt",std::ios::in); //lecture de fichier
    int nbsommet;
    int nbaretes;
    Vertex v;
    //Vérifier l'ouverture
    if(fichierA)
    {
        fichierA>>nbsommet;
        m_ordre=nbsommet;
        fichierA>>nbaretes;
        m_arete=nbaretes;
        for(int i=0;i<nbsommet;i++)
        {
          fichierA>>i>>value>>m_x>>m_y>>nom;
          add_interfaced_vertex(i,value,m_x,m_y,nom);
          v.m_id=i;

        }
        for(int i=0;i<nbaretes;i++)
        {
          fichierA>>i>>de>>ar>>poids;
          add_interfaced_edge(i,de,ar,poids);

        }
    }

    else //S'il y a une erreur
        cout << " error recuperation" << endl;
        //Fermer le fichier
        fichierA.close();
}

void Graph::recuperation2()
{
    //Ouvrir le fichier
    m_interface = std::make_shared<GraphInterface>(50, 0, 750, 600);

    double value; //poids du sommet
    int m_x,m_y; //coordonnées du sommet
    string nom; //nom du sommet
    double poids; //poids de l'arete
    int ar,de; //numéro de sommet d'arrivée et de départ
    std::ifstream fichierA("fiche2.txt",std::ios::in);
    int nbsommet;
    int nbaretes;
    //Vérifier l'ouverture
    if(fichierA)
    {
        fichierA>>nbsommet;
        m_ordre=nbsommet;
        fichierA>>nbaretes;
        m_arete=nbaretes;
        for(unsigned int i=0;i< nbsommet;i++)
        {
          fichierA>>i>>value>>m_x>>m_y>>nom;
          add_interfaced_vertex(i,value,m_x,m_y,nom);
        }
        for(unsigned int i=0;i< nbaretes;i++)
        {
          fichierA>>i>>de>>ar>>poids;
          add_interfaced_edge(i,de,ar,poids);
        }
    }

    else //S'il y a une erreur
    cout << " error recuperation" << endl;
    //Fermer le fichier
    fichierA.close();
}

void Graph::recuperation3()
{
    //Ouvrir le fichier
    m_interface = std::make_shared<GraphInterface>(50, 0, 750, 600);

    double value; //poids du sommet
    int m_x,m_y; //coordonnées du sommet
    string nom; //nom du sommet
    double poids; //poids de l'arete
    int ar,de; //numéro de sommet d'arrivée et de départ
    std::ifstream fichierA("fiche3.txt",std::ios::in);
    int nbsommet;
    int nbaretes;
    //Vérifier l'ouverture
    if(fichierA)
    {
        fichierA>>nbsommet;
        m_ordre=nbsommet;
        fichierA>>nbaretes;
        m_arete=nbaretes;
        for(unsigned int i=0;i< nbsommet;i++)
        {
          fichierA>>i>>value>>m_x>>m_y>>nom;
          add_interfaced_vertex(i,value,m_x,m_y,nom);
        }
        for(unsigned int i=0;i< nbaretes;i++)
        {
          fichierA>>i>>de>>ar>>poids;
          add_interfaced_edge(i,de,ar,poids);
        }
    }

    else //S'il y a une erreur
    cout << " error recuperation" << endl;
    //Fermer le fichier
    fichierA.close();
}

/// Effectuer une sauvegarde
void Graph::sauvegarde1()
{
    //Ouvrir le fichier
    m_interface = std::make_shared<GraphInterface>(50, 0, 750, 600);

    ofstream fichierA("fiche1.txt", ios::out | ios::ate);
    //Vérifier l'ouverture
    if(fichierA)
    {
        fichierA<<m_vertices.size()<<endl;
        fichierA<<m_edges.size()<<endl;

        for(int i=0;i<m_vertices.size();i++)
        { m_vertices[i].m_interface->m_top_box.set_posx(m_vertices[i].m_interface->m_top_box.get_posx()+2);
        m_vertices[i].m_interface->m_top_box.set_posy(m_vertices[i].m_interface->m_top_box.get_posy()+2);
        fichierA<<i<<" "<<m_vertices[i].m_value<<" "<<m_vertices[i].m_interface->m_top_box.get_posx()<<" "<<m_vertices[i].m_interface->m_top_box.get_posy()<<" "<<m_vertices[i].m_interface->m_img.get_pic_name()<<endl;
        }

        for(int i=0;i<m_edges.size();i++)
        {
          fichierA<<i<<" "<<m_edges[i].m_from<<" "<<m_edges[i].m_to<<" "<<m_edges[i].m_weight<<endl;
        }
    }
    else //S'il y a une erreur
    cout << " error" << endl;
    //Fermer le fichier
    fichierA.close();
}

void Graph::sauvegarde2()
{
    //Ouvrir le fichier
    m_interface = std::make_shared<GraphInterface>(50, 0, 750, 600);

    ofstream fichierA("fiche2.txt", ios::out | ios::ate);
    //Vérifier l'ouverture
    if(fichierA)
    {
        fichierA<<m_vertices.size()<<endl;
        fichierA<<m_edges.size()<<endl;

        for(int i=0;i<m_vertices.size();i++)
        { m_vertices[i].m_interface->m_top_box.set_posx(m_vertices[i].m_interface->m_top_box.get_posx()+2);
        m_vertices[i].m_interface->m_top_box.set_posy(m_vertices[i].m_interface->m_top_box.get_posy()+2);
        fichierA<<i<<" "<<m_vertices[i].m_value<<" "<<m_vertices[i].m_interface->m_top_box.get_posx()<<" "<<m_vertices[i].m_interface->m_top_box.get_posy()<<" "<<m_vertices[i].m_interface->m_img.get_pic_name()<<endl;
        }

       for(int i=0;i<m_edges.size();i++)
       {
         fichierA<<i<<" "<<m_edges[i].m_from<<" "<<m_edges[i].m_to<<" "<<m_edges[i].m_weight<<endl;
       }
   }
   else //S'il y a une erreur
    cout << " error" << endl;
    //Fermer le fichier
    fichierA.close();
}

void Graph::sauvegarde3()
{
    //Ouvrir le fichier
    m_interface = std::make_shared<GraphInterface>(50, 0, 750, 600);

    ofstream fichierA("fiche3.txt", ios::out | ios::ate);
    //Vérifier l'ouverture
    if(fichierA)
    {
        fichierA<<m_vertices.size()<<endl;
        fichierA<<m_edges.size()<<endl;

        for(int i=0;i<m_vertices.size();i++)
        { m_vertices[i].m_interface->m_top_box.set_posx(m_vertices[i].m_interface->m_top_box.get_posx()+2);
        m_vertices[i].m_interface->m_top_box.set_posy(m_vertices[i].m_interface->m_top_box.get_posy()+2);
        fichierA<<i<<" "<<m_vertices[i].m_value<<" "<<m_vertices[i].m_interface->m_top_box.get_posx()<<" "<<m_vertices[i].m_interface->m_top_box.get_posy()<<" "<<m_vertices[i].m_interface->m_img.get_pic_name()<<endl;
        }

       for(int i=0;i<m_edges.size();i++)
       {
        fichierA<<i<<" "<<m_edges[i].m_from<<" "<<m_edges[i].m_to<<" "<<m_edges[i].m_weight<<endl;
       }
   }
   else //S'il y a une erreur
    cout << " error" << endl;
    //Fermer le fichier
    fichierA.close();
}

/// Rajouter un sommet
void Graph::rajout1() //idx numéro sommet, edx numéro aretes
{
    std::ifstream fichier("fiche1.txt",::ios::in);
    //Vérifier l'ouverture
    int nbsommet;
    int nbaretes;

    if(fichier)
    {
        fichier>>nbsommet;
        int idx=nbsommet-1;
        fichier>>nbaretes;
        int edx=nbaretes-1;

        string raj;
        cout<<endl<<endl<<"Vous pouvez rajouter soit insectes ou serpents"<<endl<<endl;
        cout<<"Tapez le nom du sommet a rajouter"<<endl;
    do{
        cin>>raj;

        if(raj=="insectes")
        {
          int iidx=idx+1;
          edx=edx+1;
          add_interfaced_vertex(iidx,500.0,0,100,"Insectes.jpg");
          add_interfaced_edge(edx,idx,iidx,40.0);

        }
        else if(raj=="serpents")
        {
          int iidx=idx+1;
          edx=edx+1;
          add_interfaced_vertex(iidx,20.0,100,0,"Serpents.jpg");
          add_interfaced_edge(edx,idx,iidx,40.0);
        }
        else
        {
          cout<<"nom incorrect"<<endl;
          cout<<"Veuillez retaper un nom entre insectes et serpents (Attention aux s)"<<endl;
        }
      }while((raj!="serpents")&&(raj!="insectes"));
    }
}

void Graph::rajout2() //idx numéro sommet, edx numéro aretes
{
    std::ifstream fichier("fiche2.txt",::ios::in);
    //Vérifier l'ouverture
    int nbsommet;
    int nbaretes;

    if(fichier)
    {
        fichier>>nbsommet;
        int idx=nbsommet-1;
        fichier>>nbaretes;
        int edx=nbaretes-1;

        string raj;
        cout<<endl<<endl<<"Vous pouvez rajouter soit bebe ou sportifs"<<endl;
        cout<<"Tapez le nom du sommet a rajouter"<<endl;
    do{
        cin>>raj;
        if(raj=="bebe")
        {
          int iidx=idx+1;
          edx=edx+1;
          add_interfaced_vertex(iidx,50.0,200,100,"Bebe.jpg");
          add_interfaced_edge(edx,idx,iidx,40.0);
        }
        else if(raj=="sportifs")
        {
          int iidx=idx+1;
          edx=edx+1;
          add_interfaced_vertex(iidx,20.0,200,100,"Sportifs.jpg");
          add_interfaced_edge(edx,idx,iidx,40.0);
        }
        else
        {
          cout<<endl<<"nom incorrect"<<endl;
          cout<<"Veuillez retaper un nom entre bebe et sportifs (Attention aux s)"<<endl;
        }
     }while((raj!="bebe")&&(raj!="sportifs"));
   }
}

void Graph::rajout3() //idx numéro sommet, edx numéro aretes
{
    std::ifstream fichier("fiche3.txt",::ios::in);
    //Vérifier l'ouverture
    int nbsommet;
    int nbaretes;

    if(fichier)
    {
        fichier>>nbsommet;
        int idx=nbsommet-1;
        fichier>>nbaretes;
        int edx=nbaretes-1;

        string raj;
        cout<<endl<<endl<<"Vous pouvez rajouter soit Homme ou pieuvres"<<endl<<endl;
        cout<<"Tapez le nom du sommet a rajouter"<<endl;
    do{
        cin>>raj;

        if(raj=="Homme")
        {
          int iidx=idx+1;
          edx=edx+1;
          add_interfaced_vertex(iidx,50.0,200,100,"Homme3.jpg");
          add_interfaced_edge(edx,idx,iidx,40.0);
        }
        else if(raj=="pieuvres")
        {
          int iidx=idx+1;
          edx=edx+1;
          add_interfaced_vertex(iidx,20.0,200,100,"Pieuvres.jpg");
          add_interfaced_edge(edx,idx,iidx,40.0);
        }
        else
        {
          cout<<"nom incorrect"<<endl;
          cout<<"Veuillez retaper un nom entre Homme et pieuvres (Attention aux s)"<<endl;
        }
      }while((raj!="Homme")&&(raj!="pieuvres"));
    }
}

///Supprimer
void Graph::supprimer_edge(int edx)
{
    //Référence vers le Edge à enlever
    Edge &remed=m_edges.at(edx);

    //Test : on a bien des éléments interfacés
    if (m_interface && remed.m_interface)
    {
        m_interface->m_main_box.remove_child( remed.m_interface->m_top_edge );
    }

    //Il reste encore à virer l'arc supprimé de la liste des entrants et sortants des 2 sommets to et from !
    //Références sur les listes de edges des sommets from et to
    std::vector<int> &vefrom = m_vertices[remed.m_from].m_out;
    std::vector<int> &veto = m_vertices[remed.m_to].m_in;
    vefrom.erase( std::remove( vefrom.begin(), vefrom.end(), edx ), vefrom.end() );
    veto.erase( std::remove( veto.begin(), veto.end(), edx ), veto.end() );

    //Le Edge ne nécessite pas non plus de delete car on n'a pas fait de new (sémantique par valeur)
    //Il suffit donc de supprimer l'entrée de la map pour supprimer à la fois l'Edge et le EdgeInterface
    //mais malheureusement ceci n'enlevait pas automatiquement l'interface top_edge en tant que child de main_box !
    m_edges.erase( edx );
}

void Graph::supprimer_Sommet(int idx)
{
    //Référence vers le sommet à enlever
    Vertex &remed=m_vertices.at(idx);

   vector <int> supp;
   //Pour trouver les aretes correspondantes au sommet
   for(int i=0; i<remed.m_in.size(); i++)
   {
       supp.push_back(remed.m_in[i]);
   }
   for(int i=0; i<remed.m_out.size();i++)
   {
       supp.push_back(remed.m_out[i]);
   }
   //On supprime les aretes trouvées
   for(int i=0;i<supp.size();i++)
    {
       supprimer_edge(supp.at(i));
    }

   if (m_interface && remed.m_interface)
    {
        m_interface->m_main_box.remove_child( remed.m_interface->m_top_box );
    }

    for(unsigned int i=0; i<m_vertices.size();i++)
    {
        if(i==idx)
        {
            m_vertices.erase(m_vertices[i].m_id);
        }

    }
}


/// Reglage
void Graph::reglage1()
{
       ///"Echelle 1kg = 10 1h=0,1"
        reglage_multi_moins(1,0,0.008);//sommet 0
        reglage_div_p(2,7,1,2,70);//sommet 1
        reglage_plus_moins(5,1,2,0.8,2);//sommet 2
        reglage_2plus_1moins_p(1,2,4,3,12,9,1);//sommet 3
        reglage_moins_plus(5,3,4,0.016,1);// sommet 4
        reglage_2plus_2moins(6,4,7,2,5,0.8,0.016,0.5,0.8); // sommet 5
        reglage_2moins_1plus(8,1,5,7,20.0,40.0,0.06); // reglage sommet 7
        reglage_div(7,8,20.0); // reglage sommet 8
}

void Graph::reglage2()
{
       /// "Echelle 1kg=10"
        reglage_div_p(1,5,0,125.0,5.0);// reglage sommet 0
        reglage_2moins_1plus_p(0,2,4,1,125.0,1.0,0.1); // reglage sommet 1
        reglage_div(3,2,2.0); // reglage sommet 2
        reglage_multi_p(5,1,4,0.1,0.1); // reglage sommet 4
        reglage_multi(4,5,0.1); // reglage sommet 5
}

void Graph::reglage3()
{
       /// "Echelle 1kg=10"
        reglage_4plus(1,2,3,4,0,2.0,2.0,2.0,2.0); // sommet 0
        reglage_moins_plus(0,8,1,2.0,4.0); // sommet 1
        reglage_3plus_1moins(5,6,7,0,2,7.0,7.0,7.0,2.0); // sommet 2
        reglage_2plus_1moins(8,7,0,3,0.7,1.0,2.0); // sommet 3
        reglage_moins_plus(0,7,8,2.0,1.0); // sommet 4
        reglage_moins_plus(2,8,5,7.0,1.0); // sommet 5
        reglage_moins_plus(2,8,6,7.0,0.8);// sommet 6
        reglage_3moins_1plus(3,4,2,8,7,1.0,1.0,7.0,0.5); //sommet 7
        reglage_2plus_5moins(9,10,1,3,7,5,6,8,2.0,1.0,4.0,0.7,0.5,1.0,0.8); //sommet 8
        reglage_multi_moins(8,9,2.0); //sommet 9
        reglage_3moins(11,8,2,10,10.0,1.0,5.0);// reglage sommet 10
        reglage_div(10,11,10.0);//reglage sommet 11
}

///SOUS PROGRAMMES DE REGLAGE///
void Graph ::reglage_div(int i,int j,double p)
{

    double m_poids=(m_vertices[i].m_interface->m_slider_value.get_value())/p;

    m_vertices[j].m_interface->m_slider_value.set_value(m_poids);
    m_vertices[j].m_interface->m_label_value.set_message( std::to_string( (int)m_poids ) );
    m_interface->m_top_box.update();
    m_poids =  m_vertices[j].m_interface->m_slider_value.get_value();

    double m_value =  m_vertices[i].m_interface->m_slider_value.get_value();

    if(m_poids==0.0)
    {
        if(key[KEY_S])
        {
            supprimer_Sommet(j);
        }

    }
}

void Graph ::reglage_div_moins(int i,int j,double p)
{

    double m_poids=(m_vertices[i].m_interface->m_slider_value.get_value())/p;
    double m_total= m_vertices[j].m_interface->m_slider_value.get_value()-m_poids;

    m_vertices[j].m_interface->m_slider_value.set_value(m_total);
    m_vertices[j].m_interface->m_label_value.set_message( std::to_string( (int)m_total ) );
    m_interface->m_top_box.update();
    m_total =  m_vertices[j].m_interface->m_slider_value.get_value();

    double m_value =  m_vertices[i].m_interface->m_slider_value.get_value();

    if(m_total==0.0)
    {
        if(key[KEY_S])
        {
            supprimer_Sommet(j);
        }

    }
}

void Graph::reglage_moins_plus(int i, int k, int j, double p, double w)
{
    double m_poids=(m_vertices[i].m_interface->m_slider_value.get_value())*p;
    double m_weight =(m_vertices[k].m_interface->m_slider_value.get_value())/w;
    double m_total= m_vertices[j].m_interface->m_slider_value.get_value()-m_poids+m_weight;

    m_vertices[j].m_interface->m_slider_value.set_value(m_total);
    m_vertices[j].m_interface->m_label_value.set_message( std::to_string( (int)m_total ) );
    m_interface->m_top_box.update();
    m_total =  m_vertices[j].m_interface->m_slider_value.get_value();

    double m_value1 = m_vertices[i].m_interface->m_slider_value.get_value();
    double m_value2 = m_vertices[k].m_interface->m_slider_value.get_value();

    if(m_total==0.0)
    {
        if(key[KEY_S])
        {
            supprimer_Sommet(j);
        }

    }
}

void Graph::reglage_div_p(int i, int k, int j, double p, double w)
{
    double m_poids=(m_vertices[i].m_interface->m_slider_value.get_value())/p;
    double m_weight =(m_vertices[k].m_interface->m_slider_value.get_value())/w;
    double m_total=(m_poids+m_weight);

    m_vertices[j].m_interface->m_slider_value.set_value(m_total);
    m_vertices[j].m_interface->m_label_value.set_message( std::to_string( (int)m_total ) );
    m_interface->m_top_box.update();
    m_total =  m_vertices[j].m_interface->m_slider_value.get_value();

    double m_value1 = m_vertices[i].m_interface->m_slider_value.get_value();
    double m_value2 = m_vertices[k].m_interface->m_slider_value.get_value();

    if(m_total==0.0||(m_value1+m_value2)<(p+w))
    {
        if(key[KEY_S])
        {
            supprimer_Sommet(j);
        }

    }
}

void Graph ::reglage_multi_moins(int i,int j,double p)
{

    double m_poids= m_vertices[i].m_interface->m_slider_value.get_value()*p;
    double m_total=m_vertices[j].m_interface->m_slider_value.get_value()-m_poids;

    m_vertices[j].m_interface->m_slider_value.set_value(m_total);
    m_vertices[j].m_interface->m_label_value.set_message( std::to_string( (int)m_total ) );
    m_interface->m_top_box.update();
    m_total =  m_vertices[j].m_interface->m_slider_value.get_value();

    double m_value =  m_vertices[i].m_interface->m_slider_value.get_value();

    if(m_total==0.0)
    {
        if(key[KEY_S])
        {
            supprimer_Sommet(j);
        }

    }
}

void Graph::reglage_multi(int i,int j,double p)
{

    double m_poids=(m_vertices[i].m_interface->m_slider_value.get_value())*p;

    m_vertices[j].m_interface->m_slider_value.set_value(m_poids);
    m_vertices[j].m_interface->m_label_value.set_message( std::to_string( (int)m_poids ) );
    m_interface->m_top_box.update();
    m_poids =  m_vertices[j].m_interface->m_slider_value.get_value();
    double m_value =  m_vertices[i].m_interface->m_slider_value.get_value();

    if(m_poids==0.0)
    {
        if(key[KEY_S])
        {
            supprimer_Sommet(j);
        }

    }
}

void Graph::reglage_multi_p(int i, int k, int j, double p, double w)
{
    double m_poids=(m_vertices[i].m_interface->m_slider_value.get_value())*p;
    double m_weight =(m_vertices[k].m_interface->m_slider_value.get_value())*w;
    double m_total= m_poids+m_weight;

    m_vertices[j].m_interface->m_slider_value.set_value(m_total);
    m_vertices[j].m_interface->m_label_value.set_message( std::to_string( (int)m_total ) );
    m_interface->m_top_box.update();
    m_total =  m_vertices[j].m_interface->m_slider_value.get_value();
    double m_value1 = m_vertices[i].m_interface->m_slider_value.get_value();
    double m_value2 = m_vertices[k].m_interface->m_slider_value.get_value();

    if(m_total==0.0)
    {
        if(key[KEY_S])
        {
            supprimer_Sommet(j);
        }

    }
}

void Graph::reglage_plus_moins(int i, int k, int j, double p, double w)
{
    double m_poids=(m_vertices[i].m_interface->m_slider_value.get_value())/p;
    double m_weight =(m_vertices[k].m_interface->m_slider_value.get_value())*w;
    double m_total= m_vertices[j].m_interface->m_slider_value.get_value()+m_poids-m_weight;

    m_vertices[j].m_interface->m_slider_value.set_value(m_total);
    m_vertices[j].m_interface->m_label_value.set_message( std::to_string( (int)m_total ) );
    m_interface->m_top_box.update();
    m_total =  m_vertices[j].m_interface->m_slider_value.get_value();
    double m_value1 = m_vertices[i].m_interface->m_slider_value.get_value();
    double m_value2 = m_vertices[k].m_interface->m_slider_value.get_value();

    if(m_total==0.0)
    {
        if(key[KEY_S])
        {
            supprimer_Sommet(j);
        }

    }
}

void Graph::reglage_2moins_1plus(int i, int k,int l, int j, double p, double v,double w)
{
    double m_poids=(m_vertices[i].m_interface->m_slider_value.get_value())*p;
    double m_weight =(m_vertices[k].m_interface->m_slider_value.get_value())*v;
    double m_v=m_vertices[l].m_interface->m_slider_value.get_value()/w;
    double m_total= m_vertices[j].m_interface->m_slider_value.get_value()-(m_poids+m_weight)+m_v;

    m_vertices[j].m_interface->m_slider_value.set_value(m_total);
    m_vertices[j].m_interface->m_label_value.set_message( std::to_string( (int)m_total ) );
    m_interface->m_top_box.update();
    m_total =  m_vertices[j].m_interface->m_slider_value.get_value();
    double m_value1 = m_vertices[i].m_interface->m_slider_value.get_value();
    double m_value2 = m_vertices[k].m_interface->m_slider_value.get_value();

    if(m_total==0.0)
    {
        if(key[KEY_S])
        {
            supprimer_Sommet(j);
        }

    }
}

void Graph::reglage_2moins_1plus_p(int i, int k,int l, int j, double p, double v,double w)
{
    double m_poids=(m_vertices[i].m_interface->m_slider_value.get_value())/p;
    double m_weight =(m_vertices[k].m_interface->m_slider_value.get_value())/v;
    double m_v=m_vertices[l].m_interface->m_slider_value.get_value()*w;
    double m_total= m_vertices[j].m_interface->m_slider_value.get_value()-(m_poids+m_weight)+m_v;

    m_vertices[j].m_interface->m_slider_value.set_value(m_total);
    m_vertices[j].m_interface->m_label_value.set_message( std::to_string( (int)m_total ) );
    m_interface->m_top_box.update();
    m_total =  m_vertices[j].m_interface->m_slider_value.get_value();
    double m_value1 = m_vertices[i].m_interface->m_slider_value.get_value();
    double m_value2 = m_vertices[k].m_interface->m_slider_value.get_value();

    if(m_total==0.0)
    {
        if(key[KEY_S])
        {
            supprimer_Sommet(j);
        }

    }
}

void Graph::reglage_2plus_1moins(int i, int k,int l, int j, double p, double v,double w)
{
    double m_poids=(m_vertices[i].m_interface->m_slider_value.get_value())/p;
    double m_weight =(m_vertices[k].m_interface->m_slider_value.get_value())/v;
    double m_v=m_vertices[l].m_interface->m_slider_value.get_value()*w;
    double m_total= m_vertices[j].m_interface->m_slider_value.get_value()+(m_poids+m_weight)-m_v;

    m_vertices[j].m_interface->m_slider_value.set_value(m_total);
    m_vertices[j].m_interface->m_label_value.set_message( std::to_string( (int)m_total ) );
    m_interface->m_top_box.update();
    m_total =  m_vertices[j].m_interface->m_slider_value.get_value();
    double m_value1 = m_vertices[i].m_interface->m_slider_value.get_value();
    double m_value2 = m_vertices[k].m_interface->m_slider_value.get_value();

    if(m_total==0.0)
    {
        if(key[KEY_S])
        {
            supprimer_Sommet(j);
        }

    }
}

void Graph::reglage_2plus_1moins_p(int i, int k,int l, int j, double p, double v,double w)
{
    double m_poids=(m_vertices[i].m_interface->m_slider_value.get_value())*p;
    double m_weight =(m_vertices[k].m_interface->m_slider_value.get_value())*v;
    double m_v=m_vertices[l].m_interface->m_slider_value.get_value()/w;
    double m_total= m_vertices[j].m_interface->m_slider_value.get_value()+(m_poids+m_weight)-m_v;

    m_vertices[j].m_interface->m_slider_value.set_value(m_total);
    m_vertices[j].m_interface->m_label_value.set_message( std::to_string( (int)m_total ) );
    m_interface->m_top_box.update();
    m_total =  m_vertices[j].m_interface->m_slider_value.get_value();
    double m_value1 = m_vertices[i].m_interface->m_slider_value.get_value();
    double m_value2 = m_vertices[k].m_interface->m_slider_value.get_value();

    if(m_total==0.0)
    {
        if(key[KEY_S])
        {
            supprimer_Sommet(j);
        }

    }
}

void Graph::reglage_2plus_2moins(int i, int k,int l,int m, int j, double p, double v,double w, double z)
{
    double m_poids=(m_vertices[i].m_interface->m_slider_value.get_value())/p;
    double m_weight =(m_vertices[k].m_interface->m_slider_value.get_value())/v;
    double m_w=m_vertices[l].m_interface->m_slider_value.get_value()*w;
    double m_z=m_vertices[m].m_interface->m_slider_value.get_value()*z;
    double m_total= m_vertices[j].m_interface->m_slider_value.get_value()+(m_poids+m_weight)-(m_w+m_z);

    m_vertices[j].m_interface->m_slider_value.set_value(m_total);
    m_vertices[j].m_interface->m_label_value.set_message( std::to_string( (int)m_total ) );
    m_interface->m_top_box.update();
    m_total =  m_vertices[j].m_interface->m_slider_value.get_value();
    double m_value1 = m_vertices[i].m_interface->m_slider_value.get_value();
    double m_value2 = m_vertices[k].m_interface->m_slider_value.get_value();

    if(m_total==0.0)
    {
        if(key[KEY_S])
        {
            supprimer_Sommet(j);
        }

    }
}

void Graph::reglage_4plus(int i, int k,int l,int m, int j, double p, double v,double w, double z)
{
    double m_poids=(m_vertices[i].m_interface->m_slider_value.get_value())/p;
    double m_weight =(m_vertices[k].m_interface->m_slider_value.get_value())/v;
    double m_w=m_vertices[l].m_interface->m_slider_value.get_value()/w;
    double m_z=m_vertices[m].m_interface->m_slider_value.get_value()/z;
    double m_total= m_vertices[j].m_interface->m_slider_value.get_value()+(m_poids+m_weight+m_w+m_z);

    m_vertices[j].m_interface->m_slider_value.set_value(m_total);
    m_vertices[j].m_interface->m_label_value.set_message( std::to_string( (int)m_total ) );
    m_interface->m_top_box.update();
    m_total =  m_vertices[j].m_interface->m_slider_value.get_value();
    double m_value1 = m_vertices[i].m_interface->m_slider_value.get_value();
    double m_value2 = m_vertices[k].m_interface->m_slider_value.get_value();

    if(m_total==0.0)
    {
        if(key[KEY_S])
        {
            supprimer_Sommet(j);
        }

    }
}

void Graph::reglage_3moins(int i, int k,int l, int j, double p, double v,double w)
{
    double m_poids=(m_vertices[i].m_interface->m_slider_value.get_value())*p;
    double m_weight =(m_vertices[k].m_interface->m_slider_value.get_value())*v;
    double m_v=m_vertices[l].m_interface->m_slider_value.get_value()*w;
    double m_total= m_vertices[j].m_interface->m_slider_value.get_value()-(m_poids+m_weight+m_v);

    m_vertices[j].m_interface->m_slider_value.set_value(m_total);
    m_vertices[j].m_interface->m_label_value.set_message( std::to_string( (int)m_total ) );
    m_interface->m_top_box.update();
    m_total =  m_vertices[j].m_interface->m_slider_value.get_value();
    double m_value1 = m_vertices[i].m_interface->m_slider_value.get_value();
    double m_value2 = m_vertices[k].m_interface->m_slider_value.get_value();

    if(m_total==0.0)
    {
        if(key[KEY_S])
        {
            supprimer_Sommet(j);
        }

    }
}

void Graph::reglage_3moins_1plus(int i, int k,int l,int m, int j, double p, double v,double w,double x)
{
    double m_poids=(m_vertices[i].m_interface->m_slider_value.get_value())*p;
    double m_weight =(m_vertices[k].m_interface->m_slider_value.get_value())*v;
    double m_v=m_vertices[l].m_interface->m_slider_value.get_value()*w;
    double m_x=m_vertices[m].m_interface->m_slider_value.get_value()/x;
    double m_total= m_vertices[j].m_interface->m_slider_value.get_value()-(m_poids+m_weight+m_v)+m_x;

    m_vertices[j].m_interface->m_slider_value.set_value(m_total);
    m_vertices[j].m_interface->m_label_value.set_message( std::to_string( (int)m_total ) );
    m_interface->m_top_box.update();
    m_total =  m_vertices[j].m_interface->m_slider_value.get_value();
    double m_value1 = m_vertices[i].m_interface->m_slider_value.get_value();
    double m_value2 = m_vertices[k].m_interface->m_slider_value.get_value();

    if(m_total==0.0)
    {
        if(key[KEY_S])
        {
            supprimer_Sommet(j);
        }

    }
}

void Graph::reglage_3plus_1moins(int i, int k,int l,int m, int j, double p, double v,double w,double x)
{
    double m_poids=(m_vertices[i].m_interface->m_slider_value.get_value())/p;
    double m_weight =(m_vertices[k].m_interface->m_slider_value.get_value())/v;
    double m_v=m_vertices[l].m_interface->m_slider_value.get_value()/w;
    double m_x=m_vertices[m].m_interface->m_slider_value.get_value()*x;
    double m_total= m_vertices[j].m_interface->m_slider_value.get_value()+m_poids+m_weight+m_v-m_x;

    m_vertices[j].m_interface->m_slider_value.set_value(m_total);
    m_vertices[j].m_interface->m_label_value.set_message( std::to_string( (int)m_total ) );
    m_interface->m_top_box.update();
    m_total =  m_vertices[j].m_interface->m_slider_value.get_value();
    double m_value1 = m_vertices[i].m_interface->m_slider_value.get_value();
    double m_value2 = m_vertices[k].m_interface->m_slider_value.get_value();

    if(m_total==0.0)
    {
        if(key[KEY_S])
        {
            supprimer_Sommet(j);
        }

    }
}

void Graph::reglage_2plus_5moins(int i, int k,int l,int m,int n, int o, int q, int j, double p, double v,double w, double z, double x, double y,double s)
{
    double m_poids=(m_vertices[i].m_interface->m_slider_value.get_value())/p;
    double m_weight =(m_vertices[k].m_interface->m_slider_value.get_value())/v;
    double m_w=m_vertices[l].m_interface->m_slider_value.get_value()*w;
    double m_z=m_vertices[m].m_interface->m_slider_value.get_value()*z;
    double m_x=m_vertices[n].m_interface->m_slider_value.get_value()*x;
    double m_y=m_vertices[o].m_interface->m_slider_value.get_value()*y;
    double m_s=m_vertices[q].m_interface->m_slider_value.get_value()*s;
    double m_total= m_vertices[j].m_interface->m_slider_value.get_value()+(m_poids+m_weight)-(m_w+m_z+m_x+m_y+m_s);

    m_vertices[j].m_interface->m_slider_value.set_value(m_total);
    m_vertices[j].m_interface->m_label_value.set_message( std::to_string( (int)m_total ) );
    m_interface->m_top_box.update();
    m_total =  m_vertices[j].m_interface->m_slider_value.get_value();
    double m_value1 = m_vertices[i].m_interface->m_slider_value.get_value();
    double m_value2 = m_vertices[k].m_interface->m_slider_value.get_value();

    if(m_total==0.0)
    {
        if(key[KEY_S])
        {
            supprimer_Sommet(j);
        }

    }
}
