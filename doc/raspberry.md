# Raspberry Pi

## Pinout

Header:

| Pin               | Connection       | Pin             | Connection           |
|-------------------|------------------|-----------------|----------------------|
| 1 (3V3)           | -                | 2 (5V)          | Main power           |
| 3 (GPIO2/I2C SDA) | I2C bus (data)   | 4 (5V)          | ADC                  |
| 5 (GPIO3/I2C SCL) | I2C bus (clock)  | 6 (GND)         | GND                  |
| 7 (GPIO4)         | -                | 8 (GPIO14/TXD)  | -                    |
| 9 (GND)           | I2C bus (ground) | 10 (GPIO15/RXD) | -                    |
| ..                | ..               | ..              | ..                   |
| ..                | ..               | ..              | ..                   |
| ..                | ..               | ..              | ..                   |
| ..                | ..               | ..              | ..                   |
| 19 (GPIO 10/MOSI) | -                | 20 (GND)        | Led - (and resistor) |
| 21 (GPIO 9/MISO)  | -                | 22 (GPIO 25)    | Led +                |

## Install and update OS

- Write Rasp Pi OS (minimal, without GUI) on a new SD card, enabling SSH and WiFi and setting a custom user, with no telemetry.

- find IP address, example: `nmap -sn 192.168.1.0/24`

- secure SSH with `ssh-copy-id pi@address` and disabling login with password

- add virtual host to local `/etc/hosts`: `<Raspberry IP> lunatic`

```sh
sudo apt update
sudo apt upgrade
sudo apt install mc micro
sudo reboot
```

### Set periferals and boot options

#### Enable I2C

- `sudo raspi-config` and enable I2C on Interface Options submenu

- `sudo apt install i2c-tools`

- check modules availability: `sudo i2cdetect -y 1`

#### Enable MIDI and OTG

`/boot/firmware/config.txt`: add `dtoverlay=dwc2` to main section.

`/boot/firmware/cmdline.txt`: append ` modules-load=dwc2,g_midi` to the first line.

Note: `dwc2` is a driver for OTG, `g_midi` is Linux Gadget Module driver.

### Disable unused services and periferals for faster startup

`sudo raspi-config` and:

- System Options: Logging to None
- Interface Options: disable SPI, Serial Port
- Performance Options: disable Fan

Edit `/boot/firmware/config.txt`:

```conf
# Disable camera
camera_auto_detect=0
display_auto_detect=0

# Add to the main section
dtoverlay=disable-bt
```

```sh
# Disable rebuilding the manual pages index cache and man-db service
sudo rm /var/lib/man-db/auto-update
sudo systemctl disable man-db

# Disable auto update
sudo systemctl mask apt-daily-upgrade
sudo systemctl mask apt-daily
sudo systemctl disable apt-daily-upgrade.timer
sudo systemctl disable apt-daily.timer

# Disable Bluetooth & ModemManager
sudo apt remove --purge modemmanager
sudo systemctl disable bluetooth.service
sudo systemctl disable hciuart.service
sudo apt remove --purge bluez

# Misc
sudo apt remove triggerhappy
sudo systemctl disable triggerhappy

sudo systemctl disable avahi-daemon
sudo systemctl disable rpc-statd-notify

sudo apt autoremove --purge

# Disable swap
sudo dphys-swapfile swapoff
sudo dphys-swapfile uninstall
sudo systemctl disable dphys-swapfile
```

`/etc/modprobe.d/raspi-blacklist.conf`, add:

```conf
blacklist btbcm
blacklist hci_uart
```

Add to `/etc/fstab`:

```fstab
tmpfs    /var/log    tmpfs     defaults,nosuid,mode=0755,nodev,noatime 0 0
```

### Led (GPIO25) SET ON SYSTEM READY

Create and _make executable_ `/usr/local/bin/led-set.py`:

```python
#!/usr/bin/env python

import RPi.GPIO as GPIO
import time

LEDPIN = 25
GPIO.setwarnings(False)        # Suppress warnings
#GPIO.setmode( GPIO.BOARD )
GPIO.setmode( GPIO.BCM )
GPIO.setup( LEDPIN, GPIO.OUT )
GPIO.output( LEDPIN, True )
#time.sleep( 1.0 )
GPIO.output (LEDPIN, GPIO.HIGH)
#GPIO.cleanup()
```

Create `/etc/systemd/system/last_command.service`:

```ini
[Unit]
Description=System ready
#After=network.target
After=systemd-user-sessions.service
After=network-online.target

[Service]
Type=simple
ExecStart=/usr/local/bin/led-set.py

[Install]
WantedBy=multi-user.target
```

And enable:

```sh
sudo systemctl enable last_command
```

### Bin path and other dependencies

```sh
sudo mkdir /opt/lunatic
sudo chown <user>:<user> /opt/lunatic
```
