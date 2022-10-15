#ifndef INCLUDE_CAR
#define INCLUDE_CAR

#include <SDL/SDL.h>
#include <iostream>
#include <cmath>

#include "draw.h"
#include "capteur.h"
#include "m_learning.h"
#include "genetic_algorithm_const.h"
#include "world.h"

#include <thread>
#include <chrono>

typedef std::chrono::high_resolution_clock::time_point time_point;

#define COLOR_CAR SDL_MapRGB(screen->format, 200,10,100)
#define ROBOT_SPEED 5


class Trajectoire{
    public:
        Trajectoire();
        ~Trajectoire();
        double getDistanceDone();
        void setOrigin(int x, int y);
        void addPoint(int x, int y);
        void clearTrajectoire();
        void draw(SDL_Surface *screen, int x =0, int y=0);
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
        void draw(SDL_Surface *screen, int x = 0, int y = 0);
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
        void connectToUniverse(Universe *universe);
        void connectToWorld(World &world);
        void setBrain(MachineLearning *brain);
        void setUniversePos();
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
        Universe *_universe;
};





//arduino program
// class ManagerArduino{
//     public:
//         ManagerArduino();
//         ~ManagerArduino();
//         bool sleep();
//     protected:
//         time_point _tm;
// };


class RobotArduino: public Robot{
    public:
        RobotArduino();
        void updateMotor();
        void loop();
        void setup();
    protected:
        double _ard_rotation;
        int _ard_speedl;
        int _ard_speedr;
        double _ard_distance;
        double _ard_old_rotation;
        double _ard_done;

        time_point _old_time;

        //define on arduino
        double ARD_FULL_SPEED;
        double ARD_RAYON_BASE;
    private:
        void ard_updateRotation(double &rotation, double speed_l, double speed_r, unsigned long tm);
        LineD ard_setRotation(double rotation);
        double ard_getRotation(double x1, double y1, double x2, double y2);
        bool ard_goTo(double angle, double rotation, int &speed_r, int &speed_l,double &old_rotation, double &done, bool sens=false);
};

#endif