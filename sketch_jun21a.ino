//BIBLIOTECAS
#include <HX711.h>
#include <Servo.h>
#include <Wire.h>
#include <DS3231.h>
/*
Sensores:
  Célula de carga -> 2 variáveis digitais
    PinoDT = 40
    pinoSCK = 41
  
  Detectores de nível -> 2 variáveis digitais
    nivel_baixo = 32
    nivel_alto = 34
  
  Real time clock -> 2 variáveis digitais
    pinoSDA = 20
    pinoSCL = 21
    
Atuadores:
  Relé bomba = 22
  Relé motor = 26
  Pino controle Servo = 2
*/


// SENSORES
#define rtcSCL  21
#define rtcSDA  20

#define nivel_baixo 32
#define nivel_alto  33

#define pinDTCelulaDeCarga  40
#define pinSCKCelulaDeCarga 41


// ATUADORES
#define bomba  22
#define motor  26
#define pinServo  2


// VARIÁVEIS CONSTANTES AUXILIARES
#define pesoMin 0.010
#define pesoMax 30.0
#define escala -351666.0f

// DEFINIÇÃO OS OBJETOS DE BIBLIOTECAS
Servo servo;
HX711 balanca;
DS3231 clock;
//RTClib clock;

// VARIÁVEIS RECEBIDAS
float pesoDesejado = 0;
int horas[4] = {1,2,3,4};
int minutos[4] = {1,2,3,4};


// VARIÁVEIS MEDIDAS
float pesoMedido = 0;
int horaAtual = 0;
int minutoAtual = 0;
int nivelBaixo = LOW;
int nivelAlto = LOW;

// VARIÁVEIS AUXILIARES
bool enchendoRacao = false;
int i = 0;

void setup() {

  // CONFIGURAÇÕES DA BALANÇA
  Serial.begin(57600);
  balanca.begin(pinDTCelulaDeCarga, pinSCKCelulaDeCarga); // CONFIGURANDO PINOS DA BALANCA
  balanca.set_scale(escala);  // LIMPANDO VALOR DA ESCALA -> PRECISA DESCOBRIR ISSO MANUALMENTE
  delay(2000);
  balanca.tare();  //ZERANDO A BALANÇA PARA DESCONSIDERAR A MASSA DA ESTRUTURA
  
  //CONFIGURAÇÃO DO SERVO PARA O OBJETO SABER EM QUAL PINO ELE ESTÁ LIGADO
  servo.attach(pinServo);

  // CONFIGURAÇÕES DO DS3231
  Wire.begin();
  clock.begin();
  delay(500);
  // Descomentar as linhas e ajustar a data e hora corretas na primeira vez que formos rodar o código
  /*
  clock.setYear(23);  // último dois dígitos do ano (00 a 99)
  clock.setMonth(6); // mês (1 a 12)
  clock.setDate(24); // data (1 a 31)
  clock.setDoW(5);   // dia da semana (1 a 7)
  clock.setHour(12); // hora no formato de 24 horas (0 a 23)
  clock.setMinute(0); // minutos (0 a 59)
  clock.setSecond(0); // segundos (0 a 59)
  */

  // VARIÁVEIS DE SAÍDA DIGITAL
  pinMode(bomba, OUTPUT);
  pinMode(motor, OUTPUT);

  // VARIÁVEIS DE ENTRADA DIGITAL
  pinMode(nivel_baixo, INPUT);
  pinMode(nivel_alto, INPUT);

}

void loop() {
  nivelBaixo = digitalRead(nivel_baixo);
  nivelAlto = digitalRead(nivel_alto);
  levelControl(nivel_baixo, nivel_alto);


  if(!enchendoRacao){
    for(i=0; i<4; i++){
      if(horaAtual==horas[i] && minutoAtual==minutos[i]){
        enchendoRacao = true;
      }
    }
  }
  
  if(enchendoRacao){
    digitalWrite(motor, HIGH);
    balanca.power_up();
    pesoMedido = balanca.get_units(5);

    if(pesoMedido >= pesoDesejado){
      digitalWrite(motor, LOW);
      servo.write(0);
      delay(1500);
      servo.write(90);
      enchendoRacao = false;
    }
  }
  // PRA CONTROLAR A RAÇÃO:
  //   SE O HORÁRIO ATUAL BATER COM O HORÁRIO DADO PELO SUPERVISÓRIO:
  //    JOGA UMA VARIÁVEL enchendoRacao PARA TRUE
  //   SE A VARIÁVEL enchendoRacao FOR TRUE:
  //      LIGA O MOTOR E MEDE O PESO NA CÉLULA DE CARGA
  //   SE O PESO FOR IGUAL OU MAIOR AO PESO RECEBIDO PELO SUPERVISÓRIO:
  //      JOGA A VARIÁVEL enchendoRacao PARA FALSE
  //      DESLIGA O MOTOR
  //      RODA O SERVO MOTOR PARA JOGAR A RAÇÃO NA BALANÇA
  //      DÁ UM DELAY ATÉ QUE TODA A RAÇÃO CAIA NA VASILHA DO CACHORRO
  //      VOLTA O SERVO MOTOR PARA A POSIÇÃO INICIAL


  /*
    DateTime now = clock.now();
    
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(' ');
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
  */
  /*
  int hour = clock.getHour(h12, PM); // Obtém a hora atual (formato 24 horas)
  int minute = clock.getMinute();     // Obtém os minutos atuais
  int second = clock.getSecond();     // Obtém os segundos atuais

  Serial.print("Hora atual: ");
  Serial.print(hour);
  Serial.print(":");
  Serial.print(minute);
  Serial.print(":");
  Serial.println(second);

  delay(1000); // Espere 1 segundo antes de atualizar a hora
  */
  // put your main code here, to run repeatedly:

}

void levelControl(int nivelBaixo, int nivelAlto){
  if(nivelAlto == HIGH){
    digitalWrite(bomba, LOW);
  }else if(nivelBaixo == LOW){
    digitalWrite(bomba, HIGH);
  }
}
