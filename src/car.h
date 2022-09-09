#ifndef INCLUDE_CAR
#define INCLUDE_CAR

#include <SDL/SDL.h>
#include "draw.h"

class Car{
    public:
        Car();
        ~Car();
        void setMotor1(double speed);
        void setMotor2(double speed);
        void setPos(int x, int y);
        void forward();
        void setRotation(double rotation);
        void draw(SDL_Surface *screen);
    protected:
        int _x,_y;
        double _rotation;
        double _motor1, _motor2;
};

#endif