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

const PROGMEM char neuralnetwork_data[393]={3,0,0,0,4,0,0,0,10,0,0,0,4,0,0,0,78,113,30,-63,-78,-113,5,-63,-116,114,31,65,26,103,-95,-64,-84,-41,-72,64,-46,80,15,-63,61,-115,-101,-65,63,-108,18,65,74,65,29,64,94,-121,23,-63,-54,118,-66,-64,-51,6,-23,-65,106,-25,126,64,42,40,-34,64,-6,-3,-8,-64,50,-7,16,-63,-100,21,-103,64,-7,-53,6,-63,82,118,-29,61,125,-28,-54,64,-118,-7,73,64,-73,20,23,65,-52,-85,-90,64,-47,78,10,65,-83,-32,2,65,57,-11,-51,-64,26,98,64,62,-8,37,-37,64,122,0,56,64,66,94,-17,-64,-93,54,-90,63,30,13,1,65,-68,55,-90,-64,-106,-113,-116,64,-106,-59,-76,-64,34,-11,101,63,-15,103,22,-63,7,56,-43,-65,41,72,31,-63,41,8,100,64,49,-40,-91,64,-28,89,-59,64,5,-91,9,65,-110,-44,-69,-64,107,-68,85,-64,-36,-48,-66,-65,88,-70,-68,63,85,42,120,64,-26,54,-1,-64,48,-17,-23,-64,87,69,-46,-64,86,31,69,64,60,-103,-85,-64,-8,-85,-32,64,-7,25,47,64,-84,-110,2,65,107,37,35,64,-79,1,-45,-64,57,-77,4,65,-4,-62,11,-69,88,-41,10,-63,-106,-90,11,-64,-10,104,-2,-64,-128,50,-16,64,-58,88,-100,64,-25,-45,-53,-64,-54,-78,78,64,124,101,-16,64,-83,67,18,65,0,-47,14,-64,12,59,-122,64,86,124,88,64,9,-69,19,-63,-31,62,-82,-65,-46,25,30,-64,3,-111,-104,-64,-40,-24,13,65,37,-102,-6,-64,92,-86,-112,-65,56,115,98,-64,-58,90,-78,62,-98,-107,-126,64,123,-105,26,-63,102,34,-13,-64,22,4,20,64,-25,-91,-83,-66,-117,17,35,-64,119,40,-109,64,-79,1,-75,64,-15,79,-2,-64,123,86,21,-63,75,103,20,-63,-95,57,-24,-64,-38,-62,-119,64,125};

MachineLearning machine;
unsigned char memoire[] = {0,0};
unsigned char data[] = {123,12,12,24};
double rotation = 20.0;
double distance = 50.0;

unsigned long start_chrono = 0;

Servo servo_g, servo_d;

//function
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

void updateRotation(double &rotation){

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
  machine.addColumn(10);
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
  std::cout << "fps:" << 1000.0/(millis()-start_chrono) << std::endl;
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
  setSpeed(servo_g, int((machine.getOutput(0)-0.5)*2.0*100.0)+G_CORREC);
  setSpeed(servo_d, int(-(machine.getOutput(1)-0.5)*2.0*100.0)+D_CORREC);


  //on gère la fréquence de rafraichissement
  unsigned long duration = millis()-start_chrono;
  if(double(duration)<1000.0/FPS){
    delay(int(1000.0/FPS-double(duration)));
  }
}
