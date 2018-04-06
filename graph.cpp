#include "graph.h"
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
    m_top_box.set_dim(130, 110);
    m_top_box.set_moveable();

    // Le slider de r�glage de valeur
    m_top_box.add_child( m_slider_value );
    m_slider_value.set_range(0.0 , 1000.0); // Valeurs arbitraires, � adapter...
    m_slider_value.set_dim(20,90);
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
    m_box_edge.set_dim(20,20);
    m_box_edge.set_bg_color(GRISCLAIR);

    // Le slider de r�glage de valeur
  //  m_box_edge.add_child( m_slider_weight );
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
    m_top_box.set_dim(1000,740);
    m_top_box.set_gravity_xy(grman::GravityX::Right, grman::GravityY::Up);

    m_top_box.add_child(m_tool_box);
    m_tool_box.set_dim(200,720);
    m_tool_box.set_gravity_xy(grman::GravityX::Left, grman::GravityY::Up);
    m_tool_box.set_bg_color(BLANCBLEU);

    m_top_box.add_child(m_main_box);
    m_main_box.set_dim(800,720);
    m_main_box.set_gravity_xy(grman::GravityX::Right, grman::GravityY::Up);
    m_main_box.set_bg_color(BLANCBLEU);
}

/// La m�thode update � appeler dans la boucle de jeu pour les graphes avec interface
void Graph::update()
{
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
    //  Cr�ation d'une interface de sommet
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

/// Recuperation
void Graph::recuperation1()
{
   // ouvrir le fichier
   m_interface = std::make_shared<GraphInterface>(50, 0, 750, 600);

    double value; //poid du sommet
    int m_x,m_y;//coordonnées du sommet
    string nom;//nom du sommet
    double poid;// poid de l'arete
    int ar,de; // numero de sommet d'arrive et de part
    std::ifstream fichierA("fiche1.txt",std::ios::in);
   int nbsommet;
   int nbaretes;
   // verifier l'ouverture
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
       }
       for(int i=0;i<nbaretes;i++)
       {
        fichierA>>i>>de>>ar>>poid;
        add_interfaced_edge(i,de,ar,poid);
       }
   }


else // si il y a une erreur
    cout << " error recuperation" << endl;

   //fermer le fichier
   fichierA.close();

}

void Graph::recuperation2()
{
   // ouvrir le fichier
   m_interface = std::make_shared<GraphInterface>(50, 0, 750, 600);

    double value; //poid du sommet
    int m_x,m_y;//coordonnées du sommet
    string nom;//nom du sommet
    double poid;// poid de l'arete
    int ar,de; // numero de sommet d'arrive et de part
    std::ifstream fichierA("fiche2.txt",std::ios::in);
   int nbsommet;
   int nbaretes;
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
        add_interfaced_vertex(i,value,m_x,m_y,nom);
       }
       for(unsigned int i=0;i< nbaretes;i++)
       {
        fichierA>>i>>de>>ar>>poid;
        add_interfaced_edge(i,de,ar,poid);
       }
cout <<value<<" "<<poid;
   }


else // si il y a une erreur
    cout << " error recuperation" << endl;

   //fermer le fichier
   fichierA.close();

}

void Graph::recuperation3()
{
   // ouvrir le fichier
   m_interface = std::make_shared<GraphInterface>(50, 0, 750, 600);

    double value; //poid du sommet
    int m_x,m_y;//coordonnées du sommet
    string nom;//nom du sommet
    double poid;// poid de l'arete
    int ar,de; // numero de sommet d'arrive et de part
    std::ifstream fichierA("fiche3.txt",std::ios::in);
   int nbsommet;
   int nbaretes;
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
        add_interfaced_vertex(i,value,m_x,m_y,nom);
       }
       for(unsigned int i=0;i< nbaretes;i++)
       {
        fichierA>>i>>de>>ar>>poid;
        add_interfaced_edge(i,de,ar,poid);
       }
   }
else // si il y a une erreur
    cout << " error recuperation" << endl;

   //fermer le fichier
   fichierA.close();

}

/// Sauvegarde
void Graph::sauvegarde1()
{
   // ouvrir le fichier
   m_interface = std::make_shared<GraphInterface>(50, 0, 750, 600);

    ofstream fichierA("fiche1.txt", ios::out | ios::ate);
   // verifier l'ouverture
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

else // si il y a une erreur
    cout << " error" << endl;

   //fermer le fichier
   fichierA.close();

}

void Graph::sauvegarde2()
{
   // ouvrir le fichier
   m_interface = std::make_shared<GraphInterface>(50, 0, 750, 600);

    ofstream fichierA("fiche2.txt", ios::out | ios::ate);
   // verifier l'ouverture
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

else // si il y a une erreur
    cout << " error" << endl;

   //fermer le fichier
   fichierA.close();

}

void Graph::sauvegarde3()
{
   // ouvrir le fichier
   m_interface = std::make_shared<GraphInterface>(50, 0, 750, 600);

    ofstream fichierA("fiche3.txt", ios::out | ios::ate);
   // verifier l'ouverture
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

else // si il y a une erreur
    cout << " error" << endl;

   //fermer le fichier
   fichierA.close();

}

/// Rajout
void Graph::rajout1() // idx numero sommet , edx numero aretes
{
 std::ifstream fichier("fiche1.txt",::ios::in);
   // verifier l'ouverture
   int nbsommet;
   int nbaretes;

    if(fichier)
 {
        fichier>>nbsommet;
        int idx=nbsommet-1;
        fichier>>nbaretes;
        int edx=nbaretes-1;


        string raj;
        cout<<"Vous pouvez rajouter soit insectes ou serpents"<<endl;
        cout<<"Tapez le nom du sommet à rajouter"<<endl;
        cin>>raj;
    if(raj=="insectes")
    {   int iidx=idx+1;
        edx=edx+1;
        add_interfaced_vertex(iidx,50.0,200,100,"Insectes.jpg");
        add_interfaced_edge(edx,idx,iidx,40.0);
    }
    else if(raj=="serpents")
    {   int iidx=idx+1;
        edx=edx+1;
        add_interfaced_vertex(iidx,20.0,200,100,"Serpents.jpg");
        add_interfaced_edge(edx,idx,iidx,40.0);
    }
    else
    {
    cout<<"nom incorrect"<<endl;
    }
 }
}

void Graph::rajout2() // idx numero sommet , edx numero aretes
{
    std::ifstream fichier("fiche2.txt",::ios::in);
   // verifier l'ouverture
   int nbsommet;
   int nbaretes;

    if(fichier)
 {
        fichier>>nbsommet;
        int idx=nbsommet-1;
        fichier>>nbaretes;
        int edx=nbaretes-1;


        string raj;
        cout<<"Vous pouvez rajouter soit bebe ou sportifs"<<endl;
        cout<<"Tapez le nom du sommet à rajouter"<<endl;
        cin>>raj;
    if(raj=="bebe")
    {   int iidx=idx+1;
        edx=edx+1;
        add_interfaced_vertex(iidx,50.0,200,100,"Bebe.jpg");
        add_interfaced_edge(edx,idx,iidx,40.0);
    }
    else if(raj=="sportifs")
    {   int iidx=idx+1;
        edx=edx+1;
        add_interfaced_vertex(iidx,20.0,200,100,"Sportifs.jpg");
        add_interfaced_edge(edx,idx,iidx,40.0);
    }
    else
    {
    cout<<"nom incorrect"<<endl;
    }
   }
}

void Graph::rajout3() // idx numero sommet , edx numero aretes
{
    std::ifstream fichier("fiche3.txt",::ios::in);
   // verifier l'ouverture
   int nbsommet;
   int nbaretes;

    if(fichier)
 {
        fichier>>nbsommet;
        int idx=nbsommet-1;
        fichier>>nbaretes;
        int edx=nbaretes-1;


        string raj;
        cout<<"Vous pouvez rajouter soit Homme ou pieuvres"<<endl;
        cout<<"Tapez le nom du sommet à rajouter"<<endl;
        cin>>raj;
    if(raj=="Homme")
    {   int iidx=idx+1;
        edx=edx+1;
        add_interfaced_vertex(iidx,50.0,200,100,"Homme3.jpg");
        add_interfaced_edge(edx,idx,iidx,40.0);
    }
    else if(raj=="pieuvres")
    {   int iidx=idx+1;
        edx=edx+1;
        add_interfaced_vertex(iidx,20.0,200,100,"Pieuvres.jpg");
        add_interfaced_edge(edx,idx,iidx,40.0);
    }
    else
    {
    cout<<"nom incorrect"<<endl;
    }
 }
}

/// Supprimer
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

    for(int i=0; i<m_vertices.size();i++)
    {
        if(m_vertices[i].m_id==idx)
        {
            m_vertices.erase(m_vertices[i].m_id);
        }

    }

}


/// Reglage
void Graph::reglage1()
{
           ///1kg = 10 1h=0,1
        reglage_multi_moins(1,0,0.008);//sommet 0
        reglage_div_p(2,7,1,2,40);//sommet 1
        reglage_plus_moins(5,1,2,0.8,2);//sommet 2
        reglage_2plus_1moins_p(1,2,4,3,12,9,1);//sommet 3
        reglage_moins_plus(5,3,4,0.016,1);// sommet 4
        reglage_2plus_2moins(6,4,7,2,5,0.8,0.016,0.5,0.8); // sommet 5
        reglage_2moins_1plus(8,1,5,7,20.0,40.0,0.5); // reglage sommet 7
        reglage_div(7,8,20.0); // reglage sommet 8
}

void Graph::reglage2()
{
            /// 1kg=10
        reglage_div_p(1,5,0,125.0,5.0);// reglage sommet 0
        reglage_2moins_1plus_p(0,2,4,1,125.0,1.0,0.1); // reglage sommet 1
        reglage_div(3,2,2.0); // reglage sommet 2
        reglage_multi_p(5,1,4,0.1,0.1); // reglage sommet 4
        reglage_multi(4,5,0.1); // reglage sommet 5
}

void Graph::reglage3()
{

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

    if(m_poids==0.0||m_value<p)
    { cout<<"Pour supprimer appuyez sur S "<<j<<endl;
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

    if(m_total==0.0||m_value<p)
    { cout<<"Pour supprimer appuyez sur S "<<j<<endl;
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
    { cout<<"Pour supprimer appuyez sur S "<<j<<endl;
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
    { cout<<"Pour supprimer appuyez sur S "<<j<<endl;
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
    { cout<<"Pour supprimer appuyez sur S "<<j<<endl;
        if(key[KEY_S])
        {
            supprimer_Sommet(j);
        }

    }
}

void Graph ::reglage_multi(int i,int j,double p)
{

    double m_poids=(m_vertices[i].m_interface->m_slider_value.get_value())*p;

    m_vertices[j].m_interface->m_slider_value.set_value(m_poids);
    m_vertices[j].m_interface->m_label_value.set_message( std::to_string( (int)m_poids ) );
    m_interface->m_top_box.update();
    m_poids =  m_vertices[j].m_interface->m_slider_value.get_value();
    double m_value =  m_vertices[i].m_interface->m_slider_value.get_value();

    if(m_poids==0.0)
    { cout<<"Pour supprimer appuyez sur S "<<j<<endl;
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
    { cout<<"Pour supprimer appuyez sur S "<<j<<endl;
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
    { cout<<"Pour supprimer appuyez sur S "<<j<<endl;
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
    { cout<<"Pour supprimer appuyez sur S "<<j<<endl;
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
    { cout<<"Pour supprimer appuyez sur S "<<j<<endl;
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
    { cout<<"Pour supprimer appuyez sur S "<<j<<endl;
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
    { cout<<"Pour supprimer appuyez sur S "<<j<<endl;
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
    { cout<<"Pour supprimer appuyez sur S "<<j<<endl;
        if(key[KEY_S])
        {
            supprimer_Sommet(j);
        }

    }
}

