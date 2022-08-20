#include "stdio.h"
#include <math.h>
#include "stdlib.h"
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h> 


int NUMMOT=2;
int NUMCARACMAX=1000; //100: válido para 2

void itroducirMotores(float matriz[NUMMOT][4]);
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
//Los valores que se cambien en matriz afectarán a los valores de listaValores porque apuntan a los mismos.
//Por lo que no es necesario crear de nuevo listaValores ya que siempre estará actualizada.
int main(){
	printf("Introduzca el número de motores: ");
	scanf(" %d",&NUMMOT);
	float matriz[NUMMOT][4]; 
	char matriztem[4][4][NUMCARACMAX]; 
	char matrizPosFijo[4][4][NUMCARACMAX]; 
	float *listaValores[(int)(NUMCARACMAX/4)];
	//inicializarArray(listaValores);
	int senosycosenos[NUMCARACMAX];//1 seno, 2 coseno. 3 -sen
	int possenoycoseno=0;
	float resultado[4][4];
	int introduccionDatos=0;
	
	introduccionDatos=tipoIntroduccionDatos();
	inicializarMatrizZeros(matrizPosFijo);
	inicializarArrayZeros(senosycosenos);
	
	if (introduccionDatos==1){
		itroducirMotores(matriz);
	}else if (introduccionDatos==2){
		//itroducirMotoresDH(matriz);
	}
	
	inicializarMatrizZeros(matriztem);
	csvAMatriz(matriztem);
	sacarListaValores(matriz,matriztem,listaValores);
	parsearMatrizTem(matriztem,listaValores,matrizPosFijo,senosycosenos,possenoycoseno);
	sustitucionYCalculoDatosMatrizParseada(matrizPosFijo,listaValores,senosycosenos,resultado);
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

void inicializarMatrizZeros2(char cola[NUMCARACMAX]){
	for(int i=0;i<NUMCARACMAX;i++){
        cola[i]='0';
	}
}

void inicializarArrayZeros(int cola[NUMCARACMAX]){
	for(int i=0;i<NUMCARACMAX;i++){
        cola[i]=0;
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
	printf("valores a sustituir en matriz\n");
	for (int z =0;z<=posicionListaValores-1;z++){  
		printf("%f  ",*listaValores[z]);
	}
}

void parsearMatrizTem(char matriztem[4][4][NUMCARACMAX],float *listaValores[(int)(NUMCARACMAX/4)],char matrizPosFijo[4][4][NUMCARACMAX],int senosycosenos[NUMCARACMAX],int possenoycoseno){	
    char temporal[NUMCARACMAX];
    char cadenaTemp[NUMCARACMAX]; 
    //int senosycosenos[NUMCARACMAX];//1 seno, 2 coseno. 3 -sen
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
				senosycosenos[possenoycoseno]=3;//3
				i=i+5;
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
				if(pila[posicionpila]== '('){ //Esto es infinito
					pila[posicionpila]='0'; //Esto no lo hace bien 
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
            while(prioridad(pila[posicionpila]) >= prioridad(expresion[i])){//Esto también da infinito
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

void matrizCharAString(char temporal[NUMCARACMAX], char cadenaTemp[NUMCARACMAX]){
	char str1[2];
	str1[1] = '\0';
	for (int i=0;i<NUMCARACMAX && temporal[i]!='0';i++){
		str1[0] = temporal[i];
		strcat(cadenaTemp,str1);
	}
}

void sustitucionYCalculoDatosMatrizParseada(char matrizPosFijo[4][4][NUMCARACMAX],float *listaValores[(int)(NUMCARACMAX/4)],int senosycosenos[NUMCARACMAX],float resultado[4][4]){
	
	for (int i=0;i<4;i++){
		for (int j=0;j<4;j++){
			resultado[i][j]=postfixStringAResultado(matrizPosFijo[i][j],listaValores,senosycosenos);
		}		
	}
	
	
	//resultado[0][0]=postfixStringAResultado(matrizPosFijo[0][0],listaValores,senosycosenos);
	printf("\nMatriz Final\n");
	for (int o=0;o<4;o++){
		for(int p=0;p<4;p++){
			printf("%f	",resultado[o][p]);
		}
	}
	
}

float postfixStringAResultado(char arrayPostFijo[NUMCARACMAX],float *listaValores[(int)(NUMCARACMAX/4)],int senosycosenos[NUMCARACMAX]){
	int resultadoTemporal=0;
	float pila[strlen(arrayPostFijo)];//Será más pequeño siempre que el número de elementos que haya.
	int posicionPila=0;
	int posicionListaValores=0;
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
	for (int q =0;q<=NUMCARACMAX;q++){  
		printf("%d",senosycosenos[q]);
	}
	
	for (int i =0;i< strlen(arrayPostFijo);i++){
		
		printf("\n\n--------------------------------pila_----, iteracion: %d\n",i);
		for (int j=0;j<strlen(arrayPostFijo);j++ ){
			printf("%f : ",pila[j]);
		}
		printf("\n\n");
		
		if (arrayPostFijo[i]=='2'){
			printf("Es 2\n");
			//seno 1 coseno 2 -seno 3
			//Pasamos los ángulos de grados a radiantes para aplicar las funciones matemáticas.
			if(senosycosenos[posicionListaValores]==1){
				printf("Estamos en seno\n");
				pila[posicionPila]=sin(*listaValores[posicionListaValores]*(3.141592/180));//*(3.141592/180)
				//printf("SENO-------%f",pila[posicionPila]);
			}
			if(senosycosenos[posicionListaValores]==2){
				printf("Estamos en coseno\n");
				pila[posicionPila]=cos(*listaValores[posicionListaValores]*(3.141592/180));
				printf("posicionListaValores: %d\n",posicionListaValores);
				printf("listaValores: %f\n",*listaValores[posicionListaValores]);
				//printf("COSENO-------%f",pila[posicionPila]);

			}
			if(senosycosenos[posicionListaValores]==3){
				printf("Estamos en -seno\n");
				pila[posicionPila]=-sin(*listaValores[posicionListaValores]*(3.141592/180));
				//printf("-SENO-------%f",pila[posicionPila]);
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
	for (int j=0;j<strlen(arrayPostFijo);j++ ){
		printf("%f : ",pila[j]);
	}
	printf("\n\n");
	printf("-------RESULTADOS: %f",pila[0]);
	return pila[0];
}




