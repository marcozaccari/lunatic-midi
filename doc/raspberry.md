# Raspberry Pi

## Prepare a Raspberry Pi OS for devel environment

- Write Rasp Pi OS (minimal, without GUI) on a new SD card, enabling SSH and WiFi

- find IP address, example: `nmap -sn 192.168.1.0/24`

- `sudo apt update`, `sudo apt upgrade`, `sudo reboot`

- secure SSH with `ssh-copy-id pi@address` and disabling login with password

- add virtual host to local `/etc/hosts`: `raspberry_ip lunatic`

### Enable I2C

- `sudo raspi-config` and enable I2C on Interfacing submenu

- `sudo apt install i2c-tools`

- check modules availability: `sudo i2cdetect -y 1`

### Install Go

```sh
cd

sudo su
echo "export PATH=$PATH:/usr/local/go/bin" >> /etc/profile
exit

mkdir downloads
cd downloads
wget https://go.dev/dl/go1.18.3.linux-armv6l.tar.gz

sudo su
rm -rf /usr/local/go && tar -C /usr/local -xzf go1.18.3.linux-armv6l.tar.gz
exit

go version
```

### Bin path and dependencies

```sh
sudo mkdir /opt/lunatic
sudo chown pi:pi /opt/lunatic

sudo apt install libasound2-dev
```
