## Sprint Log ##

CodeSprint2009Log


## Code Review Documentation ##

http://code.google.com/p/support/wiki/CodeReviews


## Extensions ##
```
  Dhiren: 4605
  Dhiren Office: C-block Office 213
  Louis: 2444 / 083 4074610
  Elektra: C 234
  Antoine: 082 448 5157
```


## Gatewaying to Mesh ##

If your development machine is not physically on the mesh segment you can still access the mesh nodes by running:

```
  sudo route add -net 172.30.1.0/24 gw 146.64.19.207
```

## Network ##
```
  Internet Gateway -> 146.64.19.207 | 172.30.1.1
  Edwe -> 172.30.1.2
  Nkosinathi -> 172.30.1.3
  Jonathan -> 172.30.1.4
  Cynthia -> 172.30.1.5
  David -> 172.30.1.6
```

## Compile B.A.T.M.A.N. trunk ##

```
  mkdir ~/batman-svn
  cd ~/batman-svn
  svn co http://downloads.open-mesh.net/svn/batman/trunk/batman/
  cd batman
  make
```

## B.A.T.M.A.N. startup Script ##

  * Edit: ~/batmand.sh
```
#!/bin/bash                                                                                                                                                                             

# config settings                                                                                                                                                                       
ADDRESS=172.30.1.x
INTERFACE=wlan0
VIS_SERVER=172.30.1.1
BATMAND=/home/sprinter/batman-svn/batman/batmand

# nuke all batmand                                                                                                                                                                      
killall -9 batmand

# set regulatory domain                                                                                                                                                                 
iw reg set ZA

# configure interface for ad-hoc                                                                                                                                                        
ifconfig $INTERFACE down
iwconfig $INTERFACE mode ad-hoc
iwconfig $INTERFACE essid afrimesh
iwconfig $INTERFACE channel 11
iwconfig $INTERFACE ap 02:CA:FF:EE:BA:BE
ifconfig $INTERFACE $ADDRESS/24

# start batmand and configure vis server                                                                                                                                                
$BATMAND $INTERFACE -s $VIS_SERVER
$BATMAND -cd1
```


## Setting Regulatory Domain For Zyxel WiFi adaptor ##

  * Run:
```
  iw reg set ZA
```