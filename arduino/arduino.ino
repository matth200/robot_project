#include "src/m_learning.h"
#include <ArduinoSTL.h>
#define DIST_MAX 315
#define FPS 40.0


#include <Servo.h>

#define SERVO_G_PIN 9
#define SERVO_D_PIN 10
#define G_CORREC -14
#define D_CORREC 8

#define CAPTEUR_US 11
#define TIMEOUT 18500
#define VITESSE_SON 340.0


#define RAYON_BASE 8.0
#define FULL_SPEED 7.0
#define FACTEUR_DISTANCE 1.5

const PROGMEM char neuralnetwork_data[753]={3,0,0,0,4,0,0,0,20,0,0,0,4,0,0,0,-9,-12,-40,-64,-83,-8,-56,64,105,-79,53,-64,98,6,-95,64,-108,-55,-115,64,29,-96,-101,64,56,119,-103,64,-35,-127,31,-63,-33,71,-115,61,-82,-37,-103,64,126,66,28,-64,-10,81,7,-63,-96,120,-111,64,1,-39,14,65,107,78,-14,64,-12,-3,-119,-65,54,-124,-10,64,15,-85,-17,-64,118,-73,103,-64,106,20,74,-64,15,0,111,63,30,119,-78,64,-87,-54,-123,-65,-99,34,-120,64,-40,74,-6,64,57,-80,-34,63,-1,-122,104,-65,-103,43,-38,64,-74,84,110,64,27,-102,-7,64,44,58,-77,-65,-10,23,10,-63,123,124,78,-64,54,118,-82,-65,89,94,-17,64,-60,-42,-25,-64,-103,9,-83,-64,84,-9,88,64,-114,-43,-84,-65,-116,-44,16,-63,98,42,-43,-65,107,49,4,65,-108,63,-76,-64,18,-109,21,65,75,-54,-64,-64,57,-61,80,-64,-44,-30,-37,-64,-10,-8,57,63,115,-14,93,63,29,-39,-67,64,-37,-29,56,-65,-50,-42,-58,-64,-62,127,-121,64,-128,86,85,64,-23,49,4,-63,-42,-95,-42,-67,53,72,-109,-64,-114,-29,27,-64,32,39,-11,64,-58,-61,23,-63,41,-10,69,-64,105,-114,-103,-64,-43,43,67,-64,-107,-125,79,64,-4,-128,30,64,-40,-86,-91,-65,-107,57,-83,-65,101,-87,85,-64,-97,-70,70,-64,4,102,-21,-64,116,-72,106,-66,2,-4,25,-63,46,32,-102,64,77,-28,23,65,-125,96,10,65,-89,47,106,63,-116,-29,12,-63,-122,-15,-47,63,-23,-87,109,64,-105,12,-16,63,-109,-99,-18,63,4,-46,-79,-65,-61,107,-39,-64,-4,63,64,-65,-70,72,21,-63,-16,-11,12,-65,66,-60,-57,64,126,56,-57,-64,-128,48,107,-64,83,-84,-32,63,8,-119,13,64,86,-77,26,65,-126,47,38,64,-32,-118,27,-64,1,40,4,-63,-18,-42,72,-64,-43,-95,24,-63,-33,7,-62,64,78,13,12,-63,-4,70,-122,64,-97,-91,36,64,108,-39,-14,64,110,-66,-56,-65,-103,69,32,64,123,-28,15,65,-123,45,-77,-66,-65,123,-61,-64,14,121,-15,64,14,-66,-76,63,-105,102,-57,-64,113,-36,-37,-64,-40,-20,-90,64,122,-51,113,-64,-52,61,8,65,-108,26,-58,64,32,-87,-14,-64,99,21,55,64,53,-48,-123,-64,106,-38,30,65,-78,-125,-9,-65,120,-98,61,64,23,127,9,-64,-29,-43,54,64,26,-104,-12,-65,-126,106,33,-65,123,74,30,-63,-15,62,-119,64,-33,-57,-58,-64,-7,-27,-63,64,-115,-81,-84,64,97,116,-61,-65,-119,91,-74,63,-89,-112,-102,-65,87,-85,-110,-65,127,81,2,65,113,51,-66,-64,6,122,10,-63,86,-36,-110,-65,-42,-86,-107,64,-27,-10,10,63,-22,-69,30,-63,-1,99,-90,64,60,-21,-94,-64,-90,53,-21,-64,-79,80,-122,64,-76,35,-45,62,101,-50,-59,63,4,111,29,64,-12,88,-43,64,42,107,-7,64,-16,-19,-112,63,33,-89,-24,64,88,79,83,64,-46,-15,-68,64,-55,70,1,64,59,3,-9,-64,78,51,49,-65,-18,118,0,65,72,107,127,61,9,5,-38,-64,97,-53,18,-63,-35,0,-11,64,91,-103,-13,-64,83,-72,-4,-64,85,31,-21,-64,108,25,19,63,46,-85,12,-63,27,-90,-14,-64,-114,83,114,-64,37,-68,13,-63,-28,-127,41,-65,111,-110,41,63,12,124,-101,64,32,81,73,-64,18,-3,-1,64,99,-62,31,-63,72,-122,1,65,73,-7,-113,-65,101,-32,-22,-64,109,-89,-36,-66,113,-9,22,-63,-36,29,6,-63,-93,54,7,-63,-38,-55,-100,-64,125};
//const PROGMEM char neuralnetwork_data[753]={3,0,0,0,4,0,0,0,20,0,0,0,4,0,0,0,57,-76,-52,64,-31,122,-60,64,125,63,-91,-65,-123,-21,-35,-64,63,53,-22,-64,-25,-5,9,65,31,-123,107,63,72,-31,-42,64,-49,-9,21,-63,-123,-21,-59,64,-57,75,21,65,-100,-60,32,62,-111,-19,-124,64,94,-70,11,-63,-119,65,0,64,-31,122,-84,64,-86,-15,-94,63,-53,-95,29,-63,104,-111,-19,60,-86,-15,-14,63,-63,-54,-103,64,51,51,3,-63,66,96,-11,64,-90,-101,68,64,-100,-60,56,-64,-2,-44,-72,62,80,-115,11,-63,-31,122,-16,-64,-102,-103,1,-64,-66,-97,2,64,-53,-95,85,-64,-109,24,-76,-64,-86,-15,-82,-64,8,-84,-36,63,49,8,36,-64,113,61,50,-64,51,51,83,-64,82,-72,-106,64,113,61,-62,-64,111,18,43,-64,-72,30,101,63,-39,-50,25,65,6,-127,117,64,-127,-107,9,-63,109,-25,17,-63,-121,22,-123,-64,-33,79,-39,64,100,59,-65,-65,-121,22,-11,-64,-6,126,-38,-64,31,-123,25,-63,57,-76,-28,64,96,-27,-96,-64,-102,-103,-115,64,-119,65,96,62,-92,112,5,64,-63,-54,13,-63,-78,-99,-65,63,49,8,108,63,111,18,99,-65,-119,65,30,65,82,-72,-14,64,84,-29,37,-66,-127,-107,19,-63,23,-39,-118,-64,-59,32,104,64,117,-109,0,-64,-96,26,23,64,47,-35,-44,-65,25,4,-10,63,-82,71,-63,64,33,-80,-78,64,100,59,7,-64,-74,-13,-3,-67,104,-111,85,64,-10,40,-100,-64,127,106,4,64,-109,24,4,62,-59,32,104,64,-72,30,-115,64,-4,-87,-79,-66,-90,-101,-60,62,-57,75,-57,63,53,94,122,63,-117,108,13,-63,74,12,2,65,-41,-93,22,65,-104,110,-10,-64,57,-76,-128,-64,86,14,-83,62,106,-68,-76,63,-111,-19,-8,-64,51,51,-73,-64,-106,67,-13,64,-109,24,-32,-64,111,18,-125,-67,-92,112,-99,63,27,47,125,63,78,98,10,65,115,104,-67,64,63,53,20,65,74,12,-106,64,18,-125,-64,63,102,102,54,-64,-8,83,17,-63,-121,22,-91,-64,16,88,17,64,2,43,63,64,76,55,-87,63,0,0,-8,-64,49,8,-56,-64,78,98,16,-63,117,-109,40,64,29,90,-44,63,-2,-44,-40,-65,43,-121,22,62,-84,28,90,-66,35,-37,17,-64,68,-117,28,64,-31,122,-72,64,-86,-15,0,65,-41,-93,28,-63,-27,-48,-118,64,-37,-7,10,65,25,4,-6,64,-125,-64,-22,64,78,98,-80,-65,-12,-3,116,-65,102,102,-42,-65,-16,-89,46,-64,76,55,-95,-64,-123,-21,-63,-64,-12,-3,4,65,29,90,52,64,31,-123,-117,63,20,-82,19,65,78,98,-128,64,115,104,27,65,2,43,-73,-65,-33,79,-107,-64,-115,-105,0,-63,29,90,20,64,-27,-48,-22,-64,-86,-15,114,63,106,-68,26,-63,-39,-50,119,63,-55,118,14,-63,-78,-99,-17,61,61,10,11,65,-121,22,-39,-67,18,-125,-8,-64,-55,118,70,64,61,10,-41,61,4,86,-46,64,47,-35,-28,63,41,92,-73,-64,14,45,122,64,78,98,-40,64,49,8,84,64,66,96,9,65,-14,-46,25,-63,98,16,-108,64,2,43,-17,-64,2,43,11,65,-55,118,-58,-64,111,18,3,-67,-61,-11,-120,64,-123,-21,-7,64,-27,-48,-94,-66,-2,-44,-28,-64,90,100,-37,-64,39,49,0,-64,92,-113,-62,63,121,-23,-6,-64,104,-111,-87,64,8,-84,-20,63,66,96,5,-65,-39,-50,47,64,-121,22,5,65,53,94,-22,-65,-68,116,-21,-64,-23,38,23,-63,-98,-17,-13,64,-16,-89,102,-65,125};

MachineLearning machine;
unsigned char memoire[] = {0,0};
unsigned char data[] = {0,0,0,0};
double rotation = 0;
double distance = 0;

unsigned long start_chrono = 0;
int speed_l = 0, speed_r = 0;

Servo servo_g, servo_d;

struct Line{
  double x1,y1;
  double x2,y2;
};

//function
double getRotation(double x1, double y1, double x2, double y2){
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
        rotation+=PI;
    }

    if(rotation<0) rotation+=2*PI;
    
    return rotation;
}

void goTo(double angle, double &rotation, int &speed_r, int &speed_l, bool sens=false){
  //si la différence est de plus de 5 degres, on fait tout bouger
  if(int(abs(angle-rotation)/PI*180.0)%360>5){
    speed_r = (sens)?100.0:-100.0;
    speed_l = (sens)?-100.0:100.0;
  }else{
    speed_r = 0;
    speed_l = 0;
  }
}

Line setRotation(double rotation){
    Line line;
    //on place le baton
    line.x1 =(0+RAYON_BASE*cos(M_PI+rotation));
    line.y1 = (0-RAYON_BASE*sin(M_PI+rotation));
    line.x2 = (0+RAYON_BASE*cos(rotation));
    line.y2 = (0-RAYON_BASE*sin(rotation));
    return line;
}

double getDistance(int pin, int timeout){
  pinMode(pin, OUTPUT);
  digitalWrite(pin, HIGH);
  delayMicroseconds(5);
  digitalWrite(pin, LOW);
  pinMode(pin, INPUT);
  double duration = (double)pulseIn(pin, HIGH, timeout);
  if(duration==0){
    duration = timeout;
  }

  double distance = VITESSE_SON*100.0*duration/1000000.0;
  distance = distance/2.0;
  distance = (distance>DIST_MAX)?DIST_MAX:distance;
  return distance;
}

void updateRotation(double &rotation, double speed_l, double speed_r, unsigned long tm){
    //intégration de la vitesse de la roue pour trouver l'angle de rotation

    //modif sur la vitesse pour qu'elle soit angulaire
    speed_r = speed_r/100.0*FULL_SPEED/RAYON_BASE;
    speed_l = speed_l/100.0*FULL_SPEED/RAYON_BASE;

    //on tourne autour de la roue gaguche
    double angle = rotation;
    Line line = setRotation(angle);

    if(speed_r!=0){
        angle+=speed_r*double(tm)/1000.0;
        line.x2 = line.x1+2.0*RAYON_BASE*cos(angle);
        line.y2 = line.y1-2.0*RAYON_BASE*sin(angle);
    }

    if(speed_l!=0){
        angle-=speed_l*double(tm)/1000.0;
        line.x1 = line.x2+2.0*RAYON_BASE*cos(M_PI+angle);
        line.y1 = line.y2-2.0*RAYON_BASE*sin(M_PI+angle);
    }
    
    //on enregistre la rotation
    rotation = getRotation(line.x1,line.y1,line.x2,line.y2);
}

void setSpeed(Servo &servo, int vitesse){
  servo.writeMicroseconds(map(vitesse, -100, 100, 1300, 1700));
}
//main

void setup() {
  srand(analogRead(0));
  
  Serial.begin(9600);

  std::cout << "Construction du model" << std::endl;
  //neural network
  machine.open(4);
  machine.addColumn(20);
  //machine.addColumn(20);
  machine.addColumn(4);

  //on charge le réseau de neurones
  machine.backupTraining(neuralnetwork_data);
  std::cout << "Reseau de neurones chargé." << std::endl;

  //on gère les servommoteurs
  servo_g.attach(SERVO_G_PIN);
  servo_d.attach(SERVO_D_PIN);
  pinMode(CAPTEUR_US, OUTPUT);
  digitalWrite(CAPTEUR_US, LOW);
  delay(100);
}

void loop() {
  unsigned long tm = millis()-start_chrono;
  start_chrono = millis();
  updateRotation(rotation,speed_l,speed_r, tm);


  std::cout << "fps:" << 1000.0/(double)tm << std::endl;
  std::cout << "rotation:" << rotation/PI*180.0 << std::endl;
  //on doit maintenir la loop a une certaine fréquence
  distance = getDistance(CAPTEUR_US, TIMEOUT) * FACTEUR_DISTANCE;
  if(distance>DIST_MAX) distance = DIST_MAX;
  //Serial.println("distance:"+String(distance)+"cm");
  
  //info réel
  data[0] = (unsigned char)(int(double(int(rotation)%360)/360.0*255.0));
  data[1] = (unsigned char)(int(distance/DIST_MAX*255.0));

  //info de memoire
  memcpy((char*)(data+2), memoire, 2);
  
  //on donne les infos au réseau de neurones
  machine.setInput((char*)data);
  machine.calcul();
  
  //on remplit la mémoire pour prochain passage
  memoire[0] = (unsigned char)machine.getOutput(2);
  memoire[1] = (unsigned char)machine.getOutput(3);
  
  //std::cout << "Motor1:" << machine.getOutput(0) << std::endl;
  //std::cout << "Motor2:" << machine.getOutput(1) << std::endl;

  //on envoie les infos moteur
  speed_l = int((machine.getOutput(0)-0.5)*2.0*100.0);
  speed_r = int((machine.getOutput(1)-0.5)*2.0*100.0);

  //goTo(90.0/180.0*PI, rotation, speed_l, speed_r);
  
  setSpeed(servo_g, int(speed_l*0.6)+G_CORREC);
  setSpeed(servo_d, int(-speed_r)+D_CORREC);


  //on gère la fréquence de rafraichissement
  unsigned long duration = millis()-start_chrono;
  if(double(duration)<1000.0/FPS){
    delay(int(1000.0/FPS-double(duration)));
  }
}
