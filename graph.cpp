#include "graph.h"
#include "grman/grman.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#define NIVEAU1 makecol(0,0,0)
#define NIVEAU2 makecol(255,0,0)
#define NIVEAU3 makecol(0,0,255)
#define REGLE_DU_JEU makecol(0,255,0)
#define QUITTER makecol(255,255,0)

using namespace std;

/***************************************************
                    VERTEX
****************************************************/

/// Le constructeur met en place les �l�ments de l'interface
VertexInterface::VertexInterface(int idx, int x, int y, std::string pic_name, int pic_idx)
{
    // La boite englobante
    m_top_box.set_pos(x, y);
    m_top_box.set_dim(130, 100);
    m_top_box.set_moveable();

    // Le slider de r�glage de valeur
    m_top_box.add_child( m_slider_value );
    m_slider_value.set_range(0.0 , 100.0); // Valeurs arbitraires, � adapter...
    m_slider_value.set_dim(20,80);
    m_slider_value.set_gravity_xy(grman::GravityX::Left, grman::GravityY::Up);

    // Label de visualisation de valeur
    m_top_box.add_child( m_label_value );
    m_label_value.set_gravity_xy(grman::GravityX::Left, grman::GravityY::Down);

    // Une illustration...
    if (pic_name!="")
    {
        m_top_box.add_child( m_img );
        m_img.set_pic_name(pic_name);
        m_img.set_pic_idx(pic_idx);
        m_img.set_gravity_x(grman::GravityX::Right);
    }

    // Label de visualisation d'index du sommet dans une boite
    m_top_box.add_child( m_box_label_idx );
    m_box_label_idx.set_gravity_xy(grman::GravityX::Right, grman::GravityY::Down);
    m_box_label_idx.set_dim(20,12);
    m_box_label_idx.set_bg_color(BLANC);

    m_box_label_idx.add_child( m_label_idx );
    m_label_idx.set_message( std::to_string(idx) );
}

/// Gestion du Vertex avant l'appel � l'interface
void Vertex::pre_update()
{
    if (!m_interface)
        return;
    /// Copier la valeur locale de la donn�e m_value vers le slider associ�
    m_interface->m_slider_value.set_value(m_value);
    /// Copier la valeur locale de la donn�e m_value vers le label sous le slider
    m_interface->m_label_value.set_message( std::to_string( (int)m_value) );
}

/// Gestion du Vertex apr�s l'appel � l'interface
void Vertex::post_update()
{
    if (!m_interface)
        return;
    /// Reprendre la valeur du slider dans la donn�e m_value locale
    m_value = m_interface->m_slider_value.get_value();
}

/***************************************************
                    EDGE
****************************************************/

/// Le constructeur met en place les �l�ments de l'interface
EdgeInterface::EdgeInterface(Vertex& from, Vertex& to)
{
    // Le WidgetEdge de l'interface de l'arc
    if ( !(from.m_interface && to.m_interface) )
    {
        std::cerr << "Error creating EdgeInterface between vertices having no interface" << std::endl;
        throw "Bad EdgeInterface instanciation";
    }
    m_top_edge.attach_from(from.m_interface->m_top_box);
    m_top_edge.attach_to(to.m_interface->m_top_box);
    m_top_edge.reset_arrow_with_bullet();

    // Une boite pour englober les widgets de r�glage associ�s
    m_top_edge.add_child(m_box_edge);
    m_box_edge.set_dim(24,60);
    m_box_edge.set_bg_color(BLANCBLEU);

    // Le slider de r�glage de valeur
    m_box_edge.add_child( m_slider_weight );
    m_slider_weight.set_range(0.0 , 100.0); // Valeurs arbitraires, � adapter...
    m_slider_weight.set_dim(16,40);
    m_slider_weight.set_gravity_y(grman::GravityY::Up);

    // Label de visualisation de valeur
    m_box_edge.add_child( m_label_weight );
    m_label_weight.set_gravity_y(grman::GravityY::Down);

}

/// Gestion du Edge avant l'appel � l'interface
void Edge::pre_update()
{
    if (!m_interface)
        return;
    /// Copier la valeur locale de la donn�e m_weight vers le slider associ�
    m_interface->m_slider_weight.set_value(m_weight);
    /// Copier la valeur locale de la donn�e m_weight vers le label sous le slider
    m_interface->m_label_weight.set_message( std::to_string( (int)m_weight ) );
}

/// Gestion du Edge apr�s l'appel � l'interface
void Edge::post_update()
{
    if (!m_interface)
        return;
    /// Reprendre la valeur du slider dans la donn�e m_weight locale
    m_weight = m_interface->m_slider_weight.get_value();
}

/***************************************************
                    GRAPH
****************************************************/

/// Ici le constructeur se contente de pr�parer un cadre d'accueil des
/// �l�ments qui seront ensuite ajout�s lors de la mise ne place du Graphe
GraphInterface::GraphInterface(int x, int y, int w, int h)
{
    //Totalité de l'écran d'affichage
    m_top_box.set_dim(1000,740);
    m_top_box.set_gravity_xy(grman::GravityX::Right, grman::GravityY::Up);

    m_top_box.add_child(m_Menu);
    m_Menu.set_dim(1000,740);
    m_Menu.set_gravity_xy(grman::GravityX::Left, grman::GravityY::Up);
    m_Menu.set_bg_color(BLANC);
    m_Menu.add_child(m_Menu_image);
    m_Menu_image.set_pic_name("Menu.jpg");

    m_Menu.add_child(m_Reseau1);
    m_Reseau1.set_dim(380, 110);
    m_Reseau1.set_pos(567, 200);

    m_Menu.add_child(m_Reseau2);
    m_Reseau2.set_dim(380, 110);
    m_Reseau2.set_pos(567, 342);

    m_Menu.add_child(m_Reseau3);
    m_Reseau3.set_dim(380, 110);
    m_Reseau3.set_pos(567, 484);

    m_Menu.add_child(m_MenuQuitter);
    m_MenuQuitter.set_dim(240, 58);
    m_MenuQuitter.set_pos(707, 626);

    /*//Barre d'informations et d'outils à gauche avec les boutons
    m_top_box.add_child(m_tool_box);
    m_tool_box.set_dim(200,720);
    m_tool_box.set_gravity_xy(grman::GravityX::Left, grman::GravityY::Up);
    m_tool_box.set_bg_color(BLANCBLEU);
    //Cadre où on placera les réseaux d'écosystèmes
    m_top_box.add_child(m_main_box);
    m_main_box.set_dim(800,720);
    m_main_box.set_gravity_xy(grman::GravityX::Right, grman::GravityY::Up);
    m_main_box.set_bg_color(BLANCBLEU);

    //Cadre et bouton pour ajouter un sommet
    m_tool_box.add_child(m_boutonAjout);
    m_boutonAjout.set_dim(70,50);
    m_boutonAjout.set_pos(10, 150);
    //m_boutonAjout.set_bg_color(NOIR);
    m_boutonAjout.add_child(m_boutonAjout_image);
    m_boutonAjout_image.set_pic_name("Ajout_sommet.jpg");
    //Cadre et bouton pour supprimer un ou plusieurs sommets
    m_tool_box.add_child(m_boutonSupp);
    m_boutonSupp.set_dim(70,50);
    m_boutonSupp.set_pos(110,150);
    //m_boutonSupp.set_bg_color(NOIR);
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
    m_algo2_4.set_pos(0,695);*/
}

/// M�thode sp�ciale qui construit un graphe arbitraire (d�mo)
/// Cette m�thode est � enlever et remplacer par un syst�me
/// de chargement de fichiers par exemple.
/// Bien s�r on ne veut pas que vos graphes soient construits
/// "� la main" dans le code comme �a.
/*void Graph::make_example()
{
    m_interface = std::make_shared<GraphInterface>(50, 0, 750, 600);
    // La ligne pr�c�dente est en gros �quivalente � :
    // m_interface = new GraphInterface(50, 0, 750, 600);

    /// Les sommets doivent �tre d�finis avant les arcs
    // Ajouter le sommet d'indice 0 de valeur 30 en x=200 et y=100 avec l'image clown1.jpg etc...
    add_interfaced_vertex(0, 30.0, 200, 100, "Homme.jpg");
    add_interfaced_vertex(1, 60.0, 400, 100, "Loup.jpg");
    add_interfaced_vertex(2,  50.0, 200, 300, "Lapin.jpg");
    add_interfaced_vertex(3,  0.0, 400, 300, "Decomposeurs.jpg");
    add_interfaced_vertex(4,  100.0, 600, 300, "Sels_mineraux.jpg");
    add_interfaced_vertex(5,  100.0, 100, 500, "Vegetation.jpg");
    add_interfaced_vertex(6,  100.0, 300, 500, "Soleil_pluie.jpg");
    add_interfaced_vertex(7,  100.0, 500, 500, "Campagnols.jpg");
    add_interfaced_vertex(8,  100.0, 600, 500, "Buse.jpg");

    /// Les arcs doivent �tre d�finis entre des sommets qui existent !
    // AJouter l'arc d'indice 0, allant du sommet 1 au sommet 2 de poids 50 etc...
    add_interfaced_edge(0, 1, 0, 300.0);
    add_interfaced_edge(1, 2, 1, 2.0);
    add_interfaced_edge(2, 2, 3, 1.0);
    add_interfaced_edge(3, 3, 4, 0.0);
    add_interfaced_edge(4, 4, 5, 8.0);
    add_interfaced_edge(5, 6, 5, 8.0);
    add_interfaced_edge(6, 7, 5, 50.0);
    add_interfaced_edge(7, 7, 8, 20.0);
    add_interfaced_edge(8, 1, 3, 0.0);
    add_interfaced_edge(9, 7, 1, 0.0);
    add_interfaced_edge(10, 2, 5, 80.0);
}
*/
/// La m�thode update � appeler dans la boucle de jeu pour les graphes avec interface
void Graph::update()
{
    m_interface = std::make_shared<GraphInterface>(50, 0, 750, 600);

    m_interface->m_top_box.update();

    if (m_interface->m_Reseau1.clicked())
    {
        std::cout<<"Reseau numero 1" << std::endl;
    }
    if (m_interface->m_Reseau2.clicked())
    {
        std::cout<<"Reseau numero 2" << std::endl;
    }
    if (m_interface->m_Reseau3.clicked())
    {
        std::cout<<"Reseau numero 3" << std::endl;
    }
    if (m_interface->m_MenuQuitter.clicked())
    {
        std::cout<<"Demande de quitter l'interface!" << std::endl;
    }

    

    if (m_interface->m_boutonAjout.clicked())
    {
        std::cout<<"Demande d'ajout de sommet !" << std::endl;
    }

    if (m_interface->m_boutonSupp.clicked())
    {
        std::cout<<"Demande de suppression de sommet!" << std::endl;
    }

    if (m_interface->m_boutonCharger.clicked())
    {
        std::cout<<"Demande de chargement d'un reseau!" << std::endl;
    }

    if (m_interface->m_boutonSauvegarder.clicked())
    {
        std::cout<<"Demande de sauvegarde!" << std::endl;
    }

    if (m_interface->m_boutonQuitter.clicked())
    {
        std::cout<<"Demande de quitter le reseau!" << std::endl;
    }

    if (m_interface->m_boutonAlgoChoix.clicked())
    {
        std::cout<<"Demande d'algorithme 2!" << std::endl;
    }

    if (!m_interface)
        return;

    for (auto &elt : m_vertices)
        elt.second.pre_update();
    for (auto &elt : m_edges)
        elt.second.pre_update();

    m_interface->m_top_box.update();

    for (auto &elt : m_vertices)
        elt.second.post_update();
    for (auto &elt : m_edges)
        elt.second.post_update();
}

/// Aide � l'ajout de sommets interfac�s
void Graph::add_interfaced_vertex(int idx, double value, int x, int y, std::string pic_name)
{
    if ( m_vertices.find(idx)!=m_vertices.end() )
    {
        std::cerr << "Error adding vertex at idx=" << idx << " already used..." << std::endl;
        throw "Error adding vertex";
    }
    // Cr�ation d'une interface de sommet
    VertexInterface *vi = new VertexInterface(idx, x, y, pic_name);
    // Ajout de la top box de l'interface de sommet
    m_interface->m_main_box.add_child(vi->m_top_box);
    // On peut ajouter directement des vertices dans la map avec la notation crochet :
    m_vertices[idx] = Vertex(value, vi);
}

/// Aide � l'ajout d'arcs interfac�s
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

/*void Graph::recuperation1()
{
   // ouvrir le fichier
   m_interface = std::make_shared<GraphInterface>(50, 0, 750, 600);

   // double value;
    //int m_x,m_y;
    //string nom;
    //double poid;
    //int a,d;
    std::ifstream fichierA("arete1.txt",std::ios::in);
   std::ifstream fichierS("sommet1.txt",std::ios::in);
   // verifier l'ouverture
   if(fichierS)
   {
       fichierS>>m_ordre;
       for(unsigned int i=0;i< m_ordre;i++)
       {
        fichierS>>i>>m_vertices[i].m_value>>m_vertices[i].m_interface->m_top_box.get_posx()>>m_vertices[i].m_interface->m_top_box.set_posy()>>m_vertices[i].m_nom;
        add_interfaced_vertex(i,m_vertices[i].m_value,m_vertices[i].m_interface->m_top_box.get_posx(),m_vertices[i].m_interface->m_top_box.get_posy(),m_vertices[i].m_nom);
       }

   }

    if(fichierA)
   {
       fichierA>>m_arete;
       for(unsigned int i=0;i< m_arete;i++)
       {
        fichierA>>i>>m_edges[i].m_from>>m_edges[i].m_to>>m_edges[i].m_weight;
        add_interfaced_edge(i,m_edges[i].m_from,m_edges[i].m_to,m_edges[i].m_weight);
       }

   }

else // si il y a une erreur
    cout << " error" << endl;

   //fermer le fichier
   fichierS.close();
   fichierA.close();

}
*/

void Graph::recuperation2()
{
   // ouvrir le fichier
   m_interface = std::make_shared<GraphInterface>(50, 0, 750, 600);

    double value;
    int m_x,m_y;
    string nom;
    double poid;
    int ar,de;
    std::ifstream fichierA("arete1.txt",std::ios::in);
   std::ifstream fichierS("sommet1.txt",std::ios::in);
   int nbsommet;
   int nbaretes;
   Edge a;
   Vertex s;
   // verifier l'ouverture
   if(fichierS)
   {
       fichierS>>nbsommet;
       m_ordre=nbsommet;
       for(unsigned int i=0;i< nbsommet;i++)
       {
        fichierS>>i>>value>>m_x>>m_y>>nom;
        s.m_id=i;
        s.m_value=value;
        s.m_x=m_x;
        s.m_Y=m_y;
        s.m_nom=nom;

        add_interfaced_vertex(i,value,m_x,m_y,nom);
       }

   }

    if(fichierA)
   {
       fichierA>>nbaretes;
       m_arete=nbaretes;
       for(unsigned int i=0;i< nbaretes;i++)
       {
        fichierA>>i>>de>>ar>>value;
        a.m_i=i;
        a.m_weight=value;
        a.m_from=de;
        a.m_to=ar;
        add_interfaced_edge(i,de,ar,value);
       }

   }

else // si il y a une erreur
    cout << " error" << endl;

   //fermer le fichier
   fichierS.close();
   fichierA.close();

}

void Graph::recuperation3()
{
   // ouvrir le fichier
   m_interface = std::make_shared<GraphInterface>(50, 0, 750, 600);

    double value;
    int m_x,m_y;
    string nom;
    double poid;
    int ar,de;
    std::ifstream fichierA("arete1.txt",std::ios::in);
   int nbsommet;
   int nbaretes;
   Edge a;
   Vertex s;
   // verifier l'ouverture
   if(fichierA)
   {
       fichierA>>nbsommet;
       m_ordre=nbsommet;
        fichierA>>nbaretes;
       m_arete=nbaretes;
       for(unsigned int i=0;i< nbsommet;i++)
       {
        fichierA>>i>>value>>m_x>>m_y>>nom;
        s.m_id=i;
        s.m_value=value;
        s.m_x=m_x;
        s.m_Y=m_y;
        s.m_nom=nom;
        add_interfaced_vertex(i,value,m_x,m_y,nom);
       }
       for(unsigned int i=0;i< nbaretes;i++)
       {
        fichierA>>i>>de>>ar>>value;
        a.m_i=i;
        a.m_weight=value;
        a.m_from=de;
        a.m_to=ar;
        add_interfaced_edge(i,de,ar,value);
       }

   }


else // si il y a une erreur
    cout << " error" << endl;

   //fermer le fichier
   fichierA.close();

}

void Graph::sauvegarde1()
{
   // ouvrir le fichier
   m_interface = std::make_shared<GraphInterface>(50, 0, 750, 600);

    ofstream fichierS("sommet1.txt", ios::out | ios::ate);
    ofstream fichierA("arete1.txt", ios::out | ios::ate);
   // verifier l'ouverture
   if(fichierS)
{
       fichierS<<m_ordre<<endl;
       for(int i=0;i<m_ordre;i++)
       {
        fichierS<<i<<" "<<m_vertices[i].m_value<<" "<<m_vertices[i].m_interface->m_top_box.get_posx()<<" "<<m_vertices[i].m_interface->m_top_box.get_posy()<<" "<<m_vertices[i].m_nom<<endl;
       }
   }

    if(fichierA)
   {
       fichierA<<m_arete<<endl;
       for(int i=0;i< m_arete;i++)
       {
        fichierS<<i<<" "<<m_edges[i].m_from<<" "<<m_edges[i].m_to<<" "<<m_edges[i].m_weight<<endl;
       }

   }

else // si il y a une erreur
    cout << " error" << endl;

   //fermer le fichier
   fichierS.close();
   fichierA.close();

}

void Graph::sauvegarde2()
{
   // ouvrir le fichier
   m_interface = std::make_shared<GraphInterface>(50, 0, 750, 600);

    ofstream fichierA("arete1.txt", ios::out | ios::ate);
   // verifier l'ouverture
    if(fichierA)
   {

        fichierA<<m_ordre<<endl;
        fichierA<<m_arete<<endl;
         for(int i=0;i<m_ordre;i++)
       {
        fichierA<<i<<" "<<m_vertices[i].m_value<<" "<<m_vertices[i].m_interface->m_top_box.get_posx()<<" "<<m_vertices[i].m_interface->m_top_box.get_posy()<<" "<<m_vertices[i].m_nom<<endl;
       }


       for(int i=0;i< m_arete;i++)
       {
        fichierA<<i<<" "<<m_edges[i].m_from<<" "<<m_edges[i].m_to<<" "<<m_edges[i].m_weight<<endl;
       }

   }

else // si il y a une erreur
    cout << " error" << endl;

   //fermer le fichier
   fichierA.close();

}

void Graph::sauvegarde3()
{
   // ouvrir le fichier
   m_interface = std::make_shared<GraphInterface>(50, 0, 750, 600);

    double value;
    int m_x,m_y;
    string nom;
    double poid;
    int a,d;
    ofstream fichierS("sommet3.txt", ios::out | ios::trunc);
    ofstream fichierA("arete3.txt", ios::out | ios::trunc);
   int nbsommet;
   int nbaretes;
   // verifier l'ouverture
   if(fichierS)
   {
       fichierS <<nbsommet<<endl;
       for(unsigned int i=0;i< nbsommet;i++)
       {
        add_interfaced_vertex(i,value,m_x,m_y,nom);
        fichierS << i<<value<<m_x<<m_y<<nom<<endl;

       }

   }

    if(fichierA)
   {
       fichierA<<nbaretes<<endl;
       for(unsigned int i=0;i< nbaretes;i++)
       {
        add_interfaced_edge(i,d,a,value);
        fichierA<<i<<d<<a<<value<<endl;

       }

   }

else // si il y a une erreur
    cout << " error" << endl;

   //fermer le fichier
   fichierS.close();
   fichierA.close();

}

void Graph::rajout1(int idx,int edx) // idx numero sommet , edx numero aretes
{
string raj;
cout<<"Vous pouvez rajouter soit insectes ou serpents"<<endl;
cout<<"Tapez le nom du sommet à rajouter"<<endl;
cin>>raj;
    if(raj=="insectes")
    {int iidx=idx+1;
    edx=edx+1;
    add_interfaced_vertex(iidx,50.0,200,100,"Insectes.jpg");
    add_interfaced_edge(edx,idx,iidx,40.0);
    }
    else if(raj=="serpents")
    {int iidx=idx+1;
    edx=edx+1;
    add_interfaced_vertex(iidx,20.0,200,100,"Serpents.jpg");
    add_interfaced_edge(edx,idx,iidx,40.0);
    }
    else
    {
    cout<<"nom incorrect"<<endl;
    }
}

void Graph::rajout2(int idx,int edx) // idx numero sommet , edx numero aretes
{
string raj;
cout<<"Vous pouvez rajouter soit bebe ou sportifs"<<endl;
cout<<"Tapez le nom du sommet à rajouter"<<endl;
cin>>raj;
    if(raj=="bebe")
    {int iidx=idx+1;
    edx=edx+1;
    add_interfaced_vertex(iidx,50.0,200,100,"Bebe.jpg");
    add_interfaced_edge(edx,idx,iidx,40.0);
    }
    else if(raj=="sportifs")
    {int iidx=idx+1;
    edx=edx+1;
    add_interfaced_vertex(iidx,20.0,200,100,"Sportifs.jpg");
    add_interfaced_edge(edx,idx,iidx,40.0);
    }
    else
    {
    cout<<"nom incorrect"<<endl;
    }
}

void Graph::rajout3(int idx,int edx) // idx numero sommet , edx numero aretes
{
string raj;
cout<<"Vous pouvez rajouter soit Homme ou pieuvres"<<endl;
cout<<"Tapez le nom du sommet à rajouter"<<endl;
cin>>raj;
    if(raj=="Homme")
    {int iidx=idx+1;
    edx=edx+1;
    add_interfaced_vertex(iidx,50.0,200,100,"Homme3.jpg");
    add_interfaced_edge(edx,idx,iidx,40.0);
    }
    else if(raj=="pieuvres")
    {int iidx=idx+1;
    edx=edx+1;
    add_interfaced_vertex(iidx,20.0,200,100,"Pieuvres.jpg");
    add_interfaced_edge(edx,idx,iidx,40.0);
    }
    else
    {
    cout<<"nom incorrect"<<endl;
    }
}

/// edx index of edge to remove
void Graph::Supprimer_edge(int edx)
{
    /// référence vers le Edge à enlever
    Edge &remed=m_edges.at(edx);

    /// test : on a bien des éléments interfacés
    if (m_interface && remed.m_interface)
    {
        m_interface->m_main_box.remove_child( remed.m_interface->m_top_edge );
    }

    /// Il reste encore à virer l'arc supprimé de la liste des entrants et sortants des 2 sommets to et from !
    /// References sur les listes de edges des sommets from et to
    std::vector<int> &vefrom = m_vertices[remed.m_from].m_out;
    std::vector<int> &veto = m_vertices[remed.m_to].m_in;
    vefrom.erase( std::remove( vefrom.begin(), vefrom.end(), edx ), vefrom.end() );
    veto.erase( std::remove( veto.begin(), veto.end(), edx ), veto.end() );

    /// Le Edge ne nécessite pas non plus de delete car on n'a pas fait de new (sémantique par valeur)
    /// Il suffit donc de supprimer l'entrée de la map pour supprimer à la fois l'Edge et le EdgeInterface
    /// mais malheureusement ceci n'enlevait pas automatiquement l'interface top_edge en tant que child de main_box !
    m_edges.erase( edx );


}

void Graph::supprimer_Sommet(int idx)
{
        /// référence vers le Edge à enlever
    Vertex &remed=m_vertices.at(idx);

   vector <int> supp;
   // pour trouver les aretes correspondant au sommet
   for(int i=0; i<remed.m_in.size(); i++)
   {
       supp.push_back(remed.m_in[i]);
   }
   for(int i=0; i<remed.m_out.size();i++)
   {
       supp.push_back(remed.m_out[i]);
   }
   // on supprimer les aretes trouvé
   for(int i=0;i<supp.size();i++)
   {
       Supprimer_edge(supp.at(i));
   }
       if (m_interface && remed.m_interface)
    {
        m_interface->m_main_box.remove_child( remed.m_interface->m_top_box );
    }

    m_vertices.erase( idx );
}

void Graph::menu()
{
   //Déclaration des variables pour le menu
    int couleurPixel;
    int reglesbool = 0;
    int niveau = 0;

    //Déclaration des bitmap pour le menu
    BITMAP* buffer;
    BITMAP* menu;
    BITMAP* carteChoix;
      //Initialisation Allegro
    allegro_init();
    install_keyboard();
    install_mouse();
    set_color_depth(desktop_color_depth());
    if(set_gfx_mode(GFX_AUTODETECT_WINDOWED, 1200, 600, 0, 0)!=0)
        {
        allegro_message("erreur graphique");
        allegro_exit();
        exit(EXIT_FAILURE);
        }
    show_mouse(screen); //Fin de l'initialisation Allegro
    //Chargement et création des bitmaps utiles au menu
    buffer = create_bitmap(SCREEN_W,SCREEN_H);
    menu = load_bitmap ("menu.bmp", NULL);
    carteChoix = load_bitmap ("carteChoix.bmp", NULL);

        //Affichage de la bitmap pour la détection du clic par rapport au choix dans le menu
    draw_sprite(buffer,carteChoix,0,0);

      //Début de la boucle de jeu
    while(!key[KEY_ESC])
    {
        //Booléen pour détecter le clic de la souris
        if (reglesbool == 0)
        {
            //couleurPixel prend la valeur de la couleur au niveau du clic de la souris
            couleurPixel = getpixel(carteChoix,mouse_x,mouse_y);
            //Affichage des différentes bitmaps
            draw_sprite(buffer,carteChoix,0,0);
            draw_sprite(buffer,menu,0,0);

            //Quand on clique
            if (mouse_b & 1)
            {
                //Si la couleur détectée est blanche, on appelle le sous programme du jeu du niveau1
                 if(couleurPixel == NIVEAU1 || niveau == 1)
                {
                     cout<<"OK"<<endl;
                }
                //Si la couleur détectée est rouge, on appelle le sous programme du jeu du niveau2
                else if(couleurPixel == NIVEAU2 || niveau == 2)
                {
                    cout<<"OK2"<<endl;
                }
                //Si la couleur détectée est bleu, on appelle le sous programme du jeu du niveau3
                else if(couleurPixel == NIVEAU3 || niveau == 3)
                {
                    cout<<"OK3"<<endl;
                }
                //Si la couleur détectée est verte
                //Si la couleur détectée est jaune
                else if(couleurPixel == QUITTER)
                {
                    //Quand on appuye sur le quitter, on libére toutes les bitmaps
                    free(buffer);
                    free(menu);
                    free(carteChoix);
                    allegro_exit();
                    exit(0);
                }
            }
        }

        //Quand le niveau est différent de 0
        if (!niveau) {
            //Affichage du buffer sur le screen
            blit(buffer,screen,0,0,0,0, SCREEN_W, SCREEN_H);
        }
        //On libère le buffer
        clear_bitmap(buffer);
    }

    rest(20);
}
