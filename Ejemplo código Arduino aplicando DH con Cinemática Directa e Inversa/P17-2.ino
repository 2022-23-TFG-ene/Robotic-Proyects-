// Nueva arquitectura de 4 brazos con 3 motores – Caminar sobre trayectoria regular usando Cinemática Inversa - caminarPasoRegular
// Se baja antes el motor 2 en cada brazo para alcanzar la vertical con cada extremo.
// Luego se baja ese extremo al suelo moviendo los motores 2 y 3.
// Se van a subir peldaños. Se hace modificando subirPasoRegular()
// Dada la longitud del cuerpo y de cada paso, una vez subidos los brazos delanteros se darán varios pasos anes de subir los brazos traseros.
// Cada brazo dependerá de una única tarjeta de expansión para mejorar el rendimiento de la intensidad aplicada.
// Los movimientos de un único brazo se van a hacer sin reforzar la posición de los otros brazos para no debilitar su par de los motores.
// Se añade un rabo para que parezca un perro 
#include <Wire.h> // Adafruit
#include <Adafruit_PWMServoDriver.h> // Adafruit
// Se usarán 4 tarjetas Adafruit para dar soporte a los 3 motores de cada tarjeta y mantener las mejores condiciones de voltaje e intensidad en cada motor.
Adafruit_PWMServoDriver servos1 = Adafruit_PWMServoDriver(0x44); // Controla el brazo delantero izquierdo
Adafruit_PWMServoDriver servos2 = Adafruit_PWMServoDriver(0x42); // Controla el brazo delantero derecho
Adafruit_PWMServoDriver servos3 = Adafruit_PWMServoDriver(0x50); // Controla el brazo trasero izquierdo
Adafruit_PWMServoDriver servos4 = Adafruit_PWMServoDriver(0x40); // Controla el brazo trasero derecho
// Variables que guardarán las asignaciones de los grados en cada comando servos1/2/3/4.setPWM().
//double limMin[] =            {200, 130, 130, 195, 200, 170,   190, 180, 150, 205, 150, 160}; // Límites mínimos en pulsos de cada motor. Por debajo se daña o da vueltas sin parar.
double limMin[] =            {200, 130, 130, 195, 200, 170,   190, 180, 130, 205, 150, 160}; // Límites mínimos en pulsos de cada motor. Por debajo se daña o da vueltas sin parar.
double limMax[] =            {495, 495, 535, 495, 575, 666,   495, 535, 666, 520, 485, 666}; // Límites máximos en pulsos de cada motor. Por encima se daña o da vueltas sin parar.
double msec0[] =             {335, 335, 335, 355, 360, 365,   345, 335, 335, 345, 335, 355}; // Pulsos de cada motor equivalente a "0" grados.
// Los motores 1 y 3 están montados al revés. Pero 1 y 2 están montados iguales. Y 3 y 4 también por su parte.
//double msecMin[] =           {205, 200, 185, 225, 220, 220,   215, 195, 195, 215, 185, 205}; // Mínimos pulsos de cada motor. Es equivalente al array msecMin[].
double msecMin[] =           {205, 200, 185, 225, 220, 220,   215, 185, 205, 215, 235, 205}; // Mínimos pulsos de cada motor. Es equivalente al array msecMin[].
//double msecMax[] =           {475, 475, 465, 490, 505, 515,   475, 495, 485, 490, 475, 500}; // Máximos pulsos de cada motor. Es equivalente al array msecMax[].
double msecMax[] =           {475, 475, 465, 490, 505, 515,   475, 495, 475, 490, 475, 500}; // Máximos pulsos de cada motor. Es equivalente al array msecMax[].
double angMin[] =            {-90, -90, -90, -90,  90,  90,   -90, -90, -90, -90,  90,  90}; // Mínimos ángulos de cada motor 
double angMax[] =            { 90,  90,  90,  90, -90, -90,    90,  90,  90,  90, -90, -90}; // Máximos ángulos de cada motor
double posReposoDePie[] =    {335, 335, 335, 355, 360, 365,   345, 335, 335, 345, 335, 355}; // Pulsos de cada motor para reposo (de pie).
double posPasoAlto[] =       {335, 235, 170, 355, 475, 540,   335, 225, 170, 355, 455, 540}; // Pulsos de cada motor para el paso alto.
double posPasoAdelantado[] = {335, 285, 285, 355, 405, 405,   335, 285, 285, 355, 385, 405}; // Pulsos de cada motor para el paso adelantado.
double posPasoBajo[] =       {335, 335, 335, 355, 360, 365,   335, 335, 335, 355, 335, 355}; // Pulsos de cada motor para el paso bajo.
double posPasoRetrasado[] =  {335, 385, 385, 355, 285, 315,   335, 385, 390, 355, 285, 305}; // Pulsos de cada motor para el paso retrasado.
double msec0rabo[] =  {500}; // Pulsos de la posición de reposo del motor del rabo.

int msecMotor[12]; // Pulsos de cada posición de cada motor de cada brazo en cada momento del programa - 205 - 495 - 345
double angMotor[12]; // Ángulo de cada posición de cada motor de cada brazo en cada momento del programa
int tareasConcurrentes[6]; // Será "1" cuando se deba ejecutar el movimiento de cada brazo de forma paralela. Una fracción cada vez.
double fraccion[12]; // Contiene el valor calculado de cada fracción para cada motor de cada brazo en movimiento paralelo.
//int iteracion = 0; // Número de iteraciones realizadas sobre el total de "divisor".
  
// Fracciones de cada pequeño movimiento. Por encima de 10 dará problemas porque no podrá dividir el dato en movimientos perceptibles y las unidades de 
// pulsos son enteras y no decimales.
double divisor = 10; // 20 - Utilizado para crear las fracciones de movimiento y que debe ser el número de iteraciones de pequeños movimientos
// LA, LH, L1 y L2 - números en centímetros
double LA = 40; // 18.5; // Largo de la primera sección del brazo que resulta ser la altura del robot en parada y de pie
double LH = 0; //11.2; // Largo de la segunda sección
double L1 = 11.5; //10.5; // Largo de la tercera sección del brazo
double L2 = 17.5;//13.9; //4.3; // Largo de la cuarta sección (4cm más el pie a colocar)
double Pi = 3.14159; // Valor de pi
double distanciaPasoMaximo = 6; // 7; // Distancia máxima que puede ser alcanzada en un paso no forzado: (L1+L2)*0,4
double cabeceo = 4; // distancia a adelantar los brazos para evitar el cabeceo al mover los brazos hacia atrás al mismo tiempo.
double distanciaBrazoMaximo = 25 ;//30; // Distancia máxima que puede ser alcanzada con el brazo: (L1+L2)*0,8
double distanciaMaximaRectaParalela = 12; // Máxima distancia del cuerpo a la recta que sigue el paso en paralelo al sentido de movimiento: (LH+L1)*0,9 = 13
double alturaLim = 1; // Límite de altura sobre el robot a partir del cual se utilizará también el segundo motor (ω) por ser un punto no alcanzable. 
int cambioModelo = 0; // Se usa para que se resuelva mediante ω, con valor "1", al ser por debajo del robot.
double inversaOmegaRadianes = 0; // Se inicializa ω suponiendo que no se vaya a usar por no superar θ el límite.
double titaGrados; double titasec; // Pulsos de movimiento aplicados al primer motor (θ). Dato de destino.
double omegaGrados; double omegasec; // Pulsos de movimiento aplicados al segundo motor (ω). Dato de destino.
double tita1Grados; double tita1sec; // Pulsos de movimiento aplicados al tercer motor (θ1). Dato de destino.
double tita2Grados; double tita2sec; // Pulsos de movimiento aplicados al cuarto motor (θ2). Dato de destino.
//double PxPrima = 0; // Componente x destino. Para caminar se irá calculado.
// PxPrima - Componente x o separación desde el cuerpo en paralelo al movimiento en 4 brazos con 3 motores.
double PxPrima = distanciaPasoMaximo; // Componente x destino del paso. Para caminar se irá calculado.
// PyPrima = distanciaMaximaRectaParalela; // Componente y de la altura en la primera parte del paso en 4 brazos con 3 motores.
//double PyPrima = 0; // Componente y destino. Para caminar se irá calculado pero en caminarPasoRegular() será "distanciaMaximaRectaParalela"
double PyPrima = 23; // Altura de la base del robot.
double desfaseY = 3; // Altura de la primera fase de cada paso para bajar el extremo del brazo en vertical. 
double PzPrima = 4; // Desplazamiento lateral para modificar la localización lateral del centro de gravedad.
double tiempoPausaDivisorMovimientos = 18; // 2 - Tiempo de pausa de los motores para moverse en paralelo
double multiplicadorDivisorTiempoPausaDivisorMovimientos = 2; // 12 - Tiempo que baja y sube la velocidad de movimiento para que no cabecear y caerse al adelantar y retrasar el cuerpo.
double tiempoEntreMovimientos = 50; // 5 - Tiempo entre movimientos de los distintos brazos para no producirse intersecciones de intervalos de tiempos al moverse en paralelo
double alturaEscalon = 10; // Altura del escalón
double numPasosIntermediosEscalera = 2; // Número de pasos inermedios tras subir los brazos delanteros y antes de subir los brazos traseros.
///Límites:   if ((pos20 > 900) and (pos20 < 3000) and (pos21 > 450) and (pos21 < 2700) and (pos22 > 600) and (pos22 < 2400) and (pos23 > 0) and (pos23 < 3000) and (datoModulo < 28))
//------------
// Funciones comunespara cinemática directa e inversa
double radianesAGrados(double radianes) { // Pasa ángulos en radianes a grados – Datos: Radianes
  //Serial.println("***  Pasa - radianesAGrados()--------"); 
  //Serial.print("radianesAGrados()"); Serial.print(" - Grados= "); Serial.println(radianes * (180.0 / Pi));
  return radianes * (180.0 / Pi);
}
double gradosARadianes(double& grados) { // Pasa ángulos en radianes a grados – Datos: Radianes
  //Serial.println("***  Pasa - gradosARadianes()--------"); 
  //Serial.print("radianesAGrados()"); Serial.print(" - Grados= "); Serial.println(grados * Pi / 180.0);
  return grados * Pi / 180.0;
}
// Funciones de cinemática inversa
double inversaPz(double A, double B) { // Datos: Pz', LA
  //Serial.println("***  Pasa - inversaPz()--------"); 
  double div = (A - abs(B));
  return div;
}
double inversaOmega3Motores(double A, double B, double C, double D, double& E) { // Datos: Px', Py', Pz', LA, θ
  //Serial.println("***  Pasa - inversaOmega3Motores()--------"); 
  // Por tener 3 motores sólo con "ω" se podrán alcanzar los puntos fuera de la línea de avance del eje X.
  double div = 0; // Se calcula ω con Py' y Pz'.
  if (B!=0) {
    div = (C/B); // Se calcula ω con Py' y Pz'.
   }else{
    div = 0;
  } 
  //Serial.print("inversaOmega3Motores()= "); Serial.print("ω (radianes)= "); Serial.println(atan(div));
  return atan(div);
}
double inversaModuloPcuadradoSimpleOmega3Motores(double A, double B, double C, double D, double E, double& F, double& G) { 
  // Datos: Px' , Py' , Pz', LH, LA, θ, ω - Modelo basado en ω - Módulo (x',z') para calcular la base de la gráfica de cálculos
  //Serial.println("***  Pasa - inversaModuloPcuadradoSimpleOmega3Motores()--------"); 
  double div = (sqrt((A*A)+(B*B)));
  //Serial.print("inversaModuloPcuadradoSimpleOmega3Motores()"); Serial.print(" - div= "); Serial.println(div);
  return div;
}
double inversaModuloPcuadradoCompletoOmega3Motores(double A, double B, double C, double D, double E, double& F, double G) { 
  //Serial.println("***  Pasa - inversaModuloPcuadradoCompletoOmega3Motores()--------"); 
  // Datos: Px' , Py' , Pz', LH, LA, θ, ω - Modelo basado en ω - Módulo (x',z') para calcular la base de la gráfica de cálculos
  double var18 = inversaModuloPcuadradoSimpleOmega3Motores(A, B, C, D, E, F, G);
  double div = (var18*var18 + (C*C));
  //Serial.print("inversaModuloPcuadradoCompletoOmega3Motores()"); Serial.print(" - div= "); Serial.println(div);
  return div;
} 
double inversaFi3Motores(double A, double B, double C, double D, double E, double F, double G, double& H, double& I) { 
  // Datos: Px' , Py', Pz', L1, L2, LH, LA, θ, ω
  //Serial.println("***  Pasa - inversaFi3Motores()--------"); 
  double var12;
  var12 = inversaModuloPcuadradoCompletoOmega3Motores(A, B, C, F, G, H, I);
  double div = (((D * D) + var12 - (E * E)) / abs(2 * D * sqrt(var12)));
  if (div < -1.0) // div debe estar entre -1 y 1. En caso de no ser así se estima que será por fallo de cálculo y se pone -1 o 1. 
    div = -1.0;
  if (div > 1.0)
    div = 1.0;
  //Serial.print("inversaFi3Motores() -"); Serial.print(" var12= "); Serial.print(var12);
  //Serial.print(" div= "); Serial.print(div);Serial.print(" acos(div)= "); Serial.println(acos(div));
  return acos(div);
}
double inversaGamma3Motores(double A, double B, double C, double D, double E, double F, double G, double& H, double& I) { 
  // Datos: Px' , Py', Pz', L1, L2, LH, LA, θ, ω
  //Serial.println("***  Pasa - inversaGamma3Motores()--------"); 
  double var17 = inversaModuloPcuadradoSimpleOmega3Motores(A, B, C, F, G, H, I); // Sólo Px' y Py' dado que γ está en el plano XY al ser la proyección.
  double div = 0;
  if (abs(var17)!=0) {
    div = (abs(acos(abs(B) / abs(var17)))); // Al ser la proyección sobre el planoXY sólo se considera Py' y Px'y' para calcular γ
  //Serial.print("inversaGamma3Motores() -"); Serial.print(" div= "); Serial.println(div);
   }else{
    div = 0;
  }
  //Serial.print("inversaGamma3Motores() -"); Serial.print(" div= "); Serial.println(div);
  return div;
}
double inversaTita13Motores(double A, double B, double C, double D, double E, double F, double G, double& H, double& I) { 
  // Datos: Px', Py', Pz', L1, L2, LH, LA, θ, ω
  double div = 0;
  double var15 = inversaGamma3Motores(A, B, C, D, E, F, G, H, I);
  double var16 = inversaFi3Motores(A, B, C, D, E, F, G, H, I);
  //********
    //div = (3.1416 - abs(atan((C - G) / var1)) - abs(var2)); // Cuadrante totalmente superior izquierdo. θ1 = γ-Real - φ  (θ1<0) 
  if ((A < 0) and (abs(var15) >= abs(var16))) {
    // Cuadrante totalmente superior (|γ| >= |φ|). Zona tras el robot. θ1 = (|γ| - |φ|) (θ1 > 0 si L2 corta "X" y θ1 < 0 si L2 está totalmente por debajo de "X")
    div = (abs(var15) - abs(var16)); 
    Serial.println(F("      ***θ1-Caso 1*** (A < 0) and (abs(var15) >= abs(var16))")); 
  } else if ((A < 0) and (abs(var15) < abs(var16))) { // ... (puntoInalcanzableConTita[brazoN-1]: 0)
    // Cuadrante superior e inferior (|γ| < |φ|). Zona tras el robot pero con el extremo de L1 por delante de x=0.  θ1 = (-|γ| + |φ|) (θ1 > 0)
    div = (abs(var15) - abs(var16)); // Cuadrante inferior. θ1 = (|γ| - |φ|) (θ1 > 0)    
    Serial.println(F("      ***θ1-Caso 2*** (A < 0) and (abs(var15) < abs(var16))")); 
  } else if (A >= 0) { //inversaPz = Pz'-LA por ser θ=0 ... (puntoInalcanzableConTita[brazoN-1]: 1) ... θ1(-)= - (3.1416/2)-|γ| + |φ|
    // Cuadrante totalmente inferior. Zona por delante del robot.  θ1 = -|γ| - |φ|  (θ1> 0)
     div = (-abs(var15) - abs(var16)); //3.1416/2 + 
    Serial.println(F("      ***θ1-Caso 3*** (A >= 0)")); 
  }else{ // Punto inalcanzable
    Serial.println(F("      ***θ1-Caso 4 - No desarrollado*** "));
  }
  //Serial.print("****************** abs(inversaGamma3Motores)(radianes)="); Serial.print(var15); Serial.print(" - (grados)="); Serial.println(radianesAGrados(var15));
  //Serial.print(" - abs(inversaFi3Motores) (radianes)="); Serial.print(var16); Serial.print(" - (grados)="); Serial.println(radianesAGrados(var16));
  //Serial.print(" - inversaTita13Motores (radianes)="); Serial.print(div); Serial.print(" - (grados)="); Serial.println(radianesAGrados(div));
  return div;
}
double inversaTita23Motores(double A, double B, double C, double D, double E, double F, double G, double& H, double& I, int J) { 
  //Serial.println("***  Pasa - inversaTita23Motores()--------"); 
  // Datos: Px' , Py', Pz', L1, L2, LH, LA, θ, ω, brazo
  double var11 = inversaModuloPcuadradoCompletoOmega3Motores(A, B, C, F, G, H, I);  // Datos: Px', Py', Pz', LH , LA, θ, ω
  double div = ((var11 - (D * D) - (E * E)) / abs(2 * D * E));
  //Serial.print("--- inversaModuloPcuadradoSimpleOmega3Motores="); Serial.print(" var11="); Serial.print(var11);
  //Serial.print(" - var11 - (D * D) - (E * E)="); Serial.print(var11 - (D * D) - (E * E));
  //Serial.print(" - abs(2 * D * E)="); Serial.print(abs(2 * D * E));
  //Serial.print(" - div="); Serial.print(div); Serial.print(" - acos(div)="); Serial.println(acos(div));
  if (div>1) {
    div = 1; 
    //Serial.print("**** HAY QUE REVISAR inversaTita23Motores() PORQUE NO EXISTE cos(div) CON div>1"); 
  }else if (div<-1) {
    div = -1;
    //Serial.print("**** Hay que revisar inversaTita23Motores() porque no existe el cos(div) con div<-1"); 
  }
  return acos(div);
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

void prepararMovimientoDeUnBrazo(int brazoN, double B, double C, double D, double E, double F) {
  // Datos = A, B, C, D, E, F = brazoN, titasec, Px' (o possig), omegasec, tita1sec, tita2sec 
  // Prepara los movimientos de cada brazo. Controla cada motor sea cual sea el brazo a través de la fracción a ejecutar en cada instante.
  // El segundo motor no se mueve dado que sólo se avanza con los motores de avanzar (motor 1) y de subir (motores 3 y 4)
  // Se moverá sólo en el caso de que no sea un punto alcanzable y deba girar el brazo por encima del cuerpo.
  // prepararMovimientoDeUnBrazo(brazoN, titasec, Long5, omegasec, tita1sec, tita2sec); 
  //Serial.println("***  Pasa - prepararMovimientoDeUnBrazo()--------"); 
  int z = ((brazoN-1)*3);
  if ((msecMotor[z] > limMin[z]) and (msecMotor[z] < limMax[z]) and (msecMotor[z+1] > limMin[z+1]) and (msecMotor[z+1] < limMax[z+1]) and (msecMotor[z+2] > limMin[z+2]) and (msecMotor[z+2] < limMax[z+2]) ) { // and (datoModulo < 50)     
    //Serial.print("* msecMotor[z]: "); Serial.print(msecMotor[z]);Serial.print(" - msecMotor[z+1]: "); Serial.print(msecMotor[z+1]);Serial.print(" - msecMotor[z+2]: "); Serial.print(msecMotor[z+2]); // Serial.print(" - datoModulo: "); Serial.println(datoModulo);
    //angMotor[z+1] = map(msec0[z+1], msecMin[z+1], msecMax[z+1], angMin[z+1], angMax[z+1]);  // No sirve para nada salvo para guardar los valores.
    fraccion[z] = (D - msecMotor[z]) / divisor; // omegasec
    fraccion[z+1] = (E - msecMotor[z+1]) / divisor; // tita1sec
    fraccion[z+2] = (F - msecMotor[z+2]) / divisor; // tita2sec
    tareasConcurrentes[brazoN-1] = 1;
  }else{
    Serial.println(F("***  Fuera de límites------------------------------------------------------"));    
  }
  //return; 
} 
void ejecutarParalelo(double A) { // Datos: Px' - Esta variable no se usa para nada
  // Ejecuta los movimientos de cada brazo en paralelo independientemente del número de estos y de cuáles sean.
  // Se recorren los 6 brazos verificando si se ha definido algún movimiento en cada uno de ellos (levantar, andar, avanzar, ...).
  // Por ejemplo en el caso de ejecutar pasos en un camino regular.
  // Está basado en modificaciones de destino con puntoFinalAbsoluto() y el preparado de los movimientos con prepararMovimientoDeUnBrazo() cuando camina
  //Serial.println("***  Pasa ------------------ ejecutarParalelo() ------------------");
  //Serial.print("   Pz'= "); Serial.print(A);
  int z;
  int zz;
  int zzz; 
  int zzzz; 
  for (int indpos = 1; indpos <= divisor; indpos  += 1) { // Movimientos divididos en un número de movimientos más pequeños (100)
    delay(tiempoPausaDivisorMovimientos);
    for (int i = 0; i <= 3; i  += 1) { // Se recorren los 4 brazos (0-3). 
      // Sólo se ejecutan movimientos en los que tareasConcurrentes[i]==1. El resto de brazos no refuerza la posición para no debilitar el par de los motores.
      if (tareasConcurrentes[i] == 1) {
        //Serial.print("indpos:"); Serial.print(indpos);Serial.print("i:"); Serial.print(i);Serial.print(" - tareasConcurrentes[i]:"); Serial.println(tareasConcurrentes[i]);
        z = (i*3);
        msecMotor[z] = msecMotor[z] + fraccion[z]; // SE SUPONE QUE ESTOS MOTORES YA ESTABAN EN SU POSICIÓN FINAL // Éste estaría repetido y quedaría modificado y por tanto mal si no fuera siempre la misma posición.
        msecMotor[z+1] = msecMotor[z+1] + fraccion[z+1]; 
        msecMotor[z+2] = msecMotor[z+2] + fraccion[z+2];
        angMotor[z] = map(msecMotor[z], msec0[z], msecMax[z], 0, angMax[z]);  // No sirve para nada salvo para guardar el dato.
        angMotor[z+1] = map(msecMotor[z+1], msec0[z+1], msecMax[z+1], 0, angMax[z+1]);  // No sirve para nada salvo para guardar el dato.
        angMotor[z+2] = map(msecMotor[z+2], msec0[z+2], msecMax[z+2], 0, angMax[z+2]);  // No sirve para nada salvo para guardar el dato.
        //Serial.print(" - z= "); Serial.print(z);
        //Serial.print(" - fraccion[z]= "); Serial.print(fraccion[z]);
        //Serial.print(" - fraccion[z+1]= "); Serial.print(fraccion[z+1]);
        //Serial.print(" - fraccion[z+2]= "); Serial.println(fraccion[z+2]);
        //Serial.print("   msecMotor[z]= "); Serial.print(msecMotor[z]);
        //Serial.print(" - msecMotor[z+1]= "); Serial.print(msecMotor[z+1]);
        //Serial.print(" - msecMotor[z+2]= "); Serial.println(msecMotor[z+2]);
        if (z < 3) { // Los 3 primeros servos se ejecutan sobre la primera tarjeta de expansión
          servos1.setPWM(z, 0, msecMotor[z]); 
          servos1.setPWM(z+1, 0, msecMotor[z+1]);
          servos1.setPWM(z+2, 0, msecMotor[z+2]);
        } else if ((z > 2) and (z < 6)) { // Los siguientes 3 servos trabajan sobre la segunda tarjeta de expansión
          zz = z - 3; // "zz" sólo se usa en el número de servo porque los arrays con los datos mantienen datos de los 12 motores
          servos2.setPWM(zz, 0, msecMotor[z]); 
          servos2.setPWM(zz+1, 0, msecMotor[z+1]);
          servos2.setPWM(zz+2, 0, msecMotor[z+2]);
        } else if ((z > 5) and (z < 9)) { // Los siguientes 3 servos trabajan sobre la tercera tarjeta de expansión
          zzz = z - 6; // "zzz" sólo se usa en el número de servo porque los arrays con los datos mantienen datos de los 12 motores
          servos3.setPWM(zzz, 0, msecMotor[z]); 
          servos3.setPWM(zzz+1, 0, msecMotor[z+1]);
          servos3.setPWM(zzz+2, 0, msecMotor[z+2]);
        } else { // Los otros 3 servos trabajan sobre la cuarta tarjeta de expansión
          zzzz = z - 9; // "zzzz" sólo se usa en el número de servo porque los arrays con los datos mantienen datos de los 12 motores
          servos4.setPWM(zzzz, 0, msecMotor[z]); 
          servos4.setPWM(zzzz+1, 0, msecMotor[z+1]);
          servos4.setPWM(zzzz+2, 0, msecMotor[z+2]);
        }
      }
      if (indpos == divisor) {
        //imprimirDatos2(i); //imprimirDatos2(i) - Se descomentará si se quiere seguir el valor de las variables.
        tareasConcurrentes[i] = 0; // La tarea se anula una vez terminado el movimiento y sus fracciones se ponen a "0"
      }
    }
  }
}
void levantarUnBrazo0(int brazoN) { // Datos: Brazo (brazo)
  // Para levantar un brazo al caminar, se levantará antes de moverlo en horizontal.
  // Levanta un brazo independientemente de cuál sea éste. Necesitará de la ejecución del movimiento en paralelo.
  // levantarUnBrazo0(brazoN); // Paso alto // Datos = A = Brazo
  // Se levanta el brazo con levantarUnBrazo0() y ejecutarParalelopulsos0() antes de desplazarse con levantarUnBrazo1() y ejecutarParalelopulsos1()
  //Serial.println("***  Pasa - levantarUnBrazo0()--------");
  int z = ((brazoN-1)*3);
  fraccion[z+1] = (posPasoAlto[z+1] - msecMotor[z+1]) / divisor;
  tareasConcurrentes[brazoN-1] = 1; // Se marca para ser movidos al mismo tiempo todos los brazos.
  //delay(10);
  // imprimirDatos2(brazoN-1) - Se descomentará si se quiere seguir el valor de las variables.
  imprimirDatos2(brazoN-1); // Se pone "brazoN-1" porque los identificadores de brazos empiezan en "1" mientras que los contadores de los bucles en otras partes del código empiezan en "0" 
  //return; 
} 
void ejecutarParalelopulsos0() { //  Está basado en modificaciones de destino con levantarUnBrazo0() cuando levanta brazos. 
  // Se recorren los 6 brazos verificando si se ha definido algún movimiento de levantar en cada uno de ellos.
  // El resto de movimientos los hace la siguiente función, ejecutarParalelopulsos0()
  // En este caso se ejecutan los movimientos de levantar cada brazo en paralelo independientemente del número de estos y de cuáles sean.
  ///////////////// PODRÍA EVITARSE USANDO LA FUNCIÓN () CON MOVIMIENTOS BASADOS EN msecMotor[]
  //Serial.println("***  Pasa - ejecutarParalelopulsos0()--------");
  int z;
  int zz;
  int zzz;
  int zzzz;
  for (int indpos = 1; indpos <= divisor; indpos  += 1) { // Movimientos divididos en 100 más pequeños
    delay(tiempoPausaDivisorMovimientos);
    for (int i = 0; i <= 3; i  += 1) { // Se recorren los 4 brazos (0-3)
     if (tareasConcurrentes[i] == 1) {
        z = (i*3);
        msecMotor[z+1] = msecMotor[z+1] + fraccion[z+1];
        if (z < 3) { // Los 3 primeros servos se ejecutan sobre la primera tarjeta de expansión
          servos1.setPWM(z+1, 0, msecMotor[z+1]);
        } else if ((z > 2) and (z < 6)) { // Los siguientes 3 servos trabajan sobre la segunda tarjeta de expansión
          zz = z - 3; // "zz" sólo se usa en el número de servo porque los arrays con los datos mantienen datos de los 12 motores
          servos2.setPWM(zz+1, 0, msecMotor[z+1]);
        } else if ((z > 5) and (z < 9)) { // Los siguientes 3 servos trabajan sobre la tercera tarjeta de expansión
          zzz = z - 6; // "zzz" sólo se usa en el número de servo porque los arrays con los datos mantienen datos de los 12 motores
          servos3.setPWM(zzz+1, 0, msecMotor[z+1]);
        } else { // Los otros 3 servos trabajan sobre la tercera tarjeta de expansión
          zzzz = z - 9; // "zzzz" sólo se usa en el número de servo porque los arrays con los datos mantienen datos de los 12 motores
          servos4.setPWM(zzzz+1, 0, msecMotor[z+1]);
        }
      }
      if (indpos == divisor) {
        //imprimirDatos2(i); // imprimirDatos2(i) - Se descomenta si se necesita seguir el valor de las variables.
        tareasConcurrentes[i] = 0; // La tarea se anula una vez terminado el movimiento y sus fracciones se ponen a "0"
      }
    }
  }
  //return; 
}
void levantarUnBrazo1(int brazoN) { // Datos: Brazo (brazo) 
  // z no sirve para nada porque ese motor no se mueve para este paso luego se podría evitar su cálculo.
  // Sin embargo se puede considerar de forma general en caso de necesitarse más adelante, a la hora de pasos irregulares.
  // Se desplaza el brazo con levantarUnBrazo1() y ejecutarParalelopulsos1() después de levantarse con levantarUnBrazo0() y ejecutarParalelopulsos0().
  //Serial.println("***  Pasa - levantarUnBrazo1()--------");
  int z = ((brazoN-1)*3);
  fraccion[z] = (posPasoAlto[z] - msecMotor[z]) / divisor;
  fraccion[z+1] = (posPasoAlto[z+1] - msecMotor[z+1]) / divisor;
  fraccion[z+2] = (posPasoAlto[z+2] - msecMotor[z+2]) / divisor;
  tareasConcurrentes[brazoN-1] = 1; // Se marca para ser movidos al mismo tiempo todos los brazos.
  //delay(10);
  // imprimirDatos2(brazoN-1) - Se descomenta si se necesita seguir el valor de las variables.
  imprimirDatos2(brazoN-1); // Se pone "brazoN-1" porque los identificadores de brazos empiezan en "1" mientras que los contadores de los bucles en otras partes del código empiezan en "0" 
  //return; 
}
void ejecutarParalelopulsos1() { //  Está basado en modificaciones de destino con levantarUnBrazo0() cuando levanta brazos
  // Se recorren los 6 brazos verificando si se ha definido algún movimiento de levantar en cada uno de ellos.
  // El resto de movimientos los hace la siguiente función, ejecutarParalelopulsos1()
  // En este caso se ejecutan los movimientos de levantar cada brazo en paralelo independientemente del número de estos y de cuáles sean.
 //Serial.println("***  Pasa - ejecutarParalelopulsos1()--------");
  int z;
  int zz;
  int zzz;
  int zzzz;
  for (int indpos = 1; indpos <= divisor; indpos  += 1) { // Movimientos divididos en 100 más pequeños
    delay(tiempoPausaDivisorMovimientos);
    for (int i = 0; i <= 3; i  += 1) { // Se recorren los 4 brazos (0-3)
     if (tareasConcurrentes[i] == 1) {
        z = (i*3);
        msecMotor[z] = msecMotor[z] + fraccion[z]; // Aquí sirve porque al no ser un punto inalcanzable, se moverá como el resto de los motores.
        msecMotor[z+1] = msecMotor[z+1] + fraccion[z+1];
        msecMotor[z+2] = msecMotor[z+2] + fraccion[z+2];
       if (z < 3) { // Los 3 primeros servos se ejecutan sobre la primera tarjeta de expansión
          servos1.setPWM(z+2, 0, msecMotor[z+2]); // A menor número más hacia arriba // Adafruit
          servos1.setPWM(z, 0, msecMotor[z]); // Estos primeros motores "z" no se moverán en pasos normales.
          servos1.setPWM(z+1, 0, msecMotor[z+1]); //
        } else if ((z > 2) and (z < 6)) { // Los siguientes 3 servos trabajan sobre la segunda tarjeta de expansión
          zz = z - 3; // "zz" sólo se usa en el número de servo porque los arrays con los datos mantienen datos de los 12 motores
          servos2.setPWM(zz+2, 0, msecMotor[z+2]);
          servos2.setPWM(zz, 0, msecMotor[z]);
          servos2.setPWM(zz+1, 0, msecMotor[z+1]);
        } else if ((z > 5) and (z < 9)) { // Los siguientes 3 servos trabajan sobre la tercera tarjeta de expansión
          zzz = z - 6; // "zzz" sólo se usa en el número de servo porque los arrays con los datos mantienen datos de los 12 motores
          servos3.setPWM(zzz+2, 0, msecMotor[z+2]);
          servos3.setPWM(zzz, 0, msecMotor[z]);
          servos3.setPWM(zzz+1, 0, msecMotor[z+1]);
        } else { // Los otros 3 servos trabajan sobre la cuarta tarjeta de expansión
          zzzz = z - 9; // "zzzz" sólo se usa en el número de servo porque los arrays con los datos mantienen datos de los 12 motores
          servos4.setPWM(zzzz+2, 0, msecMotor[z+2]);
          servos4.setPWM(zzzz, 0, msecMotor[z]);
          servos4.setPWM(zzzz+1, 0, msecMotor[z+1]);
         }
      }
      if (indpos == divisor) {
        //imprimirDatos2(i); // imprimirDatos2(i) - Se descomenta si se necesita seguir el valor de las variables.
        tareasConcurrentes[i] = 0; // La tarea se anula una vez terminado el movimiento y sus fracciones se ponen a "0"
      }
    }
  }
  //return; 
}

void puntoFinalAbsoluto(double Long1, double Long2, double Long3, double Long4, double Long5, double Long6, double Long7, double angulo1, int brazoN) {
  // Datos: LA, LH, L1, L2, Px', Py', Pz' (o possig), θ, Brazo
  // Mueve un brazo hasta un punto por cinemática inversa independientemente de cuál sea éste.
  // Para Cinemática inversa
  // Irá desde una posición predeterminada y prefijada de reposo a la posición que se determine mediante 
  // omegaGrados respecto a angMotor[z], tita1Grados respecto a angMotor[z+1] y tita2Grados respecto a angMotor[z+2].
  // Se verifica si es un punto no alcanzable. De serlo se fijará ω como el límite establecido y se usarán θ1 y θ2.
  //Serial.println("******  Pasa - puntoFinalAbsoluto()--------");
  rabo(); // Se mueve siempre que el robot esté activo  
 
  int z = ((brazoN-1) * 3); // Al ser 3 motores por brazo se multiplica por 3. Luego con 3 motores, el primer motor sería "ω".
  // En un caso básico: ω=0. Y para puntos alejados del cuerpo se usa y' y z'. Además se cambian los movimientos para andar sobre el eje y'.
  inversaOmegaRadianes = inversaOmega3Motores(Long5, Long6, Long7, Long1, titaGrados); // Datos: Px', Py', Pz', LA, θ
  if ((brazoN == 2) or (brazoN == 3)) {
    omegaGrados = -radianesAGrados(inversaOmegaRadianes);
  }else{
    omegaGrados = radianesAGrados(inversaOmegaRadianes);
  }
   //****** Arreglos de cuadrantes **
  //omegaGrados = omegaGrados + (180 - (2 * abs(omegaGrados))); // LO TENDRÉ QUE ARREGLAR CUANDO AÑADA Pz'
  //omegaGrados = -10;
  //omegaGrados = -omegaGrados;
//  omegaGrados = omegaGrados + (180 - (2 * abs(omegaGrados))); // LO TENDRÉ QUE ARREGLAR CUANDO AÑADA Pz'

  double tita1Radianes = inversaTita13Motores(Long5, Long6, Long7, Long3, Long4, Long2, Long1, titaGrados, inversaOmegaRadianes); 
  // Datos: Px', Py', Pz', L1, L2, LH, LA, θ, ω
  tita1Grados = radianesAGrados(tita1Radianes);
  double tita2Radianes = inversaTita23Motores(Long5, Long6, Long7, Long3, Long4, Long2, Long1, titaGrados, inversaOmegaRadianes, brazoN); // Datos: Px', Py', Pz', L1, L2, LH, LA, θ, ω, brazo
  tita2Grados = -radianesAGrados(tita2Radianes); // θ2 es siempre negativo por ser siempre codo arriba 
  //tita2Grados = -90;

  double fiRadianes = inversaFi3Motores(Long5, Long6, Long7, Long3, Long4, Long2, Long1, titaGrados, omegaGrados); 
  // Datos: Px' , Py', Pz', L1, L2, LH, LA, θ, ω
  double fiGrados = radianesAGrados(fiRadianes);
  double gammaRadianes = inversaGamma3Motores(Long5, Long6, Long7, Long3, Long4, Long2, Long1, titaGrados, omegaGrados); // Datos: Px' , Py', Pz', L1, L2, LH, LA, θ, ω
  double gammaGrados = radianesAGrados(gammaRadianes);  
  //omegaGrados = 10;
  omegasec = map(omegaGrados, angMin[z], angMax[z], msecMin[z], msecMax[z]); // omegasec // 4 BRAZOS
  //omegasec = map(omegaGrados, angMin[z], 0, msecMin[z], msec0[z]); // omegasec // 4 BRAZOS
  //tita1sec = map(tita1Grados, angMin[z+1], angMax[z+1], msecMin[z+1], msecMax[z+1]); // tita1sec // 4 BRAZOS
  //tita1Grados = -90;
  //tita2Grados = -90;
  tita1sec = map(tita1Grados, angMin[z+1], angMax[z+1], msecMin[z+1], msecMax[z+1]); // tita1sec // 4 BRAZOS
  // Por ser siempre codo arriba, el valor de θ2 es siempre negativo y de ser insuficiente se trata como un punto inalcanzable.
  tita2sec = map(tita2Grados, angMin[z+2], angMax[z+2], msecMin[z+2], msecMax[z+2]); // tita2sec // 4 BRAZOS
  // imprimirDatos1() - Se descomenta si se necesita seguir el valor de las variables.
  //imprimirDatos1(brazoN, puntoInalcanzableConTita[brazoN-1], alturaLim, titaLim, anguloAlturaTitaLim, Long1, Long2, Long3, Long4, Long5, Long6, Long7, Px, Py, Pz, titaPrimaGrados, fiGrados, gammaGrados, omegaGrados);
  //**************** Límites ********************
  double datoModulo = sqrt(inversaModuloPcuadradoCompletoOmega3Motores(Long5, Long6, Long7, Long2, Long1, angulo1, omegaGrados)); 
  //**** Desde aquí dependerá del brazo a utilizar puesto que tienen parámetros diferentes. Se usa la función 
  //Serial.println("/////////////// puntoFinalAbsoluto()-2-"); Serial.print("   z="); Serial.print(z);
  //Serial.print(" - "); Serial.print("brazoN="); Serial.print(brazoN); 
  //Serial.print(" - "); Serial.print("gammaGrados="); Serial.print(gammaGrados);
  //Serial.print(" - "); Serial.print("omegaGrados="); Serial.print(omegaGrados);
  //Serial.print(" - "); Serial.print("tita1Grados="); Serial.print(tita1Grados);
  //Serial.print(" - "); Serial.print("tita2Grados="); Serial.println(tita2Grados);
  //Serial.print(" - "); Serial.print("omegasec="); Serial.print(omegasec);
  //Serial.print(" - "); Serial.print("tita1sec="); Serial.print(tita1sec);
  //Serial.print(" - "); Serial.print("tita2sec="); Serial.println(tita2sec);
  prepararMovimientoDeUnBrazo(brazoN, titasec, Long5, omegasec, tita1sec, tita2sec); // Datos = A, B, C, D, E, F = brazo, titaGrados, Px', omegaGrados, tita1Grados, tita2Grados 
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
void subirPasoRegular(double Long1, double Long2, double Long3, double Long4, double Long5, double Long6, double Long7) {
  // Datos: LA, LH, L1, L2, Px', Py', Pz' 
  // Genera la secuencia de pasos. Esta función sí diferencia y sincroniza cada brazo y contempla su secuencia de movimientos.
  //Serial.println("******  Pasa - caminarPasoRegular()--------");
  // Desde la posición de reposo de pie va hacia reposo arriba en x pequeños avances.
  double avance; // Es la distancia a avanzar en cada paso en paralelo al movimiento sobre el eje Z.
  double alcance = sqrt((distanciaPasoMaximo * distanciaPasoMaximo) + (Long7 * Long7)); // Es el alcance estimado del brazo
  if (alcance > distanciaBrazoMaximo) { // (19.2) Si el alcance es superior se reduce hasta su alcance máximo.
    avance == sqrt((distanciaBrazoMaximo * distanciaBrazoMaximo) - (Long7 * Long7)); // (19.2 * 19.2 - 5 * 5 = 18.53) // (19.2 * 19.2 - 12 * 12 = 14.98)
  }else{
    avance = distanciaPasoMaximo; // (9.6) Es la posición más alejada en el eje X
  } 
  double desfaseX = avance * 0.7; // Desfase a retrasar o adelantar en cada movimiento para equilibrar el centro de gravedad. 
  double possig; // Longitud del paso
  int indpaso1;
  int brazoN;
  // No hace falta las posiciones iniciales porque ya viene de los datos de pos20, pos21 y pos22 de reposo en alto.
  possig = avance; // Avance
  // Inicialmente se adelantan los brazos 1 (Delantero Izquierdo), 4 (Medio Derecho) y 5 (Trasero Izquierdo)
  // Y se retrasan los brazos 2 (Delantero Derecho), 3 (Medio Izquierdo) y 6 (Trasero Derecho) 
  for (indpaso1 = 1; indpaso1 <= 3; indpaso1  += 1) { // SE PONDRÁ A 1 EN SU MOMENTO PORQUE LO REPETIRÁ loop().
    delay(tiempoPausaDivisorMovimientos); // Una espera entre porciones de cada movimiento.

//--------------- Paso 1 
    // Se adelantan los 4 brazos apoyados en el suelo un cierto desfase igual al 70% del paso (desfaseX) para equilibrar el robot 
    // al levantar un brazo delantero (X). También se desplazan (Long7) hacia el brazo 2 (Z) para que su centro de gravedad se vaya
    // hacia el lado del brazo 2. Se suma/resta "cabeceo" para que al adelantar el cuerpo la inercia no haga cabecear al robot.
    // Primero se suben y adelantan los brazos delanteros
    puntoFinalAbsoluto(LA, LH, L1, L2, desfaseX+cabeceo, Long6, Long7, 0, 1); //Datos: LA, LH, L1, L2, Px', Py', Pz', θ, brazo 
    puntoFinalAbsoluto(LA, LH, L1, L2, desfaseX+cabeceo, Long6, -Long7, 0, 2); //Datos: LA, LH, L1, L2, Px', Py', Pz', θ, brazo
    puntoFinalAbsoluto(LA, LH, L1, L2, desfaseX, Long6, Long7, 0, 3); //Datos: LA, LH, L1, L2, Px', Py', Pz', θ, brazo
    puntoFinalAbsoluto(LA, LH, L1, L2, desfaseX, Long6, -Long7, 0, 4); //Datos: LA, LH, L1, L2, Px', Py', Pz', θ, brazo
    ejecutarParalelo(possig); // Datos: Px' //Se ejecuta siempre que haya avance con fracciones y no un posicionamiento directo
    delay(tiempoEntreMovimientos); // Una espera adecuada podía ser de 100...

    // Brazo Delantero Izquierdo
    brazoN = 1;
///    int z = (brazoN - 1) * 3;
    Serial.println(F("   Se levanta el brazo 1 (Delantero Izquierdo) para adelantarse a la posición del robot (Movimiento 1)"));
    //Serial.println("");Serial.println("-- Delantero Izquierdo y Medio Derecho");
    // Una vez arriba busca el primer punto, el más avanzado en 100 pequeños avances y se va moviendo hacia atrás en 100 pequeñs avances. 
    // Para todos los brazos de la izquierda se considera avance positivo y para los de la derecha, negativos.
    //Serial.println("caminarPasoRegular()-2-");
    //Serial.print("   possig= "); Serial.print(possig);
    //Serial.print(" - "); Serial.print("z= "); Serial.print(z);
    //Serial.print(" - "); Serial.print("angMotor[z]= "); Serial.print(angMotor[z]);
    //Serial.print(" - "); Serial.print("angMotor[z+1]= "); Serial.print(angMotor[z+1]);
    //Serial.print(" - "); Serial.print("angMotor[z+2]= "); Serial.println(angMotor[z+2]);
    // Ya tenemos X (0), Y (Long6) y Z (possig) para llamar a la función que calcula grados y mueve motores.
    // Una vez arriba busca el primer ángulo, el más alejado y se va moviendo hacia atrás en 100 pequeños avances. 
    // El movimiento del brazo hacia el punto final se divide en dos partes para mejorar el rendimiento de la fuerza de los motores.
    // Se hace así para que no ceda por escasa fuerza de los motores ya que el par debería ser más alto para poder ejecutar los movimientos.
    // Primero mueve el motor 2 hasta colocar el extremo del brazo sobre la vertical de la posición final.
    //Serial.print("**** caminarPasoRegular() - "); Serial.print("possig= "); Serial.print(possig);
    //Serial.print(" - Long6= "); Serial.print(Long6); Serial.print(" - Long7= "); Serial.println(Long7);
    
    puntoFinalAbsoluto(LA, LH, L1, L2, desfaseX+cabeceo, Long6-desfaseY-alturaEscalon, Long7, 0, brazoN); //Datos: LA, LH, L1, L2, Px', Py', Pz', θ, brazo
    ejecutarParalelo(0);
    delay(tiempoEntreMovimientos); // Una espera adecuada podía ser de 100...    
    // Primero levantará el extremo con el tercer motor y luego usará el resto de motores para no arrastrar el extremo.
    puntoFinalAbsoluto(LA, LH, L1, L2, possig+desfaseX+(desfaseX/2)+cabeceo, Long6-desfaseY-alturaEscalon, Long7, 0, brazoN); //Datos: LA, LH, L1, L2, Px', Py', Pz', θ, brazo
    ejecutarParalelo(0);
    delay(tiempoEntreMovimientos); // Una espera adecuada podía ser de 100...    
    // Ahora baja cada brazo moviendo los motores 2 y 3 con el extremo del brazo bajando en vertical.
    puntoFinalAbsoluto(LA, LH, L1, L2, possig+desfaseX+cabeceo, Long6-alturaEscalon, Long7, 0, brazoN); //Datos: LA, LH, L1, L2, Px', Py', Pz', θ, brazo
    ejecutarParalelo(0);
    delay(tiempoEntreMovimientos); // Una espera adecuada podía ser de 100...

    // Brazo Delantero Derecho
    brazoN = 2;
//    z = (brazoN - 1) * 3;
    //Serial.println("caminarPasoRegular()-2-");
    Serial.println(F("   Se levanta el brazo 2 (Delantero Derecho) para emparejarse con el brazo Delantero Izquierdo, adelantándose a la posición del robot (Movimiento 1)"));
    //Serial.println("");Serial.println("-- Delantero Derecho");
    // Una vez arriba busca el primer punto, el más avanzado en 100 pequeños avances y se va moviendo hacia atrás en 100 pequeñs avances. 
    // No hace falta las posiciones iniciales porque ya viene de los datos de pos20, pos21 y pos22 de reposo en alto.
    // Para todos los brazos de la izquierda se considera avance positivo y para los de la derecha, negativos.
    //Serial.println(""); Serial.print("possig= "); Serial.print(possig);
    //Serial.print(" - "); Serial.print("z= "); Serial.print(z);
    //Serial.print(" - "); Serial.print("angMotor[z]= "); Serial.println(angMotor[z]);
    // Ya tenemos X (possig), Y (Long6) y Z (0) para llamar a la función que calcula grados y mueve motores.
    // Una vez arriba busca el primer ángulo, el más alejado y se va moviendo hacia atrás en 100 pequeños avances. 
    // El movimiento del brazo hacia el punto final se divide en dos partes para mejorar el rendimiento de la fuerza de los motores.
    // Se hace así para que no ceda por escasa fuerza de los motores ya que el par debería ser más alto para poder ejecutar los movimientos.
    // Primero mueve el motor 2 hasta colocar el extremo del brazo sobre la vertical de la posición final.

    // Y se desplazan hacia el brazo 1 para que su centro de gravedad se vaya hacia el lado del brazo 1.
    puntoFinalAbsoluto(LA, LH, L1, L2, possig+desfaseX+cabeceo, Long6-alturaEscalon, -Long7, 0, 1); //Datos: LA, LH, L1, L2, Px', Py', Pz', θ, brazo 
    puntoFinalAbsoluto(LA, LH, L1, L2, desfaseX+cabeceo, Long6-alturaEscalon, Long7, 0, 2); //Datos: LA, LH, L1, L2, Px', Py', Pz', θ, brazo
    puntoFinalAbsoluto(LA, LH, L1, L2, desfaseX, Long6, -Long7, 0, 3); //Datos: LA, LH, L1, L2, Px', Py', Pz', θ, brazo
    puntoFinalAbsoluto(LA, LH, L1, L2, desfaseX, Long6, Long7, 0, 4); //Datos: LA, LH, L1, L2, Px', Py', Pz', θ, brazo
    ejecutarParalelo(possig); // Datos: Px' //Se ejecuta siempre que haya avance con fracciones y no un posicionamiento directo
    delay(tiempoEntreMovimientos); // Una espera adecuada podía ser de 100...

    puntoFinalAbsoluto(LA, LH, L1, L2, desfaseX+cabeceo, Long6-desfaseY-alturaEscalon, Long7, 0, brazoN); //Datos: LA, LH, L1, L2, Px', Py', Pz', θ, brazo
    ejecutarParalelo(0);
    delay(tiempoEntreMovimientos); // Una espera adecuada podía ser de 100...   
    // Primero levantará el extremo con el tercer motor y luego usará el resto de motores para no arrastrar el extremo.
    puntoFinalAbsoluto(LA, LH, L1, L2, possig+desfaseX+(desfaseX/2)+cabeceo, Long6-desfaseY-alturaEscalon, Long7, 0, brazoN); //Datos: LA, LH, L1, L2, Px', Py', Pz', θ, brazo
    ejecutarParalelo(0);
    delay(tiempoEntreMovimientos); // Una espera adecuada podía ser de 100...    
    // Ahora baja cada brazo moviendo los motores 2 y 3 con el extremo del brazo bajando en vertical.
    puntoFinalAbsoluto(LA, LH, L1, L2, possig+desfaseX+cabeceo, Long6-alturaEscalon, Long7, 0, brazoN); //Datos: LA, LH, L1, L2, Px', Py', Pz', θ, brazo
    ejecutarParalelo(0); // Px'
    delay(tiempoEntreMovimientos); // Una espera adecuada podía ser de 100...

    // Ahora se atrasan los 4 brazos apoyados en el suelo un cierto desfase igual a la mitad del paso para equilibrar el robot al levantar un brazo delantero.
    // Y se desplazan hacia el brazo 2 para que su centro de gravedad se vaya hacia el lado del brazo 2.
    tiempoPausaDivisorMovimientos = tiempoPausaDivisorMovimientos * multiplicadorDivisorTiempoPausaDivisorMovimientos; // Baja la velocidad de movimiento para que no cabecear y caerse.
    puntoFinalAbsoluto(LA, LH, L1, L2, -desfaseX+cabeceo, Long6-alturaEscalon, Long7, 0, 1); //Datos: LA, LH, L1, L2, Px', Py', Pz', θ, brazo 
    puntoFinalAbsoluto(LA, LH, L1, L2, -desfaseX+cabeceo, Long6-alturaEscalon, -Long7, 0, 2); //Datos: LA, LH, L1, L2, Px', Py', Pz', θ, brazo
    puntoFinalAbsoluto(LA, LH, L1, L2, -possig-desfaseX, Long6, Long7, 0, 3); //Datos: LA, LH, L1, L2, Px', Py', Pz', θ, brazo
    puntoFinalAbsoluto(LA, LH, L1, L2, -possig-desfaseX, Long6, -Long7, 0, 4); //Datos: LA, LH, L1, L2, Px', Py', Pz', θ, brazo
    ejecutarParalelo(0); // Datos: Px' //Se ejecuta siempre que haya avance con fracciones y no un posicionamiento directo
    tiempoPausaDivisorMovimientos = tiempoPausaDivisorMovimientos / multiplicadorDivisorTiempoPausaDivisorMovimientos; // Ahora lo sube
    delay(tiempoEntreMovimientos); // Una espera adecuada podía ser de 100...

    // Brazo Trasero Izquierdo
    // Ahora se adelantan sin subir los brazos traseros y delanteros durante al menos dos pasos para acercar los brazos a los peldaños que tienen delante.
    int indpaso2;
    for (indpaso2 = 1; indpaso2 <= numPasosIntermediosEscalera; indpaso2  += 1) { // SE PONDRÁ A 1 EN SU MOMENTO PORQUE LO REPETIRÁ loop().
      brazoN = 3;
///      z = (brazoN - 1) * 3;
      //Serial.println("caminarPasoRegular()-2-");
      Serial.println(F("   Se levanta el brazo 3 (Trasero Izquierdo) para emparejarse con los dos brazos delanteros (Movimiento 1)"));
      //Serial.println("");Serial.println("-- Delantero Derecho");
      // Una vez arriba busca el primer punto, el más avanzado en 100 pequeños avances y se va moviendo hacia atrás en 100 pequeñs avances. 
      // No hace falta las posiciones iniciales porque ya viene de los datos de pos20, pos21 y pos22 de reposo en alto.
      // Para todos los brazos de la izquierda se considera avance positivo y para los de la derecha, negativos.
      //Serial.println(""); Serial.print("possig= "); Serial.print(possig);
      //Serial.print(" - "); Serial.print("z= "); Serial.print(z);
      //Serial.print(" - "); Serial.print("angMotor[z]= "); Serial.println(angMotor[z]);
      // Ya tenemos X (possig), Y (Long6) y Z (0) para llamar a la función que calcula grados y mueve motores.
      // Una vez arriba busca el primer ángulo, el más alejado y se va moviendo hacia atrás en 100 pequeños avances. 
      // El movimiento del brazo hacia el punto final se divide en dos partes para mejorar el rendimiento de la fuerza de los motores.
      // Se hace así para que no ceda por escasa fuerza de los motores ya que el par debería ser más alto para poder ejecutar los movimientos.
      // Primero mueve el motor 2 hasta colocar el extremo del brazo sobre la vertical de la posición final.

      // Primero levantará el extremo con el tercer motor y luego usará el resto de motores para no arrastrar el extremo.
      puntoFinalAbsoluto(LA, LH, L1, L2, -possig-desfaseX, Long6-desfaseY, Long7, 0, brazoN); //Datos: LA, LH, L1, L2, Px', Py', Pz', θ, brazo
      ejecutarParalelo(0);
      delay(tiempoEntreMovimientos); // Una espera adecuada podía ser de 100...    
      // Se cambia la componente X para reajustar el paso
      //puntoFinalAbsoluto(LA, LH, L1, L2, 0, Long6-desfaseY-alturaEscalon, Long7, 0, brazoN); //Datos: LA, LH, L1, L2, Px', Py', Pz', θ, brazo
      puntoFinalAbsoluto(LA, LH, L1, L2, desfaseX/2, Long6-desfaseY, Long7, 0, brazoN); //Datos: LA, LH, L1, L2, Px', Py', Pz', θ, brazo
      ejecutarParalelo(0);
      delay(tiempoEntreMovimientos); // Una espera adecuada podía ser de 100...    
      // Ahora baja cada brazo moviendo los motores 2 y 3 con el extremo del brazo bajando en vertical.
      puntoFinalAbsoluto(LA, LH, L1, L2, -desfaseX, Long6, Long7, 0, brazoN); //Datos: LA, LH, L1, L2, Px', Py', Pz', θ, brazo
      ejecutarParalelo(0);
      delay(tiempoEntreMovimientos); // Una espera adecuada podía ser de 100...

      // Y se desplazan hacia el brazo 1 para que su centro de gravedad se vaya hacia el lado del brazo 1.
      puntoFinalAbsoluto(LA, LH, L1, L2, -desfaseX+cabeceo, Long6-alturaEscalon, -Long7, 0, 1); //Datos: LA, LH, L1, L2, Px', Py', Pz', θ, brazo 
      puntoFinalAbsoluto(LA, LH, L1, L2, -desfaseX+cabeceo, Long6-alturaEscalon, Long7, 0, 2); //Datos: LA, LH, L1, L2, Px', Py', Pz', θ, brazo
      puntoFinalAbsoluto(LA, LH, L1, L2, -desfaseX, Long6, -Long7, 0, 3); //Datos: LA, LH, L1, L2, Px', Py', Pz', θ, brazo
      puntoFinalAbsoluto(LA, LH, L1, L2, -possig-desfaseX, Long6, Long7, 0, 4); //Datos: LA, LH, L1, L2, Px', Py', Pz', θ, brazo
      ejecutarParalelo(0); // Datos: Px' //Se ejecuta siempre que haya avance con fracciones y no un posicionamiento directo
      delay(tiempoEntreMovimientos); // Una espera adecuada podía ser de 100...
 
      // Brazo Trasero Derecho
      brazoN = 4;
///      z = (brazoN - 1) * 3;
      //Serial.println("caminarPasoRegular()-2-");
      Serial.println(F("   Se levanta el brazo 4 (Trasero Derecho) y se reagrupa con los tres primeros (Movimiento 1)"));
      // Primero levantará el extremo con el tercer motor y luego usará el resto de motores para no arrastrar el extremo.
      puntoFinalAbsoluto(LA, LH, L1, L2, -possig-desfaseX, Long6-desfaseY, Long7, 0, brazoN); //Datos: LA, LH, L1, L2, Px', Py', Pz', θ, brazo
      ejecutarParalelo(0);
      delay(tiempoEntreMovimientos); // Una espera adecuada podía ser de 100...    
      puntoFinalAbsoluto(LA, LH, L1, L2, 0, Long6-desfaseY, Long7, 0, brazoN); //Datos: LA, LH, L1, L2, Px', Py', Pz', θ, brazo
      ejecutarParalelo(0);
      delay(tiempoEntreMovimientos); // Una espera adecuada podía ser de 100...    
      // Ahora baja cada brazo moviendo los motores 2 y 3 con el extremo del brazo bajando en vertical.
      puntoFinalAbsoluto(LA, LH, L1, L2, -desfaseX, Long6, Long7, 0, brazoN); //Datos: LA, LH, L1, L2, Px', Py', Pz', θ, brazo
      ejecutarParalelo(0);
      delay(tiempoEntreMovimientos); // Una espera adecuada podía ser de 100...

//return; // Punto de verificación de posición y par de los motores

      // Brazo Delantero Izquierdo
      brazoN = 1;
///      int z = (brazoN - 1) * 3;
      Serial.println(F("   Se levanta el brazo 1 (Delantero Izquierdo) para adelantarse a la posición del robot (Movimiento 2)"));
      //Serial.println("");Serial.println("-- Delantero Izquierdo y Medio Derecho");
      // Una vez arriba busca el primer punto, el más avanzado en 100 pequeños avances y se va moviendo hacia atrás en 100 pequeñs avances. 
      // Para todos los brazos de la izquierda se considera avance positivo y para los de la derecha, negativos.
      //Serial.println("caminarPasoRegular()-2-");
      //Serial.print("   possig= "); Serial.print(possig);
      //Serial.print(" - "); Serial.print("z= "); Serial.print(z);
      //Serial.print(" - "); Serial.print("angMotor[z]= "); Serial.print(angMotor[z]);
      //Serial.print(" - "); Serial.print("angMotor[z+1]= "); Serial.print(angMotor[z+1]);
      //Serial.print(" - "); Serial.print("angMotor[z+2]= "); Serial.println(angMotor[z+2]);
      // Ya tenemos X (0), Y (Long6) y Z (possig) para llamar a la función que calcula grados y mueve motores.
      // Una vez arriba busca el primer ángulo, el más alejado y se va moviendo hacia atrás en 100 pequeños avances. 
      // El movimiento del brazo hacia el punto final se divide en dos partes para mejorar el rendimiento de la fuerza de los motores.
      // Se hace así para que no ceda por escasa fuerza de los motores ya que el par debería ser más alto para poder ejecutar los movimientos.
      // Primero mueve el motor 2 hasta colocar el extremo del brazo sobre la vertical de la posición final.
      //Serial.print("**** caminarPasoRegular() - "); Serial.print("possig= "); Serial.print(possig);
      //Serial.print(" - Long6= "); Serial.print(Long6); Serial.print(" - Long7= "); Serial.println(Long7);
    
      puntoFinalAbsoluto(LA, LH, L1, L2, desfaseX+cabeceo, Long6-desfaseY-alturaEscalon, Long7, 0, brazoN); //Datos: LA, LH, L1, L2, Px', Py', Pz', θ, brazo
      ejecutarParalelo(0);
      delay(tiempoEntreMovimientos); // Una espera adecuada podía ser de 100...    
      // Primero levantará el extremo con el tercer motor y luego usará el resto de motores para no arrastrar el extremo.
      puntoFinalAbsoluto(LA, LH, L1, L2, possig+desfaseX+(desfaseX/2)+cabeceo, Long6-desfaseY-alturaEscalon, Long7, 0, brazoN); //Datos: LA, LH, L1, L2, Px', Py', Pz', θ, brazo
      ejecutarParalelo(0);
      delay(tiempoEntreMovimientos); // Una espera adecuada podía ser de 100...    
      // Ahora baja cada brazo moviendo los motores 2 y 3 con el extremo del brazo bajando en vertical.
      puntoFinalAbsoluto(LA, LH, L1, L2, possig+desfaseX+cabeceo, Long6-alturaEscalon, Long7, 0, brazoN); //Datos: LA, LH, L1, L2, Px', Py', Pz', θ, brazo
      ejecutarParalelo(0);
      delay(tiempoEntreMovimientos); // Una espera adecuada podía ser de 100...

      // Brazo Delantero Derecho
      brazoN = 2;
///      z = (brazoN - 1) * 3;
      //Serial.println("caminarPasoRegular()-2-");
      Serial.println(F("   Se levanta el brazo 2 (Delantero Derecho) para emparejarse con el brazo Delantero Izquierdo, adelantándose a la posición del robot (Movimiento 2)"));
      //Serial.println("");Serial.println("-- Delantero Derecho");
      // Una vez arriba busca el primer punto, el más avanzado en 100 pequeños avances y se va moviendo hacia atrás en 100 pequeñs avances. 
      // No hace falta las posiciones iniciales porque ya viene de los datos de pos20, pos21 y pos22 de reposo en alto.
      // Para todos los brazos de la izquierda se considera avance positivo y para los de la derecha, negativos.
      //Serial.println(""); Serial.print("possig= "); Serial.print(possig);
      //Serial.print(" - "); Serial.print("z= "); Serial.print(z);
      //Serial.print(" - "); Serial.print("angMotor[z]= "); Serial.println(angMotor[z]);
      // Ya tenemos X (possig), Y (Long6) y Z (0) para llamar a la función que calcula grados y mueve motores.
      // Una vez arriba busca el primer ángulo, el más alejado y se va moviendo hacia atrás en 100 pequeños avances. 
      // El movimiento del brazo hacia el punto final se divide en dos partes para mejorar el rendimiento de la fuerza de los motores.
      // Se hace así para que no ceda por escasa fuerza de los motores ya que el par debería ser más alto para poder ejecutar los movimientos.
      // Primero mueve el motor 2 hasta colocar el extremo del brazo sobre la vertical de la posición final.

      // Y se desplazan hacia el brazo 1 para que su centro de gravedad se vaya hacia el lado del brazo 1.
      puntoFinalAbsoluto(LA, LH, L1, L2, possig+desfaseX+cabeceo, Long6-alturaEscalon, -Long7, 0, 1); //Datos: LA, LH, L1, L2, Px', Py', Pz', θ, brazo 
      puntoFinalAbsoluto(LA, LH, L1, L2, desfaseX+cabeceo, Long6-alturaEscalon, Long7, 0, 2); //Datos: LA, LH, L1, L2, Px', Py', Pz', θ, brazo
      puntoFinalAbsoluto(LA, LH, L1, L2, desfaseX, Long6, -Long7, 0, 3); //Datos: LA, LH, L1, L2, Px', Py', Pz', θ, brazo
      puntoFinalAbsoluto(LA, LH, L1, L2, desfaseX, Long6, Long7, 0, 4); //Datos: LA, LH, L1, L2, Px', Py', Pz', θ, brazo
      ejecutarParalelo(possig); // Datos: Px' //Se ejecuta siempre que haya avance con fracciones y no un posicionamiento directo
      delay(tiempoEntreMovimientos); // Una espera adecuada podía ser de 100...

      puntoFinalAbsoluto(LA, LH, L1, L2, desfaseX+cabeceo, Long6-desfaseY-alturaEscalon, Long7, 0, brazoN); //Datos: LA, LH, L1, L2, Px', Py', Pz', θ, brazo
      ejecutarParalelo(0);
      delay(tiempoEntreMovimientos); // Una espera adecuada podía ser de 100...   
      // Primero levantará el extremo con el tercer motor y luego usará el resto de motores para no arrastrar el extremo.
      puntoFinalAbsoluto(LA, LH, L1, L2, possig+desfaseX+(desfaseX/2)+cabeceo, Long6-desfaseY-alturaEscalon, Long7, 0, brazoN); //Datos: LA, LH, L1, L2, Px', Py', Pz', θ, brazo
      ejecutarParalelo(0);
      delay(tiempoEntreMovimientos); // Una espera adecuada podía ser de 100...    
      // Ahora baja cada brazo moviendo los motores 2 y 3 con el extremo del brazo bajando en vertical.
      puntoFinalAbsoluto(LA, LH, L1, L2, possig+desfaseX+cabeceo, Long6-alturaEscalon, Long7, 0, brazoN); //Datos: LA, LH, L1, L2, Px', Py', Pz', θ, brazo
      ejecutarParalelo(0); // Px'
      delay(tiempoEntreMovimientos); // Una espera adecuada podía ser de 100...

      // Ahora se atrasan los 4 brazos apoyados en el suelo un cierto desfase igual a la mitad del paso para equilibrar el robot al levantar un brazo delantero.
      // Y se desplazan hacia el brazo 2 para que su centro de gravedad se vaya hacia el lado del brazo 2.
      tiempoPausaDivisorMovimientos = tiempoPausaDivisorMovimientos * multiplicadorDivisorTiempoPausaDivisorMovimientos; // Baja la velocidad de movimiento para que no cabecear y caerse.
      puntoFinalAbsoluto(LA, LH, L1, L2, -desfaseX+cabeceo, Long6-alturaEscalon, Long7, 0, 1); //Datos: LA, LH, L1, L2, Px', Py', Pz', θ, brazo 
      puntoFinalAbsoluto(LA, LH, L1, L2, -desfaseX+cabeceo, Long6-alturaEscalon, -Long7, 0, 2); //Datos: LA, LH, L1, L2, Px', Py', Pz', θ, brazo
      puntoFinalAbsoluto(LA, LH, L1, L2, -possig-desfaseX, Long6, Long7, 0, 3); //Datos: LA, LH, L1, L2, Px', Py', Pz', θ, brazo
      puntoFinalAbsoluto(LA, LH, L1, L2, -possig-desfaseX, Long6, -Long7, 0, 4); //Datos: LA, LH, L1, L2, Px', Py', Pz', θ, brazo
      ejecutarParalelo(0); // Datos: Px' //Se ejecuta siempre que haya avance con fracciones y no un posicionamiento directo
      tiempoPausaDivisorMovimientos = tiempoPausaDivisorMovimientos / multiplicadorDivisorTiempoPausaDivisorMovimientos; // Ahora lo sube
      delay(tiempoEntreMovimientos); // Una espera adecuada podía ser de 100...
    }

    // Brazo Trasero Izquierdo
    brazoN = 3;
///    z = (brazoN - 1) * 3;
    //Serial.println("caminarPasoRegular()-2-");
    Serial.println(F("   Se levanta el brazo 3 (Trasero Izquierdo) para emparejarse con los dos brazos delanteros (Movimiento 2)"));
    //Serial.println("");Serial.println("-- Delantero Derecho");
    // Una vez arriba busca el primer punto, el más avanzado en 100 pequeños avances y se va moviendo hacia atrás en 100 pequeñs avances. 
    // No hace falta las posiciones iniciales porque ya viene de los datos de pos20, pos21 y pos22 de reposo en alto.
    // Para todos los brazos de la izquierda se considera avance positivo y para los de la derecha, negativos.
    //Serial.println(""); Serial.print("possig= "); Serial.print(possig);
    //Serial.print(" - "); Serial.print("z= "); Serial.print(z);
    //Serial.print(" - "); Serial.print("angMotor[z]= "); Serial.println(angMotor[z]);
    // Ya tenemos X (possig), Y (Long6) y Z (0) para llamar a la función que calcula grados y mueve motores.
    // Una vez arriba busca el primer ángulo, el más alejado y se va moviendo hacia atrás en 100 pequeños avances. 
    // El movimiento del brazo hacia el punto final se divide en dos partes para mejorar el rendimiento de la fuerza de los motores.
    // Se hace así para que no ceda por escasa fuerza de los motores ya que el par debería ser más alto para poder ejecutar los movimientos.
    // Primero mueve el motor 2 hasta colocar el extremo del brazo sobre la vertical de la posición final.

    // Primero levantará el extremo con el tercer motor y luego usará el resto de motores para no arrastrar el extremo.
    puntoFinalAbsoluto(LA, LH, L1, L2, -possig-desfaseX, Long6-desfaseY-alturaEscalon, Long7, 0, brazoN); //Datos: LA, LH, L1, L2, Px', Py', Pz', θ, brazo
    ejecutarParalelo(0);
    delay(tiempoEntreMovimientos); // Una espera adecuada podía ser de 100...    
    // Se cambia la componente X para reajustar el paso
    //puntoFinalAbsoluto(LA, LH, L1, L2, 0, Long6-desfaseY-alturaEscalon, Long7, 0, brazoN); //Datos: LA, LH, L1, L2, Px', Py', Pz', θ, brazo
    puntoFinalAbsoluto(LA, LH, L1, L2, desfaseX/2, Long6-desfaseY-alturaEscalon, Long7, 0, brazoN); //Datos: LA, LH, L1, L2, Px', Py', Pz', θ, brazo
    ejecutarParalelo(0);
    delay(tiempoEntreMovimientos); // Una espera adecuada podía ser de 100...    
    // Ahora baja cada brazo moviendo los motores 2 y 3 con el extremo del brazo bajando en vertical.
    puntoFinalAbsoluto(LA, LH, L1, L2, -desfaseX, Long6-alturaEscalon, Long7, 0, brazoN); //Datos: LA, LH, L1, L2, Px', Py', Pz', θ, brazo
    ejecutarParalelo(0);
    delay(tiempoEntreMovimientos); // Una espera adecuada podía ser de 100...

     // Y se desplazan hacia el brazo 1 para que su centro de gravedad se vaya hacia el lado del brazo 1.
    puntoFinalAbsoluto(LA, LH, L1, L2, -desfaseX+cabeceo, Long6-alturaEscalon, -Long7, 0, 1); //Datos: LA, LH, L1, L2, Px', Py', Pz', θ, brazo 
    puntoFinalAbsoluto(LA, LH, L1, L2, -desfaseX+cabeceo, Long6-alturaEscalon, Long7, 0, 2); //Datos: LA, LH, L1, L2, Px', Py', Pz', θ, brazo
    puntoFinalAbsoluto(LA, LH, L1, L2, -desfaseX, Long6-alturaEscalon, -Long7, 0, 3); //Datos: LA, LH, L1, L2, Px', Py', Pz', θ, brazo
    puntoFinalAbsoluto(LA, LH, L1, L2, -possig-desfaseX, Long6, Long7, 0, 4); //Datos: LA, LH, L1, L2, Px', Py', Pz', θ, brazo
    ejecutarParalelo(0); // Datos: Px' //Se ejecuta siempre que haya avance con fracciones y no un posicionamiento directo
    delay(tiempoEntreMovimientos); // Una espera adecuada podía ser de 100...
 
    // Brazo Trasero Derecho
    brazoN = 4;
///    z = (brazoN - 1) * 3;
    //Serial.println("caminarPasoRegular()-2-");
    Serial.println(F("   Se levanta el brazo 4 (Trasero Derecho) y se reagrupa con los tres primeros (Movimiento 2)"));
    // Primero levantará el extremo con el tercer motor y luego usará el resto de motores para no arrastrar el extremo.
    puntoFinalAbsoluto(LA, LH, L1, L2, -possig-desfaseX, Long6-desfaseY-alturaEscalon, Long7, 0, brazoN); //Datos: LA, LH, L1, L2, Px', Py', Pz', θ, brazo
    ejecutarParalelo(0);
    delay(tiempoEntreMovimientos); // Una espera adecuada podía ser de 100...    
    puntoFinalAbsoluto(LA, LH, L1, L2, 0, Long6-desfaseY-alturaEscalon, Long7, 0, brazoN); //Datos: LA, LH, L1, L2, Px', Py', Pz', θ, brazo
    ejecutarParalelo(0);
    delay(tiempoEntreMovimientos); // Una espera adecuada podía ser de 100...    
    // Ahora baja cada brazo moviendo los motores 2 y 3 con el extremo del brazo bajando en vertical.
    puntoFinalAbsoluto(LA, LH, L1, L2, -desfaseX, Long6-alturaEscalon, Long7, 0, brazoN); //Datos: LA, LH, L1, L2, Px', Py', Pz', θ, brazo
    ejecutarParalelo(0);
    delay(tiempoEntreMovimientos); // Una espera adecuada podía ser de 100...
  }  
  return; 
}
void limpiarVariables() {
  //Serial.print("***  Pasa - limpiarVariables()--------"); Serial.println("---Se borran variables---");
  for (int i = 0; i <= 3; i  += 1) { // Recorre los 4 brazos.
    tareasConcurrentes[i] = 0; // Será "1" cuando se deba ejecutar el movimiento de cada brazo de forma paralela. Una fracción cada vez.
    for (int j = 0; j <= 2; j  += 1) { // Recorre los 3 motores de cada brazo
      fraccion[(i*4)+j]; // Contiene el valor calculado de cada fracción para cada motor de cada brazo en movimiento paralelo.
    }
  }
}
void rabo() { // Se mueve constantemente por lo que no necesita variables ni control alguno.
  //delay(tiempoPausaDivisorMovimientos);
  //msec0rabo[0] = msec0rabo[0] + 200; 
  unsigned long tiempo1 = 0;
  unsigned long tiempo2 = 0;
  int pp = 4;//13;
  int valor = 300;
  valor = valor + 200;
  tiempo1 = millis();
  servos4.setPWM(pp, 0, valor); // Podría ser servos2, servos3 o servos4. Y podría ser z=4 por lo que habría que usar otra variable para no confundirlas.
  tiempo2 = millis();
  while ((tiempo2 - tiempo1) < 200) {
    tiempo2 = millis();
  }
  valor = valor - 200; 
  servos4.setPWM(pp, 0, valor); // Podría ser servos2, servos3 o servos4. Y podría ser z=4 por lo que habría que usar otra variable para no confundirlas.  
}
void setup() {
  Serial.begin(115200);
  int i;
  servos1.begin(); // Start the servo shield // Adafruit
  servos1.reset(); // Start the servo shield // Adafruit
  servos1.setPWMFreq(60); // Frecuecia PWM (modulación por ancho de pulsos) de 60Hz o T=16,66ms // Adafruit
  servos2.begin(); // Start the servo shield // Adafruit
  servos2.reset(); // Start the servo shield // Adafruit
  servos2.setPWMFreq(60); // Frecuecia PWM (modulación por ancho de pulsos) de 60Hz o T=16,66ms // Adafruit
  servos3.begin(); // Start the servo shield // Adafruit
  servos3.reset(); // Start the servo shield // Adafruit
  servos3.setPWMFreq(60); // Frecuecia PWM (modulación por ancho de pulsos) de 60Hz o T=16,66ms // Adafruit
  servos4.begin(); // Start the servo shield // Adafruit
  servos4.reset(); // Start the servo shield // Adafruit
  servos4.setPWMFreq(60); // Frecuecia PWM (modulación por ancho de pulsos) de 60Hz o T=16,66ms // Adafruit
  Serial.println(F(""));Serial.println(F("-------- Posicionamiento Inicial (posReposoDePie[]) --------"));
  delay(30);
  rabo();
  int j; 
  int z;
  int zz; 
  int zzz; 
  int zzzz; 
  for (i = 0; i < 4; i = i + 1) { // Recorre los 4 brazos. 
    // Sólo se va a hacer al mismo tiempo en la activación inicial del robot para no debilitar el par de los motores en futuros movimientos.
    for (j = 0; j <= 2; j = j + 1) { // Recorre los 3 motores de cada brazo.
      z = (i*3)+j; 
      // posPasoAlto - posReposoDePie
      if (z < 3) { // Los 3 primeros servos se ejecutan sobre la primera tarjeta de expansión
        servos1.setPWM(z, 0, posReposoDePie[z]); // int setbounds(int servo, int minposition, int maxposition); (Returns 0 if successfully set; returns 1 if instruction failed) // Adafruit
      } else if ((z > 2) and (z < 6)){ // Los siguientes 3 servos trabajan sobre la segunda tarjeta de expansión
        zz = z - 3; // "zz" sólo se usa en el número de servo porque los arrays con los datos mantienen datos de los 12 motores
        servos2.setPWM(zz, 0, posReposoDePie[z]); // int setbounds(int servo, int minposition, int maxposition); (Returns 0 if successfully set; returns 1 if instruction failed) // Adafruit
      } else if ((z > 5) and (z < 9)){ // Los siguientes 3 servos trabajan sobre la tercera tarjeta de expansión
        zzz = z - 6; // "zzz" sólo se usa en el número de servo porque los arrays con los datos mantienen datos de los 12 motores
        servos3.setPWM(zzz, 0, posReposoDePie[z]); // int setbounds(int servo, int minposition, int maxposition); (Returns 0 if successfully set; returns 1 if instruction failed) // Adafruit
      } else { // Los otros 3 servos trabajan sobre la cuarta tarjeta de expansión
        zzzz = z - 9; // "zzzz" sólo se usa en el número de servo porque los arrays con los datos mantienen datos de los 12 motores
        servos4.setPWM(zzzz, 0, posReposoDePie[z]); // int setbounds(int servo, int minposition, int maxposition); (Returns 0 if successfully set; returns 1 if instruction failed) // Adafruit
      }
      msecMotor[z] = posReposoDePie[z]; //Se inicializan para comenzar desde un valor conocido.
      angMotor[z] = map(posReposoDePie[z], msecMin[z], msecMax[z], angMin[z], angMax[z]);
//      Serial.print(" z= "); Serial.print(z); Serial.print(" - msecMotor[z]= "); Serial.print(msecMotor[z]); Serial.print(" - angMotor[z]= "); Serial.println(angMotor[z]);
    }
  }
  delay(3000);
//  PxPrima = 3; // Componente x o separación desde el cuerpo en paralelo al movimiento en 4 brazos con 3 motores.
//  PyPrima = 20; // = distanciaMaximaRectaParalela; // Componente y de la altura en la primera parte del paso en 4 brazos con 3 motores.
//  PzPrima = 0; //= distanciaPasoMaximo; // Componente z destino en 4 brazos con 3 motores.
  subirPasoRegular(LA, LH, L1, L2, PxPrima, PyPrima, PzPrima); //Datos: LA, LH, L1, L2, Px', Py', Pz'
  //puntoFinalAbsoluto(LA, LH, L1, L2, 0, PxPrima, PyPrima, PzPrima, 1); //Datos: LA, LH, L1, L2, Px', Py', Pz', θ, Brazo  
 // puntoFinalAbsoluto(LA, LH, L1, L2, 0, -10, 10, 5, 1); //Datos: LA, LH, L1, L2, Px', Py', Pz', θ, Brazo
 // ejecutarParalelo(PzPrima); // Ejecuta los movimientos marcados para ejecutarse en paralelo
//  PxPrima = -14; PyPrima = 20; PzPrima = 0;
//  Serial.print("**************** setup() - Px'= "); Serial.print(PxPrima); Serial.print(" - Py'="); Serial.print(PyPrima); Serial.print(" - Pz'="); Serial.println(PzPrima);
//  puntoFinalAbsoluto(LA, LH, L1, L2, PxPrima, PyPrima, PzPrima, 0, 1); //Datos: LA, LH, L1, L2, Px', Py', Pz', θ, brazo
//  PxPrima = -15; PyPrima = 20; PzPrima = 0;
//  Serial.print("**************** setup() - Px'= "); Serial.print(PxPrima); Serial.print(" - Py'="); Serial.print(PyPrima); Serial.print(" - Pz'="); Serial.println(PzPrima);
//  puntoFinalAbsoluto(LA, LH, L1, L2, PxPrima, PyPrima, PzPrima, 0, 2); //Datos: LA, LH, L1, L2, Px', Py', Pz', θ, brazo
//  PxPrima = -16; PyPrima = 20; PzPrima = 0;
//  Serial.print("**************** setup() - Px'= "); Serial.print(PxPrima); Serial.print(" - Py'="); Serial.print(PyPrima); Serial.print(" - Pz'="); Serial.println(PzPrima);
//  puntoFinalAbsoluto(LA, LH, L1, L2, PxPrima, PyPrima, PzPrima, 0, 3); //Datos: LA, LH, L1, L2, Px', Py', Pz', θ, brazo
//  PxPrima = -17; PyPrima = 20; PzPrima = 0;
//  Serial.print("**************** setup() - Px'= "); Serial.print(PxPrima); Serial.print(" - Py'="); Serial.print(PyPrima); Serial.print(" - Pz'="); Serial.println(PzPrima);
//  puntoFinalAbsoluto(LA, LH, L1, L2, PxPrima, PyPrima, PzPrima, 0, 4); //Datos: LA, LH, L1, L2, Px', Py', Pz', θ, brazo
//  ejecutarParalelo(0); //Datos: Px'
}
void loop() {
  rabo(); // Se mueve siempre que el robot esté activo  
  //delay(tiempoPausaDivisorMovimientos);
  
  //PxPrima = 0; // Componente x destino. Para caminar se irá calculado.
  //PyPrima = 0; // Componente y destino. Para caminar se irá calculado pero en caminarPasoRegular() será "distanciaMaximaRectaParalela"
  //PzPrima = 5; // Componente z destino. Para caminar se irá calculado pero en caminarPasoRegular() será "0".
  //puntoFinalAbsoluto(LA, LH, L1, L2, 0, PxPrima, PyPrima, PzPrima, 1); //Datos: LA, LH, L1, L2, Px', Py', Pz', θ, Brazo  
  //puntoFinalAbsoluto(LA, LH, L1, L2, 0, PxPrima, PyPrima, PzPrima, 4); //Datos: LA, LH, L1, L2, Px', Py', Pz', θ, Brazo  
  //ejecutarParalelo(PxPrima); // Datos: Px' //Se ejecuta siempre que haya avance con fracciones y no un posicionamiento directo
  //caminarPasoRegular(LA, LH, L1, L2, PxPrima, distanciaMaximaRectaParalela, PzPrima); //Datos: LA, LH, L1, L2, Px', Py', Pz'
  //subirPasoRegular(LA, LH, L1, L2, PxPrima, PyPrima, PzPrima); //Datos: LA, LH, L1, L2, Px', Py', Pz'
}
