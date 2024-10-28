#include <RF24.h>
#include <RF24_config.h>
#include <nRF24L01.h>
//#include <printf.h>
#include <SPI.h>
#include <Servo.h>

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>


RF24 radio(9, 10);  //CE, CSN

const byte direccion1[6] = "00001";
Servo mi_servo;
int motorWidth = 0;

struct Paquete {
  int throttle;
  byte anguloX;
};

struct PaqueteAcc {
  double accX;
  double accY;
  double accZ;
  double percentage;
};

float realVoltage = 0;
int sensorValue = 0;
float voltage = 0;

Paquete datos;
PaqueteAcc telemetria;
Adafruit_MPU6050 mpu;

//VARIABLES BTS7960
int rpwm = 5;
int lpwm = 6;
int ren = 2;
int len = 3;
int vel = 0;


void setup() {

  Serial.begin(9600);

  //PRECONFIG RADIO
  radio.begin();
  radio.openReadingPipe(0, direccion1);

  radio.setPALevel(RF24_PA_MIN);
  radio.enableAckPayload();

  //PRECONFIG SERVO
  mi_servo.attach(4);
  pinMode(4, OUTPUT);

  Serial.println("CALIBRANDO");
  mi_servo.write(50);
  delay(500);
  mi_servo.write(60);
  delay(500);
  mi_servo.write(70);
  delay(500);
  Serial.println("CALIBRADO");
  delay(100);
  Serial.println("RECIBIENDO...");

  //PRECONFIG MOTOR DE TRACCIÓN
  datos.throttle = 0;

  //PRECONFIG PINS BTS7960
  pinMode(rpwm, OUTPUT);
  pinMode(lpwm, OUTPUT);
  pinMode(ren, OUTPUT);
  pinMode(len, OUTPUT);
  digitalWrite(ren, HIGH);
  digitalWrite(len, HIGH);

  //PRECONFIG ACELERÓMETRO
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  // set accelerometer range to +-8G
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);

  // set gyro range to +- 500 deg/s
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);

  // set filter bandwidth to 21 Hz
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  delay(100);

  Serial.println("X, Y, Z");
}

void loop() {


  radio.startListening();

  //BATERIA
  int sensorValue = analogRead(A0);

  float voltage = sensorValue * (5.0 / 1023.0);

  realVoltage = (voltage * 2 - 0.07);
  telemetria.percentage = ((realVoltage - 6) * 10) / 2.;
  telemetria.percentage = telemetria.percentage * 10;

  Serial.println(realVoltage);
  Serial.println(telemetria.percentage);


  //LECTURAS DEL ACELERÓMETRO
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  telemetria.accX = a.acceleration.x;
  telemetria.accY = a.acceleration.y;

  if (radio.available()) {


    radio.read(&datos, sizeof(Paquete));

    //CONTROL DEL MOTOR DE DIRECCIÓN
    mi_servo.write(datos.anguloX);

    //CONTROL DEL MOTOR DE TRACCIÓN
    if (datos.throttle <= 508) {

      vel = -170. / 512. * datos.throttle + 170.;
      analogWrite(rpwm, vel);
      analogWrite(lpwm, 0);


    } else {

      vel = 170. / 512. * datos.throttle - 170.;
      analogWrite(rpwm, 0);
      analogWrite(lpwm, vel);
    }





    Serial.println((int)datos.anguloX);
    Serial.println(vel);
    Serial.print("\t");
  }

  //ENVIAR TELEMETRIA AL EMISOR
  radio.writeAckPayload(0, &telemetria, sizeof(PaqueteAcc));  // pre-load data
}
