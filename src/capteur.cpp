#include "capteur.h"
using namespace std;

Capteur::Capteur():_x(0),_y(0),_rotation(0),_distance(MAX_DIST)
{
    _real_map = NULL;

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
double Capteur::getDistance(){
    _distance = _max_dist;
    for(int i(0);i<_detectionLines.size()&&_real_map!=NULL;i++){
        int distance = 0;
        vector<Element> liste_elt = _real_map->getDetection(_detectionLines[i], distance);
        for(int j(0);j<liste_elt.size();j++){
            //detection des murs blancs
            if(liste_elt[j].state==_detection_state&&distance<_distance){
                _distance = distance;
            }
        }
    }
    return _distance;
}
void Capteur::connectToWorld(World &world){
    _real_map = world.getRealWorld();
}