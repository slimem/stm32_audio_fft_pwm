# stm32_audio_fft_pwm
---
This is a **STM32F407VG** project that reads audio data from the input pin ```PC0```, applies FFT and uses the low, mid and high audio frequencies to control 3 RGB led strips using 3x3 PWMs (9 in total). Light intesity changes are eased out using a **exp(-t/100)** exponent.

This project also configures a Virtual COM Port (VCP) which will make the STM32 appear as a COM port, so you can transmit and receive data using the functions ```VCP_send_str()``` and ```VCP_get_string()```.

## How to connect Audio input and PWM outputs (to be done)
* ADC input (audio): PC0
* PWM Outputs
  * RGB1: PB4, PB0, PB1
  * RGB2: PB5, PB6, PB7
  * RGB3: PB8, PB9, PC8

(to be extended)

## Compiler Setup
---
This project builds with the [CooCox IDE](https://www.st.com/en/development-tools/coide.html)

Enjoy this little project as you see fit, and feel free to contact me!

Email: slimlimem@gmail.com Linkedin: https://www.linkedin.com/in/slim-limem/
