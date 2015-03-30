# OpenWRT Configuration #

Run:
```
./scripts/feeds install kmod-batman-adv
./scripts/feeds install afrimesh-batman-adv
make menuconfig
Administration->afrimesh-batman-adv
```


# Flash Meraki #
```
fis init
load -r -b 0x80041000 -m http -p 80 -h 192.168.84.9 /~antoine/firmware.backfire.stable/atheros/openwrt-atheros-vmlinux.gz
fis create -r 0x80041000 -e 0x80041000 linux
load -r -b 0x80041000 -m http -p 80 -h 192.168.84.9 /~antoine/firmware.backfire.stable/atheros/openwrt-atheros-root.jffs2-64k
fis create -r 0x80041000 -e 0x80041000 rootfs
```

# Node Configuration #
```
uci delete wireless.wifi0.disabled
uci delete wireless.@wifi-iface[0].network
uci set wireless.wifi0.channel=1
uci set wireless.@wifi-iface[0].mode=adhoc
uci set wireless.@wifi-iface[0].ssid=afrimesh
uci set network.lan.mtu=1500
uci set network.lan.ifname="eth0 bat0"
uci set network.lan.proto=dhcp
uci delete network.lan.ipaddr
uci delete network.lan.netmask
uci set network.wifi0=interface
uci set network.wifi0.ifname=ath0
uci set network.wifi0.proto=none
#uci set network.wifi0.mtu=1524
uci set network.wifi0.mtu=1548
uci set network.bat0=interface
uci set network.bat0.ifname=bat0
uci set network.bat0.proto=none
uci set network.bat0.mtu=1500
uci set batman-adv.bat0.interfaces=ath0
uci commit
```



# Vis Server Configuration #
```
uci set batman-adv.bat0.vis_mode=server
uci commit
```