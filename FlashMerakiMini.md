# Flashing a Meraki Mini Router #

  1. Download the appropriate afrimesh firmware image  TODO
  1. See: http://oldwiki.openwrt.org/OpenWrtDocs(2f)Hardware(2f)Meraki(2f)Mini.html

Steps:

  1. Set computer IP to 192.168.84.9
  1. Put firmware on a web server on your computer
  1. Plug computer ethernet into router ethernet
  1. Hook router up to computer serial port
  1. Start your favorite serial terminal software
  1. Power up router and hit ^C in serial terminal to break into boot loader
  1. Delete existing partitions:
```
    fis init
```
  1. Flash the kernel:
```
    load -r -b 0x80041000 -m http -p 80 -h 192.168.84.9 /path/to/firmware/openwrt-atheros-vmlinux.gz
    fis create -r 0x80041000 -l 0x180000 -e 0x80041000 linux
```
  1. Flash the root filesystem:
```
    load -r -b 0x80041000 -m http -p 80 -h 192.168.84.9 /path/to/firmware/openwrt-atheros-root.jffs2-64k
    fis create -r 0x80041000 -e 0x80041000 rootfs
```
  1. Update boot script:
```
    fconfig
    >> fis load -d linux
    >> exec
```
  1. Check boot script:
```
    fconfig -l
```
  1. Reboot