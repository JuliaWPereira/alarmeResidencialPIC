/*
  PROJETO FINAL - ALARME RESIDENCIAL

  @version last version
  @author Julia Wotzasek Pereira
  @edited 23 de junho de 2019

*/

/* Ligacoes entre o PIC e o LCD */
  sbit LCD_RS at RE2_bit;   // PINO 2 DO PORTD LIGADO AO RS DO DISPLAY
  sbit LCD_EN at RE1_bit;   // PINO 3 DO PORTD LIGADO AO EN DO DISPLAY
  sbit LCD_D7 at RD7_bit;  // PINO 7 DO PORTD LIGADO AO D7 DO DISPLAY
  sbit LCD_D6 at RD6_bit;  // PINO 6 DO PORTD LIGADO AO D6 DO DISPLAY
  sbit LCD_D5 at RD5_bit;  // PINO 5 DO PORTD LIGADO AO D5 DO DISPLAY
  sbit LCD_D4 at RD4_bit;  // PINO 4 DO PORTD LIGADO AO D4 DO DISPLAY

/* Selecionando direcao do fluxo de dados dos pinos utilizados para comunicacao com o display */
  sbit LCD_RS_Direction at TRISE2_bit;  // SETA DIRECAO DO FLUXO DE DADOS DO PINO 2 DO PORTD
  sbit LCD_EN_Direction at TRISE1_bit;  // SETA DIRECAO DO FLUXO DE DADOS DO PINO 3 DO PORTD
  sbit LCD_D7_Direction at TRISD7_bit;  // SETA DIRECAO DO FLUXO DE DADOS DO PINO 7 DO PORTD
  sbit LCD_D6_Direction at TRISD6_bit;  // SETA DIRECAO DO FLUXO DE DADOS DO PINO 6 DO PORTD
  sbit LCD_D5_Direction at TRISD5_bit;  // SETA DIRECAO DO FLUXO DE DADOS DO PINO 5 DO PORTD
  sbit LCD_D4_Direction at TRISD4_bit;  // SETA DIRECAO DO FLUXO DE DADOS DO PINO 4 DO PORTD

/* Contantes do Programa */
  #define NDIGITOS    4
  
/* Variaveis Globais */
  unsigned int uiAlarmeLigado;
  unsigned int uiValorAD;
  unsigned int dutyCicle;
  unsigned int pot1 = 0;
  char ucRead; // Variavel para armazenar o dado lido
  unsigned int uiSenha[4] = {1,2,3,4};
  unsigned int uiSenhaDigitada[4];
  unsigned int uiMudaSenha = 0;
  unsigned int uiLeSenha = 0;
  unsigned int uiSenhaFoiVerificada = 0;
  unsigned int uiSenhaFoiDigitada = 0;
  unsigned int uiContadorDigitos = NDIGITOS;
  unsigned int uiLigarAlarme = 0;
  unsigned int uiDesligarAlarme = 0;
  unsigned int uiSenhaDeveMudar = 0;
  unsigned char ucContador; // variavel de armazenamento do contador
  unsigned int uiAtivarAlarme = 0;
  unsigned int uiValorB = 0;
  unsigned int uiPrimeiraVez = 0;
  unsigned int uiContadorNovo = 0;
  unsigned int uiIniciaTimer = 0;

/* Prototipos das funcoes auxiliares */
  void setup_LCD();
  void setup_PIR();
  void setup_Potenciometro();
  void setup_LEDs();
  void setup_USART();
  void setup_TIMER();
  void read_Potenciometro();
  void acende_leds(int valor);
  void read_keypad();
  void execute_ucRead();
  void verifica_senha();
  void interrupt();

/* Funcao principal */
  void main() {
    /* Chamada das funcoes de setup */
     setup_LCD();
     setup_PIR();
     setup_Potenciometro();
     setup_LEDs();
     setup_USART();
     setup_TIMER();

     uiAlarmeLigado = 0;

     while(1)
     {
         read_Potenciometro();
         read_keypad();
         execute_ucRead();
         delay_ms(500);
         if(uiAlarmeLigado == 1){
           if(uiPrimeiraVez == 1){
             Lcd_Cmd(_LCD_CURSOR_OFF);//Apaga cursor
             Lcd_Cmd(_LCD_CLEAR);//Limpa display
             uiPrimeiraVez = 0;
           }
           Lcd_Out(1,4,"Alarme On!");
           if(PORTC.RC3 == 0 ){
              PORTC.RC1 = 1;
              delay_ms(100);
           }else{
              PORTC.RC1 = 0;
              delay_ms(100);
           }
        }else{
            Lcd_Out(1,3,"Alarme Off!");
            PORTC.RC1 = 1;
            delay_ms(100);
        }

     }
  }

/* Funcao de setup do LCD */
  void setup_LCD()
  {
     ADCON1  = 0x0E;//Configura os pinos do PORTB como digitais, e RA0 (PORTA) como analogico
     Lcd_Init();//Inicializa modulo LCD
     Lcd_Cmd(_LCD_CURSOR_OFF);//Apaga cursor
     Lcd_Cmd(_LCD_CLEAR);//Limpa display

  }

/* Funcao de setup do sensor de presen?a PIR */
   void setup_PIR()
   {
      TRISC = 0b00001100; // Configura como entrada
   }

/* Funcao de setup do potenciometro */
   void setup_Potenciometro()
   {
      TRISA = 0xFF;
      PORTA = 0xFF;
      ADCON0 = 0b00000001;
      ADCON1 = 0b11001110;
      ADCON2 = 0b10111111;
   }

/* Funcao para leitura do potenciometro */
   void read_Potenciometro()
   {
     Delay_10us;
     pot1 = ADC_Get_Sample(0);
     pot1 = pot1 * 5 / 1023;
     acende_leds(pot1);
   }

/* Funcao para acender os leds */
   void acende_leds(int valor)
   {
      unsigned int faixa;
      
      if(uiIniciaTimer == 1){
         faixa = (255 - ucContador * (255/(pot1 * 5)))/31;
         if(faixa == 1) PORTB = 0b10000000;
         if(faixa == 2) PORTB = 0b11000000;
         if(faixa == 3) PORTB = 0b11100000;
         if(faixa == 4) PORTB = 0b11110000;
         if(faixa == 5) PORTB = 0b11111000;
         if(faixa == 6) PORTB = 0b11111100;
         if(faixa == 7) PORTB = 0b11111110;
         if(faixa == 8) PORTB = 0b11111111;
      }else{
         PORTB = 0x00;
      }
   }

/* Funcao de setup dos LEDs */
   void setup_LEDs()
   {
      TRISB =  0x00; // configura os pinos de B como saida
      PORTB =  0x00; // configura todas as saidas como zero
   }

/* Funcao de setup da comunicacao USART */
   void setup_USART()
   {
      UART1_Init(9600); // Utiliza bibliotecas do compilador para configuracao do Baud rate
   }

/* Funcao de setup do TIMER0 */
   void setup_TIMER()
   {
       ADCON1 = 0x0F;           // Configura todos os pinos A/D como I/O  
       TRISD = 0;               // Define todos os pinos Do PORTD como saida.
       ucContador=0;            // Inicializa a variavel com o valor 0.

       // Configuracao do Timer0.
                                // Cristal de 8Mhz, ciclo de m?quina: 8MHz / 4 = 2Mhz --> 1/2Mhz = 0,5us.
       T0CON.T0CS = 0;          // Timer0 operando como temporizador.
       T0CON.PSA = 0;           // Prescaler ativado.
       T0CON.T0PS2 = 1;         // Define prescaler 1:256.
       T0CON.T0PS1 = 1;         // Define prescaler 1:256.
       T0CON.T0PS0 = 1;         // Define prescaler 1:256.
       T0CON.T08BIT = 0;        // Define contagem no modo 16 bits.
       // Valor para 1 segundo.
       TMR0H = 0xE1;            // Carrega o valor alto do numero 57723.
       TMR0L = 0x7B;            // Carrega o valor baixo do numero 57723.
       INTCON.TMR0IE = 1;       // Habilita interrupcao do timer0.
       INTCON.TMR0IF = 0;       // Apaga flag de estouro do timer0, pois eh fundamental para a sinalizacao do estouro.
       T0CON.TMR0ON = 0;        // Liga timer0.
       INTCON.GIE = 1;          // Habilita as interrupcoes nao-mascaradas.
       INTCON.PEIE = 1;         // Habilita as interrupcoes dos perifericos.
   }

/* Funcao para ler o teclado do arduino */
   void read_keypad()
   {
      if(UART1_Data_Ready()){  // Verifica se um dado foi recebido no buffer
         Delay_ms(50);
         ucRead = UART1_Read(); // Le o dado recebido do buffer
         Delay_ms(50);
      }
   }

/* Funcao para realizar a atividade de acordo com o botao apertado */
   void execute_ucRead()
   {
     int i;
       switch(ucRead){
          case 'A': // Botao para ligar o alarme
             if(uiLeSenha == 0){
                uiLeSenha = 1;
                uiContadorDigitos = NDIGITOS;
                uiLigarAlarme = 1;
                Lcd_Cmd(_LCD_CURSOR_OFF);//Apaga cursor
                Lcd_Cmd(_LCD_CLEAR);//Limpa display
                Lcd_Out(2,1,"Senha: ");
             }
             break;
          case 'B': // Botao para desligar o alarme
             if(uiLeSenha == 0){
                uiLeSenha = 1;
                uiContadorDigitos = NDIGITOS;
                uiDesligarAlarme = 1;
                uiIniciaTimer = 0;
                Lcd_Cmd(_LCD_CURSOR_OFF);//Apaga cursor
                Lcd_Cmd(_LCD_CLEAR);//Limpa display
                Lcd_Out(2,1,"Senha: ");
             }
             break;
          case 'C': // Botao para mudar a senha
             if(uiAlarmeLigado == 1){
                Lcd_Cmd(_LCD_CURSOR_OFF);//Apaga cursor
                Lcd_Cmd(_LCD_CLEAR);//Limpa display
                Lcd_Out(1,2,"Alarme Ligado!");
                Lcd_Out(2,2,"Acesso Negado!");
                delay_ms(5000);
                Lcd_Cmd(_LCD_CURSOR_OFF);//Apaga cursor
                Lcd_Cmd(_LCD_CLEAR);//Limpa display
             } else {
                if(uiLeSenha == 0){
                   uiLeSenha = 1;
                   uiMudaSenha = 1;
                   uiContadorDigitos = NDIGITOS;
                   Lcd_Cmd(_LCD_CURSOR_OFF);//Apaga cursor
                   Lcd_Cmd(_LCD_CLEAR);//Limpa display
                   Lcd_Out(2,1,"Senha: ");
                }
             }

             break;
          default: // Leitura dos numeros
               if(uiLeSenha == 1 && !(ucRead == 69)){
                  uiSenhaDigitada[NDIGITOS - uiContadorDigitos] = (int)ucRead - 48; // arruma ucRead para int
                  Lcd_Out(2,7 + (NDIGITOS - uiContadorDigitos),"*");
                  uiContadorDigitos--;
                  if(uiContadorDigitos == 0){
                     if(uiSenhaDeveMudar == 1){
                        uiLeSenha = 0;
                        for(i = 0;i < NDIGITOS;i++){
                           uiSenha[i] = uiSenhaDigitada[i];
                        }
                        uiSenhaDeveMudar = 0;
                        ucContador = 0;
                        Lcd_Cmd(_LCD_CURSOR_OFF);//Apaga cursor
                        Lcd_Cmd(_LCD_CLEAR);//Limpa display
                        Lcd_Out(1,6,"Senha");
                        Lcd_Out(2,4,"Alterada");
                        delay_ms(1000);
                        Lcd_Cmd(_LCD_CURSOR_OFF);//Apaga cursor
                        Lcd_Cmd(_LCD_CLEAR);//Limpa display
                     }else{
                       verifica_senha();
                       uiLeSenha = 0;
                       if(uiSenhaFoiVerificada == 1){
                           Lcd_Cmd(_LCD_CURSOR_OFF);//Apaga cursor
                           Lcd_Cmd(_LCD_CLEAR);//Limpa display
                          if(uiLigarAlarme == 1)
                          {
                             T0CON.TMR0ON = 1;
                             uiIniciaTimer = 1;
                             uiLigarAlarme = 0;
                          }
                          if(uiDesligarAlarme == 1){
                             ucContador = 0;
                             uiAlarmeLigado = 0;
                              T0CON.TMR0ON = 0;
                             uiDesligarAlarme = 0;
                          }
                          if(uiMudaSenha == 1){
                             uiLeSenha = 1;
                             uiSenhaDeveMudar = 1;
                             uiMudaSenha = 0;
                             ucContador = 0;
                             uiIniciaTimer = 0;
                             T0CON.TMR0ON = 0;
                             uiContadorTentativas = NTENTATIVAS - 1;
                             uiContadorDigitos = NDIGITOS;
                             uiDesligarAlarme = 0;
                             Lcd_Cmd(_LCD_CURSOR_OFF);//Apaga cursor
                             Lcd_Cmd(_LCD_CLEAR);//Limpa display
                             Lcd_Out(2,1,"Senha: ");
                          }
                       }else{
                           Lcd_Cmd(_LCD_CURSOR_OFF);//Apaga cursor
                           Lcd_Cmd(_LCD_CLEAR);//Limpa display
                           Lcd_Out(1,6,"Senha");
                           Lcd_Out(2,4,"Incorreta");
                           delay_ms(1000);
                           Lcd_Cmd(_LCD_CURSOR_OFF);//Apaga cursor
                           Lcd_Cmd(_LCD_CLEAR);//Limpa display
                       }
                    }
               }
            }
       }
   }

/* Funcao para verificar se a senha esta correta */
   void verifica_senha()
   {
      int i; // contador
      for(i = 0, uiSenhaFoiVerificada = 1;i < NDIGITOS; i++){
         if(uiSenhaDigitada[i] != uiSenha[i]){
            uiSenhaFoiVerificada = 0;
         }
      }
   }
   
/* Funcao para interrupcao via timer */
   void interrupt(){
      // Incrementa Contador.
      if(INTCON.TMR0IF==1){       // Incrementa somente quando existir o overflow do timer 0.
         // Recarrega o timer0.
         TMR0H = 0xE1 ;           // Carrega o valor alto do n?mero 57723.
         TMR0L = 0x7B;            // Carrega o valor baixo do numero 57723.

         if(uiIniciaTimer == 1){
            if(uiContadorNovo == 5 * pot1){
               uiAlarmeLigado = 1;
               uiContadorNovo = 0;
               uiIniciaTimer = 0;
               uiPrimeiraVez = 1;
               uiMovimento = 1;
            }
            uiContadorNovo++;
         }
         INTCON.TMR0IF = 0;       // Limpa o flag de estouro do timer0 para uma nova contagem de tempo.
         uiValorB = ucContador;
         ucContador++;            // Esse operador aritmetico(++) realiza o mesmo que variavel = variavel + 1.
      }
   }