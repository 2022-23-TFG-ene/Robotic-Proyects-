/*
 *	engdemo.cpp
 *
 *	A simple program to illustrate how to call MATLAB
 *	Engine functions from a C++ program.
 *
 * Copyright 1984-2016 The MathWorks, Inc.
 * All rights reserved
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "engine.h"
#define  CONTADORMOTORES 5

void IncluirMatlabSyms(int contadorMotores,Engine *ep);
void CrearMatricesNormalesSimplesMatlab(int contadorMotores,Engine *ep, char variablesNormales[contadorMotores][10]);
void CrearMatricesInversasSimplesMatlab(int contadorMotores,Engine *ep);

int main(){
	Engine *ep;
	int contadorMotores=CONTADORMOTORES;
	char variablesNormales[CONTADORMOTORES][10];
	

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
	//CrearMatricesInversasSimplesMatlab(contadorMotores,ep);
	/*
	for (int j=0; j<contadorMotores;j++){
		printf("%s ", variablesNormales[j]);
	}*/
	printf("El 0: %s ", variablesNormales[0]);
	printf("El 1: %s ", variablesNormales[1]);
	printf("El 2: %s ", variablesNormales[2]);
	printf("El 3: %s ", variablesNormales[3]);
	printf("El 4: %s ", variablesNormales[4]);
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

	
	for (int i=0;i<contadorMotores;i++){
		sprintf(intastring,"%d",i+1);
		
		strcpy(variablesNormales[i],"N");
		strcat(variablesNormales[i],intastring);
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
	}
}

void CrearMatricesInversasSimplesMatlab(int contadorMotores,Engine *ep){
	//Crear matrices Inversas
	char matriBaseInversa[100000];
	char intastring[100];
	for (int i=0;i<contadorMotores;i++){
		sprintf(intastring,"%d",i+1);
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
	}
	printf("****************************\n\n");
}

