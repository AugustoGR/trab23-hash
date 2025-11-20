/*
TITULO: V2 Biblioteca com funções para calcular o tempo
DATA: 24/Agosto/2018

24/8/2018 - Abandono do gettimeofday para usar a função clock que
* é padrão C ANSI. 
*/

#ifndef __TEMPO__
/* Sempre que se cria uma biblioteca é prudente usar um define para ela.
 * Veja, as definições abaixo só ocorrerão se NÃO HOUVER AINDA a constante
 * __TEMPO__. Isto evita erros com duplas inclusões, ou seja, dois ou mais
 * #include "tempo.h". Apenas o primeiro terá efeito e os demais nada
 * definirão. Todos os .h deveriam usar isto.
 * */
#define __TEMPO__

/* 24/8 Maravilha. Agora de um jeito só para Linux e Windows. 
 * Testado com sucesso em:
 * - Windows XP 32 bits com Dev++
 * - Windows XP 32 bits com Codeblocks
 * - Linux 64 bits
 * */
 
#include <time.h>

/* para muitos códigos, um inteiro longo sem sinal é o ideal.
Se for um compilador de 32 bits, isto resultará em um inteiro
de 32 bits. Porém se for um compilador de 64 bits, será um inteiro
de 64 bits.

Crio um typedef pois é muita coisa ficar digitando unsigned long int
Assim, só se coloca ulong
*/
typedef unsigned long int ulong;

/* uma função para medir o tempo.
 * Ela retorna a quantidade de microsegundos que se passaram
 * desde a última vez que a mesma função foi chamada.
 *  
 * Em 24 de agosto de 2018 ela passou a ser a mesma para Linux e para Windows, 
 * pois o problema é que no
 * Windows não tinha a função gettimeofday
*/
ulong tempo()
{
    static clock_t ti, tf;
    static int vezes = 0;
    static int fator=1000000/CLOCKS_PER_SEC;
    /* Em muitos casos isso acima dará apenas 1. Porém pode ser que 
     * CLOCKS_PER_SEC seja menor 
     * */
    
    ulong ms;
    
    if (vezes == 0) {
        /* Primeira invocação */
        vezes = 1;
        ti = clock();
        /* Clock é ANSI e retorna quantos ciclos de CPU se passaram 
         * desde o início do programa. O padrão POSIX, segundo manual
         * do clock, estabelece que 1s tem 1.000.000 de clocks independente
         * do sistema. Sendo isso verdade (que é setado pela constante CLOCKS_PER_SEC)
         * então o retorno de clock acaba já sendo em microssegundos.
         */
        return (0);
    }

//printf("DEBUG: CLOCKS_PER_SEC = %u\n", CLOCKS_PER_SEC);

/* pega o tempo atual. Funcao para LINUX */
    tf = clock();
		
    //CLOCKS_PER_SEC
	ms = (tf - ti)*fator;
	
	ti = tf;
    
    vezes++;
    return (ms);
}


/* Retorna uma string com o tempo formatado em  seg,  mseg e useg */
char *formata(ulong m)
{
    static char tempo[30];
    ulong s, ms, us;
    s = m / 1000000;
    ms = (m % 1000000) / 1000;
    us = (m % 1000);
    sprintf(tempo, "%02lus %03lums %03luus", s, ms, us);
    return (tempo);
}
#endif

