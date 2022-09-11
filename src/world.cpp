#include "world.h"
using namespace std;

World::World(){
    real_map.setSize(1500,1000);
}

World::~World(){

}

void World::draw(SDL_Surface *screen){
        //affichage de carte
        for(int i(0);i<_carte.size();i++){
            drawLine(screen, _carte[i], COLOR_WHITE);
        }

        //même chose mais rouge
        for(int i(0);i<_carte_red.size();i++){
            drawLine(screen, _carte_red[i], COLOR_RED);
        }
        //même chose mais rouge
        for(int i(0);i<_carte_green.size();i++){
            //cout << "affichage _carte_green x1:" << _carte_green[i].x1 << ", y1" << _carte_green[i].y1 << endl; 
            drawLine(screen, _carte_green[i], COLOR_GREEN);
        }
}

bool World::loadMap(const char* filename){
    ifstream file(filename, ios::binary);
    if(!file.is_open()){
        return false;
    }

    Line line;
    int size=0;
    //white lines
    file.read((char*)&size,sizeof(size));
    for(int i(0);i<size;i++){
        file.read((char*)&line, sizeof(line));
        _carte.push_back(Line(line));
    }
    //red lines
    file.read((char*)&size, sizeof(size));
    for(int i(0);i<size;i++){
        file.read((char*)&line, sizeof(line));
        _carte_red.push_back(Line(line));
    }
    //green lines
    file.read((char*)&size, sizeof(size));
    for(int i(0);i<size;i++){
        file.read((char*)&line, sizeof(line));
        _carte_green.push_back(Line(line));
    }
    return true;
}

VirtualWorld* World::getRealWorld()
{
    return &real_map;
}


//virtualworldd
VirtualWorld::VirtualWorld(){
    _map = new vector<vector<Element>>();
}

VirtualWorld::~VirtualWorld(){
    delete _map;
    _map = NULL;
}

void VirtualWorld::setSize(int w, int h){
    vector<Element> liste;
    for(int j(0);j<h;j++){
        for(int i(0);i<w;i++)
        {
            Element elt;
            elt.state = 0;
            elt.p_line = NULL;
            liste.push_back(elt);
        }
        _map->push_back(liste); 
        liste.clear();
    }
    //cout << (*_map)[0].size() << endl;
}

