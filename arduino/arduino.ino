#include "src/m_learning.h"
#include <ArduinoSTL.h>
#define DIST_MAX 315
#define FPS 40.0


#include <Servo.h>

#define SERVO_G_PIN 9
#define SERVO_D_PIN 10
#define G_CORREC 8
#define D_CORREC -14

#define CAPTEUR_US 11
#define TIMEOUT 18500
#define VITESSE_SON 340.0


#define RAYON_BASE 0.10
#define FULL_SPEED 1.0

//const PROGMEM char neuralnetwork_data[393]={3,0,0,0,4,0,0,0,10,0,0,0,4,0,0,0,78,113,30,-63,-78,-113,5,-63,-116,114,31,65,26,103,-95,-64,-84,-41,-72,64,-46,80,15,-63,61,-115,-101,-65,63,-108,18,65,74,65,29,64,94,-121,23,-63,-54,118,-66,-64,-51,6,-23,-65,106,-25,126,64,42,40,-34,64,-6,-3,-8,-64,50,-7,16,-63,-100,21,-103,64,-7,-53,6,-63,82,118,-29,61,125,-28,-54,64,-118,-7,73,64,-73,20,23,65,-52,-85,-90,64,-47,78,10,65,-83,-32,2,65,57,-11,-51,-64,26,98,64,62,-8,37,-37,64,122,0,56,64,66,94,-17,-64,-93,54,-90,63,30,13,1,65,-68,55,-90,-64,-106,-113,-116,64,-106,-59,-76,-64,34,-11,101,63,-15,103,22,-63,7,56,-43,-65,41,72,31,-63,41,8,100,64,49,-40,-91,64,-28,89,-59,64,5,-91,9,65,-110,-44,-69,-64,107,-68,85,-64,-36,-48,-66,-65,88,-70,-68,63,85,42,120,64,-26,54,-1,-64,48,-17,-23,-64,87,69,-46,-64,86,31,69,64,60,-103,-85,-64,-8,-85,-32,64,-7,25,47,64,-84,-110,2,65,107,37,35,64,-79,1,-45,-64,57,-77,4,65,-4,-62,11,-69,88,-41,10,-63,-106,-90,11,-64,-10,104,-2,-64,-128,50,-16,64,-58,88,-100,64,-25,-45,-53,-64,-54,-78,78,64,124,101,-16,64,-83,67,18,65,0,-47,14,-64,12,59,-122,64,86,124,88,64,9,-69,19,-63,-31,62,-82,-65,-46,25,30,-64,3,-111,-104,-64,-40,-24,13,65,37,-102,-6,-64,92,-86,-112,-65,56,115,98,-64,-58,90,-78,62,-98,-107,-126,64,123,-105,26,-63,102,34,-13,-64,22,4,20,64,-25,-91,-83,-66,-117,17,35,-64,119,40,-109,64,-79,1,-75,64,-15,79,-2,-64,123,86,21,-63,75,103,20,-63,-95,57,-24,-64,-38,-62,-119,64,125};
const PROGMEM char neuralnetwork_data[753]={3,0,0,0,4,0,0,0,20,0,0,0,4,0,0,0,43,-121,-58,63,73,75,-119,63,-78,-89,-17,61,113,61,-22,-65,-12,-3,28,-64,-31,82,-44,62,-72,30,29,64,-90,-101,12,-64,-14,-46,-35,-65,-31,122,-108,-66,-9,-91,-37,62,-8,83,43,-64,-125,-64,-110,-64,-10,-88,54,64,43,-121,54,63,106,-68,-92,-65,-68,116,51,-64,98,16,104,-64,92,-113,-98,64,-104,110,106,64,29,90,60,-64,4,86,46,-64,45,-78,-19,63,-16,-89,70,-65,-59,32,-48,-65,98,16,-104,62,113,61,90,64,-102,-103,57,63,116,-27,112,-64,14,45,98,62,80,-115,23,-65,-20,81,-72,-65,-94,69,54,64,-27,-48,-122,-64,-109,24,-112,64,-53,-95,13,-64,2,86,-102,-64,-125,-64,-6,63,31,-118,-21,63,61,-25,-58,-65,27,47,85,-64,119,-66,108,64,-20,81,56,-64,10,-41,91,64,-39,-50,-73,63,-41,-93,80,-65,98,16,96,64,111,18,-125,-68,-96,26,15,63,27,47,93,64,-49,-9,-97,64,-111,24,-124,-65,82,-72,-110,64,-69,50,-45,63,84,-29,101,-65,-100,-60,-96,-66,63,75,27,-64,-2,-44,-104,64,111,18,123,-64,74,12,-14,-65,-23,38,-115,-64,-112,-62,-11,-68,4,86,-122,64,-37,-7,-110,64,-70,94,83,-64,90,100,0,-64,-49,-9,-13,63,-18,124,34,64,25,4,-106,64,88,57,-112,-64,-66,79,-118,-65,0,0,96,64,47,-35,76,-64,127,106,-104,64,106,-62,-126,59,113,61,-86,63,-98,-77,-57,-65,-23,-2,43,-65,-29,-91,27,-66,-37,-7,-82,63,49,8,-100,-64,37,6,73,-64,47,-35,20,64,33,-80,-126,64,117,-109,96,64,-90,-101,100,-64,-114,24,-100,-64,-66,95,15,64,71,-111,114,-64,92,-113,114,64,28,-84,28,-66,0,0,-48,-65,-39,14,9,-64,-59,32,-80,62,117,-114,120,-64,92,-113,90,-64,-18,-22,64,64,78,98,-28,-66,55,-119,1,-64,-45,-99,122,64,103,-72,-60,63,-72,-66,37,-65,-29,50,-125,-64,-98,-17,15,64,-98,-17,82,64,43,71,-11,63,35,-37,121,63,-32,-85,92,-64,81,32,35,64,-4,-87,-15,61,10,-1,35,62,43,-121,-110,64,88,57,-116,-64,-106,47,-113,-64,-64,-54,57,-64,116,-77,-122,63,-37,85,55,63,-10,0,-116,63,-101,-102,-105,-64,-90,40,36,64,93,-113,2,63,-72,30,-127,-65,-125,-64,18,64,64,48,-50,-65,68,-53,85,-64,107,-63,116,-66,-126,45,-117,64,-25,67,-110,63,-82,-44,0,64,-6,126,-6,-65,-33,79,-115,-66,72,-31,58,64,104,-111,-67,-65,31,-123,-121,-64,-109,24,-100,64,-127,-107,75,64,35,-37,-107,-64,121,-23,-126,64,109,-25,-37,-65,-123,-21,-119,64,-80,114,8,64,-29,-91,-21,-65,-47,34,-85,63,98,16,-8,-65,72,-31,-114,-64,-119,65,-96,63,-82,71,-111,63,-6,126,-6,-65,4,86,86,-64,104,-111,-127,64,43,-121,-98,64,39,49,-128,-64,-92,112,-123,64,39,49,88,64,-82,71,-107,-65,-88,-58,115,-64,47,-35,68,-65,35,-37,81,64,8,-84,-100,64,78,98,112,-64,-8,83,107,-64,47,-35,-128,-64,-109,24,4,63,-127,-107,115,-64,-100,-60,80,-64,94,-70,-55,62,49,8,-116,64,-84,28,90,64,76,55,-123,-64,-20,81,-128,64,123,20,110,-65,-14,-46,45,-65,-27,-48,0,64,-37,-7,-110,-64,117,-109,24,64,86,14,45,64,105,119,-2,-65,104,-91,-67,-65,102,102,6,64,68,-117,-84,-66,72,-31,122,64,-68,116,3,64,-117,108,63,-64,23,-39,14,-65,125};

MachineLearning machine;
unsigned char memoire[] = {0,0};
unsigned char data[] = {123,12,12,24};
double rotation = 20.0;
double distance = 50.0;

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
        rotation+=M_PI;
    }
    return rotation;
}

void goTo(double angle, double &rotation, int &speed_r, int &speed_l, bool sens=false){
  //si la différence est de plus de 5 degres, on fait tout bouger
  if(abs(angle-rotation)/PI*180.0>5.0){
    speed_r = (sens)?FULL_SPEED:-FULL_SPEED;
    speed_l = (sens)?-FULL_SPEED:FULL_SPEED;
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
  return distance/2.0;
}

void updateRotation(double &rotation, double speed_l, double speed_r, unsigned long tm){
    //intégration de la vitesse de la roue pour trouver l'angle de rotation

    //modif sur la vitesse
    speed_r = speed_r/100.0*FULL_SPEED/RAYON_BASE;
    speed_l = speed_l/100.0*FULL_SPEED/RAYON_BASE;

    //on tourne autour de la roue gaguche
    double angle = rotation;
    Line line = setRotation(angle);

    if(speed_r!=0){
        angle+=speed_r*double(tm);
        line.x2 = line.x1+2.0*RAYON_BASE*cos(angle);
        line.y2 = line.y1-2.0*RAYON_BASE*sin(angle);
    }

    if(speed_l!=0){
        angle-=speed_l*double(tm);
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
  updateRotation(rotation,speed_l,speed_r, tm);


  std::cout << "fps:" << 1000.0/(double)tm << std::endl;
  start_chrono = millis();
  //on doit maintenir la loop a une certaine fréquence
  distance = getDistance(CAPTEUR_US, TIMEOUT);
  Serial.println("distance:"+String(distance)+"cm");
  
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
  speed_r = -int((machine.getOutput(1)-0.5)*2.0*100.0);
  setSpeed(servo_g, speed_l+G_CORREC);
  setSpeed(servo_d, speed_r+D_CORREC);


  //on gère la fréquence de rafraichissement
  unsigned long duration = millis()-start_chrono;
  if(double(duration)<1000.0/FPS){
    delay(int(1000.0/FPS-double(duration)));
  }
}
