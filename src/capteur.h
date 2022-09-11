#ifndef INCLUDE_CAPTEUR
#define INCLUDE_CAPTEUR

#include <iostream>
#include <SDL/SDL.h>
#include "world.h"

//caractéristique d'un capteur ultrason
#define MAX_DIST 400
#define MAX_ANGLE 15.0/180.0*M_PI

class Capteur{
    public:
        Capteur(double x=0.0, double y=0.0, double rotation=0.0);
        ~Capteur();
        void setRotation(double rotation);
        void setPos(double x, double y);
        double getDistance();
        void draw(SDL_Surface *screen);
        void connectToWorld(World &world);
    protected:
        double _distance;
        double _x,_y;
        double _rotation;
        VirtualWorld *_real_map;
};

#endif