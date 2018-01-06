Group 15
Ting-Yu (Jacky) Wang, Denis Jivaikin, Novin Changizi
5 / Apr / 2017

Our code first exports the GPIO (44, 45, 46) and PWM (bone_pwm_P8_13) pins. Then it sets the direction of the GPIO pins to out. Opens the values for each of these pins. Then inside a loop we use a counter variable that goes from 0 to 8, and mask it to find the value for each led, and use it to index into a frequency and duration array for the pwm. The pwm signal is applied to a buzzer to play the Super Mario Theme.
