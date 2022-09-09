#ifndef INCLUDE_CAR
#define INCLUDE_CAR

#include <SDL/SDL.h>
#include <iostream>
#include "draw.h"

#define COLOR_CAR SDL_MapRGB(screen->format, 200,10,100)

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
    private:
        void calcCenter(Line line, int &x, int &y);
    protected:
        int _x,_y;
        double _rotation;
        double _rayon;
        double _motor1, _motor2;
        Line _line;
};

#endif