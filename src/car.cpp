#include "car.h"
using namespace std;

Car::Car():_x(0),_y(0),_rotation(0),_motor1(0),_motor2(0){

}

Car::~Car(){

}

void Car::setPos(int x, int y){
    _x  = x;
    _y = y;
}

void Car::setRotation(double rotation){
    _rotation = rotation;
}

void Car::setMotor1(double speed){
    _motor1 = speed;
}

void Car::setMotor2(double speed){
    _motor2 = speed;
}

void Car::forward(){
    
}

void Car::draw(SDL_Surface *screen){
    //roue gauche

    drawCircle(screen,_x+25*cos(M_PI+_rotation),_y-25*sin(M_PI+_rotation),5,SDL_MapRGB(screen->format, 200,10,100));
    //roue droite
    drawCircle(screen,_x+25*cos(_rotation),_y-25*sin(_rotation),5,SDL_MapRGB(screen->format, 200,10,100));
    //corps
    drawCircle(screen, _x, _y, 20, SDL_MapRGB(screen->format, 200,10,100));
}