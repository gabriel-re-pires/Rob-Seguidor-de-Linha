#include <XboxSeriesXControllerESP32_asukiaaa.hpp>

// O MAC Address do teu Xbox Wireless Controller
XboxSeriesXControllerESP32_asukiaaa::Core xboxController("Coloque_o_MAC_Qui");

// --- Pinos da Ponte H L298N (SEM ENA E ENB) ---
const int pinoIN1 = 27;
const int pinoIN2 = 26;
const int pinoIN3 = 25;
const int pinoIN4 = 33;

// --- Pinos dos Sensores TCRT5000 ---
const int pinoSensorEsq = 34;
const int pinoSensorDir = 35;

// --- Pino dos filamentos de LEDs ---
const int pinoFarol = 4;

// --- Variáveis de Estado ---
bool modoManual = false;
bool estadoBotaoYAnterior = false;
bool estadoFarol = false;
bool estadoBotaoXAnterior = false;

void setup() {
  Serial.begin(115200);
  pinMode(pinoIN1, OUTPUT);
  pinMode(pinoIN2, OUTPUT);
  pinMode(pinoIN3, OUTPUT);
  pinMode(pinoIN4, OUTPUT);
  pinMode(pinoSensorEsq, INPUT);
  pinMode(pinoSensorDir, INPUT);
  pinMode(pinoFarol, OUTPUT);
  digitalWrite(pinoFarol, LOW);
  pararMotores();
  xboxController.begin();
}

void loop() {
  xboxController.onLoop();
  if (xboxController.isConnected()) {
    // --- LÓGICA DO BOTÃO Y (Alternar Modos) ---
    bool apertouY = xboxController.xboxNotif.btnY;
    if (apertouY && !estadoBotaoYAnterior) {
      modoManual = !modoManual;
      pararMotores();
    }
    estadoBotaoYAnterior = apertouY;

    // --- LÓGICA DO BOTÃO X (Ligar/Desligar Farol) ---
    bool apertouX = xboxController.xboxNotif.btnX;
    if (apertouX && !estadoBotaoXAnterior) {
      estadoFarol = !estadoFarol;
      digitalWrite(pinoFarol, estadoFarol ? HIGH : LOW);
    }
    estadoBotaoXAnterior = apertouX;

    // --- EXECUÇÃO DOS MODOS ---
    if (modoManual) {
      controleForzaXbox();
    } else {
      seguirLinha();
    }
  } else {
    if (modoManual == true) {
      modoManual = false;
      pararMotores();
    }
    seguirLinha();
  }
}

// --- LÓGICA DO CONTROLE XBOX (ESTILO FORZA) ---
void controleForzaXbox() {
  uint16_t gatilhoRT = xboxController.xboxNotif.trigRT;
  uint16_t gatilhoLT = xboxController.xboxNotif.trigLT;
  uint16_t eixoX_raw = xboxController.xboxNotif.joyLHori;

  int forcaFrente = map(gatilhoRT, 0, 1023, 0, 255);
  int forcaRe = map(gatilhoLT, 0, 1023, 0, 255);
  int aceleracaoTotal = forcaFrente - forcaRe;
  int direcao = map(eixoX_raw, 0, 65535, -255, 255);
  if (abs(direcao) < 50) {
    direcao = 0;
  }
  int velEsq = aceleracaoTotal - direcao;
  int velDir = aceleracaoTotal + direcao;
  velEsq = constrain(velEsq, -255, 255);
  velDir = constrain(velDir, -255, 255);
  acionarMotores(velEsq, velDir);
}

// Convenção: HIGH = sensor está vendo a linha preta / LOW = chão.
void seguirLinha() {
  int valE = digitalRead(pinoSensorEsq);
  int valD = digitalRead(pinoSensorDir);

  // ======================================================================
  // TUNING
  // ======================================================================
  int velEsqReta = 80;   // velocidade da roda esquerda na reta
  int velDirReta = 95;   // velocidade da roda direita na reta
  int velGiro    = 100;  // velocidade ao virar
  // ======================================================================

  // CASO 1: os DOIS sensores na linha -> marca de PARADA.
  if (valE == HIGH && valD == HIGH) {
    acionarMotores(0, 0);
    delay(300);
    acionarMotores(-velEsqReta, -velDirReta);   // ré curtinha
    delay(150);
    acionarMotores(0, 0);
    delay(3000);
  }
  // CASO 2: só o sensor DIREITO na linha -> vira para a DIREITA.
  else if (valD == HIGH) {
    acionarMotores(velGiro, -velGiro);
  }
  // CASO 3: só o sensor ESQUERDO na linha -> vira para a ESQUERDA.
  else if (valE == HIGH) {
    acionarMotores(-velGiro, velGiro);
  }
  // CASO 4: nenhum sensor na linha -> segue em frente.
  else {
    acionarMotores(velEsqReta, velDirReta);
  }
}

// --- FUNÇÃO MESTRE DOS MOTORES ---
void acionarMotores(int velEsq, int velDir) {
  // Roda Esquerda
  if (velEsq > 0) {
    analogWrite(pinoIN1, 0);
    analogWrite(pinoIN2, abs(velEsq));
  } else if (velEsq < 0) {
    analogWrite(pinoIN1, abs(velEsq));
    analogWrite(pinoIN2, 0);
  } else {
    analogWrite(pinoIN1, 255);
    analogWrite(pinoIN2, 255);
  }
  // Roda Direita
  if (velDir > 0) {
    analogWrite(pinoIN3, abs(velDir));
    analogWrite(pinoIN4, 0);
  } else if (velDir < 0) {
    analogWrite(pinoIN3, 0);
    analogWrite(pinoIN4, abs(velDir));
  } else {
    analogWrite(pinoIN3, 255);
    analogWrite(pinoIN4, 255);
  }
}

void pararMotores() {
  acionarMotores(0, 0);
}