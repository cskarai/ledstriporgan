# LED szalag orgona

[Angol fordítás - English translation](README.md)

## Tartalomjegyzék
  * [Bevezetés](#bevezetés)
  * [LED-ek vezérlése](#led-ek-vezérlése)
    * [Háttérszín LED-ek](#háttérszín-led-ek)
    * [Effekt LED-ek](#effekt-led-ek)
  * [Kapcsolási rajz](#kapcsolási-rajz)
    * [Áramkör](#Áramkör)
    * [Nyák-terv](#nyák-terv)
    * [Fényképek az elkészült panelről](#fényképek-az-elkészült-panelről)
  * [Használat](#használat)
    * [Fordítás és telepítés](#fordítás-és-telepítés)
    * [Földhurok leválasztó kapcsoló](#földhurok-leválasztó-kapcsoló)
    * [Megfelelő hangerő kiválasztása](#megfelelő-hangerő-kiválasztása)
  * [Jelfeldolgozás](#jelfeldolgozás)
    * [Mintavételezés és keretek (frame-k)](#mintavételezés-és-keretek-frame-k)
    * [Mono átalakítás](#mono-átalakítás)
    * [Jelenergia számítás](#jelenergia-számítás)
    * [Spektrum analízis (FFT)](#spektrum-analízis-fft)
    * [Aluláteresztő (basszus) szűrés](#aluláteresztő-basszus-szűrés)
    * [Basszus energia számítás](#basszus-energia-számítás)
    * [Beat érzékelés](#beat-érzékelés)
  * [Nyomkövetés](#nyomkövetés)

## Bevezetés

Kölyök koromban fantasztikusat buliztunk stroboszkóppal, amit az egyik haver készített, a mai napig emlékszem rá.
Egyszerű kapcsolás volt, ha egy frekvenciatartományban a frekvencia meghaladott egy bizonyos értéket, akkor a lámpa bekapcsolt, egyébként meg kikapcsolt. Potméterrel lehetett mindhárom lámpára beállítani, hogy hol kapcsoljon be és ki.
Ez a világ a 90-es évek volt, ma már sokkal ütősebb eszközök készíthetőek el jóval kevesebb pénzből. Gyakorlatilag bármit leprogramozhatunk és nem csak 3 lámpával, hanem tetszőleges számú LED-del.

Youtube videó:

[![LED strip organ video](docs/images/youtube-video.png)](https://www.youtube.com/watch?v=_tZMUPP6Tb4 "LED strip organ")

## LED-ek vezérlése

A LED-ek színsémája ezen a képen látható:

![LED color scheme](docs/images/ledstrip_hu.png)

### Háttérszín LED-ek

A háttérszín LED-ek folyamatosan világítanak, színt váltanak amikor a mikrovezérlő ütemet érzékel (beat detection). A feladatuk a háttér alapszín biztosítása

### Effekt LED-ek

| Szám   | Név&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;| Feladat                                                                                                                             |
|:------:|:---------------------------------------------------|:------------------------------------------------------------------------------------------------------------------------------------|
| 1. / A | Energia-mérő&nbsp;LED (fehér)                      | Négyzetes energia közép alapján állítja a LED-ek intenzitását a fehér egy árnyalatára.                                              |
| 1. / B | Energia-mérő&nbsp;LED (világos&nbsp;háttér)        | Négyzetes energia közép alapján állítja a LED-ek intenzitását a háttérszín LED színének egy világosabb árnyalatára.                 |
| 2. / A | Energiacsúcs-mérő&nbsp;LED (fehér)                 | A négyzetes energia közép függvény csúcsát érzékelve a LED-et fehéren felvillantja.                                                 |
| 2. / B | Energiacsúcs-mérő&nbsp;LED (világos&nbsp;háttér)   | A négyzetes energia közép függvény csúcsát érzékelve a LED-et világosított háttérszínen felvillantja.                               |
| 3.     | Basszuscsúcs-mérő&nbsp;LED                         | A LED-et kéken felvillantja, amikor a basszus frekvenciákon energiacsúcsot érzékel.                                                 |
| 4.     | Spektrumcsúcs-mérő&nbsp;LED                        | Az FFT-vel számolt frekvenciatartomány 6 részre van felosztva. Egy LED egy intervallum csúcsának érzékelésekor villan fel.          |
| 5. / A | RGB&nbsp;villogó&nbsp;(1,4,6)                      | A LED színe a frekvenciaintervallumok alapján számolódik, az (1,4,6) intervallumok a vörös, zöld, kék komponenseket határozzák meg. |
| 5. / B | RGB&nbsp;villogó&nbsp;(5,3,2)                      | A LED színe a frekvenciaintervallumok alapján számolódik, az (5,3,2) intervallumok a vörös, zöld, kék komponenseket határozzák meg. |

## Kapcsolási rajz

TODO

### Áramkör

TODO

### Nyák-terv

TODO

### Fényképek az elkészült panelről

TODO

## Használat

TODO

### Fordítás és telepítés

TODO

### Földhurok leválasztó kapcsoló

TODO

### Megfelelő hangerő kiválasztása

TODO

## Jelfeldolgozás

TODO

### Mintavételezés és keretek (frame-k)

TODO

### Mono átalakítás

TODO

### Jelenergia számítás

TODO

### Spektrum analízis (FFT)

TODO

### Aluláteresztő (basszus) szűrés

TODO

### Basszus energia számítás

TODO

### Beat érzékelés

TODO

## Nyomkövetés

TODO
