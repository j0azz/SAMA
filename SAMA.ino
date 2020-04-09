/*
 * 
 * SISTEMA AUTOMÁTICO DE MEDIÇÃO AXIAL - SAMA
 *
 * Modelo do Encoder: E6B2-CWZ6C
 * Modelo do LCD:     LCD 1602
 * Microcontrolador:  Arduino Mega 2560
 * 
 * Desenvolvido pelos colaboradores do projeto SAMA (2019)
 * Mantido e utilizado pelo Laboratório de Usinagem do Instituto Federal da Paraíba - IFPB (Itabaiana)
 * 
 * Orientação: Dr. Verilton Nunes
 * Desenvolvimento: Joás de Brito
 *
 */

#include <LiquidCrystal.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

int zeroButton = 10;
int currState=0, lastState=0;

int encoderPin1 = 2;
int encoderPin2 = 3;

volatile int lastEncoded = 0;
volatile long encoderValue = 0;

long lastencoderValue = 0;

int lastMSB = 0;
int lastLSB = 0;

float mm = 0, test = 0;

void setup() {
  Serial.begin(9600);
  
  lcd.begin (16,2);
  lcd.setCursor(0,0);
  lcd.print("SAMA (IFPB-ITA)");

  pinMode(zeroButton, INPUT);
  
  pinMode(encoderPin1, INPUT_PULLUP); 
  pinMode(encoderPin2, INPUT_PULLUP);

  digitalWrite(encoderPin1, HIGH); //turn pullup resistor on
  digitalWrite(encoderPin2, HIGH); //turn pullup resistor on

  //call updateEncoder() when any high/low changed seen
  //on interrupt 0 (pin 2), or interrupt 1 (pin 3) 
  attachInterrupt(0, updateEncoder, CHANGE); 
  attachInterrupt(1, updateEncoder, CHANGE);

}

void loop(){ 

  currState = !digitalRead(10);   //lê o estado do botão
  if(currState != lastState)
  {
    lastState = currState;
    Serial.println(currState);
    //mm = 0.0;
    encoderValue = 0;             //zera o valor medido pelo encoder
    lcd.setCursor(0,1);           //prepara para escrever na segunda linha do LCD
    lcd.print("                ");//apaga o que estava escrito
    lcd.setCursor(0,1);
    lcd.print("M= 0.0");
  }
 
  //a cada revolução do encoder, 1000 pulsos são detectados
  //a cada revolução da engrenagem, 142.87 mm são percorridos (C = 142.87 mm)
  //dessa forma, a cada pulso, são percorridos 0.14287 mm
 
   test = encoderValue * 0.14287;
  if(mm != test)                  //testa mudança de posição
  {
    mm = test;
    lcd.setCursor(0,1);           //prepara para escrever na segunda linha do LCD
    lcd.print("                ");//apaga o que estava escrito
    lcd.setCursor(0,1);
    lcd.print("M= ");
    lcd.print(mm);
  }
}

void updateEncoder(){
  int MSB = digitalRead(encoderPin1); //MSB = most significant bit
  int LSB = digitalRead(encoderPin2); //LSB = least significant bit

  int encoded = (MSB << 1) |LSB; //converting the 2 pin value to single number
  int sum  = (lastEncoded << 2) | encoded; //adding it to the previous encoded value

  if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) encoderValue ++;
  if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) encoderValue --;

  lastEncoded = encoded; //store this value for next time
}
