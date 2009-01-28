console.debug("loading afrimesh.js ...");


/** - bootstrap afrimesh -------------------------------------------------- */
var afrimesh = new function() {

    this.settings = { // new function() { this.locale  = "en_US.UTF-8"; }
      locale : "en_US.UTF-8",
      hosts  : {
        dashboard        : "dashboard.7degrees.co.za",
        vis_server       : "dashboard.7degrees.co.za",
        internet_gateway : "192.168.20.1",
        mesh_gateway     : "192.168.20.200"
      }
    };

    this.network = BootNetwork(this);

    this.customers  = undefined;
    
    this.villagebus = BootVillageBus(this);

  };


