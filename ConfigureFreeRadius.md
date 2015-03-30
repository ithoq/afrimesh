# Setting up FreeRADIUS With MySQL Backend #

## Configure FreeRADIUS ##

> Copy config from source repository # TODO  -> step-by-step instructions

  * Run:
```
  # TODO - this is evil. We should be adding village-bus-radius to the freeradius group rather
  chmod 0777 /var/log/freeradius
  chmod 0666 /var/log/freeradius/radutmp
```

  * Check: /etc/freeradius/clients.conf
```
  client mesh.network/address {
    secret     = theradiussecret
    shortname  = meshportal
  }
```


TODO document add counter for  prepaid to sites-enabled/default


## Configure village-bus-radius Makefile ##

  * Check: village-bus-radius/Makefile
```
  MYSQL_ADMIN_USERNAME=root
  MYSQL_ADMIN_PASSWORD=xxx
  RADIUS_DATABASE=radius
  RADIUS_DATABASE_USERNAME=radius
  RADIUS_DATABASE_PASSWORD=xxx
```

## Set up MySQL User&Database ##

  * Run:
```
  make mysql
  make mysql-populate
```

## Test ##

  * Run:
```
  make test-list
```

## Configure coova-chilli on gateway ##

  * Check:
```
  CGI_BIN/uam.pl
```

## Configure coova-chilli on routers ##

  * Check: /etc/chilli/defaults
```
```



# Setting up FreeRADIUS With Memcachedb Backend #