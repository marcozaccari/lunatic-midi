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
| ..            | ..                | ..              | ..                        |
| ..            | ..                | ..              | ..                        |
| ..            | ..                | ..              | ..                        |
| ..            | ..                | ..              | ..                        |
| -             | 19 (GPIO 10/MOSI) | 20 (GND)        | Led - (and resistor)      |
| -             | 21 (GPIO 9/MISO)  | 22 (GPIO 25)    | Led +                     |

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

```txt
  GND Spark GND   X    
    
VREF2 Spark VREF1 X----X #1 (3.3V) Rasp #2 (5V)

 SDA2 Spark SDA1  X----X #3 (SDA)  Rasp #4 (5V)

 SCL2 Spark SCL1  X----X #5 (SCL)  Rasp

                 X #7        Rasp

                 X #9 (GND)  Rasp
```
