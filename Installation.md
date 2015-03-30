# Step 1 - Install the Afrimesh Dashboard Server #

---


### Option 1 - Ubuntu Packages ###

Before you can install the afrimesh-dashboard package you will need to
add the Afrimesh package archive & signing keys to the system.

To do this, please open up a Terminal and run:

```
  sudo apt-add-repository ppa:afrimesh/ppa
```

On older versions of Ubuntu you may need to run this instead:

```
  sudo apt-key adv --keyserver keyserver.ubuntu.com --recv-keys 50CFBA3E
  sudo sh -c "echo deb http://ppa.launchpad.net/afrimesh/ppa/ubuntu hardy main >> /etc/apt/sources.list"
  sudo apt-get update
```

![http://afrimesh.googlecode.com/svn/wiki/Installation.images/ubuntu-terminal.png](http://afrimesh.googlecode.com/svn/wiki/Installation.images/ubuntu-terminal.png)

You should only ever have to do this once to access all present
and future Afrimesh releases from your machine.

Finally, you can install the afrimesh-dashboard package by running:

```
  sudo apt-get install afrimesh-dashboard
```

Alternatively, the afrimesh-dashboard package will now also be
available for installation from Synaptic Package Manager:

![http://afrimesh.googlecode.com/svn/wiki/Installation.images/ubuntu-synaptic.png](http://afrimesh.googlecode.com/svn/wiki/Installation.images/ubuntu-synaptic.png)

That's it!

Now you can go on to: **Step 2 - Configure the Afrimesh Dashboard Server**

---



### Option 2 - OpenWRT ###

TODO

---



### Option 3 - FreeBSD ###

TODO

```
  cd /usr/ports/net-mgmt/afrimesh-dashboard
  make install clean
```

---



### Option 4 - From Source ###

The latest stable version of the Afrimesh sources can always be
obtained from: http://code.google.com/p/afrimesh/downloads/list

```
  make all
  make install
```

---



# Step 2 - Configure the Afrimesh Dashboard Server #

Once you have installed the afrimesh dashboard package you can
access the web interface by opening a browser and going to:

```
  http://<your machine's address>/afrimesh/ 
```

Assuming everything went according to plan you should now see the
Network Map display:

![http://afrimesh.googlecode.com/svn/wiki/Installation.images/network-map.png](http://afrimesh.googlecode.com/svn/wiki/Installation.images/network-map.png)

```
  ==================== IMPORTANT ====================
 
  You will see that all the dashboard service addresses are set to
 `demo.afrimesh.org` by default.

  This is so that you can verify that the afrimesh-dashboard
  installation was successful even if your own network is not yet
  properly configured.

  You HAVE to set these addresses to the appropriate values for your
  network in order for the dashboard to function on your network.

  By default a fresh installation of afrimesh-dashboard is configured
  to manage MY network and not yours!!! :-)

  ==================== IMPORTANT ==================== 
```

To configure the Afrimesh Dashboard for your network you need to
open the Utility->Settings menu:

![http://afrimesh.googlecode.com/svn/wiki/Installation.images/utility-settings-menu.png](http://afrimesh.googlecode.com/svn/wiki/Installation.images/utility-settings-menu.png)

Let's go through each setting individually:

---




![http://afrimesh.googlecode.com/svn/wiki/Installation.images/utility-settings-mesh.png](http://afrimesh.googlecode.com/svn/wiki/Installation.images/utility-settings-mesh.png)

### Visualization Server ###

The **Visualization Server** is responsible for tracking how the nodes in
your mesh network connect to one another.

All mesh nodes periodically connect to the **Visualization Server** and
update it with information about their neighbors.

It is recommended that you install the **Visualization Server** for your
network on the server or mesh node which acts as the mesh gateway
although, in practice, any system reachable from the mesh network can
be used.

TODO - how to install vis on each of OpenWRT, FreeBSD, Ubuntu

### Accounting Server ###

The **Accounting Server** is responsible for tracking the amount of data
downloaded from the Internet by each mesh node.

You _must_ install it on the server or mesh node acting as the
Internet Gateway. Installing it elsewhere will provide misleading
data!

Afrimesh currently supports only the [pmacct](http://www.pmacct.net/)
service for the collection of traffic data.

It should have been installed by default on your dashboard server when
you installed the afrimesh-dashboard package.

If your dashboard server is on the same machine as your gateway you
can simply set this field to the address of your dashboard server.

TODO - manual installation & configuration of pmacct

#### note ####

The pmacct packages in OpenWRT have a nasty bug that requires the
mysql libraries be installed on the router (!)

As a temporary workaround, I've compiled a fixed version from the
latest 0.12.0 pmacct sources. You can install it by adding the
following line to the top of your routers `/etc/opkg.conf` file:

```
  src packages http://downloads.afrimesh.org/stable/atheros/packages
```

and running:

```
  opkg update
  opkg install afrimesh-pmacctd afrimesh-pmacct-client
```

---




![http://afrimesh.googlecode.com/svn/wiki/Installation.images/utility-settings-internet.png](http://afrimesh.googlecode.com/svn/wiki/Installation.images/utility-settings-internet.png)

### IP Address ###

Set this to the address of your internet router.

### Community String ###

Afrimesh uses a protocol called SNMP to monitor your internet
router.

Usually the default setting of `public` will work just fine.

In the event that it doesn't, your ISP will know what you should set
it to.

### Interface ###

Set this to the correct network interface on your Internet router. If
you don't know, the easiest is to try each option and check if
Network->Health is displaying the correct traffic graph for your
Internet connection.

### Bandwidth ###

These settings are in KiloBitsPerSecond. Set them to the capacity of
your Internet connection.

---




![http://afrimesh.googlecode.com/svn/wiki/Installation.images/utility-settings-radius.png](http://afrimesh.googlecode.com/svn/wiki/Installation.images/utility-settings-radius.png)

### Radius Server ###

TODO - how to install & [ConfigureFreeRadius](ConfigureFreeRadius.md)

---




![http://afrimesh.googlecode.com/svn/wiki/Installation.images/utility-settings-map.png](http://afrimesh.googlecode.com/svn/wiki/Installation.images/utility-settings-map.png)

### Map Server ###

You can leave this on the default setting if you have an Internet Connection.

If you do not have an Internet Connection on your mesh you will need
to set up a Map Server on your local network and set this field to the
address of that server.

Instructions for doing this can be found [TODO](TODO.md).

### Location ###

You can set the location of your dashboard server using either the map
display or by manually entering your coordinates into the boxes
provided.

The map can be zoomed using the controls, dragged by clicking anywhere
on it with your mouse and the marker can be moved by clicking on it
and dragging it.

---





# Step 3 - Install Afrimesh on your mesh nodes #

You can ignore the instructions below and skip straight to the
configuration section if you are running Mesh Potato hardware as they
are already pre-installed to work with the Afrimesh Dashboard out of
the box.

### Option 1 - Install Afrimesh on an existing OpenWRT router ###

TODO

### Option 2 - Reflash router with an OpenWRT firmware image preconfigured with Afrimesh ###

TODO

---



# Step 4 - Configure your mesh nodes #

TODO

Assuming that Network & Wireless settings are correctly configured for
your particular network, the only configuration required consists of
setting the B.A.T.M.A.N. Vis Server to the same value you used in the
Dashboard configuration above.