# Node Configuration #

## Mesh Potato Configuration ##

  1. Plug Mesh Potato into power outlet
  1. Plug POTS handset into mesh potato
  1. Pick up Phone
  1. Router asks you for your name
  1. Voila! Dial tone!


## Manual Configuration ##

Given, a freshly flashed router with the afrimesh-webconfig package installed.

Connecting Via LAN interface:
  1. Plug computer into mesh router LAN socket
  1. Mesh router assigns client DHCP in the range 192.168.1.0/24

Connecting Via WIFI interface:
  1. Join router SSID
  1. Router assigns client DHCP in the range 192.168.2.0/24 ???

Common:
  1. Mesh router re-routes all client web requests to configuration page on own internal web server
  1. When client tries to surf s/he is presented with a series of steps:
    1. Specify WIFI IP                               (default: 10.x.x.x)
    1. Specify B.A.T.M.A.N. Vis server  (default: 10.0.0.2)
    1. Specify B.A.T.M.A.N. routing\_class (default: 3)
    1. Router reconfigures
    1. (Optional) Position router on map or via entering Lon/Lat
    1. (Optional) Enter personal information
    1. Reboot
  1. Router disables web redirection
  1. Router reboots


## Auto Configuration ##

Any particular reason to re-invent R.O.B.I.N.  ?