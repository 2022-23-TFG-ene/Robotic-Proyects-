#include<stdio.h>
#include<stdbool.h>
#include<math.h>
#include<stdlib.h>


float matriz[10][4]; 
float PI = 3.14159265358979323846;

void AnadirMotor(int fila);
void AnadirBrazo(int fila);
void calculaMatrizDH(int filasusadas);
void multiply(float mat1[4][4],float mat2[4][4],float mul[4][4]);
void inicializarMatrizIdentidad(float mat1[4][4]);
void copiarMatriz(float mat1[4][4],float mat2[4][4]);


int main(){
	printf("************************************************************\n");
	printf("* Introduzca los motores y brazos que se van a usar.       *\n");
	printf("* El primer elemento a introducir deberá ser un motor.     *\n");
	printf("* Posteriormente se irán intercalando motores y segmentos. *\n");
	printf("* Los segmentos no tienen dirección ya uqe va determinada  *\n");
	printf("* Por el motor.											   *\n");
	printf("************************************************************\n");
	//Contador tamaño
	int tamano=0;
	while(1){
		AnadirMotor(tamano);
		tamano=tamano+1;
		printf( "Pulsa cualquier tecla para continuarañadiendo motores/segmentos \n" );
		printf( "Pulsa Q para acabar.\n" );
		char c;
		scanf(" %c", &c);
		if (c == 'Q'){
			break;
		} 
	}
	calculaMatrizDH(tamano);
return 0;
}

//Esta función lo que hará será añadir Motores a la secuencia de motores y Segmentos
void AnadirMotor(int fila){
	//beta->giro sobre z, alfa-> giro sobre x, r-> distancia entre ejes x de motores, d-> distancia en z
	//z apunta hacia donde se desplaza y si es giro sobre el pivote que se gira
	printf("Introducir Motor beta alfa r d (separado por espacios)\n");
	scanf("%f" "%f" "%f" "%f",&matriz[fila][0],&matriz[fila][1],&matriz[fila][2],&matriz[fila][3] );
	//Los motores serán representados con un 0
	matriz[fila][4]=0;
	//pasamos de grados a radianes
	matriz[fila][0]=matriz[fila][0];//*(3.14/180) para pasar a radianes
	matriz[fila][1]=matriz[fila][1];//*(3.14/180) para pasar a radianes

}


void calculaMatrizDH(int filasusadas){
	float matriztem1[4][4]; //La usaremos si hay datos en beta 
	float matriztem2[4][4]; //La usaremos si hay datos en alfa
	float matriztem3[4][4]; //La usaremos si hay datos en r
	float matriztem4[4][4]; //La usaremos si hay datos en d
	float matrizTem[4][4]; //Matriz para calcular datos temporales
	float matrizTemb[4][4];
	float matrizFinal[4][4]; //Matriz Final
	float matrizFinalb[4][4];

	//inicializar
	inicializarMatrizIdentidad(matriztem1);
	inicializarMatrizIdentidad(matriztem2);
	inicializarMatrizIdentidad(matriztem3);
	inicializarMatrizIdentidad(matriztem4);
	inicializarMatrizIdentidad(matrizTem);
	inicializarMatrizIdentidad(matrizFinal);
	inicializarMatrizIdentidad(matrizTemb);
	inicializarMatrizIdentidad(matrizFinalb);

	for(int i=0;i<filasusadas;i++){      //Recorremos todas las filas usadas y finalmente multiplicaremos todas. 
        inicializarMatrizIdentidad(matriztem1);
		inicializarMatrizIdentidad(matriztem1);
		inicializarMatrizIdentidad(matriztem1);
		inicializarMatrizIdentidad(matriztem1);
		inicializarMatrizIdentidad(matrizTem);
		
		//Beta
		matriztem1[0][0]=cos(matriz[i][0]*(PI/180));
		matriztem1[0][1]=-sin(matriz[i][0]*(PI/180));
		matriztem1[1][0]=sin(matriz[i][0]*(PI/180));
		matriztem1[1][1]=cos(matriz[i][0]*(PI/180));
		//Alfa
		matriztem2[1][1]=cos(matriz[i][1]*(PI/180));
		matriztem2[1][2]=-sin(matriz[i][1]*(PI/180));
		matriztem2[2][1]=sin(matriz[i][1]*(PI/180));
		matriztem2[2][2]=cos(matriz[i][1]*(PI/180));
		//r
		matriztem3[0][3]=matriz[i][2];
		//d
		matriztem4[2][3]=matriz[i][3];

		//Realizamos las multiplicaciones necesarias
		//El orden de multiplicaciones debe ser beta, d, r, alfa
		multiply(matriztem1,matriztem4,matrizTem);
		multiply(matrizTem,matriztem3,matrizTemb);
		multiply(matrizTemb,matriztem2,matrizTem);
		copiarMatriz(matrizFinal,matrizFinalb);
		multiply(matrizFinalb,matrizTem,matrizFinal);
		}	
	printf("MATRIZ_----FINAL \n");
	int q =0;
	int r=0;
	for (q=0;q<4;q++){
		for(r=0;r<4;r++){
			printf("%f " ,roundf(matrizFinal[q][r]));
		}
	printf("\n");
	}
}

//Multiplica dos matrices y devuelve la matriz resultante. 
void multiply(float mat1[4][4],float mat2[4][4],float mul[4][4]){
	float valor=0;//Pruebo esto porque antes daba error llamar a matrices donde se guarda
    for(int i=0;i<4;i++){
        for(int j=0;j<4;j++){
            mul[i][j]=0;
            for(int k=0;k<4;k++){
                mul[i][j]=mul[i][j]+mat1[i][k]*mat2[k][j];
            }
        }
    }
}

void inicializarMatrizIdentidad(float mat1[4][4]){
	for(int i=0;i<4;i++){
        for(int j=0;j<4;j++){
			if(i==j){
				mat1[i][j]=1;
			}else{
				mat1[i][j]=0;
			}			
		}
	}
}

void copiarMatriz(float mat1[4][4],float mat2[4][4]){
	for(int i=0;i<4;i++){
        for(int j=0;j<4;j++){
			mat2[i][j]=mat1[i][j];
		}
	}
}
