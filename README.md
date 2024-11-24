# SYNOPSIS

noprivexec *cmd* *...*

# DESCRIPTION

noprivexec: disable setuid privileges

`noprivexec` disables the ability to use setuid privileges before executing
a command.

# EXAMPLES

```
$ noprivexec echo test
test

$ noprivexec sudo echo test
sudo: effective uid is not 0, is /usr/bin/sudo on a file system with the 'nosuid' option set or an NFS file system without root privileges?

$ noprivexec ping 8.8.8.8
ping: icmp open socket: Operation not permitted
```

# Build

```
make

#### static executable using musl
./musl-make
```

# OPTIONS

-h, --help
: usage summary

# ALTERNATIVES

* [noprivexec-go](https://codeberg.org/msantos/noprivexec-go)
* [noprivexec-rs](https://codeberg.org/msantos/noprivexec-rs)
