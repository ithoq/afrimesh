/*
 * Afrimesh: easy management for mesh networks
 *  
 * This software is licensed as free software under the terms of the
 * New BSD License. See /LICENSE for more information.
 */


/**
 * @namespace
 * All interaction with external systems, both local and on the network
 * occur via the VillageBus
 *
 * sync calls returns any result from the request
 * async calls return the XMLHttpRequest object associated with the request
 */
var BootVillageBus = function (afrimesh) {

  // TODO - nasty hack to tide us over for demos
  var demo_server = "demo.afrimesh.org";

  var villagebus = function() {
    var ret = [];
    for (var p in this) { if (p != "prototype") ret.push(p); }
    return ret;
  };

  /** - ajax_proxy ------------------------------------------------------ */
  villagebus.ajax_proxy = function(prefix) { 
    console.log("EDGE: " + afrimesh.settings.edge);
    // edge_proxy : "/http/192.168.20.108/cgi-bin/villagebus.lua"
    var edge_proxy = afrimesh.settings.edge ? "/http/" + afrimesh.settings.edge + "/cgi-bin/villagebus.lua" 
                                            : "";  
    return prefix + afrimesh.settings.address + afrimesh.settings.ajax_proxy + edge_proxy; 
  };

  /** - villagebus.api -------------------------------------------------- */

  /* Either this:
  var name     = afrimesh.villagebus.Name("/@self/db/keys/status");
  var channel  = afrimesh.villagebus.GET(name, "*");
  var response = afrimesh.villagebus.Read(channel);
  if (afrimesh.villagebus.Fail(response)) return console.log(response.error);
  for (key in response) {
    // dadadada
  }

  // Or this:
  var name = afrimesh.villagebus.Name("/@self/db/keys/status");
  name = Bind(name, function(error, response) {
      if (error) return console.log(error);
      for (key in response) { // dadadadada }
      return response;
    });
  var channel = afrimesh.villagebus.GET(name, "*");
  var response = Read(channel); */ 

  /* TODO - A short note on the use of ajax_proxy below:
       I'm applying proxy universally for purposes of the SteveDemo
       as there's not necesarily routing between the mesh, the dashboard
       and the viewer.
       What needs to happen here in future is that we should _ONLY_
       be proxying if an address below is not directly accessible from 
       the browser. HOW to do that is utterly beyond me right now but
       I'm sure there's a nice solution. 
       Also see the work I'm going to be doing w/ Keith & the CloudBoard
     */
  function proxy(host, path) {
    var url = "/" + host + path;
    if (host != afrimesh.settings.address) {  // address may or may not be routable - TODO we vant to be sure!
      return afrimesh.villagebus.ajax_proxy("/") + "/http" + url;
    }
    return url;
  };
  villagebus.Name = function(name) {
    console.log("name: " + name);
    name = name.split('/').map(function(node) {   // perform path transformations for network locations
      if (node == "@root") {
        //return "/" + afrimesh.settings.root + "/cgi-bin/villagebus";
        return proxy(afrimesh.settings.root, "/cgi-bin/villagebus");
      } else if (node == "@self") {
        return "/" + afrimesh.settings.address + "/cgi-bin/villagebus"; 
      } else if (node == "@redis") {
        console.log("REDIS IS AT: " + afrimesh.settings.redis_server + " (" + afrimesh.settings.address + ") ");
        return proxy(afrimesh.settings.redis_server, "/cgi-bin/villagebus");
      } else if (node == "@topology") {
        //return "/" + afrimesh.settings.network.mesh.vis_server + ":2005";  
        return proxy(afrimesh.settings.network.mesh.vis_server, ":2005");
      } else if (node == "@pmacct") {
        //return "/" + afrimesh.settings.network.mesh.accounting_server + "/cgi-bin/villagebus";  
        return proxy(afrimesh.settings.network.mesh.accounting_server, "/cgi-bin/villagebus");
      } else if (node == "@radius") {
        //return "/" + afrimesh.settings.radius.server + "/cgi-bin/village-bus-radius";
        return proxy(afrimesh.settings.radius.server, "/cgi-bin/village-bus-radius");
      } else if (node == "@a3glue") {
        // TODO afrimesh.settings.servers.a3glue
        return proxy(afrimesh.settings.address, "/a3glue");
        //return proxy(afrimesh.settings.address, "/plink");
      } else if (node[0] == '@') {
        //return "/" + node.substring(1) + "/cgi-bin/villagebus"; 
        return proxy(node.substring(1), "/cgi-bin/villagebus");
      }
      return node;
    }).join('/');
    console.log("txlate: " + name);
    name = {
      async       : true,
      type        : "GET",
      url         : "http:" + name, 
      contentType : "application/json",
      //contentType : "application/x-javascript",
      dataType    : "jsonp",
      context     : document.body,
      success     : function(response) { return response; }, // TODO - default sync handler
      error       : function(response) { return response; }, // TODO - default sync handler
      complete    : function() { },
      beforeSend  : function() { }
    };
    name.continuations = [];
    return name;
  };
  
  /**
   * Bind name to a continuation
   * @name Bind
   * @function
   */
  villagebus.Bind = function(name, continuation) { // TODO -> Bind(name1, name2)
    if (isString(name)) {
      name = villagebus.Name(name);
    }
    name.continuations.push(continuation);
    //name.complete = function() { console.log("COMPLETE " + name.url); };
    name.success = function(response) {
      if (villagebus.Fail(response)) {
        console.error("VBUS.FAIL " + response.error + " " + name.url);
        return continuation(response.error, null);
      }
      name._response_ = response;
      return continuation(null, response);
    };
    name.error = function(response) {
      console.error("VBUS.XHR-ERROR " + response + " " + name.url);
      return continuation(response, null);
    };
    return name;
  };

  villagebus.Send = function(name, args) {
    console.log("VBUS." + name.type + " " + name.url);
    // TODO - if there are no continuations bound to name configure a
    //        sync JSON request so that we can block on read.
    if (name.continuations.length == 0) {
      console.log("No continuations registered. Converting " + name.url + " to Sync JSON call (" + name.dataType + ")");
      name.async = false;            // convert to SYNC call
      if (name.dataType != "text") { // don't convert from JSONP if it's already a JSON call
        name = jsonp_to_json(name, args); 
      }
      return name;
    }
    if (args) {
      var search = "?";
      for (var arg in args) {
        if (search.length > 1) {
          search += "&";
        }
        search += arg + "=" + args[arg];
      }
      name.url += search;
    }
    name.beforeSend(); // Grf. jQuery does not trigger beforeSend for jsonp calls
    name._xhr_ = $.ajax(name);
    return name;
  };

  villagebus.GET = function(name, args) {
    name.type = "GET";
    return villagebus.Send(name, args);
  };

  villagebus.PUT = function(name, data, args, raw) {
    name      = jsonp_to_json(name, data, raw);
    name.type = "PUT";
    return villagebus.Send(name, args);
  };

  villagebus.POST = function(name, data, args, raw) {
    name      = jsonp_to_json(name, data, raw);
    name.type = "POST";
    return villagebus.Send(name, args);
  };

  villagebus.DELETE = function(name, args, raw) {
    name      = jsonp_to_json(name, null, raw);
    name.type = "DELETE";
    return villagebus.Send(name, args);
  };

  // jsonp does not support POST so we need to adjust our strategy to use JSON 
  function jsonp_to_json(name, data, raw) {
    if (data) {
      name.data     = JSON.stringify(data);
    } 
    name.dataType = "text";
    var success = name.success; 
    name.success = function(response) {
      try {
        if (raw) {
          name._response_ = eval(response);
          return success(name._response_);
        } 
        function jsonp(payload) { // handle a jsonp reply
          name._response_ = payload;
          return success(name._response_);
        };
        eval(response); // evaluates jsonp reply and calls ^^^^^^^
      } catch (fail) { 
        return success({ error : fail }); // call original success fn
      }
    };
    if (parseURL(name.url).hostname != afrimesh.settings.address) {
      var proxy = afrimesh.villagebus.ajax_proxy("http://");
      console.log("JSONP_TO_JSON IS PROXYING: " + name.url);
      if (name.url.indexOf(proxy) == 0) {
        console.log("JSONP_TO_JSON HOWEVER CHANGED ITS MIND");
      } else {
        name.url = proxy + name.url;
        console.log("JSONP_TO_JSON SUBSEQUENTLY PRODUCED: " + name.url);
      }
    } else {
      console.log("JSONP_TO_JSON IS FEELING NO NEED TO PROXY: " + name.url);
    }
    return name;
  };
  villagebus.jsonp_to_json = jsonp_to_json; // export it - TODO - better Naming please


  /**
   * Intended semantics is that calling Read() on an async request will 
   * block until call returns. In practice, there's no way to block on 
   * an async request w/ Browser JS. :-/
   *
   * What we're doing at the moment is to reconfigure the request as a
   * sync JSON call (via ajax-proxy as needed)
   *
   * Best bet would prob. be to use Web Workers to manage XHR execution. 
   * WebKit and Gecko both support so mayhap 'tis time to just do it.
   *
   *   See: http://caniuse.com/#feat=webworkers
   *        http://html5test.com
   */
  villagebus.Read = function(name) {
    name._xhr_ = $.ajax(name);
    return name._response_;
  };
  
  /**
   * Check for a response of type: { 'error' : 'some message' }
   */
  villagebus.Fail = function(response) { 
    //console.log("Is this an error: " + show(response) + response.hasOwnProperty("error"));
    return (response ? response.hasOwnProperty("error") : response);
  };



  /** - VillageBus Message Queue ---------------------------------------- */
  
  villagebus.mq = {};

  // notify continuation whenever new message(s) are available in the queue
  villagebus.mq.Bind = function(name, continuation, rate) {
    var queue = "/@self/db/keys/message:" + name;
    continuation.mq       = { };
    continuation.mq.timer = undefined;
    continuation.mq.cache = { };
    name = (function poll(name, continuation, rate) {
      name = afrimesh.villagebus.Bind(queue, function(error, response) {
        if (error) return continuation(error, null);  
        if (!response) return; // empty queue
        response.map(function(message) {
          if (continuation.mq.cache[message]) { // has this continuation received this message yet?          
            return continuation.mq.cache[message];
          } 
          continuation.mq.cache[message] = true;
          return afrimesh.villagebus.GET(afrimesh.villagebus.Bind("/@self/db/" + message, continuation));
        });
      });
      name = afrimesh.villagebus.GET(name);
      continuation.mq.timer = setTimeout(function() {
        poll(name, continuation, rate);
      }, (rate ? rate : 60000));
      return name;
    })(name, continuation, rate);
    return continuation;
  };

  // delete message(s) from the queue
  // TODO - notify everyone subscribed to the queue
  // e.g. del message:device:<mac>:provision
  villagebus.mq.DELETE = function(name, continuation, queue) {
    name = "message:" + name;
    if (queue && queue.mq && queue.mq.cache) {
      queue.mq.cache[name] = false;
    }
    name = afrimesh.villagebus.Bind("/@self/db/" + name, continuation);
    return afrimesh.villagebus.DELETE(name);  
  };

  // stop notifying the continuation - TODO support multiple queues on continuations?
  villagebus.mq.Unbind = function(continuation) {
    console.log("UNBINDING: " + name);
    clearTimeout(name.mq.timer);
    name.mq.cache = {};
    return name;
  };  

  // add a new message to the queue
  villagebus.mq.POST = function(name, message, continuation) {
  };

  



  /** - villagebus.login ------------------------------------------------ */
  // TODO - support multiple authentication mechanisms e.g.  luci, htaccess, cert, ldap etc.
  villagebus.login = function(username, password, continuation, error) {
    return this.login.async(username, password, continuation, error);
  };
  villagebus.login.url = function() {
    return "http://" + afrimesh.settings.address + "/cgi-bin/luci/rpc/auth";
  };
  villagebus.login.async = function(username, password, continuation, error) {
    // LuCI RPC doesn't give us a session token so RPC login is f.useless 
    // for accessing the web interface  *sigh* 
    return rpc_async(this.url(), "login", [ username, password ], function(session) {
        //Set-Cookie: sysauth=7F6D11773AF2D11DFC308242C4E71A32; path=/cgi-bin/luci/;stok=15C3499EA0FDC5D342288A07A90CF643
        //console.error("GOTTA COOKIE: " + document.cookie);
        //console.error("sysauth: " + afrimesh.storage.cookie("sysauth"));
        //console.error("stok: " + afrimesh.storage.cookie("stok"));
        //console.error("path: " + afrimesh.storage.cookie("path"));
        continuation(session);
      }, error); 
  };
  

  /** - JSON/RPC helper functions --------------------------------------- */
  
  // only used for LuCI login - to be deprecated!
  
  /**
   * Invoke a JSON/RPC interface
   * @deprecate
   * @return XMLHttpRequest
   */
  var rpc = function(url, method, parameters) {   // TODO - don't require parameters to be in an array - rather use varargs!
    // TODO - check host & path
    var request = {
      url         : url, //"http://" + rpc.host + rpc.path, 
      type        : "POST",
      contentType : "application/json",
      dataType    : "json",
      async       : false
    };
    request.data = JSON.stringify({ 
        id      : 1234,
        version : "2.0",
        method  : method,
        params  : parameters
      });
    request.success = function(response, result) {
      if (!response) {
        console.error("JSON/RPC ERROR: " + JSON.stringify(request.data) + " - empty response");
      } else if (response.error) {
        console.error("JSON/RPC ERROR: " + JSON.stringify(request.data) + " - " + response.error);
      } else if (response.result) {
        request.result = response.result;
      } else {
        console.error("JSON/RPC ERROR: " + JSON.stringify(request.data) + " - unknown");
      }
    };
    var xml = $.ajax(request);
    return request.result;
  }; 

  /**
   * Invoke a JSON/RPC interface
   * @deprecate
   * @return XMLHttpRequest
   */
  function rpc_async(url, method, parameters, continuation, error) {
    var request = {
      url          :  url, //"http://" + rpc.host + rpc.path, 
      type         : "POST",
      contentType  : "application/json",
      dataType     : "json",
      async        : true,
      continuation : continuation,
    };
    request.data = JSON.stringify({ 
        id      : 1234,
        version : "2.0",
        method  : method,
        params  : parameters
      });
    request.error   = (typeof error == "undefined" ? rpc_async_error(request) : error);
    request.success = rpc_async_success(request, request.error);
    //console.debug("making request: " + show(request));
    return $.ajax(request);
  };

  function rpc_async_error(request) {
    return function(xhr, status, error) {
      console.error("FATAL JSON/RPC ERROR: " + JSON.stringify(request) + " - xhr: " + xhr + " status: " + status + " error: " + error);
    };
  };

  function rpc_async_success(request, error) {
    return function(response, result) {
      if (!response) {
        console.error("JSON/RPC ERROR: " + JSON.stringify(request.data) + " - empty response");
        error("empty response");
      } else if (response.error) {
        console.error("JSON/RPC ERROR: " + JSON.stringify(request.data) + " - " + response.error);
        error(response.error);
      } else if (response.result) {
        request.continuation(response.result);
      } else {
        console.error("JSON/RPC ERROR: " + JSON.stringify(request.data) + " - unknown error: " + JSON.stringify(response));
        error("unknown error");
      }
    };
  };
  

  return villagebus;
};
exports.BootVillageBus = BootVillageBus;
console.debug("loaded afrimesh.villagebus.js");


