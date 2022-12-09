#!/bin/bash
#Necesario exportar variables para ejecutar el programa CreacionMatricesMatlab
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/MATLAB/R2022b/bin/glnxa64
export PATH=$PATH:/usr/local/MATLAB/R2022b/bin

PS3="Elija el programa a ejecutar: "
select lng in Denavit_Hartenberg IntroduccionDatosMotores ProgramaSustituirVariablesMatriz CreacionMatricesMatlab Salir
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
		"Salir")
		exit 0
		break;;
		*)
          echo "Ooops";;
	esac
done
