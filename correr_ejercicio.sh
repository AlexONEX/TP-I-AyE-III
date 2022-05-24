## Invocación: bash correr_ejercicio.sh X T
## Dónde X es el número de ejercicio (1 a 4) y T es el tiempo máximo en segundos

#!/bin/bash
test -e /usr/bin/time
if [ $? == 1 ];
then
	echo "Comando \"time\" no instalado."
	echo "Instalar con: sudo apt install time"
	exit 1
else
	ej=$1
	TIMEOUT=$2
	echo "Ejercicio $ej (con timeout de $TIMEOUT seg)"
	rm --force --recursive resultados-$ej; mkdir resultados-$ej
	
	instancias=$ej
	if [ $ej == 2 ]; then instancias=1; fi
	for inst in $(ls -U instancias-$instancias)
	do
		/usr/bin/time --format %e --output _tiempo.temp timeout $TIMEOUT \
			./ej$ej < instancias-$instancias/$inst > resultados-$ej/$inst.out &
		wait $!
		
		if [ $? == 124 ]
		then
			string="?\t>$TIMEOUT\t"
			echo -en " $string"
			echo -en     $string >> resultados-$ej/_todas.txt
			rm resultados-$ej/$inst.out
		
		else
			valor_resultado=$(head -n1 resultados-$ej/$inst.out)
			tiempo=$(cat _tiempo.temp)
			var=$(sed '2q;d' resultados-$ej/$inst.out)
			string="$valor_resultado\t$tiempo\t"
			echo -en " $string"
			echo -en     $string >> resultados-$ej/_todas.txt
		fi
		
		echo $inst
		echo $inst >> resultados-$ej/_todas.txt
	done
	rm _tiempo.temp
fi

