/*
Relógio, Calendário e controle de alarme de aulas para o IFRN - São Paulo do Potengi
Autor: Alex Santos
Com grandes contribuições de:
  Diego Cirilo [Função timeToStudy()]
  Blog Filipeflop [Setando e imprimindo valores do RTC] - http://blog.filipeflop.com/modulos/relogio-rtc-ds1307-arduino.html
Data: 19/09/2014

Pinos utilizados:
* RTC - SDA - SCL(AN4, AN5)
* BOTAO1 - PIN2
* RELE1/BUZZER - PIN13
* LCD Pin Allocation:
* LCD D4 - PIN4
* LCD D5 - PIN5
* LCD D6 - PIN6
* LCD D7 - PIN7
* LCD RS - PIN8
* LCD Enable - PIN9
* LCD Backlight - PIN10

Hardware Utilizado:
--- Arduino UNO R3 Development Board Microcontroller MEGA328P ATMEGA16U2 Compat for Arduino - Blue + Black
http://www.dx.com/p/215600

--- LCD KeyPad Shield ----
http://www.dx.com/p/118059 
Datasheet: http://www.dfrobot.com/wiki/index.php/Arduino_LCD_KeyPad_Shield_(SKU:_DFR0009)

--- Tiny RTC I2C Module w/ 24C32 Memory + DS1307 Clock - White ---
http://www.dx.com/p/258549

-- 2-Channel Relay Shield Module for Arduino (Relê)
http://www.dx.com/p/144140

*/
//Carrega a biblioteca do RTC DS1307 e do LCD
#include <DS1307.h>
#include <LiquidCrystal.h>
 
// Modulo RTC DS1307 ligado as portas A4 e A5 do Arduino 
// Inicializando as bibliotecas com os numeros dos pinos do arduino
DS1307 rtc(A4, A5);
LiquidCrystal lcd(8,9,4,5,6,7);

// aliases para os pinos
#define BOTAO1 2
#define SINETA 13

//variáveis globais
boolean botao1press;
uint8_t segundo;       // Esse tipo uint8_t vem da biblioteca do RTC
uint8_t minuto;
uint8_t hora;
uint8_t weekDay;       // Dia da semana (1-segunda, 2-terca, 3-quarta, 4-quinta, 5-sexta, 6-sabado, 7-domingo)
byte tempoLongo = 4;
byte tempoCurto = 2;

//funções existentes
void AcertaHora();     // Acerta o horario do RTC baseado com o que for escrito no codigo fonte
void getDate();        // Guarda as informações como hora, minuto, segundo em variaveis para posteior uso
void printTime ();     // Imprime data/hora no LCD e na serial
boolean timeToStudy(); // Verifica se o horario atual eh a hora de tocar sineta
 
void setup(){

  lcd.begin(16, 2);        // Seta o numero de Colunas e Linhas do LCD
  lcd.print("IFRN SPP");   // Mensagem de boas-vindas ao ligar o arduino
  delay(2000);             // Pequeno intervalo 
  lcd.clear();             // Limpa a tela
  
  pinMode(BOTAO1, INPUT);  // definindo função dos pinos
  pinMode(SINETA, OUTPUT); // definindo função dos pinos
  
  // Aciona o relogio
  rtc.halt(false);
  
  // Definicoes do pino SQW/Out
  rtc.setSQWRate(SQW_RATE_1);
  rtc.enableSQW(true);
  
  // Inicio da conexão serial na USB (serial monitor para exergar) 
  Serial.begin(9600);
}

void AcertaHora (){
  rtc.setDOW(5);             // Define o dia da semana (1-segunda, 2-terca, 3-quarta, 4-quinta, 5-sexta, 6-sabado, 7-domingo)
  rtc.setTime(13, 19, 00);   // Define o horario
  rtc.setDate(19, 9, 2014);  // Define o dia, mes e ano
}

// lê a data do RTC e escreve nas variáveis globais de data, hora, etc
void getDate(){
  // Pega a hora do RTC, separando hora, minuto, segundo...(olhar na biblioteca DS1307 o arquivo DS1307.cpp )
  // separando em variaveis para que a funcao timeToStudy() possa saber se eh hora de tocar
  segundo = rtc.getTime().sec;   // segundo
  minuto  = rtc.getTime().min;   // minuto
  hora    = rtc.getTime().hour;  // hora formato 24hrs
  weekDay = rtc.getTime().dow;   // dia da semana (1-segunda, 2-terca, 3-quarta, 4-quinta, 5-sexta, 6-sabado, 7-domingo)
}

void printTime(){
  //Mostra as informações no Serial Monitor
  Serial.print("Hora : ");
  Serial.print(rtc.getTimeStr());               // Escreve hora no formato 24hs
  Serial.print(" ");
  Serial.print("Data : ");
  Serial.print(rtc.getDateStr(FORMAT_SHORT));   // Escreve data no formato dd.mm.aaa
  Serial.print(" ");
  Serial.println(rtc.getDOWStr(FORMAT_SHORT));  // Escreve dia da semana com 3 digitos em inglês (se quiser que exiba em portugues editar o arquivo DS1307.cpp

  //Escreve no LCD
  lcd.setCursor(1, 0);  
  lcd.print(rtc.getDOWStr(FORMAT_SHORT));    // Escreve dia da semana com 3 digitos em inglês (se quiser que exiba em portugues editar o arquivo DS1307.cpp
  lcd.setCursor(5, 0);
  lcd.print(rtc.getDateStr(FORMAT_SHORT));  // Escreve data no formato dd.mm.aaa
  lcd.setCursor(5, 1);
  lcd.print(rtc.getTimeStr());              // Escreve hora no formato 24hs 23:59:00
}


// seletor dos momentos de tocar, retorna true se tiver na hora!
boolean timeToStudy(){

  // se não for domingo(0) nem sábado(6) 
  // verifica pelas horas se toca 4s(intervalos e inicio/fim) ou 2s(entre aulas)
  // alguns horários foram comentados porque o campus IFRN SPP ainda não está com aulas a noite.
  if((weekDay!=6 && weekDay!=7)){
    // toque de tempoLongo s para 07h00m, 12h00m, 13h00m, 18h00m, 19h00m
    if((hora==7 || hora==12 || hora==13 || hora==18 /*|| hora==19*/) && minuto==0 && (segundo>=0 && segundo<=tempoLongo))
      return true;

    // toque de tempoLongo s para 8h30m, 10h30m, 14h30m, 16h30m, 20h30m
    if((hora==8 || hora==10 || hora==14 || hora==16 /*|| hora==20*/) && minuto==30 && (segundo>=0 && segundo<=tempoLongo))
      return true;

    // toque de tempoLongo s para 08h50m, 14h50m
    if((hora==8 || hora==14) && minuto==50 && (segundo>=0 && segundo<=tempoLongo))
      return true;

    // toque de tempoLongo s para 10h20m, 16h20m
    if((hora==10 || hora==16) && minuto==20 && (segundo>=0 && segundo<=tempoLongo))
      return true;

    // toque de tempoLongo s para 20h40m, 22h10m
   /* if(((hora==20 && minuto==40) || (hora==22 && minuto==10)) && (segundo>=0 && segundo<=tempoLongo))
      return true;*/

    // toque de tempoCurto s para 07h45m, 13h45m, 19h45
    if((hora==7 || hora==13 /*|| hora==19*/) && minuto==45 && (segundo>=0 && segundo<=tempoCurto))
      return true;

    // toque de tempoCurto s para 09h35, 15h35m
    if((hora==9 || hora==15) && minuto==35 && (segundo>=0 && segundo<=tempoCurto))
      return true;

    // toque de tempoCurto s para 11h15m, 17h15m
    if((hora==11 || hora==17) && minuto==15 && (segundo>=0 && segundo<=tempoCurto))
      return true;

    // toque de tempoCurto s para 21h25m
  /*  if(hora==21 && minuto==25 && (segundo>=0 && segundo<=tempoCurto))
      return true;*/
  } else{
    return false;
  }
  return false;
}
   
void loop(){
  delay(100);  // Diminui em 100ms o tempo de repetição
  botao1press =  digitalRead(BOTAO1);  
  if(botao1press){
    AcertaHora ();
  }

  getDate();  
  printTime();  
  
  if(timeToStudy()){
    digitalWrite(SINETA, HIGH); // Se estiver na hora de tocar, aciona buzzer/relé que controla a sirene
  }
  else{
    digitalWrite(SINETA, LOW); // Desliga Buzzer/Relé
  }
 
}
