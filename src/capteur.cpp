#include "capteur.h"
using namespace std;

Capteur::Capteur(double x, double y, double rotation):_x(x),_y(y),_rotation(rotation),_distance(0)
{
    _real_map = NULL;
}

Capteur::~Capteur()
{
}

void Capteur::setPos(double x, double y){
    _x = x;
    _y = y;
}

void Capteur::draw(SDL_Surface *screen){
    drawLine(screen, _x,_y, _x+MAX_DIST*cos(_rotation-MAX_ANGLE/2.0),_y-MAX_DIST*sin(_rotation-MAX_ANGLE/2.0), COLOR_WHITE);
    drawLine(screen, _x,_y, _x+MAX_DIST*cos(_rotation),_y-MAX_DIST*sin(_rotation), COLOR_WHITE);
    drawLine(screen, _x,_y, _x+MAX_DIST*cos(_rotation+MAX_ANGLE/2.0),_y-MAX_DIST*sin(_rotation+MAX_ANGLE/2.0), COLOR_WHITE);
}

void Capteur::setRotation(double rotation){
    _rotation = rotation;
}
double Capteur::getDistance(){
    return _distance;
}
void Capteur::connectToWorld(World &world){
    _real_map = world.getRealWorld();
}