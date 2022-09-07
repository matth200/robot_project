#include "world.h"
using namespace std;

World::World(){

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
}

bool World::loadMap(const char* filename){
    ifstream file(filename, ios::binary);
    if(!file.is_open()){
        return false;
    }

    int size=0;
    file.read((char*)&size,sizeof(size));
    Line line;
    for(int i(0);i<size;i++){
        file.read((char*)&line, sizeof(line));
        _carte.push_back(Line(line));
    }
    file.read((char*)&size, sizeof(size));
    for(int i(0);i<size;i++){
        file.read((char*)&line, sizeof(line));
        _carte_red.push_back(Line(line));
    }
    return true;
}