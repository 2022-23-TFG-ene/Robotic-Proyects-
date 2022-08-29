/**
 * @file programaSustituirVarialbes.c
 * @author Jaime Sáiz de la Peña
 * @brief Este programa parsea las matrices de DH y sustituye los parámetros de
 * DH a partir de un CSV. Además calcula el resultado final. 
 */

#include "stdio.h"
#include <math.h>
#include "stdlib.h"
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h> 

int NUMMOT=2;
int NUMCARACMAX=100000; //1000: válido para 2 - 10000: para 3 - 100000 para 5 y 6
int CONTADOR=0;
float PI = 3.14159265358979323846;

void csvAMatriz(char matriztem[4][4][NUMCARACMAX]);
void inicializarMatrizZeros(char mat[4][4][NUMCARACMAX]);
void sacarListaValores(float matriz[NUMMOT][4],char matriztem[4][4][NUMCARACMAX],float *listaValores[(int)(NUMCARACMAX/4)]);
void parsearMatrizTem(char matriztem[4][4][NUMCARACMAX],float *listaValores[(int)(NUMCARACMAX/4)],char matrizPosFijo[4][4][NUMCARACMAX],int senosycosenos[NUMCARACMAX],int possenoycoseno);
int parsearString(char expresion[NUMCARACMAX],float *listaValores[(int)(NUMCARACMAX/4)],char matrizPosFijo[4][4][NUMCARACMAX],char temporal[NUMCARACMAX],int senosycosenos[NUMCARACMAX],int possenoycoseno);
void matrizCharAString(char temporal[NUMCARACMAX], char cadenaTemp[NUMCARACMAX]);
int prioridad(char x);
void inicializarMatrizZeros2(char cola[NUMCARACMAX]);
void sustitucionYCalculoDatosMatrizParseada(char matrizPosFijo[4][4][NUMCARACMAX],float *listaValores[(int)(NUMCARACMAX/4)],int senosycosenos[NUMCARACMAX],float resultado[4][4]);
float postfixStringAResultado(char matrizPosFijo[NUMCARACMAX],float *listaValores[(int)(NUMCARACMAX/4)],int senosycosenos[NUMCARACMAX]);
void inicializarArrayZeros(int cola[NUMCARACMAX]);
int tipoIntroduccionDatos();
void informacionMotoresAutomatico();
int encontrarEjeY(int x, int z);
int filasCSVParametrosDHespecificos();
void leerfilasCSVParametrosDHespecificos(float matriz[NUMMOT][4]);

//Los valores que se cambien en matriz afectarán a los valores de listaValores porque apuntan a los mismos.
//Por lo que no es necesario crear de nuevo listaValores ya que siempre estará actualizada.
int main(){
	NUMMOT=filasCSVParametrosDHespecificos();	
	float matriz[NUMMOT][4]; 					//matriz donde se almacenan los parámetros de DH
	char matriztem[4][4][NUMCARACMAX]; 			//matriz donde se almacenan la matriz de DH sin operar (con strings en las celdas)
	char matrizPosFijo[4][4][NUMCARACMAX]; 		//matriz donde se guardan las operaciones en postfijo
	float *listaValores[(int)(NUMCARACMAX/4)];	//lista que almacena en orden los valores a sustituir en la matriz de DH
	int senosycosenos[NUMCARACMAX];			//relacción de todos los tipos de valores a sustituir en la matriz de DH
	int possenoycoseno=0;						//marca la posición en la lista de senosycosenos.
	float resultado[4][4];						//matriz final donde se encuentran los valores finales de la matriz calculada de DH
	
	leerfilasCSVParametrosDHespecificos(matriz);
	inicializarMatrizZeros(matrizPosFijo);
	inicializarArrayZeros(senosycosenos);
	inicializarMatrizZeros(matriztem);
	csvAMatriz(matriztem);
	sacarListaValores(matriz,matriztem,listaValores);
	parsearMatrizTem(matriztem,listaValores,matrizPosFijo,senosycosenos,possenoycoseno);
	sustitucionYCalculoDatosMatrizParseada(matrizPosFijo,listaValores,senosycosenos,resultado);
	return 0;

}

/**
* @fn csvAMatriz(char matriztem[4][4][NUMCARACMAX])
* @brief pasa de CSV a matriz la matriz de DH sin haber operado (con strings en las celdas).
* @param matriztem matriz donde se guarda la matriz del CSV.
* @return no devuelve nada pero actualiza la matriz matriztem.
*/
void csvAMatriz(char matriztem[4][4][NUMCARACMAX]){
	char buffer[NUMCARACMAX];
	
	char *record,*line;
	int i=0,j=0;
	FILE *pfichero = fopen("MatrizDHGenerica.csv","r");
	
	if(pfichero == NULL){
      printf("\n No se ha podido abrir el fichero ");
      exit(0);
	}

	for (int i =0;i<4;i++){
		line=fgets(buffer,sizeof(buffer),pfichero);
		record = strtok(line,"/");
		
		for (int j =0;j<4;j++){
			strcpy(matriztem[i][j],record) ;
			record = strtok(NULL,"/");
		}
	}
	fclose(pfichero);
	printf("MATRIZ\n");
	int a =0;
	int b=0;
	for (a=0;a<4;a++){
		for(b=0;b<4;b++){
			printf("%s           " ,matriztem[a][b]);
		}	
		printf("\n\n\n");
	}
}

/**
* @fn void inicializarMatrizZeros(char mat[4][4][NUMCARACMAX])
* @brief inicializa la matriz que recibe con 0.
* @param matriz a inicializar.
* @return no devuelve nada pero actualiza la matriz mat.
*/
void inicializarMatrizZeros(char mat[4][4][NUMCARACMAX]){
	for(int i=0;i<4;i++){
        for(int j=0;j<4;j++){
			strcpy(mat[i][j],"0");
		}
	}
}

/**
* @fn void inicializarMatrizZeros2(char cola[NUMCARACMAX])
* @brief inicializa el array que recibe con 0;
* @param array a inicializar (char).
* @return no devuelve nada pero actualiza el array cola.
*/
void inicializarMatrizZeros2(char cola[NUMCARACMAX]){
	for(int i=0;i<NUMCARACMAX;i++){
        cola[i]='0';
	}
}

/**
* @fn void inicializarArrayZeros(char cola[NUMCARACMAX])
* @brief inicializa el array que recibe con 0;
* @param array a inicializar (int).
* @return no devuelve nada pero actualiza el array cola.
*/
void inicializarArrayZeros(int cola[NUMCARACMAX]){
	for(int i=0;i<NUMCARACMAX;i++){
        cola[i]=0;
	}
}

/**
* @fn sacarListaValores(float matriz[NUMMOT][4],char matriztem[4][4][NUMCARACMAX],float *listaValores[(int)(NUMCARACMAX/4)])
* @brief guarda en listaValores la secuencia de valores que se necesitan para las operaciones (para poder sustituir cuando
* se vaya recorriendo el string de operaciones)
* @param matriz "matriz" dodnde se almacenan la matriz de DH (las operaciones), matriztem matriz donde se almacenan los parámetros
* de DH, listaValores lista en la que se almacenan en el orden a usar los valores de las variables de la matriz "matriz".
* @return no devuelve nada pero actualiza el array cola.
*/
void sacarListaValores(float matriz[NUMMOT][4],char matriztem[4][4][NUMCARACMAX],float *listaValores[(int)(NUMCARACMAX/4)]){
	int posicionListaValores=0;
	
	for(int i=0;i<4;i++){
		for(int j=0;j<4;j++){
			for(int len=0;len<strlen(matriztem[i][j]);len++){
				if(matriztem[i][j][len]=='b'){
					listaValores[posicionListaValores]=&matriz[(atoi(&matriztem[i][j][len+1]))-1][0];
					posicionListaValores++;
				}
				if(matriztem[i][j][len]=='a'){
					listaValores[posicionListaValores]=&matriz[(atoi(&matriztem[i][j][len+1]))-1][1];
					posicionListaValores++;
				}
				if(matriztem[i][j][len]=='r'){
					listaValores[posicionListaValores]=&matriz[(atoi(&matriztem[i][j][len+1]))-1][2];
					posicionListaValores++;
				}
				if(matriztem[i][j][len]=='d'){
					listaValores[posicionListaValores]=&matriz[(atoi(&matriztem[i][j][len+1]))-1][3];
					posicionListaValores++;
				}		
			}
		}
	}	
	
	//Borrar cuando esté
	printf("valores a sustituir en matriz\n");
	for (int z =0;z<=posicionListaValores-1;z++){  
		printf("%f  ",*listaValores[z]);
	}
}

/**
* @fn void parsearMatrizTem(char matriztem[4][4][NUMCARACMAX],float *listaValores[(int)(NUMCARACMAX/4)],char matrizPosFijo[4][4][NUMCARACMAX],int senosycosenos[NUMCARACMAX/100],int possenoycoseno)
* @brief gestiona la iteración de las celdas de matriz para parsear las celdas y obtener un resultado numérico.
* @param matriztem matriz en la que se almacena la matriz de DH sin operar, listaValores es donde se guardan los valores en orden a sustituir
* en  la matriztem, matrizPosFijo es la matriztem pasada a postfijo, senosycosenos es la relacción de operaciones que hay que realizar a los 
* valores de listaValores (coseno, seno, ...)
* @return -
*/
void parsearMatrizTem(char matriztem[4][4][NUMCARACMAX],float *listaValores[(int)(NUMCARACMAX/4)],char matrizPosFijo[4][4][NUMCARACMAX],int senosycosenos[NUMCARACMAX],int possenoycoseno){	
    char temporal[NUMCARACMAX];
    char cadenaTemp[NUMCARACMAX]; 
    
    for(int i=0;i<4;i++){
		for(int j=0;j<4;j++){
			strcpy(cadenaTemp,"");
			inicializarMatrizZeros2(temporal);
			possenoycoseno=parsearString(matriztem[i][j],listaValores,matrizPosFijo,temporal,senosycosenos,possenoycoseno);
			matrizCharAString(temporal,cadenaTemp);
			strcpy(matrizPosFijo[i][j],cadenaTemp);
		}
	}
	
	printf("\n\nMATRIZ PARSEADA\n");
	int a =0;
	int b=0;
	for (a=0;a<4;a++){
		for(b=0;b<4;b++){
			printf("%s           " ,matrizPosFijo[a][b]);
		}	
		printf("\n\n\n");
	}
}

/**
* @fn int parsearString(char expresion[NUMCARACMAX],float *listaValores[(int)(NUMCARACMAX/4)],char matrizPosFijo[4][4][NUMCARACMAX],char temporal[NUMCARACMAX],int senosycosenos[NUMCARACMAX/100],int possenoycoseno)
* @brief parsea la matriz de DH y realiza las operaciones que se recogen en esta. 
* @param expresion es la celda de matriztem a parsear, listaValores almacena los valores en orden a sustituir
* en  la matriztem, matrizPosFijo es la matriztem pasada a postfijo, senosycosenos es la relacción de operaciones que hay que realizar a los 
* valores de listaValores (coseno, seno, ...)
* @return devuelve la posicion que se ha usado hasta el momento a la hora de parsear la matriz. 
*/
int parsearString(char expresion[NUMCARACMAX],float *listaValores[(int)(NUMCARACMAX/4)],char matrizPosFijo[4][4][NUMCARACMAX],char temporal[NUMCARACMAX],int senosycosenos[NUMCARACMAX],int possenoycoseno){
	char pila[NUMCARACMAX];
	char cola[NUMCARACMAX]; 
	inicializarMatrizZeros2(cola);
	inicializarMatrizZeros2(pila);
	
	
    int posicionpila = -1;
    //int senosycosenos[NUMCARACMAX];//1 seno, 2 coseno. 3 -sen
    int posicioncola=0;
    int posicionlistaValores=1;//Empieza en 0 para que el primer valor sea 1 y se vea entre los 0
    char str[10];
    char *ptr;
    char paraPuntero;
    int contador=0; // temporal borrar cuando acabe pruebas
	char str1[2];
	char str2[5] = "";
    
    for(int i =0;i<strlen(expresion);i++){
		if (expresion[i]==' '){
			continue;
		}
		if (expresion[i]=='1'&& posicioncola==0){
			cola[0]='1';
			posicioncola++;
			break;
		}
		if(expresion[i]=='c'||expresion[i]=='s'||expresion[i]=='r'||expresion[i]=='d'||expresion[i]=='-'){ //Añadimos el menos, poruqe solo hay menos delantes de s
			sprintf( str, "%d", posicionlistaValores );
			cola[posicioncola]='2';
			if(expresion[i]== 'c'){
				senosycosenos[possenoycoseno]=2;//2
				i=i+4;
				possenoycoseno++;
			}
			if(expresion[i]== 's'){
				senosycosenos[possenoycoseno]=1;
				i=i+4;
				possenoycoseno++;
			}
			if(expresion[i]== '-'){
				
				if(expresion[i+1]== 's'){
					senosycosenos[possenoycoseno]=3;//3
					i=i+5;
					possenoycoseno++;
				}else{
					senosycosenos[possenoycoseno]=5;
					i=i+2;
					possenoycoseno++;
				}
			}
			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////NUEVO
			if(expresion[i]== 'r'||expresion[i]== 'd'){
				senosycosenos[possenoycoseno]=4;//4
				i=i+1;
				possenoycoseno++;
			}
			
			
			posicionlistaValores++;
			posicioncola++;//hacer en el resto para marcar posicion de cola distinta de posicion de listaValores
			
		}
	
		else if(expresion[i] == '('){
			posicionpila=posicionpila+1;
			pila[posicionpila] = '(';
		}
        
        else if(expresion[i] == ')'){
			while(1){
				if (posicionpila<0){
					break;
				}
				if(pila[posicionpila]== '('){ 
					pila[posicionpila]='0'; 
					posicionpila--;
					break;
				}
				else{
					cola[posicioncola]=pila[posicionpila];
					pila[posicionpila]='0';
					posicionpila--;	
					posicioncola++;
				}
			}
        }
        
        else{
            while(prioridad(pila[posicionpila]) >= prioridad(expresion[i])){
				if(pila[posicionpila]=='('){
						break;
					}
				if (posicionpila>=0){
					cola[posicioncola]=pila[posicionpila];
					pila[posicionpila]='0';
					posicionpila--;
					posicioncola++;
				}
				
				else{
					break;
				}
			}
			
			switch(expresion[i]){
				case '*':
					posicionpila++;
					pila[posicionpila]='*';
					break;
				case '+':
					posicionpila++;
					pila[posicionpila]='+';
					break;
			}
        } 
	}
	while(posicionpila>=0){
		cola[posicioncola]=pila[posicionpila];
		pila[posicionpila]='0';
		posicioncola++;
		posicionpila--;	
	}
	cola[posicioncola]='\0';
	strcpy(temporal,cola);
	
	return possenoycoseno;
}

/**
* @fn int prioridad(char x)
* @brief devuelve la prioridad de distintos operadores.
* @param x es un operador.
* @return devuelve la prioridad de x.
*/
int prioridad(char x){
    if(x=='('){
        return 0;
    }
    if(x =='+'){
        return 1;
    }
    if(x =='*'){
        return 2;
	}
    return 0;
}

/**
* @fn matrizCharAString(char temporal[NUMCARACMAX], char cadenaTemp[NUMCARACMAX])
* @brief 
* @param 
* @return 
*/
void matrizCharAString(char temporal[NUMCARACMAX], char cadenaTemp[NUMCARACMAX]){
	char str1[2];
	str1[1] = '\0';
	for (int i=0;i<NUMCARACMAX && temporal[i]!='0';i++){
		str1[0] = temporal[i];
		strcat(cadenaTemp,str1);
	}
}

/**
* @fn matrizCharAString(char temporal[NUMCARACMAX], char cadenaTemp[NUMCARACMAX])
* @brief Calcula el resultado de la matriz paerseada.
* @param matrizPosFijo matriz parseada, listaValores valores a sustituir en orden en la matriz parseada, senosycosenos son las operaciones a realizar
* sobre los valores de listaValores, resultado es la matriz resultado
* @return 
*/
void sustitucionYCalculoDatosMatrizParseada(char matrizPosFijo[4][4][NUMCARACMAX],float *listaValores[(int)(NUMCARACMAX/4)],int senosycosenos[NUMCARACMAX],float resultado[4][4]){
	
	for (int i=0;i<4;i++){
		for (int j=0;j<4;j++){
			resultado[i][j]=postfixStringAResultado(matrizPosFijo[i][j],listaValores,senosycosenos);
		}		
	}
	printf("\nMatriz Final\n");
	for (int o=0;o<4;o++){
		for(int p=0;p<4;p++){
			printf("%f	",resultado[o][p]);
		}
		printf("\n");
	}
	
}

/**
* @fn float postfixStringAResultado(char arrayPostFijo[NUMCARACMAX],float *listaValores[(int)(NUMCARACMAX/4)],int senosycosenos[NUMCARACMAX/100])
* @brief parsea el array que le llega y aplica las operaciones a cada elemento que se indican en senosucosenos
* @param matrizPosFijo matriz parseada, listaValores valores a sustituir en orden en la matriz parseada, senosycosenos son las operaciones a realizar
* sobre los valores de listaValores, resultado es la matriz resultado
* @return devuelve el valor de cada conjunto de operaciones que hay que realizar en la celda recibida.
*/
float postfixStringAResultado(char arrayPostFijo[NUMCARACMAX],float *listaValores[(int)(NUMCARACMAX/4)],int senosycosenos[NUMCARACMAX]){
	int resultadoTemporal=0;
	float pila[strlen(arrayPostFijo)/100];//Será más pequeño siempre que el número de elementos que haya.
	int posicionPila=0;
	int posicionListaValores=CONTADOR;
	float temporal1=0,temporal2=0;

	if(arrayPostFijo[0]=='\0'){
		return 0;
	}
	
	
	if(arrayPostFijo[0]=='1'){
		return 1;
	}
	
	printf("\npost fijo es, %ld\n",strlen(arrayPostFijo));
	for (int z =0;z<=strlen(arrayPostFijo);z++){  
		printf("%c",arrayPostFijo[z]);
	}
	
	printf("\nESTO ES SENOS Y COSENOS\n");
	for (int q =0;q<=400;q++){  
		printf("%d",senosycosenos[q]);
	}
	
	for (int i =0;i< strlen(arrayPostFijo);i++){
		
		printf("\n\nPILA, iteracion: %d\n",i);
		for (int j=0;j<sizeof(pila);j++ ){
			printf("%f : ",pila[j]);
		}
		printf("\n\n");
		
		if (arrayPostFijo[i]=='2'){
			printf("Es 2, u el valor del arrayPostFijo %d y el valor es %d\n",posicionListaValores,senosycosenos[posicionListaValores]);
			//seno 1 coseno 2 -seno 3
			if(senosycosenos[posicionListaValores]==1){
				printf("Estamos en seno\n");
				printf("El valor es %f, el original es %f \n",sin(*listaValores[posicionListaValores]*(PI/180)) , *listaValores[posicionListaValores]);
				pila[posicionPila]=sin(*listaValores[posicionListaValores]*(PI/180));//Las funciones sin y cos se hacen en radianes por eso el doble cambio, lo redondeamso porque no da 0 ni los valores exactos
				//printf("SENO-------%f",pila[posicionPila]);
			}
			
			if(senosycosenos[posicionListaValores]==2){
				printf("Estamos en coseno\n");
				printf("El valor es %f , el original es %f \n",cos(*listaValores[posicionListaValores]*(PI/180)), *listaValores[posicionListaValores]);
				pila[posicionPila]=cos(*listaValores[posicionListaValores]*(PI/180));
				printf("posicionListaValores: %d\n",posicionListaValores);
				printf("listaValores: %f\n",*listaValores[posicionListaValores]);
				//printf("COSENO-------%f",pila[posicionPila]);
			}
			
			if(senosycosenos[posicionListaValores]==3){
				printf("Estamos en -seno \n");
				printf("El valor es %f , el original es %f \n",-sin(*listaValores[posicionListaValores]*(PI/180)) , *listaValores[posicionListaValores]);
				pila[posicionPila]=-sin(*listaValores[posicionListaValores]*(PI/180));
				//printf("-SENO-------%f",pila[posicionPila]);
			}
			
			if(senosycosenos[posicionListaValores]==4){
				printf("Estamos en R o D*************************************************************************************************************************************************\n");
				pila[posicionPila]=*listaValores[posicionListaValores];
				//printf("R o D-------%f",pila[posicionPila]);
			}
			
			if(senosycosenos[posicionListaValores]==5){
				printf("Estamos en ----- R o D*************************************************************************************************************************************************\n");
				pila[posicionPila]=-*listaValores[posicionListaValores];
				//printf("R o D-------%f",pila[posicionPila]);
			}
			
			posicionPila++;
			posicionListaValores++;	
		}
		else{
			posicionPila--;
			temporal2=pila[posicionPila];
			posicionPila--;
			temporal1=pila[posicionPila];
			pila[posicionPila+1]=0;
			if (arrayPostFijo[i]=='+'){
				pila[posicionPila]=temporal2+temporal1;
				posicionPila++;
			}
			else{
				pila[posicionPila]=temporal2*temporal1;
				posicionPila++;
			}
		}
	}
	
	
	printf("\n\n--------------------------------pila*\n");
	for (int j=0;j<sizeof(pila);j++ ){
		printf("%f : ",pila[j]);
	}
	printf("\n\n");
	printf("-------RESULTADOS: %f",pila[0]);
	CONTADOR=posicionListaValores;
	return pila[0];
}

/**
* @fn int filasCSVParametrosDHespecificos()
* @brief cuenta el número de filas que tiene la matriz de parámetros de DH.
* @param -
* @return devuelve el número de filas que tiene la matriz de parámetros de DH.
*/
int filasCSVParametrosDHespecificos(){
    FILE *fichero;
    int lineas = 0;
    char chr;
    fichero = fopen("ParametrosDHespecificos.csv", "r");
    if(fichero==NULL){
		printf("No se ha encontrado el archivo ParametrosDHespecificos\n");
		exit(0);
	}
    chr = getc(fichero);
    while (chr != EOF){
        if (chr == '\n'){
            lineas = lineas+1;
        }
        chr = getc(fichero);
    }
    fclose(fichero); 
    printf("Hay %d elementos\n", lineas);
    return lineas;
}

/**
* @fn void leerfilasCSVParametrosDHespecificos(float matriz[NUMMOT][4])
* @brief lee y copia las celdas de la matriz de parámetros de DH (CSV).
* @param recibe la matriz donde se guardan dichos datos. 
* @return no devuelve nada pero guarda los datos en matriz pasada por referencia. 
*/
void leerfilasCSVParametrosDHespecificos(float matriz[NUMMOT][4]){
	char buffer[1000];
	
	char *record,*line;
	int i=0,j=0;
	FILE *pfichero = fopen("ParametrosDHespecificos.csv","r");
	
	if(pfichero == NULL){
      printf("\n No se ha podido abrir el fichero ");
      exit(0);
	}
	
	while(line=fgets(buffer,sizeof(buffer),pfichero)){
		record = strtok(line,"/");
		while(record != NULL){
			matriz[i][j]=atoi(record) ;
			record = strtok(NULL,"/");
			j++;
		}
		j=0;
		i++ ;
	}
	fclose(pfichero);
	printf("MATRIZ datos DH obtenidos del CSV\n");
	int a =0;
	int b=0;
	for (a=0;a<NUMMOT;a++){
		for(b=0;b<4;b++){
			printf("%f           " ,matriz[a][b]);
		}	
		printf("\n\n\n");
	}
	
}

