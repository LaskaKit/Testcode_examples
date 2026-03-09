import machine
import utime

CONVERSION_CONSTANT = 3.3 / 65535
adc = machine.ADC(0)  # kanal 0 odpovida GP26

while True:
    value = adc.read_u16()
    voltage = value * CONVERSION_CONSTANT
    print("{} V".format(voltage))
    utime.sleep(1)
