# OE8HSRotor

Kleiner **Azimut- und Elevations-Antennenrotor**, überwiegend **3D-gedruckt**, mit Schrittmotorantrieb und Arduino-Firmware. Das Projekt entstand als Eigenbau für den Amateurfunk; Steuerung erfolgt seriell (EasyComm-ähnliches Protokoll).

**Upstream-Repository:** [OE8HSR/OE8HSRotor auf GitHub](https://github.com/OE8HSR/OE8HSRotor)

---

## Überblick

- **Zwei Achsen:** Azimut (AZ) und Elevation (EL), jeweils mit NEMA-17-Schrittmotor und Getriebeübersetzung (in der Firmware als `RATIOAZ` / `RATIOEL` hinterlegt).
- **Nullstellung:** mechanischer Referenzschalter (`ZEROSW`); beim Start wird darauf referenziert.
- **Serielle Schnittstelle:** 19200 Baud; Befehle für Zielwinkel, Positionsabfrage und Reset (Details in den Kommentaren der `.ino`-Dateien).
- **Firmware-Basis:** inspiriert von früher [SatNogs](https://satnogs.org)-Arduino-Firmware (Hinweis im Quellcode der Originalvariante).

**Sicherheit:** Rotierende Teile und automatische Bewegung können Verletzungen oder Beschädigungen verursachen. Nutzung auf **eigenes Risiko**; Finger und teure Hardware von beweglichen Teilen fernhalten.

---

## Repository-Inhalt

| Bereich | Beschreibung |
|--------|----------------|
| [**STL/**](STL/) | Druckbare `.STL`-Teile (Getriebe, Gehäuse, Motorhalter, Adapter für verschiedene Antennen). |
| [**Firmware/**](Firmware/) | Arduino-Sketches für die Steuerung (siehe unten). |
| [**Dokumentation/**](Dokumentation/) | Stücklisten, Bauanleitung als PDF, Fotos. |

### Firmware

| Datei | Zweck |
|-------|--------|
| [`Firmware/OE8HSRotorMK-1.ino`](Firmware/OE8HSRotorMK-1.ino) | **Original-Firmware** (MK-1): serielle Befehle für AZ/EL, kombinierte Positionsabfrage `AZ EL`. Benötigt u. a. die Arduino-Bibliothek **AccelStepper**. |
| [`Firmware/OE8HSRotorMK-1-gpredict-hamlib/OE8HSRotorMK-1-gpredict-hamlib.ino`](Firmware/OE8HSRotorMK-1-gpredict-hamlib/OE8HSRotorMK-1-gpredict-hamlib.ino) | **Variante für gpredict / Hamlib (`rotctld`):** Antworten auf einzelne `AZ`- und `EL`-Positionsabfragen (EasyComm II), Zeilenenden `\r`/`\n`, damit Tracking mit **gpredict** zuverlässiger funktioniert. Im Dateikopf: Beispielaufruf für `rotctld`. |

Abhängigkeiten in den Sketches: `AccelStepper`, `EEPROM` (letzteres wird im aktuellen Stand der MK-1-Dateien eingebunden; bei Bedarf für zukünftige Kalibrierung o. Ä.).

### Dokumentation (Auswahl)

| Datei | Inhalt |
|-------|--------|
| [`Dokumentation/Partslist_Printed-1.txt`](Dokumentation/Partslist_Printed-1.txt) | Liste der **gedruckten** Teile inkl. optionaler Adapter (z. B. ELK, Arrow). |
| [`Dokumentation/Mechanical_Partslist.txt`](Dokumentation/Mechanical_Partslist.txt) | **Mechanische** Kleinteile (Schrauben, Lager, Gewindestangen, …). |
| [`Dokumentation/Stückliste-EL-1.txt`](Dokumentation/Stückliste-EL-1.txt) | **Elektronik**-Stückliste mit Bezugsquellen (Stand der Datei; Links/Händler nur als historische Referenz). |
| [`Dokumentation/OE8HSRotor01.pdf`](Dokumentation/OE8HSRotor01.pdf) | Dokumentation / Zeichnungen zum Aufbau. |
| [`Dokumentation/Beschreibung-Deutsch.pdf`](Dokumentation/Beschreibung-Deutsch.pdf) | Beschreibung auf Deutsch. |
| Bilder (`image1.jpg`, `image2.jpg`, `OE8HSR-Elektronik.jpg`, …) | Fotos vom Aufbau und der Elektronik. |

### 3D-Druck

Alle Modelle liegen unter [`STL/`](STL/). Die gedruckte Stückliste verweist u. a. auf **80 % Infill** für `16-Stativhalterung` — siehe [`Partslist_Printed-1.txt`](Dokumentation/Partslist_Printed-1.txt).

---

## Kurzstart (Firmware)

1. **Arduino IDE** (oder PlatformIO) mit passendem Board-Paket.
2. Bibliothek **AccelStepper** installieren.
3. Passenden Sketch wählen (Standard **MK-1** oder **gpredict-Hamlib**-Variante).
4. **Pin-Belegung** und **Getriebefaktoren** (`SPR`, `RATIOAZ`, `RATIOEL`) an deine Mechanik anpassen, falls abweichend.
5. Seriell **19200** Baud; für gpredict ggf. `rotctld` wie im Kopf der Hamlib-Variante beschrieben.

---

## Mitwirken

Der Autor schreibt im Quellcode selbstironisch von „Metallworker-Code“ — Verbesserungen und saubere PRs sind willkommen.

---

## Danksagung / Quellen

- Anknüpfung an Ideen aus der frühen **SatNogs**-Firmware (siehe Kommentar in [`OE8HSRotorMK-1.ino`](Firmware/OE8HSRotorMK-1.ino)).

---

*Projekt: **OE8HSRotor** — mostly 3D printed small antenna rotor.*
