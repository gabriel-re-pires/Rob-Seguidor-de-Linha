# Guia de Montagem e Ligação — Carrinho Seguidor de Linha (ESP32)

Este guia descreve a montagem do **carrinho seguidor de linha na versão
melhorada com ESP32**, controle Xbox sem fio, farol de LED e bateria de
íon-lítio recarregável. Ele complementa a apostila original da aula
(`robotica_carrinho_seguidor.pdf`), mas a **pinagem e a eletrônica aqui são as
desta versão** — diferentes do kit padrão com Arduino Uno.

> ⚠️ **Antes de tudo:** mantenha a alimentação **desligada** e o cabo USB
> **desconectado** durante toda a fiação. Confira a polaridade duas vezes —
> inverter VCC/GND queima o L298N ou o ESP32.

---

## 1. Lista de componentes

| Componente | Qtd. | Função |
|------------|:----:|--------|
| ESP32 DevKit | 1 | Microcontrolador (cérebro) |
| Ponte H L298N | 1 | Driver de potência dos motores |
| Motor DC com redutor + roda amarela | 2 | Tração traseira |
| Rodízio (roda boba dianteira) | 1 | Apoio frontal |
| Sensor infravermelho TCRT5000 | 2 | Detecção da linha preta |
| Célula Li-ion 3,7 V (reaproveitada de vape) | 2 | Alimentação |
| Módulo carregador TP4056 | 2 | Recarga das células |
| LED(s) para farol + resistor | 1+ | Iluminação frontal |
| Chave liga/desliga | 1 | Corte de energia |
| Cases impressas em 3D (ESP32 + bateria) | — | Estrutura/fixação |
| Jumpers, parafusos M3, espaçadores | — | Conexões e fixação |

**Ferramentas:** chave Phillips pequena, chave de fenda (bornes do L298N),
alicate de bico fino, ferro de solda (para os sensores/bateria), multímetro.

---

## 2. Montagem mecânica

A montagem mecânica segue a mesma ideia da apostila da aula:

1. **Motores** — fixe os dois motores DC nas laterais traseiras do chassi, com
   o eixo apontando para fora. Aperte os parafusos M3 sem forçar (o chassi pode
   rachar). Deixe os fios saindo para o interior do chassi.
2. **Rodízio dianteiro** — parafuse a roda boba no furo frontal. Ela deve girar
   livre em qualquer direção.
3. **Rodas** — encaixe as rodas amarelas direto no eixo dos motores (o chanfro
   trava a roda). O carrinho deve ficar nivelado nas 3 rodas.
4. **Sensores IR na frente** — fixe os dois TCRT5000 na dianteira, voltados para
   o chão, a **1–2 cm** de altura. Sensor **esquerdo** à esquerda da linha,
   **direito** à direita, espaçados ~2–3 cm.
5. **Cases 3D** — fixe a case do **ESP32** e a case da **bateria** no chassi
   (arquivos em `Print3D/`). Elas organizam a eletrônica e protegem as células.

---

## 3. Alimentação — bateria Li-ion + 2× TP4056

Esta versão **não usa pilhas AA**. A alimentação é feita por **duas células
Li-ion de 3,7 V** (reaproveitadas de um vape *Ignite* usado), ligadas em
**série** para fornecer ≈ **7,4 V** aos motores e ao regulador do L298N.

A recarga é feita por **dois módulos TP4056**, um para cada célula.

```
        +7,4V (para VCC do L298N)
          ▲
          │
   [ Célula B 3,7V ] ── TP4056 (B)
          │
          ├──────── ponto central (3,7V)
          │
   [ Célula A 3,7V ] ── TP4056 (A)
          │
          ▼
         GND (para GND do L298N)
```

> ⚠️ **Cuidado com Li-ion em série:** o TP4056 é um carregador de **1 célula**.
> Ao ligar células em série, carregue/monitore cada célula com seu próprio
> TP4056 e evite descarregar abaixo de ~3,0 V por célula. Células de vape não
> têm BMS — manuseie com atenção (curto/sobreaquecimento são perigosos).
> Idealmente, use células com tensões e capacidades parecidas.

| Ligação | Origem | Destino |
|---------|--------|---------|
| +7,4 V (positivo da série) | terminal + da bateria | **VCC** do L298N |
| GND | terminal − da bateria | **GND** do L298N |
| Recarga | USB do TP4056 | uma célula cada |

> Mantenha o jumper **J1** do L298N **instalado** (entrada < 12 V) — assim o
> regulador interno de 5 V do L298N fica ativo para alimentar o ESP32.

---

## 4. Ligação do driver L298N

### 4.1 Motores e alimentação

| Borne / Pino do L298N | Conectar em | Observação |
|-----------------------|-------------|------------|
| **OUT1 / OUT2** | Motor **Esquerdo** | Se girar ao contrário, inverta os 2 fios |
| **OUT3 / OUT4** | Motor **Direito** | Idem |
| **VCC** | +7,4 V da bateria | Alimentação dos motores |
| **GND** | GND da bateria **e** GND do ESP32 | **GND comum é obrigatório** |
| **+5V** (saída regulada) | Pino **5V / VIN** do ESP32 | Alimenta o ESP32 pela bateria (J1 instalado) |

### 4.2 Jumpers ENA / ENB — **mantenha-os instalados**

Nesta versão o ESP32 **não controla ENA/ENB**. Os jumpers de *enable* ficam
**instalados** (canais sempre habilitados) e o **PWM é aplicado diretamente nos
pinos IN1–IN4**. Ou seja, **velocidade e direção** dos motores saem todas dos
pinos IN.

### 4.3 Controle (ESP32 → L298N)

| GPIO do ESP32 | Pino do L298N | Função |
|:-------------:|:-------------:|--------|
| **27** | IN1 | Motor Esquerdo — PWM (sentido + velocidade) |
| **26** | IN2 | Motor Esquerdo — PWM (sentido + velocidade) |
| **25** | IN3 | Motor Direito — PWM (sentido + velocidade) |
| **33** | IN4 | Motor Direito — PWM (sentido + velocidade) |

**Como o código aciona cada motor (`acionarMotores`):**

| Movimento da roda | Pino A | Pino B |
|-------------------|:------:|:------:|
| Frente | 0 | PWM (velocidade) |
| Ré | PWM (velocidade) | 0 |
| Parar (freio) | 255 | 255 |

> Os dois pinos em 255 colocam o canal em **freio** (ambos em nível alto),
> segurando a roda parada.

---

## 5. Sensores infravermelhos TCRT5000

| Pino do sensor | Conectar em | Observação |
|----------------|-------------|------------|
| VCC | 3V3 (ou 5V) do ESP32 | Alimentação |
| GND | GND comum | Mesmo barramento do L298N/ESP32 |
| OUT — sensor **Esquerdo** | GPIO **34** | *Input-only* no ESP32 |
| OUT — sensor **Direito** | GPIO **35** | *Input-only* no ESP32 |

**Convenção desta versão (importante — diferente da apostila):**

> **HIGH = sensor sobre a LINHA PRETA** · **LOW = sensor sobre o chão claro**

Cada TCRT5000 tem um **trimpot** (potenciômetro azul) para ajustar a
sensibilidade. Calibração:

1. Sobre o **chão claro**, ambos os sensores devem ler **LOW**.
2. Sobre a **linha preta**, o sensor deve ler **HIGH**.
3. Gire o trimpot até obter essa resposta; use o LED indicador do módulo como
   guia visual.
4. Altura ideal: **5 mm a 15 mm** do chão.

> ℹ️ Se o comportamento ficar invertido (carrinho foge da linha), confira esta
> convenção HIGH/LOW e/ou os trimpots antes de mexer no código.

---

## 6. Farol de LED

| Ligação | Destino |
|---------|---------|
| Anodo do(s) LED(s) via resistor | GPIO **4** do ESP32 |
| Catodo | GND comum |

O farol liga/desliga pelo **botão X** do controle Xbox (funciona nos dois modos).
Use um resistor adequado (ex.: 220–330 Ω) em série com o LED.

---

## 7. Controle Xbox (modo manual)

O modo manual usa um **joystick Xbox Series X** por Bluetooth, via biblioteca
`XboxSeriesXControllerESP32_asukiaaa`. É preciso informar o **MAC Address** do
controle no código:

```cpp
XboxSeriesXControllerESP32_asukiaaa::Core xboxController("Coloque_o_MAC_Qui");
```

Comandos:

| Comando do controle | Ação |
|---------------------|------|
| Botão **Y** | Alterna **Manual ⇄ Autônomo** |
| Botão **X** | Liga/desliga o **farol** |
| Gatilho **RT** | Acelera para frente |
| Gatilho **LT** | Ré / freio |
| Analógico esquerdo (eixo X) | Esterçamento (vira) |

Se o controle desconectar no modo manual, o carrinho volta sozinho para o modo
**Seguir Linha**.

---

## 8. Checklist final antes de ligar

- [ ] Motores nos bornes OUT1/OUT2 (esq.) e OUT3/OUT4 (dir.).
- [ ] Bateria: +7,4 V → VCC do L298N · GND → GND do L298N.
- [ ] **GND comum** entre bateria, L298N e ESP32.
- [ ] 5V do L298N → pino 5V/VIN do ESP32 (jumper J1 instalado).
- [ ] Jumpers ENA/ENB do L298N **instalados**.
- [ ] GPIO 27, 26, 25, 33 → IN1, IN2, IN3, IN4.
- [ ] Sensor esq. OUT → GPIO 34 · sensor dir. OUT → GPIO 35 (VCC/GND ligados).
- [ ] Farol no GPIO 4 com resistor.
- [ ] Sensores calibrados (LINHA = HIGH, CHÃO = LOW).
- [ ] Nenhum fio solto, nenhum borne mal apertado.
- [ ] MAC do controle preenchido no código.

Tudo conferido? **Ligue, posicione o carrinho na pista e divirta-se.** 🏁
