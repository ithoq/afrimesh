/**
 * Afrimesh: easy management for B.A.T.M.A.N. wireless mesh networks
 * Copyright (C) 2008-2009 Meraka Institute of the CSIR
 * All rights reserved.
 *  
 * This software is licensed as free software under the terms of the
 * New BSD License. See /LICENSE for more information.
 */


User = { 
  username : String,
  password : String,
  limit    : { seconds : Number,
               bytes   : Number, 
               kbps    : Number },
  used     : { seconds : Number,
               bytes   : Number }
};
Users = [ User ];


Customer = { 
  users        : Users,
  name         : String,
  surname      : String,
  address      : String,
  transactions : []
};
Customers = [ Customer ];


NetworkInterface = { 
  name     : String,
  hostname : String,
  address  : String,
  netmask  : String
};

WirelessInterface = { prototype : NetworkInterface };
LanInterface = { prototype : NetworkInterface };
WanInterface = { prototype : NetworkInterface };


NetworkNode = {
  interfaces : [], /** [type], name, hostname, address */  /** e.g. mynode.inter
                                                               faces["WirelessInterface"].name = ath0 */
  snmp : {},
  uci  : {}
};


Router = { 
  prototype : NetworkNode,
  clients   : [ NetworkNode ],
  gateways  : [ /* Network / Router??? */ ] 
};
Routers = [ Router ];

BatmanRouter = {
  prototype : Router,
  interfaces : [],
  neighbours : [],  /** address : "", lq : float    */
  gateways   : [],  /** route   : "", label : "HNA" */
  snmp       : {},
  uci        : {}   /** network.routers -> populated by output from uci.show(rou
                        ter ip) */
};


Host = {
  prototype : NetworkNode
};


or = function() {
  node = arguments[0];
  for (i = 1; i < arguments.length; i++) {
    property = arguments[i][0];
    if (node[property]) {
      return arguments[i];
    }
  }
  return undefined;
};



/** - The Afrimesh Schema ------------------------------------------------- */
Schema = {
  settings   : { 
    locale : String,
    location : {
      longitude : Number,
      latitude  : Number 
    },
    map {
      extent : Number
    }
  },
  network    : { 
    routers    : [ Router ],
    mesh       : [ Router ],
    gateways   : [ Router ],
    supernodes : [ Router ],
    hosts      : [ Host ],
    users      : [ User ]
  },
  customers  : [ Customer ],
  villagebus : { 
    batman : [ { router : String,
                 route  : [ or, { neighbour : String }, { gateway   : String }  ],
                 label  : String } ],
    radius : {
      who    : function who()  {},
      list   : function list() {},
      create : function create(username, type, seconds) {},  // TODO - currently this is called new
      delete : function delete(username) {},
      modify : function modify(username, new_username, new_password, new_type) {}
    },
    snmp : {
      show : function show(address, community, oid) { [ ] }
    },
    uci : {
      show : function show(address, path) { return [ { package_name : { section_name : { option_name : String } } } ]; },
      set  : function set (package, section, option, value) { }
      // TODO         ^^^ -> set : function set(path, value) {}
    }
  }
};

