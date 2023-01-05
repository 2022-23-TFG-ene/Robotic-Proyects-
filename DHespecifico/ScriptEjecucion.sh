#!/bin/bash

#Necesario exportar variables para ejecutar el programa CreacionMatricesMatlab
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/MATLAB/R2022b/bin/glnxa64
export PATH=$PATH:/usr/local/MATLAB/R2022b/bin

#Si se quiere llamar a matlab usando un terminal con el alias matlab
#alias matlab='/usr/local/MATLAB/R2022b/bin/glnxa64/MATLAB'  

PS3="Elija el programa a ejecutar: "
select lng in Denavit_Hartenberg IntroduccionDatosMotores ProgramaSustituirVariablesMatriz CreacionMatricesMatlab Programa_Seguido_1_2_3 Programa_Seguido_2_4 Salir
do
	case $lng in
		"Denavit_Hartenberg")
			echo "$lng - Ejecutando Denavit_Hartenberg"
			gcc Denavit_Hartenberg.c
			./a.out;;
		"IntroduccionDatosMotores")
			echo "$lng - Ejecutando IntroduccionDatosMotores"
			gcc IntroduccionDatosMotores.c
			./a.out;;
		"ProgramaSustituirVariablesMatriz")
			echo "$lng - Ejecuntando ProgramaSustituirVariablesMatriz"
			gcc ProgramaSustituirVariablesMatriz.c -lm
			./a.out;;
		"CreacionMatricesMatlab")
			echo "$lng - Ejecutando CreacionMatricesMatlab"
			mex -client engine CreacionMatricesMatlab.c 
			./CreacionMatricesMatlab;;
		"Programa_Seguido_1_2_3")
			echo "$lng - Ejecutando Denavit_Hartenberg"
			gcc Denavit_Hartenberg.c
			./a.out
			echo "$lng - Ejecutando IntroduccionDatosMotores"
			gcc IntroduccionDatosMotores.c
			./a.out
			echo "$lng - Ejecuntando ProgramaSustituirVariablesMatriz"
			gcc ProgramaSustituirVariablesMatriz.c -lm
			./a.out;;
		"Programa_Seguido_2_4")
			echo "$lng - Ejecutando IntroduccionDatosMotores"
			gcc IntroduccionDatosMotores.c
			./a.out
			echo "$lng - Ejecutando CreacionMatricesMatlab"
			mex -client engine CreacionMatricesMatlab.c 
			./CreacionMatricesMatlab;;
			
		"Salir")
		exit 0
		break;;
		*)
          echo "Elija una de las opciones propuestas";;
	esac
done
