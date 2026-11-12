# Roadmap

Wohin LedCube geht: was erledigt ist, was als Nächstes kommt, und die Ideen, die noch nicht
entschieden sind. Jeder offene Punkt hat eine Id, damit ein Commit, ein Pull Request oder eine
Diskussion ihn nennen kann. Die Meilensteine stehen in der Reihenfolge, in der sie angegangen
werden; was nur „nice to have“ ist, steht unter den Ideen. Ein erledigter Punkt bleibt in der
Liste seines Meilensteins, mit dem Commit oder PR, der ihn erledigt hat.

Die Quellen beziehen sich auf das Repo [AndreasBur/MaxMatrix](https://github.com/AndreasBur/MaxMatrix),
Ordner `LED/LED Cube/`, falls nicht anders angegeben; was jede taugt, steht unter
[Vorlagen](#vorlagen). Welche weiteren Projekte und Techniken es gibt und was davon in die Roadmap
kam, steht in [docs/Research.md](docs/Research.md).

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
| T-5 | `make fmt` einmal über `src/` und `examples/`: Google-Stil, `const` rechts, Doxygen-Blöcke statt der Kopf- und Trennkommentare. Dazu Copyright und SPDX (GPL-3.0-or-later) statt „All rights reserved“; `digitalWriteFast.h` und die Font-Tabellen nur formatiert, ohne eigenen Kopf | erledigt |
| T-6 | CodingStyle.md: die Klassen in den Namespace `ledcube` (F-9); `typedef enum` mit Präfix `LEDCUBE_` zu `enum class`; `byte`/`boolean` zu `uint8_t`/`bool`; `StandardTypes.h` (`stdReturnType`, `E_OK`) ablösen; Makros wie `B(...)` und `LEDCUBE_*`-Konstanten zu `constexpr`. Dazu `ledcube::Error` (`kNone`, `kOutOfRange`, `kWrongState`) mit `LEDCUBE_NODISCARD`, und Google-Namen: `Begin()`, `Update()`, `SetVoxel()`, … | erledigt |
| T-7 | Host-Build: `digitalWriteFast.h` greift direkt auf AVR-Register zu und kompiliert nicht gegen `test/mock`. Die Pins hinter eine kleine Schnittstelle, damit `LedCube` auf dem Host testbar ist; erster GoogleTest in `test/` (ohne einen Test bricht CMake ab). Die Pins als Template-Parameter, `LedCube<13, 11, 12>`; auf dem AVR weiter `digitalWriteFast`, auf dem Host der Core, den der Mock aufzeichnet | erledigt |
| T-8 | `examples/SelfCheck` für simavr schreiben, dann `SIMAVR_MCU` und `SELF_CHECK_EXAMPLE` in `tools/project.sh` wieder setzen | offen |
| T-9 | Was die neuen Warnungen (`-Wconversion`, `-Wsign-conversion`, `-Wshadow`), clang-tidy und cppcheck finden: jeweils beheben oder NOLINT mit Grund | offen |
| T-10 | `make docs`: alles, was ein Sketch nutzen kann, dokumentiert | offen |
| T-11 | README: Hardware (Platinenvarianten 2x/3x 2891), Pinbelegung, Bauanleitung, Bilder, Tabelle Flash/RAM (`make sizes`) | offen |
| T-12 | `make coverage` zählt bei einem Template nur die Funktionen, die ein Test instanziiert: `Transformation` und jede ungetestete Funktion von `LedCube` fehlen im Bericht, statt als Lücke zu erscheinen. Explizite Instanziierung im Test, etwa `template class LedCube<13, 11, 12>;`, damit jede Zeile zählt. Danach zählt `LedCube.h` 176 Zeilen statt 81 (43 % erreicht), `Transformation.h` 114 (0 %) | erledigt |

### M2 – Fehler im bestehenden Code

Jeder zuerst als Test, der fehlschlägt (braucht T-7).

| Id | Punkt | Status |
|---|---|---|
| B-1 | `start()`: `State == READY \|\| LEDCUBE_STATE_STOPPED` ist immer wahr; gemeint ist `\|\| State == LEDCUBE_STATE_STOPPED` | erledigt |
| B-2 | Lesen und Schreiben auf verschiedenen Puffern: `get*()` liest `CurrentFrame`, `set*()` schreibt `NextFrame`. Nach dem Tausch enthält `NextFrame` den vorletzten Frame, inkrementelles Zeichnen verliert Änderungen. Semantik festlegen, etwa Kopieren beim Tausch oder ein explizites `beginFrame()` | offen |
| B-3 | `Transformation::rotate()`, `mirror()`, `rotateWithShift()` deklariert, aber nicht implementiert: Linkerfehler beim Aufruf. Implementieren (G-4, G-5) oder entfernen | offen |
| B-4 | `Begin()`: [LedCube.h:177](src/ledcube/LedCube.h#L177) ruft `ClearCube()` auf, bevor die Pins Ausgänge sind. Auf dem AVR schaltet ein Schreiben auf einen Eingang nur den Pull-up: das Löschen der Schieberegister kommt vermutlich nicht an, nach dem Einschalten zeigen sie Zufälliges. Pins zuerst als Ausgänge, dann löschen | offen |

### M3 – Treiber

| Id | Punkt | Quelle | Status |
|---|---|---|---|
| D-1 | Multiplexing per Timer-ISR: `Update()` aus der Timer-ISR statt aus `loop()`, konstante Layer-Frequenz, kein Flackern bei langen Animationen. Mindestens 100 Hz pro Frame, also 800 Hz pro Layer: Timer1 CTC, Prescaler 8, `OCR1A = 2499` (Timer1 belegt dann `Servo`, das gehört in die Doku). Puffertausch per `volatile`-Flag nur beim Übergang von Layer 7 auf 0, danach den neuen Frame kopieren (löst B-2); die API meldet „Tausch offen“, statt zu warten (H-1). ISR-Kosten in simavr messen, bevor die Frequenz feststeht | Led_Cube_MK2 (`ISR(TIMER1_OVF_vect)`), led_cube_1.2, Kevin Darrah (über Bradnam, GPL3+), [avr-libc atomic](https://avrdudes.github.io/avr-libc/avr-libc-user-manual/group__util__atomic.html) | offen |
| D-2 | Hardware-SPI: `SendData()` über SPI statt Bit-Banging, per Schalter `LEDCUBE_USE_SPI` (eine Variante in `tools/project.sh`). Pins prüfen: Uno MOSI 11 / SCK 13, Mega 51 / 52. Bei f_cpu/2 1,125 µs pro Byte, rund 10 µs für 9 Bytes, geschätzt 1–2 % CPU statt bis zu 37 % beim Bit-Banging. SPIF in der ISR pollen statt ein Interrupt pro Byte; D10 (SS, muss ohnehin Ausgang sein) als Latch-Pin | MaxMatrix (`MAXMATRIX_USE_SPI`), [Nick Gammon: SPI](http://www.gammon.com.au/spi), itsharryle/LED_CUBE | offen |
| D-3 | Ghosting vermeiden: Layer- und Spaltenbytes latchen an derselben Flanke, der Layer-Transistor schaltet verzögert ab. Mit OE (D-6): austasten, schieben, latchen, einschalten. Ohne OE: doppelter Latch, erst Layer-Byte 0, dann die Daten (+10 µs pro ISR). Die Schiebezeit ist Totzeit genug, keine `nop`s | led_cube_1.2 (`nop`s), xythobuz, Kevin Darrah, [chris-labs](https://hackaday.io/project/2167/log/13782) | offen |
| D-4 | Frame-Timing: `SetNextFrameTimeElapsed()` durch einen internen Frame-Timer (ms/Frame) ersetzen; eine Animation gibt nur die Frame-Dauer vor, Tempo und Haltezeit getrennt | RGBCubeMK4, Led_Cube_MK2 (`lat`-Flag), MD_Parola (`setSpeed`, `setPause`) | offen |
| D-5 | Schutz: `Stop()`, jeder Fehlerpfad und ein hängender Refresh tasten aus, sonst bleibt ein Layer mit 100 % statt 1/8 Tastgrad an. Optional ein Budget leuchtender LEDs pro Layer per Popcount (74HC595: ±70 mA durch VCC/GND je Chip, also höchstens 8,75 mA pro LED bei 8 an) und ein Watchdog, zurückgesetzt aus `Update()` | [TI SN74HC595](https://www.ti.com/document-viewer/SN74HC595/datasheet/GUID-0D3ADCDD-E940-48DD-AE9D-3DAC1D42CB5E), FastLED `power_mgt` | offen |
| D-6 | Hardware-Frage vor D-3 und I-2: Führt die Platine OE (und Q7S des letzten 595) heraus? Ein OE-Pin als optionaler Template-Parameter bringt dunkles Einschalten (B-4), Austasten, Helligkeit per PWM ohne CPU-Last und hardware-getimtes BAM; Q7S auf MISO prüft die ganze Kette beim Schieben | Kevin Darrah, [Recherche](docs/Research.md) | offen |

### M4 – Grafik und Transformationen

| Id | Punkt | Quelle | Status |
|---|---|---|---|
| G-1 | Primitive: 3D-Bresenham-Linie (höchstens 8 Schritte), Kreis, Rechteck, Box (gefüllt/Kanten) als `Box(start, end, style)`, Kugel. Zeilenweise als Bitmaske, da eine X-Zeile ein Byte ist: Quader `(0xFF << x0) & (0xFF >> (7 − x1))`, Kugel pro (y, z) ein x-Intervall, 64 Byte-Operationen statt 512 Voxeltests | ledcube8x8x8 (`line`, `cirp`, MIT), cube.cpp (`sphere`), Freetronics Cube4 (Box-Stile), MD_Cubo | offen |
| G-2 | Ebenen: `SetPlane(axis, index)`, `ClearPlane()` | LEDcubeSoftware, Led_Cube_MK2 (`bouncePlane`), MD_Cubo (`fillPlane`, `copyPlane`) | offen |
| G-3 | Ganzzahl-Mathematik ohne `float`: 8-Bit-Winkel (256 Schritte pro Umdrehung) mit Sinus-LUT in PROGMEM, 256 B als `int8_t` Q1.7 oder 65 B als Viertelwelle, `cos(a) = sin(a + 64)`. Statt einer Wurzel-LUT das Abstandsquadrat zur Mitte in halben Voxeln (3 bis 147, passt in `uint8_t`): Kugel `d2 <= r²`. Vorher nach M-3 ETL und Utl fragen | eigene Optimierung für die cube.cpp-Effekte, FastLED lib8tion (Konvention; Lizenz prüfen) | offen |
| G-4 | Rotation 90/180/270°: `Rotate(axis, degrees)` (B-3). Um x eine Byte-Permutation `frame[y][z] → frame[z][7−y]`, um y oder z eine 8x8-Bit-Transposition | Hacker's Delight (Transposition) | offen |
| G-5 | Spiegeln: `Mirror(axis)` (B-3). In x die Bits jedes Bytes umkehren, in y oder z Bytes tauschen | – | offen |

### M5 – Animations-Framework

| Id | Punkt | Quelle | Status |
|---|---|---|---|
| A-1 | Basis `Animation` mit `Begin()`, `Update()` → fertig, optional Fade-In/-Out; nicht blockierend. Zeit (Tick) getrennt vom Zeichnen (Paint), damit Effekte mit einer Fake-Uhr auf dem Host testbar sind; Zustände `stopped`, `running`, `stopping` für einen sauberen Ausklang | RGBCubeMK4 (`Animation::animate`), [led-cube-engine](https://github.com/LimpSquid/led-cube-engine) (GPL-3.0, nur Architektur), MD_Parola (`displayAnimate()`) | offen |
| A-2 | Sequencer/Playlist: Animationen nacheinander, Dauer oder Wiederholungen je Animation, Zufallsmodus. Effekte als PROGMEM-Tabelle `{Name, Init, Tick}`, Playlist `{Effekt, Dauer, Parameter}`, Zustand nur des aktiven Effekts in einem gemeinsamen Slot. Übergänge per 9-Bit-LFSR-Dissolve (jedes Voxel genau einmal, 2 B Zustand) und Maskenmischung `(a & m) \| (b & ~m)` | RGBCubeMK4, Led_Cube_MK2 (`switch(a)`), led-cube-engine (Registry, Playlists) | offen |

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
| I-1 | Montage-Ausrichtung: Abbildung logischer auf physische Koordinaten, wenn der Würfel gedreht oder gespiegelt verbaut ist. 48 Varianten (6 Achsenpermutationen mal 8 Spiegelungen) als Template-Parameter wie die Pins, ohne Laufzeitkosten, mit benannten Aliasen für übliche Bauweisen | MaxMatrix (`setModuleOrientation`), MD_MAX72XX (Hardware-Flags, `FC16_HW`), AlexGyver/LEDcube (`INVERT_X`, `INVERT_Y`) |
| I-2 | Helligkeit: zuerst global über OE-PWM der 595 (0 % CPU, braucht OE an einem PWM-Pin, D-6), danach Software-BAM mit variablem Intervall. 4 Bit (16 Stufen) kosten doppelt gepuffert 512 B RAM und geschätzt 5,6 % CPU, 6 Bit sind die ISR-Grenze. Bit-Gewichte im Host-Test prüfen (Darrahs V7 gewichtet falsch) | RGBCubeMK4 (BAM), LED_Cube_MAX6966, Kevin Darrah (über Bradnam, GPL3+), [batsocks: BCM](http://www.batsocks.co.uk/readme/art_bcm_1.htm) |
| I-3 | Rotation in 15°-Schritten, für flüssiges Drehen um Z. Für Geometrie das Modell drehen (Ecken mit dem Sinus aus G-3, Kanten neu zeichnen); für Voxel-Inhalte Paeths Drei-Scherungen-Verfahren, bijektiv ohne Löcher, Versätze für 24 Winkel in etwa 576 B PROGMEM statt 1,5 KiB Lookup | led_cube_1.2 (`rotate_15..75_deg`), [Paeth: Rotation by Shearing](https://ocf.berkeley.edu/~fricke/projects/israel/paeth/rotation_by_shearing.html) |
| I-4 | Eingabe-Abstraktion: sechs Richtungen und zwei Tasten, aus entprellten Tastern, einem Joystick mit Totzone oder seriell/Bluetooth (TPM2-Befehle, I-7). Damit Animation weiterschalten und Spiele steuern (I-5) | Led_Cube_MK2 (`sw`), Adafruit 3D-Snake, Cornell ECE4760 |
| I-5 | Weitere Effekte: seitliche Sinuswelle (`side_sine`), Platten-Moden (`plate_mode`), Snake (auch als Spiel), Feuerwerk/Explosion, Atom, Space Invaders/Walking Cube, Sterne/Funkeln. Game of Life 3D mit Bays' Regeln 4555 (überleben 4–5, geboren bei 5) oder 5766 (Bereich am Papier prüfen), nächste Generation im zweiten Puffer, Neustart bei Stillstand. Neu aus der Demoszene: Metaballs, Plasma, Wasseroberfläche (256 B), Lissajous- und Torusknoten-Spuren, 3D-Pipes, Labyrinth, Langton-Ameise, Warp-Sternenfeld. Spiele: 3D-Tetris (geschätzt 2–4 KB Flash), Tanks, Tunnel | cube.cpp, RGBCubeMK4, led_cube_1.2, xythobuz, [Bays](https://www.complex-systems.com/?p=1882), AlexGyver/LEDcube (MIT), itsharryle/LED_CUBE (MIT) |
| I-6 | Uhr mit RTC DS1307 | Led_Cube_MK2_ClOCK |
| I-7 | Frame-Streaming über UART mit TPM2 statt eines eigenen Protokolls: `0xC9`, Typ `0xDA` (Daten) oder `0xC0` (Befehl), 16-Bit-Länge, Nutzlast, `0x36`, optional Quittung `0xAC`. Nutzlast 64 Byte (eigenes Bitformat, ~167 fps bei 115200) oder 512 Byte (ein Kanal pro LED, per Schwelle zu Bits, Puffer bleibt 64 B). Jinx!, Glediator und PixelController senden TPM2 schon; 512 LEDs sind ein DMX-Universum, Art-Net nur über eine PC-Brücke. Steuerbefehle mit denselben Verben wie die API. Offen: verliert der UART bei 1 MBaud neben der ISR Bytes? | cube.cpp (`sendChars`), CubeApp, xythobuz, [TPM2](https://www.ledstyles.de/index.php/Thread/18969-tpm2-Protokoll-zur-Matrix-Lichtsteuerung/), [Jinx!](https://live-leds.de/?p=20), Freetronics Cube4 (Befehle wie die API) |
| I-8 | PC-Simulator: dieselben Animationsklassen auf dem Host, gegen den Mock statt der Hardware, als `HOST_CODE` des Templates (`tools/project.sh`); löst `extras/CodeBlocksSim` ab. Im Browser per Emscripten mit exportiertem `step(ms)`, aufgerufen aus `requestAnimationFrame`: Weil nichts blockiert (H-1), braucht es kein Asyncify. 3D-Ansicht aus dem Wokwi-Bauteil oder ledcube-webgl; Quadrum als Editor prüfen | eigenes `LedCube_Cb`, xythobuz CubeControl, [fastled-wasm](https://github.com/jandelgado/fastled-wasm) (MIT), [wokwi/led-3d-cube](https://github.com/wokwi/led-3d-cube) (MIT), [ledcube-webgl](https://github.com/ultrafez/ledcube-webgl) (GPL-3.0), [Quadrum](https://github.com/geckoanton/Quadrum) (GPL-3.0) |
| I-9 | Vorberechnete Animationen speichern und abspielen: als 64-Byte-Frames in PROGMEM, optional XOR-Delta zum Vorframe und Nullläufe-RLE (Kompression ungemessen), oder als mitgeschnittener TPM2-Strom (ein Parser mit I-7). Das interne EEPROM (1 KiB, ~16 Frames) nur für Einstellungen, extern ein I²C-EEPROM | xythobuz (`mem`, `memLayer`), [DMX4ALL App-Note](https://www.dmx4all.de/download/appnotes/appnote_importieren_von_jinx_scenen_mit_dmx_configurator_english.pdf) |
| I-10 | Audio-Visualizer mit MSGEQ7: Spektrum- oder VU-Anzeige. Den MSGEQ7 als Zustandsmaschine über mehrere Ticks lesen (72 µs Strobe, 36 µs Einschwingen) statt mit `delayMicroseconds` (H-1). Ohne Zusatzchip ArduinoFHT (128 Punkte in 1,3 ms, 256 B RAM; Lizenz unklar, Sampling ohne Interrupt-Sperre nötig); arduinoFFT ist mit Gleitkomma zu langsam | xythobuz AudioFirmware, [ArduinoFHT](https://www.openmusiclabs.com/wiki/ArduinoFHT) |
| I-11 | Selbsttest-Animation für die Lötstellen: Layer, Säulen und Ebenen je Achse einzeln, eine LED läuft alle Positionen ab, am Ende Voxel (0,0,0) und die +X-Achse, damit die Ausrichtung (I-1) sofort sichtbar ist. Ergänzt T-8, das die Firmware prüft | MD_MAX72XX (`control(TEST, ON)`), [Hackster 5x5x5](https://www.hackster.io/doug-domke/5x5x5-led-cube-3c2d5f) |
| I-12 | Digitaler 3D-Sand mit Beschleunigungssensor: Körner fallen, wohin die Schwerkraft zeigt, aus rohen Beschleunigungswerten ohne `atan2`. Kornzahl als Template-Parameter (64 Körner geschätzt 384 B RAM). Dazu Schütteln zum Weiterschalten, Wasserwaage, Murmel-Labyrinth | [Adafruit_PixelDust](https://github.com/adafruit/Adafruit_PixelDust) (BSD, 2D), [3D Digital Sand](https://jpralves.net/post/2018/06/26/3d-digital-sand.html) |
| I-13 | Strom sparen: `Sleep()` tastet aus, hält den Multiplex-Timer an und legt den ATmega328P in Power-down (~355 nA mit ADC aus), Aufwachen per Pin-Change. Nachtmodus mit der Uhr (I-6), automatische Helligkeit mit einem LDR (I-2) | [Nick Gammon: Power saving](https://www.gammon.com.au/power) |
| I-14 | `Shift(axis, dir)` mit Wrap-around und einem Callback, der die neue Ebene liefert: Regen, Laufband (X-4), Sternenfeld und Tetris-Fall aus einer Funktion | MD_MAX72XX (Shift-Callbacks) |

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
| **MD_Cubo** (MajicDesigns) | Arduino, verschiedene Würfel | LGPL-2.1 | Einzige Würfel-Bibliothek im Arduino Library Manager (ruht seit 2022): Form einer Arduino-Bibliothek, Doxygen-Doku, Ebenen-Primitive. Auf dem 328P teuer (virtuelles `setVoxel` mit 32-Bit-Farbe, `delay()` in `animate()`). **Konkurrent und Vorbild für die Form.** |
| **itsharryle/LED_CUBE**, Fork **AlexGyver/LEDcube** | Arduino, 8x8x8 über SPI | MIT | **Exakt dieselbe Topologie** (Layer-Byte, dann 8 Zeilenbytes per SPI), nicht blockierende Effekte mit Tick-Zählern; AlexGyver ergänzt Snake, Tanks, Tunnel und `INVERT_X/Y`. Refresh aus `loop()`. **Vorlage für Effekte und Spiele, Code übernehmbar.** |
| **LimpSquid/led-cube-engine** | PC/Embedded-Linux, C++17 | GPL-3.0 | Beste moderne **Effekt-Architektur**: Tick getrennt von Paint, Zustand `stopping`, Registry, Playlists mit Parametern, Mock-Display. Braucht Heap, STL, Exceptions: nur das Design übernehmen. |
| **Freetronics Cube4** | Arduino Leonardo, 4x4x4 RGB | GPLv3 (Dateiköpfe) | Jeder API-Aufruf hat einen seriellen Textbefehl als Zwilling, Box-Stile, versteckte Staging-Ebene, `suspend()`/`resume()`. **Vorlage für Befehle (I-7).** |
| **Kevin Darrah 8x8x8** (über John Bradnams Hackster-Kopie) | Arduino, 595, BAM | GPL3+ (Kopie; Original ungeprüft) | Bekannteste Vorlage für **BAM in einer Timer1-ISR** und für Austasten vor dem Latch. Stil kein Vorbild; V7 hat einen Gewichtsfehler und einen Latch-Bug (`PORTD &= 0<<latch_pin`). |
| **CHR, Instructables 8x8x8** | AVR | angeblich CC BY-NC-SA | Meistkopiertes Vokabular aus Zeichenfunktionen und rund 30 Effekten. **Nur als Ideenquelle**, Lizenz nicht kompatibel. |
| **MD_MAX72XX / MD_Parola** (MajicDesigns) | Arduino, MAX7219 | LGPL-2.1 | API-Ideen: Hardware-Varianten als Flags mit Aliasen (I-1), Shift mit Callbacks (I-14), gebündelte Updates, „oft aufrufen, `true` wenn fertig“ mit Tempo und Haltezeit (A-1, D-4). |
