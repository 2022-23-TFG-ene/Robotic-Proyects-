#include "stdio.h"
#include <math.h>
#include "stdlib.h"
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h> 


int NUMMOT=2;

void itroducirMotores(float matriz[NUMMOT][4]);
void itroducirMotoresAutomatico(float matriz[NUMMOT][4],int angulos90);
int tipoIntroduccionDatos();
void informacionMotoresAutomatico();
int encontrarEjeY(int x, int z);
void introducimosEjesMotores(int datosMotores[NUMMOT][3],int ejes90[10]);
void introducirDatosMatrizMotores(int datosMotores[NUMMOT][3],float matriz[NUMMOT][4]);
char cambiarNumeroSimbolo(int n);
int cambiarSimboloNumero(char n);
void guardarMatrizenFichero(float matriz[NUMMOT][4]);

int main(){
	int introduccionDatos=0;
	int angulos90=0;
	printf("el número de veces que para llegar de un motor a otro, te tienes que desplazar en más de dos sentidos respecto a los ejes de coordenadas de la base\n ");
	scanf("%d",&angulos90);	
	printf("Introduzca el número de motores + el número de ángulos de 90º entre ejes de motores: ");
	scanf(" %d",&NUMMOT);
	NUMMOT=NUMMOT+angulos90;
	float matriz[NUMMOT][4];
	
	introduccionDatos=tipoIntroduccionDatos();
	
	if (introduccionDatos==1){
		itroducirMotores(matriz);
	}else if (introduccionDatos==2){
		itroducirMotoresAutomatico(matriz,angulos90);
	}
	
	for (int i=0;i<NUMMOT-1;i++){
		for (int j=0;j<4;j++){
				printf("%f      ",matriz[i][j]);
			} 
			printf("\n");
	}
	guardarMatrizenFichero(matriz);
	return 0;

}

int tipoIntroduccionDatos(){
	int introduccionDatos=0;
	printf("\nIntroduzca la forma en la que desea introducir los motores: \n");
	printf("	- \"1\" si desea hacerlo de forma manual \n");
	printf("	- \"2\" si desea seguir los pasos de DH \n");
	scanf(" %d",&introduccionDatos);
	return introduccionDatos;
}

void itroducirMotores(float matriz[NUMMOT][4]){
	for (int i =0;i<NUMMOT;i=i+1){
		printf("Para introducir los datos correctamente, siga los pasos que se muestran a continuación:\n\n");
		printf("\t0) Cree una matriz de 4 x nº de motores\n");
		printf("\t   La primera fila representa el giro de nuestro motor desde el origen (beta)\n");
		printf("\t   La segunda el desplazamiento en z para que coincidan los origenes (distancia entre ejes de motores) (d)\n");
		printf("\t   La tercera el desplazamiento en x (r)\n");
		printf("\t   La cuarta la rotación para qeu coincian los ejes z (alfa)\n");
		printf("\t   ***T(i-2)/i=rotz(betai)*transz(di)*transx(ri)*rotx(alfai)***\n");
		printf("\t1) Situe z0 hasta zn estando z0 en la base zn en la punta del brazo.\n");
		printf("\t   Se debe situar las z de forma coincidente a los ejes de los motores.\n");
		printf("\t   z0 irá en la dirección del segmento entre la base y la articulación 1\n");
		printf("\t2) Añada x1 (está en la normal de z0 y z1)\n");
		printf("\t   Añada x0 si es posible en la misma dirección que x1\n");
		printf("\t   Añada hasta xn respetando la regla de la mano derecha y que x tiene que estar en la normal de zn y zn-1\n");
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
		
		printf("Introducir Motor beta alfa r d (separado por espacios) y de forma creciente\n");
		scanf("%f" "%f" "%f" "%f",&matriz[i][0],&matriz[i][1],&matriz[i][2],&matriz[i][3]);
	}
}

void itroducirMotoresAutomatico(float matriz[NUMMOT][4],int angulos90){
	int datosMotores[NUMMOT][3]; 	
	int ejes90[10]; //podemos guardar hasta 10 ejes (no se llega en ningún caso normal)
	int temporal=0;
	int flag=0;
	int fila=0;
	int columna=0;
	char valor=0;
	char decision=' ';
	for(int i=0;i< angulos90;i++){
		printf("Introduzca las posiciones en las para llegar de un motor a otro tienes que moverte en más de 2 direcciones. Ej  2 significa que está entre el motor 1 y el 2\n");
		printf("Introducir de una en una pulsando enter");
		scanf("%d",&temporal);
		ejes90[i]=temporal;
	}
	
	
	informacionMotoresAutomatico();
	introducimosEjesMotores(datosMotores,ejes90);
	
	for (int i=0;i<NUMMOT-1;i++){
		for (int j=0;j<3;j++){
				printf("%c      ",cambiarNumeroSimbolo(datosMotores[i][j]));
			} 
			printf("\n");
	}
	
	while (flag==0){ 
		printf("Desea cambiar alguna x de la matriz? Cambiar si para llegar al motor i ni la xi ni zi-1 apuntan hacia el siguiente motor (La x no puede ser igual que la z) y/n");
		scanf(" %c", &decision);
		if (decision=='n'){
			flag=1;
		}else{
			printf("\nIndique la fila que quiere cambiar (Empieza en 1): ");
			scanf(" %d",&fila);
			printf("\nIntroduce el valor caracter nuevo (< izquierda 1,> derecha 2,^ arriba 3,v abajo 4,. hacia el usuario 5,x hacia el fondo 6)");
			scanf(" %c",&valor);
			printf("\n");
			
			datosMotores[fila-1][0]=valor;
			
			printf("****************Esto es lo que devuelve el cambio %d\n",encontrarEjeY(valor,datosMotores[fila-1][2]));
			
			datosMotores[fila-1][1]=cambiarNumeroSimbolo(encontrarEjeY(cambiarSimboloNumero(valor),datosMotores[fila-1][2]));
			
			for (int m=0;m<NUMMOT-1;m++){
				for (int n=0;n<3;n++){
					printf("%c      ",cambiarNumeroSimbolo(datosMotores[m][n]));
				} 
			printf("\n");
			}
		}
	}
	
	
	introducirDatosMatrizMotores(datosMotores,matriz);
}

void introducimosEjesMotores(int datosMotores[NUMMOT][3],int ejes90[10]){
	//NUMMOT  //Numero de motores del problema.
	// x y z (< izquierda 1,> derecha 2,^ arriba 3,v abajo 4,. hacia el usuario 5,x hacia el fondo 6)
	int temp=0;
	int sentido=0;
	int noMotor=0;
	//colocamos las z
	for (int i=1;i<NUMMOT;i++){
		if(ejes90[noMotor]-1==i-1){ //Cuando no es un motor
			printf("El elememnto a analizar no es un motor:\n");
			printf("El eje conecta dos motores, indique el sentido y dirección del segmento que une el vertice con el segundo motor\n");
			printf("(< izquierda 1,> derecha 2,^ arriba 3,v abajo 4,. hacia el usuario 5,x hacia el fondo 6)");
			scanf("%d", &sentido);
			datosMotores[i-1][2]=sentido;
			noMotor=noMotor+1;
		}else{
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
	}
	//colocamos las x
	for (int i=1;i<=NUMMOT;i++){
		if (i==1){
			datosMotores[i-1][0]=1;
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
	// x y z (< izquierda 1,> derecha 2,^ arriba 3,v abajo 4,. hacia el usuario 5,x hacia el fondo 6)
}

void introducirDatosMatrizMotores(int datosMotores[NUMMOT][3],float matriz[NUMMOT][4]){
	int distancia=0;
	int giro=0;
	printf("\nIntroduzca la distancia de la base al primer motor:");
	scanf("%d", &distancia);
	// beta alfa r d
	matriz[0][0]=0;
	matriz[0][3]=distancia;
	matriz[0][2]=0;
	matriz[0][1]=0;
	
	printf("A continuación se van a suceder los motores y ejes definidos anteriormente. Se deben contestar a las siguientes preguntas usando siempre la regla de la mano derecha.\n");
	for (int i=1;i<NUMMOT-1;i++){
		//Giro sobre z para que coincida x - Beta
		printf("Estamos en el elemento %d empezando por la base.\n",i+1);
		printf("El eje x %d es %c y el eje x del motor %d es %c\n",i , cambiarNumeroSimbolo(datosMotores[i-1][0]),i+1,cambiarNumeroSimbolo(datosMotores[i][0]));
		printf("Para llegar del x %d al x %d (sobre z %c) cuanto hay que girar (regla de la mano derecha)(0,90,180,-90)\n",i, i+1,cambiarNumeroSimbolo(datosMotores[i-1][2]));
		scanf(" %d", &giro);
		matriz[i][0]=giro;
		giro=0;
		//Desplazamiento sobre el eje z - d
		printf("Cuanto hay que desplazarse sobre el eje z (%c) desde el elemento (%d) para llegar al siguiente elemento (%d)?\n",cambiarNumeroSimbolo(datosMotores[i-1][2]),i,i+1);
		scanf(" %d",&distancia);
		matriz[i][3]=distancia;
		
		//Desplazamiento sobre el eje x - r
		printf("Cuanto hay que desplazarse sobre el eje x desde el elemento (%d) para llegar al siguiente elemento (%d)?\n",i,i+1);
		printf("Tener en cuenta el desplazamiento que se ha hecho anteriormente (era %c y se ha aplicado un desplazamiento de %f) ",cambiarNumeroSimbolo(datosMotores[i-1][0]),matriz[i][0]);
		scanf(" %d",&distancia);
		matriz[i][2]=distancia;
		
		//Giro sobre x para que coincida z -alfa
		printf("El eje z %d es %c y el eje z del motor %d es %c (x es %c)\n",i , cambiarNumeroSimbolo(datosMotores[i-1][2]),i+1,cambiarNumeroSimbolo(datosMotores[i][2]),cambiarNumeroSimbolo(datosMotores[i-1][0]));
		printf("Para llegar del z %d al z %d (sobre z) cuanto hay que girar (regla de la mano derecha)(0,90,180,-90)\n",i, i+1);
		scanf(" %d", &giro);
		matriz[i][1]=giro;
		giro=0;
		
		printf("***********************************\n\n");
	}
}

char cambiarNumeroSimbolo(int n){
	//printf("Entraaaaaaaaaaaaaa1 %d\n",n);
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

int cambiarSimboloNumero(char n){
	//printf("Entraaaaaaaaaaaaaa2, %c \n",n);
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


//tambien sirve para sacar la x si mentemos la z del 2 motor y la z del primer motor en ese orden. 
int encontrarEjeY(int x, int z){
	// x y z (< izquierda 1,> derecha 2,^ arriba 3,v abajo 4,. hacia el usuario 5,x hacia el fondo 6)
	//printf("Entraaaaaaaaaaaaaaaaaaaa3, %d - %d\n", x,z);
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
						printf("*********************************");
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
	printf("\tp2 Inicialmente colocaremos el robot de tal manera uqe los ejes de los motores esten perpendiculares o paralelos al usuario.");
}



void guardarMatrizenFichero(float matriz[NUMMOT][4]){
	FILE *fp;
	
	fp = fopen("ParametrosDHespecificos.csv", "w+");
	
	for (int i =0;i<NUMMOT;i++){
		//separación " / "
		//Se guarda en otro orden para que cuadre con el siguiente programa. 
		fprintf(fp,"%f / %f / %f / %f\n", matriz[i][0], matriz[i][3], matriz[i][2], matriz[i][1]);
	} 
	fclose(fp);
}
