pwm.duty_u16(65535>>2)  # nastaveni stridy
pwm.duty_ns(1/200/2*1000000)  # nastaveni stridy
pwm.freq(120)  # nastaveni frekvence
pwm.deinit()  # vypnuti
