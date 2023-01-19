// "CAN-Prog01-RMD-X8-PRO-UBU-01.ino"
// Con RMDx8ArduinoUBU.h, cuyos elementos se ajustan a los nombres reales del fabricante 
// y añade todas las funciones que le faltaban al original. 

#include <mcp_can.h>
#include <SPI.h>
#include <RMDx8ArduinoUBU.h> // Obliga a utilizar "mcp_can.h"
#define BAUDRATE 115200

//const uint16_t MOTOR_ADDRESS = 0x141; //0x140 + ID(1~32)
int MOTOR_ADDRESS1 = 322; //0x140 + ID(1~32) = 320 + 2
int MOTOR_ADDRESS2 = 321; //0x140 + ID(1~32)
const int SPI_CS_PIN = 10;
MCP_CAN CAN(SPI_CS_PIN); //set CS PIN

RMDx8ArduinoUBU rmd1(CAN, MOTOR_ADDRESS1);
RMDx8ArduinoUBU rmd2(CAN, MOTOR_ADDRESS2);
float pos01;
float pos02;

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
  rmd2.canSetup();
  rmd2.readPID();
  Serial.print("anglePidKp-1: "); Serial.print(rmd1.anglePidKp); 
  Serial.print(" - anglePidKi-1: "); Serial.print(rmd1.anglePidKi); 
  Serial.print(" - MOTOR_ADDRESS1: "); Serial.println(rmd1.MOTOR_ADDRESS); 
  Serial.print("anglePidKp-2: "); Serial.print(rmd2.anglePidKp); 
  Serial.print(" - anglePidKi-2: "); Serial.print(rmd2.anglePidKi); 
  Serial.print(" - MOTOR_ADDRESS2: "); Serial.println(rmd2.MOTOR_ADDRESS); 
  delay(2000);
}

void loop() {
    Serial.print("pos01= "); Serial.print(pos01); Serial.println("\t");
    pos01 = pos01 + 10; // Varía muy despacio y se muestran las lecturas
    rmd1.writePosition3(pos01);
    Mostrar_datos1();
    delay(500);
}
void Mostrar_datos1() {
    Serial.print("Command: "); Serial.print(rmd1.command); 
    Serial.print(" - POSEncoder1: "); Serial.print(rmd1.encoder); 
    Serial.print(" - Temp1: "); Serial.print(rmd1.temperature); 
    
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
void Mostrar_datos2() {
    Serial.print("POS-2: "); Serial.print(rmd2.encoder); Serial.print("\t");
    Serial.print("Temp-2: "); Serial.print(rmd2.temperature); Serial.print("\t");

    Serial.print("MOTOR_ADDRESS2: ");
    Serial.print(rmd2.MOTOR_ADDRESS); Serial.println("\t");
    Serial.print("cmd_buf-2: "); Serial.print("\t");
    Serial.print(rmd2.cmd_buf[0], HEX); Serial.print("\t"); Serial.print(rmd2.cmd_buf[1]); Serial.print("\t"); Serial.print(rmd2.cmd_buf[2]); Serial.print("\t"); Serial.print(rmd2.cmd_buf[3]); Serial.print("\t");
    Serial.print(rmd2.cmd_buf[4]); Serial.print("\t"); Serial.print(rmd2.cmd_buf[5]); Serial.print("\t"); Serial.print(rmd2.cmd_buf[6]); Serial.print("\t"); Serial.print(rmd2.cmd_buf[7]); Serial.println("\t");
    Serial.print("reply_buf-2: "); Serial.print("\t");
    Serial.print(rmd2.reply_buf[0]); Serial.print("\t"); Serial.print(rmd2.reply_buf[1]); Serial.print("\t"); Serial.print(rmd2.reply_buf[2]); Serial.print("\t"); Serial.print(rmd2.reply_buf[3]); Serial.print("\t");
    Serial.print(rmd2.reply_buf[4]); Serial.print("\t"); Serial.print(rmd2.reply_buf[5]); Serial.print("\t"); Serial.print(rmd2.reply_buf[6]); Serial.print("\t"); Serial.print(rmd2.reply_buf[7]); Serial.println("\t");
}
