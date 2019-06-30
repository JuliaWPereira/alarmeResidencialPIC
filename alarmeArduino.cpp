/* PROJETO DE ALARME RESIDENCIAL
	
	@version final version
	@author Júlia Wotzasek Pereira
	@edited 23 de junho de 2019

*/

/* Bibliotecas utilizadas */
	#include <Keypad.h>

/* Definicao do teclado */
	const byte linhas = 4; //4 linhas
	const byte colunas = 4; //4 colunas
	byte pinoslinhas[linhas] = {46,47,48,49}; //pinos utilizados nas linhas
	byte pinoscolunas[colunas] = {50,51,52,53}; //pinos utilizados nas colunas

	/* Pinagem do buzzer*/
	const int buzzer = 10; // buzzer conectado no pino 10

	/* Pinagem do PIR e da saida digital */
	const int PIR = 13; // sensor PIR no pino 13
	const int out = 2;

/* Inicialização do teclado */
	Keypad teclado = Keypad( makeKeymap(matrizteclado), pinoslinhas, pinoscolunas, linhas, colunas );
	int tom; // variavel para apitar o buzzer quando apertar o botao

/* Leitura do teclado */
	int leDigito(){
	  char digito = teclado.getKey();
	    while(!digito){
	      digito = teclado.getKey();
	    }
	  int d = (int)digito;
	  tom = 420 + 100 * (d - 48);
	  digitalWrite(buzzer,HIGH);
	  delayMicroseconds(tom);
	  delay(100);
	  digitalWrite(buzzer,LOW);
	  delayMicroseconds(tom);
	  delay(100);
	  return d - 48;
	}

/* Setup */
	void setup() {
	   Serial1.begin(9600);
	   pinMode(buzzer,OUTPUT);
	   pinMode(PIR,INPUT);
	   pinMode(out,OUTPUT);
	}

/* Loop */
	void loop(){
	    int d,p;
	    d = leDigito();
	    p = digitalRead(PIR);
	    digitalWrite(out,p);
	    Serial1.Write(d);
	    Serial1.write('E');
	}
