import machine

pwm = machine.PWM(25, freq=200, duty_u16=65535>>1)
