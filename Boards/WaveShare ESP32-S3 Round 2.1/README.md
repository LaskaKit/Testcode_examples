# DistanceRing

Laserový měřič vzdálenosti s kruhovou stupnicí na displeji Waveshare
ESP32-S3-Touch-LCD-2.1. Uprostřed běží vzdálenost velkým fontem, po obvodu
stupnice se značkou nastavené meze a dole tlačítka, kterými se nastavuje
upozornění vestavěným bzučákem.

Funguje s laserovým dálkoměrem **VL53L1X** (stupnice 0–4 m) i **VL53L0X**
(stupnice 0–2 m). Který je připojený, firmware pozná sám.

Firmware si vystačí s **ESP32 core a knihovnami pro senzory** — žádná grafická
knihovna není potřeba. Kreslí se vlastní vrstvou (`Gfx.cpp`) přímo do
framebufferu RGB panelu v PSRAM, fonty jsou vygenerované z Barlow Bold do
vlastního formátu.

## Hardware

| Díl | Poznámka |
| --- | --- |
| [Waveshare ESP32-S3-Touch-LCD-2.1](https://www.laskakit.cz/waveshare-esp32-s3-round-2-1--480--480-ips-touch-wifi-modul/) | kruhový 2,1" IPS 480 × 480, řadič ST7701S (RGB), dotyk CST820, bzučák na expandéru TCA9554 |
| [VL53L1X](https://www.laskakit.cz/laserovy-senzor-vzdalenosti-vl53l1x-i2c/) | laserový dálkoměr, dosah do 4 m, I2C — **nebo** |
| [VL53L0X](https://www.laskakit.cz/laserovy-senzor-vzdalenosti-vl53l1x-i2c-2/) | laserový dálkoměr, dosah do 2 m, I2C |

### Rozpoznání senzoru

Oba senzory mají stejnou I2C adresu `0x29` i zapojení, liší se identifikačním
registrem. Firmware při startu přečte ID — nejdřív v podobě, jakou používá
VL53L1X (16bitová adresa `0x010F`, odpověď `0xEACC`), pak VL53L0X (8bitová
adresa `0xC0`, odpověď `0xEE`). Pořadí není náhodné: dotaz pro VL53L1X zapíše
do VL53L0X jen neškodnou hodnotu, kterou inicializace stejně přepíše.

Podle senzoru se pak nastaví stupnice (0–4 m nebo 0–2 m s popisky po půl
metru), nejvyšší mez alarmu a práh silného odrazu. Při výměně senzoru za běhu
se obrazovka překreslí. Uložená mez se kvůli kratšímu senzoru nepřepisuje —
omezí se jen pro zobrazení a po návratu VL53L1X platí zase původní.

Pod tlačítky je vidět název rozpoznaného senzoru.

### Zapojení

Senzor se připojí na I2C sběrnici desky — buď na I2C konektor, nebo na
12pinovou lištu:

| Senzor | Deska |
| --- | --- |
| VIN | 3V3 |
| GND | GND |
| SDA | GPIO15 |
| SCL | GPIO7 |
| XSHUT, GPIO1 | nezapojeno |

Adresa senzoru `0x29` se s ničím na desce nebije — obsazené adresy
jsou `0x15` (dotyk), `0x20` (expandér), `0x51` (RTC), `0x6B` a `0x7E`. Pozor
jen na to, že **GPIO7 a GPIO15 nelze použít jako běžné GPIO**, jsou natvrdo
spojené s čipy na desce.

## Ovládání

| Prvek | Funkce |
| --- | --- |
| velké číslo uprostřed | aktuální vzdálenost v metrech |
| prstenec po obvodu | stupnice 0–4 m, vyplněná část odpovídá naměřené hodnotě |
| žlutá značka na prstenci | nastavená mez upozornění |
| `−` / `+` | posun meze po 5 cm, podržením se mez posouvá rychle |
| `ZAP` / `VYP` | zapnutí a vypnutí upozornění |
| pilulka „blíž než …" | přepíná, jestli se alarm spustí při přiblížení, nebo při vzdálení |

Při splnění podmínky se prstenec i číslo zbarví červeně a bzučák přerušovaně
pípá. Alarm má hysterezi 5 cm, aby nedrnčel přesně na hranici. Nastavení se
ukládá do NVS, zápis je odložený o dvě sekundy, aby držení tlačítka
neznamenalo desítky zápisů do flash.

### Jak filtr rozhoduje, čemu věřit

Samotný stav, který senzor ke vzorku hlásí, není spolehlivé vodítko. Při
slabém odrazu hlásí „wrap target fail" i u zcela správné vzdálenosti — a
naopak „range valid" občas i u náhodného čísla. Filtr proto vzorky třídí podle
stavu **a** síly odrazu (`classifySample()` v `AlarmLogic.h`):

| Druh | Kdy | Co s ním filtr dělá |
| --- | --- | --- |
| silný | platný stav a odraz ≥ 1,5 MCPS | platí hned |
| slabý | platný stav se slabším odrazem, nebo wrap target fail | musí ho potvrdit další |
| nic | signal fail, out of bounds, sigma fail | senzor nic nevidí |
| chyba | porucha, timeout, cíl příliš blízko | — |

Slabé vzorky se nehodnotí jednotlivě, ale podle toho, jestli na sebe
navazují. Skutečný cíl dává řetěz hodnot blízko sebe — i když se pohybuje —
kdežto šum senzoru skáče o stovky milimetrů. Platí tedy až třetí slabý vzorek
v řadě, který se od předchozího liší nejvýš o 15 cm, a zobrazí se medián
posledních tří. To stačí i na cíl, který se pohybuje rychlostí kolem 0,75 m/s.

Při velmi slabém odrazu se jednotlivé vzorky rozptylují o ±30 cm a řetěz
nevznikne. Pro ten případ má filtr záložní cestu: medián posledních sedmi
vzorků, pokud je jejich rozptyl menší než u šumu bez cíle a nejnovější vzorek
medián podporuje. Hodnota je pak jen odhad (přesnost zhruba ±10 cm) a nadpis
to dá najevo žlutým „SLABÝ ODRAZ". Lepší přesnost v takové situaci nedá
žádný filtr — je potřeba silnější odraz (viz diagnostika).

Když 1,2 s (`HOLD_MS`) nepřijde nic, čemu by šlo věřit, přepne se displej na
„MIMO DOSAH". Senzor při tom střídá druhy selhání, takže by nadpis blikal mezi
„MIMO DOSAH" a pomlčkami — přepíše se proto až po třetím vzorku nového druhu.

Pravidla jsou odladěná na skutečných záznamech ze senzoru ve
`tools/logs/` a hlídá je regresní test (viz níže).

Když je cíl dál, než senzor dohlédne, nadpis se změní na „MIMO DOSAH",
prstenec se vyplní celý a hodnota ukáže konec stupnice — obojí tlumenou
barvou, aby bylo poznat, že nejde o naměřený údaj. Pro alarm se takový stav
počítá jako vzdálenost na konci stupnice, takže upozornění „dál než" funguje
i za dosahem.

Když senzor neodpovídá vůbec (odpojený kabel), nadpis se změní na „SENZOR
NEODPOVÍDÁ", hodnota zešedne na pomlčky a firmware se každé dvě sekundy
pokouší senzor znovu inicializovat.

## Diagnostika senzoru

V `DistanceRing.ino` je nahoře `#define SENSOR_DIAGNOSTICS 1`. Zapnuté
vypisuje na sériovou linku při startu seznam adres na I2C sběrnici a potom
**každý** vzorek, jak dorazí — řidší výpis by zakryl právě ty série chyb,
kvůli kterým se diagnostika zapíná:

```
I2C zarizeni: 0x15 0x20 0x29 0x51 0x6B
VL53L1X pripraven, rozpocet 140028 us, perioda 180 ms
VL53L1X status=0 (range valid) mm=1847 signal=1.43 ambient=0.21
```

Totéž ve zkratce ukazuje řádek pod tlačítky (stav, surová vzdálenost, síla
odrazu). Co která hodnota znamená:

| Stav | Význam | Kde hledat příčinu |
| --- | --- | --- |
| 0, 3, 6 | platné měření | — |
| 2 (signal fail) | slabý odraz | cíl daleko, tmavý nebo šikmý povrch; nízký `signal` to potvrdí |
| 4 (phase out of bounds) | nic v dosahu | stejné jako výše, nebo clona senzoru (fólie na čočce, krabička) |
| 7 (wraparound) | nejednoznačná vzdálenost | silný odrazný předmět za dosahem |
| 1 (sigma fail) | měření příliš rozkolísané | silné okolní světlo — vysoký `ambient` |
| 5 (hardware fail) | chyba senzoru | napájení, kabeláž |

Když senzor nereaguje vůbec, chybí v seznamu adresa `0x29` — pak je problém
v zapojení, ne v konfiguraci měření.

### Kdy VL53L1X dosáhne 4 m

Čtyři metry jsou nejlepší případ, ne běžný provoz. Výrobce v datasheetu
(tabulka 18, režim Long, rozpočet měření 100 ms) uvádí:

| Cíl | Tma | Okolní světlo 50 kcps/SPAD | Silné okolní světlo 200 kcps/SPAD |
| --- | --- | --- | --- |
| bílý (88 % odrazivost) | 360 cm | 166 cm | 73 cm |
| šedý (54 %) | 340 cm | 154 cm | 69 cm |
| tmavě šedý (17 %) | 170 cm | 114 cm | 68 cm |

„Silné okolní světlo“ odpovídá podle výrobce slunečnému dni za oknem s přímým
osvitem senzoru. Delší rozpočet měření dosah ještě o něco prodlouží, přes
tyhle hranice se ale nedostane — rozhoduje odrazivost cíle a okolní světlo.

V režimu Short je dosah jen asi 130 cm, ale okolní světlo ho skoro neovlivní.
Pod přímým sluncem tak Short měří dál než Long.

VL53L0X má podle výrobce dosah do 2 m, platí pro něj totéž: tma a světlý cíl.

### Dosah versus rychlost

Nejčastější příčina stavů 2 a 7 na vzdálenost, kterou by senzor měl zvládnout,
je příliš krátký rozpočet měření. Čím déle senzor sbírá fotony, tím slabší
odraz rozezná od okolního světla — a hodnota `ambient` v diagnostice ukazuje,
kolik toho světla je. Konstanty jsou v `DistanceSensor.cpp`:

| Rozpočet / perioda | Odezva | Dosah v osvětlené místnosti |
| --- | --- | --- |
| 50 / 60 ms | 16× za sekundu | zhruba metr |
| 140 / 180 ms (výchozí) | 5× za sekundu | celé 4 m |
| 200 / 250 ms | 4× za sekundu | maximální, nejodolnější vůči světlu |

Perioda musí vždy zůstat delší než rozpočet.

### Práh síly odrazu

Senzor zahodí měření, jehož odraz je slabší než nastavený práh, a nahlásí
stav 2 — i když je výsledek stabilní. Výchozí hodnota knihovny (192, tedy
1,5 MCPS) je na tmavé, matné nebo šikmé povrchy přísná, firmware ji proto
snižuje na 32 (0,25 MCPS) konstantou `VL53L1X_SIGNAL_THRESHOLD` v
`DistanceSensor.cpp`. U VL53L0X se práh nastavuje na 0,1 MCPS
(`VL53L0X_SIGNAL_LIMIT_MCPS`) spolu s delšími laserovými pulzy podle příkladu
knihovny pro delší dosah. Pokud by se objevovaly nesmyslné hodnoty tam, kde není
nic, práh zvyš; pokud naopak mizí měření, která vypadají správně, sniž ho —
hodnota `signal` v diagnostice ukazuje, kam ho posadit.

Když se po snížení prahu začne objevovat stav 1 (sigma fail), je rozkolísané
samotné měření; pak pomůže povolit i `RANGE_CONFIG__SIGMA_THRESH` (výchozí
360, jednotka je čtvrtina milimetru).

## Knihovny

- deska **esp32 by Espressif** ≥ 3.0.0
- **VL53L1X by Pololu**
- **VL53L0X by Pololu**

Potřeba jsou obě, i když je připojený jen jeden senzor — firmware rozhoduje
až za běhu.

## Překlad

### Arduino IDE

1. deska **ESP32S3 Dev Module**,
2. PSRAM: **OPI PSRAM**, Flash Size: **16MB**, Partition Scheme: **Huge APP**,
3. nahrávejte přes USB-C port označený **UART** (převodník CH343P s automatickým
   download obvodem — není potřeba mačkat BOOT a RESET).

### arduino-cli

```bash
arduino-cli lib install VL53L1X VL53L0X
./build.sh
```

## Vývoj bez desky

Vykreslovací vrstva i obrazovka nejsou nijak svázané s Arduinem, takže jdou
přeložit na počítači a vyrenderovat náhled do PPM:

```bash
cd tools
g++ -std=c++17 -O2 -I.. -o /tmp/preview preview.cpp ../Gfx.cpp ../Screen.cpp ../Font*.cpp
/tmp/preview /tmp/preview.ppm         # normální stav
/tmp/preview /tmp/alarm.ppm alarm     # spuštěný alarm
```

Zachycený výpis diagnostiky jde prohnat filtrem a podívat se, co by bylo na
displeji — hodí se k ladění chování při slabém odrazu bez připojené desky:

```bash
cd tools
g++ -std=c++17 -I.. -o /tmp/replay replay.cpp
/tmp/replay < logs/pohyb-a-ztrata-cile.txt
```

Výstup ukazuje u každého vzorku, jak ho filtr zařadil a co by bylo na displeji.

Skutečné záznamy ve `tools/logs/` hlídá regresní test, který u každého úseku
záznamu ví, co smí být na displeji (cíl na 1,55 m musí držet, po ztrátě cíle
se nesmí objevit vymyšlená hodnota a podobně):

```bash
cd tools
g++ -std=c++17 -I.. -o /tmp/test_logs test_logs.cpp && /tmp/test_logs
```

Nový záznam, na kterém se filtr chová špatně, stačí uložit do `logs/` a
dopsat do testu, co se v něm má zobrazit.

Výpočty (filtr měření, hystereze alarmu, mapování na úhel, formátování) mají
vlastní testy:

```bash
cd tools
g++ -std=c++17 -I.. -o /tmp/test_alarm_logic test_alarm_logic.cpp && /tmp/test_alarm_logic
```

Fonty se generují z TTF skriptem (potřebuje Pillow):

```bash
python3 tools/generate_fonts.py cesta/k/Barlow-Bold.ttf
```

## Struktura

| Soubor | Obsah |
| --- | --- |
| `DistanceRing.ino` | hlavní smyčka, obsluha dotyku a tlačítek, spojení senzoru s obrazovkou |
| `AlarmLogic.h` | filtr měření, vyhodnocení alarmu, geometrie stupnice — bez vazby na Arduino |
| `Screen.cpp/.h` | rozložení a vykreslení obrazovky, zásahové plochy tlačítek |
| `Gfx.cpp/.h` | kreslení do framebufferu: obdélníky, mezikruží, text |
| `GfxFont.h`, `Font*.cpp` | formát fontu a vygenerovaná písma (Barlow Bold) |
| `DistanceSensor.cpp/.h` | obsluha VL53L1X |
| `Buzzer.cpp/.h` | pípání přes expandér (EXIO8) ve vlastním tasku |
| `Settings.cpp/.h` | uložení nastavení do NVS s odloženým zápisem |
| `Display_ST7701.cpp/.h` | inicializace panelu, framebuffer v PSRAM, podsvícení |
| `Touch_CST820.cpp/.h` | dotykový řadič, vzorkování v tasku |
| `I2C_Driver`, `TCA9554PWR` | sdílená I2C sběrnice a GPIO expandér |

### Jak se kreslí

Panel scanuje framebuffer v PSRAM průběžně, takže se do něj kreslí přímo a
změna se objeví při nejbližším obnovení obrazu — není potřeba double buffering
ani čekání na snímek. Překresluje se jen to, co se změnilo: při novém měření
hodnota a prstenec, při stisku jen dotyčné tlačítko. Mezikruží se vykresluje
po paprscích s krokem úhlu voleným tak, aby na vnějším okraji nevznikaly díry.

Dotykový řadič, expandér s bzučákem i dálkoměr visí na jedné I2C sběrnici,
takže všechny přístupy procházejí přes `I2CBusGuard`, který celé transakce
serializuje — jinak by se cizí přenos vklínil doprostřed čtení.

## Odkud co pochází

- Inicializace panelu ST7701S, ovladač dotyku CST820, obsluha expandéru
  TCA9554 a rozložení pinů vycházejí z projektu
  [CooLajz/waveshare-hodiny](https://github.com/CooLajz/waveshare-hodiny)
  (licence MIT). Převzaté části jsou osekané na to, co tento firmware
  potřebuje.
- Fonty jsou vygenerované z písma **Barlow Bold** (SIL Open Font License 1.1,
  text licence je v `OFL.txt`).

## Co by šlo přidat

- jednotky v centimetrech pro krátké vzdálenosti (pod 1 m)
- podržení hodnoty tlačítkem (hold/max), hodí se při měření do rohů
- kalibrace offsetu senzoru podle vzdálenosti od čela krabičky
- volba rozsahu stupnice (1 m / 2 m / 4 m) pro jemnější odečet
