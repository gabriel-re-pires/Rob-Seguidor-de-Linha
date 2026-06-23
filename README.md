# 🏎️ Carrinho Seguidor de Linha — Versão ESP32

Robô móvel autônomo que segue uma linha preta no chão usando sensores
infravermelhos, com um **modo manual extra** controlado por um joystick de
**Xbox Series X** via Bluetooth (estilo *Forza*: gatilhos aceleram/freiam e o
analógico esquerdo esterça).

Este projeto nasceu da atividade *Hands-On de Robótica* da disciplina de
**Robótica Aplicada** (Engenharia de Computação — Prof. Milton Miranda Neto).
O kit padrão da aula usava **Arduino Uno** e pilhas AA; esta é uma **versão
melhorada e repaginada** do carrinho, com **ESP32**, controle sem fio, farol de
LED e bateria de íon-lítio recarregável.

<p align="center">
  <img src="Imagens/01.webp" alt="Carrinho seguidor de linha - vista 1" width="45%">
  <img src="Imagens/02.webp" alt="Carrinho seguidor de linha - vista 2" width="45%">
</p>

---

## ✨ O que mudou em relação ao modelo da aula

| Item | Kit padrão da aula | **Esta versão** |
|------|--------------------|-----------------|
| Cérebro | Arduino Uno (5 V) | **ESP32** (3,3 V, Wi-Fi + Bluetooth) |
| Alimentação | 4× pilhas AA (6 V) | **2× células 3,7 V Li-ion** (reaproveitadas de um *vape* descartado) em série ≈ 7,4 V |
| Carregamento | Trocar pilhas | **2× módulos TP4056** ligados em série (recarga via USB) |
| Controle manual | — | **Joystick Xbox Series X** por Bluetooth (modo *Forza*) |
| Driver dos motores | L298N usando ENA/ENB | **L298N sem ENA/ENB** — o PWM vai direto nos pinos IN |
| Extras | — | **Farol de LED** liga/desliga pelo controle |
| Estrutura | Chassi do kit | Chassi + **cases impressas em 3D** (ESP32 e bateria) |

---

## 🧠 Como funciona

O firmware tem **dois modos**, alternados pelo botão **Y** do controle:

### 1. Modo Autônomo — Seguir Linha (padrão)
Lê os dois sensores TCRT5000 e decide o movimento. Aqui a convenção é:

> **HIGH = sensor sobre a linha preta** · **LOW = sensor sobre o chão claro**

| Sensor Esq. | Sensor Dir. | Situação | Ação |
|:-----------:|:-----------:|----------|------|
| HIGH | HIGH | Ambos na linha → **marca de parada** | Para, dá uma ré curtinha e fica parado 3 s |
| LOW  | HIGH | Linha só à direita | Gira para a **direita** |
| HIGH | LOW  | Linha só à esquerda | Gira para a **esquerda** |
| LOW  | LOW  | Nenhum na linha | Segue **em frente** |

É um controle **On/Off** (liga-desliga), simples e robusto. As velocidades são
ajustáveis no topo da função `seguirLinha()`:

```cpp
int velEsqReta = 80;   // velocidade da roda esquerda na reta
int velDirReta = 95;   // velocidade da roda direita na reta
int velGiro    = 100;  // velocidade ao virar
```

> 💡 As rodas têm velocidades de reta diferentes (80 vs 95) para **compensar**
> pequenas diferenças mecânicas entre os dois motores e o carrinho andar reto.

### 2. Modo Manual — Controle Xbox (estilo Forza)
Pressione **Y** para assumir o controle pelo joystick:

- **Gatilho RT** → acelera para frente
- **Gatilho LT** → ré / freio
- **Analógico esquerdo (eixo X)** → esterçamento (vira para os lados)
- **Botão X** → liga/desliga o **farol** de LED (funciona nos dois modos)
- **Botão Y** → alterna entre Manual e Autônomo

A mistura de aceleração + direção é feita por *differential drive*:

```cpp
int velEsq = aceleracaoTotal - direcao;
int velDir = aceleracaoTotal + direcao;
```

> 🔌 **Segurança:** se o controle desconectar durante o modo manual, o carrinho
> volta automaticamente para o modo **Seguir Linha**.

---

## 🛠️ Hardware

| Componente | Qtd. | Função |
|------------|:----:|--------|
| ESP32 DevKit | 1 | Microcontrolador (cérebro) |
| Ponte H L298N | 1 | Driver de potência dos motores |
| Motor DC com redutor + roda | 2 | Tração traseira |
| Rodízio (roda boba) | 1 | Apoio dianteiro |
| Sensor TCRT5000 (IR) | 2 | Detecção da linha |
| Célula Li-ion 3,7 V | 2 | Alimentação (em série ≈ 7,4 V) |
| Módulo carregador TP4056 | 2 | Recarga das células (em série) |
| LEDs (farol) | 1+ | Iluminação frontal |
| Chassi + cases 3D | — | Estrutura |

### 📍 Pinagem (ESP32)

| Sinal | GPIO | Observação |
|-------|:----:|-----------|
| L298N **IN1** (Motor Esq.) | 27 | PWM de direção/velocidade |
| L298N **IN2** (Motor Esq.) | 26 | PWM de direção/velocidade |
| L298N **IN3** (Motor Dir.) | 25 | PWM de direção/velocidade |
| L298N **IN4** (Motor Dir.) | 33 | PWM de direção/velocidade |
| Sensor IR **Esquerdo** (OUT) | 34 | *Input-only* no ESP32 |
| Sensor IR **Direito** (OUT) | 35 | *Input-only* no ESP32 |
| **Farol** (LEDs) | 4 | Saída digital |

> ⚙️ Como o **ENA/ENB não são usados**, os jumpers de *enable* do L298N ficam
> **instalados** (canais sempre habilitados) e tanto a **velocidade** quanto o
> **sentido** dos motores são controlados pelo PWM nos pinos **IN1–IN4**.

📄 O passo a passo completo de montagem e ligação está em
**[`Montagem/`](Montagem/)**.

---

## 📂 Estrutura do repositório

```
Carrinho_Segue_Linha/
├── README.md                       ← este arquivo
├── seguelinha/
│   └── seguelinha.ino              ← firmware do ESP32 (código principal)
├── Montagem/
│   ├── Montagem_Carrinho_ESP32.pdf ← guia de montagem e ligação desta versão
│   ├── MONTAGEM.md                 ← fonte do guia (Markdown)
│   └── robotica_carrinho_seguidor.pdf ← apostila original da aula
├── Imagens/
│   ├── 01.webp
│   └── 02.webp
└── Print3D/                        ← cases impressas em 3D (ESP32 e bateria)
    ├── CaseEsp32.stl / CaseEsp32Tampa.stl / CaseBat.stl
    └── *.gcode                     ← fatiados para Creality Ender 3 V3 SE
```

---

## 💾 Como compilar e gravar

1. Instale o **Arduino IDE** e o suporte às placas **ESP32**
   (`Boards Manager` → *esp32 by Espressif Systems*).
2. Instale a biblioteca **`XboxSeriesXControllerESP32_asukiaaa`**
   (pelo *Library Manager*).
3. Abra `seguelinha/seguelinha.ino`.
4. Descubra o **MAC Address** do seu controle Xbox e coloque na linha:
   ```cpp
   XboxSeriesXControllerESP32_asukiaaa::Core xboxController("Coloque_o_MAC_Qui");
   ```
5. Selecione a placa **ESP32 Dev Module**, a porta COM correta e **carregue**.
6. *(Opcional)* Abra o **Serial Monitor** a **115200 baud** para depurar.

---

## 🖨️ Impressão 3D

A pasta **[`Print3D/`](Print3D/)** contém as cases impressas em 3D do carrinho
(suporte do ESP32, tampa e suporte da bateria), tanto em **`.stl`** (modelo)
quanto em **`.gcode`** já fatiado para a impressora **Creality Ender 3 V3 SE**.

---

## 🏁 Créditos

- Atividade base: *Hands-On de Robótica — Carrinho Seguidor de Linha*,
  disciplina de Robótica Aplicada (Eng. de Computação), Prof. Milton Miranda Neto.
- Modificações, firmware ESP32, controle Xbox, farol, bateria Li-ion e
  cases 3D: versão melhorada desenvolvida pelo autor deste repositório.
