#ifndef INCLUDE_CAR
#define INCLUDE_CAR

#include <SDL/SDL.h>
#include <iostream>
#include "draw.h"
#include "capteur.h"
#include "m_learning.h"

#define COLOR_CAR SDL_MapRGB(screen->format, 200,10,100)
#define ROBOT_SPEED 5


class Car{
    public:
        Car();
        ~Car();
        void setMotor1(double speed);
        void setMotor2(double speed);
        void setPos(double x, double y);
        void forward();
        void setRotation(double rotation);
        double getRotation();
        double getRotation(double x1, double y1, double x2, double y2);
        void draw(SDL_Surface *screen);
    private:
        void calcCenter(double x1, double y1, double x2, double y2, double &x, double &y);
    protected:
        double _x,_y;
        double _rotation;
        double _angle_d,_angle_g;
        double _rayon;
        double _motor1, _motor2;
        LineD _line;
};

class Robot: public Car{
    public:
        Robot();
        void draw(SDL_Surface *screen);
        void connectToWorld(World &world);
        void setBrain(MachineLearning *brain);
        bool isAlive();
        void update();
    protected:
        Capteur _capteur;
        MachineLearning *_brain;
        bool _alive;
        //Capteur _capteur_ext;
};

#endif