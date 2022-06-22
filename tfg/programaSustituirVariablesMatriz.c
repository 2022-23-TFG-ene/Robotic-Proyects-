#include "stdio.h"
#include <math.h>
#include "stdlib.h"
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>


int NUMMOT=2;
int NUMCARACMAX=100;

void itroducirMotores(float matriz[NUMMOT][4]);
void csvAMatriz(char matriztem[4][4][NUMCARACMAX]);
void inicializarMatrizZeros(char mat[4][4][NUMCARACMAX]);
void sacarListaValores(float matriz[NUMMOT][4],char matriztem[4][4][NUMCARACMAX],float *listaValores[(int)(NUMCARACMAX/4)]);
void parsearMatrizTem(char matriztem[4][4][NUMCARACMAX],float *listaValores[(int)(NUMCARACMAX/4)],char matrizPosFijo[4][4][NUMCARACMAX]);
void parsearString(char expresion[NUMCARACMAX],float *listaValores[(int)(NUMCARACMAX/4)],char matrizPosFijo[4][4][NUMCARACMAX]);

int prioridad(char *x);
void inicializarMatrizZeros2(char cola[NUMCARACMAX][14]);
//Los valores que se cambien en matriz afectarán a los valores de listaValores porque apuntan a los mismos.
//Por lo que no es necesario crear de nuevo listaValores ya que siempre estará actualizada.
int main(){
	float matriz[NUMMOT][4]; 
	char matriztem[4][4][NUMCARACMAX]; 
	char matrizPosFijo[4][4][NUMCARACMAX]; 
	//inicializarMatrizZeros(matrizPosFijo);
	float *listaValores[(int)(NUMCARACMAX/4)];
	//inicializarArray(listaValores);
	
	itroducirMotores(matriz);
	inicializarMatrizZeros(matriztem);
	csvAMatriz(matriztem);
	sacarListaValores(matriz,matriztem,listaValores);
	parsearMatrizTem(matriztem,listaValores,matrizPosFijo);
	
	return 0;
}

void itroducirMotores(float matriz[NUMMOT][4]){
	for (int i =0;i<NUMMOT;i=i+1){
		printf("Introducir Motor beta alfa r d (separado por espacios) y de forma creciente\n");
		scanf("%f" "%f" "%f" "%f",&matriz[i][0],&matriz[i][1],&matriz[i][2],&matriz[i][3]);
	}
}

void csvAMatriz(char matriztem[4][4][NUMCARACMAX]){
	char buffer[1024];
	
	char *record,*line;
	int i=0,j=0;
	FILE *pfichero = fopen("MatrizDHGenerica.csv","r");
	
	if(pfichero == NULL){
      printf("\n No se ha podido abrir el fichero ");
      exit(0);
	}
	
	while(line=fgets(buffer,sizeof(buffer),pfichero)){
		record = strtok(line,"/");
		
		while(record != NULL){
			strcpy(matriztem[i][j],record) ;
			record = strtok(NULL,"/");
			j++;
		}
		j=0;
		i++ ;
	}
	
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


void inicializarMatrizZeros(char mat[4][4][NUMCARACMAX]){
	for(int i=0;i<4;i++){
        for(int j=0;j<4;j++){
			strcpy(mat[i][j],"0");
		}
	}
}

void inicializarMatrizZeros2(char cola[NUMCARACMAX][14]){
	for(int i=0;i<NUMCARACMAX-1;i++){
        strcpy(cola[i],"0");
	}
}

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
	for (int z =0;z<=posicionListaValores-1;z++){  
		printf("%f  ",*listaValores[z]);
	}
}

void parsearMatrizTem(char matriztem[4][4][NUMCARACMAX],float *listaValores[(int)(NUMCARACMAX/4)],char matrizPosFijo[4][4][NUMCARACMAX]){	
    parsearString(matriztem[0][0],listaValores,matrizPosFijo);
    /*
    for(int i=0;i<4;i++){
		for(int j=0;j<4;j++){
			printf("\n\n\n NUEVA ITERACION: %d-%d", i, j);
			parsearString(matriztem[i][j],listaValores,matrizPosFijo);
		}
	}
	*/
}

void parsearString(char expresion[NUMCARACMAX],float *listaValores[(int)(NUMCARACMAX/4)],char matrizPosFijo[4][4][NUMCARACMAX]){
	char pila[NUMCARACMAX][14];
	char cola[NUMCARACMAX][14]; 
	inicializarMatrizZeros2(cola);
	inicializarMatrizZeros2(pila);
	
    int posicionpila = -1;
    int senosycosenos[NUMCARACMAX];//1 seno, 2 coseno. 3 -sen
    int posicioncola=0;
    int posicionlistaValores=1;//Empieza en 0 para que el primer valor sea 1 y se vea entre los 0
    char str[10];
    char *ptr;
    int contador=0; // temporal borrar cuando acabe pruebas
	
    
    for(int i =0;i<strlen(expresion);i++){
		if (expresion[i]==' '){
			break;
		}
		printf("\n\nEXPRESION: %c, POSCOLA: %d, POSPILA: %d\n",expresion[i],posicioncola,posicionpila);
		if(expresion[i]=='c'||expresion[i]=='s'||expresion[i]=='r'||expresion[i]=='d'||expresion[i]=='-'){ //Añadimos el menos, poruqe solo hay menos delantes de s
			sprintf( str, "%d", posicionlistaValores );
			strcpy(cola[posicioncola],str);
			
			if(expresion[i]== 'c'){
				senosycosenos[posicioncola]=2;
				i=i+4;
			}
			if(expresion[i]== 's'){
				senosycosenos[posicioncola]=1;
				i=i+4;
			}
			
			if(expresion[i]== '-'){
				printf("**********************************");
				senosycosenos[posicioncola]=3;
				i=i+5;
			}
			posicionlistaValores++;
			posicioncola++;//hacer en el resto para marcar posicion de cola distinta de posicion de listaValores
			
		}

		
		else if(expresion[i] == '('){
			posicionpila=posicionpila+1;
			strcpy(pila[posicionpila] , "(");
		}
        
        else if(expresion[i] == ')'){
			//printf("ESTAMOS EN EL ELSE IF");
			
			while(1){
				printf("\nEstamos aqui, posicion pila: %d", posicionpila);
				printf("El valor en pila es %s",pila[posicionpila]);
				if (posicionpila<0){
					break;
				}
				
				if(strcmp(pila[posicionpila], "(")==0){ //Esto es infinito
					printf("\nEntra\n");
					strcpy(pila[posicionpila],"0"); //Esto no lo hace bien 
					posicionpila--;
					break;
				}
				else{
					printf("\nEntra aquí?\n");
					strcpy(cola[posicioncola],pila[posicionpila]);
					posicionpila=posicionpila-1;	
					posicioncola++;
					break;
				}
			}
        }
        
        else{
			printf("\nEntra aquí??\n");
			ptr=&expresion[i];
            while(prioridad(pila[posicionpila]) >= prioridad(ptr)){//Esto también da infinito
				//printf("Se queda pinzado aqui en el else");
				if (posicionpila>=0){
					//printf("ESTAAAAAAAMOS AQUIIIIIIII, topes: %d",top);
					//strcpy(x,pila[top]);
					
					
					strcpy(cola[posicioncola],pila[posicionpila]);
					strcpy(pila[posicionpila],"0");
					posicionpila=posicionpila-1;
					posicioncola++;
					
				}
				else{
					break;
				}
			
			}
			
			printf("Estamos aquíi jejeje: %c ", expresion[i]);
			switch(expresion[i]){
				case '*':
					printf("Vamonoooos");
					posicionpila++;
					strcpy(pila[posicionpila],"*");
					break;
				case '+':
					printf("Vamonoooos2");
					posicionpila++;
					strcpy(pila[posicionpila],"+");
					break;
			}

        }
		printf("\n\nCCCCCCCCOOOOOOOOOOOOOOLLLLLLLLLLLLLLLAAAAAAAA\n");
		for (int j=0;j<(sizeof(cola) / sizeof(cola[0]));j++){
			printf("%s ,", cola[j]);
		}
		printf("\n\nPPPPPPPPPPPPPIIIIIIIIIIIIIILLLLLLLLLLLLLLLLLLLLAAAAAAAAAAAAAAAAAAAA\n");
		for (int r=0;r<(sizeof(pila) / sizeof(pila[0]));r++){
			printf("%s ,", pila[r]);
		}
		
        
         
	}
	while(posicionpila>0){
		strcpy(cola[posicioncola],pila[posicionpila]);
		posicioncola++;
		posicionpila--;	
	}
	
    
}


int prioridad(char *x){
    if(strcmp(x,"("))
        return 3;
    if(strcmp(x ,"+"))
        return 1;
    if(strcmp(x ,"*"))
        return 2;
    return 0;
}







