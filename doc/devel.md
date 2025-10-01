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
# For Raspberry 4
rm -rf /usr/local/go && tar -C /usr/local -xzf go1.25.1.linux-arm64.tar.gz

exit

# (reboot and check Go version)
sync
sudo reboot
go version
```

## Local

### VSCode

Install following extension for SSH remote development:

- [Go](https://marketplace.visualstudio.com/items?itemName=golang.Go) (for remote coding).
- [Go Outliner](https://marketplace.visualstudio.com/items?itemName=766b.go-outliner)
- [Go Profiling](https://marketplace.visualstudio.com/items?itemName=MaxMedia.go-prof)
- [Project Tasks](https://marketplace.visualstudio.com/items?itemName=haugerbr.project-tasks)

#### Workarounds

- If gopls server crashes continuously:

  ```sh
  go install -ldflags="-extldflags=-Wl,-ld_classic" golang.org/x/tools/gopls@latest
  ```
