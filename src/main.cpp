#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
// I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include "I2Cdev.h"       // Librerias I2C para controlar el mpu6050
#include "MPU6050.h"
#include <Adafruit_GFX.h>               //libreria necesaria para inicializar oled, graficos
#include <Adafruit_SSD1306.h>           //libreria necesaria para inicializar oled
#include <Fonts/FreeMonoBold9pt7b.h>    //Fuente de bienvenida para el oled
#include <Fonts/FreeSans9pt7b.h>    //Fuente de Mensajes
#include <NTPClient.h>    // Actualizar la hora por medio del servidor NTPClient - Esta librería ha sido modificada https://github.com/arduino-libraries/NTPClient/issues/36
#include <WiFiUdp.h>      // Udp.cpp: Biblioteca para enviar / recibir paquetes UDP


////////////////////////// Inicializar NTPCliente //////////////////////////

// WiFi Network Credentials
const char *ssid =  "RED_CANGO";   // name of your WiFi network
const char *password =  "!d:bhYn36t//B9^s"; // password of the WiFi network


// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

// Variables to save date and time
String formattedDate;
String formattedTime;
String day, dayText;
String dayStamp;


////////////////////////// Inicializar MPU6050 //////////////////////////

// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for InvenSense evaluation board)
// AD0 high = 0x69

// Instancia objeto de la clase MPU6050
MPU6050 sensor;

// Valores RAW (sin procesar) del giroscopio en los ejes x,y,z
const int timeData = 3; // Tiempo en recolectar datos en segundos
int16_t accelX = 0, accelY = 0, accelZ = 0;
int16_t gyroX = 0, gyroY =0, gyroZ = 0;


///////////////////////////////////// Initialize SSD1306 /////////////////////////////////////
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);


//////////////////////////////////// CARACTERISTICAS CNN ////////////////////////////////////
// Caracteristicas Acelerómetro
double mavXAxis = 0.0,mavYAxis = 0.0,mavZAxis = 0.0;
double rmsXAxis = 0.0,rmsYAxis = 0.0,rmsZAxis = 0.0;
double wlXAxis = 0.0, wlYAxis = 0.0, wlZAxis = 0.0;
double wlaXAxis = 0.0, wlaYAxis = 0.0, wlaZAxis = 0.0;

// Caracteristicas Giroscopio
double GmavXAxis = 0.0, GmavYAxis = 0.0, GmavZAxis = 0.0;
double GrmsXAxis = 0.0, GrmsYAxis = 0.0, GrmsZAxis = 0.0;
double GwlXAxis = 0.0, GwlYAxis = 0.0, GwlZAxis = 0.0;
double GwlaXAxis = 0.0, GwlaYAxis = 0.0, GwlaZAxis = 0.0;

///////////////////////////////////// Variables globales /////////////////////////////////////

const int pinStart = 4;     //Button D4
boolean flag = false;       //Bandera, utilizada para determinar si se presionó el boton
unsigned long debounce = 0; // Tiempo del rebote.

unsigned long lastTime = 0, sampleTime = 100;

//////////////////////////////////// Obtener la Hora NTP ////////////////////////////////////
void ttime() {
  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }

  formattedDate = timeClient.getFormattedDate();
  formattedTime = timeClient.getFormattedTime();
  day = timeClient.getDay();

  if(day == "1"){
    dayText = "LUNES";
  }
  if(day == "2"){
    dayText = "MARTES";
  }
  if(day == "3"){
    dayText = "MIERCOLES";
  }
  if(day == "4"){
    dayText = "JUEVES";
  }
  if(day == "5"){
    dayText = "VIERNES";
  }
  if(day == "6"){
    dayText = "SABADO";
  }
  if(day == "0"){
    dayText = "DOMINGO";
  }


  display.clearDisplay();
  display.setFont();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  //display.setCursor(18,16);
  display.setCursor(0,0);
  display.println(String(dayText));
  //display.setCursor(20,27);
  display.setCursor(18,22);
  display.println(String(formattedTime));
  display.drawRect(0, 41, 128, 0, WHITE);
  //display.setCursor(8,51);
  display.setCursor(5,46);
  display.println(String(formattedDate));
  display.display();
}

////////////////////////////////// Connect to WiFi network //////////////////////////////////
void setup_wifi() {
  display.setFont(&FreeMonoBold9pt7b);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(20,30);
  display.println("WiFi...");
  display.display();
  delay(500);

  WiFi.begin(ssid, password);
    delay(1000);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
  display.clearDisplay();
  display.setCursor(20,30);
  display.println("WiFi: OK");
  display.display();
}


void featuresExtraction() {
  
  int sizeSample = timeData*10;
  // Caracteristicas Acelerómetro
  mavXAxis = 0.0;
  mavYAxis = 0.0;
  mavZAxis = 0.0;

  rmsXAxis = 0.0;
  rmsYAxis = 0.0;
  rmsZAxis = 0.0;

  wlXAxis = 0.0;
  wlYAxis = 0.0;
  wlZAxis = 0.0;

  // Caracteristicas Giroscopio
  GmavXAxis = 0.0;
  GmavYAxis = 0.0;
  GmavZAxis = 0.0;

  GrmsXAxis = 0.0;
  GrmsYAxis = 0.0;
  GrmsZAxis = 0.0;

  GwlXAxis = 0.0;
  GwlYAxis = 0.0;
  GwlZAxis = 0.0;

  for (int k = 0; k<sizeSample;k++)
  { 
    sensor.getAcceleration(&accelX, &accelY, &accelZ);
    sensor.getRotation(&gyroX, &gyroY, &gyroZ);
   
    double ax_m_s2 = accelX * (9.81/16384.0);
    double ay_m_s2 = accelY * (9.81/16384.0);
    double az_m_s2 = accelZ * (9.81/16384.0);

    float gx_deg_s = gyroX * (250.0/32768.0);
    float gy_deg_s = gyroY * (250.0/32768.0);
    float gz_deg_s = gyroZ * (250.0/32768.0);

    ////////////////// Caracteristicas Acelerómetro //////////////////

    //Valor absoluto medio (MAV)
    mavXAxis = mavXAxis + abs(ax_m_s2);
    mavYAxis = mavYAxis + abs(ay_m_s2);
    mavZAxis = mavZAxis + abs(az_m_s2);

    //Valor eficaz (RMS)
    rmsXAxis = rmsXAxis + ax_m_s2*ax_m_s2;
    rmsYAxis = rmsYAxis + ay_m_s2*ay_m_s2;
    rmsZAxis = rmsZAxis + az_m_s2*az_m_s2;

    //Longitud de forma de onda (WL)
    wlXAxis = wlXAxis + abs(ax_m_s2 - wlaXAxis);
    wlYAxis = wlYAxis + abs(ay_m_s2 - wlaYAxis);
    wlZAxis = wlZAxis + abs(az_m_s2 - wlaZAxis);

    wlaXAxis = ax_m_s2;
    wlaYAxis = ay_m_s2;
    wlaZAxis = az_m_s2;

    ////////////////// Caracteristicas Giroscopio //////////////////
    //Valor absoluto medio (MAV)
    GmavXAxis = GmavXAxis + abs(gx_deg_s);
    GmavYAxis = GmavYAxis + abs(gy_deg_s);
    GmavZAxis = GmavZAxis + abs(gz_deg_s);

    ///Valor eficaz (RMS)
    GrmsXAxis = GrmsXAxis + gx_deg_s*gx_deg_s;
    GrmsYAxis = GrmsYAxis + gy_deg_s*gy_deg_s;
    GrmsZAxis = GrmsZAxis + gz_deg_s*gz_deg_s;

    //Longitud de forma de onda (WL)
    GwlXAxis = GwlXAxis + abs(gx_deg_s - GwlaXAxis);
    GwlYAxis = GwlYAxis + abs(gy_deg_s - GwlaYAxis);
    GwlZAxis = GwlZAxis + abs(gz_deg_s - GwlaZAxis);

    GwlaXAxis = gx_deg_s;
    GwlaYAxis = gy_deg_s;
    GwlaZAxis = gz_deg_s;
    
    delay(100);
  }
  
  ////////////////// Caracteristicas Acelerómetro //////////////////
  mavXAxis = mavXAxis/(double)sizeSample;
  mavYAxis = mavYAxis/(double)sizeSample;
  mavZAxis = mavZAxis/(double)sizeSample;
  
  rmsXAxis = sqrt(rmsXAxis/(double)sizeSample);
  rmsYAxis = sqrt(rmsYAxis/(double)sizeSample);
  rmsZAxis = sqrt(rmsZAxis/(double)sizeSample);

  ////////////////// Caracteristicas Giroscopio //////////////////
  GmavXAxis = GmavXAxis/(double)sizeSample;
  GmavYAxis = GmavYAxis/(double)sizeSample;
  GmavZAxis = GmavZAxis/(double)sizeSample;
  
  GrmsXAxis = sqrt(GrmsXAxis/(double)sizeSample);
  GrmsYAxis = sqrt(GrmsYAxis/(double)sizeSample);
  GrmsZAxis = sqrt(GrmsZAxis/(double)sizeSample);
}


void pulse() {
  if(!digitalRead(pinStart) && (millis()-debounce > 500))
  {
    debounce = millis();
    flag = true;
   } 
} 


void setup() {

    Serial.begin(115200);

    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
      Serial.println("SSD1306 allocation failed");
      for(;;);
    }
    delay(2000);

    setup_wifi(); //Connect to network

    // Initialize a NTPClient to get time
    timeClient.begin();
    timeClient.setTimeOffset(-18000);   //GMT-5

    pinMode(pinStart,INPUT_PULLUP);
    
    // join I2C bus (I2Cdev library doesn't do this automatically)
    Wire.begin();           //Iniciando I2C  
    sensor.initialize();    //Iniciando el sensor

    //Serial.println("Iniciando MPU6050");
    if (sensor.testConnection()) delay(500); //Serial.println("Sensor iniciado correctamente"); 
    else delay(500); //Serial.println("Error al iniciar el sensor");

}

void loop() {

    display.clearDisplay();
    ttime();    //Inicio del reloj
    display.clearDisplay();
    
    //Condición si se presionó el botón, este retorna un true, en la bandera
    if(!digitalRead(pinStart)) {
        pulse();
    }
    
    //Si la bandera es true, sucede el ingreso de datos
    if(flag) {
        //Mensaje de inicio de ingreso de datos
        display.setFont();
        display.clearDisplay();
        display.setTextSize(2);
        display.setTextColor(WHITE);
        display.setCursor(62,25);
        display.println("2");
        display.display();
        delay(1000);
        display.clearDisplay();
        display.setCursor(62,25);
        display.println("1");
        display.display();
        delay(1000);
        display.setFont(&FreeSans9pt7b);
        display.clearDisplay();
        display.setTextSize(1);
        display.setCursor(0,32);
        display.println("¡Empiece Ahora!");
        display.display();
        delay(100);
        
        //Extracción de datos
        featuresExtraction();

        //Imprime los datos recogidos del sensor MPU6050

        ////////////////// Caracteristicas Acelerómetro //////////////////
        Serial.println(mavXAxis);
        Serial.println(mavYAxis);
        Serial.println(mavZAxis);
        
        Serial.println(rmsXAxis);
        Serial.println(rmsYAxis);
        Serial.println(rmsZAxis);

        Serial.println(wlXAxis);
        Serial.println(wlYAxis);
        Serial.println(wlZAxis);

        ////////////////// Caracteristicas Giroscopio //////////////////
        Serial.println(GmavXAxis);
        Serial.println(GmavYAxis);
        Serial.println(GmavZAxis);
        
        Serial.println(GrmsXAxis);
        Serial.println(GrmsYAxis);
        Serial.println(GrmsZAxis);

        Serial.println(GwlXAxis);
        Serial.println(GwlYAxis);
        Serial.println(GwlZAxis);

        //La bandera regresa a false
        flag = false;

        //Mensaje de fin de ingreso de datos
        display.clearDisplay();
        display.setCursor(23,32);
        display.println("¡Muy bien!");
        display.display();            
        delay(2000);
        display.clearDisplay();    //Finaliza el proceso y reinicia el reloj
    }
}


