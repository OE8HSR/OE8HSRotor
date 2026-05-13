# OE8HSRotor – 3D‑gedruckter Antennen‑Rotor für kleine Richtantennen

OE8HSRotor ist ein kompakter, weitgehend 3D‑gedruckter Azimut/Elevation‑Antennenrotor für kleine Satelliten‑ und UKW‑Richtantennen.  
Der Rotor ist leicht, mobil, auf einem Stativ montierbar und wird von einem Raspberry Pi und einem Arduino‑basierenden CNC‑Shield mit zwei NEMA17‑Schrittmotoren angesteuert. [file:52][file:46]

> **Hinweis:** Das Projekt befindet sich im Beta‑Status. Der Rotor kann bei Fehlbedienung oder Fehlern in der Steuerung Schäden an Antennen, Funkgeräten oder Personen verursachen. Immer mit gesundem Menschenverstand arbeiten. [file:46][file:52]

---

## Projektüberblick

![Steuerbox – Innenansicht](OE8HSR-Elektronik.jpg)  
*Steuerbox mit Raspberry Pi, Arduino‑CNC‑Shield und 24 V‑Versorgung in einer Stahlbox.* [file:49][file:52]

OE8HSRotor wurde entwickelt, um portable Satelliten‑Funkstationen zu vereinfachen:  
Statt Antennen manuell nachzuführen, richtet der Rotor die Antenne automatisch nach Azimut und Elevation aus, gesteuert durch Tracking‑Software (z. B. Gpredict) auf dem Raspberry Pi. [file:52][file:46]

**Hauptmerkmale**

- Zweiachsiger Rotor (Azimut + Elevation) mit 3D‑gedruckten Getriebestufen. [file:46][file:52]  
- Antrieb über zwei NEMA17‑Schrittmotoren mit CNC‑Shield und A4988‑Treibern. [file:46][file:50]  
- Steuerung über Raspberry Pi (WiFi, VNC, Gpredict + hamlib) und Arduino als Motorkontroller. [file:52][file:46]  
- Betrieb aus 12–24 V‑Versorgung; interne DC/DC‑Wandlung auf 5 V. [file:52][file:50]  
- Mechanik überwiegend 3D‑gedruckt, nur Standardlager und Schrauben als Zukaufteile. [file:46][file:52]  
- Elektronik in geschirmter Stahlbox zur Reduktion von HF‑Störungen. [file:52][file:50]

---

## Systemarchitektur

### Mechanik

Der Rotor besteht aus mehreren Baugruppen (Build Groups), die im englischen Manual detailliert beschrieben werden. [file:46][file:52]

- **Elevation Stage (BG1):** Abtriebwelle mit Z60‑Zahnrad, gelagert in 6006‑Kugellagern. [file:46][file:48]  
- **Azimuth Stage (BG2):** Drehteil mit Innenlagerung und 1:10‑Übersetzung für ruhigen Lauf und hohes Drehmoment. [file:46][file:48]  
- **Redirecting Stage (BG3):** Umlenkrad Z60 mit 6000‑Lager und M10‑Schraube zur Umlenkung der Kraft. [file:46][file:48]  
- **Bottom Part (BG8):** Innenverzahnung mit 6006‑ und 51114‑Lager, dient als untere Hauptlagerung. [file:46][file:48]  
- **Side Extensions (BG6):** Seitliche Verlängerungen mit M8‑Gewindestange zur Antennenmontage. [file:46][file:52]  

Die vollständige mechanische Beschreibung und die Schritt‑für‑Schritt‑Montage finden sich im englischen Aufbau‑Manual:  
👉 [`OE8HSRotor01.pdf`](OE8HSRotor01.pdf) [file:46]

---

### Elektronik

![Elektronik-Skizze](image1-2.jpg)  
*Prinzipielle Verschaltung von 24 V‑Versorgung, DC/DC‑Wandler, Raspberry Pi, Arduino und Schrittmotoren.* [file:47][file:52]

Die Steuerungselektronik besteht aus:

- **Raspberry Pi 2/3** mit WiFi‑Stick, Gpredict, VNC‑Server und hamlib. [file:52][file:50]  
- **Arduino (Uno‑Clone)** mit CNC‑Shield und A4988‑Treibern für zwei NEMA17‑Motoren. [file:50][file:46]  
- **24 V‑Netzteil** und DC/DC‑Abwärtswandler auf 5 V für Raspberry Pi und Logik. [file:50][file:52]  
- **Stahl‑Steuerbox** (z. B. Rittal KL 1514.510) als HF‑Schirmung. [file:52][file:50]  
- **Bedienelemente**:  
  - Ein/Aus‑Schalter für 24 V  
  - Taster zum Aktivieren/Deaktivieren des Rotors  
  - Taster für sicheren Shutdown des Raspberry Pi  
  - zweifarbige Status‑LED (Rot/Grün) [file:50][file:52]  

Die detaillierte elektrische Stückliste befindet sich in:  
👉 [`St-ckliste-EL-1.txt`](St-ckliste-EL-1.txt)  
👉 [`Mechanical_Partslist.txt`](Mechanical_Partslist.txt) [file:50][file:48]

---

### Software-Flow

![Software-Flowgraph](image2.jpg)  
*Software‑Datenfluss zwischen Bediengerät, Raspberry Pi, hamlib und Arduino.* [file:54][file:52]

- Bedienung per **PC/Tablet/Smartphone** via VNC auf den Raspberry Pi. [file:52]  
- Auf dem Pi läuft **Gpredict** als Tracking‑Software sowie **hamlib** für das Rotor‑Protokoll. [file:52][file:46]  
- Der Pi sendet die Soll‑Azimut/Elevationsdaten seriell (USB) an den Arduino. [file:52]  
- Der Arduino setzt die kommandierten Winkel in Schritt‑Sequenzen für die beiden NEMA17‑Motoren um. [file:52]  

---

## Verzeichnisstruktur (empfohlen)

> Die tatsächliche Struktur in deinem Repo kann leicht abweichen – passe die Pfade ggf. an.

```text
OE8HSRotor/
├─ firmware/
│  ├─ arduino/
│  │  ├─ src/
│  │  │  └─ main.ino
│  │  └─ README.md
│  └─ raspberrypi/
│     ├─ gpredict/
│     └─ scripts/
├─ docs/
│  ├─ OE8HSRotor01.pdf          # englisches Aufbau-Manual
│  ├─ Beschreibung-Deutsch.pdf  # deutsche Projektbeschreibung
│  ├─ St-ckliste-EL-1.txt       # elektrische Stückliste
│  ├─ Mechanical_Partslist.txt  # mechanische Stückliste
│  ├─ Partslist_Printed-1.txt   # gedruckte Teile
│  └─ images/
│     ├─ OE8HSR-Elektronik.jpg
│     ├─ image1-2.jpg
│     ├─ image1.jpg
│     └─ image2.jpg
├─ stl/
│  └─ *.stl
└─ README.md
