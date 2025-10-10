# Raspberry Header

## Pinout

Raspberry header:

| Connection    | Pin               | Pin             | Connection                |
|---------------|-------------------|-----------------|---------------------------|
| PCA9306 VREF1 | 1 (3V3)           | 2 (5V)          | Main power, PCA9306 VREF2 |
| PCA9306 SDA1  | 3 (GPIO2/I2C SDA) | 4 (5V)          | ADC                       |
| PCA9306 SCL1  | 5 (GPIO3/I2C SCL) | 6 (GND)         | GND                       |
| -             | 7 (GPIO4)         | 8 (GPIO14/TXD)  | -                         |
| PCA9306 GND   | 9 (GND)           | 10 (GPIO15/RXD) | -                         |
| ..            | 11 (GPIO17)       | ..              | ..                        |
| OLED DC       | 13 (GPIO27)       | 14 (GND)        | ..                        |
| OLED RES      | 15 (GPIO22)       | ..              | ..                        |
| OLED VCC      | 17 (3V3)          | ..              | ..                        |
| OLED SDA      | 19 (GPIO10/MOSI)  | 20 (GND)        | Led - (and resistor)      |
| -             | 21 (GPIO9/MISO)   | 22 (GPIO 25)    | Led +                     |
| OLED SCL      | 23 (GPIO11/SCLK)  | 24 (GPIO 8)     |                           |

Sparkfun level translator PCA9306:

| Pin           | Connection                  |
|---------------|-----------------------------|
| PCA9306 VREF1 | 3.3V                        |
| PCA9306 VREF2 | 5V                          |
| PCA9306 GND   | GND                         |
| PCA9306 SDA1  | Raspberry 3 (GPIO2/I2C SDA) |
| PCA9306 SCL1  | Raspberry 5 (GPIO3/I2C SCL) |
| PCA9306 SDA2  | I2C bus (data)              |
| PCA9306 SCL2  | I2C bus (clock)             |

OLED ER-OLEDM015-2:

| Pin      | Connection                 |
|----------|----------------------------|
| OLED GND | GND                        |
| OLED VCC | Raspberry 17 (3V3)         |
| OLED SCL | Raspberry 23 (GPIO11/SCLK) |
| OLED SDA | Raspberry 19 (GPIO10/MOSI) |
| OLED RES | Raspberry 15 (GPIO22)      |
| OLED DC  | Raspberry 13 (GPIO27)      |
| OLED CS  | GND                        |
