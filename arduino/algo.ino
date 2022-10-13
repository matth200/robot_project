#include <ArduinoSTL.h>
#define DIST_MAX 400
#define FPS 40.0

#include <Servo.h>

#define SERVO_G_PIN 9
#define SERVO_D_PIN 10
#define G_CORREC 8
#define D_CORREC -14

#define CAPTEUR_US 11
#define TIMEOUT 18500
#define VITESSE_SON 340.0


#define RAYON_BASE 0.7
#define FULL_SPEED 0.5
#define FACTEUR_DISTANCE 0.7

double rotation = 0;
double distance = 0;

unsigned long start_chrono = millis();
int speed_l = 0, speed_r = 0;

Servo servo_g, servo_d;

struct Line{
  double x1,y1;
  double x2,y2;
};

//function
double getRotation(double x1, double y1, double x2, double y2);
void goTo(double angle, double &rotation, int &speed_r, int &speed_l, bool sens=false);
Line setRotation(double rotation);
double getDistance(int pin, int timeout);
void updateRotation(double &rotation, double speed_l, double speed_r, unsigned long tm);
void setSpeed(Servo &servo, int vitesse);

//main

void setup() {
  //srand(analogRead(0));
  Serial.begin(9600);

  //on charge le réseau de neurones
  std::cout << "Lancement de l'algorithme simple" << std::endl;

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
  std::cout << "distance:" << distance << "cm" << std::endl;
  
  
  std::cout << "Motor1:" << speed_l << std::endl;
  std::cout << "Motor2:" << speed_r << std::endl;


  goTo(270.0/180.0*PI, rotation, speed_l, speed_r);
  
  setSpeed(servo_g, speed_l+G_CORREC);
  setSpeed(servo_d, -speed_r+D_CORREC);


  //on gère la fréquence de rafraichissement
  unsigned long duration = millis()-start_chrono;
  if(double(duration)<1000.0/FPS){
    delay(int(1000.0/FPS-double(duration)));
  }
}


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
    line.x1 =(0+RAYON_BASE*cos(PI+rotation));
    line.y1 = (0-RAYON_BASE*sin(PI+rotation));
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
        line.x1 = line.x2+2.0*RAYON_BASE*cos(PI+angle);
        line.y1 = line.y2-2.0*RAYON_BASE*sin(PI+angle);
    }
    
    //on enregistre la rotation
    rotation = getRotation(line.x1,line.y1,line.x2,line.y2);
}

void setSpeed(Servo &servo, int vitesse){
  servo.writeMicroseconds(map(vitesse, -100, 100, 1300, 1700));
}