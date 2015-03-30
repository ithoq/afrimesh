TODO do this in real detail for newbies (when we've got something rock solid)

Afrimesh requires an out-of-the-box Openwrt install plus the afrimesh-base package and its dependencies (libuci, libjson, batmand, libnetsnmp).  The overall proceess is:
  * Install openwrt
  * update opkg source file
  * edit afrimesh config script with your values
  * run afrimesh config script

# Flash router with Openwrt #

Afrimesh currently uses Kamikaze - 8.09.2

  * Get the correct openwrt image for your platform from here http://downloads.openwrt.org/kamikaze/8.09.2/
  * Flash your router...

# Download Config Files and Scripts #

telnet to your router (192.168.1.1) and run:
```
cd /etc
rm opkg.conf
wget http://afrimesh.googlecode.com/files/opkg.conf
wget http://afrimesh.googlecode.com/files/amcfg-node.sh
wget http://afrimesh.googlecode.com/files/amcfg-gw.sh
```

# Customize Your Configuration #

This is the tricky part. Since every router is different, hese scripts might not work as is for your hardware.  In order to help you modify the scripts if they don't work for you, here are some key things to remember:
  * Batman creates a separate virtual interface that mesh traffic routes through
  * Gateway nodes and regular mesh nodes have different batman configurations
  * Gateway nodes determine their default route based on which of its networks is advertising a gateway (this can include one gotten through DHCP).
  * The routing created by batman doesn't show up in your main routing table.  If you're like, "Whhhhhaaaat?" go here: http://www.open-mesh.net/wiki/RoutingVodoo
  * Debug outputs are helpful (`batmand -cd[1-3]`) you can run them in parallel with another batman instance.
  * batman man page: http://downloads.open-mesh.net/batman/development/sources/batmand_0.4-alpha-current_sources/man/batmand.8.html

_Note: Script has so far been tested on linksys wrt54gl and ASUS wl-520gu. It will likely work fine on any device with similar hardware configuration (multiple ethernet ports, one radio)_

Here goes...


## Make a gateway node ##

Edit the `amcfg-gw.sh` script as follows:

```
# Settings for the internet gateway, assuming upstream DHCP
WANIF=wan # software defined interface for the wan.  probably doesn't need to be changed
WANDEVICE=eth0.1 # physical device name for gateway
WANRATE=100mbit # expected data rate

# Mesh Parameters 
MESHIP=10.16.1.1 # unique for each node
MESHMASK=255.0.0.0 # same for all nodes. don't change me
MESHIF=mesh # software defined interface for the mesh don't change me
MESHDEVICE=wl0  # physical device name for radio. meraki potato = ath0, linksys = wl0
MESHCHANNEL=5 # wireless channel, same for all nodes
MESHBSSID=BB:BB:BB:BB:BB:BB # same for all nodes
MESHSSID=batman # same for all nodes

# dashboard connection parameters
VIS=8.9.7.4 # IP of vis server
SYSLOG=8.9.7.4 # IP of remote syslog capture
LONGITUDE=28.280891 # Location of your node
LATITUDE=-25.743542

# LAN Settings
LANIP=172.16.1.1 # lan ip address
LANMASK=255.255.255.0 # lan gateway
LANDEVICE=eth0.0  # physical device name for gateway
LANIF=lan  # software defined interface for the wan.  probably doesn't need to be changed

# Other useful stuff
HNAME=bat1 # system name so you know who you are in the shell
```

then run the script and you're done.

## Make a (non-gateway) Mesh Node ##

  * Edit the `amcfg-node.sh`  script.  The parameters are a subset of those for the gateway as explained above
  * run the script



# Old Stuff That's not Quite Ripe for Deletion... #

## Basic Router Configuration ##
  1. Plug computer into router ethernet
  1. Set computer IP to 192.168.1.9
  1. Telnet into router
```
  telnet 192.168.1.1
```
  1. Run:
```
  uci delete network.lan.type
  uci set network.lan.dns=<dns>
  uci set network.wifi=interface
  uci set network.wifi.ifname=<ifname>
  uci set network.wifi.proto=static
  uci set network.wifi.ipaddr=<ipaddr>
  uci set network.wifi.netmask=<netmask>
  uci set wireless.wifi0.disabled=0
  uci set wireless.wifi0.channel=<channel>
  uci set wireless.@wifi-iface[0].ssid=<ssid>
  uci set wireless.@wifi-iface[0].mode=adhoc 
  uci set wireless.@wifi-iface[0].bssid=<bssid>
  uci set wireless.@wifi-iface[0].network=wifi        
  uci set batmand.general.routing_class=3
  uci set batmand.general.visualisation_srv=192.168.20.225
  uci add firewall include
  uci set firewall.@include[-1].path="/etc/firewall.afrimesh"   
  uci commit
  /etc/init.d/batmand enable
  reboot
```


## afrimesh-portal (Optional) ##
afrimesh-portal provides the configuration files to run a [CoovaChilli](http://coova.org) captive portal on each mesh router.

The captive portal software is used to control access to the mesh&internet as well as enforcing bandwidth allocation.

  1. Run:
```
  opkg update
  opkg install afrimesh-portal
  uci set network.lan.ipaddr=0.0.0.0
  uci commit
  # Broken ??? /etc/init.d/chilli enable
  cd /etc/rc.d ; ln -s ../init.d/chilli S70chilli 
  reboot
```