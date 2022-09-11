#include "capteur.h"
using namespace std;

Capteur::Capteur(double x, double y, double rotation):_x(x),_y(y),_rotation(rotation),_distance(0)
{

}

Capteur::~Capteur()
{
}

void Capteur::draw(SDL_Surface *screen){
    
}