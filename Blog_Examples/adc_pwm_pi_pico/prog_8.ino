from machine import PWM, Pin
from utime import sleep_us

LED_PIN = 25
BLINK_FREQUENCY = 1
PWM_FREQUENCY = 200 # nastavime tak aby lidske oko nedokazalo rozpoznat blikani
# hranice je okolo 120 Hz muzete vyzkouset

led = Pin(LED_PIN)
pwm = PWM(led)
pwm.freq(PWM_FREQUENCY)

duty = 0
direction = 1

while True:
    pwm.duty_u16(duty * duty)
    duty += direction
    if duty >= 255:
        direction = -1
        elif duty <= 0:
        direction = 1
        sleep_us(1_000_000 // BLINK_FREQUENCY // 255 // 2)
