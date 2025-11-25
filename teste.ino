/*
 
  Calibrando com tampão pH 7 e 4:
    - Insira o valor obtido coma sonda no tampão de pH 7 em "calibracao_ph7", utilize ponto para separação no ligar da vírgula.
 
    - Insira o valor obtido coma sonda no tampão de pH 4 em "calibracao_ph4", utilize ponto para separação no ligar da vírgula.
 
    - Pronto, basta utilizar o sensor em soluções neutras e acídas.
 
 
  Calibrando com tampão pH 7 e 10:
    - Insira o valor obtido coma sonda no tampão de pH 7 em "calibracao_ph7", utilize ponto para separação no ligar da vírgula.
 
    - Insira o valor obtido coma sonda no tampão de pH 10 em "calibracao_ph10", utilize ponto para separação no ligar da vírgula.
    
    - Mude o valor da variavel "UTILIZAR_PH_10" de "false" para "true".
 
    - Pronto, basta utilizar o sensor em soluções neutras e alcalinas.
*/
#include <Wire.h>
// #include <LiquidCrystal.h>
// LiquidCrystal quimicativa(2,3,4,5,11,12);
#include <LiquidCrystal_I2C.h>
 
LiquidCrystal_I2C quimicativa(0x27, 16, 2);
 
 
float calibracao_ph7 = 4.3;   // Tensão obtida em solução de calibração pH 7
float calibracao_ph4 = 4.6;   // Tensão obtida em solução de calibração pH 4
float calibracao_ph10 = 3.9;  // Tensão obtida em solução de calibração pH 10
String solucao = "solucoes de ph.";
 
#define UTILIZAR_PH_10 false  // Habilita calibração entre pH 7 e 10, \
                                 caso contrário utiliza pH 7 e 4.

int ma7 = 7;
int me7 = 8;
bool ph10 = false;

float m;
float b;
int buf[10];
 
void setup() {
  Serial.begin(9600);
 
  if (calibracao_ph7 == 0 && calibracao_ph4 == 0 && calibracao_ph10 == 0) {
    delay(500);
    Serial.println();
    Serial.println("Erro - Necessário colocar os valores de calibração no código!");
    while (1)
      ;
  }
 
  if (!UTILIZAR_PH_10 && calibracao_ph4 == 0 && calibracao_ph10 != 0 && calibracao_ph7 != 0) {
    delay(500);
    Serial.println();
    Serial.println("Erro - Você não marcou a opção UTILIZAR_PH_10 como true para utilizar pH 7 e 10.");
    while (1)
      ;
  }
  quimicativa.init();
  quimicativa.begin(16, 2);    
  quimicativa.backlight();        // Inicializa todos os caracteres
  quimicativa.setCursor(0, 0);         // Posiciona o cursor na posição
  quimicativa.print("Quimicativa");      // Escreve no monitor serial
  quimicativa.setCursor(0, 1);         // Posiciona o cursor na posição
  quimicativa.print("Leitura de pH");  // Escreve no monitor serial
  delay(2000);
  quimicativa.clear();  // Limpa o conteúdo do display


  pinMode(ma7, INPUT);
  pinMode(me7, INPUT);
}
 
 
void loop() {

  if(digitalRead(ma7) == HIGH){
    ph10 = true;
  }
  if(digitalRead(me7) == HIGH){
    ph10 = false;
  }
  
  if (ph10) {
    m = (7.0 - 10.0) / (calibracao_ph7 - calibracao_ph10);
    b = 10.0 - m * calibracao_ph10;
  } else {
    m = (4.0 - 7.0) / (calibracao_ph4 - calibracao_ph7);
    b = 7.0 - m * calibracao_ph7;
  }

  for (int i = 0; i < 10; i++) {  // 11 amostras
    buf[i] = analogRead(A0);      // Ler o sensor PH
    delay(10);
  }
 
  for (int i = 0; i < 9; i++) {  // Ordena em ordem crescente
    for (int j = i + 1; j < 10; j++) {
      if (buf[i] > buf[j]) {
        int temp = buf[i];
        buf[i] = buf[j];
        buf[j] = temp;
      }
    }
  }
 
  int valorMedio = 0;
  for (int i = 2; i < 8; i++) {  // Realiza o valor médio utilizando 6 amostras
    valorMedio += buf[i];
  }
 
  float tensao = (valorMedio * 5.0) / 1024.0 / 6;  // Realiza a média e \
                                                    transforma o valor \
                                                    analógico em volt
  float ph = m * tensao + b; // Converte para pH
  if(ph == 7) {
    solucao = "Solucao Neutra";
  }else if(ph < 7){
    solucao = "Solucao Acida";
  }else{
    solucao = "Solucao Basica";
  }
  Serial.println("Valor pH: ");  // Escreve no display
  Serial.println(ph);
  Serial.println(solucao);
 
  if(ph10){
    quimicativa.setCursor(15,0);
    quimicativa.print(0);
  }else{
    quimicativa.setCursor(15,0);
    quimicativa.print(1);
  }
  quimicativa.setCursor(0, 0);      // Posiciona o cursor no display
  quimicativa.print("Valor pH: ");  // Escreve no display
  quimicativa.setCursor(11, 0);     // Posiciona o cursor no display
  quimicativa.print(ph, 1);         // Escreve o pH com uma casa decimal
  quimicativa.setCursor(0, 1);
  quimicativa.print(solucao);
  delay(100);      // Aguarda para próxima leitura
}