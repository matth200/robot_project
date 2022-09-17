#include "world.h"
using namespace std;

World::World(int w, int h){
    real_map.setSize(w,h);
}

World::~World(){

}

vector<Pos> *World::getPoints(){
    return &_points;
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
        //même chose mais vert
        for(int i(0);i<_carte_green.size();i++){
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
    //purple points
    file.read((char*)&size, sizeof(size));
    Pos pos;
    for(int i(0);i<size;i++){
        file.read((char*)&pos, sizeof(pos));
        _points.push_back(Pos(pos));
    }
    return true;
}

VirtualWorld* World::getRealWorld()
{
    return &real_map;
}

void World::buildVirtualWorld(){
    //lignes blanches
    for(int i(0);i<_carte.size();i++){
        real_map.addLine(_carte[i], WORLD_WHITE);
    }

    //même chose mais rouge
    for(int i(0);i<_carte_red.size();i++){
        real_map.addLine(_carte_red[i], WORLD_RED);
    }
    //même chose mais vert
    for(int i(0);i<_carte_green.size();i++){
        real_map.addLine(_carte_green[i], WORLD_GREEN);
    }
}


//virtualworldd
VirtualWorld::VirtualWorld(){
    _map = new vector<vector<vector<Element>>>();
}

VirtualWorld::~VirtualWorld(){
    delete _map;
    _map = NULL;
}

void VirtualWorld::addLine(Line &line, int status){
    Element elt;
    elt.p_line = &line;
    elt.state = status;

    int deltaX = line.x2-line.x1;
    int deltaY = line.y2-line.y1;
    int h = (int)sqrt(deltaX*deltaX + deltaY*deltaY) + 1;
    if(h!=0){
        double rapportX = (double(deltaX)/h),
               rapportY = (double(deltaY)/h);
        for(int i(0);i<h;i++){
            int x = int(line.x1+rapportX*i),
                y =  int(line.y1+rapportY*i);

            (*_map)[y][x].push_back(elt);
        }
    }
}

vector<Element> VirtualWorld::getDetection(Line &line, int &distance){
    int height = _map->size();
    int width = (*_map)[0].size();

    int deltaX = line.x2-line.x1;
    int deltaY = line.y2-line.y1;
    int h = (int)sqrt(deltaX*deltaX + deltaY*deltaY) + 1;
    if(h!=0){
        double rapportX = (double(deltaX)/h),
               rapportY = (double(deltaY)/h);
        for(int i(0);i<h;i++){
            int x = int(line.x1+rapportX*i),
                y =  int(line.y1+rapportY*i);

            if(0<=x&&x<width&&0<=y&&y<height&&(*_map)[y][x].size()>0){
                distance = i;
                return (*_map)[y][x];
            }
        }
    }
    return vector<Element>();
}

void VirtualWorld::setSize(int w, int h){
    _map->clear();
    vector<vector<Element>> liste;
    for(int j(0);j<h;j++){
        for(int i(0);i<w;i++)
        {
            vector<Element> liste_elt;
            liste.push_back(liste_elt);
        }
        _map->push_back(liste); 
        liste.clear();
    }
}

//universe

Universe::Universe(int w, int h):_indexWorld(-1), _indexPos(-1){
    _w = w;
    _h = h;
}

Universe::~Universe(){
    for(int i(0);i<_worlds.size();i++){
        delete _worlds[i];
    }
}

bool Universe::addLevel(const char *filename){
    World *world = new World(_w, _h);
    bool state = world->loadMap(filename);
    _worlds.push_back(world);
    if(!state){
		cout << "Erreur de chargement de la map" << endl;
	}else{
		cout << "Okay map." << endl;
	}
    return state;
}

void Universe::setLevel(int index){
    _indexWorld = index;
}
int Universe::getNbrWorld(){
    return _worlds.size();
}

int Universe::getNbrPos(){
    return getCurrentWorld()->getPoints()->size();
}
void Universe::setIndexPos(int index){
    _indexPos = index;
}
int Universe::getIndexPos(){
    return _indexPos;
}
Pos Universe::getCurrentPos(){
    return (*(getCurrentWorld()->getPoints()))[_indexPos];
}
int Universe::getLevel(){
    return _indexWorld;
}

void Universe::buildUniverse(){
    for(int i(0);i<_worlds.size();i++){
        _worlds[i]->buildVirtualWorld();
    }
}

bool Universe::isFinished(){
    return getLevel()==getNbrWorld();
}
void Universe::nextStep(){
    if(getIndexPos()<getNbrPos()){
        _indexPos++;
    }

    if(getIndexPos()==getNbrPos()){
        _indexPos = 0;
        _indexWorld++;
    }
}
void Universe::initStep(){
    setLevel(0);
    setIndexPos(0);
}
World* Universe::getCurrentWorld(){
    return _worlds[_indexWorld];
}