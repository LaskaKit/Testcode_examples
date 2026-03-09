long spusteniMereni(byte mericiMod) {
    ....
    while (digitalRead(pDT));
    for (index=0;index<24;index++) {
        digitalWrite(pSCK,1);
        vysledek=vysledek<<1| digitalRead(pDT);
        digitalWrite(pSCK,0);
    }
    for (index=0;index<mericiMod;;index++) {
        digitalWrite(pSCK,1); digitalWrite(pSCK,0);
    }
    if (vysledek>=0x800000) vysledek=vysledek | 0xFF000000L;
    switch (mericiMod) {
        case 1: vysledek=vysledek/128/2; break;
        case 2: vysledek=vysledek/32/2; break;
        case 3: vysledek=vysledek/64/2; break;
    }
    return vysledek;
}
