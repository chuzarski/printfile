# printfile

Simple utility that prints files, but is sandboxed using [Landlock](https://landlock.io/)!

This utility will only print files within the users home directory. This is because Landlock restricts the program from opening files outside of the user home directory (even root).

## Requirements

This program requires **at least** Linux 5.13.9 with an up to date version of libc

(if you're running Arch, use the example libc.PKGBUILD included in the repo)

## Build/install

For unsandboxed version:
```
make nonlandlock
```

For sandboxed version:
```
make landlock
```

Install using:
```
sudo make install
```