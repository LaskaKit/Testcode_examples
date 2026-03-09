import machine
import utime

CONVERSION_FACTOR = 3.3 / 65535
temp_sensor = machine.ADC(4)

while True:
    reading = temp_sensor.read_u16() * CONVERSION_FACTOR
    temperature = 27 - (reading - 0.706 ) / 0.001721
    print("Voltage: " + str(reading) + " V, Temperature: " + str(temperature) + " °C")
    utime.sleep(1)
