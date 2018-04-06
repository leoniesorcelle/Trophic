#include "algo.h"
#include "graph.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
/*
void algo::reset_marquages()
{
    for (auto& v : m_vectives)
        v.set_marque(false);
}

void algo:: connexe(Graph *v, int s)
{
    v.reset_marquages();
    list<int> q;
    v.m_sommets[s].m_marque=true;
    q.push_back(s);
    while (!q.empty())
    {
        s = q.front();
        cout<<s<<" ";
        q.pop_front();
        for(auto i : v.m_sommets[s].m_adjacents)
        {
            if(!v.m_sommets[i].m_marque)
            {
                v.m_sommets[i].m_marque = true;
                q.push_back(i);
            }
        }
    }

}
*/
