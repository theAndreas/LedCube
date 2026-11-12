# Roadmap

Wohin LedCube geht: was erledigt ist, was als Nächstes kommt, und die Ideen, die noch nicht
entschieden sind. Jeder offene Punkt hat eine Id, damit ein Commit, ein Pull Request oder eine
Diskussion ihn nennen kann. Die Meilensteine stehen in der Reihenfolge, in der sie angegangen
werden; was nur „nice to have“ ist, steht unter den Ideen. Ein erledigter Punkt bleibt in der
Liste seines Meilensteins, mit dem Commit oder PR, der ihn erledigt hat.

Die Quellen beziehen sich auf das Repo [AndreasBur/MaxMatrix](https://github.com/AndreasBur/MaxMatrix),
Ordner `LED/LED Cube/`, falls nicht anders angegeben; was jede taugt, steht unter
[Vorlagen](#vorlagen).

## Meilensteine

### M1 – Auf dem Template (in Arbeit)

LedCube auf [embedded-cpp-template](https://github.com/theAndreas/embedded-cpp-template), wie
Utl dorthin kam (dessen M2): erst die Dateien, dann was die neuen Checks finden. Bis M1 erledigt
ist, schlägt `make check` fehl.

| Id | Punkt | Status |
|---|---|---|
| T-1 | Arduino-Library-Layout: `src/LedCube.h` als Header, den ein Sketch einbindet, die Module in `src/ledcube/`, der Sketch als `examples/Demo`, die Eagle-Dateien in `hardware/`. Atmel-Studio-Projekt, kopierter Arduino-Core und Build-Artefakte (`.o`, `.a`, `.eep`, `.exe`) entfernt | erledigt |
| T-2 | `copier copy --overwrite`, Template v0.5.0, `skeleton=false`; `test/mock` aus dem Skelett übernommen; `tools/project.sh`: `DATABASE_EXAMPLE=Demo`, simavr aus bis T-8 | erledigt |
| T-3 | Das Repo und der Ordner heißen `LED-Cube`, cpplint nimmt die Header-Guards aber vom Ordnernamen, und das Template verlangt den Projektnamen `LedCube`. Repo umbenennen, oder die Guards anders lösen | offen |
| T-4 | Dev Container baut hinter dem Firmen-Proxy nicht: der Download von arduino-cli bricht mit curl 60 ab (TLS). Site-Variante unter `.devcontainer/vector/` auf dem internen Basis-Image, wie `.devcontainer/README.md` sie beschreibt | offen |
| T-5 | `make fmt` einmal über `src/` und `examples/`: Google-Stil, `const` rechts, Doxygen-Blöcke statt der Kopf- und Trennkommentare | offen |
| T-6 | CodingStyle.md: die Klassen in den Namespace `ledcube` (F-9); `typedef enum` mit Präfix `LEDCUBE_` zu `enum class`; `byte`/`boolean` zu `uint8_t`/`bool`; `StandardTypes.h` (`stdReturnType`, `E_OK`) ablösen; Makros wie `B(...)` und `LEDCUBE_*`-Konstanten zu `constexpr` | offen |
| T-7 | Host-Build: `digitalWriteFast.h` greift direkt auf AVR-Register zu und kompiliert nicht gegen `test/mock`. Die Pins hinter eine kleine Schnittstelle, damit `LedCube` auf dem Host testbar ist; erster GoogleTest in `test/` (ohne einen Test bricht CMake ab). Die Pins als Template-Parameter, `LedCube<13, 11, 12>`; auf dem AVR weiter `digitalWriteFast`, auf dem Host der Core, den der Mock aufzeichnet | erledigt |
| T-8 | `examples/SelfCheck` für simavr schreiben, dann `SIMAVR_MCU` und `SELF_CHECK_EXAMPLE` in `tools/project.sh` wieder setzen | offen |
| T-9 | Was die neuen Warnungen (`-Wconversion`, `-Wsign-conversion`, `-Wshadow`), clang-tidy und cppcheck finden: jeweils beheben oder NOLINT mit Grund | offen |
| T-10 | `make docs`: alles, was ein Sketch nutzen kann, dokumentiert | offen |
| T-11 | README: Hardware (Platinenvarianten 2x/3x 2891), Pinbelegung, Bauanleitung, Bilder, Tabelle Flash/RAM (`make sizes`) | offen |

### M2 – Fehler im bestehenden Code

Jeder zuerst als Test, der fehlschlägt (braucht T-7).

| Id | Punkt | Status |
|---|---|---|
| B-1 | `start()`: [LedCube.h:237](src/ledcube/LedCube.h#L237) `State == READY \|\| LEDCUBE_STATE_STOPPED` ist immer wahr; gemeint ist `\|\| State == LEDCUBE_STATE_STOPPED` | offen |
| B-2 | Lesen und Schreiben auf verschiedenen Puffern: `get*()` liest `CurrentFrame`, `set*()` schreibt `NextFrame`. Nach dem Tausch enthält `NextFrame` den vorletzten Frame, inkrementelles Zeichnen verliert Änderungen. Semantik festlegen, etwa Kopieren beim Tausch oder ein explizites `beginFrame()` | offen |
| B-3 | `Transformation::rotate()`, `mirror()`, `rotateWithShift()` deklariert, aber nicht implementiert: Linkerfehler beim Aufruf. Implementieren (G-4, G-5) oder entfernen | offen |

### M3 – Treiber

| Id | Punkt | Quelle | Status |
|---|---|---|---|
| D-1 | Multiplexing per Timer-ISR: `task()` aus der Timer-ISR statt aus `loop()`, konstante Layer-Frequenz, kein Flackern bei langen Animationen. Puffertausch nur in der ISR am Frame-Ende, geteilte Daten atomar gelesen | Led_Cube_MK2 (`ISR(TIMER1_OVF_vect)`), led_cube_1.2 | offen |
| D-2 | Hardware-SPI: `sendData()` über SPI statt Bit-Banging, per Schalter `LEDCUBE_USE_SPI` (eine Variante in `tools/project.sh`). Pins prüfen: Uno MOSI 11 / SCK 13, Mega 51 / 52 | MaxMatrix (`MAXMATRIX_USE_SPI`) | offen |
| D-3 | Ghosting vermeiden: vor dem Layerwechsel alle Layer aus (oder OE der 595), kurze Totzeit | led_cube_1.2 (`nop`s), xythobuz | offen |
| D-4 | Frame-Timing: `setNextFrameTimeElapsed()` durch einen internen Frame-Timer (ms/Frame) ersetzen; eine Animation gibt nur die Frame-Dauer vor | RGBCubeMK4, Led_Cube_MK2 (`lat`-Flag) | offen |

### M4 – Grafik und Transformationen

| Id | Punkt | Quelle | Status |
|---|---|---|---|
| G-1 | Primitive: 3D-Bresenham-Linie, Kreis, Rechteck, Box (gefüllt/Kanten), Kugel | ledcube8x8x8 (`line`, `cirp`, MIT), cube.cpp (`sphere`) | offen |
| G-2 | Ebenen: `setPlane(axis, index)`, `clearPlane()` | LEDcubeSoftware, Led_Cube_MK2 (`bouncePlane`) | offen |
| G-3 | Sinus- und Wurzel-LUT in PROGMEM (Sinus 48 Stützstellen, Abstand zur Mitte 8x8): kein `float` im Frame-Loop | eigene Optimierung für die cube.cpp-Effekte | offen |
| G-4 | Rotation 90/180/270°: `rotate(axis, degrees)` (B-3) | – | offen |
| G-5 | Spiegeln: `mirror(axis)` (B-3) | – | offen |

### M5 – Animations-Framework

| Id | Punkt | Quelle | Status |
|---|---|---|---|
| A-1 | Abstrakte Basis `Animation` mit `begin()`, `task()` → fertig, optional Fade-In/-Out; nicht blockierend | RGBCubeMK4 (`Animation::animate`) | offen |
| A-2 | Sequencer/Playlist: Animationen nacheinander, Dauer oder Wiederholungen je Animation, Zufallsmodus | RGBCubeMK4, Led_Cube_MK2 (`switch(a)`) | offen |

### M6 – Effekte

| Id | Effekt | Kurzbeschreibung | Quelle | Status |
|---|---|---|---|---|
| E-1 | 3D-Sinuswelle | `z = sin(phase + √(x²+y²))`, Welle von der Mitte aus; der erste Effekt auf A-1 | cube.cpp `sine`, RGBCubeMK4 `SineWave` | offen |
| E-2 | Cos-Fläche | `cos(x) + cos(y)`, wogende Fläche | cube.cpp `wave` | offen |
| E-3 | Regen | zufällige Tropfen fallen nach unten | cube.cpp `rain`, RGBCubeMK4 `Raindrops`, LEDcubeSoftware `rainfall` | offen |
| E-4 | Wachsende/hüpfende Box | Box wächst aus einer Ecke, springt umher | LEDcubeSoftware `growingBox`, `bouncingBox`, `outline_shrink` | offen |
| E-5 | Ebene hüpft | Ebene läuft entlang X/Y/Z hin und her | Led_Cube_MK2 `bouncePlane` | offen |
| E-6 | Pulsierender Würfel | Würfel wächst und schrumpft aus der Mitte | Led_Cube_MK2 `cubePulse` | offen |
| E-7 | Helix/Spirale | rotierende Doppelhelix | Led_Cube_MK2 `helix`, cube.cpp `spiral`, RGBCubeMK4 `Spiral` | offen |
| E-8 | Zufällig füllen/leeren | Voxel zufällig an/aus bis voll/leer | LEDcubeSoftware `randomFill`, cube.cpp `random_expand` | offen |

### M7 – Text

| Id | Punkt | Quelle | Status |
|---|---|---|---|
| X-1 | Text-Task, nicht blockierend: `setText()` und ein Task, der Zeichen für Zeichen einblendet | MaxMatrix `setTextWithShift`, `stringShiftTask` | offen |
| X-2 | Proportional-Font: `SpriteTable` (Breite und Spalten, PROGMEM, Umlaute) | MaxMatrix | offen |
| X-3 | Text fliegt nach vorne: Zeichen erscheint hinten und wird nach vorne geschoben | LEDcubeSoftware `textToFront`, led_cube_1.2 `cube_string_to_front` | offen |
| X-4 | Laufband um den Würfel: Text läuft über alle vier Seitenflächen (`shiftCircle`) | LEDcubeSoftware `textBelt`, led_cube_1.2 `cube_string_belt` | offen |

## Ideen, noch nicht entschieden

| Id | Idee | Quelle |
|---|---|---|
| I-1 | Montage-Ausrichtung: Abbildung logischer auf physische Koordinaten, wenn der Würfel gedreht oder gespiegelt verbaut ist | MaxMatrix (`setModuleOrientation`) |
| I-2 | Helligkeit: global über OE-PWM der 595 oder Software-BAM (2–3 Bit) | RGBCubeMK4 (BAM), LED_Cube_MAX6966 |
| I-3 | Rotation in 15°-Schritten per Lookup-Tabelle, für flüssiges Drehen um Z | led_cube_1.2 (`rotate_15..75_deg`) |
| I-4 | Animation per Taster/Encoder weiterschalten | Led_Cube_MK2 (`sw`) |
| I-5 | Weitere Effekte: seitliche Sinuswelle (`side_sine`), Platten-Moden (`plate_mode`), Snake (auch als Spiel), Feuerwerk/Explosion, Atom, Game of Life 3D, Space Invaders/Walking Cube, Sterne/Funkeln | cube.cpp, RGBCubeMK4, led_cube_1.2, xythobuz |
| I-6 | Uhr mit RTC DS1307 | Led_Cube_MK2_ClOCK |
| I-7 | Frame-Streaming über UART: Sync-Byte und 64 Byte Frame, Animationen am PC rechnen | cube.cpp (`sendChars`), CubeApp, xythobuz |
| I-8 | PC-Simulator: dieselben Animationsklassen auf dem Host, gegen den Mock statt der Hardware, als `HOST_CODE` des Templates (`tools/project.sh`); löst `extras/CodeBlocksSim` ab | eigenes `LedCube_Cb`, xythobuz CubeControl |
| I-9 | Vorberechnete Animationen im (externen) EEPROM speichern und abspielen | xythobuz (`mem`, `memLayer`) |
| I-10 | Audio-Visualizer mit MSGEQ7: Spektrum- oder VU-Anzeige | xythobuz AudioFirmware |

## Entscheidungen

- **Auf dem Template**: Build mit arduino-cli und CMake, Checks und CI aus
  embedded-cpp-template, aktuell gehalten mit `copier update`. Das Atmel-Studio-Projekt liegt
  nur noch in der Git-Historie.
- **Lizenz GPL-3.0-or-later**, der Default des Templates. Damit darf auch Code aus den
  GPL-Vorlagen (LEDcubeSoftware, led_cube_1.2, xythobuz) übernommen werden, mit Herkunftsangabe;
  Code aus ledcube8x8x8 (MIT) ebenso.
- **Treiber**: der eigene `LedCube` bleibt; das ISR-Konzept kommt von Led_Cube_MK2.
- **Animationen**: Architektur von RGBCubeMK4, Mathematik aus cube.cpp, Effekt-Ideen aus
  LEDcubeSoftware und led_cube_1.2.
- **Text**: von MaxMatrix (Task und Proportional-Font).
- **PC-Anbindung**, später: Ideen von xythobuz.

## Vorlagen

| Projekt | Plattform | Lizenz | Was taugt es als Vorlage? |
|---|---|---|---|
| **MaxMatrix** (`MaxMatrix/async/orientation`) | Arduino, MAX7219 | eigenes | Coding-Style, nicht-blockierender Shift-Task (Zustandsmaschine), Proportional-Font `SpriteTable`, `USE_SPI`-Schalter. **Vorlage für API-Stil und Text.** |
| **Led_Cube_MK2_all_arduino / _ClOCK** (Nick Schulze) | Arduino (ATmega328), Schieberegister | keine (Bitte um Namensnennung) | Hardware fast identisch: Layer-Byte und 8 Datenbytes per Bit-Banging, **Multiplexing in Timer1-ISR** mit konstanter Frequenz. ClOCK-Variante mit DS1307-RTC. **Vorlage für Treiber/ISR.** |
| **RGBCubeMK4** (Nick Schulze) | chipKIT/PIC32, RGB, BAM | keine | Beste **Animations-Architektur**: jede Animation eine Klasse mit `animate()` → fertig, Fade-In/-Out, Sequencer. Effekte: SineWave, Raindrops, Snake, Spiral, Spin, Atom, Explosion, Flip, Arrows, Wisps, GameOfLife. RGB/BAM nicht direkt übertragbar. **Vorlage für `Animation`.** |
| **cube / cubeflow** (Nick Schulze) | PC (Code::Blocks), sendet seriell | keine | Größter Satz **mathematischer Effekte**: `sine`, `side_sine`, `wave`, `plate_mode`, `sphere`, `fireworks`, `spiral`, `spin`, `spin_image`, `grow`, `rain`, `random_swap`, `snake`, `spell`, `atom`, `fade`, `random_expand`, `arrow`, BMP→Array. Float-Mathe, für AVR in LUTs umrechnen (G-3). **Vorlage für Effekt-Mathematik.** |
| **CubeApp** (Nick Schulze) | Windows-App, seriell | keine | PC-Steuerung mit Sound. Nur als Idee für PC-Streaming. |
| **LEDcubeSoftware_v1.0** | AVR, einfarbig 8x8x8 | GPL | Viele klassische Effekte (rainfall, wave3D, growingBox, bouncingBox, outline_shrink, randomFill, snake, laser, textBelt, textToFront, diagonalFill, dimmingCube), Zeichensatz, Transformationen. Blockierend. |
| **led_cube_1.2** (fornax, leyanda.de) | AVR (avr-gcc), einfarbig 8x8x8 | GPL | Rotation in 15°-Schritten, Translation, `cube_invaders`, `cube_walking`, `cube_belt`/`string_belt`, `cube_explosion`, `cube_sonic`, `cube_stars`. ISR-Multiplexing. |
| **xythobuz-LED-Cube** | ATmega32 + ATmega8, einfarbig 8x8x8 | GPL v3 | Komplettsystem: **serielles Upload-Protokoll**, Animationen im externen I²C-EEPROM, **Audio-Visualizer** (MSGEQ7), Snake-Spiel, Text, Selbsttest, Java3D-Editor *CubeControl*. |
| **ledcube8x8x8-master** | STC 8051 (China-Bausatz) | MIT | Zeichenprimitive `point`, `line`, `cirp` (Kreis), `print`. Code mit Lizenzhinweis übernehmbar. |
| **LED_Cube_MAX6966** | nur Eagle-Dateien | – | Alternative Hardware mit MAX6966 (PWM-Treiber) für Helligkeitsstufen. Nur bei einem Hardware-Redesign relevant. |
| **font/** | Font-Header/ASM | – | Viele Schriftgrößen und -orientierungen; `8x8_horizontal_LSB_1` ist bereits im Projekt. |
