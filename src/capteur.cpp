#include "capteur.h"
using namespace std;

Capteur::Capteur():_x(0),_y(0),_rotation(0),_distance(MAX_DIST)
{
    _real_map = NULL;
    _max_angle_rebound = 45.0/180.0*M_PI;

    _nbr_line = NBR_LINE_DETECTION;
    _max_dist = MAX_DIST;
    _max_angle = MAX_ANGLE;

    _detection_state = WORLD_WHITE;
    
    initDetectionLines();
}

Capteur::~Capteur()
{
}

void Capteur::setDetection(int state){
    _detection_state = state;
}

void Capteur::setMaxAngle(double angle){
    _max_angle = angle;
    initDetectionLines();
}

void Capteur::setMaxDist(int distance){
    _max_dist = distance;
    initDetectionLines();
}

void Capteur::setNbrLineDetection(int nbr){
    _nbr_line = nbr;
    initDetectionLines();
}

void Capteur::initDetectionLines()
{
    //on ajoute des lignes de detection
    Line line;
    line.x1 = _x;
    line.y1 = _y;
    _detectionLines.clear();
    for(int i(0);i<_nbr_line;i++){
        double angle = (_rotation-_max_angle/(_nbr_line-1))+_max_angle/(_nbr_line-1)*i;
        line.x2 = _x+_max_dist*cos(angle);
        line.y2 = _y-_max_dist*sin(angle);
        _detectionLines.push_back(line);
    }
}

void Capteur::setPos(double x, double y){
    _x = x;
    _y = y;
    initDetectionLines();
}

void Capteur::draw(SDL_Surface *screen){
    for(int i(0);i<_detectionLines.size();i++){
        drawLine(screen, _detectionLines[i], COLOR_WHITE);
    }
}

void Capteur::setRotation(double rotation){
    _rotation = rotation;
    initDetectionLines();
}

void Capteur::setMaxAngleRebound(double angle){
    _max_angle_rebound = angle;
}

double Capteur::getAngleTriangle(double a, double b, double c){
    return acos((a*a-b*b-c*c)/(-2*b*c));
}

double Capteur::getDistance(){
    _distance = _max_dist;
    for(int i(0);i<_detectionLines.size()&&_real_map!=NULL;i++){
        int distance = 0;
        Pos pos;
        vector<Element> liste_elt = _real_map->getDetection(_detectionLines[i], distance, &pos.x, &pos.y);
        for(int j(0);j<liste_elt.size();j++){
            
            //detection des murs blancs
            if(liste_elt[j].state==_detection_state&&distance<_distance){

                double angle_detection = getAngleDetection(liste_elt[j], pos);
                if(abs(int(angle_detection/M_PI*180.0))<_max_angle_rebound/M_PI*180){
                    _distance = distance;
                    //cout << "distance:" << _distance << ", angle:" << angle_detection/M_PI*180.0 << "°" << endl;
                }
            }
        }
    }
    return _distance;
}

double Capteur::getAngleDetection(Element &elt, Pos &pos_detection){
     Line *p_line = elt.p_line;

    //on récupère l'angle de détection
    
    //on place la ligne de detection
    Line line_detection;
    line_detection.x1 = _x;
    line_detection.y1 = _y;
    line_detection.x2 = pos_detection.x;
    line_detection.y2 = pos_detection.y;

    //on trouve la deuxième ligne
    Line line_wall;
    line_wall.x1 = line_detection.x2;
    line_wall.y1 = line_detection.y2;


    //deux lignes
    Line line_left;
    line_left.x1 = _x;
    line_left.y1 = _y;
    line_left.x2 = p_line->x1;
    line_left.y2 = p_line->y1;

    Line line_right;
    line_right.x1 = _x;
    line_right.y1 = _y; 
    line_right.x2 = p_line->x2;
    line_right.y2 = p_line->y2;

    //troisième ligne
    Line line_3;
    line_3.x1 = line_detection.x1;
    line_3.y1 = line_detection.y1;

    //trouver une condition pour trouver le bon point
    Pos pos = getWhichPoint(line_detection, line_left, line_right);
    line_wall.x2 = pos.x;
    line_wall.y2 = pos.y;

    //on switch
    line_3.x2 = line_wall.x2;
    line_3.y2 = line_wall.y2;

    double a = getLengthLine(line_3),
           b = getLengthLine(line_detection),
           c = getLengthLine(line_wall);
    
    
    return getAngleTriangle(a,b,c)-M_PI/2.0;
}

Pos Capteur::getWhichPoint(Line line_detection, Line line_left, Line line_right){
    double angle_detection = getRotation(line_detection);
    double angle_left = getRotation(line_left);
    double angle_right = getRotation(line_right);
    angle_left = angle_left - angle_detection;
    angle_right = angle_right - angle_detection;

    bool left = (_rotation-angle_detection) <= 0;
    
    //detection sur la droite ou sur la gauche
    //cout << ((left)?"left":"right") << endl;
    if(angle_left<0){
        //c'est que les deux points sont inversé
        Line tmp = line_left;
        line_left = line_right;
        line_right = tmp;
    }

    Pos pos;
    if(!left){
        pos.x = line_left.x2;
        pos.y = line_left.y2;
    }else{
        pos.x = line_right.x2;
        pos.y = line_right.y2;
    }

    return pos;
}

double Capteur::getRotation(Line line){
    double deltaX = line.x2-line.x1;
    double deltaY = line.y1-line.y2;
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

double Capteur::getLengthLine(Line line){
    double deltaX = line.x1-line.x2;
    double deltaY = line.y1-line.y2;

    return sqrt(deltaX*deltaX+deltaY*deltaY);
}

void Capteur::connectToWorld(World &world){
    _real_map = world.getRealWorld();
}