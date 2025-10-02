# Development and testing

## Raspberry

### Dependencies

```sh
sudo apt install libasound2-dev
```

### Go

```sh
cd
sudo su
echo "export PATH=$PATH:/usr/local/go/bin" >> /etc/profile
exit

mkdir downloads
cd downloads

# For Raspberry Zero
wget https://go.dev/dl/go1.25.1.linux-armv6l.tar.gz
# For Raspberry 4
wget https://go.dev/dl/go1.25.1.linux-arm64.tar.gz

sudo su

# For Raspberry Zero
rm -rf /usr/local/go && tar -C /usr/local -xzf go1.25.1.linux-armv6l.tar.gz
# For Raspberry 4, Zero2
rm -rf /usr/local/go && tar -C /usr/local -xzf go1.25.1.linux-arm64.tar.gz

exit

# (reboot and check Go version)
sync
sudo reboot
go version
```

## Local

### VSCode

#### Remote development

Install following extensions for SSH remote development:

- [Project Tasks](https://marketplace.visualstudio.com/items?itemName=haugerbr.project-tasks)
- [Go](https://marketplace.visualstudio.com/items?itemName=golang.Go)
- [Go Outliner](https://marketplace.visualstudio.com/items?itemName=766b.go-outliner)
- [Go Profiling](https://marketplace.visualstudio.com/items?itemName=MaxMedia.go-prof)

#### Local development

Install following extensions for general development:

- [Project Tasks](https://marketplace.visualstudio.com/items?itemName=haugerbr.project-tasks)

Install following extensions for Go:

- [Go](https://marketplace.visualstudio.com/items?itemName=golang.Go)
- [Go Outliner](https://marketplace.visualstudio.com/items?itemName=766b.go-outliner)
- [Go Profiling](https://marketplace.visualstudio.com/items?itemName=MaxMedia.go-prof)

Install following extensions for MPLAB Microchip development:

- [C/C++ Extension Pack](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools-extension-pack])
- [Services for MPLAB](https://marketplace.visualstudio.com/items?itemName=Microchip.mplab-extensions-core])
- [Clangd for MPLAB](https://marketplace.visualstudio.com/items?itemName=Microchip.mplab-clangd)
- [CMake Runner for MPLAB](https://marketplace.visualstudio.com/items?itemName=Microchip.runcmake)
- [Code Configurator (MCC) for Microchip devices](https://marketplace.visualstudio.com/items?itemName=Microchip.mplab-code-configurator)
- [Platform for MPLAB](https://marketplace.visualstudio.com/items?itemName=Microchip.mplab-extensions-platforms)
- [Toolchain Support for MPLAB](https://marketplace.visualstudio.com/items?itemName=Microchip.toolchains])
- [User Interfaces for MPLAB](https://marketplace.visualstudio.com/items?itemName=Microchip.mplab-ui)

#### Workarounds

- If gopls server crashes continuously:

  ```sh
  go install -ldflags="-extldflags=-Wl,-ld_classic" golang.org/x/tools/gopls@latest
  ```

### Microchip

#### Microchip PIC XC8

Arch Linux:

```sh
yay --noconfirm -S microchip-mplabxc8-bin
```

#### Pickit 2 programmer

Arch Linux:

```sh
cd /tmp
yay --G pk2cmd-plus

sed -i 's+PK2DeviceFile_v1.63.148.zip::http://www.microchip.com/forums/download.axd?file=0;749972+PK2DeviceFile.zip::https://web.archive.org/web/20201128020840/http://www.microchip.com/forums/download.axd?file=0;749972+g' /tmp/pk2cmd-plus/PKGBUILD

cd /tmp/pk2cmd-plus
makepkg --noconfirm -si

rm -rf /tmp/pk2cmd-plus
```
