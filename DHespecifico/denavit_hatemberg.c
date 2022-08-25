#include<stdio.h>
#include<stdbool.h>
#include<math.h>
#include<stdlib.h>
#include<string.h>

int NUMCARACMAX=100000;
char  matriz[4][10][10];
 
int datosIniciales();
void AnadirMotor(int fila);
void calculaMatrizDH(int filasusadas,char matrizFinal[4][4][NUMCARACMAX]);
void multiply(char mat1[4][4][NUMCARACMAX],char mat2[4][4][NUMCARACMAX],char mul[4][4][NUMCARACMAX]);
void inicializarMatrizIdentidad(char mat1[4][4][NUMCARACMAX]);
void copiarMatriz(char mat1[4][4][NUMCARACMAX],char mat2[4][4][NUMCARACMAX]);
void inicializarMatrizZeros(char mat1[4][4][NUMCARACMAX]);
void envolverParentesis(char mat1[4][4][NUMCARACMAX]);
void guardarMatrizenFichero(char mat1[4][4][NUMCARACMAX]);


int main(){
	char matrizFinal[4][4][NUMCARACMAX];
	int tamano=0;
	
	printf("************************************************************\n");
	printf("* Este programa crea la matriz de DH de los motores que    *\n");
	printf("* indique el usuario.                                      *\n");
	printf("*                                                          *\n");
	printf("* Dicha matriz es creada en un archivo CSV.                *\n");
	printf("*                                                          *\n");
	printf("* Autor: Jaime Sáiz de la Peña                             *\n");
	printf("*                                                          *\n");
	printf("************************************************************\n");
	
	tamano=datosIniciales();
	calculaMatrizDH(tamano,matrizFinal);
	guardarMatrizenFichero(matrizFinal);
return 0;
}

int datosIniciales(){
	char temporal [1]="";
	int nmotores=0;
	printf("Introduzca el número de motores con los que desea trabajar: ");
	scanf(" %d",&nmotores);
	for (int i=0;i<nmotores;i++){
		for (int j=0;j<4;j++){
			if (j==0){
				//strcpy(matriz[i][j],"b");
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

//Esta función lo que hará será añadir Motores a la secuencia de motores y Segmentos
//de forma manual (sustituiría a datosIniciales que lo hace de forma automática)
/*
void AnadirMotor(int fila){
	printf("Introducir Motor beta alfa r d (separado por espacios)\n");
	scanf("%s" "%s" "%s" "%s",matriz[fila][0],matriz[fila][1],matriz[fila][2],matriz[fila][3] );
}
*/

void calculaMatrizDH(int filasusadas,char matrizFinal[4][4][NUMCARACMAX]){
	
	//numero de elementos, filas, columnas
	
	char matriztem1[4][4][NUMCARACMAX]; //La usaremos si hay datos en beta 
	char matriztem2[4][4][NUMCARACMAX]; //La usaremos si hay datos en alfa
	char matriztem3[4][4][NUMCARACMAX]; //La usaremos si hay datos en r
	char matriztem4[4][4][NUMCARACMAX]; //La usaremos si hay datos en d
	char matrizTem[4][4][NUMCARACMAX]; //Matriz para calcular datos temporales
	char matrizTemb[4][4][NUMCARACMAX];
	//char matrizFinal[4][4][NUMCARACMAX]; //Matriz Final
	char matrizFinalb[4][4][NUMCARACMAX];

	int flag=0;
	
	inicializarMatrizIdentidad(matriztem1);
	inicializarMatrizIdentidad(matriztem2);
	inicializarMatrizIdentidad(matriztem3);
	inicializarMatrizIdentidad(matriztem4);
	inicializarMatrizZeros(matrizTem);
	inicializarMatrizZeros(matrizTemb);
	inicializarMatrizZeros(matrizFinal);

	char strtemp[10]="";
	for(int i=0;i<filasusadas;i++){ 
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
		multiply(matrizTem,matriztem2,matrizFinalb);
		envolverParentesis(matrizFinalb);
		printf("MATRIZ ----MUUUUUUUUUUUUUUUUUUUL\n");
		int c =0;
		int v=0;
		for (c=0;c<4;c++){
			for(v=0;v<4;v++){
				printf("%s           " ,matrizFinalb[c][v]);
			}	
			printf("\n\n\n");
		}
		//En este caso si multiplicamos una matriz identidad por una matriz, pro como 
		//se ha programado la multiplicación no devuelve la matriz no identidad por lo
		//que creamos un flag para la primera iteración de matrizFinal
		if (flag==0){
			flag=1;
			copiarMatriz(matrizFinal,matrizFinalb);
		}else{
			copiarMatriz(matrizTemb,matrizFinal);
			inicializarMatrizZeros(matrizFinal);
			printf("\n\nMATRIZ ----matrizTemb\n");
			multiply(matrizTemb,matrizFinalb,matrizFinal);
		}
		
		char temporal[NUMCARACMAX];
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
	printf("MATRIZ ----FINAL\n");
	int a =0;
	int b=0;
	for (a=0;a<4;a++){
		for(b=0;b<4;b++){
			printf("%s           " ,matrizFinal[a][b]);
		}	
		printf("\n\n\n");
	}

}

//Multiplica dos matrices y devuelve la matriz resultante. 
void multiply(char mat1[4][4][NUMCARACMAX],char mat2[4][4][NUMCARACMAX],char mul[4][4][NUMCARACMAX]){
	char temporal[NUMCARACMAX];
    for(int i=0;i<4;i++){
        for(int j=0;j<4;j++){
            strcpy(temporal,"");
            for(int k=0;k<4;k++){ 
                //if(strcmp(mat1[i][k],"0")!=0 && strcmp(mat2[k][j],"0")!=0 ){
				if(strcmp(mat1[i][k],"0")!=0 && strcmp(mat2[k][j],"0")!=0 && strcmp(mul[i][j],"0")!=0){
					
					
					//printf("Opcion 1\n");
					//entra siempre

//					if (strcmp(mat1[i][k],"1" )==0  && strcmp(mat2[k][j],"1" )==0 && strcmp(mul[k][j],"1" )==0 ){		//Comprobar k o i
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
					}else if(strcmp(mat2[k][j],"1")==0){
						strcat(mul[i][j],"+");//nuevo
						strcat(mul[i][j],mat1[i][k]);
						continue;
					}else{
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
						//strcpy(mul[i][j],mat2[k][j]);
						continue;
					}else if(strcmp(mat2[k][j],"1")==0){
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

void inicializarMatrizZeros(char mat1[4][4][NUMCARACMAX]){
	for(int i=0;i<4;i++){
        for(int j=0;j<4;j++){
			strcpy(mat1[i][j],"0");
		}
	}
}

void copiarMatriz(char mat1[4][4][NUMCARACMAX],char mat2[4][4][NUMCARACMAX]){
	for(int i=0;i<4;i++){
        for(int j=0;j<4;j++){
			for(int k=0;k<30;k++){
				strcpy(mat1[i][j],mat2[i][j]);
			}
		}
	}
}

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

void guardarMatrizenFichero(char mat1[4][4][NUMCARACMAX]){
	FILE *fp;
	
	fp = fopen("MatrizDHGenerica.csv", "w+");
	
	for (int i =0;i<4;i++){
		//separación " / "
		fprintf(fp,"%s / %s / %s / %s\n", mat1[i][0], mat1[i][1], mat1[i][2], mat1[i][3]);
	} 
	
	fclose(fp);
}













