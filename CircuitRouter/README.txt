Estrutura dos diretórios (relativamente ao atual):
	- ./CircuitRouter-ParSolver -> diretório com solução paralela
	- ./CircuitRouter-SeqSolver -> diretório com solução sequencial
	- ./lib -> source files necessários para correr o programa
	- ./Makefile
	- ./doTest.sh -> Shell script que testa o speedup da solução sequencial
	- ./inputs -> pasta com os inputs dados pelo enunciado
	- ./results -> diretório com os resultados de speedup

Compilação do programa:
No diretório principal, executar o comando "make", que automaticamente compilará todos os ficheiros necessários para executar.
Caso queira limpar todos os ficheiros não source files e ficheiros .res e .old, usar make clean (não apaga ficheiros de speedup).

Como correr o programa:
	Opção 1 - ficheiro doTest.sh
		1. Caso não tenha sido feito, correr "make"
		2. Executar o comando "./doTest.sh <numero de threads a usar> <input file>"
		3. Speedup irá aparecer na pasta results
	Opção 2 - Execução manual
		Executar o binário sequencial:
			./CircuitRouter-SeqSolver/CircuitRouter-SeqSolver <input file>
		Executar o binário paralelo:
			./CircuitRouter-ParSolver/CircuitRouter-Parsolver -t <numero de threads> <input file>


CPU utilizado:
	Intel Core i5 8250U
	4 cores, 8 threads
	Clock Rate: 1.60GHz
	Turbo Boost: 3.4GHz
	Modelo de laptop: Lenovo Ideapad 520s

Sistema Operativo usado para testar: Deepin 15.7
Ao executar "uname -a" resultado é: Linux rijura 4.15.0-29deepin-generic #31 SMP Fri Jul 27 07:12:08 UTC 2018 x86_64 GNU/Linux
