#ifndef INCLUDE_CAR
#define INCLUDE_CAR

#include <SDL/SDL.h>
#include <iostream>
#include <cmath>

#include "draw.h"
#include "capteur.h"
#include "m_learning.h"

#define COLOR_CAR SDL_MapRGB(screen->format, 200,10,100)
#define ROBOT_SPEED 5

struct Pos{
    int x, y;
};

class Trajectoire{
    public:
        Trajectoire();
        ~Trajectoire();
        double getDistanceDone();
        void setOrigin(int x, int y);
        void addPoint(int x, int y);
        void clearTrajectoire();
        void draw(SDL_Surface *screen);
    private:
        double distance(Pos pos1, Pos pos2);
    protected:
        std::vector<Pos> _liste;
        double _sum;
};


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
        Trajectoire _trajectoire;
};

class Robot: public Car{
    public:
        Robot();
        void draw(SDL_Surface *screen);
        void connectToWorld(World &world);
        void setBrain(MachineLearning *brain);
        bool isAlive();
        void update();
        void clearTick();
        double getDistanceDone();
        unsigned int getDuration();
        void setAlive(bool state);
        bool getWin();
    protected:
        Capteur _capteur_ext;
        Capteur _capteur;
        MachineLearning *_brain;
        bool _alive, _win;
        unsigned int _tick;
        unsigned char _memoire, _memoire2;
};

#endif