#include "car.h"
using namespace std;



//Trajectoire
Trajectoire::Trajectoire(){
    _sum = 0.0;
}
Trajectoire::~Trajectoire(){

}

double Trajectoire::getDistanceDone(){
    // double sum = 0.0;
    // for(int i(0);i<_liste.size()-1;i++){
    //     Pos pos1, pos2;
    //     pos1.x = _liste[i].x;
    //     pos1.y = _liste[i].y;

    //     pos2.x = _liste[i+1].x;
    //     pos2.y = _liste[i+1].y;

    //     sum+= distance(pos1,pos2);
    // }

    //elle est calculé au fûr est à mesure qu'on ajoute des points
    //plus efficace
    //on enleve les calculs inutiles
    return _sum;
}
void Trajectoire::setOrigin(int x, int y){
    clearTrajectoire();

    Pos pos;
    pos.x = x;
    pos.y = y;
    _liste.push_back(pos);
}
void Trajectoire::addPoint(int x, int y){
    Pos pos;
    pos.x = x;
    pos.y = y;
    _liste.push_back(pos);

    //si _liste>=2 alors on peut ajouter les distances
    if(_liste.size()>1){
        Pos pos1, pos2;
        pos1.x = _liste[_liste.size()-2].x;
        pos1.y = _liste[_liste.size()-2].y;

        pos2.x = _liste[_liste.size()-1].x;
        pos2.y = _liste[_liste.size()-1].y;
        _sum+=distance(pos1,pos2);
    }
}
void Trajectoire::clearTrajectoire(){
    _liste.clear();
    _sum = 0;
}
void Trajectoire::draw(SDL_Surface *screen, int x, int y){
    int size = _liste.size()-1;
    for(int i(0);i<size;i++){
        Line line;
        line.x1 = _liste[i].x;
        line.y1 = _liste[i].y;

        line.x2 = _liste[i+1].x;
        line.y2 = _liste[i+1].y;
        drawLine(screen, line.x1-x, line.y1-y, line.x2-x, line.y2-y, COLOR_RED);
    }

}
double Trajectoire::distance(Pos pos1, Pos pos2){
    int x1 = pos1.x, y1 = pos1.y;
    int x2 = pos2.x, y2 = pos2.y;
    return sqrt(pow(x2-x1,2)+pow(y2-y1,2));
}

//CAR
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

void Car::draw(SDL_Surface *screen, int x, int y){
    //roue gauche
    drawCircle(screen,(int)(_x+(_rayon+5.0)*cos(M_PI+_rotation))-x,(int)(_y-(_rayon+5)*sin(M_PI+_rotation))-y,5,COLOR_CAR);
    //roue droite
    drawCircle(screen,(int)(_x+(_rayon+5.0)*cos(_rotation))-x,(int)(_y-(_rayon+5)*sin(_rotation))-y,5,COLOR_CAR);
    //corps
    drawCircle(screen, (int)(_x)-x, (int)(_y)-y, _rayon, COLOR_CAR);
    //Line
    drawLine(screen, _line.x1-x, _line.y1-y, _line.x2-x, _line.y2-y, COLOR_CAR);
    _trajectoire.draw(screen, x, y);
}


//Robot
Robot::Robot():Car()
{
    _brain = NULL;
    _universe = NULL;

    _capteur_ext.setMaxAngle(M_PI*2.0);
    _capteur_ext.setNbrLineDetection(6);
    _capteur_ext.setMaxDist(30);

    _memoire = 0;
    _memoire = 0;

    clearTick();

    update();
}

bool Robot::getWin(){
    return _win;
}

bool Robot::isAlive(){
    return _alive;
}

void Robot::clearTick(){
    _tick = 0;
    _alive = true;
    _win = false;
    _trajectoire.clearTrajectoire();
}

unsigned int Robot::getDuration(){
    return (unsigned int)(double(_tick)/40.0*1000);
}

void Robot::setAlive(bool state){
    _alive = state;
}

double Robot::getDistanceDone(){
    return _trajectoire.getDistanceDone();
}

void Robot::setUniversePos(){
    Pos pos = _universe->getCurrentPos();
    setPos(pos.x,pos.y);
}
void Robot::update(){
    //on connecte au monde en question
    if(_universe!=NULL){
        connectToWorld(*_universe->getCurrentWorld());
        //on centre le robot
        _universe->getCurrentWorld()->setView(_x-SCREEN_WIDTH/2,_y-SCREEN_HEIGHT/2);
    }

    //ensuite on avance
    forward();
    _capteur.setPos(_x,_y);
    _capteur.setRotation(_rotation+M_PI/2.0);

    _capteur_ext.setPos(_x,_y);
    _capteur_ext.setRotation(_rotation+M_PI/2.0);

    //Les lignes rouges et blanches tuent le robot
    //on detecte les lignes blanches
    _capteur_ext.setDetection(WORLD_WHITE);
    if(_capteur_ext.getDistance()<30){
        _alive = false;
    }
    //on detecte les lignes rouges
    _capteur_ext.setDetection(WORLD_RED);
    if(_capteur_ext.getDistance()<30){
        _alive = false;
    }

    //Alors que les lignes vertes lui permettent de remporter la victoire
    //on detecte les lignes vertes
    _capteur_ext.setDetection(WORLD_GREEN);
    if(_capteur_ext.getDistance()<30){
        _alive = false;
        _win = true;
    }


    if(_brain!=NULL){
        //on donne les infos au cerveau
        int rotation = int(double(int(_rotation/M_PI*180)%360)/360.0*255.0);
        //cout << "rot:" << rotation << endl;
        unsigned char data[2];
        data[0] = (unsigned char)(rotation);
        data[1] = (unsigned char)(int(double(_capteur.getDistance())/double(MAX_DIST)*255.0));
        data[2] = _memoire;
        data[3] = _memoire2;
        _brain->setInput((char*)data);

        //on calcul
        _brain->calcul();

        _memoire = int(_brain->getOutput(2)*255.0);
        _memoire2 = int(_brain->getOutput(3)*255.0);

        //on connecte les moteurs
        if(_alive){
            setMotor1(ROBOT_SPEED*2.0*(_brain->getOutput(0)-1.0/2.0));
            setMotor2(ROBOT_SPEED*2.0*(_brain->getOutput(1)-1.0/2.0));
        }else{
            setMotor1(0);
            setMotor2(0);
            //cout << _trajectoire.getDistanceDone() << endl;
        }
    }

    if(_tick%5==1){
        _trajectoire.addPoint(_x,_y);
    }

    //on augmente le tick
    _tick++;
}

void Robot::connectToUniverse(Universe *universe){
    _universe = universe;
    connectToWorld(*_universe->getCurrentWorld());
}
void Robot::connectToWorld(World &world){
    _capteur.connectToWorld(world);
    _capteur_ext.connectToWorld(world);
}

void Robot::setBrain(MachineLearning *brain){
    _brain = brain;
}

void Robot::draw(SDL_Surface *screen){
    Pos viewPos = _universe->getCurrentWorld()->getView();
    Car::draw(screen, viewPos.x, viewPos.y);
    _capteur.draw(screen, viewPos.x, viewPos.y);
    //_trajectoire.draw(screen);
    //_capteur_ext.draw(screen);
}



RobotArduino::RobotArduino():Robot(){
    ARD_RAYON_BASE = 10.0;
    ARD_FULL_SPEED = 1.0;
    setup();
}

void RobotArduino::setup(){
    _ard_rotation = 0;
    _ard_speedl = 0;
    _ard_speedr = 0;
    _ard_distance = 0;
    _old_time = chrono::high_resolution_clock::now();
}

void RobotArduino::loop(){
    //on récupére la distance
    _ard_distance = _capteur.getDistance();
    unsigned long tm = chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now()-_old_time).count();
    _old_time = chrono::high_resolution_clock::now();
    
    ard_updateRotation(_ard_rotation,_ard_speedl,_ard_speedr, tm);
    cout << "Rotation arduino: " << _ard_rotation/M_PI*180.0 << endl;
    //cout << "speedr:" << _ard_speedr << ", speedl:" << _ard_speedl << endl;


    ard_goTo(270.0/180.0*M_PI, _ard_rotation, _ard_speedl, _ard_speedr);

    //update motor
    updateMotor();
}

void RobotArduino::updateMotor(){
    setMotor1(double(_ard_speedl)/100.0*ARD_FULL_SPEED);
    setMotor2(double(_ard_speedr)/100.0*ARD_FULL_SPEED);
    Robot::update();
}

//fonction arduino
void RobotArduino::ard_updateRotation(double &rotation, double speed_l, double speed_r, unsigned long tm){
    //intégration de la vitesse de la roue pour trouver l'angle de rotation

    //modif sur la vitesse pour qu'elle soit angulaire
    speed_r = speed_r/100.0*ARD_FULL_SPEED/ARD_RAYON_BASE;
    speed_l = -speed_l/100.0*ARD_FULL_SPEED/ARD_RAYON_BASE;

    //on tourne autour de la roue gaguche
    double angle = rotation;
    Line line = ard_setRotation(angle);

    if(speed_r!=0){
        angle+=speed_r*double(tm)/1000.0;
        line.x2 = line.x1+2.0*ARD_RAYON_BASE*cos(angle);
        line.y2 = line.y1-2.0*ARD_RAYON_BASE*sin(angle);
    }

    if(speed_l!=0){
        angle-=speed_l*double(tm)/1000.0;
        line.x1 = line.x2+2.0*ARD_RAYON_BASE*cos(M_PI+angle);
        line.y1 = line.y2-2.0*ARD_RAYON_BASE*sin(M_PI+angle);
    }
    
    //on enregistre la rotation
    rotation = ard_getRotation(line.x1,line.y1,line.x2,line.y2);
}

Line RobotArduino::ard_setRotation(double rotation){
    Line line;
    //on place le baton
    line.x1 =(0+ARD_RAYON_BASE*cos(M_PI+rotation));
    line.y1 = (0-ARD_RAYON_BASE*sin(M_PI+rotation));
    line.x2 = (0+ARD_RAYON_BASE*cos(rotation));
    line.y2 = (0-ARD_RAYON_BASE*sin(rotation));
    return line;
}


double RobotArduino::ard_getRotation(double x1, double y1, double x2, double y2){
    return getRotation(x1,y1,x2,y2);
}

void RobotArduino::ard_goTo(double angle, double &rotation, int &speed_r, int &speed_l, bool sens){
  //si la différence est de plus de 5 degres, on fait tout bouger
  if(int(abs(angle-rotation)/M_PI*180.0)%360>5){
    speed_r = (sens)?100.0:-100.0;
    speed_l = (sens)?-100.0:100.0;
  }else{
    speed_r = 0;
    speed_l = 0;
  }
}