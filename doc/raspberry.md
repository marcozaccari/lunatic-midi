# Raspberry Pi

## Header

See [Raspberry Header](raspberry_header/README.md)

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

`/boot/firmware/config.txt`: add `dtparam=i2c_arm_baudrate=800000` to main section.

- `sudo apt install i2c-tools`

- check modules availability: `sudo i2cdetect -y 1`

Notes: the I2C frequency, even if it starts and works perfectly at 800khz, after several reads it drops to about half (measured 320khz).
Even going down, however, it remains an acceptable speed (~46us per byte).

#### Enable MIDI and OTG

`/boot/firmware/config.txt`: add `dtoverlay=dwc2` to main section.

`/boot/firmware/cmdline.txt`: append `modules-load=dwc2,g_midi` to the first line.

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
