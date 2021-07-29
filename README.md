# hvac-automation

Running this on a NodeMCU. Chose to use GPIO4 and 5 (labelled on the board as D1 and D2) because they are the only pins that stay low at boot as per:

"Additionally, the other GPIOs, except GPIO5 and GPIO4, can output a low-voltage signal at boot, which can be problematic if these are connected to transistors or relays. You can read this article that investigates the state and behavior of each GPIO on boot"

https://randomnerdtutorials.com/esp8266-pinout-reference-gpios/

https://rabbithole.wwwdotorg.org/2017/03/28/esp8266-gpio.html
