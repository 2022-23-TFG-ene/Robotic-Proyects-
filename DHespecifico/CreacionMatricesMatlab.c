
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "engine.h"
#define  CONTADORMOTORES 5

void IncluirMatlabSyms(int contadorMotores,Engine *ep);
void CrearMatricesNormalesSimplesMatlab(int contadorMotores,Engine *ep, char variablesNormales[contadorMotores][10]);
void CrearMatricesInversasSimplesMatlab(int contadorMotores,Engine *ep, char variablesInversas[CONTADORMOTORES][10]);
void CalculosMatlabConjuntos(int contadorMotores,Engine *ep, char variablesInversas[CONTADORMOTORES][10], char variablesNormales[contadorMotores][10]);
void CalculosMatlabConjuntosTotal(int contadorMotores,Engine *ep, char variablesInversas[CONTADORMOTORES][10], char variablesNormales[contadorMotores][10]);
void CalculosMatlabConjuntosParciales(int contadorMotores,Engine *ep, char variablesInversas[CONTADORMOTORES][10], char variablesNormales[contadorMotores][10]);
void GuardarEnCSVdesdeMatlab(Engine *ep,char nombreCSV[200],char nombreVariable[10]);
void ModificacionMatricesNormalesEInversas(Engine *ep);

int main(){
	Engine *ep;
	int contadorMotores=CONTADORMOTORES;
	char variablesNormales[CONTADORMOTORES][10];
	char variablesInversas[CONTADORMOTORES][10];

	/*
	 * Call engOpen with a NULL string. This starts a MATLAB process 
    	 * on the current host using the command "matlab".
	 */
	if (!(ep = engOpen(""))) {
		fprintf(stderr, "\nCan't start MATLAB engine\n");
		return EXIT_FAILURE;
	}
	
	IncluirMatlabSyms(contadorMotores,ep);
	CrearMatricesNormalesSimplesMatlab(contadorMotores,ep,variablesNormales);
	CrearMatricesInversasSimplesMatlab(contadorMotores,ep,variablesInversas);
	CalculosMatlabConjuntos(contadorMotores,ep,variablesInversas,variablesNormales);
	CalculosMatlabConjuntosParciales(contadorMotores,ep,variablesInversas,variablesNormales);
	
	
	//COMPROBACIONES
	printf("\nNormales\n");
	printf("El 0: %s ", variablesNormales[0]);
	printf("El 1: %s ", variablesNormales[1]);
	printf("El 2: %s ", variablesNormales[2]);
	printf("El 3: %s ", variablesNormales[3]);
	printf("El 4: %s ", variablesNormales[4]);
	printf("\nInversas\n");
	printf("El 4: %s ", variablesInversas[0]);
	printf("El 3: %s ", variablesInversas[1]);
	printf("El 2: %s ", variablesInversas[2]);
	printf("El 1: %s ", variablesInversas[3]);
	printf("El 0: %s ", variablesInversas[4]);
	/*
	engEvalString(ep, "NF=simplify(N1*N2)");
	engEvalString(ep, "fid = fopen('MATRIZMATLAB.csv', 'w');"); 
	engEvalString(ep, "fprintf(fid, char(NF));"); 
	engEvalString(ep, "fclose(fid)");
	*/
	
	engClose(ep);
	
return EXIT_SUCCESS;
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
		GuardarEnCSVdesdeMatlab(ep,nombreCSVMatlab,nombreVariableTemporal);
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
		GuardarEnCSVdesdeMatlab(ep,nombreCSVMatlab,nombreVariableTemporal);
	}
	printf("****************************\n\n");
}

void CalculosMatlabConjuntos(int contadorMotores,Engine *ep, char variablesInversas[CONTADORMOTORES][10], char variablesNormales[contadorMotores][10]){
	//Total
	CalculosMatlabConjuntosTotal(contadorMotores,ep,variablesInversas,variablesNormales);
	//parejas 12-23-34...
	
	//parciales 1-2/1-3/1-4/2-5
	
}


void CalculosMatlabConjuntosTotal(int contadorMotores,Engine *ep, char variablesInversas[CONTADORMOTORES][10], char variablesNormales[contadorMotores][10]){
	char stringOperacionesMatlab[100000];
	char temporal [100];
	char intastring[100];
	char nombreTemporalVarialbe [20];
	printf("TOTAL:\n");
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
	printf("\nOperación: %s\n", stringOperacionesMatlab); //Ej: NT=simplify(N1*N2*N3*N4*N5)
	//Iteraciones
	for (int j=0;j<contadorMotores-1;j++){
			//SNx
			strcpy(temporal,"SN");
			sprintf(intastring,"%d",j+1);
			strcat(temporal,intastring);
			strcpy(stringOperacionesMatlab,temporal);
			strcat(stringOperacionesMatlab,"=simplify(");
			for (int m=0;m<contadorMotores-j;m++){
				strcat(stringOperacionesMatlab,variablesNormales[m+1+j]);	
				if (m<contadorMotores-2-j){
					strcat(stringOperacionesMatlab,"*");
				}
			}
			strcat(stringOperacionesMatlab,")");
			//engEvalString(ep, stringOperacionesMatlab);					 //ABILITAR FUERA DE PRUEBAS
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
			//engEvalString(ep, stringOperacionesMatlab); 				//ABILITAR FUERA DE PRUEBAS
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
			//engEvalString(ep, stringOperacionesMatlab); 				//ABILITAR FUERA DE PRUEBAS
			printf("\nOperación: %s", stringOperacionesMatlab); 
		
		printf("\n");
	}
	printf("\n");
}


void CalculosMatlabConjuntosParciales(int contadorMotores,Engine *ep, char variablesInversas[CONTADORMOTORES][10], char variablesNormales[contadorMotores][10]){
	char stringOperacionesMatlab[100000];
	char temporal [100];
	char intastring[100];
	int subconjuntos=2;
	int contador=contadorMotores-1;
	int contador2=contador;
	int contador3=1;
	printf("PARCIALES:");
	for (int i=contadorMotores;i-1>1;i--){ //nº de combinaciones de subconjuntos
		for (int j=0; j<subconjuntos;j++){ //nº de subconjuntos por combinacióbn
			printf("\n\nPARCIAL :%d - %d\n", j+1,contador2);
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
				strcat(stringOperacionesMatlab,"=simplify(II");
				strcat(stringOperacionesMatlab,intastring);
				strcat(stringOperacionesMatlab,"*NT)");
				//engEvalString(ep, stringOperacionesMatlab);					 //ABILITAR FUERA DE PRUEBAS
				printf("\nOperación: %s\n", stringOperacionesMatlab); 
			}
			contador3=1;
		}
		contador2=i-2;
		subconjuntos++;
		contador--;
	}
}

void GuardarEnCSVdesdeMatlab(Engine *ep,char nombreCSV[200],char nombreVariable[10]){
	char stringAEvaluar [1000];
	char stringTemporal [100];
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
	//Guardar variable deseada
	strcpy(stringAEvaluar,"fprintf(fid, char(");
	strcat(stringAEvaluar, nombreVariable);
	strcat(stringAEvaluar, "));");
	engEvalString(ep, stringAEvaluar);
	engEvalString(ep, "fprintf(fid, newline)");  
	engEvalString(ep, "fclose(fid)");
}

void ModificacionMatricesNormalesEInversas(Engine *ep){
	char decision;
	printf("Si desea sustituir o modificar las matrices normales o inversas en los ficheros \"VariablesNormalesMATLAB\" o \"VariablesInversasMATLAB\"\n");
	printf("¿Han sido modificadas manualmente las matrices anteriormente mencionadas? (s/n): ");
	scanf("%c",&decision);
	if (decision =='s'){
			
	}
}




