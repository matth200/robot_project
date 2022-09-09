#include "car.h"
using namespace std;

Car::Car():_x(0),_y(0),_rotation(0),_motor1(0),_motor2(0),_rayon(20),_angle_d(0),_angle_g(0){
    setRotation(0);
}

Car::~Car(){

}

void Car::setPos(int x, int y){
    _x  = x;
    _y = y;
    //on place le baton
    _line.x1 = (int)(_x+_rayon*cos(M_PI+_rotation));
    _line.y1 = (int)(_y-_rayon*sin(M_PI+_rotation));
    _line.x2 = (int)(_x+_rayon*cos(_rotation));
    _line.y2 = (int)(_y-_rayon*sin(_rotation));
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

void Car::calcCenter(Line line, int &x, int &y){
    x = (line.x1+line.x2)/2.0;
    y = (line.y1+line.y2)/2.0;
}

void Car::forward(){
    //on tourne autour de la roue gauche
    if(_motor2!=0){
        if(_angle_g!=0){
            _angle_d = 0;
        }
        _angle_d+=_motor2/40;
        _line.x2 = _line.x1+2*_rayon*cos(_angle_d);
        _line.y2 = _line.y1-2*_rayon*sin(_angle_d);
    }
    
    if(_motor1!=0){
        if(_angle_d!=0){
            _angle_g = 0;
        }
        _angle_g+=_motor1/40;
        _line.x1 = _line.x2+2*_rayon*cos(M_PI-_angle_g);
        _line.y1 = _line.y2-2*_rayon*sin(M_PI-_angle_g);
    }
    
    //on enregistre la position
    int x,y;
    calcCenter(_line,x,y);
    int deltaX = _line.x2-_line.x1;
    int deltaY = _line.y1-_line.y2;
    double rotation = 0;
    if(deltaX!=0){
        rotation = atan((double)(deltaY)/deltaX);
    }else{
        //h n'est jamais nul ici
        double h = sqrt(deltaX*deltaX+deltaY*deltaY);
        rotation = asin((double)(deltaY)/h);
    }
    _x = x;
    _y = y;
    cout << rotation << " " << deltaX << " " << deltaY << endl;
    setRotation(rotation);
}

void Car::draw(SDL_Surface *screen){
    //roue gauche
    drawCircle(screen,_x+(_rayon+5)*cos(M_PI+_rotation),_y-(_rayon+5)*sin(M_PI+_rotation),5,COLOR_CAR);
    //roue droite
    drawCircle(screen,_x+(_rayon+5)*cos(_rotation),_y-(_rayon+5)*sin(_rotation),5,COLOR_CAR);
    //corps
    drawCircle(screen, _x, _y, _rayon, COLOR_CAR);
    //Line
    drawLine(screen, _line, COLOR_CAR);
}