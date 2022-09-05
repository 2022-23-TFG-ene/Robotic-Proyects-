/**
 * @file introduccionDatosMotores.c
 * @author Jaime Sáiz de la Peña
 * @brief Este programa recoge, analiza y adapta los parámetros de DH. Se 
 * recogen mediante un CSV, de forma manual o mediante preguntas. Si se quiere
 * recoger a traves del CSV se debe haber contestado las preguntas con el mismo 
 * número de motores. El el resultado se guarda en un CSV que recoge el programa 
 * programaSustituirVariablesMatriz.
 */

#include "stdio.h"
#include <math.h>
#include "stdlib.h"
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h> 

int NUMMOT=2;

void itroducirMotores(float matriz[NUMMOT][4]);
void itroducirMotoresAutomatico(float matriz[NUMMOT][4],FILE *datosCSV);
int tipoIntroduccionDatos();
void informacionMotoresAutomatico();
int encontrarEjeY(int x, int z);
void introducimosEjesMotores(int datosMotores[NUMMOT][3],int ejes90[10]);
void introducirDatosMatrizMotores(int datosMotores[NUMMOT][3],float matriz[NUMMOT][4],FILE *datosCSV);
char cambiarNumeroSimbolo(int n);
int cambiarSimboloNumero(char n);
void guardarMatrizenFichero(float matriz[NUMMOT][4]);
void cambiarValoresdatosMotores(int datosMotores[NUMMOT][3]);
void importarDatosCSV();
void introducirMotores();
void guardarDatosInicialesCSV(FILE *datosCSV,int motorSaltado);

int main(){
	FILE *datosCSV;				//Archivo donde se guardará el resultado final
	int introduccionDatos=0;	//Variable que indica como se obtendrán los datos.		
	int decision=0;				//Indica si se quieren importar o crear los datos.
	
	printf("Desea importar los datos desde un fichero CSV (1) o generar nuevos datos (0): ");
	scanf(" %d",&decision);
	
	if (decision==0){
		introducirMotores();
		//guardarDatosInicialesCSV(datosCSV);
		
		float matriz[NUMMOT][4];
		introduccionDatos=tipoIntroduccionDatos();
	
		if (introduccionDatos==1){
			itroducirMotores(matriz);
		}else if (introduccionDatos==2){
			itroducirMotoresAutomatico(matriz,datosCSV);
		}

		for (int i=0;i<NUMMOT;i++){
			for (int j=0;j<4;j++){
				printf("%f      ",matriz[i][j]);
			} 
			printf("\n");
		}
		guardarMatrizenFichero(matriz);
	}else{
		importarDatosCSV();
	}
	return 0;
}

/**
* @fn void guardarDatosInicialesCSV(FILE *datosCSV,int motorSaltado)
* @brief guarda en el CSV el número de motores y el número de ejes de 
* coordenadas que no coinciden con ningún motor.  
* @param datosCSV es el fichero donde se guardan los datos.
* @return -
*/
void guardarDatosInicialesCSV(FILE *datosCSV,int motorSaltado){
	datosCSV = fopen("DatosMotoresModificables.csv", "w+");	
	fprintf(datosCSV,"motores \n");
	fprintf(datosCSV,"%d\n",NUMMOT-motorSaltado);
	fprintf(datosCSV,"*************************************************\n");
	fclose(datosCSV);
}

/**
* @fn introducirMotores()
* @brief El usuario introduce el número de motores y el número de ejes de
* coordenadas que no coinciden con ningún motor.
* @param -
* @return Devuelve el tipo de introducción de datos.
*/
void introducirMotores(){
	printf("Introduzca el número de motores: ");
	scanf(" %d",&NUMMOT);
}

/**
* @fn int datosIniciales()
* @brief función en la que el usuario introduce como quiere introducir los
* datos al problema. 
* @param -
* @return Devuelve el tipo de introducción de datos.
*/
int tipoIntroduccionDatos(){
	int introduccionDatos=0;
	printf("\nIntroduzca la forma en la que desea introducir los motores: \n");
	printf("	- \"1\" si desea hacerlo de forma manual \n");
	printf("	- \"2\" si desea seguir los pasos de DH \n");
	scanf(" %d",&introduccionDatos);
	return introduccionDatos;
}

/**
* @fn void itroducirMotores(float matriz[NUMMOT][4])
* @brief explica como y recoge los datos para introducir de forma manual los
* parámetros de DH. 
* @param matriz con los parámetros de DH. 
* @return no devuelve nada pero actualiza los datos de la matriz con los 
* parámetros de DH.
*/
void itroducirMotores(float matriz[NUMMOT][4]){
	for (int i =0;i<NUMMOT;i=i+1){
		printf("Para introducir los datos correctamente, siga los pasos que se muestran a continuación:\n\n");
		printf("\t0) Cree una matriz de 4 x (nº de motores + giros de 90º entre los segmentos que unen motores)\n");
		printf("\t   La primera fila representa el giro de nuestro motor desde el origen (beta) + el giro sobre z para que coincida xi con xi-1\n");
		printf("\t   La segunda el desplazamiento en z para que coincidan los origenes (distancia entre ejes de motores) (d)\n");
		printf("\t   La tercera el desplazamiento en x (r)\n");
		printf("\t   La cuarta la rotación para que coincian los ejes z (alfa)\n");
		printf("\t   ***T(i-2)/i=rotz(betai)*transz(di)*transx(ri)*rotx(alfai)***\n");
		printf("\t1) Situe z0 hasta zn estando z0 en la base zn en la punta del brazo.\n");
		printf("\t   Se debe situar las z de forma coincidente a los ejes de los motores.\n");
		printf("\t   z0 irá en la dirección del segmento entre la base y el motor 1\n");
		printf("\t2) Añada x1 (está en la normal de z0 y z1)\n");
		printf("\t   Añada x0 si es posible en la misma dirección que x1\n");
		printf("\t   Añada hasta xn respetando la regla de la mano derecha y que x tiene que estar en la normal de zn y zn-1\n");
		printf("\t   Comprobar que zi-1 o xi están en la dirección del segmento para llegar del motor i-1 al i. Sinó cambiar x para que se cumpla\n");
		printf("\t3) Añada las y a los sistemas de coordenadas respetando la regla de la mano derecha\n");
		printf("\t   Regla de la mano derecha:\n");
		printf("\t   z    y          \n");
		printf("\t   |   /           \n");
		printf("\t   |  /            \n");
		printf("\t   | /             \n");
		printf("\t   . --------x     \n");
		printf("\t4) A continuación se rellena la matriz creada como exponemos a continuación\n");
		printf("\t   1- Rotación sobre eje z para que coincidan las x\n");
		printf("\t   2- En d desplazamiento sobre el eje z entre ejes del motor. (distancia entre ejes teniendo en cuenta que están en un mismo plano)\n");
		printf("\t   3- En r va el desplazamiento en x\n");
		printf("\t   4- rotación sobre x para que coincidan ejes z\n");
		printf("\t   En los 4 pasos anteriores es importante tener en cuenta los cambios que tienen lugar en lo pasos anteriores.\n");
		
		printf("Introducir Motor beta alfa r d (separado por espacios) y de forma creciente\n");
		scanf("%f" "%f" "%f" "%f",&matriz[i][0],&matriz[i][1],&matriz[i][2],&matriz[i][3]);
	}
}

/**
* @fn void itroducirMotoresAutomatico(float matriz[NUMMOT][4],FILE *datosCSV)
* @brief realiza las preguntas necesarias con el fin de tratar y guardar los datos de la
* matriz de los parámetros de DH.
* @param matriz donde se guardan los parámetros de DH, datosCSV fichero donde se guarda la matriz final. 
* @return no devuelve nada pero actualiza la matriz final. 
*/
void itroducirMotoresAutomatico(float matriz[NUMMOT][4],FILE *datosCSV){
	int datosMotores[NUMMOT][3]; 	//matriz donde se guardan los datos de los ejes
	int ejes90[10]; 				//en esta variable se almacenan las posiciones de los ejes de coordenadas que no coinciden con motores. 
	int temporal=0;
	int flag=0;
	int fila=0;
	int columna=0;
	char valor=0;
	char decision=' ';
			
	informacionMotoresAutomatico();
	introducimosEjesMotores(datosMotores,ejes90);
	
	for (int i=0;i<NUMMOT;i++){
		for (int j=0;j<3;j++){
				printf("%c      ",cambiarNumeroSimbolo(datosMotores[i][j]));
			} 
			printf("\n");
	}
	
	while (flag==0){ 
		printf("Desea cambiar alguna x de la matriz?(La x no puede ser igual que la z) y/n");
		scanf(" %c", &decision);
		if (decision=='n'){
			printf("Desea FORZAR el cambio de la matriz para pruebas de funcionamiento?y/n"); //permite no solo cambiar la x (suficiente para arreglar cualquier error) sino todos los campos. No comprueba que se mantenga la regla de la mano derecha
			scanf(" %c", &decision);
			if (decision=='y'){
				cambiarValoresdatosMotores(datosMotores);
			}
			flag=1;
		}else{
			printf("\nIndique la fila que quiere cambiar (Empieza en 1): ");
			scanf(" %d",&fila);
			printf("\nIntroduce el caracter nuevo (\"<\" izquierda, \">\" derecha, \"^\" arriba, \"v\" abajo, \".\" hacia el usuario, \"x\" hacia el fondo)");
			scanf(" %c",&valor);
			printf("\n");
			
			datosMotores[fila-1][0]=valor;
			
			printf("****************Esto es lo que devuelve el cambio %d\n",encontrarEjeY(valor,datosMotores[fila-1][2]));
			
			datosMotores[fila-1][1]=cambiarNumeroSimbolo(encontrarEjeY(cambiarSimboloNumero(valor),datosMotores[fila-1][2]));
			
			for (int m=0;m<NUMMOT;m++){
				for (int n=0;n<3;n++){
					printf("%c      ",cambiarNumeroSimbolo(datosMotores[m][n]));
				} 
			printf("\n");
			}
		}
	}
	
	datosCSV = fopen("DatosMotoresModificables.csv", "a+");	
	for (int n=0;n<NUMMOT;n++){
		fprintf(datosCSV,"%c / %c / %c\n",cambiarNumeroSimbolo(datosMotores[n][0]),cambiarNumeroSimbolo(datosMotores[n][1]),cambiarNumeroSimbolo(datosMotores[n][2]));
	}
	fclose(datosCSV);
	introducirDatosMatrizMotores(datosMotores,matriz,datosCSV);
}

/**
* @fn introducimosEjesMotores(int datosMotores[NUMMOT][3],int ejes90[10])
* @brief situal los ejes de los motores y de los segmentos con ángulos de 90º.
* @param datosMotores matriz donde se guardan los ejes de coordenadas, ejes90
* número de ángulos de 90º que no son motores.
* @return no devuelve nada pero actualiza la matriz donde se guardan los ejes
* de coordenadas.
*/
void introducimosEjesMotores(int datosMotores[NUMMOT][3],int ejes90[10]){
	// x y z (< izquierda 1,> derecha 2,^ arriba 3,v abajo 4,. hacia el usuario 5,x hacia el fondo 6)
	int temp=0;
	int sentido=0;
	int noMotor=0;
	
	//colocamos las z
	for (int i=1;i<=NUMMOT;i++){
		printf("\nEl motor %d tiene el eje perpendicular al suelo (1), perpendicular al usuario (2) o paralelo al suelo y al usuario (3)", i-noMotor);
		scanf(" %d", &temp);
		if (temp==1){
			datosMotores[i-1][2]=3;
		}
		if (temp==2){
			datosMotores[i-1][2]=6;
		}
		if (temp==3){
			datosMotores[i-1][2]=1;
		}
	}
	
	
	//colocamos las x
	for (int i=1;i<=NUMMOT;i++){
		if (i==1){
			if(datosMotores[i-1][2]==1){
				datosMotores[i-1][0]=6;
			}else{
				datosMotores[i-1][0]=1;
			}
		}else{
			//Compara motor 2 que se guarda en la posicion 1 con motor 1 que se guarda en la posicion 0 y asi sucesivamente
			if (datosMotores[i-1][2]==datosMotores[i-2][2]){
				datosMotores[i-1][0]=datosMotores[i-2][0];
			}
			else{
				//si metemos el 2 z y despues el 1 z nos devuelve la dirección de x
				datosMotores[i-1][0]=encontrarEjeY(datosMotores[i-1][2],datosMotores[i-2][2]);
			}
		}
	}
	//colocamos las y
	for (int i=1;i<=NUMMOT;i++){
		datosMotores[i-1][1]=encontrarEjeY(datosMotores[i-1][0],datosMotores[i-1][2]);
	}
}

/**
* @fn void introducirDatosMatrizMotores(int datosMotores[NUMMOT][3],float matriz[NUMMOT][4],FILE *datosCSV)
* @brief Rellena la matriz de parámetros a partir de una serie de preguntas. 
* @param datosMotores matriz donde se guardan los ejes de coordenadas, matriz matriz donde se guardan
* los parámetros de DH, datos fichero donde se guardan dichos datos.
* @return no devuelve nada pero actualiza la matriz donde se guardan los parámetros de DH.
*/
void introducirDatosMatrizMotores(int datosMotores[NUMMOT][3],float matriz[NUMMOT][4],FILE *datosCSV){
	int distancia=0;
	int giro=0;
	int motorSaltado=0;
	char temporal[10];
	char igualesz0z1=' '; 
	

	fprintf(datosCSV,"**************************************************\n");
	printf("***********************************\n\n");	
	
	printf("\n\nA continuación se van a suceder los motores y ejes definidos anteriormente. Se deben contestar a las siguientes preguntas usando siempre la regla de la mano derecha.\n");
	printf("ATENCION: En caso de que los dos ejes no pertenezcan al mismo plano los desplazamientos se realizarán en dirección a la normal entre las dos z. Se deberán realizar todos los desplazamientos posibles.\n");
	printf("ATENCION: En el caso de que para llegar de un motor a otro no se hayan realizado todos los desplazamientos necesarios, estos se realizarán en el siguiente motor.\n");
	for (int i=0;i<NUMMOT-motorSaltado;i++){
		//Giro sobre z para que coincida x - Beta
		if(i==0){
			printf("¿Son z0 (sentido del segmento que une la base con el motor 1 y en su ausencia igual a z1) y z1 (%c) iguales? (s/n): ", cambiarNumeroSimbolo(datosMotores[i][2]));
			scanf(" %c",&igualesz0z1);
			if (igualesz0z1=='s'){
				guardarDatosInicialesCSV(datosCSV,motorSaltado);
				motorSaltado=1;
				datosCSV = fopen("DatosMotoresModificables.csv", "a+");	
				
				//Giro sobre z - b
				printf("Queremos llegar al motor 2 empezando por la base.\n");
				printf("El eje x0 es igual al eje x1 (%c)  del motor 2 es (%c)\n",cambiarNumeroSimbolo(datosMotores[0][0]),cambiarNumeroSimbolo(datosMotores[1][0]));
				printf("Para llegar del x0 al x2 cuanto hay que girar sobre z0 (%c)(0,90,180,-90)\n",cambiarNumeroSimbolo(datosMotores[0][2]));
				fprintf(datosCSV,"Queremos llegar al motor 2 empezando por la base.\n");
				fprintf(datosCSV,"El eje x0 es igual al eje x1 (%c)  del motor 2 es (%c)\n",cambiarNumeroSimbolo(datosMotores[0][0]),cambiarNumeroSimbolo(datosMotores[1][0]));
				fprintf(datosCSV,"Para llegar del x0 al x2 cuanto hay que girar sobre z0 (%c)(0,90,180,-90)\n",cambiarNumeroSimbolo(datosMotores[0][2]));
				scanf(" %d", &giro);
				fprintf(datosCSV,"\t-VALOR: %d\n",giro);
				matriz[0][0]=giro;
			
				//Desplazamiento sobre el eje z - d
				printf("Cuanto hay que desplazarse sobre el eje z0 desde la base para llegar motor 2?\n");
				fprintf(datosCSV,"Cuanto hay que desplazarse sobre el eje z0 desde la base para llegar motor 2?\n");
				scanf(" %d",&distancia);
				fprintf(datosCSV,"\t-VALOR: %d\n",distancia);
				matriz[i][3]=distancia;
				
				//Desplazamiento sobre el eje x - r
				printf("Cuanto hay que desplazarse sobre el eje x desde el elemento (%d) para llegar al siguiente elemento (%d)?\n",i,i+1);
				printf("Tener en cuenta el desplazamiento que se ha hecho anteriormente (era (%c) y se ha aplicado un desplazamiento de %f\n) ",cambiarNumeroSimbolo(datosMotores[0][0]),matriz[i][0]);
				fprintf(datosCSV,"Cuanto hay que desplazarse sobre el eje x desde el elemento (%d) para llegar al siguiente elemento (%d)?\n",i,i+1);
				fprintf(datosCSV,"Tener en cuenta el desplazamiento que se ha hecho anteriormente (era (%c) y se ha aplicado un giro de %f\n) ",cambiarNumeroSimbolo(datosMotores[0][0]),matriz[i][0]);
				scanf(" %d", &distancia);
				fprintf(datosCSV,"\t-VALOR: %d\n",distancia);
				matriz[i][2]=distancia;
				
				//Giro sobre x para que coincida z -alfa
				printf("El eje z0 es (%c) y el eje z2 es (%c) (x0 es (%c))\n",cambiarNumeroSimbolo(datosMotores[0][2]),cambiarNumeroSimbolo(datosMotores[1][2]),cambiarNumeroSimbolo(datosMotores[0][0]));
				printf("Para llegar del z0 a z2 cuanto hay que girar sobre x0(regla de la mano derecha)(0,90,180,-90)\n");
				printf("Tener en cuenta el desplazamiento que se ha hecho anteriormente sobre x(era (%c) y se ha aplicado un desplazamiento de %f)\n",cambiarNumeroSimbolo(datosMotores[0][0]),matriz[0][0]);
				fprintf(datosCSV,"El eje z0 es (%c) y el eje z2 es (%c) (x0 es (%c))\n",cambiarNumeroSimbolo(datosMotores[0][2]),cambiarNumeroSimbolo(datosMotores[1][2]),cambiarNumeroSimbolo(datosMotores[0][0]));
				fprintf(datosCSV,"Para llegar del z0 a z2 cuanto hay que girar sobre x0(regla de la mano derecha)(0,90,180,-90)\n");
				fprintf(datosCSV,"Tener en cuenta el desplazamiento que se ha hecho anteriormente sobre x(era (%c) y se ha aplicado un desplazamiento de %f)\n",cambiarNumeroSimbolo(datosMotores[0][0]),matriz[0][0]);
				scanf(" %d", &giro);
				fprintf(datosCSV,"\t-VALOR: %d\n",giro);
				matriz[i][1]=giro;
				
				giro=0;
				fclose(datosCSV);
				printf("***********************************\n\n");	
				continue;
			}
		}
		
		datosCSV = fopen("DatosMotoresModificables.csv", "a+");
		
		fprintf(datosCSV,"\n\nElemento %d:\n",i+1+motorSaltado);
		fprintf(datosCSV,"--------------------\n");

		//Giro sobre z - b
		printf("Queremos llegar al motor %d empezando por la base.\n",i+1+motorSaltado);
		printf("El eje x %d es (%c) y el eje x del motor %d es (%c)\n",i+motorSaltado , cambiarNumeroSimbolo(datosMotores[i-1+motorSaltado][0]),i+1+motorSaltado,cambiarNumeroSimbolo(datosMotores[i+motorSaltado][0]));
		printf("Para llegar del x %d al x %d (sobre z (%c)) cuanto hay que girar (regla de la mano derecha)(0,90,180,-90)\n",i+motorSaltado, i+1+motorSaltado,cambiarNumeroSimbolo(datosMotores[i-1+motorSaltado][2]));
		fprintf(datosCSV,"Queremos llegar al elemento %d empezando por la base.\n",i+1+motorSaltado);
		fprintf(datosCSV,"El eje x %d es (%c) y el eje x del motor %d es (%c)\n",i+motorSaltado , cambiarNumeroSimbolo(datosMotores[i-1+motorSaltado][0]),i+1+motorSaltado,cambiarNumeroSimbolo(datosMotores[i+motorSaltado][0]));
		fprintf(datosCSV,"Para llegar del x %d al x %d (sobre z (%c)) cuanto hay que girar (regla de la mano derecha)(0,90,180,-90)\n",i+motorSaltado, i+1+motorSaltado,cambiarNumeroSimbolo(datosMotores[i-1+motorSaltado][2]));
		scanf(" %d", &giro);
		fprintf(datosCSV,"\t-VALOR: %d\n",giro);
		matriz[i][0]=giro;
		giro=0;
		
		//Desplazamiento sobre el eje z - d
		printf("Cuanto hay que desplazarse sobre el eje z (%c) desde el elemento (%d) para llegar al siguiente elemento (%d)?\n",cambiarNumeroSimbolo(datosMotores[i-1+motorSaltado][2]),i+motorSaltado,i+1+motorSaltado);
		fprintf(datosCSV,"Cuanto hay que desplazarse sobre el eje z (%c) desde el elemento (%d) para llegar al siguiente elemento (%d)?\n",cambiarNumeroSimbolo(datosMotores[i-1+motorSaltado][2]),i+motorSaltado,i+1+motorSaltado);
		scanf(" %d",&distancia);
		//sprintf(temporal,"%d",distancia);
		fprintf(datosCSV,"\t-VALOR: %d\n",distancia);
		matriz[i][3]=distancia;
		
		//Desplazamiento sobre el eje x - r
		printf("Cuanto hay que desplazarse sobre el eje x desde el elemento (%d) para llegar al siguiente elemento (%d)?\n",i+motorSaltado,i+1+motorSaltado);
		printf("Tener en cuenta el desplazamiento que se ha hecho anteriormente (era (%c) y se ha aplicado un desplazamiento de %f\n) ",cambiarNumeroSimbolo(datosMotores[i-1+motorSaltado][0]),matriz[i][0]);
		fprintf(datosCSV,"Cuanto hay que desplazarse sobre el eje x desde el elemento (%d) para llegar al siguiente elemento (%d)?\n",i+motorSaltado,i+1+motorSaltado);
		fprintf(datosCSV,"Tener en cuenta el desplazamiento que se ha hecho anteriormente (era (%c) y se ha aplicado un desplazamiento de %f)\n",cambiarNumeroSimbolo(datosMotores[i-1+motorSaltado][0]),matriz[i][0]);
		scanf(" %d",&distancia);
		fprintf(datosCSV,"\t-VALOR: %d\n",distancia);
		matriz[i][2]=distancia;
		
		//Giro sobre x para que coincida z -alfa
		printf("El eje z %d es (%c) y el eje z del motor %d es (%c) (x es (%c))\n",i+motorSaltado , cambiarNumeroSimbolo(datosMotores[i-1+motorSaltado][2]),i+1+motorSaltado,cambiarNumeroSimbolo(datosMotores[i+motorSaltado][2]),cambiarNumeroSimbolo(datosMotores[i-1+motorSaltado][0]));
		printf("Para llegar del z %d al z %d (sobre z) cuanto hay que girar (regla de la mano derecha)(0,90,180,-90)\n",i+motorSaltado, i+1+motorSaltado);
		printf("Tener en cuenta el desplazamiento que se ha hecho anteriormente sobre x(era (%c) y se ha aplicado un desplazamiento de %f)\n",cambiarNumeroSimbolo(datosMotores[i-1+motorSaltado][0]),matriz[i][0]);
		fprintf(datosCSV,"El eje z %d es (%c) y el eje z del motor %d es (%c) (x es (%c))\n",i , cambiarNumeroSimbolo(datosMotores[i-1+motorSaltado][2]),i+1+motorSaltado,cambiarNumeroSimbolo(datosMotores[i+motorSaltado][2]),cambiarNumeroSimbolo(datosMotores[i-1+motorSaltado][0]));
		fprintf(datosCSV,"Para llegar del z %d al z %d (sobre z) cuanto hay que girar (regla de la mano derecha)(0,90,180,-90)\n",i+motorSaltado, i+1+motorSaltado);
		fprintf(datosCSV,"Tener en cuenta el desplazamiento que se ha hecho anteriormente sobre x(era (%c) y se ha aplicado un desplazamiento de %f)\n",cambiarNumeroSimbolo(datosMotores[i-1+motorSaltado][0]),matriz[i][0]);
		scanf(" %d", &giro);
		fprintf(datosCSV,"\t-VALOR: %d\n",giro);
		matriz[i][1]=giro;
		giro=0;
		
		printf("***********************************\n\n");	
	}
	NUMMOT=NUMMOT-motorSaltado;
	fclose(datosCSV);
}

/**
* @fn char cambiarNumeroSimbolo(int n)
* @brief transforma números en símbolos de direcciones.
* @param numero a transformar en carácter.
* @return devuelve el símbolo equivalente al data pasado.
*/
char cambiarNumeroSimbolo(int n){
	switch(n){
			case 1:
				return '<';
			case 2:
				return '>';
			case 3:
				return '^';
			case 4:
				return 'v';
			case 5:
				return '.';
			case 6:
				return 'x';
	}
}

/**
* @fn int cambiarSimboloNumero(char n)
* @brief transforma carácteres en números equivalentes al sentido de ejes.
* @param carácter a transformar en número. 
* @return devuelve el número equivalente al simbolo pasado por parámetro.
*/
int cambiarSimboloNumero(char n){
	switch(n){
			case '<':
				return 1;
			case '>':
				return 2;
			case '^':
				return 3;
			case 'v':
				return 4;
			case '.':
				return 5;
			case 'x':
				return 6;
	}
}

/**
* @fn int encontrarEjeY(int x, int z)
* @brief A partir de los ejes de coordenadas x y z devuelve el eje y respetando la
* regla de la mano derecha. También devuelve xi si se le pasa zi- zi.
* @param x y z, ejes de coordenadas.
* @return eje de coordenadas y;
*/
int encontrarEjeY(int x, int z){
	switch(x){
		case 1:
			switch(z){
					case 3:
						return 5;
					case 4:
						return 6;
					case 5:
						return 4;
					case 6:
						return 3;
				}
		case 2:
			switch(z){
					case 3:
						return 6;
					case 4:
						return 5;
					case 5:
						return 3;
					case 6:
						return 4;
				}
		case 3:
			switch(z){
					case 1:
						return 6;
					case 2:
						return 5;
					case 5:
						return 1;
					case 6:
						return 2;
				}
		case 4:
			switch(z){
					case 1:
						return 5;
					case 2:
						return 6;
					case 5:
						return 2;
					case 6:
						return 1;
				}
		case 5:
			switch(z){
					case 1:
						return 3;
					case 2:
						return 4;
					case 3:
						return 2;
					case 4:
						return 1;
				}
		case 6:
			switch(z){
					case 1:
						return 4;
					case 2:
						return 3;
					case 3:
						return 1;
					case 4:
						return 2;
				}
	}
}

/**
* @fn informacionMotoresAutomatico()
* @brief Muestra información al usuario si quiere introducir los motores de
* forma automática.
* @param -
* @return -
*/
void informacionMotoresAutomatico(){
	printf("Presmisas iniciales\n");
	printf("\tp1) Cuando se habla de distancia hasta un motor, si está de forma perpendicular es hasta el eje del mismo\n");
	printf("\tp   y si está en vertical será hasta el punto medio del mismo (punto medio del eje) \n");
	printf("\tp2) Consideraremos la base con z0 en la dirección del primer segmento del brazo hasta el 1º motor 0 de altura en caso de no existir\n");
	printf("\t    Los sistemas de coordenadas deben respetar la regla de la mano derecha\n");
	printf("\t    Regla de la mano derecha:\n");
	printf("\t    z    y          \n");
	printf("\t    |   /           \n");
	printf("\t    |  /            \n");
	printf("\t    | /             \n");
	printf("\t    . --------x     \n");
	printf("\tp2 Inicialmente colocaremos el robot de tal manera uqe los ejes de los motores esten perpendiculares al usuario, ,perpendiculares al suelo o paralelos al usuario y al suelo.");
}

/**
* @fn void guardarMatrizenFichero(float matriz[NUMMOT][4])
* @brief procedimiento que guarda la matriz que recibe en un fichero CSV. Se usa
* para guardar la matriz final.
* @param matriz es la matriz con los parámetros de DH.
* @return -
*/
void guardarMatrizenFichero(float matriz[NUMMOT][4]){
	FILE *fp;
	fp = fopen("ParametrosDHespecificos.csv", "w+");
	for (int i =0;i<NUMMOT;i++){
		//separación " / "
		//Se guarda en otro orden para que cuadre con el siguiente programa. B A r d
		fprintf(fp,"%f / %f / %f / %f\n", matriz[i][0], matriz[i][1], matriz[i][2], matriz[i][3]);
	} 
	fclose(fp);
}

/**
* @fn void cambiarValoresdatosMotores(int datosMotores[NUMMOT][3])
* @brief Nos permite cambiar los valores de los ejes de coordenadas (no solo motores).
* @param datosMotores es la matriz donde se guardan los ejes de coordenadas. 
* @return no devuelve nada pero actualiza la matriz donde se guardan la información
* de los ejes de coordenadas.
*/
void cambiarValoresdatosMotores(int datosMotores[NUMMOT][3]){
	int flag=0;
	char decision='y';
	int fila=0;
	int columna=0;
	int valor=0;
	while (flag==0){ 
		
		printf("\nIndique la fila que quiere cambiar (Empieza en 1): ");
		scanf(" %d",&fila);
		printf("\nIndique la columna que quiere cambiar (Empieza en 1): ");
		scanf(" %d",&columna);
		printf("\nIntroduce el valor numérico nuevo (< izquierda 1,> derecha 2,^ arriba 3,v abajo 4,. hacia el usuario 5,x hacia el fondo 6)");
		scanf(" %d",&valor);
		printf("\n");
		
		datosMotores[fila-1][columna-1]=valor;
		
		for (int m=0;m<NUMMOT;m++){
			for (int n=0;n<3;n++){
				printf("%c      ",cambiarNumeroSimbolo(datosMotores[m][n]));
			} 
			printf("\n");
		}
		
		printf("Desea seguir cambiando variables? (y/n)\n");
		scanf("  %c", &decision);
		if (decision=='n'){
			flag=1;
		}
	}
}

/**
* @fn void importarDatosCSV()
* @brief permite introducir los datos al programa a partir de un CSV modificable
* previamente creado con este programa.
* @param -
* @return -
*/
void importarDatosCSV(){
	float matriz[10][4];	
	char *record,*line;
	char buffer[1024];
	int i=0,j=0;
	int sumaEjes=0;
	int contador=0;
	FILE *pfichero = fopen("DatosMotoresModificables.csv","r");
	
	if(pfichero == NULL){
      printf("\n No se ha podido abrir el fichero ");
      exit(0);
	}
	
	while(line=fgets(buffer,sizeof(buffer),pfichero)){
		if (contador==1){
			record = strtok(line,"/");
			while(record != NULL){
				printf("RECORD %s\n",record);
				sumaEjes=sumaEjes+atoi(record);
				record = strtok(NULL,"/");
			}
		}
		contador++;
		//if(contador>2){
			if (strstr(line,"VALOR:")){

				record = strtok(line,":");
				record = strtok(NULL,":");
				
				matriz[i][j]=atoi(record);
				
				if(j==3){
					j=0;
					i++;
				}else{
					j++;
				}		
			}
		//}	
	}
	
	guardarMatrizenFichero(matriz);
	
	fclose(pfichero);
	printf("SUMA %d\n", sumaEjes);
	for (int k=0;k<sumaEjes;k++){
		for (int l=0;l<4;l++){
			printf("%f      ",matriz[k][l]);
		} 
		printf("\n");
	}
}


