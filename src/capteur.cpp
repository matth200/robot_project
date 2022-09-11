#include "capteur.h"
using namespace std;

Capteur::Capteur(double x, double y, double rotation):_x(x),_y(y),_rotation(rotation),_distance(MAX_DIST)
{
    _real_map = NULL;
    initDetectionLines();
}

Capteur::~Capteur()
{
}

void Capteur::initDetectionLines()
{
    //on ajoute des lignes de detection
    Line line;
    line.x1 = _x;
    line.y1 = _y;
    _detectionLines.clear();
    for(int i(0);i<NBR_LINE_DETECTION;i++){
        double angle = (_rotation-MAX_ANGLE/(NBR_LINE_DETECTION-1))+MAX_ANGLE/(NBR_LINE_DETECTION-1)*i;
        line.x2 = _x+MAX_DIST*cos(angle);
        line.y2 = _y-MAX_DIST*sin(angle);
        _detectionLines.push_back(line);
    }
}

void Capteur::setPos(double x, double y){
    _x = x;
    _y = y;
    initDetectionLines();
}

void Capteur::draw(SDL_Surface *screen){
    for(int i(0);i<_detectionLines.size();i++){
        drawLine(screen, _detectionLines[i], COLOR_WHITE);
    }
}

void Capteur::setRotation(double rotation){
    _rotation = rotation;
    initDetectionLines();
}
double Capteur::getDistance(){
    _distance = MAX_DIST;
    for(int i(0);i<_detectionLines.size();i++){
        int distance = 0;
        vector<Element> liste_elt = _real_map->getDetection(_detectionLines[i], distance);
        for(int j(0);j<liste_elt.size();j++){
            //detection des murs blancs
            if(liste_elt[j].state==WORLD_WHITE&&distance<_distance){
                _distance = distance;
            }
        }
    }
    return _distance;
}
void Capteur::connectToWorld(World &world){
    _real_map = world.getRealWorld();
}