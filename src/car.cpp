#include "car.h"
using namespace std;

Car::Car():_x(0),_y(0),_rotation(0),_motor1(0),_motor2(0),_rayon(20),_angle_d(0),_angle_g(0){
    setRotation(0);
}

Car::~Car(){

}

void Car::setPos(double x, double y){
    _x  = x;
    _y = y;
    //on place le baton
    setRotation(_rotation);
}

void Car::setRotation(double rotation){
    _rotation = rotation;
    //on place le baton
    _line.x1 =(_x+_rayon*cos(M_PI+_rotation));
    _line.y1 = (_y-_rayon*sin(M_PI+_rotation));
    _line.x2 = (_x+_rayon*cos(_rotation));
    _line.y2 = (_y-_rayon*sin(_rotation));
}

double Car::getRotation(){
    return _rotation;
}

void Car::setMotor1(double speed){
    _motor1 = speed;
}

void Car::setMotor2(double speed){
    _motor2 = speed;
}

void Car::calcCenter(double x1, double y1, double x2, double y2, double &x, double &y){
    x = (x1+x2)/2.0;
    y = (y1+y2)/2.0;
}

double Car::getRotation(double x1, double y1, double x2, double y2){
    double deltaX = x2-x1;
    double deltaY = y1-y2;
    double rotation;
    if(deltaX!=0){
        rotation = atan((double)(deltaY)/deltaX);
    }else{
        //h n'est jamais nul ici
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
    //on tourne autour de la roue gaguche
    if(_motor2!=0){
        _angle_d+=_motor2/40.0;
        _line.x2 = _line.x1+2.0*_rayon*cos(_angle_d);
        _line.y2 = _line.y1-2.0*_rayon*sin(_angle_d);
    }
    _angle_g = _angle_d;

    if(_motor1!=0){
        _angle_g-=_motor1/40.0;
        _line.x1 = _line.x2+2.0*_rayon*cos(M_PI+_angle_g);
        _line.y1 = _line.y2-2.0*_rayon*sin(M_PI+_angle_g);
    }
    
    //on enregistre la position et la rotation
    calcCenter(_line.x1,_line.y1,_line.x2,_line.y2,_x,_y);
    _rotation = getRotation(_line.x1,_line.y1,_line.x2,_line.y2);
}

void Car::draw(SDL_Surface *screen){
    //roue gauche
    drawCircle(screen,(int)(_x+(_rayon+5.0)*cos(M_PI+_rotation)),(int)(_y-(_rayon+5)*sin(M_PI+_rotation)),5,COLOR_CAR);
    //roue droite
    drawCircle(screen,(int)(_x+(_rayon+5.0)*cos(_rotation)),(int)(_y-(_rayon+5)*sin(_rotation)),5,COLOR_CAR);
    //corps
    drawCircle(screen, (int)(_x), (int)(_y), _rayon, COLOR_CAR);
    //Line
    drawLine(screen, _line, COLOR_CAR);
}


//Robot
Robot::Robot():Car()
{
    _brain = NULL;
    update();
}


void Robot::update(){
    forward();
    _capteur.setPos(_x,_y);
    _capteur.setRotation(_rotation+M_PI/2.0);

    if(_brain!=NULL){
        //on donne les infos au cerveau
        int rotation = int(double(int(_rotation/M_PI*180)%360)/360.0*255.0);
        //cout << "rot:" << rotation << endl;
        unsigned char data[2];
        data[0] = (unsigned char)(rotation);
        data[1] = (unsigned char)(int(double(_capteur.getDistance())/double(MAX_DIST)*255.0));
        _brain->setInput((char*)data);

        //on calcul
        _brain->calcul();
    }
}

void Robot::connectToWorld(World &world){
    _capteur.connectToWorld(world);
}

void Robot::setBrain(MachineLearning *brain){
    _brain = brain;
}

void Robot::draw(SDL_Surface *screen){
    Car::draw(screen);
    _capteur.draw(screen);
}
