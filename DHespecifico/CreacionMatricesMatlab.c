#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "engine.h"
#define  CONTADORMOTORES 3

void IncluirMatlabSyms(int contadorMotores,Engine *ep);
void CrearMatricesNormalesSimplesMatlab(int contadorMotores,Engine *ep, char variablesNormales[contadorMotores][10]);
void CrearMatricesInversasSimplesMatlab(int contadorMotores,Engine *ep, char variablesInversas[CONTADORMOTORES][10]);
void CalculosMatlabConjuntos(int contadorMotores,Engine *ep, char variablesInversas[CONTADORMOTORES][10], char variablesNormales[contadorMotores][10]);
void CalculosMatlabConjuntosTotalNumerica(int contadorMotores,Engine *ep, char variablesInversas[CONTADORMOTORES][10], char variablesNormales[contadorMotores][10]);
void CalculosMatlabConjuntosTotal(int contadorMotores,Engine *ep, char variablesInversas[CONTADORMOTORES][10], char variablesNormales[contadorMotores][10]);
void CalculosMatlabConjuntosParciales(int contadorMotores,Engine *ep, char variablesInversas[CONTADORMOTORES][10], char variablesNormales[contadorMotores][10]);
void GuardarEnCSVdesdeMatlab(Engine *ep,char nombreCSV[200],char nombreVariable[10],int tipo);
int ModificacionMatricesNormalesEInversas(Engine *ep);
void CalculosMatlabConjuntosParcialesConSalto(int contadorMotores,Engine *ep, char variablesInversas[CONTADORMOTORES][10], char variablesNormales[contadorMotores][10], int iteracion, int todoNumeros);
void leerfilasCSVParametrosDHespecificos(float matriz[CONTADORMOTORES][4]);
void sustituirParametrosDH(Engine *ep, float matriz[CONTADORMOTORES][4]);

int main(){
	Engine *ep;
	int contadorMotores=CONTADORMOTORES;
	char variablesNormales[CONTADORMOTORES][10];
	char variablesInversas[CONTADORMOTORES][10];
	int todoNumeros=0;
	
	//Borramos la información de ejecuciones anteriores.
	if (remove("MatricesCalculadas")==0){
		printf("MatricesCalculadas.txt actualizado\n");
	}  
	
	/*
	 * Call engOpen with a NULL string. This starts a MATLAB process 
     * on the current host using the command "matlab".
	 */
	if (!(ep = engOpen(""))) {
		fprintf(stderr, "\nCan't start MATLAB engine\n");
		return EXIT_FAILURE;
	}
	engEvalString(ep, "MatricesC = fopen('MatricesCalculadas', 'a+');"); 				//QUEDA CERRARLO
	
	IncluirMatlabSyms(contadorMotores,ep);
	CrearMatricesNormalesSimplesMatlab(contadorMotores,ep,variablesNormales);
	CrearMatricesInversasSimplesMatlab(contadorMotores,ep,variablesInversas);
	todoNumeros=ModificacionMatricesNormalesEInversas(ep);
	if(todoNumeros==0){
		//CalculosMatlabConjuntos(contadorMotores,ep,variablesInversas,variablesNormales);
		//CalculosMatlabConjuntosParciales(contadorMotores,ep,variablesInversas,variablesNormales);  //No se usa
		CalculosMatlabConjuntosTotal(contadorMotores,ep,variablesInversas,variablesNormales);
		CalculosMatlabConjuntosParcialesConSalto(contadorMotores,ep,variablesInversas,variablesNormales,contadorMotores,todoNumeros);
	}
	if(todoNumeros==1){
		CalculosMatlabConjuntosTotalNumerica(contadorMotores,ep,variablesInversas,variablesNormales);
		CalculosMatlabConjuntosParcialesConSalto(contadorMotores,ep,variablesInversas,variablesNormales,contadorMotores,todoNumeros);
	}

	engEvalString(ep, "fclose('all');"); 	
	engClose(ep);
	
	return 0;
}

void IncluirMatlabSyms(int contadorMotores,Engine *ep){
	char syms [1000];
	char temporal[100];
	char intastring[100];
	
	strcpy(syms, "syms");
	//int j=0;
	for (int i=0;i<contadorMotores;i++){
		//j=i+1;
		sprintf(intastring,"%d",i+1);
		strcpy(temporal, " b");
		strcat(temporal, intastring);
		strcat(temporal, " a");
		strcat(temporal, intastring);
		strcat(temporal, " r");
		strcat(temporal, intastring);
		strcat(temporal, " d");
		strcat(temporal, intastring);
		strcat(syms, temporal);	
	}
	printf("%s\n\n", syms);
	engEvalString(ep, syms); 

}

void CrearMatricesNormalesSimplesMatlab(int contadorMotores,Engine *ep, char variablesNormales[contadorMotores][10]){
	//Crear matrices Normales
	char matriBaseNormal[100000];
	char intastring[100];
	char nombreCSVMatlab[200];
	char nombreVariableTemporal[10];
	
	if (remove("VariablesNormalesMATLAB")==0){
		printf("VariablesNormalesMATLAB.txt actualizado\n");
	}
	
	for (int i=0;i<contadorMotores;i++){
		sprintf(intastring,"%d",i+1);
		
		strcpy(variablesNormales[i],"N");
		strcat(variablesNormales[i],intastring);
		strcpy(nombreVariableTemporal,variablesNormales[i]);
		printf("Esto es variablesNormales: %s\n", variablesNormales[i]);
		
		//FILA 1
		strcpy(matriBaseNormal,"N");
		strcat(matriBaseNormal,intastring);
		strcat(matriBaseNormal,"=[cos(b");
		strcat(matriBaseNormal,intastring);
		strcat(matriBaseNormal,")");
		
		strcat(matriBaseNormal," -cos(a");
		strcat(matriBaseNormal,intastring);
		strcat(matriBaseNormal,")*sin(b");
		strcat(matriBaseNormal,intastring);
		strcat(matriBaseNormal,")");
		
		strcat(matriBaseNormal," sin(a");
		strcat(matriBaseNormal,intastring);
		strcat(matriBaseNormal,")*sin(b");
		strcat(matriBaseNormal,intastring);
		strcat(matriBaseNormal,")");
		
		strcat(matriBaseNormal," r");
		strcat(matriBaseNormal,intastring);
		strcat(matriBaseNormal,"*cos(b");
		strcat(matriBaseNormal,intastring);
		strcat(matriBaseNormal,");");
		
		//FILA 2
		strcat(matriBaseNormal,"sin(b");
		strcat(matriBaseNormal,intastring);
		strcat(matriBaseNormal,")");
		
		strcat(matriBaseNormal," cos(a");
		strcat(matriBaseNormal,intastring);
		strcat(matriBaseNormal,")*cos(b");
		strcat(matriBaseNormal,intastring);
		strcat(matriBaseNormal,")");
		
		strcat(matriBaseNormal," -sin(a");
		strcat(matriBaseNormal,intastring);
		strcat(matriBaseNormal,")*cos(b");
		strcat(matriBaseNormal,intastring);
		strcat(matriBaseNormal,")");
		
		strcat(matriBaseNormal," r");
		strcat(matriBaseNormal,intastring);
		strcat(matriBaseNormal,"*sin(b");
		strcat(matriBaseNormal,intastring);
		strcat(matriBaseNormal,");");
		
		//FILA 3
		strcat(matriBaseNormal,"0");
		
		strcat(matriBaseNormal," sin(a");
		strcat(matriBaseNormal,intastring);
		strcat(matriBaseNormal,")");
		
		strcat(matriBaseNormal," cos(a");
		strcat(matriBaseNormal,intastring);
		strcat(matriBaseNormal,")");
		
		strcat(matriBaseNormal," d");
		strcat(matriBaseNormal,intastring);
		strcat(matriBaseNormal,";");
		
		//FILA 4
		strcat(matriBaseNormal,"0 0 0 1]");
		
		engEvalString(ep, matriBaseNormal); 
		printf("%s\n\n", matriBaseNormal);
		strcpy(nombreCSVMatlab,"VariablesNormalesMATLAB");
		GuardarEnCSVdesdeMatlab(ep,nombreCSVMatlab,nombreVariableTemporal,0);
	}
	printf("****************************\n\n");
}

void CrearMatricesInversasSimplesMatlab(int contadorMotores,Engine *ep, char variablesInversas[CONTADORMOTORES][10]){
	//Crear matrices Inversas
	char matriBaseInversa[100000];
	char intastring[100];
	char nombreCSVMatlab[200];
	char nombreVariableTemporal[10];
	
	if (remove("VariablesInversasMATLAB")==0){
		printf("VariablesInversasMATLAB.txt actualizado\n");
	}
	
	for (int i=0;i<contadorMotores;i++){
		sprintf(intastring,"%d",i+1);
		
		strcpy(variablesInversas[i],"I");
		strcat(variablesInversas[i],intastring);
		strcpy(nombreVariableTemporal,variablesInversas[i]);
		printf("Esto es variablesNormales: %s\n", variablesInversas[i]);
		//FILA 1
		strcpy(matriBaseInversa,"I");
		strcat(matriBaseInversa,intastring);
		strcat(matriBaseInversa,"=[cos(b");
		strcat(matriBaseInversa,intastring);
		strcat(matriBaseInversa,")");
		
		strcat(matriBaseInversa," sin(b");
		strcat(matriBaseInversa,intastring);
		strcat(matriBaseInversa,")");
		
		strcat(matriBaseInversa," 0");
		
		strcat(matriBaseInversa," -r");
		strcat(matriBaseInversa,intastring);
		strcat(matriBaseInversa,";");
		
		//FILA 2
		strcat(matriBaseInversa," -cos(a");
		strcat(matriBaseInversa,intastring);
		strcat(matriBaseInversa,")*sin(b");
		strcat(matriBaseInversa,intastring);
		strcat(matriBaseInversa,")");
		
		strcat(matriBaseInversa," cos(a");
		strcat(matriBaseInversa,intastring);
		strcat(matriBaseInversa,")*cos(b");
		strcat(matriBaseInversa,intastring);
		strcat(matriBaseInversa,")");
		
		strcat(matriBaseInversa," sin(a");
		strcat(matriBaseInversa,intastring);
		strcat(matriBaseInversa,")");
		
		strcat(matriBaseInversa," -sin(a");
		strcat(matriBaseInversa,intastring);
		strcat(matriBaseInversa,")*(d");
		strcat(matriBaseInversa,intastring);
		strcat(matriBaseInversa,");");
		
		//FILA 3
		strcat(matriBaseInversa," sin(a");
		strcat(matriBaseInversa,intastring);
		strcat(matriBaseInversa,")*sin(b");
		strcat(matriBaseInversa,intastring);
		strcat(matriBaseInversa,")");
		
		strcat(matriBaseInversa," -sin(a");
		strcat(matriBaseInversa,intastring);
		strcat(matriBaseInversa,")*cos(b");
		strcat(matriBaseInversa,intastring);
		strcat(matriBaseInversa,")");
		
		strcat(matriBaseInversa," cos(a");
		strcat(matriBaseInversa,intastring);
		strcat(matriBaseInversa,")");
		
		strcat(matriBaseInversa," -cos(a");
		strcat(matriBaseInversa,intastring);
		strcat(matriBaseInversa,")*(d");
		strcat(matriBaseInversa,intastring);
		strcat(matriBaseInversa,");");
		
		//FILA 4
		strcat(matriBaseInversa,"0 0 0 1]");
		
		engEvalString(ep, matriBaseInversa); 
		printf("%s\n\n", matriBaseInversa);
		strcpy(nombreCSVMatlab,"VariablesInversasMATLAB");
		GuardarEnCSVdesdeMatlab(ep,nombreCSVMatlab,nombreVariableTemporal,0);
	}
	printf("****************************\n\n");
}

/*
void CalculosMatlabConjuntos(int contadorMotores,Engine *ep, char variablesInversas[CONTADORMOTORES][10], char variablesNormales[contadorMotores][10]){
	//Total
	CalculosMatlabConjuntosTotal(contadorMotores,ep,variablesInversas,variablesNormales);
	//parejas 12-23-34...
	
	//parciales 1-2/1-3/1-4/2-5
	
}*/


void CalculosMatlabConjuntosTotal(int contadorMotores,Engine *ep, char variablesInversas[CONTADORMOTORES][10], char variablesNormales[contadorMotores][10]){
	char stringOperacionesMatlab[100000];
	char temporal [10];
	char intastring[100];
	//char nombreTemporalVarialbe [20];
	char nombreCSVMatlab[200];
	char nombreVariableTemporal[10];
	
	strcpy(nombreCSVMatlab,"MatricesCalculadas");
	
	for (int v=0;v<contadorMotores;v++){
		printf("%s ", variablesNormales[v]);
	}
	printf("\nTOTAL:\n");
	//Mismo comentario en fichero  
	engEvalString(ep, "fprintf(MatricesC, 'TOTAL')");  
	engEvalString(ep, "fprintf(MatricesC, newline)"); 
	
	//NT
	strcpy(stringOperacionesMatlab,"NT=simplify(");
	for (int i=0;i<contadorMotores;i++){
		strcat(stringOperacionesMatlab,variablesNormales[i]);
		if (i<contadorMotores-1){
			strcat(stringOperacionesMatlab,"*");
		}
	}
	strcat(stringOperacionesMatlab,")");
	engEvalString(ep, stringOperacionesMatlab); 
	strcpy(temporal,"NT");
	GuardarEnCSVdesdeMatlab(ep,nombreCSVMatlab,temporal,0);
	engEvalString(ep, "fprintf(MatricesC, newline)");  
	printf("\nOperación332: %s", stringOperacionesMatlab); //Ej: NT=simplify(N1*N2*N3*N4*N5)
	//Iteraciones
	for (int j=0;j<contadorMotores-1;j++){
			//SNx
			strcpy(temporal,"SN");
			sprintf(intastring,"%d",j+1);
			strcat(temporal,intastring);
			strcpy(stringOperacionesMatlab,temporal);
			strcat(stringOperacionesMatlab,"=simplify(");
			for (int m=0;m<contadorMotores-j-1;m++){
				strcat(stringOperacionesMatlab,variablesNormales[m+1+j]);	
				//printf("Esto es: %s",variablesNormales[m+1+j]);
				if (m<contadorMotores-2-j){
					strcat(stringOperacionesMatlab,"*");
				}
			}
			strcat(stringOperacionesMatlab,")");
			engEvalString(ep, stringOperacionesMatlab);					 //ABILITAR FUERA DE PRUEBAS
			GuardarEnCSVdesdeMatlab(ep,nombreCSVMatlab,temporal,0);
			engEvalString(ep, "fprintf(MatricesC, newline)");  
			printf("\nOperación: %s", stringOperacionesMatlab); 
			//IIx
			strcpy(temporal,"II");
			sprintf(intastring,"%d",j+1);
			strcat(temporal,intastring);
			strcpy(stringOperacionesMatlab,temporal);
			strcat(stringOperacionesMatlab,"=simplify(");
			for (int n=0;n<j+1;n++){
				strcat(stringOperacionesMatlab,variablesInversas[j-n]);	
				if (n<j){
					strcat(stringOperacionesMatlab,"*");
				}
			}
			strcat(stringOperacionesMatlab,")");
			engEvalString(ep, stringOperacionesMatlab); 				//ABILITAR FUERA DE PRUEBAS
			GuardarEnCSVdesdeMatlab(ep,nombreCSVMatlab,temporal,0);
			engEvalString(ep, "fprintf(MatricesC, newline)");  
			printf("\nOperación: %s", stringOperacionesMatlab); 
			//SIx
			strcpy(temporal,"SI");
			sprintf(intastring,"%d",j+1);
			strcat(temporal,intastring);
			strcpy(stringOperacionesMatlab,temporal);
			strcat(stringOperacionesMatlab,"=simplify(");
			for (int o=0;o<j+1;o++){
				strcat(stringOperacionesMatlab,variablesInversas[j-o]);	
				if (o<j){
					strcat(stringOperacionesMatlab,"*");
				}
			}
			strcat(stringOperacionesMatlab,"*NT)");
			engEvalString(ep, stringOperacionesMatlab); 				//ABILITAR FUERA DE PRUEBAS
			GuardarEnCSVdesdeMatlab(ep,nombreCSVMatlab,temporal,0);
			engEvalString(ep, "fprintf(MatricesC, newline)");  
			printf("\nOperación: %s", stringOperacionesMatlab); 
		
		printf("\n");
	}
	printf("\n");
}


void CalculosMatlabConjuntosParciales(int contadorMotores,Engine *ep, char variablesInversas[CONTADORMOTORES][10], char variablesNormales[contadorMotores][10]){
	char stringOperacionesMatlab[100000];
	char temporal [10];
	char intastring[100];
	int subconjuntos=2;
	int contador=contadorMotores-1;
	int contador2=contador;
	int contador3=1;
	int contador4=1;
	printf("PARCIALES:\n");
	for (int i=contadorMotores;i-1>1;i--){ //nº de combinaciones de subconjuntos
		for (int j=0; j<subconjuntos;j++){ //nº de subconjuntos por combinacióbn
			printf("\n");
			for (int v=0;v<contadorMotores;v++){
				printf("%s ", variablesNormales[v]);
			}
			printf("\nPARCIAL :%d - %d\n", j+1,contador2);
			contador2++;
			//NTx
			strcpy(stringOperacionesMatlab,"NT=simplify(");
			for (int m=0;m<contador;m++){ //bucle 
				strcat(stringOperacionesMatlab,variablesNormales[j+m]);	
				if (m<contador-1){
					strcat(stringOperacionesMatlab,"*");
				}
				
			}
			strcat(stringOperacionesMatlab,")");
			//engEvalString(ep, stringOperacionesMatlab);					 //ABILITAR FUERA DE PRUEBAS
			printf("\nOperación: %s", stringOperacionesMatlab); 
			
			for (int k=0;k<contador2-(j+2);k++){
				//SNx
				strcpy(temporal,"SN");
				sprintf(intastring,"%d",k+1);
				strcat(temporal,intastring);
				strcpy(stringOperacionesMatlab,temporal);
				strcat(stringOperacionesMatlab,"=simplify(");
				for (int l=0;l<contador2-(j+2+k);l++){
					strcat(stringOperacionesMatlab,variablesNormales[j+1+l+k]);	
					if (l<contador2-(j+2+k)-1){
						strcat(stringOperacionesMatlab,"*");
					}
				}
				strcat(stringOperacionesMatlab,")");
				//engEvalString(ep, stringOperacionesMatlab);					 //ABILITAR FUERA DE PRUEBAS
				printf("\nOperación: %s", stringOperacionesMatlab); 
				
				//IIx
				strcpy(temporal,"II");
				sprintf(intastring,"%d",k+1);
				strcat(temporal,intastring);
				strcpy(stringOperacionesMatlab,temporal);
				strcat(stringOperacionesMatlab,"=simplify(");
				for (int n=0;n<contador3;n++){
					strcat(stringOperacionesMatlab,variablesInversas[j+contador3-n-1]);	
					if (n<contador3-1){
						strcat(stringOperacionesMatlab,"*");
					}
				}
				contador3++;
				strcat(stringOperacionesMatlab,")");
				//engEvalString(ep, stringOperacionesMatlab);					 //ABILITAR FUERA DE PRUEBAS
				printf("\nOperación: %s", stringOperacionesMatlab); 
				
				//SIx
				strcpy(temporal,"SI");
				sprintf(intastring,"%d",k+1);
				strcpy(stringOperacionesMatlab,temporal);
				strcat(stringOperacionesMatlab,intastring);
				strcat(stringOperacionesMatlab,"=simplify(I");
				for (int s=0;s<contador4;s++){
					strcat(stringOperacionesMatlab,variablesInversas[j+contador3-s-1]);	
					if (s<contador4-1){
						strcat(stringOperacionesMatlab,"*");
					}
				}
				contador4++;
				strcat(stringOperacionesMatlab,"*NT)");
				//engEvalString(ep, stringOperacionesMatlab);					 //ABILITAR FUERA DE PRUEBAS
				printf("\nOperación: %s\n", stringOperacionesMatlab); 
			}
			contador3=1;
			contador4=1;
		}
		contador2=i-2;
		subconjuntos++;
		contador--;
	}
}

void GuardarEnCSVdesdeMatlab(Engine *ep,char nombreCSV[200],char nombreVariable[10], int tipo){
	char stringAEvaluar [1000];
	char stringTemporal [100];
	char buffer [256];
	//engOutputBuffer(ep, buffer, 256);									//BORRARRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR
	//Abrir/Crear CSV
	strcpy(stringAEvaluar,"fid = fopen('");
	strcat(stringAEvaluar,nombreCSV);
	strcat(stringAEvaluar,"', 'a+');");
	engEvalString(ep, stringAEvaluar); 
	//Variable =
	strcpy(stringTemporal,nombreVariable);
	strcat(stringTemporal,"=");
	strcpy(stringAEvaluar,"fprintf(fid,'");
	strcat(stringAEvaluar,stringTemporal);
	strcat(stringAEvaluar,"')");
	engEvalString(ep, stringAEvaluar);

	//printf("\nBUFFER_1: %s", buffer);									//BORRARRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR
	if (tipo==0){
		//Guardar variable deseada
		strcpy(stringAEvaluar,"fprintf(fid, char(");
		strcat(stringAEvaluar, nombreVariable);
		strcat(stringAEvaluar, "));");
		engEvalString(ep, stringAEvaluar);
		//printf("\nBUFFER_2: %s", buffer);									//BORRARRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR
		engEvalString(ep, "fprintf(fid, newline)");  
		engEvalString(ep, "fclose(fid)");
	}
	if (tipo==1){
		//Guardar variable deseada
		strcpy(stringAEvaluar,"fprintf(fid, num2str(");
		strcat(stringAEvaluar, nombreVariable);
		strcat(stringAEvaluar, "));");
		engEvalString(ep, stringAEvaluar);
		//printf("\nBUFFER_2: %s", buffer);									//BORRARRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR
		engEvalString(ep, "fprintf(fid, newline)");  
		engEvalString(ep, "fclose(fid)");
	}
	
}

int ModificacionMatricesNormalesEInversas(Engine *ep){
	char decision;
	char nombreArchivo[100];
	int parametrosDH[CONTADORMOTORES][4];
	float matriz[CONTADORMOTORES][4];
	char bufer[1000000]; 
	int todoNumeros=0; 
	
	strcpy(nombreArchivo,"VariablesNormalesMATLAB");
	printf("Si desea sustituir o modificar las matrices normales o inversas en los ficheros \"VariablesNormalesMATLAB\" o \"VariablesInversasMATLAB\"\n");
	printf("¿Han sido modificadas manualmente las matrices anteriormente mencionadas? (s/n): ");
	scanf(" %c",&decision);
	if (decision =='s'){
		for(int i=0;i<2;i++){
			FILE *archivo = fopen(nombreArchivo, "r"); // Modo lectura

			printf("\nLas nuevas variables %s son:\n",nombreArchivo);
			while (fgets(bufer, 1000000, archivo)){
				// Aquí, justo ahora, tenemos ya la línea. Le vamos a remover el salto
				strtok(bufer, "\n");
				// La imprimimos, pero realmente podríamos hacer cualquier otra cosa
				printf("%s\n", bufer);
				engEvalString(ep, bufer);
				//engEvalString(ep, "b1=0");
			}
			fclose(archivo);	
			strcpy(nombreArchivo,"VariablesInversasMATLAB");
		}			
	}
	printf("¿Desea sustituir los parámetros de DH con los creados con el programa IntroduccionDatosMotores.c? (s/n): ");
	scanf(" %c",&decision);
	if (decision =='s'){
		todoNumeros=1;	
	}
	
	if (decision =='s'){
		leerfilasCSVParametrosDHespecificos(matriz);
		sustituirParametrosDH(ep, matriz);
		
		for(int j=0;j<2;j++){
			FILE *archivo = fopen(nombreArchivo, "r"); // Modo lectura
			//char bufer[10000];         // Aquí vamos a ir almacenando cada línea
			printf("\nLas nuevas variables %s son:\n",nombreArchivo);
			while (fgets(bufer, 1000000, archivo)){
				// Aquí, justo ahora, tenemos ya la línea. Le vamos a remover el salto
				strtok(bufer, "\n");
				// La imprimimos, pero realmente podríamos hacer cualquier otra cosa
				printf("%s\n", bufer);
				engEvalString(ep, bufer);
			}
			fclose(archivo);	
			strcpy(nombreArchivo,"VariablesInversasMATLAB");
		}	
	}
	
	return todoNumeros;
}

void CalculosMatlabConjuntosParcialesConSalto(int contadorMotores,Engine *ep, char variablesInversas[CONTADORMOTORES][10], char variablesNormales[contadorMotores][10], int iteracion, int todoNumeros){
	char variablesNormalesModificadas[CONTADORMOTORES][10];
	char variablesInversasModificadas[CONTADORMOTORES][10];
	char variablesNormalesTemporal[CONTADORMOTORES][10];
	char variablesInversasTemporal[CONTADORMOTORES][10];
	char separadorParaCSV[200];
	int contador=0;
	//Bucle para ir eliminando variables 
	for (int i=0;i<iteracion;i++){
		strcpy(separadorParaCSV,"fprintf(MatricesC, '-------------************************** NORMALES: ");
		for (int j=0;j<iteracion;j++){
			//copia las varialbles a nuevas listas
			if (i!=j){
				strcpy(variablesNormalesModificadas[j-contador],variablesNormales[j]);
				strcpy(variablesInversasModificadas[j-contador],variablesInversas[j]);
			}
			else{
				contador++;	
			}
		}
		contador=0;
		printf("-------------************************** NORMALES: ");
		for(int k=0;k<iteracion-1;k++){
			printf("%s ", variablesNormalesModificadas[k]);
			strcat(separadorParaCSV,variablesNormalesModificadas[k]);
			strcat(separadorParaCSV," ");
		}
		printf("\n");
		strcat(separadorParaCSV,"')");
		
		engEvalString(ep, separadorParaCSV);  
		engEvalString(ep, "fprintf(MatricesC, newline)");  
		engEvalString(ep, "fprintf(MatricesC, newline)");  
		if(todoNumeros==0){
			CalculosMatlabConjuntosTotal(iteracion-1,ep,variablesInversasModificadas,variablesNormalesModificadas);
			//CalculosMatlabConjuntosParciales(iteracion-1,ep,variablesInversasModificadas,variablesNormalesModificadas);
		}
		if(todoNumeros==1){
			CalculosMatlabConjuntosTotalNumerica(iteracion-1,ep,variablesInversasModificadas,variablesNormalesModificadas);
		}
		if(iteracion>3){
			CalculosMatlabConjuntosParcialesConSalto(contadorMotores,ep,variablesInversasModificadas,variablesNormalesModificadas,iteracion-1,todoNumeros);
		}
	}
}


/**
* @fn void leerfilasCSVParametrosDHespecificos(float matriz[NUMMOT][4])
* @brief lee y copia las celdas de la matriz de parámetros de DH (CSV).
* @param recibe la matriz donde se guardan dichos datos. 
* @return no devuelve nada pero guarda los datos en matriz pasada por referencia. 
*/
void leerfilasCSVParametrosDHespecificos(float matriz[CONTADORMOTORES][4]){
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
	printf("\nMATRIZ datos DH obtenidos del CSV\n");
	int a =0;
	int b=0;
	for (a=0;a<CONTADORMOTORES;a++){
		for(b=0;b<4;b++){
			printf("%f           " ,matriz[a][b]);
		}	
		printf("\n\n\n");
	}
}

//Sustituir cosas en MATLAB --No funciona
void sustituirParametrosDH(Engine *ep, float matriz[CONTADORMOTORES][4]){
	char beta[10];
	char alfa[10];
	char r[10];
	char d[10];
	char intastring[10];
	char intastringMotor[10];
	char stringOperacionesMatlab[100000];
	//Orden en el que aparecen las variables en el CSV.
	for (int i=0;i<CONTADORMOTORES;i++){
		//Numero de motor
		sprintf(intastringMotor,"%d",i+1);
		
		strcpy(stringOperacionesMatlab,"b");
		strcat(stringOperacionesMatlab,intastringMotor);
		strcat(stringOperacionesMatlab,"=");
		sprintf(intastring,"%f",matriz[i][0]);
		strcat(stringOperacionesMatlab,intastring);
		printf("Operacion: %s\n",stringOperacionesMatlab);
		engEvalString(ep, stringOperacionesMatlab);  
		
		strcpy(stringOperacionesMatlab,"a");
		strcat(stringOperacionesMatlab,intastringMotor);
		strcat(stringOperacionesMatlab,"=");
		sprintf(intastring,"%f",matriz[i][1]);
		strcat(stringOperacionesMatlab,intastring);
		printf("Operacion: %s\n",stringOperacionesMatlab);
		engEvalString(ep, stringOperacionesMatlab);  
		
		strcpy(stringOperacionesMatlab,"r");
		strcat(stringOperacionesMatlab,intastringMotor);
		strcat(stringOperacionesMatlab,"=");
		sprintf(intastring,"%f",matriz[i][2]);
		strcat(stringOperacionesMatlab,intastring);
		printf("Operacion: %s\n",stringOperacionesMatlab);
		engEvalString(ep, stringOperacionesMatlab);  
		
		strcpy(stringOperacionesMatlab,"d");
		strcat(stringOperacionesMatlab,intastringMotor);
		strcat(stringOperacionesMatlab,"=");
		sprintf(intastring,"%f",matriz[i][3]);
		strcat(stringOperacionesMatlab,intastring);
		printf("Operacion: %s\n",stringOperacionesMatlab);
		engEvalString(ep, stringOperacionesMatlab);  
	}
}

void CalculosMatlabConjuntosTotalNumerica(int contadorMotores,Engine *ep, char variablesInversas[CONTADORMOTORES][10], char variablesNormales[contadorMotores][10]){
	char stringOperacionesMatlab[100000];
	char temporal [10];
	char intastring[100];
	//char nombreTemporalVarialbe [20];
	char nombreCSVMatlab[200];
	char nombreVariableTemporal[10];
	char *arrayPrueba [1000];
	
	strcpy(nombreCSVMatlab,"MatricesCalculadasNumerica");
	
	for (int v=0;v<contadorMotores;v++){
		printf("%s ", variablesNormales[v]);
	}
	printf("\nTOTAL:\n");
	//Mismo comentario en fichero  
	engEvalString(ep, "fprintf(MatricesC, 'TOTAL')");  
	engEvalString(ep, "fprintf(MatricesC, newline)"); 
	
	//NT
	strcpy(stringOperacionesMatlab,"NT=(");
	for (int i=0;i<contadorMotores;i++){
		strcat(stringOperacionesMatlab,variablesNormales[i]);
		if (i<contadorMotores-1){
			strcat(stringOperacionesMatlab,"*");
		}
	}
	strcat(stringOperacionesMatlab,")");
	engEvalString(ep, stringOperacionesMatlab); 
	strcpy(temporal,"NT");
	GuardarEnCSVdesdeMatlab(ep,nombreCSVMatlab,temporal,1);
	engEvalString(ep, "fprintf(MatricesC, newline)");  
	printf("\nOperación: %s", stringOperacionesMatlab); //Ej: NT=simplify(N1*N2*N3*N4*N5)
	//Iteraciones
	for (int j=0;j<contadorMotores-1;j++){ 
			//SNx
			strcpy(temporal,"SN");
			sprintf(intastring,"%d",j+1);
			strcat(temporal,intastring);
			strcpy(stringOperacionesMatlab,temporal);
			strcat(stringOperacionesMatlab,"=(");
			for (int m=0;m<contadorMotores-j-1;m++){
				strcat(stringOperacionesMatlab,variablesNormales[m+1+j]);	
				//printf("Esto es: %s",variablesNormales[m+1+j]);
				if (m<contadorMotores-2-j){
					strcat(stringOperacionesMatlab,"*");
				}
			}
			strcat(stringOperacionesMatlab,")");
			engEvalString(ep, stringOperacionesMatlab);					 //ABILITAR FUERA DE PRUEBAS
			GuardarEnCSVdesdeMatlab(ep,nombreCSVMatlab,temporal,1);
			engEvalString(ep, "fprintf(MatricesC, newline)");  
			printf("\nOperación: %s", stringOperacionesMatlab); 
			//IIx
			strcpy(temporal,"II");
			sprintf(intastring,"%d",j+1);
			strcat(temporal,intastring);
			strcpy(stringOperacionesMatlab,temporal);
			strcat(stringOperacionesMatlab,"=(");
			for (int n=0;n<j+1;n++){
				strcat(stringOperacionesMatlab,variablesInversas[j-n]);	
				if (n<j){
					strcat(stringOperacionesMatlab,"*");
				}
			}
			strcat(stringOperacionesMatlab,")");
			engEvalString(ep, stringOperacionesMatlab); 				//ABILITAR FUERA DE PRUEBAS
			GuardarEnCSVdesdeMatlab(ep,nombreCSVMatlab,temporal,1);
			engEvalString(ep, "fprintf(MatricesC, newline)");  
			printf("\nOperación: %s", stringOperacionesMatlab); 
			//SIx
			strcpy(temporal,"SI");
			sprintf(intastring,"%d",j+1);
			strcat(temporal,intastring);
			strcpy(stringOperacionesMatlab,temporal);
			strcat(stringOperacionesMatlab,"=(");
			for (int o=0;o<j+1;o++){
				strcat(stringOperacionesMatlab,variablesInversas[j-o]);	
				if (o<j){
					strcat(stringOperacionesMatlab,"*");
				}
			}
			strcat(stringOperacionesMatlab,"*NT)");
			engEvalString(ep, stringOperacionesMatlab); 				//ABILITAR FUERA DE PRUEBAS
			GuardarEnCSVdesdeMatlab(ep,nombreCSVMatlab,temporal,1);
			engEvalString(ep, "fprintf(MatricesC, newline)");  
			printf("\nOperación: %s", stringOperacionesMatlab); 
		
		printf("\n");
	}
	printf("\n");
	//engEvalString(ep, "NZ=33"); 
	//printf("****************************************************************************************eyooo\n");
	//arrayPrueba=mxArrayToString(engGetVariable(ep, "NZ"));
	//printf("Esto es: %s\n",arrayPrueba);
	//strcpy(arrayPrueba,engGetVariable(ep, "NZ"));
	//printf("Esto es2: %s\n",arrayPrueba);
	//printf("\neyoo2");
}



