// "CAN-Prog01-RMD-X8-PRO-02.ino"
// Con RMDx8ArduinoUBU.h, cuyos elementos se ajustan a los nombres reales del fabricante 
// y añade todas las funciones que le faltaban al original. 

#include <mcp_can.h>
#include <SPI.h>
#include <RMDx8ArduinoUBU.h> // Obliga a utilizar "mcp_can.h"
#define BAUDRATE 115200

//const uint16_t MOTOR_ADDRESS = 0x141; //0x140 + ID(1~32)
int MOTOR_ADDRESS1 = 322; //0x140 + ID(1~32) = 320 + 2
const int SPI_CS_PIN = 10;
MCP_CAN CAN(SPI_CS_PIN); //set CS PIN

RMDx8ArduinoUBU rmd1(CAN, MOTOR_ADDRESS1);
float pos01;

void setup() {
  Serial.begin(BAUDRATE);
  while (CAN_OK != CAN.begin(CAN_500KBPS)) {
    Serial.println("CAN BUS Init Failed");
    delay(100);
  }
  Serial.println("CAN BUS Init OK!");

  //SPI.begin();  
  rmd1.canSetup();
  rmd1.readPID();
  // Con el juego modificado de variables de RMDx8ArduinoUBU.cpp con variables como "poskp",... 
  Serial.print("anglePidKp: "); Serial.print(rmd1.anglePidKp); 
  Serial.print(" - anglePidKi: "); Serial.print(rmd1.anglePidKi); 
  Serial.print(" - MOTOR_ADDRESS1: "); Serial.print(rmd1.MOTOR_ADDRESS); 
  delay(2000);
  rmd1.readEncoder();
  pos01 = rmd1.encoder; // No variaría mientras no haya respuesta del motor
}

void loop() {
    //rmd1.writeCurrent(0);
    //Mostrar_datos();
    rmd1.readEncoder(); 
    Mostrar_datos();
    delay(500);
 }

void Mostrar_datos() {
    Serial.print("POS1: "); Serial.print(rmd1.encoder); Serial.print("\t");
    Serial.print("Temp1: "); Serial.print(rmd1.temperature); Serial.print("\t");

    Serial.print("\nValor de la posición calculado ((36000/360)=100 ::: (65536/360)=182) ::: (182/100)=18.2): ");
    Serial.println(rmd1.reply_buf[6]);

    Serial.print("MOTOR_ADDRESS1: ");
    Serial.print(rmd1.MOTOR_ADDRESS); Serial.println("\t");
    Serial.print("cmd_buf: "); Serial.print("\t");
    Serial.print(rmd1.cmd_buf[0], HEX); Serial.print("\t"); Serial.print(rmd1.cmd_buf[1]); Serial.print("\t"); Serial.print(rmd1.cmd_buf[2]); Serial.print("\t"); Serial.print(rmd1.cmd_buf[3]); Serial.print("\t");
    Serial.print(rmd1.cmd_buf[4]); Serial.print("\t"); Serial.print(rmd1.cmd_buf[5]); Serial.print("\t"); Serial.print(rmd1.cmd_buf[6]); Serial.print("\t"); Serial.print(rmd1.cmd_buf[7]); Serial.println("\t");
    Serial.print("reply_buf: "); Serial.print("\t");
    Serial.print(rmd1.reply_buf[0], HEX); Serial.print("\t"); Serial.print(rmd1.reply_buf[1]); Serial.print("\t"); Serial.print(rmd1.reply_buf[2]); Serial.print("\t"); Serial.print(rmd1.reply_buf[3]); Serial.print("\t");
    Serial.print(rmd1.reply_buf[4]); Serial.print("\t"); Serial.print(rmd1.reply_buf[5]); Serial.print("\t"); Serial.print(rmd1.reply_buf[6]); Serial.print("\t"); Serial.print(rmd1.reply_buf[7]); Serial.println("\t");
}
