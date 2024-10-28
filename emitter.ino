#include <RF24.h>
#include <RF24_config.h>
#include <nRF24L01.h>
#include <SPI.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3D ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

RF24 radio(9,10); //CE, CSN
const byte direccion1[6]="00001";
int posX,posY=0; 
int VRx=A0, VRy=A1;

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

Paquete datos; 
PaqueteAcc telemetria;

char text[32] = "";
int i=0;

void setup() {

    Serial.begin(9600);

    //PRECONFIG RADIO
    radio.begin();
    radio.openWritingPipe(direccion1);
    radio.setPALevel(RF24_PA_MIN);
    radio.enableAckPayload();

    //PRECONFIG JOYSTICK
    pinMode(VRx, INPUT);
    pinMode(VRy, INPUT);

      // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  display.clearDisplay();
  display.setTextSize(1);             // Normal 1:1 escala pixel
  display.setTextColor(SSD1306_WHITE);        // Texto blanco
  display.setCursor(0,0);             // Arriba izquierda
  display.println("SALUDOS INTERNAUTAS");
  display.display();
  display.clearDisplay();
  delay(500);

  
  display.setTextSize(1);        
  display.setTextColor(SSD1306_WHITE);        
  display.setCursor(0,0);             
  display.println("ESTAMOS CALIBRADOS");
  display.display();

  delay(500);


 display.clearDisplay();


}


void loop() {

    bool rslt;
    i++;

    display.clearDisplay(); 
    radio.stopListening();

    posX=analogRead(VRx);
    posY=analogRead(VRy);
  

    datos.throttle= posY;
    datos.anguloX = map(posX, 0, 1023, 120, 0);
    
    Serial.println( (int)datos.anguloX);
    Serial.println( (int)datos.throttle);
    Serial.print("\t");

    rslt= radio.write(&datos, sizeof(Paquete));

    if (rslt) {
        if ( radio.isAckPayloadAvailable() ) {
          
            radio.read(&telemetria, sizeof(PaqueteAcc));
           
        }
        else {
            Serial.println("Acknowledge sin datos ");
        }
        
    }
    else {
        Serial.println("Tx failed");
    }
    

    //MOSTRAR ACELERACION EJE X
    display.setTextSize(1);             
    display.setTextColor(SSD1306_WHITE);       
    display.setCursor(0,5);             
    display.println("accX:");
    display.setTextSize(1);  
    display.setCursor(55,5);  
    if(i=500){
    display.println(telemetria.accX/9.81);
    
    }
    display.setCursor(85,5); 
    display.println("G");

    //MOSTRAR ACELERACION EJE Y
    display.setCursor(0,15); 
    display.println("accY:");
    display.setTextSize(1);  
    display.setCursor(55,15);  
    if(i=500){
    display.println(telemetria.accY/9.81);
    i=0; 
    }
    display.setCursor(85,15); 
    display.println("G");

    //MOSTRAR VOLTAJE BATERÍA
    display.setCursor(0,25); 
    display.println("VOL:");
    display.setTextSize(1);  
    display.setCursor(50,25);  
    display.println(telemetria.percentage);
    display.setCursor(87,25); 
    display.println("%");


    display.display();


}
