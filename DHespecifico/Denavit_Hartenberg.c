/**
 * @file denavit_hatemberg.c
 * @author Jaime Sáiz de la Peña
 * @brief Este programa crea tanto la matriz de DH Genérica como las distintas 
 * matrices inversas que se necesitan para calcular la cinemática inversa mediante
 * DH. Además realiza la multiplicación de las mismas y las guarda en CSVs.
 */

#include<stdio.h>
#include<stdbool.h>
#include<math.h>
#include<stdlib.h>
#include<string.h>

int NUMCARACMAX=50000;
char matriz[4][10][10];
 
int datosIniciales();
void AnadirMotor(int fila);
void calculaMatrizDH(int filasusadas,char matrizFinal[4][4][NUMCARACMAX], int i);
void multiply(char mat1[4][4][NUMCARACMAX],char mat2[4][4][NUMCARACMAX],char mul[4][4][NUMCARACMAX]);
void inicializarMatrizIdentidad(char mat1[4][4][NUMCARACMAX]);
void copiarMatriz(char mat1[4][4][NUMCARACMAX],char mat2[4][4][NUMCARACMAX]);
void inicializarMatrizZeros(char mat1[4][4][NUMCARACMAX]);
void envolverParentesis(char mat1[4][4][NUMCARACMAX]);
void guardarMatrizenFichero(char mat1[4][4][NUMCARACMAX], int i);
void matrizInversa(char mat1[4][4][NUMCARACMAX], int i);
void multiplicarYGuardarCSVMatricesInversas(int tamano);
void matrizNormalACSV(char mat1[4][4][NUMCARACMAX],int numeroMatriz);
void contarBetas(int tamano);
void contarBetasCSVconcreto(FILE *BETASCSV,char nombre[20],int tamano);

int main(){
	char matrizFinal[4][4][NUMCARACMAX];  //Matriz final o parcial de DH a guardar en un CSV
	int tamano=0;						  //Numero de motores/ejes de coordenadas de nuestro problema
	int i=0;							  //Variable temporal
	printf("**********************************************************************\n");
	printf("* Este programa crea la matriz de DH en funcion de los motores que   *\n");
	printf("* indique el usuario y la guarda en el CSV \"MatrizDHGenerica.csv\".  *\n");
	printf("* Además se crean todas las matrices necesarias para resolver.       *\n");
	printf("*                                                                    *\n");
	printf("*                                                                    *\n");
	printf("* Autor: Jaime Sáiz de la Peña                                       *\n");
	printf("*                                                                    *\n");
	printf("**********************************************************************\n");
	
	tamano=datosIniciales();
	for(int i=0; i<tamano; i++){
		calculaMatrizDH(tamano,matrizFinal,i);
		guardarMatrizenFichero(matrizFinal,i);
	}
	multiplicarYGuardarCSVMatricesInversas(tamano);
	contarBetas(tamano);
}


/**
* @fn int datosIniciales()
* @brief Pide al usuario que introduzca el número de motores con los que se
* desea crear las matrices. Asigna los nombres de las variables de dicha matriz.
* Se recomienda no cambiarlos si se desea trabajar con introduccionDatosMotores.c
* y con programaSustituirVariables.c
* @param -
* @return Devuelve el número de motores con los que se va a trabajar.
*/
int datosIniciales(){
	char temporal [1]="";         //String temporal
	int nmotores=0;				  //Numero de motores/ejes de coordenadas de nuestro problema
	printf("Introduzca el número de motores con los que desea trabajar: ");
	scanf(" %d",&nmotores);
	
	for (int i=0;i<nmotores;i++){
		for (int j=0;j<4;j++){
			//Valores que definen como se van a llamar nuestras variables
			if (j==0){
				sprintf(matriz[i][j], "b%d",i+1);
			}
			if (j==1){
				sprintf(matriz[i][j], "a%d",i+1);
			}
			if (j==2){
				sprintf(matriz[i][j], "r%d",i+1);
			}
			if (j==3){
				sprintf(matriz[i][j], "d%d",i+1);
			}
		}
	}
	return nmotores;
}

/**
* @fn void calculaMatrizDH(int filasusadas,char matrizFinal[4][4][NUMCARACMAX], int i)
* @brief Crea las 4 matrices que forman la matriz final de cada motor. Multiplica los
* motores resultantes creando la matriz final de DH que guarda en un CSV. Crea también 
* las matrices parciales que se usan para sacar las ecuaciones en cinamática inversa.  
* @param int i: marca si se crea la matriz de DH o las matrices parciales para la 
* CI, char matrizFinal[4][4][NUMCARACMAX]: donde se almacenan las matrices que se guardan
* en los CSV, int filasusadas:  es el número de motores del problema.
* @return No devuelve nada pero actualiza la matrizFinal que se pasa por referencia. 
*/
void calculaMatrizDH(int filasusadas,char matrizFinal[4][4][NUMCARACMAX], int i){	
	char matriztem1[4][4][NUMCARACMAX];   //La usaremos si hay datos en beta 
	char matriztem2[4][4][NUMCARACMAX];   //La usaremos si hay datos en alfa
	char matriztem3[4][4][NUMCARACMAX];   //La usaremos si hay datos en r
	char matriztem4[4][4][NUMCARACMAX];   //La usaremos si hay datos en d
	char matrizTem[4][4][NUMCARACMAX];    //Matriz para calcular datos temporales
	char matrizTemb[4][4][NUMCARACMAX];   //Matriz para calcular datos temporales
	char matrizFinalb[4][4][NUMCARACMAX]; //Matriz para calcular datos temporales
	char strtemp[10]="";				  //Lista temporal
	int flag=0;							  //Flag
	char temporal[NUMCARACMAX];			  //Lista temporal
	int numeroMatriz=1;
	
	inicializarMatrizIdentidad(matriztem1);
	inicializarMatrizIdentidad(matriztem2);
	inicializarMatrizIdentidad(matriztem3);
	inicializarMatrizIdentidad(matriztem4);
	inicializarMatrizZeros(matrizTem);
	inicializarMatrizZeros(matrizTemb);
	inicializarMatrizZeros(matrizFinal);

	
	for(i;i<filasusadas;i++){ 
		//Inicialización
		inicializarMatrizIdentidad(matriztem1);
		inicializarMatrizIdentidad(matriztem2);
		inicializarMatrizIdentidad(matriztem3);
		inicializarMatrizIdentidad(matriztem4);
		inicializarMatrizZeros(matrizTem);
		inicializarMatrizZeros(matrizTemb);
		inicializarMatrizZeros(matrizFinalb);
		
		//Beta
		//[0][0]
		strcpy(strtemp,"c(");
		strcat(strtemp,matriz[i][0]);
		strcat(strtemp,")");
		strcpy(matriztem1[0][0],strtemp);
		//[0][1]
		strcpy(strtemp,"(-s(");
		strcat(strtemp,matriz[i][0]);
		strcat(strtemp,"))");
		strcpy(matriztem1[0][1],strtemp);
		//[1][0]
		strcpy(strtemp,"s(");
		strcat(strtemp,matriz[i][0]);
		strcat(strtemp,")");
		strcpy(matriztem1[1][0],strtemp);
		//[1][1]
		strcpy(strtemp,"c(");
		strcat(strtemp,matriz[i][0]);
		strcat(strtemp,")");
		strcpy(matriztem1[1][1],strtemp);
		
		//Alfa
		//[0][0]
		strcpy(strtemp,"c(");
		strcat(strtemp,matriz[i][1]);
		strcat(strtemp,")");
		strcpy(matriztem2[1][1],strtemp);
		//[0][1]
		strcpy(strtemp,"(-s(");
		strcat(strtemp,matriz[i][1]);
		strcat(strtemp,"))");
		strcpy(matriztem2[1][2],strtemp);
		//[1][0]
		strcpy(strtemp,"s(");
		strcat(strtemp,matriz[i][1]);
		strcat(strtemp,")");
		strcpy(matriztem2[2][1],strtemp);
		//[1][1]
		strcpy(strtemp,"c(");
		strcat(strtemp,matriz[i][1]);
		strcat(strtemp,")");
		strcpy(matriztem2[2][2],strtemp);
		
		//R
		//[0][3]
		strcpy(matriztem3[0][3],matriz[i][2]);
		
		//D
		//[2][3]
		strcpy(matriztem4[2][3],matriz[i][3]);
	
		//Orden de multiplicación B d a alfa
		multiply(matriztem1,matriztem4,matrizTemb);
		multiply(matrizTemb,matriztem3,matrizTem);
		matrizNormalACSV(matrizTem,numeroMatriz);
		numeroMatriz=numeroMatriz+1;
		multiply(matrizTem,matriztem2,matrizFinalb);
		envolverParentesis(matrizFinalb);
		printf("MATRIZ DE UN MOTOR\n");
		int c =0;
		int v=0;
		for (c=0;c<4;c++){
			for(v=0;v<4;v++){
				printf("%s           " ,matrizFinalb[c][v]);
			}	
			printf("\n\n\n");
		}
		//En este caso si multiplicamos una matriz identidad por una matriz, pero como 
		//se ha programado la multiplicación no devuelve la matriz no identidad por lo
		//que creamos un flag para la primera iteración de matrizFinal
		if (flag==0){
			flag=1;
			copiarMatriz(matrizFinal,matrizFinalb);
		}else{
			copiarMatriz(matrizTemb,matrizFinal);
			inicializarMatrizZeros(matrizFinal);
			multiply(matrizTemb,matrizFinalb,matrizFinal);
		}
	
		int q =0;
		int w=0;
		for (q=0;q<4;q++){
			for(w=0;w<4;w++){
				if (strcmp(matrizFinal[q][w],"1" )!=0 && strcmp(matrizFinal[q][w],"0" )!=0){
					strcpy(temporal,matrizFinal[q][w]);
					strcpy(matrizFinal[q][w],"(");
					strcat(matrizFinal[q][w],temporal);
					strcat(matrizFinal[q][w],")");
				}
			}	
		}
	}
	printf("MATRIZ ----FINAL/A guardar en un CSV\n");
	int a =0;
	int b=0;
	for (a=0;a<4;a++){
		for(b=0;b<4;b++){
			printf("%s           " ,matrizFinal[a][b]);
		}	
		printf("\n\n\n");
	}
}

/**
* @fn void multiply(char mat1[4][4][NUMCARACMAX],char mat2[4][4][NUMCARACMAX],char mul[4][4][NUMCARACMAX])
* @brief Multiplica las dos primeras matrices que recibe y guarda el resultado en la tercera (mat1*mat2=mul).
* @param mat1 matriz 1 a multiplicar, mat2 matriz 2 a multiplicar (en ese orden), mul matriz donde se guarda el resultado. 
* @return No devuelve nada pero actualiza mul (pasada por referencia)
*/
void multiply(char mat1[4][4][NUMCARACMAX],char mat2[4][4][NUMCARACMAX],char mul[4][4][NUMCARACMAX]){
	char temporal[NUMCARACMAX];
    for(int i=0;i<4;i++){
        for(int j=0;j<4;j++){
            strcpy(temporal,"");
            for(int k=0;k<4;k++){ 
				if(strcmp(mat1[i][k],"0")!=0 && strcmp(mat2[k][j],"0")!=0 && strcmp(mul[i][j],"0")!=0){

					if (strcmp(mat1[i][k],"1" )==0  && strcmp(mat2[k][j],"1" )==0 && strcmp(mul[i][j],"1" )==0 ){
						continue;
					}
					
					strcpy(temporal,mul[i][j]);
					strcat(temporal,"+");
					
					if (strcmp(mat1[i][k],"1" )==0  && strcmp(mat2[k][j],"1" )==0 ){
						strcat(temporal,"1");
						strcpy(mul[i][j],temporal);
						continue;
					}
					
					if (strcmp(mat1[i][k],"1" )==0){
						strcat(mul[i][j],"+");//nuevo
						strcat(mul[i][j],mat2[k][j]);
						continue;
					}
					
					else if(strcmp(mat2[k][j],"1")==0){
						strcat(mul[i][j],"+");//nuevo
						strcat(mul[i][j],mat1[i][k]);
						continue;
					}
					
					else{
						strcat(temporal,mat1[i][k]);
						strcat(temporal,"*");
						strcat(temporal,mat2[k][j]);
						strcpy(mul[i][j],temporal);
					}

					continue;		
				}
				
				if(strcmp(mat1[i][k],"0")!=0 && strcmp(mat2[k][j],"0")!=0 && strcmp(mul[i][j],"0")==0){
					if (strcmp(mat1[i][k],"1" )==0 && strcmp(mat2[k][j],"1" )==0){
						strcpy(mul[i][j],"1");
						continue;
					}
					
					if (strcmp(mat1[i][k],"1" )==0){
						strcpy(mul[i][j],mat2[k][j]);
						continue;
					}
					
					else if(strcmp(mat2[k][j],"1")==0){
						strcpy(mul[i][j],mat1[i][k]);
						continue;
					}
					
					strcpy(temporal,mat1[i][k]);
					strcat(temporal,"*");
					strcat(temporal,mat2[k][j]);
					strcpy(mul[i][j],temporal);
					continue;	
				} 			 
            }
        }
    }
}

/**
* @fn void inicializarMatrizIdentidad(char mat1[4][4][NUMCARACMAX])
* @brief inicializa con la matriz identidad la matriz que se le pasa por parámetros.
* @param mat1 matriz a inicializar.
* @return No devuelve nada pero actualiza la matriz pasada por referencia.
*/
void inicializarMatrizIdentidad(char mat1[4][4][NUMCARACMAX]){
	for(int i=0;i<4;i++){
        for(int j=0;j<4;j++){
			if(i==j){
				strcpy(mat1[i][j],"1");
			}else{
				strcpy(mat1[i][j],"0");
			}			
		}
	}
}

/**
* @fn void inicializarMatrizZeros(char mat1[4][4][NUMCARACMAX])
* @brief inicializa la matriz con zeros.
* @param mat1 matriz a inicializar.
* @return No devuelve nada pero actualiza la matriz pasada por referencia.
*/
void inicializarMatrizZeros(char mat1[4][4][NUMCARACMAX]){
	for(int i=0;i<4;i++){
        for(int j=0;j<4;j++){
			strcpy(mat1[i][j],"0");
		}
	}
}

/**
* @fn void copiarMatriz(char mat1[4][4][NUMCARACMAX],char mat2[4][4][NUMCARACMAX])
* @brief copia la matriz mat2 en mat1.
* @param mat2 matriz a copiar, mat1 matriz donde se copia mat2.
* @return No devuelve nada pero actualiza la matriz pasada por referencia.
*/
void copiarMatriz(char mat1[4][4][NUMCARACMAX],char mat2[4][4][NUMCARACMAX]){
	for(int i=0;i<4;i++){
        for(int j=0;j<4;j++){
			for(int k=0;k<30;k++){
				strcpy(mat1[i][j],mat2[i][j]);
			}
		}
	}
}

/**
* @fn void envolverParentesis(char mat1[4][4][NUMCARACMAX])
* @brief envuelve en parentesis todos los elementos de una matriz (char) a menos que sean 0 o 1
* @param mat1 matriz a envolver en paréntesis.
* @return No devuelve nada pero actualiza la matriz pasada por referencia.
*/
void envolverParentesis(char mat1[4][4][NUMCARACMAX]){
	char temp[100];
	for(int i=0;i<4;i++){
        for(int j=0;j<4;j++){
			if (strcmp(mat1[i][j],"0")!=0 && strcmp(mat1[i][j],"1")!=0){
				strcpy(temp,"(");
				strcat(temp,mat1[i][j]);
				strcat(temp,")");
				strcpy(mat1[i][j], temp);
			}
		}
	}
}

/**
* @fn void guardarMatrizenFichero(char mat1[4][4][NUMCARACMAX], int i)
* @brief guarda la matriz recibida en el csv que la corresponde.
* @param mat1 matriz a guardar, i indica que matriz es. 
* @return -
*/
void guardarMatrizenFichero(char mat1[4][4][NUMCARACMAX], int i){
	char temporal[5];
	char nombre[20];
	if(i==0){
		FILE *fp;
		FILE *fp2;
		fp = fopen("MatrizDHGenerica.csv", "w+");
		fp2 = fopen("MatrizDHGenericaFilas.csv", "w+");
		for (int i =0;i<4;i++){
			//separación " / "
			fprintf(fp,"%s / %s / %s / %s\n", mat1[i][0], mat1[i][1], mat1[i][2], mat1[i][3]);
			fprintf(fp2,"%s\n\n%s\n\n%s\n\n%s\n\n", mat1[i][0], mat1[i][1], mat1[i][2], mat1[i][3]);
		} 
		fclose(fp);
		fclose(fp2);
	}else{
		sprintf(temporal,"%d",i);
		strcpy(nombre,"MatrizNormal-");
		strcat(nombre,temporal);
		strcat(nombre,".csv");
		
		FILE *fp;
		fp = fopen(nombre, "w+");
		for (int i =0;i<4;i++){
			//separación " / "
			fprintf(fp,"%s\n\n%s\n\n%s\n\n%s\n\n", mat1[i][0], mat1[i][1], mat1[i][2], mat1[i][3]);
		} 
		fclose(fp);
	}
}

/**
* @fn matrizInversa(char mat1[4][4][NUMCARACMAX], int i)
* @brief crea la matriz inversa de una matriz de un motor de DH. 
* @param mat1 matriz crear, i indica que matriz es (1,2,3...).
* @return No devuelve nada pero actualiza la matriz pasada por referencia.
*/
void matrizInversa(char mat1[4][4][NUMCARACMAX], int i){
	char temporal[5];
	sprintf(temporal,"%d",i);
	//[0][0]
	strcpy(mat1[0][0],"c(b");
	strcat(mat1[0][0],temporal);
	strcat(mat1[0][0],")");
	//[0][1]
	strcpy(mat1[0][1],"s(b");
	strcat(mat1[0][1],temporal);
	strcat(mat1[0][1],")");
	//[0][2]
	strcpy(mat1[0][2],"0");
	//[0][3]
	strcpy(mat1[0][3],"(-r");
	strcat(mat1[0][3],temporal);
	strcat(mat1[0][3],")");	
	//[1][0]
	strcpy(mat1[1][0],"c(a");
	strcat(mat1[1][0],temporal);
	strcat(mat1[1][0],")*");
	strcat(mat1[1][0],"(-s(b");
	strcat(mat1[1][0],temporal);
	strcat(mat1[1][0],"))");
	//[1][1]
	strcpy(mat1[1][1],"c(a");
	strcat(mat1[1][1],temporal);
	strcat(mat1[1][1],")*");
	strcat(mat1[1][1],"c(b");
	strcat(mat1[1][1],temporal);
	strcat(mat1[1][1],")");
	//[1][2]
	strcpy(mat1[1][2],"s(a");
	strcat(mat1[1][2],temporal);
	strcat(mat1[1][2],")");
	//[1][3]
	strcpy(mat1[1][3],"(-d");
	strcat(mat1[1][3],temporal);
	strcat(mat1[1][3],"*s(a");
	strcat(mat1[1][3],temporal);
	strcat(mat1[1][3],"))");
	//[2][0]
	strcpy(mat1[2][0],"s(a");
	strcat(mat1[2][0],temporal);
	strcat(mat1[2][0],")*");
	strcat(mat1[2][0],"s(b");
	strcat(mat1[2][0],temporal);
	strcat(mat1[2][0],")");
	//[2][1]
	strcpy(mat1[2][1],"(-s(a");
	strcat(mat1[2][1],temporal);
	strcat(mat1[2][1],"))*");
	strcat(mat1[2][1],"c(b");
	strcat(mat1[2][1],temporal);
	strcat(mat1[2][1],")");
	//[2][2]
	strcpy(mat1[2][2],"c(a");
	strcat(mat1[2][2],temporal);
	strcat(mat1[2][2],")");
	//[2][3]
	strcpy(mat1[2][3],"(-d");
	strcat(mat1[2][3],temporal);
	strcat(mat1[2][3],"*s(a");
	strcat(mat1[2][3],temporal);
	strcat(mat1[2][3],"))");
	//[3][0]
	strcpy(mat1[3][0],"0");
	//[3][0]
	strcpy(mat1[3][1],"0");
	//[3][0]
	strcpy(mat1[3][2],"0");
	//[3][0]
	strcpy(mat1[3][3],"1");
}

/**
* @fn void multiplicarYGuardarCSVMatricesInversas(int tamano)
* @brief multiplica las matrices inversas y las guarda en los CSV correspondientes. 
* @param tamano es el número de motores/ejes de coordenadas de nuestro problema.
* @return -
*/
void multiplicarYGuardarCSVMatricesInversas(int tamano){
	char matrizI[4][4][NUMCARACMAX];		//Matriz Inversa
	char matrizItem[4][4][NUMCARACMAX];		//Matriz para calcular datos temporales
	char matrizItem2[4][4][NUMCARACMAX];	//Matriz para calcular datos temporales
	char temporal[5];
	char nombre[20];						//Almacena el nombre de los CSVs a crear
	
	if(tamano>=1){
		matrizInversa(matrizI,1);
		FILE *fp;
		fp = fopen("MatrizInversa-1.csv", "w+");
		if(fp==NULL){
			printf("\n No se ha podido abrir el fichero ");
			exit(0);
		}
		FILE *fp2;
		fp2 = fopen("MatrizI-1.csv", "w+");
		if(fp2==NULL){
			printf("\n No se ha podido abrir el fichero ");
			exit(0);
		}
		for (int i =0;i<4;i++){
			//separación " / "
			fprintf(fp,"%s\n\n%s\n\n%s\n\n%s\n\n", matrizI[i][0], matrizI[i][1], matrizI[i][2], matrizI[i][3]);
			fprintf(fp2,"%s\n\n%s\n\n%s\n\n%s\n\n", matrizI[i][0], matrizI[i][1], matrizI[i][2], matrizI[i][3]);
		}	 
		fclose(fp);
	}
	
	for (int j=2;j<=tamano;j++){
		matrizInversa(matrizItem,j);
		FILE *fp2;
		sprintf(temporal,"%d",j);
		strcpy(nombre,"MatrizI-");
		strcat(nombre,temporal);
		strcat(nombre,".csv");
		fp2 = fopen(nombre, "w+");
		if(fp2==NULL){
			printf("\n No se ha podido abrir el fichero ");
			exit(0);
		}
		for (int a =0;a<4;a++){
			//separación " / "
			fprintf(fp2,"%s\n\n%s\n\n%s\n\n%s\n\n", matrizItem[a][0], matrizItem[a][1], matrizItem[a][2], matrizItem[a][3]);
			}	 
		fclose(fp2);
		inicializarMatrizZeros(matrizItem2);
		multiply(matrizI,matrizItem,matrizItem2);
		copiarMatriz(matrizI,matrizItem2);
		FILE *fp;
		sprintf(temporal,"%d",j);
		strcpy(nombre,"MatrizInversa-");
		strcat(nombre,temporal);
		strcat(nombre,".csv");
		
		fp = fopen(nombre, "w+");
		if(fp==NULL){
			printf("\n No se ha podido abrir el fichero ");
			exit(0);
		}
		for (int a =0;a<4;a++){
			//separación " / "
			fprintf(fp,"%s\n\n%s\n\n%s\n\n%s\n\n", matrizI[a][0], matrizI[a][1], matrizI[a][2], matrizI[a][3]);
			}	 
		fclose(fp);
	}
}

/**
* @fn matrizNormalACSV(char mat1[4][4][NUMCARACMAX],int numeroMatriz)
* @brief guarda las matrices normales a CSV.
* @param mat1 matriz a guardar en un CSV, numeroMatriz indicador del nombre con el que se tiene que guardar.
* @return -
*/
void matrizNormalACSV(char mat1[4][4][NUMCARACMAX],int numeroMatriz){
	char tecontarBetas();
	char temporal[5];
	char nombre[20];						//Almacena el nombre de los CSVs a crear
	FILE *fp;
	sprintf(temporal,"%d",numeroMatriz);
	strcpy(nombre,"MatrizN-");
	strcat(nombre,temporal);
	strcat(nombre,".csv");
	fp = fopen(nombre, "w+");
	if(fp==NULL){
		printf("\n No se ha podido abrir el fichero ");
      exit(0);
	}
	for (int a =0;a<4;a++){
		//separación " / "
		fprintf(fp,"%s\n\n%s\n\n%s\n\n%s\n\n", mat1[a][0], mat1[a][1], mat1[a][2], mat1[a][3]);
		}	 
	fclose(fp);
}	

/**
* @fn contarBetas(int tamano)
* @brief gestiona el conteo de las Betas de los CSV creados.
* @param tamano es el número de motores del problema.
* @return -
*/
void contarBetas(int tamano){
	FILE *BETASCSV;
	BETASCSV=fopen("BETASCSV.csv", "w+");
	fclose(BETASCSV);
	char temporal[2];	
	char nombre[20];
	contarBetasCSVconcreto(BETASCSV,"MatrizDHGenericaFilas.csv",tamano);
	for (int i=0;i<tamano;i++){
		strcpy(nombre,"MatrizInversa-");
		sprintf(temporal,"%d",i+1);
		strcat(nombre,temporal);
		strcat(nombre,".csv");
		contarBetasCSVconcreto(BETASCSV,nombre,tamano);
		if(i+1<tamano){
			strcpy(nombre,"MatrizNormal-");
			sprintf(temporal,"%d",i+1);
			strcat(nombre,temporal);
			strcat(nombre,".csv");
			contarBetasCSVconcreto(BETASCSV,nombre,tamano);
		}
	}


}

/**
* @fn contarBetasCSVconcreto(FILE *BETASCSV,char nombre[20],int tamano){
* @brief cuenta las Betas de los CSV creados.
* @param BETASCSV es el CSV donde se guardan betas de las matrices, 
* nombre es el nombre del CSV a evaluar y tamano es el número de motores del problema.
* @return -
*/
void contarBetasCSVconcreto(FILE *BETASCSV,char nombre[20],int tamano){
	int sumatorio[tamano];
	int temp=0;
	int flag=1;
	char c =' ';
	char cero='0';
	char variableNumero[100];
	char temporal1[10];
	char temporal2[10];
	strcpy(variableNumero," ");
	for(int j=0;j<tamano;j++){
		sumatorio[j]=0;
	}
	FILE *fp;
	fp = fopen(nombre, "r");
	BETASCSV = fopen("BETASCSV.csv", "a+");

	if(fp==NULL){
		printf("\n No se ha podido abrir el fichero ");
      exit(0);
	}
	if(BETASCSV==NULL){
		printf("\n No se ha podido abrir el fichero ");
      exit(0);
	}
	fprintf(BETASCSV,"MATRIZ: %s\n",nombre);
	while(!feof(fp)){
		c=fgetc(fp);
		if(c=='b'){
			c=fgetc(fp);
			temp=c-'0';
			sumatorio[temp-1]=sumatorio[temp-1]+1;
		}
				
		if(c=='\n' && flag==1){
			for (int i=0;i<tamano;i++){
				strcat(variableNumero,"b");
				sprintf(temporal1,"%d",i+1);
				strcat(variableNumero,temporal1);
				strcat(variableNumero,"=");
				sprintf(temporal2,"%d",sumatorio[i]);
				strcat(variableNumero,temporal2);
				strcat(variableNumero," - ");
				sumatorio[i]=0;

			}
			for(int j=0;j<tamano;j++){
				sumatorio[j]=0;
			}
			flag=0;
			fprintf(BETASCSV,"%s\n",variableNumero);
			strcpy(variableNumero," ");
		}else if(c=='\n' && flag==0){
			flag=1;
		}
	}
	fprintf(BETASCSV,"\n\n**********************************\n\n");	
	fclose(fp);
	fclose(BETASCSV);
}










