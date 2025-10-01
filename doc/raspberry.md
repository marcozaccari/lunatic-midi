# Raspberry Pi

## Prepare a Raspberry Pi OS for devel environment

- Write Rasp Pi OS (minimal, without GUI) on a new SD card, enabling SSH and WiFi and setting a custom user, with no telemetry.

- find IP address, example: `nmap -sn 192.168.1.0/24`

- `sudo apt update`, `sudo apt upgrade`, `sudo reboot`

- secure SSH with `ssh-copy-id pi@address` and disabling login with password

- add virtual host to local `/etc/hosts`: `<Raspberry IP> lunatic`

### Enable I2C

- `sudo raspi-config` and enable I2C on Interfacing submenu

- `sudo apt install i2c-tools`

- check modules availability: `sudo i2cdetect -y 1`

### Enable MIDI and OTG

Add `dtoverlay=dwc2` to main section of `/boot/firmware/config.txt`

Append `modules-load=dwc2,g_midi` to the first line of `/boot/firmware/cmdline.txt`

`dwc2` is a driver for OTG, `g_midi` is Linux Gadget Module driver.

### Disable unused services for faster startup

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
sudo systemctl mask cups.service
sudo systemctl mask cups-browsed.service

sudo systemctl disable avahi-daemon
sudo systemctl disable rpc-statd-notify

sudo apt autoremove --purge

# Disable swap
sudo dphys-swapfile swapoff
sudo dphys-swapfile uninstall
sudo systemctl disable dphys-swapfile
```

Add `dtoverlay=disable-bt` to main section of `/boot/firmware/config.txt`

Add to `/etc/modprobe.d/raspi-blacklist.conf`:

```conf
blacklist btbcm
blacklist hci_uart
```

Add to `/etc/fstab`:

```fstab
tmpfs    /var/log    tmpfs     defaults,nosuid,mode=0755,nodev,noatime 0 0
```

### Led (GPIO25) SET ON SYSTEM READY

Create and make executable `/usr/local/bin/led-set.py`:

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

sudo apt install libasound2-dev
```
