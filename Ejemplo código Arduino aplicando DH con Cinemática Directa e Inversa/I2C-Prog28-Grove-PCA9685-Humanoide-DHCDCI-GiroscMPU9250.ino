// Humanoide (2 brazos y 2 piernas)
// Arquitectura de 2 brazos (3 motores por brazo) y 2 piernas (5 motores por pierna) – Caminar sobre trayectoria regular usando Cinemática Inversa - caminarPasoRegular()
// Dos tarjeta de expansión con un total de 16 motores.
// Los movimientos de una pierna se van a hacer sin reforzar la posición de la otra para no debilitar su par de los motores.

#include <Wire.h> // Adafruit
#include <Adafruit_PWMServoDriver.h> // Adafruit
// Se usarán 2 tarjetas Adafruit para dar soporte a los 8 motores de cada tarjeta y mantener las mejores condiciones de voltaje e intensidad en cada motor.
Adafruit_PWMServoDriver servos1 = Adafruit_PWMServoDriver(0x40); // Controla la pierna y el brazo izquierdos // Otras: (0x50)(0x44)
Adafruit_PWMServoDriver servos2 = Adafruit_PWMServoDriver(0x42); // Controla la pierna y el brazo derechos

///////////////////////////
// I2Cdev and MPU9250 
#include "I2Cdev.h"
#include "RTIMUSettings.h"
#include "RTIMU.h"
#include "RTFusionRTQF.h" 
#include "BMP280.h" // Se introduce porque la biblioteca básica no incluye este sensor.
float xAxis; // Variables que definene las componentes Roll, Pitc, Yaw
float yAxis;
float zAxis;
float temperature;
float pressure;
float atm;
float altitude;
BMP280 bmp280; // Uso de la bibloteca de temperatura, presión y altura
//##################
RTIMU *imu;               // the IMU object
RTFusionRTQF fusion;      // the fusion object
RTIMUSettings settings;   // the settings object
//  DISPLAY_INTERVAL sets the rate at which results are displayed
#define DISPLAY_INTERVAL  100 // 300   // Interval between pose displays
//  SERIAL_PORT_SPEED defines the speed to use for the debug serial port
#define  SERIAL_PORT_SPEED  115200
unsigned long lastDisplay;
unsigned long lastRate;
int sampleCount;

double varXOffsetFinal = 0;
double varYOffsetFinal = 0;
double varXcorregirFinal = 0;
double varYcorregirFinal = 0;

///////////////////////////

int imprimir0 = 1; // Para imprimir o no las variables a analizar.
// Variables que guardarán las asignaciones de los grados en cada comando servos1/2/3/4.setPWM(). // Motores SPT5535LV-320
double limMin[] =            {100, 100, 100, 100, 100,   100, 100, 100,   100, 100, 100, 100, 100,   100, 100, 100}; // Límites mínimos en pulsos de cada motor. Por debajo se daña o da vueltas sin parar.
double limMax[] =            {700, 700, 700, 700, 700,   700, 700, 700,   700, 700, 700, 700, 700,   700, 700, 700}; // Límites máximos en pulsos de cada motor. Por encima se daña o da vueltas sin parar.
double msec0[] =             {407, 388, 393, 390, 403,   398, 398, 398,   397, 408, 413, 405, 380,   398, 398, 398}; // Pulsos de cada motor equivalente a "0" grados.
double msecMin[] =           {251, 232, 237, 233, 247,   242, 242, 242,   241, 252, 257, 244, 230,   242, 242, 242}; // Mínimos pulsos de cada motor. Es equivalente al array msecMin[].
double msecMax[] =           {563, 544, 549, 546, 559,   554, 554, 554,   553, 564, 571, 556, 542,   554, 554, 554}; // Máximos pulsos de cada motor. Es equivalente al array msecMax[].
double angMin[] =            {-90, -90,  90, -90, -90,    90,  90,  90,    90,  90, -90,  90,  90,   -90, -90, -90}; // Mínimos ángulos de cada motor 
double angMax[] =            { 90,  90, -90,  90,  90,   -90, -90, -90,   -90, -90,  90, -90, -90,    90,  90,  90}; // Máximos ángulos de cada motor
double posReposoDePie[] =    {407, 388, 393, 390, 403,   398, 398, 398,   397, 408, 413, 405, 380,   398, 398, 398}; // Pulsos de cada motor para reposo (de pie).
double posPasoAlto[] =       {335, 235, 170, 355, 475,   540, 335, 225,   170, 355, 455, 540, 540,   455, 540, 540}; // Pulsos de cada motor para el paso alto.
double posPasoAdelantado[] = {335, 285, 285, 355, 405,   405, 335, 285,   285, 355, 385, 405, 405,   385, 405, 405}; // Pulsos de cada motor para el paso adelantado.
double posPasoBajo[] =       {385, 385, 385, 385, 385,   385, 385, 385,   385, 385, 385, 385, 385,   385, 385, 385}; // Pulsos de cada motor para el paso bajo.
double posPasoRetrasado[] =  {335, 385, 385, 355, 285,   315, 335, 385,   390, 355, 285, 305, 305,   285, 305, 305}; // Pulsos de cada motor para el paso retrasado.

/* Motores MG996R
double limMin[] =            {160, 160, 160, 160, 160,   160, 160, 160,   160, 160, 160, 160, 160,   160, 160, 160}; // Límites mínimos en pulsos de cada motor. Por debajo se daña o da vueltas sin parar.
double limMax[] =            {666, 666, 666, 666, 666,   666, 666, 666,   666, 666, 666, 666, 666,   666, 666, 666}; // Límites máximos en pulsos de cada motor. Por encima se daña o da vueltas sin parar.
double msec0[] =             {385, 390, 385, 385, 415,   385, 385, 385,   385, 395, 420, 395, 380,   385, 385, 385}; // Pulsos de cada motor equivalente a "0" grados.
double msecMin[] =           {176, 181, 176, 176, 206,   176, 176, 176,   176, 186, 211, 186, 171,   176, 176, 176}; // Mínimos pulsos de cada motor. Es equivalente al array msecMin[].
double msecMax[] =           {594, 599, 594, 594, 624,   594, 594, 594,   594, 604, 629, 604, 589,   594, 594, 594}; // Máximos pulsos de cada motor. Es equivalente al array msecMax[].
double angMin[] =            {-90, -90, -90, -90,  90,   -90, -90, -90,   -90, -90, -90, -90, -90,   -90, -90, -90}; // Mínimos ángulos de cada motor 
double angMax[] =            { 90,  90,  90,  90, -90,    90,  90,  90,    90,  90,  90,  90,  90,    90,  90,  90}; // Máximos ángulos de cada motor
double posReposoDePie[] =    {385, 390, 385, 385, 415,   385, 385, 385,   385, 395, 420, 395, 380,   385, 385, 385}; // Pulsos de cada motor para reposo (de pie).
double posPasoAlto[] =       {335, 235, 170, 355, 475,   540, 335, 225,   170, 355, 455, 540, 540,   455, 540, 540}; // Pulsos de cada motor para el paso alto.
double posPasoAdelantado[] = {335, 285, 285, 355, 405,   405, 335, 285,   285, 355, 385, 405, 405,   385, 405, 405}; // Pulsos de cada motor para el paso adelantado.
double posPasoBajo[] =       {385, 385, 385, 385, 385,   385, 385, 385,   385, 385, 385, 385, 385,   385, 385, 385}; // Pulsos de cada motor para el paso bajo.
double posPasoRetrasado[] =  {335, 385, 385, 355, 285,   315, 335, 385,   390, 355, 285, 305, 305,   285, 305, 305}; // Pulsos de cada motor para el paso retrasado.
*/
int msecMotor[16]; // Pulsos de cada posición de cada motor de cada brazo en cada momento del programa - 205 - 495 - 345
double angMotor[16]; // Ángulo de cada posición de cada motor de cada brazo en cada momento del programa
int tareasConcurrentes[4]; // Será "1" cuando se deba ejecutar el movimiento de cada brazo de forma paralela. Una fracción cada vez.
double fraccion[16]; // Contiene el valor calculado de cada fracción para cada motor de cada brazo en movimiento paralelo.
  
// Fracciones de cada pequeño movimiento. Por encima de 10 dará problemas porque no podrá dividir el dato en movimientos perceptibles y las unidades de 
// pulsos son enteras y no decimales.
double divisor = 10; // 20 - Utilizado para crear las fracciones de movimiento y que debe ser el número de iteraciones de pequeños movimientos

// Lp1, Lp2, Lp3, Lp4, Lp5, Lp5v, Lb1, Lb2, Lb3, Lb3, Lb3v - números en milímetros
double Lp1 = 8.3; // Distancia desde el primer motor (primera mitad de la cadera) hasta el segundo motor (segunda rotación de la cadera).
double Lp2 = 7.8; // Distancia desde el segundo motor (segunda mitad de la cadera) hasta el tercer motor (rodilla).
double Lp3 = 6.4; // Distancia desde el tercer motor (rodilla) hasta el cuarto motor (segunda rotación del tobillo) .
double Lp4 = 4.8; // Distancia desde el cuarto motor (segunda rotación del tobillo) hasta el quinto motor (primera rotación del tobillo).
double Lp5 = 1.6; // Distancia desde el quinto motor (primera rotación del tobillo) hasta el suelo.
double Lp5v = 9.8;// Distancia desde la altura del cuarto motor de la pierna (cadera) hasta el centro del robot (vertical del centro del robot a la altura de la cadera).
double Lb1 = 2.4; // Distancia desde el segundo motor del brazo (codo) hasta la altura del tercer motor (altura del hombro).
double Lb2 = 9.8; // Distancia desde el primer motor del brazo (muñeca) hasta el segundo motor (codo).
double Lb3 = 7.2; // Distancia desde el extremo de la mano hasta el primer motor (muñeca).
double Lb3v = 9.8;// Distancia desde la altura del tercer motor del brazo (hombro) hasta el centro del robot (vertical del centro del robot a la altura del hombro).
  
double Teta1pgrados = 0; // Grados de movimiento aplicados al primer motor de la cadera. 
double Teta2pgrados = 0; 
double Teta3pgrados = 0;
double Teta4pgrados = 0; 
double Teta5pgrados = 0;
double Teta1bgrados = 0;
double Teta2bgrados = 0;
double Teta3bgrados = 0;
double Teta1psec = 0; // Pulsos de movimiento aplicados al primer motor de la cadera. 
double Teta2psec = 0;
double Teta3psec = 0;
double Teta4psec = 0;
double Teta5psec = 0;
double Teta1bsec = 0;
double Teta2bsec = 0;
double Teta3bsec = 0;

double tiempoPausaDivisorMovimientos = 80; // 18 - Tiempo de pausa de los motores para moverse en paralelo
double tiempoEntreMovimientos = 80; // 18 - Tiempo entre movimientos de los distintos brazos para no producirse intersecciones de intervalos de tiempos al moverse en paralelo
double alturaEscalon = 10.0; // Altura del escalón
double puntoF[] = {0, 0, 0};
double puntoF_2[] = {0, 0, 0.5};

//************************************************************************************************
// Se definen las componentes de la matriz A5 para las piernas con 5 motores
double p5varDH00=0; double p5varDH01=0; double p5varDH02=0; double p5varDH03=0;
double p5varDH10=0; double p5varDH11=0; double p5varDH12=0; double p5varDH13=0;
double p5varDH20=0; double p5varDH21=0; double p5varDH22=0; double p5varDH23=0;
double p5varDH30=0; double p5varDH31=0; double p5varDH32=0; double p5varDH33=1;

double p5varDH00_2=0; double p5varDH01_2=0; double p5varDH02_2=0; double p5varDH03_2=0;
double p5varDH10_2=0; double p5varDH11_2=0; double p5varDH12_2=0; double p5varDH13_2=0;
double p5varDH20_2=0; double p5varDH21_2=0; double p5varDH22_2=0; double p5varDH23_2=0;
double p5varDH30_2=0; double p5varDH31_2=0; double p5varDH32_2=0; double p5varDH33_2=1;

// Se definen las componentes de la matriz A3 para los brazos con 3 motores
double b3varDH00=0; double b3varDH01=0; double b3varDH02=0; double b3varDH03=0;
double b3varDH10=0; double b3varDH11=0; double b3varDH12=0; double b3varDH13=0;
double b3varDH20=0; double b3varDH21=0; double b3varDH22=0; double b3varDH23=0;
double b3varDH30=0; double b3varDH31=0; double b3varDH32=0; double b3varDH33=1;

double b3varDH00_2=0; double b3varDH01_2=0; double b3varDH02_2=0; double b3varDH03_2=0;
double b3varDH10_2=0; double b3varDH11_2=0; double b3varDH12_2=0; double b3varDH13_2=0;
double b3varDH20_2=0; double b3varDH21_2=0; double b3varDH22_2=0; double b3varDH23_2=0;
double b3varDH30_2=0; double b3varDH31_2=0; double b3varDH32_2=0; double b3varDH33_2=1;

//************************************************************************************************
// Se definen los parámetros de la matriz DH a través de vectores de tantos datos como articulaciones tiene el brazo. Estos valores podrán
// ser fijos para hacer las pruebas, pero variarán en la medida que se definan nuevos datos para alcanzar puntos diferentes con el extremo. 
// Una opción posible sería que realizaran un círulo modificando los valores de θi, di, ai o αi. 
// En el caso de hacer las pruebas de cinemática inversa se calcularían los puntos finales o la trayectoria, y con estos puntos se 
// calcularían los valores de θi, di, ai o αi. Por ejemplo ser podría calcular la trayectoria de un círculo a cierta altura.
// Se van a definir los parámetros de DH para el caso de 5 articulaciones y se meten 7 datos para prepararlo para 7 articulaciones:
//*****************************************
// Ejemplo 7.0 // Se cambian los signos de las longitudes porque sustituyen a las ri en el original de la matriz, osea sin cambiar a L que sí tienen signo.
double p5DHbi2[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // Valores de los datos bi de DH para calcular el punto final
double p5DHdi[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // Valores de los datos di de DH 
double p5DHri[] = {-8.3, -7.8, -6.4, -4.8, -1.6, 0, 0, 0, 0, 0}; // Valores de los datos ri de DH 
double p5DHai[] = {-90, 0, 0, 90, 0, 0, 0, 0, 0, 0}; // Valores de los datos ai de DH 
double b1; double b2; double b3; double b4; double b5;  
double d1; double d2; double d3; double d4; double d5; 
double r1; double r2; double r3; double r4; double r5; 
double a1; double a2; double a3; double a4; double a5; 
double b3DHbi2[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // Valores de los datos bi de DH para calcular el punto final
double b3DHdi[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // Valores de los datos di de DH 
double b3DHri[] = {-2.4, -9.8, -7.2, 0, 0, 0, 0, 0, 0, 0}; // Valores de los datos ri de DH 
double b3DHai[] = {-90, 0, 90, 0, 0, 0, 0, 0, 0, 0}; // Valores de los datos ai de DH 
double bb1; double bb2; double bb3; double bb4; double bb5;  
double bd1; double bd2; double bd3; double bd4; double bd5; 
double br1; double br2; double br3; double br4; double br5; 
double ba1; double ba2; double ba3; double ba4; double ba5; 

int numart = 0; //Número de motores = número de filas de la matriz DH con 4 columnas. 
double GiroCalculado[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // Ángulo a girar por cada motor tras ser calculado por DH y otros procedimientos como SCARA. Contendrá los Teta1grados...

//****************************************************************************************
// Variables para caminar
  int brazoN1 = 3; // Brazo Derecho (4) y Pierna Izquierda (1) (Brazo Izquierdo (3) y Pierna Derecha (2))
  int brazoN2 = 4;
  int piernaN1 = 1;
  int piernaN2 = 2;
  double Long5i = 0; // Componente x de avance en cada paso. // Avance en el movimiento de i-nclinación lateral de robot (+ hacia adelante, - hacia atrás).
  double Long6i = 1.5; // Componente y de separación a mayores entre pies para estabilizar el robot. // Movimiento lateral en el movimiento de i-nclinación lateral de robot (1.5+ HACIA AFUERA, 1.5- HACIA ADENTRO)-
  double Long7i = -25; // Altura (Componente z destino. // Altura negativa en el movimiento de i-nclinación lateral de robot (SIEMPRE - POR SER POR DEBAJO DE LA CADERA).
  double possig = 7; // Longitud del paso en cm. Es la distancia a avanzar en cada paso en paralelo al movimiento sobre el eje X (Z en DH).
  double DesfaseEquilibrioLateralDosPiernas = 6.5; // Movimiento lateral para equilibrar el Centro de Gravedad sobre una única pierna.
  double DesfaseEquilibrioLateralUnaPierna = 6.5; // Movimiento lateral para equilibrar el Centro de Gravedad sobre una única pierna.
  double DesfaseEquilibrioFrontal = 1; // Movimiento hacia adelante de la pierna de apoyo para equilibrar el Centro de Gravedad sobre una pierna al avanzar con la otra pierna.
  double AlturaDeParadaAntesDeSuelo = 2; // Altura de equilibrio antes de continuar hacia el suelo.
  double AlturaDeParadaAlta = 7; // Altura de equilibrio antes de continuar hacia el suelo.
  double bpossig = 7.5; // Es la distancia a avanzar en la mano contraria al pie en movimiento, en paralelo al movimiento sobre el eje X (Z en DH).
  double SepararManoDentro = -2; // Es la distancia a separar la mano de la vertical del cuerpo para caminar.
  double SepararManoFuera = -7.5; // Es la distancia a separar la mano de la vertical del cuerpo para caminar.
  double AlturaMano = -16; // Altura de la mano respecto al hombro al andar.

//****************************************************************************************
//****************************************************************************************

//------------
// Funciones comunes para cinemática directa e inversa
double radianesAGrados(double radianes) { // Pasa ángulos en radianes a grados – Datos: Radianes
  //Serial.println("***  Pasa - radianesAGrados()--------"); 
  //Serial.print("radianesAGrados()"); Serial.print(" - Grados= "); Serial.println(radianes * (180.0 / Pi));
  return radianes * (180.0 / PI);
}
double gradosARadianes(double& grados) { // Pasa ángulos en radianes a grados – Datos: Radianes
  //Serial.println("***  Pasa - gradosARadianes()--------"); 
  //Serial.print("radianesAGrados()"); Serial.print(" - Grados= "); Serial.println(grados * Pi / 180.0);
  return grados * PI / 180.0;
}
//------ Funciones de movimiento de brazos y robot --------------------
double msecAGrados(double msec, int brazoN, int clavemotor) { // Pasa ángulos en pulsos a grados  – Datos: Pulsos, brazo, número de motor
  //Serial.println("***  Pasa - msecAGrados()--------"); 
  int z = (brazoN-1)*3 + clavemotor;
  return (map(msec, msecMin[z], msecMax[z], angMin[z], angMax[z]));
}
double gradosAMsec(double& grados, int brazoN, int clavemotor) { // Pasa ángulos en grados a pulsos – Datos: Grados, brazo, número de motor
  //Serial.println("***  Pasa - gradosAMsec()--------"); 
  int z = (brazoN-1)*3 + clavemotor;
  return (map(grados, angMin[z], angMax[z], msecMin[z], msecMax[z]));
}

void setServos01234(){ // Se inicializan los motores en la posición inicial (posReposoDePie[]) para mantenerse de pie, ya corregidos para que sea la posición deseada.
  // Los motores se montan en su posición media de giro para tener recorrido suficiente hacia ambos sentidos de giro. 
  // Pero dependen de cada sistema de montaje y necesidades particulares. En este caso son de 320º de giro. 
  Serial.println(F("\n-------- Posicionamiento Inicial (posReposoDePie[]) --------"));
  int i; 
  for (i = 0; i <= 7; i = i + 1) { // Recorre los 2 brazos y 4 piernas e inicialia los motores a posReposoDePie[i]. 
      servos1.setPWM(i, 0, posReposoDePie[i]); // int setbounds(int servo, int minposition, int maxposition); (Returns 0 if successfully set; returns 1 if instruction failed) // Adafruit
      msecMotor[i] = posReposoDePie[i]; //Se inicializan para comenzar desde un valor conocido.
      angMotor[i] = map(posReposoDePie[i], msecMin[i], msecMax[i], angMin[i], angMax[i]);
      servos2.setPWM(i, 0, posReposoDePie[i+8]); // int setbounds(int servo, int minposition, int maxposition); (Returns 0 if successfully set; returns 1 if instruction failed) // Adafruit
      msecMotor[i+8] = posReposoDePie[i+8]; //Se inicializan para comenzar desde un valor conocido.
      angMotor[i+8] = map(posReposoDePie[i+8], msecMin[i+8], msecMax[i+8], angMin[i+8], angMax[i+8]);
      Serial.print(F("\nINICIADO - Multi 1 (Izquierda) - Motor ")); Serial.print(i); Serial.print(F(" - angMotor = ")); Serial.print(angMotor[i]);
      Serial.print(F(" - msecMotor = ")); Serial.print(msecMotor[i]); Serial.print(F(" - posReposoDePie = ")); Serial.print(posReposoDePie[i]);
      Serial.print(F("\nINICIADO - Multi 2 (Derecha) - Motor ")); Serial.print(i+8); Serial.print(F(" - angMotor = ")); Serial.print(angMotor[i+8]);
      Serial.print(F(" - msecMotor = ")); Serial.print(msecMotor[i+8]); Serial.print(F(" - posReposoDePie = ")); Serial.print(posReposoDePie[i+8]);
  }
}

void prepararMovimientoDeUnaPierna(int brazoN, double Teta1psec, double Teta2psec, double Teta3psec, double Teta4psec, double Teta5psec) {
  // Prepara los movimientos de cada pierna. Controla cada motor sea cual sea lapierna a través de la fracción a ejecutar, en cada instante.
  //Serial.println("***  Pasa - prepararMovimientoDeUnaPierna()--------"); 
  int z = ((brazoN-1)*8); // Empieza en 0 o en 7 al tratarse sólo de las piernas
  if ((msecMotor[z] > limMin[z]) and (msecMotor[z] < limMax[z]) and (msecMotor[z+1] > limMin[z+1]) and (msecMotor[z+1] < limMax[z+1]) and (msecMotor[z+2] > limMin[z+2]) and (msecMotor[z+2] < limMax[z+2]) ) {     
    //Serial.print("* msecMotor[z]: "); Serial.print(msecMotor[z]);Serial.print(" - msecMotor[z+1]: "); Serial.print(msecMotor[z+1]);Serial.print(" - msecMotor[z+2]: "); Serial.print(msecMotor[z+2]); // Serial.print(" - datoModulo: "); Serial.println(datoModulo);
    //angMotor[z+1] = map(msec0[z+1], msecMin[z+1], msecMax[z+1], angMin[z+1], angMax[z+1]);  // No sirve para nada salvo para guardar los valores.
    //double Teta1psec; // Teta1psec, ... - No se inicializan porque deben contener el dato calculado.
    // Sólo se ejecutan movimientos en los que tareasConcurrentes[i]==1. El resto de brazos no refuerza la posición para no debilitar el par de los motores.
      if (z < 8) { // Los 5 primeros servos se ejecutan sobre la primera tarjeta de expansión
        fraccion[z] = (Teta1psec - msecMotor[z]) / divisor; // Teta1psec
        fraccion[z+1] = (Teta2psec - msecMotor[z+1]) / divisor; // Teta2psec
        fraccion[z+2] = (Teta3psec - msecMotor[z+2]) / divisor; // Teta3psec
        fraccion[z+3] = (Teta4psec - msecMotor[z+3]) / divisor; // Teta4psec
        fraccion[z+4] = (Teta5psec - msecMotor[z+4]) / divisor; // Teta5psec
      } else { // Los siguientes 5 servos trabajan sobre la segunda tarjeta de expansión
        fraccion[z] = (Teta1psec - msecMotor[z]) / divisor; // Teta1psec
        fraccion[z+1] = (Teta2psec - msecMotor[z+1]) / divisor; // Teta2psec
        fraccion[z+2] = (Teta3psec - msecMotor[z+2]) / divisor; // Teta3psec
        fraccion[z+3] = (Teta4psec - msecMotor[z+3]) / divisor; // Teta4psec
        fraccion[z+4] = (Teta5psec - msecMotor[z+4]) / divisor; // Teta5psec
      }
    tareasConcurrentes[brazoN-1] = 1;

  }else{
    Serial.println(F("***  Fuera de límites - msecMotor de algún motor de la pierna inalcanzable por los límites definidos------------------------------------------------------"));   
    Serial.print("* msecMotor[z]: "); Serial.print(msecMotor[z]);Serial.print(" - msecMotor[z+1]: "); Serial.print(msecMotor[z+1]);
    Serial.print(" - msecMotor[z+2]: "); Serial.print(msecMotor[z+2]); Serial.print(" - msecMotor[z+3]: "); Serial.print(msecMotor[z+3]);
    Serial.print(" - msecMotor[z+4]: "); Serial.println(msecMotor[z+4]);       
  }
  return; 
} 
void prepararMovimientoDeUnBrazo(int brazoN,double Teta1bsec,double Teta2bsec,double Teta3bsec) {
  // Prepara los movimientos de cada brazo. Controla cada motor sea cual sea el brazo a través de la fracción a ejecutar, en cada instante.
  Serial.println("***  Pasa - prepararMovimientoDeUnBrazo()--------"); 
  int z = ((brazoN-3)*8); // Empieza en 5 o en 13 al tratarse sólo de los brazos
  if ((msecMotor[z] > limMin[z]) and (msecMotor[z] < limMax[z]) and (msecMotor[z+1] > limMin[z+1]) and (msecMotor[z+1] < limMax[z+1]) and (msecMotor[z+2] > limMin[z+2]) and (msecMotor[z+2] < limMax[z+2]) ) { // and (datoModulo < 50)     
    //angMotor[z+1] = map(msec0[z+1], msecMin[z+1], msecMax[z+1], angMin[z+1], angMax[z+1]);  // No sirve para nada salvo para guardar los valores.
    fraccion[z+5] = (Teta1bsec - msecMotor[z+5]) / divisor; // tita1bsec
    fraccion[z+6] = (Teta2bsec - msecMotor[z+6]) / divisor; // tita2bsec
    fraccion[z+7] = (Teta3bsec - msecMotor[z+7]) / divisor; // tita3bsec
    tareasConcurrentes[brazoN-1] = 1;
  }else{
    Serial.println(F("***  Fuera de límites - msecMotor de algún motor del brazo inalcanzable por los límites definidos------------------------------------------------------"));   
    Serial.print("* msecMotor[z+5]: "); Serial.print(msecMotor[z+5]);Serial.print(" - msecMotor[z+6]: "); Serial.print(msecMotor[z+6]);
    Serial.print(" - msecMotor[z+7]: "); Serial.print(msecMotor[z+7]); 
  }
  return; 
} 
void ejecutarParalelo() { 
  // Ejecuta los movimientos de cada pierna y/o brazo en paralelo independientemente del número de estos y de cuáles sean.
  // Se recorren las 2 piernas y los 2 brazos verificando si se ha definido algún movimiento en cada uno de ellos (levantar, andar, avanzar, ...).
  // Por ejemplo en el caso de ejecutar pasos en un camino regular.
  // Está basado en modificaciones de destino con puntoFinalAbsoluto() y el preparado de los movimientos con prepararMovimientoDeUnaPierna()/prepararMovimientoDeUnBrazo() cuando camina
  Serial.println("***  Pasa ------------------ ejecutarParalelo() ------------------");
  int indpos;
  int i;
  int z;
  int zz;
  for (indpos = 1; indpos <= divisor; indpos  += 1) { // Movimientos divididos en un número de movimientos más pequeños (100)
    delay(tiempoPausaDivisorMovimientos);
    for (i = 0; i <= 3; i  += 1) { // Se recorren todos los brazos. 
      // Sólo se ejecutan movimientos en los que tareasConcurrentes[i]==1. El resto de brazos no refuerza la posición para no debilitar el par de los motores.
      if (tareasConcurrentes[i] == 1) {
        //Serial.print("indpos:"); Serial.print(indpos);Serial.print(" - i:"); Serial.print(i);
        //Serial.print(" - tareasConcurrentes[i]:"); Serial.println(tareasConcurrentes[i]);
        if ((i == 0) or (i == 1)) {
          z = (i*8);
          //Serial.print("indpos:"); Serial.print(indpos);Serial.print(" - i:"); Serial.print(i);Serial.print(" - z:"); Serial.print(z);Serial.print(" - fraccion[z]:"); Serial.println(fraccion[z]);
          //Serial.print("indpos:"); Serial.print(indpos);Serial.print(" - i:"); Serial.print(i);Serial.print(" - z:"); Serial.print(z);Serial.print(" - fraccion[z]:"); Serial.println(fraccion[z]);
          //Serial.print(" - fraccion[z+1]:"); Serial.println(fraccion[z+1]); Serial.print(" - fraccion[z+2]:"); Serial.println(fraccion[z+2]);
          msecMotor[z] = msecMotor[z] + fraccion[z]; // SE SUPONE QUE ESTOS MOTORES YA ESTABAN EN SU POSICIÓN FINAL // Éste estaría repetido y quedaría modificado y por tanto mal si no fuera siempre la misma posición.
          msecMotor[z+1] = msecMotor[z+1] + fraccion[z+1]; 
          msecMotor[z+2] = msecMotor[z+2] + fraccion[z+2];
          msecMotor[z+3] = msecMotor[z+3] + fraccion[z+3];
          msecMotor[z+4] = msecMotor[z+4] + fraccion[z+4];
          angMotor[z] = map(msecMotor[z], msec0[z], msecMax[z], 0, angMax[z]);  // No sirve para nada salvo para guardar el dato.
          angMotor[z+1] = map(msecMotor[z+1], msec0[z+1], msecMax[z+1], 0, angMax[z+1]);  // No sirve para nada salvo para guardar el dato.
          angMotor[z+2] = map(msecMotor[z+2], msec0[z+2], msecMax[z+2], 0, angMax[z+2]);  // No sirve para nada salvo para guardar el dato.
          angMotor[z+3] = map(msecMotor[z+3], msec0[z+3], msecMax[z+3], 0, angMax[z+3]);  // No sirve para nada salvo para guardar el dato.
          angMotor[z+4] = map(msecMotor[z+4], msec0[z+4], msecMax[z+4], 0, angMax[z+4]);  // No sirve para nada salvo para guardar el dato.
          if (i == 0) { // Los 5 primeros servos se ejecutan sobre la primera tarjeta de expansión
            servos1.setPWM(z, 0, msecMotor[z]); 
            servos1.setPWM(z+1, 0, msecMotor[z+1]);
            servos1.setPWM(z+2, 0, msecMotor[z+2]);
            servos1.setPWM(z+3, 0, msecMotor[z+3]);
            servos1.setPWM(z+4, 0, msecMotor[z+4]);
          } else if (i == 1){ // Los siguientes 5 servos trabajan sobre la segunda tarjeta de expansión
            zz = z - 8; // "zz" sólo se usa en el número de servo porque los arrays con los datos mantienen datos de los 16 motores
            servos2.setPWM(zz, 0, msecMotor[z]); 
            servos2.setPWM(zz+1, 0, msecMotor[z+1]);
            servos2.setPWM(zz+2, 0, msecMotor[z+2]);
            servos2.setPWM(zz+3, 0, msecMotor[z+3]);
            servos2.setPWM(zz+4, 0, msecMotor[z+4]);
          }
        }
        if ((i == 2) or (i == 3)) {
          z = ((i-2)*8);
          //Serial.print("indpos:"); Serial.print(indpos);Serial.print(" - i:"); Serial.println(i); 
          //Serial.print(" - z+5:"); Serial.print(z+5); Serial.print(" - fraccion[z+5]:"); Serial.println(fraccion[z+5]); 
          //Serial.print(" - fraccion[z+6]:"); Serial.println(fraccion[z+6]); Serial.print(" - fraccion[z+7]:"); Serial.println(fraccion[z+7]);
          msecMotor[z+5] = msecMotor[z+5] + fraccion[z+5];
          msecMotor[z+6] = msecMotor[z+6] + fraccion[z+6];
          msecMotor[z+7] = msecMotor[z+7] + fraccion[z+7];
          angMotor[z+5] = map(msecMotor[z+5], msec0[z+5], msecMax[z+5], 0, angMax[z+5]);  // No sirve para nada salvo para guardar el dato.
          angMotor[z+6] = map(msecMotor[z+6], msec0[z+6], msecMax[z+6], 0, angMax[z+6]);  // No sirve para nada salvo para guardar el dato.
          angMotor[z+7] = map(msecMotor[z+7], msec0[z+7], msecMax[z+7], 0, angMax[z+7]);  // No sirve para nada salvo para guardar el dato.
          if (i == 2) { // Los 5 primeros servos se ejecutan sobre la segunda tarjeta de expansión
            servos2.setPWM(z+5, 0, msecMotor[z+5]);
            servos2.setPWM(z+6, 0, msecMotor[z+6]);
            servos2.setPWM(z+7, 0, msecMotor[z+7]);
          } else if (i == 3){ // Los siguientes 5 servos trabajan sobre la primera tarjeta de expansión
            zz = z - 8; // "zz" sólo se usa en el número de servo porque los arrays con los datos mantienen datos de los 16 motores
            servos1.setPWM(zz+5, 0, msecMotor[z+5]);
            servos1.setPWM(zz+6, 0, msecMotor[z+6]);
            servos1.setPWM(zz+7, 0, msecMotor[z+7]);
          }
        }
      }
      if (indpos == divisor) {
        //imprimirDatos2(i); //imprimirDatos2(i) - Se descomentará si se quiere seguir el valor de las variables.
        tareasConcurrentes[i] = 0; // La tarea se anula una vez terminado el movimiento y sus fracciones se ponen a "0"
      }
    }
  }
  limpiarVariables();
  return;
}

//************************************************************************************************
// Ejemplo 7-0 - Funciones de prueba de los motores en Cinemática Directa
// Se inicializan los valores de las equivalencias entre el rango de grados y el rango de pulsos.
void calDHdirectoNmotores(int brazoN,double A, double B, double C) { // Para probar la posición correcta del punto final de destino de cada brazo.
  Serial.println(F("*************** calDHdirectoNmotores ***************"));
  // Se ha cambiado los signos de los ángulos 0 y 180... en el motor 1 (segundo motor)
  if ((brazoN == 1) or (brazoN == 2)) {
    numart = 5;
  }else {
    numart = 3;
  }
  
    // Se definen los valores básicos de la matriz DH (bi, di, ri, ai) para 3 articulaciones. Salen de multiplicar las matrices según la definición de los parámetros de Denavit-Hartenberg.
    // Se usan los elementos completos de la matriz T sin haber sido sustituidos los valores, ya que algunos valores anulan términos y simplifican las ecuaciones.
  if (numart == 3) {
    b1 = gradosARadianes(b3DHbi2[0]); b2 = gradosARadianes(b3DHbi2[1]); b3 = gradosARadianes(b3DHbi2[2]); 
    d1 = b3DHdi[0]; d2 = b3DHdi[1]; d3 = b3DHdi[2]; 
    r1 = b3DHri[0]; r2 = b3DHri[1]; r3 = b3DHri[2]; 
    a1 = gradosARadianes(b3DHai[0]); a2 = gradosARadianes(b3DHai[1]); a3 = gradosARadianes(b3DHai[2]); 
    b3varDH00 = ((((cos(b1)))*(cos(b2))+(((-sin(b1))*cos(a1)))*(sin(b2)))*(cos(b3))+(((cos(b1)))*((-sin(b2))*cos(a2))+(((-sin(b1))*cos(a1)))*(cos(b2)*cos(a2))+(((-sin(b1))*(-sin(a1))))*(sin(a2)))*(sin(b3))) ;
    b3varDH01 = ((((cos(b1)))*(cos(b2))+(((-sin(b1))*cos(a1)))*(sin(b2)))*((-sin(b3))*cos(a3))+(((cos(b1)))*((-sin(b2))*cos(a2))+(((-sin(b1))*cos(a1)))*(cos(b2)*cos(a2))+(((-sin(b1))*(-sin(a1))))*(sin(a2)))*(cos(b3)*cos(a3))+(((cos(b1)))*((-sin(b2))*(-sin(a2)))+(((-sin(b1))*cos(a1)))*(cos(b2)*(-sin(a2)))+(((-sin(b1))*(-sin(a1))))*(cos(a2)))*(sin(a3))) ;
    b3varDH02 = ((((cos(b1)))*(cos(b2))+(((-sin(b1))*cos(a1)))*(sin(b2)))*((-sin(b3))*(-sin(a3)))+(((cos(b1)))*((-sin(b2))*cos(a2))+(((-sin(b1))*cos(a1)))*(cos(b2)*cos(a2))+(((-sin(b1))*(-sin(a1))))*(sin(a2)))*(cos(b3)*(-sin(a3)))+(((cos(b1)))*((-sin(b2))*(-sin(a2)))+(((-sin(b1))*cos(a1)))*(cos(b2)*(-sin(a2)))+(((-sin(b1))*(-sin(a1))))*(cos(a2)))*(cos(a3))) ; 
    b3varDH03 = ((((cos(b1)))*(cos(b2))+(((-sin(b1))*cos(a1)))*(sin(b2)))*(cos(b3)*r3)+(((cos(b1)))*((-sin(b2))*cos(a2))+(((-sin(b1))*cos(a1)))*(cos(b2)*cos(a2))+(((-sin(b1))*(-sin(a1))))*(sin(a2)))*(sin(b3)*r3)+(((cos(b1)))*((-sin(b2))*(-sin(a2)))+(((-sin(b1))*cos(a1)))*(cos(b2)*(-sin(a2)))+(((-sin(b1))*(-sin(a1))))*(cos(a2)))*(d3)+(((cos(b1)))*(cos(b2)*r2)+(((-sin(b1))*cos(a1)))*(sin(b2)*r2)+(((-sin(b1))*(-sin(a1))))*(d2)+((cos(b1)*r1))));
    b3varDH10 = ((((sin(b1)))*(cos(b2))+((cos(b1)*cos(a1)))*(sin(b2)))*(cos(b3))+(((sin(b1)))*((-sin(b2))*cos(a2))+((cos(b1)*cos(a1)))*(cos(b2)*cos(a2))+((cos(b1)*(-sin(a1))))*(sin(a2)))*(sin(b3))) ;
    b3varDH11 = ((((sin(b1)))*(cos(b2))+((cos(b1)*cos(a1)))*(sin(b2)))*((-sin(b3))*cos(a3))+(((sin(b1)))*((-sin(b2))*cos(a2))+((cos(b1)*cos(a1)))*(cos(b2)*cos(a2))+((cos(b1)*(-sin(a1))))*(sin(a2)))*(cos(b3)*cos(a3))+(((sin(b1)))*((-sin(b2))*(-sin(a2)))+((cos(b1)*cos(a1)))*(cos(b2)*(-sin(a2)))+((cos(b1)*(-sin(a1))))*(cos(a2)))*(sin(a3))) ;
    b3varDH12 = ((((sin(b1)))*(cos(b2))+((cos(b1)*cos(a1)))*(sin(b2)))*((-sin(b3))*(-sin(a3)))+(((sin(b1)))*((-sin(b2))*cos(a2))+((cos(b1)*cos(a1)))*(cos(b2)*cos(a2))+((cos(b1)*(-sin(a1))))*(sin(a2)))*(cos(b3)*(-sin(a3)))+(((sin(b1)))*((-sin(b2))*(-sin(a2)))+((cos(b1)*cos(a1)))*(cos(b2)*(-sin(a2)))+((cos(b1)*(-sin(a1))))*(cos(a2)))*(cos(a3))) ;
    b3varDH13 = ((((sin(b1)))*(cos(b2))+((cos(b1)*cos(a1)))*(sin(b2)))*(cos(b3)*r3)+(((sin(b1)))*((-sin(b2))*cos(a2))+((cos(b1)*cos(a1)))*(cos(b2)*cos(a2))+((cos(b1)*(-sin(a1))))*(sin(a2)))*(sin(b3)*r3)+(((sin(b1)))*((-sin(b2))*(-sin(a2)))+((cos(b1)*cos(a1)))*(cos(b2)*(-sin(a2)))+((cos(b1)*(-sin(a1))))*(cos(a2)))*(d3)+(((sin(b1)))*(cos(b2)*r2)+((cos(b1)*cos(a1)))*(sin(b2)*r2)+((cos(b1)*(-sin(a1))))*(d2)+((sin(b1)*r1))));
    b3varDH20 = ((((sin(a1)))*(sin(b2)))*(cos(b3))+(((sin(a1)))*(cos(b2)*cos(a2))+((cos(a1)))*(sin(a2)))*(sin(b3))) ;
    b3varDH21 = ((((sin(a1)))*(sin(b2)))*((-sin(b3))*cos(a3))+(((sin(a1)))*(cos(b2)*cos(a2))+((cos(a1)))*(sin(a2)))*(cos(b3)*cos(a3))+(((sin(a1)))*(cos(b2)*(-sin(a2)))+((cos(a1)))*(cos(a2)))*(sin(a3))) ;
    b3varDH22 = ((((sin(a1)))*(sin(b2)))*((-sin(b3))*(-sin(a3)))+(((sin(a1)))*(cos(b2)*cos(a2))+((cos(a1)))*(sin(a2)))*(cos(b3)*(-sin(a3)))+(((sin(a1)))*(cos(b2)*(-sin(a2)))+((cos(a1)))*(cos(a2)))*(cos(a3))) ;
    b3varDH23 = ((((sin(a1)))*(sin(b2)))*(cos(b3)*r3)+(((sin(a1)))*(cos(b2)*cos(a2))+((cos(a1)))*(sin(a2)))*(sin(b3)*r3)+(((sin(a1)))*(cos(b2)*(-sin(a2)))+((cos(a1)))*(cos(a2)))*(d3)+(((sin(a1)))*(sin(b2)*r2)+((cos(a1)))*(d2)+((d1))));
  }
  // Se definen los valores básicos de la matriz DH (bi, di, ri, ai) para 5 articulaciones. Salen de multiplicar las matrices según la definición de los parámetros de Denavit-Hartenberg.
  if (numart == 5) {
    // Sin simplificar. Estos originales se modifican en una hoja de cálculo para reconvertirlos al formato adecuado.
    b1 = gradosARadianes(p5DHbi2[0]); b2 = gradosARadianes(p5DHbi2[1]); b3 = gradosARadianes(p5DHbi2[2]); b4 = gradosARadianes(p5DHbi2[3]); b5 = gradosARadianes(p5DHbi2[4]); 
    d1 = p5DHdi[0]; d2 = p5DHdi[1]; d3 = p5DHdi[2]; d4 = p5DHdi[3]; d5 = p5DHdi[4];
    r1 = p5DHri[0]; r2 = p5DHri[1]; r3 = p5DHri[2]; r4 = p5DHri[3]; r5 = p5DHri[4]; 
    a1 = gradosARadianes(p5DHai[0]); a2 = gradosARadianes(p5DHai[1]); a3 = gradosARadianes(p5DHai[2]); a4 = gradosARadianes(p5DHai[3]); a5 = gradosARadianes(p5DHai[4]); 
/* //      
    // Se usan los elementos completos de la matriz T sin haber sido sustituidos los valores, ya que algunos valores anulan términos y simplifican las ecuaciones.
    p5varDH00 = ((((((cos(b1)))*(cos(b2))+(((-sin(b1))*cos(a1)))*(sin(b2)))*(cos(b3))+(((cos(b1)))*((-sin(b2))*cos(a2))+(((-sin(b1))*cos(a1)))*(cos(b2)*cos(a2))+(((-sin(b1))*(-sin(a1))))*(sin(a2)))*(sin(b3)))*(cos(b4))+((((cos(b1)))*(cos(b2))+(((-sin(b1))*cos(a1)))*(sin(b2)))*((-sin(b3))*cos(a3))+(((cos(b1)))*((-sin(b2))*cos(a2))+(((-sin(b1))*cos(a1)))*(cos(b2)*cos(a2))+(((-sin(b1))*(-sin(a1))))*(sin(a2)))*(cos(b3)*cos(a3))+(((cos(b1)))*((-sin(b2))*(-sin(a2)))+(((-sin(b1))*cos(a1)))*(cos(b2)*(-sin(a2)))+(((-sin(b1))*(-sin(a1))))*(cos(a2)))*(sin(a3)))*(sin(b4)))*(cos(b5))+(((((cos(b1)))*(cos(b2))+(((-sin(b1))*cos(a1)))*(sin(b2)))*(cos(b3))+(((cos(b1)))*((-sin(b2))*cos(a2))+(((-sin(b1))*cos(a1)))*(cos(b2)*cos(a2))+(((-sin(b1))*(-sin(a1))))*(sin(a2)))*(sin(b3)))*((-sin(b4))*cos(a4))+((((cos(b1)))*(cos(b2))+(((-sin(b1))*cos(a1)))*(sin(b2)))*((-sin(b3))*cos(a3))+(((cos(b1)))*((-sin(b2))*cos(a2))+(((-sin(b1))*cos(a1)))*(cos(b2)*cos(a2))+(((-sin(b1))*(-sin(a1))))*(sin(a2)))*(cos(b3)*cos(a3))+(((cos(b1)))*((-sin(b2))*(-sin(a2)))+(((-sin(b1))*cos(a1)))*(cos(b2)*(-sin(a2)))+(((-sin(b1))*(-sin(a1))))*(cos(a2)))*(sin(a3)))*(cos(b4)*cos(a4))+((((cos(b1)))*(cos(b2))+(((-sin(b1))*cos(a1)))*(sin(b2)))*((-sin(b3))*(-sin(a3)))+(((cos(b1)))*((-sin(b2))*cos(a2))+(((-sin(b1))*cos(a1)))*(cos(b2)*cos(a2))+(((-sin(b1))*(-sin(a1))))*(sin(a2)))*(cos(b3)*(-sin(a3)))+(((cos(b1)))*((-sin(b2))*(-sin(a2)))+(((-sin(b1))*cos(a1)))*(cos(b2)*(-sin(a2)))+(((-sin(b1))*(-sin(a1))))*(cos(a2)))*(cos(a3)))*(sin(a4)))*(sin(b5))) ;
    p5varDH01 = ((((((cos(b1)))*(cos(b2))+(((-sin(b1))*cos(a1)))*(sin(b2)))*(cos(b3))+(((cos(b1)))*((-sin(b2))*cos(a2))+(((-sin(b1))*cos(a1)))*(cos(b2)*cos(a2))+(((-sin(b1))*(-sin(a1))))*(sin(a2)))*(sin(b3)))*(cos(b4))+((((cos(b1)))*(cos(b2))+(((-sin(b1))*cos(a1)))*(sin(b2)))*((-sin(b3))*cos(a3))+(((cos(b1)))*((-sin(b2))*cos(a2))+(((-sin(b1))*cos(a1)))*(cos(b2)*cos(a2))+(((-sin(b1))*(-sin(a1))))*(sin(a2)))*(cos(b3)*cos(a3))+(((cos(b1)))*((-sin(b2))*(-sin(a2)))+(((-sin(b1))*cos(a1)))*(cos(b2)*(-sin(a2)))+(((-sin(b1))*(-sin(a1))))*(cos(a2)))*(sin(a3)))*(sin(b4)))*((-sin(b5))*cos(a5))+(((((cos(b1)))*(cos(b2))+(((-sin(b1))*cos(a1)))*(sin(b2)))*(cos(b3))+(((cos(b1)))*((-sin(b2))*cos(a2))+(((-sin(b1))*cos(a1)))*(cos(b2)*cos(a2))+(((-sin(b1))*(-sin(a1))))*(sin(a2)))*(sin(b3)))*((-sin(b4))*cos(a4))+((((cos(b1)))*(cos(b2))+(((-sin(b1))*cos(a1)))*(sin(b2)))*((-sin(b3))*cos(a3))+(((cos(b1)))*((-sin(b2))*cos(a2))+(((-sin(b1))*cos(a1)))*(cos(b2)*cos(a2))+(((-sin(b1))*(-sin(a1))))*(sin(a2)))*(cos(b3)*cos(a3))+(((cos(b1)))*((-sin(b2))*(-sin(a2)))+(((-sin(b1))*cos(a1)))*(cos(b2)*(-sin(a2)))+(((-sin(b1))*(-sin(a1))))*(cos(a2)))*(sin(a3)))*(cos(b4)*cos(a4))+((((cos(b1)))*(cos(b2))+(((-sin(b1))*cos(a1)))*(sin(b2)))*((-sin(b3))*(-sin(a3)))+(((cos(b1)))*((-sin(b2))*cos(a2))+(((-sin(b1))*cos(a1)))*(cos(b2)*cos(a2))+(((-sin(b1))*(-sin(a1))))*(sin(a2)))*(cos(b3)*(-sin(a3)))+(((cos(b1)))*((-sin(b2))*(-sin(a2)))+(((-sin(b1))*cos(a1)))*(cos(b2)*(-sin(a2)))+(((-sin(b1))*(-sin(a1))))*(cos(a2)))*(cos(a3)))*(sin(a4)))*(cos(b5)*cos(a5))+(((((cos(b1)))*(cos(b2))+(((-sin(b1))*cos(a1)))*(sin(b2)))*(cos(b3))+(((cos(b1)))*((-sin(b2))*cos(a2))+(((-sin(b1))*cos(a1)))*(cos(b2)*cos(a2))+(((-sin(b1))*(-sin(a1))))*(sin(a2)))*(sin(b3)))*((-sin(b4))*(-sin(a4)))+((((cos(b1)))*(cos(b2))+(((-sin(b1))*cos(a1)))*(sin(b2)))*((-sin(b3))*cos(a3))+(((cos(b1)))*((-sin(b2))*cos(a2))+(((-sin(b1))*cos(a1)))*(cos(b2)*cos(a2))+(((-sin(b1))*(-sin(a1))))*(sin(a2)))*(cos(b3)*cos(a3))+(((cos(b1)))*((-sin(b2))*(-sin(a2)))+(((-sin(b1))*cos(a1)))*(cos(b2)*(-sin(a2)))+(((-sin(b1))*(-sin(a1))))*(cos(a2)))*(sin(a3)))*(cos(b4)*(-sin(a4)))+((((cos(b1)))*(cos(b2))+(((-sin(b1))*cos(a1)))*(sin(b2)))*((-sin(b3))*(-sin(a3)))+(((cos(b1)))*((-sin(b2))*cos(a2))+(((-sin(b1))*cos(a1)))*(cos(b2)*cos(a2))+(((-sin(b1))*(-sin(a1))))*(sin(a2)))*(cos(b3)*(-sin(a3)))+(((cos(b1)))*((-sin(b2))*(-sin(a2)))+(((-sin(b1))*cos(a1)))*(cos(b2)*(-sin(a2)))+(((-sin(b1))*(-sin(a1))))*(cos(a2)))*(cos(a3)))*(cos(a4)))*(sin(a5))) ;
    p5varDH02 = ((((((cos(b1)))*(cos(b2))+(((-sin(b1))*cos(a1)))*(sin(b2)))*(cos(b3))+(((cos(b1)))*((-sin(b2))*cos(a2))+(((-sin(b1))*cos(a1)))*(cos(b2)*cos(a2))+(((-sin(b1))*(-sin(a1))))*(sin(a2)))*(sin(b3)))*(cos(b4))+((((cos(b1)))*(cos(b2))+(((-sin(b1))*cos(a1)))*(sin(b2)))*((-sin(b3))*cos(a3))+(((cos(b1)))*((-sin(b2))*cos(a2))+(((-sin(b1))*cos(a1)))*(cos(b2)*cos(a2))+(((-sin(b1))*(-sin(a1))))*(sin(a2)))*(cos(b3)*cos(a3))+(((cos(b1)))*((-sin(b2))*(-sin(a2)))+(((-sin(b1))*cos(a1)))*(cos(b2)*(-sin(a2)))+(((-sin(b1))*(-sin(a1))))*(cos(a2)))*(sin(a3)))*(sin(b4)))*((-sin(b5))*(-sin(a5)))+(((((cos(b1)))*(cos(b2))+(((-sin(b1))*cos(a1)))*(sin(b2)))*(cos(b3))+(((cos(b1)))*((-sin(b2))*cos(a2))+(((-sin(b1))*cos(a1)))*(cos(b2)*cos(a2))+(((-sin(b1))*(-sin(a1))))*(sin(a2)))*(sin(b3)))*((-sin(b4))*cos(a4))+((((cos(b1)))*(cos(b2))+(((-sin(b1))*cos(a1)))*(sin(b2)))*((-sin(b3))*cos(a3))+(((cos(b1)))*((-sin(b2))*cos(a2))+(((-sin(b1))*cos(a1)))*(cos(b2)*cos(a2))+(((-sin(b1))*(-sin(a1))))*(sin(a2)))*(cos(b3)*cos(a3))+(((cos(b1)))*((-sin(b2))*(-sin(a2)))+(((-sin(b1))*cos(a1)))*(cos(b2)*(-sin(a2)))+(((-sin(b1))*(-sin(a1))))*(cos(a2)))*(sin(a3)))*(cos(b4)*cos(a4))+((((cos(b1)))*(cos(b2))+(((-sin(b1))*cos(a1)))*(sin(b2)))*((-sin(b3))*(-sin(a3)))+(((cos(b1)))*((-sin(b2))*cos(a2))+(((-sin(b1))*cos(a1)))*(cos(b2)*cos(a2))+(((-sin(b1))*(-sin(a1))))*(sin(a2)))*(cos(b3)*(-sin(a3)))+(((cos(b1)))*((-sin(b2))*(-sin(a2)))+(((-sin(b1))*cos(a1)))*(cos(b2)*(-sin(a2)))+(((-sin(b1))*(-sin(a1))))*(cos(a2)))*(cos(a3)))*(sin(a4)))*(cos(b5)*(-sin(a5)))+(((((cos(b1)))*(cos(b2))+(((-sin(b1))*cos(a1)))*(sin(b2)))*(cos(b3))+(((cos(b1)))*((-sin(b2))*cos(a2))+(((-sin(b1))*cos(a1)))*(cos(b2)*cos(a2))+(((-sin(b1))*(-sin(a1))))*(sin(a2)))*(sin(b3)))*((-sin(b4))*(-sin(a4)))+((((cos(b1)))*(cos(b2))+(((-sin(b1))*cos(a1)))*(sin(b2)))*((-sin(b3))*cos(a3))+(((cos(b1)))*((-sin(b2))*cos(a2))+(((-sin(b1))*cos(a1)))*(cos(b2)*cos(a2))+(((-sin(b1))*(-sin(a1))))*(sin(a2)))*(cos(b3)*cos(a3))+(((cos(b1)))*((-sin(b2))*(-sin(a2)))+(((-sin(b1))*cos(a1)))*(cos(b2)*(-sin(a2)))+(((-sin(b1))*(-sin(a1))))*(cos(a2)))*(sin(a3)))*(cos(b4)*(-sin(a4)))+((((cos(b1)))*(cos(b2))+(((-sin(b1))*cos(a1)))*(sin(b2)))*((-sin(b3))*(-sin(a3)))+(((cos(b1)))*((-sin(b2))*cos(a2))+(((-sin(b1))*cos(a1)))*(cos(b2)*cos(a2))+(((-sin(b1))*(-sin(a1))))*(sin(a2)))*(cos(b3)*(-sin(a3)))+(((cos(b1)))*((-sin(b2))*(-sin(a2)))+(((-sin(b1))*cos(a1)))*(cos(b2)*(-sin(a2)))+(((-sin(b1))*(-sin(a1))))*(cos(a2)))*(cos(a3)))*(cos(a4)))*(cos(a5))) ; 
    p5varDH03 = ((((((cos(b1)))*(cos(b2))+(((-sin(b1))*cos(a1)))*(sin(b2)))*(cos(b3))+(((cos(b1)))*((-sin(b2))*cos(a2))+(((-sin(b1))*cos(a1)))*(cos(b2)*cos(a2))+(((-sin(b1))*(-sin(a1))))*(sin(a2)))*(sin(b3)))*(cos(b4))+((((cos(b1)))*(cos(b2))+(((-sin(b1))*cos(a1)))*(sin(b2)))*((-sin(b3))*cos(a3))+(((cos(b1)))*((-sin(b2))*cos(a2))+(((-sin(b1))*cos(a1)))*(cos(b2)*cos(a2))+(((-sin(b1))*(-sin(a1))))*(sin(a2)))*(cos(b3)*cos(a3))+(((cos(b1)))*((-sin(b2))*(-sin(a2)))+(((-sin(b1))*cos(a1)))*(cos(b2)*(-sin(a2)))+(((-sin(b1))*(-sin(a1))))*(cos(a2)))*(sin(a3)))*(sin(b4)))*(cos(b5)*r5)+(((((cos(b1)))*(cos(b2))+(((-sin(b1))*cos(a1)))*(sin(b2)))*(cos(b3))+(((cos(b1)))*((-sin(b2))*cos(a2))+(((-sin(b1))*cos(a1)))*(cos(b2)*cos(a2))+(((-sin(b1))*(-sin(a1))))*(sin(a2)))*(sin(b3)))*((-sin(b4))*cos(a4))+((((cos(b1)))*(cos(b2))+(((-sin(b1))*cos(a1)))*(sin(b2)))*((-sin(b3))*cos(a3))+(((cos(b1)))*((-sin(b2))*cos(a2))+(((-sin(b1))*cos(a1)))*(cos(b2)*cos(a2))+(((-sin(b1))*(-sin(a1))))*(sin(a2)))*(cos(b3)*cos(a3))+(((cos(b1)))*((-sin(b2))*(-sin(a2)))+(((-sin(b1))*cos(a1)))*(cos(b2)*(-sin(a2)))+(((-sin(b1))*(-sin(a1))))*(cos(a2)))*(sin(a3)))*(cos(b4)*cos(a4))+((((cos(b1)))*(cos(b2))+(((-sin(b1))*cos(a1)))*(sin(b2)))*((-sin(b3))*(-sin(a3)))+(((cos(b1)))*((-sin(b2))*cos(a2))+(((-sin(b1))*cos(a1)))*(cos(b2)*cos(a2))+(((-sin(b1))*(-sin(a1))))*(sin(a2)))*(cos(b3)*(-sin(a3)))+(((cos(b1)))*((-sin(b2))*(-sin(a2)))+(((-sin(b1))*cos(a1)))*(cos(b2)*(-sin(a2)))+(((-sin(b1))*(-sin(a1))))*(cos(a2)))*(cos(a3)))*(sin(a4)))*(sin(b5)*r5)+(((((cos(b1)))*(cos(b2))+(((-sin(b1))*cos(a1)))*(sin(b2)))*(cos(b3))+(((cos(b1)))*((-sin(b2))*cos(a2))+(((-sin(b1))*cos(a1)))*(cos(b2)*cos(a2))+(((-sin(b1))*(-sin(a1))))*(sin(a2)))*(sin(b3)))*((-sin(b4))*(-sin(a4)))+((((cos(b1)))*(cos(b2))+(((-sin(b1))*cos(a1)))*(sin(b2)))*((-sin(b3))*cos(a3))+(((cos(b1)))*((-sin(b2))*cos(a2))+(((-sin(b1))*cos(a1)))*(cos(b2)*cos(a2))+(((-sin(b1))*(-sin(a1))))*(sin(a2)))*(cos(b3)*cos(a3))+(((cos(b1)))*((-sin(b2))*(-sin(a2)))+(((-sin(b1))*cos(a1)))*(cos(b2)*(-sin(a2)))+(((-sin(b1))*(-sin(a1))))*(cos(a2)))*(sin(a3)))*(cos(b4)*(-sin(a4)))+((((cos(b1)))*(cos(b2))+(((-sin(b1))*cos(a1)))*(sin(b2)))*((-sin(b3))*(-sin(a3)))+(((cos(b1)))*((-sin(b2))*cos(a2))+(((-sin(b1))*cos(a1)))*(cos(b2)*cos(a2))+(((-sin(b1))*(-sin(a1))))*(sin(a2)))*(cos(b3)*(-sin(a3)))+(((cos(b1)))*((-sin(b2))*(-sin(a2)))+(((-sin(b1))*cos(a1)))*(cos(b2)*(-sin(a2)))+(((-sin(b1))*(-sin(a1))))*(cos(a2)))*(cos(a3)))*(cos(a4)))*(d5)+(((((cos(b1)))*(cos(b2))+(((-sin(b1))*cos(a1)))*(sin(b2)))*(cos(b3))+(((cos(b1)))*((-sin(b2))*cos(a2))+(((-sin(b1))*cos(a1)))*(cos(b2)*cos(a2))+(((-sin(b1))*(-sin(a1))))*(sin(a2)))*(sin(b3)))*(cos(b4)*r4)+((((cos(b1)))*(cos(b2))+(((-sin(b1))*cos(a1)))*(sin(b2)))*((-sin(b3))*cos(a3))+(((cos(b1)))*((-sin(b2))*cos(a2))+(((-sin(b1))*cos(a1)))*(cos(b2)*cos(a2))+(((-sin(b1))*(-sin(a1))))*(sin(a2)))*(cos(b3)*cos(a3))+(((cos(b1)))*((-sin(b2))*(-sin(a2)))+(((-sin(b1))*cos(a1)))*(cos(b2)*(-sin(a2)))+(((-sin(b1))*(-sin(a1))))*(cos(a2)))*(sin(a3)))*(sin(b4)*r4)+((((cos(b1)))*(cos(b2))+(((-sin(b1))*cos(a1)))*(sin(b2)))*((-sin(b3))*(-sin(a3)))+(((cos(b1)))*((-sin(b2))*cos(a2))+(((-sin(b1))*cos(a1)))*(cos(b2)*cos(a2))+(((-sin(b1))*(-sin(a1))))*(sin(a2)))*(cos(b3)*(-sin(a3)))+(((cos(b1)))*((-sin(b2))*(-sin(a2)))+(((-sin(b1))*cos(a1)))*(cos(b2)*(-sin(a2)))+(((-sin(b1))*(-sin(a1))))*(cos(a2)))*(cos(a3)))*(d4)+((((cos(b1)))*(cos(b2))+(((-sin(b1))*cos(a1)))*(sin(b2)))*(cos(b3)*r3)+(((cos(b1)))*((-sin(b2))*cos(a2))+(((-sin(b1))*cos(a1)))*(cos(b2)*cos(a2))+(((-sin(b1))*(-sin(a1))))*(sin(a2)))*(sin(b3)*r3)+(((cos(b1)))*((-sin(b2))*(-sin(a2)))+(((-sin(b1))*cos(a1)))*(cos(b2)*(-sin(a2)))+(((-sin(b1))*(-sin(a1))))*(cos(a2)))*(d3)+(((cos(b1)))*(cos(b2)*r2)+(((-sin(b1))*cos(a1)))*(sin(b2)*r2)+(((-sin(b1))*(-sin(a1))))*(d2)+((cos(b1)*r1))))));
    p5varDH10 = ((((((sin(b1)))*(cos(b2))+((cos(b1)*cos(a1)))*(sin(b2)))*(cos(b3))+(((sin(b1)))*((-sin(b2))*cos(a2))+((cos(b1)*cos(a1)))*(cos(b2)*cos(a2))+((cos(b1)*(-sin(a1))))*(sin(a2)))*(sin(b3)))*(cos(b4))+((((sin(b1)))*(cos(b2))+((cos(b1)*cos(a1)))*(sin(b2)))*((-sin(b3))*cos(a3))+(((sin(b1)))*((-sin(b2))*cos(a2))+((cos(b1)*cos(a1)))*(cos(b2)*cos(a2))+((cos(b1)*(-sin(a1))))*(sin(a2)))*(cos(b3)*cos(a3))+(((sin(b1)))*((-sin(b2))*(-sin(a2)))+((cos(b1)*cos(a1)))*(cos(b2)*(-sin(a2)))+((cos(b1)*(-sin(a1))))*(cos(a2)))*(sin(a3)))*(sin(b4)))*(cos(b5))+(((((sin(b1)))*(cos(b2))+((cos(b1)*cos(a1)))*(sin(b2)))*(cos(b3))+(((sin(b1)))*((-sin(b2))*cos(a2))+((cos(b1)*cos(a1)))*(cos(b2)*cos(a2))+((cos(b1)*(-sin(a1))))*(sin(a2)))*(sin(b3)))*((-sin(b4))*cos(a4))+((((sin(b1)))*(cos(b2))+((cos(b1)*cos(a1)))*(sin(b2)))*((-sin(b3))*cos(a3))+(((sin(b1)))*((-sin(b2))*cos(a2))+((cos(b1)*cos(a1)))*(cos(b2)*cos(a2))+((cos(b1)*(-sin(a1))))*(sin(a2)))*(cos(b3)*cos(a3))+(((sin(b1)))*((-sin(b2))*(-sin(a2)))+((cos(b1)*cos(a1)))*(cos(b2)*(-sin(a2)))+((cos(b1)*(-sin(a1))))*(cos(a2)))*(sin(a3)))*(cos(b4)*cos(a4))+((((sin(b1)))*(cos(b2))+((cos(b1)*cos(a1)))*(sin(b2)))*((-sin(b3))*(-sin(a3)))+(((sin(b1)))*((-sin(b2))*cos(a2))+((cos(b1)*cos(a1)))*(cos(b2)*cos(a2))+((cos(b1)*(-sin(a1))))*(sin(a2)))*(cos(b3)*(-sin(a3)))+(((sin(b1)))*((-sin(b2))*(-sin(a2)))+((cos(b1)*cos(a1)))*(cos(b2)*(-sin(a2)))+((cos(b1)*(-sin(a1))))*(cos(a2)))*(cos(a3)))*(sin(a4)))*(sin(b5))) ;
    p5varDH11 = ((((((sin(b1)))*(cos(b2))+((cos(b1)*cos(a1)))*(sin(b2)))*(cos(b3))+(((sin(b1)))*((-sin(b2))*cos(a2))+((cos(b1)*cos(a1)))*(cos(b2)*cos(a2))+((cos(b1)*(-sin(a1))))*(sin(a2)))*(sin(b3)))*(cos(b4))+((((sin(b1)))*(cos(b2))+((cos(b1)*cos(a1)))*(sin(b2)))*((-sin(b3))*cos(a3))+(((sin(b1)))*((-sin(b2))*cos(a2))+((cos(b1)*cos(a1)))*(cos(b2)*cos(a2))+((cos(b1)*(-sin(a1))))*(sin(a2)))*(cos(b3)*cos(a3))+(((sin(b1)))*((-sin(b2))*(-sin(a2)))+((cos(b1)*cos(a1)))*(cos(b2)*(-sin(a2)))+((cos(b1)*(-sin(a1))))*(cos(a2)))*(sin(a3)))*(sin(b4)))*((-sin(b5))*cos(a5))+(((((sin(b1)))*(cos(b2))+((cos(b1)*cos(a1)))*(sin(b2)))*(cos(b3))+(((sin(b1)))*((-sin(b2))*cos(a2))+((cos(b1)*cos(a1)))*(cos(b2)*cos(a2))+((cos(b1)*(-sin(a1))))*(sin(a2)))*(sin(b3)))*((-sin(b4))*cos(a4))+((((sin(b1)))*(cos(b2))+((cos(b1)*cos(a1)))*(sin(b2)))*((-sin(b3))*cos(a3))+(((sin(b1)))*((-sin(b2))*cos(a2))+((cos(b1)*cos(a1)))*(cos(b2)*cos(a2))+((cos(b1)*(-sin(a1))))*(sin(a2)))*(cos(b3)*cos(a3))+(((sin(b1)))*((-sin(b2))*(-sin(a2)))+((cos(b1)*cos(a1)))*(cos(b2)*(-sin(a2)))+((cos(b1)*(-sin(a1))))*(cos(a2)))*(sin(a3)))*(cos(b4)*cos(a4))+((((sin(b1)))*(cos(b2))+((cos(b1)*cos(a1)))*(sin(b2)))*((-sin(b3))*(-sin(a3)))+(((sin(b1)))*((-sin(b2))*cos(a2))+((cos(b1)*cos(a1)))*(cos(b2)*cos(a2))+((cos(b1)*(-sin(a1))))*(sin(a2)))*(cos(b3)*(-sin(a3)))+(((sin(b1)))*((-sin(b2))*(-sin(a2)))+((cos(b1)*cos(a1)))*(cos(b2)*(-sin(a2)))+((cos(b1)*(-sin(a1))))*(cos(a2)))*(cos(a3)))*(sin(a4)))*(cos(b5)*cos(a5))+(((((sin(b1)))*(cos(b2))+((cos(b1)*cos(a1)))*(sin(b2)))*(cos(b3))+(((sin(b1)))*((-sin(b2))*cos(a2))+((cos(b1)*cos(a1)))*(cos(b2)*cos(a2))+((cos(b1)*(-sin(a1))))*(sin(a2)))*(sin(b3)))*((-sin(b4))*(-sin(a4)))+((((sin(b1)))*(cos(b2))+((cos(b1)*cos(a1)))*(sin(b2)))*((-sin(b3))*cos(a3))+(((sin(b1)))*((-sin(b2))*cos(a2))+((cos(b1)*cos(a1)))*(cos(b2)*cos(a2))+((cos(b1)*(-sin(a1))))*(sin(a2)))*(cos(b3)*cos(a3))+(((sin(b1)))*((-sin(b2))*(-sin(a2)))+((cos(b1)*cos(a1)))*(cos(b2)*(-sin(a2)))+((cos(b1)*(-sin(a1))))*(cos(a2)))*(sin(a3)))*(cos(b4)*(-sin(a4)))+((((sin(b1)))*(cos(b2))+((cos(b1)*cos(a1)))*(sin(b2)))*((-sin(b3))*(-sin(a3)))+(((sin(b1)))*((-sin(b2))*cos(a2))+((cos(b1)*cos(a1)))*(cos(b2)*cos(a2))+((cos(b1)*(-sin(a1))))*(sin(a2)))*(cos(b3)*(-sin(a3)))+(((sin(b1)))*((-sin(b2))*(-sin(a2)))+((cos(b1)*cos(a1)))*(cos(b2)*(-sin(a2)))+((cos(b1)*(-sin(a1))))*(cos(a2)))*(cos(a3)))*(cos(a4)))*(sin(a5))) ;
    p5varDH12 = ((((((sin(b1)))*(cos(b2))+((cos(b1)*cos(a1)))*(sin(b2)))*(cos(b3))+(((sin(b1)))*((-sin(b2))*cos(a2))+((cos(b1)*cos(a1)))*(cos(b2)*cos(a2))+((cos(b1)*(-sin(a1))))*(sin(a2)))*(sin(b3)))*(cos(b4))+((((sin(b1)))*(cos(b2))+((cos(b1)*cos(a1)))*(sin(b2)))*((-sin(b3))*cos(a3))+(((sin(b1)))*((-sin(b2))*cos(a2))+((cos(b1)*cos(a1)))*(cos(b2)*cos(a2))+((cos(b1)*(-sin(a1))))*(sin(a2)))*(cos(b3)*cos(a3))+(((sin(b1)))*((-sin(b2))*(-sin(a2)))+((cos(b1)*cos(a1)))*(cos(b2)*(-sin(a2)))+((cos(b1)*(-sin(a1))))*(cos(a2)))*(sin(a3)))*(sin(b4)))*((-sin(b5))*(-sin(a5)))+(((((sin(b1)))*(cos(b2))+((cos(b1)*cos(a1)))*(sin(b2)))*(cos(b3))+(((sin(b1)))*((-sin(b2))*cos(a2))+((cos(b1)*cos(a1)))*(cos(b2)*cos(a2))+((cos(b1)*(-sin(a1))))*(sin(a2)))*(sin(b3)))*((-sin(b4))*cos(a4))+((((sin(b1)))*(cos(b2))+((cos(b1)*cos(a1)))*(sin(b2)))*((-sin(b3))*cos(a3))+(((sin(b1)))*((-sin(b2))*cos(a2))+((cos(b1)*cos(a1)))*(cos(b2)*cos(a2))+((cos(b1)*(-sin(a1))))*(sin(a2)))*(cos(b3)*cos(a3))+(((sin(b1)))*((-sin(b2))*(-sin(a2)))+((cos(b1)*cos(a1)))*(cos(b2)*(-sin(a2)))+((cos(b1)*(-sin(a1))))*(cos(a2)))*(sin(a3)))*(cos(b4)*cos(a4))+((((sin(b1)))*(cos(b2))+((cos(b1)*cos(a1)))*(sin(b2)))*((-sin(b3))*(-sin(a3)))+(((sin(b1)))*((-sin(b2))*cos(a2))+((cos(b1)*cos(a1)))*(cos(b2)*cos(a2))+((cos(b1)*(-sin(a1))))*(sin(a2)))*(cos(b3)*(-sin(a3)))+(((sin(b1)))*((-sin(b2))*(-sin(a2)))+((cos(b1)*cos(a1)))*(cos(b2)*(-sin(a2)))+((cos(b1)*(-sin(a1))))*(cos(a2)))*(cos(a3)))*(sin(a4)))*(cos(b5)*(-sin(a5)))+(((((sin(b1)))*(cos(b2))+((cos(b1)*cos(a1)))*(sin(b2)))*(cos(b3))+(((sin(b1)))*((-sin(b2))*cos(a2))+((cos(b1)*cos(a1)))*(cos(b2)*cos(a2))+((cos(b1)*(-sin(a1))))*(sin(a2)))*(sin(b3)))*((-sin(b4))*(-sin(a4)))+((((sin(b1)))*(cos(b2))+((cos(b1)*cos(a1)))*(sin(b2)))*((-sin(b3))*cos(a3))+(((sin(b1)))*((-sin(b2))*cos(a2))+((cos(b1)*cos(a1)))*(cos(b2)*cos(a2))+((cos(b1)*(-sin(a1))))*(sin(a2)))*(cos(b3)*cos(a3))+(((sin(b1)))*((-sin(b2))*(-sin(a2)))+((cos(b1)*cos(a1)))*(cos(b2)*(-sin(a2)))+((cos(b1)*(-sin(a1))))*(cos(a2)))*(sin(a3)))*(cos(b4)*(-sin(a4)))+((((sin(b1)))*(cos(b2))+((cos(b1)*cos(a1)))*(sin(b2)))*((-sin(b3))*(-sin(a3)))+(((sin(b1)))*((-sin(b2))*cos(a2))+((cos(b1)*cos(a1)))*(cos(b2)*cos(a2))+((cos(b1)*(-sin(a1))))*(sin(a2)))*(cos(b3)*(-sin(a3)))+(((sin(b1)))*((-sin(b2))*(-sin(a2)))+((cos(b1)*cos(a1)))*(cos(b2)*(-sin(a2)))+((cos(b1)*(-sin(a1))))*(cos(a2)))*(cos(a3)))*(cos(a4)))*(cos(a5))) ;
    p5varDH13 = ((((((sin(b1)))*(cos(b2))+((cos(b1)*cos(a1)))*(sin(b2)))*(cos(b3))+(((sin(b1)))*((-sin(b2))*cos(a2))+((cos(b1)*cos(a1)))*(cos(b2)*cos(a2))+((cos(b1)*(-sin(a1))))*(sin(a2)))*(sin(b3)))*(cos(b4))+((((sin(b1)))*(cos(b2))+((cos(b1)*cos(a1)))*(sin(b2)))*((-sin(b3))*cos(a3))+(((sin(b1)))*((-sin(b2))*cos(a2))+((cos(b1)*cos(a1)))*(cos(b2)*cos(a2))+((cos(b1)*(-sin(a1))))*(sin(a2)))*(cos(b3)*cos(a3))+(((sin(b1)))*((-sin(b2))*(-sin(a2)))+((cos(b1)*cos(a1)))*(cos(b2)*(-sin(a2)))+((cos(b1)*(-sin(a1))))*(cos(a2)))*(sin(a3)))*(sin(b4)))*(cos(b5)*r5)+(((((sin(b1)))*(cos(b2))+((cos(b1)*cos(a1)))*(sin(b2)))*(cos(b3))+(((sin(b1)))*((-sin(b2))*cos(a2))+((cos(b1)*cos(a1)))*(cos(b2)*cos(a2))+((cos(b1)*(-sin(a1))))*(sin(a2)))*(sin(b3)))*((-sin(b4))*cos(a4))+((((sin(b1)))*(cos(b2))+((cos(b1)*cos(a1)))*(sin(b2)))*((-sin(b3))*cos(a3))+(((sin(b1)))*((-sin(b2))*cos(a2))+((cos(b1)*cos(a1)))*(cos(b2)*cos(a2))+((cos(b1)*(-sin(a1))))*(sin(a2)))*(cos(b3)*cos(a3))+(((sin(b1)))*((-sin(b2))*(-sin(a2)))+((cos(b1)*cos(a1)))*(cos(b2)*(-sin(a2)))+((cos(b1)*(-sin(a1))))*(cos(a2)))*(sin(a3)))*(cos(b4)*cos(a4))+((((sin(b1)))*(cos(b2))+((cos(b1)*cos(a1)))*(sin(b2)))*((-sin(b3))*(-sin(a3)))+(((sin(b1)))*((-sin(b2))*cos(a2))+((cos(b1)*cos(a1)))*(cos(b2)*cos(a2))+((cos(b1)*(-sin(a1))))*(sin(a2)))*(cos(b3)*(-sin(a3)))+(((sin(b1)))*((-sin(b2))*(-sin(a2)))+((cos(b1)*cos(a1)))*(cos(b2)*(-sin(a2)))+((cos(b1)*(-sin(a1))))*(cos(a2)))*(cos(a3)))*(sin(a4)))*(sin(b5)*r5)+(((((sin(b1)))*(cos(b2))+((cos(b1)*cos(a1)))*(sin(b2)))*(cos(b3))+(((sin(b1)))*((-sin(b2))*cos(a2))+((cos(b1)*cos(a1)))*(cos(b2)*cos(a2))+((cos(b1)*(-sin(a1))))*(sin(a2)))*(sin(b3)))*((-sin(b4))*(-sin(a4)))+((((sin(b1)))*(cos(b2))+((cos(b1)*cos(a1)))*(sin(b2)))*((-sin(b3))*cos(a3))+(((sin(b1)))*((-sin(b2))*cos(a2))+((cos(b1)*cos(a1)))*(cos(b2)*cos(a2))+((cos(b1)*(-sin(a1))))*(sin(a2)))*(cos(b3)*cos(a3))+(((sin(b1)))*((-sin(b2))*(-sin(a2)))+((cos(b1)*cos(a1)))*(cos(b2)*(-sin(a2)))+((cos(b1)*(-sin(a1))))*(cos(a2)))*(sin(a3)))*(cos(b4)*(-sin(a4)))+((((sin(b1)))*(cos(b2))+((cos(b1)*cos(a1)))*(sin(b2)))*((-sin(b3))*(-sin(a3)))+(((sin(b1)))*((-sin(b2))*cos(a2))+((cos(b1)*cos(a1)))*(cos(b2)*cos(a2))+((cos(b1)*(-sin(a1))))*(sin(a2)))*(cos(b3)*(-sin(a3)))+(((sin(b1)))*((-sin(b2))*(-sin(a2)))+((cos(b1)*cos(a1)))*(cos(b2)*(-sin(a2)))+((cos(b1)*(-sin(a1))))*(cos(a2)))*(cos(a3)))*(cos(a4)))*(d5)+(((((sin(b1)))*(cos(b2))+((cos(b1)*cos(a1)))*(sin(b2)))*(cos(b3))+(((sin(b1)))*((-sin(b2))*cos(a2))+((cos(b1)*cos(a1)))*(cos(b2)*cos(a2))+((cos(b1)*(-sin(a1))))*(sin(a2)))*(sin(b3)))*(cos(b4)*r4)+((((sin(b1)))*(cos(b2))+((cos(b1)*cos(a1)))*(sin(b2)))*((-sin(b3))*cos(a3))+(((sin(b1)))*((-sin(b2))*cos(a2))+((cos(b1)*cos(a1)))*(cos(b2)*cos(a2))+((cos(b1)*(-sin(a1))))*(sin(a2)))*(cos(b3)*cos(a3))+(((sin(b1)))*((-sin(b2))*(-sin(a2)))+((cos(b1)*cos(a1)))*(cos(b2)*(-sin(a2)))+((cos(b1)*(-sin(a1))))*(cos(a2)))*(sin(a3)))*(sin(b4)*r4)+((((sin(b1)))*(cos(b2))+((cos(b1)*cos(a1)))*(sin(b2)))*((-sin(b3))*(-sin(a3)))+(((sin(b1)))*((-sin(b2))*cos(a2))+((cos(b1)*cos(a1)))*(cos(b2)*cos(a2))+((cos(b1)*(-sin(a1))))*(sin(a2)))*(cos(b3)*(-sin(a3)))+(((sin(b1)))*((-sin(b2))*(-sin(a2)))+((cos(b1)*cos(a1)))*(cos(b2)*(-sin(a2)))+((cos(b1)*(-sin(a1))))*(cos(a2)))*(cos(a3)))*(d4)+((((sin(b1)))*(cos(b2))+((cos(b1)*cos(a1)))*(sin(b2)))*(cos(b3)*r3)+(((sin(b1)))*((-sin(b2))*cos(a2))+((cos(b1)*cos(a1)))*(cos(b2)*cos(a2))+((cos(b1)*(-sin(a1))))*(sin(a2)))*(sin(b3)*r3)+(((sin(b1)))*((-sin(b2))*(-sin(a2)))+((cos(b1)*cos(a1)))*(cos(b2)*(-sin(a2)))+((cos(b1)*(-sin(a1))))*(cos(a2)))*(d3)+(((sin(b1)))*(cos(b2)*r2)+((cos(b1)*cos(a1)))*(sin(b2)*r2)+((cos(b1)*(-sin(a1))))*(d2)+((sin(b1)*r1))))));
    p5varDH20 = ((((((sin(a1)))*(sin(b2)))*(cos(b3))+(((sin(a1)))*(cos(b2)*cos(a2))+((cos(a1)))*(sin(a2)))*(sin(b3)))*(cos(b4))+((((sin(a1)))*(sin(b2)))*((-sin(b3))*cos(a3))+(((sin(a1)))*(cos(b2)*cos(a2))+((cos(a1)))*(sin(a2)))*(cos(b3)*cos(a3))+(((sin(a1)))*(cos(b2)*(-sin(a2)))+((cos(a1)))*(cos(a2)))*(sin(a3)))*(sin(b4)))*(cos(b5))+(((((sin(a1)))*(sin(b2)))*(cos(b3))+(((sin(a1)))*(cos(b2)*cos(a2))+((cos(a1)))*(sin(a2)))*(sin(b3)))*((-sin(b4))*cos(a4))+((((sin(a1)))*(sin(b2)))*((-sin(b3))*cos(a3))+(((sin(a1)))*(cos(b2)*cos(a2))+((cos(a1)))*(sin(a2)))*(cos(b3)*cos(a3))+(((sin(a1)))*(cos(b2)*(-sin(a2)))+((cos(a1)))*(cos(a2)))*(sin(a3)))*(cos(b4)*cos(a4))+((((sin(a1)))*(sin(b2)))*((-sin(b3))*(-sin(a3)))+(((sin(a1)))*(cos(b2)*cos(a2))+((cos(a1)))*(sin(a2)))*(cos(b3)*(-sin(a3)))+(((sin(a1)))*(cos(b2)*(-sin(a2)))+((cos(a1)))*(cos(a2)))*(cos(a3)))*(sin(a4)))*(sin(b5))) ;
    p5varDH21 = ((((((sin(a1)))*(sin(b2)))*(cos(b3))+(((sin(a1)))*(cos(b2)*cos(a2))+((cos(a1)))*(sin(a2)))*(sin(b3)))*(cos(b4))+((((sin(a1)))*(sin(b2)))*((-sin(b3))*cos(a3))+(((sin(a1)))*(cos(b2)*cos(a2))+((cos(a1)))*(sin(a2)))*(cos(b3)*cos(a3))+(((sin(a1)))*(cos(b2)*(-sin(a2)))+((cos(a1)))*(cos(a2)))*(sin(a3)))*(sin(b4)))*((-sin(b5))*cos(a5))+(((((sin(a1)))*(sin(b2)))*(cos(b3))+(((sin(a1)))*(cos(b2)*cos(a2))+((cos(a1)))*(sin(a2)))*(sin(b3)))*((-sin(b4))*cos(a4))+((((sin(a1)))*(sin(b2)))*((-sin(b3))*cos(a3))+(((sin(a1)))*(cos(b2)*cos(a2))+((cos(a1)))*(sin(a2)))*(cos(b3)*cos(a3))+(((sin(a1)))*(cos(b2)*(-sin(a2)))+((cos(a1)))*(cos(a2)))*(sin(a3)))*(cos(b4)*cos(a4))+((((sin(a1)))*(sin(b2)))*((-sin(b3))*(-sin(a3)))+(((sin(a1)))*(cos(b2)*cos(a2))+((cos(a1)))*(sin(a2)))*(cos(b3)*(-sin(a3)))+(((sin(a1)))*(cos(b2)*(-sin(a2)))+((cos(a1)))*(cos(a2)))*(cos(a3)))*(sin(a4)))*(cos(b5)*cos(a5))+(((((sin(a1)))*(sin(b2)))*(cos(b3))+(((sin(a1)))*(cos(b2)*cos(a2))+((cos(a1)))*(sin(a2)))*(sin(b3)))*((-sin(b4))*(-sin(a4)))+((((sin(a1)))*(sin(b2)))*((-sin(b3))*cos(a3))+(((sin(a1)))*(cos(b2)*cos(a2))+((cos(a1)))*(sin(a2)))*(cos(b3)*cos(a3))+(((sin(a1)))*(cos(b2)*(-sin(a2)))+((cos(a1)))*(cos(a2)))*(sin(a3)))*(cos(b4)*(-sin(a4)))+((((sin(a1)))*(sin(b2)))*((-sin(b3))*(-sin(a3)))+(((sin(a1)))*(cos(b2)*cos(a2))+((cos(a1)))*(sin(a2)))*(cos(b3)*(-sin(a3)))+(((sin(a1)))*(cos(b2)*(-sin(a2)))+((cos(a1)))*(cos(a2)))*(cos(a3)))*(cos(a4)))*(sin(a5))) ;
    p5varDH22 = ((((((sin(a1)))*(sin(b2)))*(cos(b3))+(((sin(a1)))*(cos(b2)*cos(a2))+((cos(a1)))*(sin(a2)))*(sin(b3)))*(cos(b4))+((((sin(a1)))*(sin(b2)))*((-sin(b3))*cos(a3))+(((sin(a1)))*(cos(b2)*cos(a2))+((cos(a1)))*(sin(a2)))*(cos(b3)*cos(a3))+(((sin(a1)))*(cos(b2)*(-sin(a2)))+((cos(a1)))*(cos(a2)))*(sin(a3)))*(sin(b4)))*((-sin(b5))*(-sin(a5)))+(((((sin(a1)))*(sin(b2)))*(cos(b3))+(((sin(a1)))*(cos(b2)*cos(a2))+((cos(a1)))*(sin(a2)))*(sin(b3)))*((-sin(b4))*cos(a4))+((((sin(a1)))*(sin(b2)))*((-sin(b3))*cos(a3))+(((sin(a1)))*(cos(b2)*cos(a2))+((cos(a1)))*(sin(a2)))*(cos(b3)*cos(a3))+(((sin(a1)))*(cos(b2)*(-sin(a2)))+((cos(a1)))*(cos(a2)))*(sin(a3)))*(cos(b4)*cos(a4))+((((sin(a1)))*(sin(b2)))*((-sin(b3))*(-sin(a3)))+(((sin(a1)))*(cos(b2)*cos(a2))+((cos(a1)))*(sin(a2)))*(cos(b3)*(-sin(a3)))+(((sin(a1)))*(cos(b2)*(-sin(a2)))+((cos(a1)))*(cos(a2)))*(cos(a3)))*(sin(a4)))*(cos(b5)*(-sin(a5)))+(((((sin(a1)))*(sin(b2)))*(cos(b3))+(((sin(a1)))*(cos(b2)*cos(a2))+((cos(a1)))*(sin(a2)))*(sin(b3)))*((-sin(b4))*(-sin(a4)))+((((sin(a1)))*(sin(b2)))*((-sin(b3))*cos(a3))+(((sin(a1)))*(cos(b2)*cos(a2))+((cos(a1)))*(sin(a2)))*(cos(b3)*cos(a3))+(((sin(a1)))*(cos(b2)*(-sin(a2)))+((cos(a1)))*(cos(a2)))*(sin(a3)))*(cos(b4)*(-sin(a4)))+((((sin(a1)))*(sin(b2)))*((-sin(b3))*(-sin(a3)))+(((sin(a1)))*(cos(b2)*cos(a2))+((cos(a1)))*(sin(a2)))*(cos(b3)*(-sin(a3)))+(((sin(a1)))*(cos(b2)*(-sin(a2)))+((cos(a1)))*(cos(a2)))*(cos(a3)))*(cos(a4)))*(cos(a5))) ;
    p5varDH23 = ((((((sin(a1)))*(sin(b2)))*(cos(b3))+(((sin(a1)))*(cos(b2)*cos(a2))+((cos(a1)))*(sin(a2)))*(sin(b3)))*(cos(b4))+((((sin(a1)))*(sin(b2)))*((-sin(b3))*cos(a3))+(((sin(a1)))*(cos(b2)*cos(a2))+((cos(a1)))*(sin(a2)))*(cos(b3)*cos(a3))+(((sin(a1)))*(cos(b2)*(-sin(a2)))+((cos(a1)))*(cos(a2)))*(sin(a3)))*(sin(b4)))*(cos(b5)*r5)+(((((sin(a1)))*(sin(b2)))*(cos(b3))+(((sin(a1)))*(cos(b2)*cos(a2))+((cos(a1)))*(sin(a2)))*(sin(b3)))*((-sin(b4))*cos(a4))+((((sin(a1)))*(sin(b2)))*((-sin(b3))*cos(a3))+(((sin(a1)))*(cos(b2)*cos(a2))+((cos(a1)))*(sin(a2)))*(cos(b3)*cos(a3))+(((sin(a1)))*(cos(b2)*(-sin(a2)))+((cos(a1)))*(cos(a2)))*(sin(a3)))*(cos(b4)*cos(a4))+((((sin(a1)))*(sin(b2)))*((-sin(b3))*(-sin(a3)))+(((sin(a1)))*(cos(b2)*cos(a2))+((cos(a1)))*(sin(a2)))*(cos(b3)*(-sin(a3)))+(((sin(a1)))*(cos(b2)*(-sin(a2)))+((cos(a1)))*(cos(a2)))*(cos(a3)))*(sin(a4)))*(sin(b5)*r5)+(((((sin(a1)))*(sin(b2)))*(cos(b3))+(((sin(a1)))*(cos(b2)*cos(a2))+((cos(a1)))*(sin(a2)))*(sin(b3)))*((-sin(b4))*(-sin(a4)))+((((sin(a1)))*(sin(b2)))*((-sin(b3))*cos(a3))+(((sin(a1)))*(cos(b2)*cos(a2))+((cos(a1)))*(sin(a2)))*(cos(b3)*cos(a3))+(((sin(a1)))*(cos(b2)*(-sin(a2)))+((cos(a1)))*(cos(a2)))*(sin(a3)))*(cos(b4)*(-sin(a4)))+((((sin(a1)))*(sin(b2)))*((-sin(b3))*(-sin(a3)))+(((sin(a1)))*(cos(b2)*cos(a2))+((cos(a1)))*(sin(a2)))*(cos(b3)*(-sin(a3)))+(((sin(a1)))*(cos(b2)*(-sin(a2)))+((cos(a1)))*(cos(a2)))*(cos(a3)))*(cos(a4)))*(d5)+(((((sin(a1)))*(sin(b2)))*(cos(b3))+(((sin(a1)))*(cos(b2)*cos(a2))+((cos(a1)))*(sin(a2)))*(sin(b3)))*(cos(b4)*r4)+((((sin(a1)))*(sin(b2)))*((-sin(b3))*cos(a3))+(((sin(a1)))*(cos(b2)*cos(a2))+((cos(a1)))*(sin(a2)))*(cos(b3)*cos(a3))+(((sin(a1)))*(cos(b2)*(-sin(a2)))+((cos(a1)))*(cos(a2)))*(sin(a3)))*(sin(b4)*r4)+((((sin(a1)))*(sin(b2)))*((-sin(b3))*(-sin(a3)))+(((sin(a1)))*(cos(b2)*cos(a2))+((cos(a1)))*(sin(a2)))*(cos(b3)*(-sin(a3)))+(((sin(a1)))*(cos(b2)*(-sin(a2)))+((cos(a1)))*(cos(a2)))*(cos(a3)))*(d4)+((((sin(a1)))*(sin(b2)))*(cos(b3)*r3)+(((sin(a1)))*(cos(b2)*cos(a2))+((cos(a1)))*(sin(a2)))*(sin(b3)*r3)+(((sin(a1)))*(cos(b2)*(-sin(a2)))+((cos(a1)))*(cos(a2)))*(d3)+(((sin(a1)))*(sin(b2)*r2)+((cos(a1)))*(d2)+((d1))))));
*/ //      
// /*   
    // Se usan los elementos de la matriz T de DH.
    // Simplificadas - Sin cambiar por 90º en el primer motor (cadera)
    p5varDH00 = sin(b1)*sin(b5)+cos(b1)*cos(b5)*cos(b2+b3+b4);
    p5varDH01 = cos(b5)*sin(b1)-cos(b1)*sin(b5)*cos(b2+b3+b4);
    p5varDH02 = cos(b1)*sin(b2+b3+b4);
    p5varDH03 = r1*cos(b1)+r2*cos(b1)*cos(b2)+r5*sin(b1)*sin(b5)-r4*sin(b2+b3)*cos(b1)*sin(b4)+r3*cos(b1)*cos(b2)*cos(b3)-r3*cos(b1)*sin(b2)*sin(b3)+r5*cos(b1)*cos(b5)*cos(b2+b3+b4)+r4*cos(b2+b3)*cos(b1)*cos(b4);
    p5varDH10 = cos(b5)*sin(b1)*cos(b2+b3+b4)-cos(b1)*sin(b5);
    p5varDH11 = -cos(b1)*cos(b5)-sin(b1)*sin(b5)*cos(b2+b3+b4);
    p5varDH12 = sin(b1)*sin(b2+b3+b4);
    p5varDH13 = r1*sin(b1)+r2*cos(b2)*sin(b1)-r5*cos(b1)*sin(b5)-r4*sin(b2+b3)*sin(b1)*sin(b4)+r3*cos(b2)*cos(b3)*sin(b1)-r3*sin(b1)*sin(b2)*sin(b3)+r5*cos(b5)*sin(b1)*cos(b2+b3+b4)+r4*cos(b2+b3)*cos(b4)*sin(b1);
    p5varDH20 = cos(b5)*sin(b2+b3+b4);
    p5varDH21 = -sin(b5)*sin(b2+b3+b4);
    p5varDH22 = -cos(b2+b3+b4);
    p5varDH23 = r3*sin(b2+b3)+r2*sin(b2)+r4*cos(b2+b3)*sin(b4)+r4*sin(b2+b3)*cos(b4)+r5*cos(b5)*(cos(b2+b3)*sin(b4)+sin(b2+b3)*cos(b4));
// */   
    // Se usan los elementos de la matriz T de DH.
    ///* //Haciendo el cambio de cos:-sin y sin:cos en a1 por tener -90 grados en el motor 1. 
    p5varDH00_2 = cos(b1)*cos(b5)*cos(b2+b3+b4)-sin(b1)*sin(b5);
    p5varDH01_2 = -cos(b5)*sin(b1)-cos(b1)*sin(b5)*cos(b2+b3+b4);
    p5varDH02_2 = cos(b1)*sin(b2+b3+b4);
    p5varDH03_2 = r1*cos(b1)+r2*cos(b1)*cos(b2)-r5*sin(b1)*sin(b5)-r4*sin(b2+b3)*cos(b1)*sin(b4)+r3*cos(b1)*cos(b2)*cos(b3)-r3*cos(b1)*sin(b2)*sin(b3)+r5*cos(b1)*cos(b5)*cos(b2+b3+b4)+r4*cos(b2+b3)*cos(b1)*cos(b4);
    p5varDH10_2 = cos(b1)*sin(b5)+cos(b5)*sin(b1)*cos(b2+b3+b4);
    p5varDH11_2 = cos(b1)*cos(b5)-sin(b1)*sin(b5)*cos(b2+b3+b4);
    p5varDH12_2 = sin(b1)*sin(b2+b3+b4);
    p5varDH13_2 = r1*sin(b1)+r2*cos(b2)*sin(b1)+r5*cos(b1)*sin(b5)-r4*sin(b2+b3)*sin(b1)*sin(b4)+r3*cos(b2)*cos(b3)*sin(b1)-r3*sin(b1)*sin(b2)*sin(b3)+r5*cos(b5)*sin(b1)*cos(b2+b3+b4)+r4*cos(b2+b3)*cos(b4)*sin(b1);
    p5varDH20_2 = -cos(b5)*sin(b2+b3+b4);
    p5varDH21_2 = sin(b5)*sin(b2+b3+b4);
    p5varDH22_2 = cos(b2+b3+b4);
    p5varDH23_2 = -r3*sin(b2+b3)-r2*sin(b2)-r4*cos(b2+b3)*sin(b4)-r4*sin(b2+b3)*cos(b4)-r5*cos(b5)*(cos(b2+b3)*sin(b4)+sin(b2+b3)*cos(b4));
    //*/
  }
  
  double A_2 = 0;
  double B_2 = 0;
  double C_2 = 0;  
  if ((numart == 5) and ((imprimir0 == 0) or (imprimir0 == 1))) {
    Serial.print(F("Sin cambio de 90º en los parámetros de DH del motor 2 (DHbi[1] = 90)"));
    Serial.print(F("\np5varDH00 = ")); Serial.print(p5varDH00); Serial.print(F(" // p5varDH01 = ")); Serial.print(p5varDH01);
    Serial.print(F(" // p5varDH02 = ")); Serial.print(p5varDH02); Serial.print(F(" // p5varDH03 = ")); Serial.println(p5varDH03);
    Serial.print(F("p5varDH10 = ")); Serial.print(p5varDH10); Serial.print(F(" // p5varDH11 = ")); Serial.print(p5varDH11);
    Serial.print(F(" // p5varDH12 = ")); Serial.print(p5varDH12); Serial.print(F(" // p5varDH13 = ")); Serial.println(p5varDH13);
    Serial.print(F("p5varDH20 = ")); Serial.print(p5varDH20); Serial.print(F(" // p5varDH21 = ")); Serial.print(p5varDH21);
    Serial.print(F(" // p5varDH22 = ")); Serial.print(p5varDH22); Serial.print(F(" // p5varDH23 = ")); Serial.println(p5varDH23);

    // Ahora se calcula la posición (x, y, z) con referencia al sistema cartesiano base, usando como dato origen un punto con referencia al último sistema de coordenadas. 
    // Resulta interesante cuando se usa con Teta2=0 ya que con un punto como puntoF=(-100,-100,100) y puntoF9_2=(0,0,5) sólo varían las componentes x,y. 
    // Para ello se ejecutarán las siguientes ecuaciones del producto matricial:
    A_2 = puntoF_2[2]; // Se intercambian las X,Z por cambiar los ejes en el cálculo por los cambios de ejes en los ejes de coordenadas de Denavit-Hartenberg
    B_2 = puntoF_2[1];
    C_2 = puntoF_2[0];
    //A_2 = puntoF_2[0]; 
    //B_2 = puntoF_2[1];
    //C_2 = puntoF_2[2];
    puntoF[0] = p5varDH00 * A_2 + p5varDH01 * B_2 + p5varDH02 * C_2 + p5varDH03 * 1; 
    puntoF[1] = p5varDH10 * A_2 + p5varDH11 * B_2 + p5varDH12 * C_2 + p5varDH13 * 1; 
    puntoF[2] = p5varDH20 * A_2 + p5varDH21 * B_2 + p5varDH22 * C_2 + p5varDH23 * 1; 
    Serial.print(F("Puntox0Extremo = ")); Serial.print(p5varDH03); Serial.print(F(" // Puntoy0Extremo = ")); Serial.print(p5varDH13);
    Serial.print(F(" // Puntoz0Extremo = ")); Serial.println(p5varDH23); 
    Serial.print(F("PuntoxRelativo0Extremo = ")); Serial.print(puntoF[0]); Serial.print(F(" // PuntoyRelativo0Extremo = ")); Serial.print(puntoF[1]);
    Serial.print(F(" // PuntozRelativo0Extremo = ")); Serial.println(puntoF[2]); 
    
    if ((numart == 5) and (imprimir0 == 1)) {
      Serial.print(F("\nCon cambio de 90º en los parámetros de DH del motor 2 (DHbi[1] = 0)"));
      Serial.print(F("\np5varDH00_2 = ")); Serial.print(p5varDH00_2); Serial.print(F(" // p5varDH01_2 = ")); Serial.print(p5varDH01_2);
      Serial.print(F(" // p5varDH02_2 = ")); Serial.print(p5varDH02_2); Serial.print(F(" // p5varDH03_2 = ")); Serial.println(p5varDH03_2);
      Serial.print(F("p5varDH10_2 = ")); Serial.print(p5varDH10_2); Serial.print(F(" // p5varDH11_2 = ")); Serial.print(p5varDH11_2);
      Serial.print(F(" // p5varDH12_2 = ")); Serial.print(p5varDH12_2); Serial.print(F(" // p5varDH13_2 = ")); Serial.println(p5varDH13_2);
      Serial.print(F("p5varDH20_2 = ")); Serial.print(p5varDH20_2); Serial.print(F(" // p5varDH21_2 = ")); Serial.print(p5varDH21_2);
      Serial.print(F(" // p5varDH22_2 = ")); Serial.print(p5varDH22_2); Serial.print(F(" // p5varDH23_2 = ")); Serial.println(p5varDH23_2);

      // Ahora se calcula la posición (x, y, z) con referencia al sistema cartesiano base, usando como dato origen un punto con referencia al último sistema de coordenadas. 
      // Resulta interesante cuando se usa con Teta2=0 ya que con un punto como puntoF=(-100,-100,100) y puntoF9_2=(0,0,5) sólo varían las componentes x,y. 
      // Para ello se ejecutarán las siguientes ecuaciones del producto matricial:
      puntoF[0] = p5varDH00_2 * A_2 + p5varDH01_2 * B_2 + p5varDH02_2 * C_2 + p5varDH03_2 * 1; 
      puntoF[1] = p5varDH10_2 * A_2 + p5varDH11_2 * B_2 + p5varDH12_2 * C_2 + p5varDH13_2 * 1; 
      puntoF[2] = p5varDH20_2 * A_2 + p5varDH21_2 * B_2 + p5varDH22_2 * C_2 + p5varDH23_2 * 1; 
      Serial.print(F("Puntox0Extremo = ")); Serial.print(p5varDH03_2); Serial.print(F(" // Puntoy0Extremo = ")); Serial.print(p5varDH13_2);
      Serial.print(F(" // Puntoz0Extremo = ")); Serial.println(p5varDH23_2); 
      Serial.print(F("PuntoxRelativo0Extremo = ")); Serial.print(puntoF[0]); Serial.print(F(" // PuntoyRelativo0Extremo = ")); Serial.print(puntoF[1]);
      Serial.print(F(" // PuntozRelativo0Extremo = ")); Serial.println(puntoF[2]); 
    }
  }
  if ((numart == 3) and ((imprimir0 == 1) or (imprimir0 == 1))) {
    Serial.print(F("\nSin cambio de 90º en los parámetros de DH del motor 2 (DHbi[1] = 90)"));
    Serial.print(F("\nb3varDH00 = ")); Serial.print(b3varDH00); Serial.print(F(" // b3varDH01 = ")); Serial.print(b3varDH01);
    Serial.print(F(" // b3varDH02 = ")); Serial.print(b3varDH02); Serial.print(F(" // b3varDH03 = ")); Serial.println(b3varDH03);
    Serial.print(F("b3varDH10 = ")); Serial.print(b3varDH10); Serial.print(F(" // b3varDH11 = ")); Serial.print(b3varDH11);
    Serial.print(F(" // b3varDH12 = ")); Serial.print(b3varDH12); Serial.print(F(" // b3varDH13 = ")); Serial.println(b3varDH13);
    Serial.print(F("b3varDH20 = ")); Serial.print(b3varDH20); Serial.print(F(" // b3varDH21 = ")); Serial.print(b3varDH21);
    Serial.print(F(" // b3varDH22 = ")); Serial.print(b3varDH22); Serial.print(F(" // b3varDH23 = ")); Serial.println(b3varDH23);

    // Ahora se calcula la posición (x, y, z) con referencia al sistema cartesiano base, usando como dato origen un punto con referencia al último sistema de coordenadas. 
    // Resulta interesante cuando se usa con Teta2=0 ya que con un punto como puntoF=(-100,-100,100) y puntoF9_2=(0,0,5) sólo varían las componentes x,y. 
    // Para ello se ejecutarán las siguientes ecuaciones del producto matricial:
    A_2 = puntoF_2[2]; // Se intercambian las X,Z por cambiar los ejes en el cálculo por los cambios de ejes en los ejes de coordenadas de Denavit-Hartenberg
    B_2 = puntoF_2[1];
    C_2 = puntoF_2[0];
    puntoF[0] = b3varDH00 * A_2 + b3varDH01 * B_2 + b3varDH02 * C_2 + b3varDH03 * 1; 
    puntoF[1] = b3varDH10 * A_2 + b3varDH11 * B_2 + b3varDH12 * C_2 + b3varDH13 * 1; 
    puntoF[2] = b3varDH20 * A_2 + b3varDH21 * B_2 + b3varDH22 * C_2 + b3varDH23 * 1; 
    Serial.print(F("Puntox0Extremo = ")); Serial.print(b3varDH03); Serial.print(F(" // Puntoy0Extremo = ")); Serial.print(b3varDH13);
    Serial.print(F(" // Puntoz0Extremo = ")); Serial.println(b3varDH23); 
    Serial.print(F("PuntoxRelativo0Extremo = ")); Serial.print(puntoF[0]); Serial.print(F(" // PuntoyRelativo0Extremo = ")); Serial.print(puntoF[1]);
    Serial.print(F(" // PuntozRelativo0Extremo = ")); Serial.println(puntoF[2]); 
    
    Serial.print(F("\nCon cambio de 90º en los parámetros de DH del motor 2 (DHbi[1] = 0)"));
    Serial.print(F("\nb3varDH00_2 = ")); Serial.print(b3varDH00_2); Serial.print(F(" // b3varDH01_2 = ")); Serial.print(b3varDH01_2);
    Serial.print(F(" // b3varDH02_2 = ")); Serial.print(b3varDH02_2); Serial.print(F(" // b3varDH03_2 = ")); Serial.println(b3varDH03_2);
    Serial.print(F("b3varDH10_2 = ")); Serial.print(b3varDH10_2); Serial.print(F(" // b3varDH11_2 = ")); Serial.print(b3varDH11_2);
    Serial.print(F(" // b3varDH12_2 = ")); Serial.print(b3varDH12_2); Serial.print(F(" // b3varDH13_2 = ")); Serial.println(b3varDH13_2);
    Serial.print(F("b3varDH20_2 = ")); Serial.print(b3varDH20_2); Serial.print(F(" // b3varDH21_2 = ")); Serial.print(b3varDH21_2);
    Serial.print(F(" // b3varDH22_2 = ")); Serial.print(b3varDH22_2); Serial.print(F(" // b3varDH23_2 = ")); Serial.println(b3varDH23_2);

    // Ahora se calcula la posición (x, y, z) con referencia al sistema cartesiano base, usando como dato origen un punto con referencia al último sistema de coordenadas. 
    // Resulta interesante cuando se usa con Teta2=0 ya que con un punto como puntoF=(-100,-100,100) y puntoF9_2=(0,0,5) sólo varían las componentes x,y. 
    // Para ello se ejecutarán las siguientes ecuaciones del producto matricial:
    puntoF[0] = b3varDH00_2 * A_2 + b3varDH01_2 * B_2 + b3varDH02_2 * C_2 + b3varDH03_2 * 1; 
    puntoF[1] = b3varDH10_2 * A_2 + b3varDH11_2 * B_2 + b3varDH12_2 * C_2 + b3varDH13_2 * 1; 
    puntoF[2] = b3varDH20_2 * A_2 + b3varDH21_2 * B_2 + b3varDH22_2 * C_2 + b3varDH23_2 * 1; 
    Serial.print(F("Puntox0Extremo = ")); Serial.print(b3varDH03_2); Serial.print(F(" // Puntoy0Extremo = ")); Serial.print(b3varDH13_2);
    Serial.print(F(" // Puntoz0Extremo = ")); Serial.println(b3varDH23_2); 
    Serial.print(F("PuntoxRelativo0Extremo = ")); Serial.print(puntoF[0]); Serial.print(F(" // PuntoyRelativo0Extremo = ")); Serial.print(puntoF[1]);
    Serial.print(F(" // PuntozRelativo0Extremo = ")); Serial.println(puntoF[2]); 
  }
  return;
}

void puntoFinalAbsoluto(double A, double B, double C, int brazoN) {
  // Datos: Px (o possig), Py, Pz, Brazo
  // Mueve una pierna/ brazo hasta un punto por cinemática inversa independientemente de cuál sea éste.
  //Serial.println("******  Pasa - puntoFinalAbsoluto()--------");
 
  //  int brazoN1 = 3;int brazoN2 = 4;int piernaN1 = 1;int piernaN2 = 2; // Brazo Derecho (4) y Pierna Izquierda (1) (Brazo Izquierdo (3) y Pierna Derecha (2))
  Serial.print(F("\n********  Punto= (")); Serial.print(A);Serial.print(F(","));Serial.print(B);Serial.print(F(","));Serial.print(C);Serial.println(F(")"));   
  double Teta1pradianes = 0;
  double Teta1pgrados = 0;
  double Teta2pradianes = 0;
  double Teta2pgrados = 0;
  double Teta3pradianes = 0;
  double Teta3pgrados = 0;
  double Teta4pradianes  = 0; 
  double Teta4pgrados = 0; 
  double Teta5pradianes = 0;
  double Teta5pgrados = 0;
  double Teta1psec = 0; 
  double Teta2psec = 0;  
  double Teta3psec = 0; 
  double Teta4psec = 0; 
  double Teta5psec = 0;
  double Teta1bradianes = 0;
  double Teta2bradianes = 0;
  double Teta3bradianes = 0;
  double Teta1bgrados = 0; 
  double Teta2bgrados = 0; 
  double Teta3bgrados = 0; 
  double Teta1bsec = 0; 
  double Teta2bsec = 0; 
  double Teta3bsec = 0; 
  
    
  if (abs(A) < 0.001) { // Para evitar las raices cuadradas de números negativos.
    A = 0.001;
  }
  if (abs(B) < 0.001) { 
    B = 0.001;
  }
  if (abs(C) < 0.001) {
    C = 0.001;
  }
  
  if ((brazoN == 1) or (brazoN == 2)){
    double Long7 = A; // Se cambian los ejes según el diagrama que representa la pierna y los parámetros de denavit-Hartenberg, respecto a los ya cambiados para las piernas y enviads a esta función....
    double Long6 = B;
    double Long5 = C;    

// Esta parte viene de aplicar una solución según em modelo de SCARA.
    Teta1pradianes = atan(Long6/(abs(Long5) - Lp5));
    Teta1pgrados = radianesAGrados(Teta1pradianes); 
    double Teta3pradianes_1 = (abs(Long5)-Lp4-Lp5-(Lp1*cos(Teta1pradianes)))/cos(Teta1pradianes);
    Teta3pradianes = acos(( (Teta3pradianes_1*Teta3pradianes_1) + (Long7*Long7)-(Lp2*Lp2)-(Lp3*Lp3))/(2*Lp2*Lp3)) ; 
    if (Long7 >= -8) {
    Teta3pradianes = - Teta3pradianes ; 
    }
    Teta3pgrados = radianesAGrados(Teta3pradianes); // +- 
    double Teta2pradianes_1 = (atan(Long7/Teta3pradianes_1));
    double Teta2pradianes_2 = atan((Lp3*sin(Teta3pradianes))/(Lp2+(Lp3*cos(Teta3pradianes)))) ; 
    Teta2pradianes = Teta2pradianes_1 - Teta2pradianes_2; // +-
    Teta2pgrados = radianesAGrados(Teta2pradianes); // +- 
    Teta4pradianes = -Teta2pradianes - Teta3pradianes; 
    Teta4pgrados = radianesAGrados(Teta4pradianes) - (varYcorregirFinal-varYOffsetFinal); // +- 
    Teta5pradianes = Teta1pradianes;
    Teta5pgrados = Teta1pgrados - (varXcorregirFinal-varXOffsetFinal);

    //Teta1pradianes = 0;  Teta2pradianes = PI/2; Teta3pradianes = 0; Teta4pradianes = 0; Teta5pradianes = 0; // PI/2
    //Teta1pgrados = 0; Teta2pgrados = 90; Teta3pgrados = 0; Teta4pgrados = 0; Teta5pgrados = 0; //Con todo 0 da la altura de la cadera: 289mm

    int zz = (brazoN - 1) * 8;
    Teta1psec = map(Teta1pgrados, angMin[zz], angMax[zz], msecMin[zz], msecMax[zz]); // Teta1psec // 2 Piernas 
    Teta2psec = map(Teta2pgrados, angMin[zz+1], angMax[zz+1], msecMin[zz+1], msecMax[zz+1]); // Teta2psec // 2 Piernas 
    Teta3psec = map(Teta3pgrados, angMin[zz+2], angMax[zz+2], msecMin[zz+2], msecMax[zz+2]); // Teta3psec // 2 Piernas 
    Teta4psec = map(Teta4pgrados, angMin[zz+3], angMax[zz+3], msecMin[zz+3], msecMax[zz+3]); // Teta4psec // 2 Piernas 
    Teta5psec = map(Teta5pgrados, angMin[zz+4], angMax[zz+4], msecMin[zz+4], msecMax[zz+4]); // Teta5psec // 2 Piernas 
    if ((imprimir0 == 0) or (imprimir0 == 1)) {
      Serial.println(F("***  ---------- "));
      Serial.print(F("*******  brazoN= "));  Serial.print(brazoN);     
      Serial.print(F("\n***  Teta1pgrados= "));  Serial.print(Teta1pgrados);     
      Serial.print(F(" ***  Teta1psec= "));  Serial.println(Teta1psec);    
      Serial.print(F("***  Teta2pgrados= "));  Serial.print(Teta2pgrados); 
      Serial.print(F(" ***  Teta2psec= "));  Serial.println(Teta2psec);    
      Serial.print(F("***  Teta3pgrados= "));  Serial.print(Teta3pgrados);  
      Serial.print(F(" ***  Teta3psec= "));  Serial.println(Teta3psec);    
      Serial.print(F("***  Teta4pgrados= "));  Serial.print(Teta4pgrados);    
      Serial.print(F(" ***  Teta4psec= "));  Serial.println(Teta4psec);     
      Serial.print(F("***  Teta5pgrados= "));  Serial.print(Teta5pgrados);    
      Serial.print(F(" ***  Teta5psec= "));  Serial.println(Teta5psec);    
    }
    prepararMovimientoDeUnaPierna(brazoN,Teta1psec,Teta2psec,Teta3psec,Teta4psec,Teta5psec); // Datos = pierna
    GiroCalculado[0] = Teta1pgrados; // Se meten los valores en un array para extraerlos cuando giren los motores hasta que no haya más giros calculados. Se podnrán tantas líneas como motores definidos con movimiento.
    GiroCalculado[1] = Teta2pgrados; GiroCalculado[2] = Teta3pgrados; GiroCalculado[3] = Teta4pgrados; GiroCalculado[4] = Teta5pgrados;
  } 
  if ((brazoN == 3) or (brazoN == 4)){
    numart = 3;
    double bLong5 = C; 
    double bLong6 = -A;
    double bLong7 = B; // Se cambian otra vez los ejes según el diagrama que representa el brazo y los parámetros de denavit-Hartenberg, respecto a los ya cambiados para las piernas y enviads a esta función.... 
    //// P1=(L1cos(Teta1),0,L1cos(Teta1))
    //// P= (Px,Py,Pz)
    //// P-P1=(Px-L1sin(Teta1),Py,Pz-L1cos(Teta1))
    double Teta1bradianes_1 = abs(bLong5);
    Teta1bradianes = -atan(bLong6/Teta1bradianes_1);
    Teta1bgrados = radianesAGrados(Teta1bradianes); 
    double Teta3bradianes_1 = ((abs(bLong5)-(Lb1*cos(Teta1bradianes)))/cos(Teta1bradianes));
    double Teta3bradianes_2 = abs(bLong7)-(Lb1*sin(Teta1bradianes));
    //double Teta3bradianes_2 = abs(bLong6)-(Lb1*sin(Teta1bradianes));
    //double Teta3bradianes_3 = abs( sqrt( (Teta3bradianes_2*Teta3bradianes_2) + (Teta3bradianes_1*Teta3bradianes_1) ) );
    Teta3bradianes = -acos(( (Teta3bradianes_1*Teta3bradianes_1) + (bLong7*bLong7)-(Lb2*Lb2)-(Lb3*Lb3))/(2*Lb2*Lb3)) ; 
    Teta3bgrados = radianesAGrados(Teta3bradianes); // +- 
    double Teta2bradianes_1 = (atan(bLong7/Teta3bradianes_1));
    double Teta2bradianes_2 = atan((Lb3*sin(Teta3bradianes))/(Lb2+(Lb3*cos(Teta3bradianes)))) ; 
    Teta2bradianes = -Teta2bradianes_1 - Teta2bradianes_2; // +-
    Teta2bgrados = radianesAGrados(Teta2bradianes); // +- 

    //Teta1bradianes = 0;  Teta2bradianes = PI/2; Teta3bradianes = 0;  // PI/2 // Para pruega de configuración y montaje de los motores
    //Teta1bgrados = 0; Teta2bgrados = 90; Teta3bgrados = 0;  //Con todo 0 da la altura de la cadera: 194mm

    int zz = ((brazoN - 3) * 8) + 5;
    Teta1bsec = map(Teta1bgrados, angMin[zz], angMax[zz], msecMin[zz], msecMax[zz]); // Teta1bsec // 2 Brazos 
    Teta2bsec = map(Teta2bgrados, angMin[zz+1], angMax[zz+1], msecMin[zz+1], msecMax[zz+1]); // Teta2bsec // 2 Brazos 
    Teta3bsec = map(Teta3bgrados, angMin[zz+2], angMax[zz+2], msecMin[zz+2], msecMax[zz+2]); // Teta3bsec // 2 Brazos 
    if ((imprimir0 == 0) or (imprimir0 == 1)) {
      Serial.println(F("***  ---------- "));
      Serial.print(F("*******  brazoN= "));  Serial.print(brazoN);     
      Serial.print(F("\n***  Teta1bgrados= "));  Serial.print(Teta1bgrados);     
      Serial.print(F(" ***  Teta1bsec= "));  Serial.println(Teta1bsec);    
      Serial.print(F("***  Teta2bgrados= "));  Serial.print(Teta2bgrados); 
      Serial.print(F(" ***  Teta2bsec= "));  Serial.println(Teta2bsec);    
      Serial.print(F("***  Teta3bgrados= "));  Serial.print(Teta3bgrados);  
      Serial.print(F(" ***  Teta3bsec= "));  Serial.println(Teta3bsec);    
    } 
    prepararMovimientoDeUnBrazo(brazoN,Teta1bsec,Teta2bsec,Teta3bsec); // Datos = brazo 
    GiroCalculado[0] = Teta1bgrados; // Se meten los valores en un array para extraerlos cuando giren los motores hasta que no haya más giros calculados. Se podnrán tantas líneas como motores definidos con movimiento.
    GiroCalculado[1] = Teta2bgrados; GiroCalculado[2] = Teta3bgrados; 
  } 

  if ((brazoN == 1) or (brazoN == 2)) { // p5DHdi[] 
    numart = 5;
    int t = (brazoN - 1) * 8;
    for (int j = 0; j < numart; j = j + 1) { // Se prepara así por si en algún momento se implementa la posibilidad de meter un codo sin motor, sólo desplazamiento.
      // Metemos en "angMotor[s]" el valor final del ángulo a girar en el motor
        int s = t + j;
        //angMotor[s] = GiroCalculado[j];  // p5DHai[j] en realidad, no hace falta porque sólo gira sobre un único eje. GiroCalculado[j] es el águlo calculado para cada motor en Cinmática Inversa.
        //msecMotor[s] = map(angMotor[s], angMin[s], angMax[s], msecMin[s], msecMax[s]); // pulso calculado = map(ángulo a transformar, ángulo mínimo, ángulo máximo, pulso mínimo, pulso máximo)
        p5DHbi2[j] = GiroCalculado[j]; // Este valor es el que se usa para el cálculo del punto mientras que msecMotor[j] es el valor del ángulo corregido para ajustar el 90º.
        // Se puede quitar p5DHbi2[s] si se usa en todas partes angMotor[s] 
      if (imprimir0 == 1) {
        Serial.print(F("s = "));Serial.print(s); // j recorre las articulaciones que correspondan: 0, 1, 2, 3, 4, 5, 6,...
        Serial.print(F(" - msecMotor[s] final: "));Serial.print(msecMotor[s]); Serial.print(F(" - angMotor[s]= "));Serial.print(angMotor[s]);
        Serial.print(F(" - p5DHbi2[j]: "));Serial.print(p5DHbi2[j]);
        Serial.print(F(" - GiroCalculado[j]: "));Serial.println(GiroCalculado[j]);
      } // Termina el if (imprimir0 == 1)
    } // Termina el for (j)
  }else if ((brazoN == 3) or (brazoN == 4)){
    numart = 3;
    int t = (brazoN - 3) * 8;
    for (int j = 0; j < numart; j = j + 1) { // Se prepara así por si en algún momento se implementa la posibilidad de meter un codo sin motor, sólo desplazamiento.
      // Metemos en "angMotor[s]" el valor final del ángulo a girar en el motor
        int s = t + j + 5;
        //angMotor[s] = GiroCalculado[j];  // b3DHai[j] en realidad, no hace falta porque sólo gira sobre un único eje. GiroCalculado[j] es el águlo calculado para cada motor en Cinmática Inversa.
        //msecMotor[s] = map(angMotor[s], angMin[s], angMax[s], msecMin[s], msecMax[s]); // pulso calculado = map(ángulo a transformar, ángulo mínimo, ángulo máximo, pulso mínimo, pulso máximo)
        b3DHbi2[j] = GiroCalculado[j]; // Este valor es el que se usa para el cálculo del punto mientras que msecMotor[j] es el valor del ángulo corregido para ajustar el 90º.
        // Se puede quitar p5DHbi2[s] si se usa en todas partes angMotor[s] 
      if (imprimir0 == 1) {
        Serial.print(F("s = "));Serial.print(s); // j recorre las articulaciones que correspondan: 0, 1, 2, 3, 4, 5, 6,...
        Serial.print(F(" - msecMotor[s] final: "));Serial.print(msecMotor[s]); Serial.print(F(" - angMotor[s]= "));Serial.print(angMotor[s]);
        Serial.print(F(" - b3DHbi2[j]: "));Serial.print(b3DHbi2[j]);
        Serial.print(F(" - GiroCalculado[j]: "));Serial.println(GiroCalculado[j]);
      } // Termina el if (imprimir0 == 1)
    } // Termina el for (j)
  } 

//-------------------------------------------------------------------------------------------------------------------------------
// Cálculo de puntos por DH
  if ((imprimir0 == 0) or (imprimir0 == 1)) {
    if ((brazoN == 1) or (brazoN == 2)) { // p5DHdi[] 
      Serial.print(F("-------------------- PUNTOS INTERMEDIOS ------------------------"));
      Serial.print(F("\nP1 = (0 , "));Serial.print(Lp1*sin(Teta1pradianes));Serial.print(F(" , "));Serial.print(-Lp1*cos(Teta1pradianes));Serial.print(F(")"));
      Serial.print(F("\nP2 = ("));Serial.print(cos(Teta1pradianes)*(-Lp1+-Lp2*cos(Teta2pradianes)));Serial.print(F(" , "));
        Serial.print(sin(Teta1pradianes)*(-Lp1-Lp2*cos(Teta2pradianes)));Serial.print(F(" , "));Serial.print(Lp2*sin(Teta2pradianes));Serial.print(F(")"));
      Serial.print(F("\nP3 = ("));Serial.print(cos(Teta1pradianes)*(-Lp1-Lp3*cos(Teta2pradianes+Teta3pradianes)-Lp2*cos(Teta2pradianes)));Serial.print(F(" , "));
        Serial.print(sin(Teta1pradianes)*(-Lp1-Lp3*cos(Teta2pradianes+Teta3pradianes)-Lp2*cos(Teta2pradianes)));Serial.print(F(" , "));
        Serial.print(Lp3*sin(Teta2pradianes+Teta3pradianes)+Lp2*sin(Teta2pradianes));Serial.print(F(")"));
      Serial.print(F("\nP4 = ("));Serial.print(cos(Teta1pradianes)*(-Lp1-Lp3*cos(Teta2pradianes+Teta3pradianes)-Lp2*cos(Teta2pradianes)-Lp4*cos(Teta2pradianes+Teta3pradianes+Teta4pradianes)));Serial.print(F(" , "));
        Serial.print(sin(Teta1pradianes)*(-Lp1-Lp3*cos(Teta2pradianes+Teta3pradianes)-Lp2*cos(Teta2pradianes)-Lp4*cos(Teta2pradianes+Teta3pradianes+Teta4pradianes)));Serial.print(F(" , "));
        Serial.print(Lp3*sin(Teta2pradianes+Teta3pradianes)+Lp2*sin(Teta2pradianes)+Lp4*sin(Teta2pradianes+Teta3pradianes+Teta4pradianes));Serial.print(F(")"));
      Serial.println(F("\n**********************************************************************************************************************************"));
    } else if ((brazoN == 3) or (brazoN == 4)){ // b3DHdi[] 
    //// P1=(L1cos(Teta1),0,L1cos(Teta1))
    //// P= (Px,Py,Pz)
    //// P-P1=(Px-L1sin(Teta1),Py,Pz-L1cos(Teta1))
      Serial.print(F("-------------------- PUNTOS INTERMEDIOS ------------------------"));
      Serial.print(F("\nP1 = ("));Serial.print(Lp1*cos(Teta1pradianes));Serial.print(F(" , 0 , "));Serial.print(-Lp1*sin(Teta1pradianes));Serial.print(F(")"));
      Serial.print(F("\nP2 = ("));;Serial.print(F(" , "));
        Serial.print(F(" , "));Serial.println(F(")"));

    }
  }
//-------------------------------------------------------------------------------------------------------------------------------
  
  calDHdirectoNmotores(brazoN,A,B,C); // Calculará la posición final de cada brazo/pierna // Hay que probar si también se intercambian (X,Z)
  return;
}

void imprimirDatos1(int brazoN, int im02, double& im03, double& im04, double im05, double im06, double im07, double im08, double im09, double im10, double im11, double im12, double im13, double im14, double& im15, double& im16, double& im17, double& im18, double& im19) { // Función de imprimir
  // Imprime los datos de un brazo independientemente de cuál sea éste.
  //Serial.println("***  Pasa - imprimirDatos1()--------");
  int z = (brazoN - 1) * 3;
//  Serial.print("* Brazo: "); Serial.print(brazoN);
/*  Serial.print(" - puntoInalcanzableConTita[brazoN-1]: "); Serial.print(im02);
  Serial.print(" - alturaLim: "); Serial.print(im03); 
  Serial.print(" - titaLim: ");  Serial.print(im04);
  Serial.print(" - anguloAlturaTitaLim: "); Serial.println(im05);
  Serial.print("* LA: "); Serial.print(im06);
  Serial.print(" - LH: "); Serial.print(im07);
  Serial.print(" - L1: "); Serial.print(im08);
  Serial.print(" - L2: "); Serial.println(im09);
  Serial.print("* Px': "); Serial.print(im10);
  Serial.print(" - Py': "); Serial.print(im11);
  Serial.print(" - Pz': "); Serial.println(im12);
  Serial.print("* Px: ");  Serial.print(im13);
  Serial.print(" - Py: ");  Serial.print(im14); 
  Serial.print(" - Pz: ");  Serial.println(im15);
  Serial.print("* θ'-Grados: "); Serial.print(im16);
  Serial.print(" - φ-Grados: "); Serial.print(im17);
  Serial.print(" - γ-Grados= "); Serial.print(im18);
  Serial.print(" - ω-Grados (sin modificar): "); Serial.print(im19);
  Serial.print(" - ω-Grados (modificado): "); Serial.println(angMotor[z+1]);
  delay(10);*/
}
void imprimirDatos2(int brazoN) { // Función de imprimir
  // Imprime los datos de un brazo independientemente de cuál sea éste.
  //Serial.print("***  Pasa - imprimirDatos2()--------");
///  int z = (brazoN) * 3;
  //Serial.print("* brazoN: "); Serial.print(brazoN);
  //Serial.print(" - ω-sec: "); Serial.print(msecMotor[z]);
  //Serial.print(" - θ1-sec: "); Serial.print(msecMotor[z+1]);
  //Serial.print(" - θ2-sec: "); Serial.println(msecMotor[z+2]);
  //  Serial.print("* θ-grados:"); Serial.print(angMotor[z]);
  //  Serial.print(" - ω-grados:"); Serial.print(angMotor[z+1]);
  //  Serial.print(" - θ1-grados:"); Serial.print(angMotor[z+2]);
  //  Serial.print(" - θ2-grados:"); Serial.println(angMotor[z+3]);
  delay(10);
  //return; 
}
void caminarPasoRegular() {
  // Genera la secuencia de pasos. Esta función diferencia y sincroniza cada brazo y pierna contrarios y contempla su secuencia de movimientos.
  //Serial.println("******  Pasa - caminarPasoRegular()--------");
  int indpaso1;

  Serial.println(F("\n  El robot se agacha hasta su posición de inicio de caminar - Movimiento 1"));
  puntoFinalAbsoluto(Long5i, Long6i, Long7i, piernaN1); 
  puntoFinalAbsoluto(Long5i, Long6i, Long7i, piernaN2); 
  ejecutarParalelo();
  delay(tiempoEntreMovimientos); 
    
//--------------- Paso 1
  // Movimiento 1 - El robot se inclina hacia la derecha para equilibrar su centro de gravedad sobre la pierna derecha 
  Serial.println(F("\n  El robot se inclina hacia la derecha para equilibrar su centro de gravedad sobre la pierna derecha - Movimiento 1"));
  puntoFinalAbsoluto(Long5i, Long6i+DesfaseEquilibrioLateralDosPiernas, Long7i, piernaN1); 
  puntoFinalAbsoluto(Long5i, Long6i-DesfaseEquilibrioLateralDosPiernas, Long7i, piernaN2); 
  ejecutarParalelo();
  delay(tiempoEntreMovimientos); 

  // Movimiento 2 - Se levanta la pierna izquierda hasta 20mm por encima del suelo (1) 
  Serial.println(F("  Se levanta la pierna izquierda hasta 20mm por encima del suelo (1)  - Movimiento 2"));
  puntoFinalAbsoluto(Long5i, (Long6i+DesfaseEquilibrioLateralUnaPierna+1), Long7i+AlturaDeParadaAntesDeSuelo, piernaN1); 
  puntoFinalAbsoluto(Long5i, (Long6i-DesfaseEquilibrioLateralUnaPierna-1), Long7i, piernaN2); 
  ejecutarParalelo();
  delay(tiempoEntreMovimientos);    

  // Movimiento 3 - Se levanta la pierna izquierda (1) 
  Serial.println(F("  Se levanta la pierna izquierda (1) - Movimiento 3"));
  puntoFinalAbsoluto(Long5i, (Long6i+DesfaseEquilibrioLateralUnaPierna+1), Long7i+AlturaDeParadaAlta, piernaN1); 
  //puntoFinalAbsoluto(Long5i, (Long6i-DesfaseEquilibrioLateralUnaPierna-1), Long7i, piernaN2); 
  ejecutarParalelo();
  delay(tiempoEntreMovimientos);    

  // Movimiento 4 - Se adelanta la pierna izquierda (1) - Altura final = 10mm
  Serial.println(F("  Se adelanta la pierna izquierda (1) - Altura final = 10mm - (Movimiento 4)"));
  puntoFinalAbsoluto(Long5i+possig, (Long6i+DesfaseEquilibrioLateralUnaPierna+1), Long7i+AlturaDeParadaAntesDeSuelo, piernaN1); 
  //puntoFinalAbsoluto(Long5i, Long6i-DesfaseEquilibrioLateralUnaPierna-1, Long7i, piernaN2); 
  puntoFinalAbsoluto(bpossig, SepararManoFuera, AlturaMano, brazoN2); 
  ejecutarParalelo();
  delay(tiempoEntreMovimientos); 
    
  // Movimiento 5 - Se termina de bajar la pierna izquierda y el robot equilibra su centro de gravedad  
  Serial.println(F("\n  Se termina de bajar la pierna izquierda y el robot equilibra su centro de gravedad - Movimiento 5"));
  puntoFinalAbsoluto(Long5i+possig, Long6i, Long7i, piernaN1); 
  puntoFinalAbsoluto(Long5i, Long6i, Long7i, piernaN2); 
  //puntoFinalAbsoluto(bpossig, SepararManoFuera, AlturaMano, brazoN2); 
  ejecutarParalelo();
  delay(tiempoEntreMovimientos); 

  // Movimiento 6 - Se lleva la pierna izquierda debajo de la vertical del robot (1) mientras se retrasa la pierna derecha (2) para avanzar el robot. 
  // Ambos pies terminan a la misma altura sobre el suelo y el cuerpo pasa a estar un paso por delante.
  Serial.println(F("   Se lleva la pierna izquierda debajo de la vertical del robot (1) mientras se retrasa la pierna derecha (2) para avanzar el robot. - Movimiento 6"));
  puntoFinalAbsoluto(Long5i+DesfaseEquilibrioFrontal, (Long6i), Long7i, piernaN1); 
  puntoFinalAbsoluto(Long5i-possig+DesfaseEquilibrioFrontal, Long6i, Long7i, piernaN2); 
  puntoFinalAbsoluto(0, SepararManoDentro, AlturaMano, brazoN2); 
  ejecutarParalelo();
  delay(tiempoEntreMovimientos); // Una espera adecuada podía ser de 100...

  for (indpaso1 = 1; indpaso1 <= 2; indpaso1  += 1) { // SE PONDRÁ A 1 CUANDO CAMINE DE FORMA CONSTANTE YA QUE SE REPETIRÁ MEDIANTE loop().
    delay(tiempoPausaDivisorMovimientos); // Una espera entre porciones de cada movimiento. 
//--------------- Paso 2 - Inicio del bucle 
    // Movimiento 7 - El robot se inclina hacia la izquierda para equilibrar su centro de gravedad sobre la pierna izquierda 
    Serial.println(F("\n  El robot se inclina hacia la izquierda para equilibrar su centro de gravedad sobre la pierna izquierda - Movimiento 7"));
    puntoFinalAbsoluto(Long5i, Long6i-DesfaseEquilibrioLateralDosPiernas, Long7i, piernaN1); 
    puntoFinalAbsoluto(Long5i-possig, Long6i+DesfaseEquilibrioLateralDosPiernas, Long7i, piernaN2); 
    //puntoFinalAbsoluto(0, SepararManoDentro, AlturaMano, brazoN2); 
    ejecutarParalelo();
    delay(tiempoEntreMovimientos); 

    // Movimiento 8 - La pierna derecha se levanta  hasta 20mm por encima del suelo (2) // Se puede quitar
    Serial.println(F("  La pierna derecha se levanta  hasta 20mm por encima del suelo (2) - Movimiento 8"));
    puntoFinalAbsoluto(Long5i, (Long6i-DesfaseEquilibrioLateralUnaPierna)-1, Long7i, piernaN1); 
    puntoFinalAbsoluto(Long5i-possig, (Long6i+DesfaseEquilibrioLateralUnaPierna+1), Long7i+AlturaDeParadaAntesDeSuelo, piernaN2); 
    //puntoFinalAbsoluto(0, SepararManoDentro, AlturaMano, brazoN2); 
    ejecutarParalelo();
    delay(tiempoEntreMovimientos);    

    // Movimiento 9 - La pierna derecha se levanta del todo debajo del cuerpo  (2) 
    Serial.println(F("  La pierna derecha se levanta del todo debajo del cuerpo  (2) - Movimiento 9"));
    puntoFinalAbsoluto(Long5i, (Long6i-DesfaseEquilibrioLateralUnaPierna)-1, Long7i, piernaN1); 
    puntoFinalAbsoluto(Long5i, (Long6i+DesfaseEquilibrioLateralUnaPierna)+1, Long7i+AlturaDeParadaAlta, piernaN2); 
    //puntoFinalAbsoluto(0, SepararManoDentro, AlturaMano, brazoN2); 
    ejecutarParalelo();
    delay(tiempoEntreMovimientos);    

    // Movimiento 10 - Se adelanta la pierna derecha (2) - Altura final = 10mm // Se retrasa la mano izquierda
    Serial.println(F("  Se adelanta la pierna derecha (2) - Altura final = 10mm - (Movimiento 10)"));
    puntoFinalAbsoluto(Long5i, Long6i-DesfaseEquilibrioLateralUnaPierna-1, Long7i, piernaN1); 
    puntoFinalAbsoluto(Long5i+possig, (Long6i+DesfaseEquilibrioLateralUnaPierna+1), Long7i+AlturaDeParadaAntesDeSuelo, piernaN2); 
    puntoFinalAbsoluto(bpossig, SepararManoFuera, AlturaMano, brazoN1); 
    puntoFinalAbsoluto(-bpossig+4, SepararManoFuera, AlturaMano, brazoN2); 
    ejecutarParalelo();
    delay(tiempoEntreMovimientos); 

    // Movimiento 11 - Se termina de bajar la pierna derecha y el robot equilibra su centro de gravedad  
    Serial.println(F("\n  Se termina de bajar la pierna derecha y el robot equilibra su centro de gravedad - Movimiento 11"));
    puntoFinalAbsoluto(Long5i, Long6i, Long7i, piernaN1); 
    puntoFinalAbsoluto(Long5i+possig, Long6i, Long7i, piernaN2); 
    //puntoFinalAbsoluto(bpossig, SepararManoFuera, AlturaMano, brazoN1); 
    //puntoFinalAbsoluto(-bpossig+4, SepararManoFuera, AlturaMano, brazoN2); 
    ejecutarParalelo();
    delay(tiempoEntreMovimientos); 

    // Movimiento 12 - Se lleva la pierna derecha debajo de la vertical del robot (2) mientras se retrasa la pierna izquierda (1) para avanzar el robot. 
    // Ambos pies terminan a la misma altura sobre el suelo y el cuerpo pasa a estar un paso por delante.
    Serial.println(F("   Se lleva la pierna derecha debajo de la vertical del robot (2) mientras se retrasa la pierna izquierda (1) para avanzar el robot. - Movimiento 12"));
    puntoFinalAbsoluto(Long5i-possig+DesfaseEquilibrioFrontal, Long6i, Long7i, piernaN1); 
    puntoFinalAbsoluto(Long5i+DesfaseEquilibrioFrontal, Long6i, Long7i, piernaN2); 
    puntoFinalAbsoluto(0, SepararManoDentro, AlturaMano, brazoN1); 
    puntoFinalAbsoluto(0, SepararManoDentro, AlturaMano, brazoN2); 
    ejecutarParalelo();
    delay(tiempoEntreMovimientos); // Una espera adecuada podía ser de 100...

//--------------- Paso 3 
    // Movimiento 13 - El robot se inclina hacia la derecha para equilibrar su centro de gravedad sobre la pierna derecha 
    Serial.println(F("\n  El robot se inclina hacia la derecha para equilibrar su centro de gravedad sobre la pierna derecha - Movimiento 13"));
    puntoFinalAbsoluto(Long5i-possig, Long6i+DesfaseEquilibrioLateralDosPiernas, Long7i, piernaN1); 
    puntoFinalAbsoluto(Long5i, Long6i-DesfaseEquilibrioLateralDosPiernas, Long7i, piernaN2); 
    //puntoFinalAbsoluto(0, SepararManoDentro, AlturaMano, brazoN1); 
    //puntoFinalAbsoluto(0, SepararManoDentro, AlturaMano, brazoN2); 
    ejecutarParalelo();
    delay(tiempoEntreMovimientos); 

    // Movimiento 14 - Se levanta la pierna izquierda hasta 20mm por encima del suelo (1) 
    Serial.println(F("  Se levanta la pierna izquierda hasta 20mm por encima del suelo (1) - Movimiento 14"));
    puntoFinalAbsoluto(Long5i-possig, (Long6i+DesfaseEquilibrioLateralUnaPierna+1), Long7i+AlturaDeParadaAntesDeSuelo, piernaN1); 
    puntoFinalAbsoluto(Long5i, (Long6i-DesfaseEquilibrioLateralUnaPierna-1), Long7i, piernaN2); 
    //puntoFinalAbsoluto(0, SepararManoDentro, AlturaMano, brazoN1); 
    //puntoFinalAbsoluto(0, SepararManoDentro, AlturaMano, brazoN2); 
    ejecutarParalelo();
    delay(tiempoEntreMovimientos);    

    // Movimiento 15 - Se levanta del todo y avanza la pierna izquierda (1) 
    Serial.println(F("  Se levanta del todo y avanza la pierna izquierda (1) - Movimiento 15"));
    puntoFinalAbsoluto(Long5i, (Long6i+DesfaseEquilibrioLateralUnaPierna+1), Long7i+AlturaDeParadaAlta, piernaN1); 
    puntoFinalAbsoluto(Long5i, (Long6i-DesfaseEquilibrioLateralUnaPierna-1), Long7i, piernaN2); 
    //puntoFinalAbsoluto(0, SepararManoDentro, AlturaMano, brazoN1); 
    //puntoFinalAbsoluto(0, SepararManoDentro, AlturaMano, brazoN2); 
    ejecutarParalelo();
    delay(tiempoEntreMovimientos);    

    // Movimiento 16 - Se adelanta la pierna izquierda (1) - Altura final = 10mm
    Serial.println(F("  Se adelanta la pierna izquierda (1) - Altura final = 10mm - (Movimiento 16)"));
    puntoFinalAbsoluto(Long5i+possig, (Long6i+DesfaseEquilibrioLateralUnaPierna+1), Long7i+AlturaDeParadaAntesDeSuelo, piernaN1); 
    //puntoFinalAbsoluto(Long5i, Long6i-DesfaseEquilibrioLateralUnaPierna-1, Long7i, piernaN2); 
    puntoFinalAbsoluto(-bpossig+4, SepararManoFuera, AlturaMano, brazoN1); 
    puntoFinalAbsoluto(bpossig, SepararManoFuera, AlturaMano, brazoN2); 
    ejecutarParalelo();
    delay(tiempoEntreMovimientos); 
    
    // Movimiento 17 - Se termina de bajar la pierna izquierda y el robot equilibra su centro de gravedad  
    Serial.println(F("\n  Se termina de bajar la pierna izquierda y el robot equilibra su centro de gravedad - Movimiento 17"));
    puntoFinalAbsoluto(Long5i+possig, Long6i, Long7i, piernaN1); 
    puntoFinalAbsoluto(Long5i, Long6i, Long7i, piernaN2); 
    //puntoFinalAbsoluto(-bpossig+4, SepararManoFuera, AlturaMano, brazoN1); 
    //puntoFinalAbsoluto(bpossig, SepararManoFuera, AlturaMano, brazoN2); 
    ejecutarParalelo();
    delay(tiempoEntreMovimientos); 

    // Movimiento 18 - Se lleva la pierna izquierda debajo de la vertical del robot (1) mientras se retrasa la pierna derecha (2) para avanzar el robot. 
    // Ambos pies terminan a la misma altura sobre el suelo y el cuerpo pasa a estar un paso por delante.
    Serial.println(F("   Se lleva la pierna izquierda debajo de la vertical del robot (1) mientras se retrasa la pierna derecha (2) para avanzar el robot. - Movimiento 18"));
    puntoFinalAbsoluto(Long5i+DesfaseEquilibrioFrontal, Long6i, Long7i, piernaN1); 
    puntoFinalAbsoluto(Long5i-possig+DesfaseEquilibrioFrontal, Long6i, Long7i, piernaN2); 
    puntoFinalAbsoluto(0, SepararManoDentro, AlturaMano, brazoN1); 
    puntoFinalAbsoluto(0, SepararManoDentro, AlturaMano, brazoN2); 
    ejecutarParalelo();
    delay(tiempoEntreMovimientos); // Una espera adecuada podía ser de 100...

  } 

//--------------- Paso 4 - Final 
  // Movimiento 19 - El robot se inclina hacia la izquierda para equilibrar su centro de gravedad sobre la pierna izquierda 
  Serial.println(F("\n  El robot se inclina hacia la izquierda para equilibrar su centro de gravedad sobre la pierna izquierda  - Movimiento 19"));
  puntoFinalAbsoluto(Long5i, Long6i-DesfaseEquilibrioLateralDosPiernas, Long7i, piernaN1); 
  puntoFinalAbsoluto(Long5i-possig, Long6i+DesfaseEquilibrioLateralDosPiernas, Long7i, piernaN2); 
  //puntoFinalAbsoluto(0, SepararManoDentro, AlturaMano, brazoN1); 
  //puntoFinalAbsoluto(0, SepararManoDentro, AlturaMano, brazoN2); 
  ejecutarParalelo();
  delay(tiempoEntreMovimientos); 

  // Movimiento 20 - La pierna derecha se levanta  hasta 20mm por encima del suelo (1) // Se puede quitar
  Serial.println(F("  La pierna derecha se levanta  hasta 20mm por encima del suelo (1) - Movimiento 20"));
  puntoFinalAbsoluto(Long5i, (Long6i-DesfaseEquilibrioLateralUnaPierna)-1, Long7i, piernaN1); 
  puntoFinalAbsoluto(Long5i-possig, (Long6i+DesfaseEquilibrioLateralUnaPierna+1), Long7i+AlturaDeParadaAntesDeSuelo, piernaN2); 
  //puntoFinalAbsoluto(0, SepararManoDentro, AlturaMano, brazoN1); 
  //puntoFinalAbsoluto(0, SepararManoDentro, AlturaMano, brazoN2); 
  ejecutarParalelo();
  delay(tiempoEntreMovimientos);    

  // Movimiento 21 - La pierna derecha se levanta del todo debajo del cuerpo  (2) 
  Serial.println(F("  La pierna derecha se levanta del todo debajo del cuerpo  (2) - Movimiento 21"));
  //puntoFinalAbsoluto(Long5i, (Long6i-DesfaseEquilibrioLateralUnaPierna-1), Long7i, piernaN1); 
  puntoFinalAbsoluto(Long5i, (Long6i+DesfaseEquilibrioLateralUnaPierna+1), Long7i+AlturaDeParadaAlta, piernaN2); 
  //puntoFinalAbsoluto(0, SepararManoDentro, AlturaMano, brazoN1); 
  //puntoFinalAbsoluto(0, SepararManoDentro, AlturaMano, brazoN2); 
  ejecutarParalelo();
  delay(tiempoEntreMovimientos);    

  // Movimiento 22 - Se baja la pierna derecha en posición de parada (2). 
  Serial.println(F("   Se baja la pierna derecha en posición de parada (2) - Movimiento 22"));
  //puntoFinalAbsoluto(Long5i, Long6i-DesfaseEquilibrioLateralUnaPierna-1, Long7i, piernaN1); 
  puntoFinalAbsoluto(Long5i, (Long6i+DesfaseEquilibrioLateralUnaPierna+1), Long7i, piernaN2); 
  //puntoFinalAbsoluto(0, SepararManoDentro, AlturaMano, brazoN1); 
  //puntoFinalAbsoluto(0, SepararManoDentro, AlturaMano, brazoN2); 
  ejecutarParalelo();
  delay(tiempoEntreMovimientos); // Una espera adecuada podía ser de 100...

  // Movimiento 23 - El robot se equilibra hasta su posición de inicio de caminar. 
  Serial.println(F("\n  El robot se equilibra hasta su posición de inicio de caminar - Movimiento 23"));
  puntoFinalAbsoluto(Long5i, Long6i, Long7i, piernaN1); 
  puntoFinalAbsoluto(Long5i, Long6i, Long7i, piernaN2); 
  //puntoFinalAbsoluto(0, SepararManoDentro, AlturaMano, brazoN1); 
  //puntoFinalAbsoluto(0, SepararManoDentro, AlturaMano, brazoN2); 
  ejecutarParalelo();
  delay(tiempoEntreMovimientos); 

return; // Punto de verificación de posición y par de los motores
}
void limpiarVariables() {
  //Serial.print("***  Pasa - limpiarVariables()--------"); Serial.println("---Se borran variables---");
  for (int j = 0; j <= 15; j  += 1) { // Recorre los 4 brazos.
    fraccion[j] = 0; // Contiene el valor calculado de cada fracción para cada motor de cada brazo en movimiento paralelo.
  }
}

  double varXOffset = 0;
  double varYOffset = 0;
  double contadorOffset = 0;
  double contadorcorregir = 100;
  double valorXOffsetAnterior = 0;
  double valorYOffsetAnterior = 0;
  double valorXOffsetControl = 0;
  double valorYOffsetControl = 0;
  
void acelerometer() {
  unsigned long now = millis();
  unsigned long delta;
  int loopCount = 1;
  double varXcorregir;
  double varYcorregir;
  while (imu->IMURead()) { // Get the latest data if ready yet
    // This flushes remaining data in case we are falling behind
    if (++loopCount >= 10) // 10
      continue;
    fusion.newIMUData(imu->getGyro(), imu->getAccel(), imu->getCompass(), imu->getTimestamp());
    sampleCount++;
    if ((delta = now - lastRate) >= 1000) { 
      Serial.print(F("\nSample rate: ")); Serial.print(sampleCount);
      if (imu->IMUGyroBiasValid()) Serial.println(F(", gyro bias valid"));
      else Serial.println(F(", calculating gyro bias"));
      sampleCount = 0;
      lastRate = now;
    }
//##################
    //temperature = bmp280.getTemperature(); // Get the temperature, bmp180ReadUT MUST be called first
    //pressure = bmp280.getPressure();// Get the temperature
    //altitude = bmp280.calcAltitude(pressure); // Uncompensated caculation - in Meters
    //atm = pressure / 101325;
//##################
    if ((now - lastDisplay) >= DISPLAY_INTERVAL) {
      lastDisplay = now;
      xAxis = (((RTVector3&)fusion.getFusionPose()).x())* RTMATH_RAD_TO_DEGREE;
      yAxis = (((RTVector3&)fusion.getFusionPose()).y())* RTMATH_RAD_TO_DEGREE;
      zAxis = (((RTVector3&)fusion.getFusionPose()).z())* RTMATH_RAD_TO_DEGREE;
      
      contadorOffset = contadorOffset + 1;
      if ((contadorOffset >= 40) and (contadorOffset < 45)) {
        varXOffset = varXOffset + xAxis;
        varYOffset = varYOffset + yAxis;
        Serial.print(F("\n ********************** contadorOffset: ")); Serial.print(contadorOffset); 
        Serial.print(F(" - Roll: ")); Serial.print(xAxis); 
        Serial.print(F(" - Pitch: ")); Serial.print(yAxis); 
        Serial.print(F(" - Yaw: ")); Serial.print(zAxis); 
        Serial.print(F(" - varXOffset: ")); Serial.print(varXOffset); 
        Serial.print(F(" - varYOffset: ")); Serial.println(varYOffset); 
      }
      if ((contadorOffset == 45)) {
        varXOffsetFinal = varXOffset/5;
        varYOffsetFinal = varYOffset/5;
        //contadorOffset = 0;
        contadorcorregir = 0;
        Serial.print(F("-------------------- contadorOffset: ")); Serial.print(contadorOffset); 
        Serial.print(F(" - Roll: ")); Serial.print(xAxis); 
        Serial.print(F(" - Pitch: ")); Serial.print(yAxis); 
        Serial.print(F(" - Yaw: ")); Serial.print(zAxis); 
        Serial.print(F(" - varXOffsetFinal: ")); Serial.print(varXOffsetFinal); 
        Serial.print(F(" - varYOffsetFinal: ")); Serial.println(varYOffsetFinal); 
      } 
      contadorcorregir= contadorcorregir + 1;
      if (contadorcorregir <100) {
        varXcorregir = varXcorregir + xAxis;
        varYcorregir = varYcorregir + yAxis;
        Serial.print(F("+++++++++++++++++++++++++ contadorcorregir: ")); Serial.print(contadorcorregir); 
        Serial.print(F(" - varXOffset: ")); Serial.print(varXOffset); 
        Serial.print(F(" - varYOffset: ")); Serial.println(varYOffset); 
      }
      if ((contadorcorregir <100) and (contadorcorregir == 3)) {
        varXcorregirFinal = varXcorregir/3;
        varYcorregirFinal = varYcorregir/3;
        contadorcorregir = 0;
        
        corregirAcelerometer();
        
        Serial.print(F("Roll: ")); Serial.print(xAxis); 
        Serial.print(F(" - Pitch: ")); Serial.print(yAxis); 
        Serial.print(F(" - Yaw: ")); Serial.print(zAxis); 
        Serial.print(F("\n - contadorOffset: ")); Serial.print(contadorOffset); 
        Serial.print(F(" - varXOffset: ")); Serial.print(varXOffset); 
        Serial.print(F(" - varYOffset: ")); Serial.print(varYOffset); 
        Serial.print(F(" - varXOffsetFinal: ")); Serial.print(varXOffsetFinal); 
        Serial.print(F(" - varYOffsetFinal: ")); Serial.println(varYOffsetFinal); 
        valorXOffsetControl = varXOffsetFinal-varXOffset;
        valorYOffsetControl = varYOffsetFinal-varYOffset;
        if ((valorXOffsetAnterior > 5) or (valorYOffsetAnterior > 5)) {
          valorXOffsetAnterior = varXOffsetFinal;
          valorYOffsetAnterior = varYOffsetFinal;
        }
      } 
      //Serial.print(F(" Roll: ")); Serial.print(xAxis); 
      //Serial.print(F(" - Pitch: ")); Serial.print(yAxis); 
      //Serial.print(F(" - Yaw: ")); Serial.print(zAxis); 
      //Serial.print(F(" - varXOffsetFinal: ")); Serial.print(varXOffsetFinal); 
      //Serial.print(F(" - varYOffsetFinal: ")); Serial.print(varYOffsetFinal); 
      //Serial.print(F(" - varXcorregirFinal: ")); Serial.print(varXcorregirFinal); 
      //Serial.print(F(" - varYcorregirFinal: ")); Serial.println(varYcorregirFinal); 
      // Lo anterior es equivalente a: 
      //RTMath::displayRollPitchYaw(" - Pose:", (RTVector3&)fusion.getFusionPose()); // fused output
//##################
      //Serial.print(F(" - Temperature: ")); Serial.print(temperature, 2); Serial.print(F(" deg C"));// Display 2 decimal places 
      //Serial.print(F(" - Pressure: ")); Serial.print(pressure, 0); Serial.print(F(" Pa"));// Whole number only. 
      //Serial.print(F(" - Ralated Atmosphere: ")); Serial.print(atm, 4); // Display 4 decimal places
      //Serial.print(F(" - Altitude: ")); Serial.print(altitude, 2);Serial.println(F(" m")); // Display 2 decimal places
//##################
      //Serial.println();
    }
  }
}
void corregirAcelerometer() {
  // Datos: Px', Py', Pz' 
  // Genera la secuencia de pasos. Esta función diferencia y sincroniza cada brazo y pierna contrarios y contempla su secuencia de movimientos.
  //Serial.println("******  Pasa - caminarPasoRegular()--------");
  int indpaso1;
    
  //for (indpaso1 = 1; indpaso1 <= 2000; indpaso1  += 1) { // SE PONDRÁ A 1 CUANDO CAMINE DE FORMA CONSTANTE YA QUE SE REPETIRÁ MEDIANTE loop().
    delay(tiempoPausaDivisorMovimientos); // Una espera entre porciones de cada movimiento. 
    // Movimiento 1 - El robot se inclina hacia la izquierda o derecha para equilibrar su centro de gravedad  
    Serial.println(F("\n  El robot se inclina hacia la izquierda para equilibrar su centro de gravedad sobre la pierna izquierda - Movimiento 1"));
    puntoFinalAbsoluto(Long5i, Long6i, Long7i, piernaN1); 
    puntoFinalAbsoluto(Long5i, Long6i, Long7i, piernaN2); 
    ejecutarParalelo();
    delay(tiempoEntreMovimientos); 

    // Movimiento 2 - El robot se inclina hacia adelante o atrás para equilibrar su centro de gravedad 
    Serial.println(F("\n  El robot se inclina hacia la derecha para equilibrar su centro de gravedad sobre la pierna derecha - Movimiento 13"));
    puntoFinalAbsoluto(Long5i, Long6i, Long7i, piernaN1); 
    puntoFinalAbsoluto(Long5i, Long6i, Long7i, piernaN2); 
    ejecutarParalelo();
    delay(tiempoEntreMovimientos); 

  //} 

return; // Punto de verificación de posición y par de los motores
}

void setup() {
  Serial.begin(115200);
  servos1.begin(); // Start the servo shield // Adafruit
  servos2.begin(); 
  servos1.reset(); // Start the servo shield // Adafruit
  servos2.reset(); 
  servos1.setPWMFreq(60); // Frecuecia PWM (modulación por ancho de pulsos) de 60Hz o T=16,66ms // Adafruit
  servos2.setPWMFreq(60); 
  delay(100);
  setServos01234();
  delay(2000);

  // MPU9250
  int errcode;
  Wire.begin();
  imu = RTIMU::createIMU(&settings); // create the imu object
  Serial.print(F("\nArduinoIMU10 starting using IMU ")); Serial.print(imu->IMUName());
  if ((errcode = imu->IMUInit()) < 0) {
      Serial.print(F("Failed to init IMU: ")); Serial.println(errcode);
  }
  if (imu->getCalibrationValid())
      Serial.println(F("Using compass calibration"));
  else
      Serial.println(F("No valid compass calibration data"));
  lastDisplay = lastRate = millis();
  sampleCount = 0;
  // Slerp power controls the fusion and can be between 0 and 1
  // 0 means that only gyros are used, 1 means that only accels/compass are used
  // In-between gives the fusion mix.
  fusion.setSlerpPower(0.02);
  // Use of sensors in the fusion algorithm can be controlled here
  // Change any of these to false to disable that sensor
  fusion.setGyroEnable(true);
  fusion.setAccelEnable(true);
  fusion.setCompassEnable(true);
//##################
  bmp280.init(); // Inicializa barómetro, ...
//##################

    //puntoFinalAbsoluto(5, -2, -16, 3); //Datos: Px', Py', Pz', brazo // Pz' máxima = 190mm // Para probarlo se descomenta uno de estos dos puntos 
    //puntoFinalAbsoluto(7, 0, -25, 1); //Datos: Px', Py', Pz', pierna //Px>0 // Pz' máxima = 257mm
    //delay(100);
    //puntoFinalAbsoluto(5, -2, -16, 4); //Datos: Px', Py', Pz', brazo //Px>0 // Para probarlo se descomenta uno de estos dos puntos 
    //puntoFinalAbsoluto(-7, 0, -25, 2); //Datos: Px', Py', Pz', pierna //Px>0 // Pz' máxima = 257mm
    //ejecutarParalelo(); // Para probarlo se descomenta para que ejecute las órdenes puntoFinalAbsoluto() en paralelo. 

  // Para caminar
  //////caminarPasoRegular(); // Función para caminar 

/*    */
  // Para equilibrarse
  Serial.println(F("\n  El robot se agacha hasta su posición de inicio de caminar - Movimiento 1"));
  puntoFinalAbsoluto(Long5i, Long6i, Long7i, piernaN1); 
  puntoFinalAbsoluto(Long5i, Long6i, Long7i, piernaN2); 
  ejecutarParalelo();
  delay(tiempoEntreMovimientos); 

}
void loop() {
acelerometer(); //Mide valores de alabeo/inclinación y cabeceo y las muestra //////
  //puntoFinalAbsoluto(); 
  //ejecutarParalelo(); //Se ejecuta siempre que haya avance con fracciones y no un posicionamiento directo
  //caminarPasoRegular(); 
}
