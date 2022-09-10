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
    setRotation(_rotation);
}

void Car::setRotation(double rotation){
    _rotation = rotation;
    //on place le baton
    _line.x1 = (int)(_x+_rayon*cos(M_PI+_rotation));
    _line.y1 = (int)(_y-_rayon*sin(M_PI+_rotation));
    _line.x2 = (int)(_x+_rayon*cos(_rotation));
    _line.y2 = (int)(_y-_rayon*sin(_rotation));
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

double Car::getRotation(double x1, double y1, double x2, double y2){
    int deltaX = x2-x1;
    int deltaY = y1-y2;
    double rotation;
    if(deltaX!=0){
        rotation = atan((double)(deltaY)/deltaX);
    }else{
        //h n'est jamais nul ici
        cout << "test" << endl;
        double h = sqrt(deltaX*deltaX+deltaY*deltaY);
        rotation = asin((double)(deltaY)/h);
    }

    if(deltaX<0){
        rotation+=M_PI;
    }
    return rotation;
}

void Car::forward(){
    _angle_d = _rotation;
    _angle_g = _rotation;

    double x1 = _line.x1, y1 = _line.y1, x2 = _line.x2, y2 = _line.y2;

    //on tourne autour de la roue gaguche
    if(_motor2!=0){
        _angle_d+=_motor2/40.0;
        x2 = x1+2.0*_rayon*cos(_angle_d);
        y2 = y1-2.0*_rayon*sin(_angle_d);
    }
    _angle_g = _angle_d;
    
    if(_motor1!=0){
        _angle_g-=_motor1/40.0;
        x1 = x2+2.0*_rayon*cos(M_PI+_angle_g);
        y1 = y2-2.0*_rayon*sin(M_PI+_angle_g);
    }

    cout << _angle_g/M_PI*180.0 << "," << _angle_d/M_PI*180.0 <<endl;
    
    //on enregistre la position et la rotation
    _line.x1 = (int)x1;
    _line.y1 = (int)y1;
    _line.x2 = (int)x2;
    _line.y2 = (int)y2;

    int x,y;
    calcCenter(_line,x,y);
    _x = x;
    _y = y;
    _rotation = getRotation(x1,y1,x2,y2);
    cout << _rotation/M_PI*180.0 << endl;
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