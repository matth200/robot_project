#ifndef INCLUDE_CAPTEUR
#define INCLUDE_CAPTEUR

#include <iostream>
#include <SDL/SDL.h>
#include "world.h"

//caractéristique d'un capteur ultrason
#define MAX_DIST 800
#define MAX_ANGLE 15.0/180.0*M_PI
#define NBR_LINE_DETECTION 3

class Capteur{
    public:
        Capteur();
        ~Capteur();
        void setDetection(int state);
        void setMaxDist(int distance);
        void setMaxAngle(double angle);
        void setNbrLineDetection(int nbr);

        void setRotation(double rotation);
        void setPos(double x, double y);
        double getDistance();
        void initDetectionLines();
        void draw(SDL_Surface *screen);
        void connectToWorld(World &world);
    protected:
        double _distance;
        double _x,_y;
        double _rotation;
        VirtualWorld *_real_map;
        std::vector<Line> _detectionLines;
        double _max_angle;
        int _max_dist, _nbr_line;
        int _detection_state;
};

#endif