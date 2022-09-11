#ifndef INCLUDE_CAPTEUR
#define INCLUDE_CAPTEUR

#include <iostream>
#include <SDL/SDL.h>

//caractéristique d'un capteur ultrason
#define MAX_DIST 400
#define MAX_ANGLE 15.0/180.0*M_PI

class Capteur{
    public:
        Capteur(double x, double y, double rotation=0.0);
        ~Capteur();
        void setRotation(double rotation);
        double getDistance();
        void draw(SDL_Surface *screen);
        void connectToWorld();
    protected:
        double _distance;
        double _x,_y;
        double _rotation;
};

#endif