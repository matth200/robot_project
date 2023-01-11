#include <ArduinoSTL.h>
#define DIST_MAX 315.0
#define FPS 40.0

#include <Servo.h>

//#define NO_INFORMATION
#define CALIBRATION

#define SERVO_G_PIN 9
#define SERVO_D_PIN 8
#define G_CORREC -14
#define D_CORREC 8

#define CAPTEUR_US 10
#define TIMEOUT 18500
#define VITESSE_SON 340.0


#define RAYON_BASE 8.0
#define FULL_SPEED 11.5
#define FACTEUR_DISTANCE 1

#define GAIN DIST_MAX/800.0
#define ANGLE_DETECTION 35.0
#define PERIODE_SNAKE 0.3

double rotation = 0;
double distance = 0;

double _ard_old_rotation = 0;
double _ard_done = -1;
bool _ard_sens_dance = false;
int32_t _ard_time_past = 0;
int number_rotation = 0;

unsigned long start_chrono = 0;
int speed_l = 0, speed_r = 0;

Servo servo_g, servo_d;

struct Line {
  double x1, y1;
  double x2, y2;
};

//function
double getRotation(double x1, double y1, double x2, double y2);
Line setRotation(double rotation);
double getDistance(int pin, int timeout);
void updateRotation(double &rotation, double speed_l, double speed_r, unsigned long tm);
void setSpeed(Servo &servo, int vitesse);
int freeRam();

double getSpeedAngle(double rotation, double old_rotation);
int ard_moveLikeSnake(int &speed_l, int &speed_r, uint32_t time_past);
void ard_calibration_rotation();
bool ard_goTo(double angle, double rotation, int &speed_r, int &speed_l,double &old_rotation, double &done, bool sens=false);

//main
void setup() {

  Serial.begin(9600);
  #ifndef NO_INFORMATION
  std::cout << "Algorithme sans IA" << std::endl;

  std::cout << "Memoire:" << freeRam() << std::endl;
  #endif
  
  //on gère les servommoteurs
  servo_g.attach(SERVO_G_PIN);
  servo_d.attach(SERVO_D_PIN);
  pinMode(CAPTEUR_US, OUTPUT);
  digitalWrite(CAPTEUR_US, LOW);
  delay(100);
}

void loop() {
  unsigned long tm = millis() - start_chrono;
  start_chrono = millis();
  updateRotation(rotation, speed_l, speed_r, tm);


  //on doit maintenir la loop a une certaine fréquence
  distance = getDistance(CAPTEUR_US, TIMEOUT) * FACTEUR_DISTANCE;
  if (distance > DIST_MAX) distance = DIST_MAX;

  #ifndef NO_INFORMATION
  std::cout << "fps:" << 1000.0 / (double)tm << std::endl;
  std::cout << "rotation:" << rotation / PI * 180.0 << std::endl;
  //Serial.println("distance:"+String(distance)+"cm");
  #endif

  _ard_time_past += tm;

  int angle_scan = 0;
  if(number_rotation==2){
    angle_scan = ard_moveLikeSnake(speed_l,speed_r, _ard_time_past);
  }
  
  
  #ifndef CALIBRATION
  if(distance<400*GAIN){
      if(angle_scan<-ANGLE_DETECTION){
          speed_l = 100;
          speed_r = 0;
      }
      else if(angle_scan>ANGLE_DETECTION){
          speed_l = 0;
          speed_r = 100;
      }
      if(distance<200*GAIN){
          speed_l = -100;
          speed_r = 100;
      }
  }else if(distance<800*GAIN){
      speed_l+=5;
  }

  if(speed_l>100){
      speed_l = 100;
  }
  #endif

  
  #ifdef CALIBRATION
  ard_calibration_rotation();
  #endif


  setSpeed(servo_g, int(speed_l) + G_CORREC);
  setSpeed(servo_d, int(-speed_r) + D_CORREC);


  //on gère la fréquence de rafraichissement
  unsigned long duration = millis() - start_chrono;
  if (double(duration) < 1000.0 / FPS) {
    delay(int(1000.0 / FPS - double(duration)));
  }
}



//fonctions
double getRotation(double x1, double y1, double x2, double y2) {
  double deltaX = x2 - x1;
  double deltaY = y1 - y2;
  double rotation;
  if (deltaX != 0) {
    rotation = atan((double)(deltaY) / deltaX);
  } else {
    //h n'est jamais nul ici
    double h = sqrt(deltaX * deltaX + deltaY * deltaY);
    rotation = asin((double)(deltaY) / h);
  }

  if (deltaX < 0) {
    rotation += PI;
  }

  if (rotation < 0) rotation += 2 * PI;

  return rotation;
}

Line setRotation(double rotation) {
  Line line;
  //on place le baton
  line.x1 = (0 + RAYON_BASE * cos(M_PI + rotation));
  line.y1 = (0 - RAYON_BASE * sin(M_PI + rotation));
  line.x2 = (0 + RAYON_BASE * cos(rotation));
  line.y2 = (0 - RAYON_BASE * sin(rotation));
  return line;
}

double getDistance(int pin, int timeout) {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, HIGH);
  delayMicroseconds(5);
  digitalWrite(pin, LOW);
  pinMode(pin, INPUT);
  double duration = (double)pulseIn(pin, HIGH, timeout);
  if (duration == 0) {
    duration = timeout;
  }

  double distance = VITESSE_SON * 100.0 * duration / 1000000.0;
  distance = distance / 2.0;
  distance = (distance > DIST_MAX) ? DIST_MAX : distance;
  return distance;
}

void updateRotation(double &rotation, double speed_l, double speed_r, unsigned long tm) {
  //intégration de la vitesse de la roue pour trouver l'angle de rotation

  //modif sur la vitesse pour qu'elle soit angulaire
  speed_r = speed_r / 100.0 * FULL_SPEED / RAYON_BASE;
  speed_l = speed_l / 100.0 * FULL_SPEED / RAYON_BASE;

  //on tourne autour de la roue gaguche
  double angle = rotation;
  Line line = setRotation(angle);

  if (speed_r != 0) {
    angle += speed_r * double(tm) / 1000.0;
    line.x2 = line.x1 + 2.0 * RAYON_BASE * cos(angle);
    line.y2 = line.y1 - 2.0 * RAYON_BASE * sin(angle);
  }

  if (speed_l != 0) {
    angle -= speed_l * double(tm) / 1000.0;
    line.x1 = line.x2 + 2.0 * RAYON_BASE * cos(M_PI + angle);
    line.y1 = line.y2 - 2.0 * RAYON_BASE * sin(M_PI + angle);
  }

  //on enregistre la rotation
  rotation = getRotation(line.x1, line.y1, line.x2, line.y2);
}

void setSpeed(Servo &servo, int vitesse) {
  servo.writeMicroseconds(map(vitesse, -100, 100, 1300, 1700));
}

int freeRam ()
{
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}


double getSpeedAngle(double rotation, double old_rotation){
    double speed_angle1 = (rotation-old_rotation)/M_PI*180.0;
    if(speed_angle1>360) speed_angle1-=360;
    if(speed_angle1<0) speed_angle1 += 360;


    double speed_angle2 = (old_rotation-rotation)/M_PI*180.0;
    if(speed_angle2>360.0) speed_angle2-=360.0;
    if(speed_angle2<0.0) speed_angle2 += 360.0;
    return (speed_angle1<=speed_angle2)?speed_angle1:speed_angle2;
}

bool ard_goTo(double angle, double rotation, int &speed_r, int &speed_l, double &old_rotation, double &done, bool sens){
    if(done==-1){
        old_rotation = rotation;
        done = 0;
    }
    angle = angle/M_PI*180.0;
    double speed_angle = getSpeedAngle(rotation, old_rotation);
    done += speed_angle;
    old_rotation = rotation;
    if(angle<=abs(done)){
        //on a atteint le bon angle
        std::cout << "OKAY---------------------------------->" << std::endl;
        speed_r = 0;
        speed_l = 0;
        return true;
    }
    std::cout << "done:" << done << ", angle:" << angle << std::endl;
    std::cout << speed_angle << std::endl;
    
    speed_r = sens?100.0:-100.0;
    speed_l = sens?-100.0:100;
    return false;
}

//return angle between 0 and 180
int ard_moveLikeSnake(int &speed_l, int &speed_r, uint32_t time_past){
    double angleg = (double(_ard_time_past)/1000.0+2.0/4.0)*2*M_PI*PERIODE_SNAKE;
    double angled = (double(_ard_time_past)/1000.0+1.0/4.0)*2*M_PI*PERIODE_SNAKE;
    speed_l = 100.0*(sin(angleg)+1.0)/2.0;
    speed_r = 100.0*(sin(angled)+1.0)/2.0;


    int angle = int(double(_ard_time_past)/1000.0*2.0*PERIODE_SNAKE*180.0)%360;

    //on map le scan
    if(0<=angle&&angle<90){
        return -angle;
    }
    else if(90<=angle&&angle<90+180){
        return (angle-90)-90;
    }

    //angle entre 90+180 et 90+180+90
    return 90-(angle-90-180);
}

void ard_calibration_rotation(){
  if(number_rotation!=2){
    if(ard_goTo(PI*2.0,rotation,speed_r,speed_l,_ard_old_rotation,_ard_done, _ard_sens_dance)){
        _ard_done = -1;
        _ard_sens_dance = !_ard_sens_dance;
        number_rotation+=1;
    }
  }
}
