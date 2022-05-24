run: correr_todos

all: compilar_todos

clean:
	@echo Limpiando ejecutables y resultados
	rm --force ej1 ej2 ej3 ej4
	rm --force --recursive resultados*
	rm --force _tiempo.temp

limpiar: clean

compilar_todos: ej1 ej2 ej3 ej4

ej1: ej1.cpp
	g++ ej1.cpp -O3 --output ej1 
ej2: ej2.cpp
	g++ ej2.cpp -O3 --output ej2 
ej3: ej3.cpp
	g++ ej3.cpp -O3 --output ej3 
ej4: ej4.cpp
	g++ ej4.cpp -O3 --output ej4 

correr_todos: compilar_todos correr_ej1 correr_ej2 correr_ej3 correr_ej4

correr_ej1: ej1
	@bash correr_ejercicio.sh 1 10000  # Subir a 600 para la entrega
correr_ej2: ej2
	@bash correr_ejercicio.sh 2 10000  # Subir a 600 para la entrega
correr_ej3: ej3
	@bash correr_ejercicio.sh 3 10000
correr_ej4: ej4
	@bash correr_ejercicio.sh 4 10000

