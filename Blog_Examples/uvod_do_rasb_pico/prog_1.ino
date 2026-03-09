# modul machine slouzi k ovladani hardware
# pro rozblikani LED budeme potrebovat tridu Pin
from machine import Pin
from utime import sleep

# vestavena LED je napojena na pin 25, nejprve je potreba pin inicializovat
led = Pin(25, Pin.OUT)

# ted uz staci jen prepinat mezi HIGH a LOW, nejjednoduseji pomoci metody toggle()
while True:
    led.toggle()
    sleep(1)
