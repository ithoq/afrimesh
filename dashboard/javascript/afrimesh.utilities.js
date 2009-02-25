/*
 * Afrimesh: easy management for B.A.T.M.A.N. wireless mesh networks
 * Copyright (C) 2008-2009 Meraka Institute of the CSIR
 * All rights reserved.
 *  
 * This software is licensed as free software under the terms of the
 * New BSD License. See /LICENSE for more information.
 */
//(function() {

  /** - Make Javascript a better Lisp ------------------------------------- */
  Array.prototype.car = function() { return (this.length > 0) ? this[0]       : []; };
  Array.prototype.cdr = function() { return (this.length > 1) ? this.slice(1) : []; };
  Array.prototype.first = Array.prototype.car;
  Array.prototype.rest  = Array.prototype.cdr;
  Array.prototype.head  = Array.prototype.car;
  Array.prototype.tail  = Array.prototype.cdr;
  
  /** - Make Javascript a better Javascript ------------------------------- */
  // See: http://thinkweb2.com/projects/prototype/instanceof-considered-harmful-or-how-to-write-a-robust-isarray/
  //      http://ajaxian.com/archives/isarray-why-is-it-so-bloody-hard-to-get-right
  //      http://erik.eae.net/archives/2005/06/06/22.13.54/
  // Also: Consider a 'Objekt' that extends Object that I can derive my own classes from
  function isArray(object) {
    //return object.constructor == Array;
    //return toString.call(object) === "[object Array]";
    return Object.prototype.toString.call(object) === "[object Array]";
  };
  String.prototype.trim = function() {
    return this.replace(/^\s+|\s+$/g,"");
  };
  String.prototype.ltrim = function() {
    return this.replace(/^\s+/g,"");
  };
  String.prototype.rtrim = function() {
    return this.replace(/\s+$/g,"");
  };
  function extend(object_1, object_2) {
    for (var property in object_2) {
      object_1[property] = object_2[property];
    }
    return object_1;
  };
  
  /** - Query object graphs using simple selectors ------------------------ */
  //Object.prototype.Q = function(selector) {
  function Q(this_object, selector) {
    if (!selector.split) {
      console.warn("Q(" + this_object.valueOf() + "): Invalid type for selector - " + selector.valueOf());
      return undefined;
    }
    selector = selector.split(/ |\.|-|\|/);
    // SUGAR drop the first selector element if it is the same as the object name:
    if (selector.car().toLowerCase() == "afrimesh") { // TODO UDE we don't have a friendly way to get object names
      selector = selector.cdr();
    };
    //return this_object.__q(selector);
    return __q(this_object, selector);
  };
  //Object.prototype.__q = function(selector) {  // TODO these are being called by DOMWindow!? and causing all kinds of problems
  function __q(this_object, selector) {
    //return (selector.length > 0) ? this[selector.car()].__q(selector.cdr()) : this;
    //if (!selector.isArray()) {
    if (!isArray(selector)) {
      console.warn("__q(" + this_object.valueOf() + "): Invalid type for selector - " + selector.valueOf());
      return undefined;
    } else if (selector.length == 0) {
      return this_object;
    } else if (this_object[selector.car()] == undefined) {
      console.warn("__q(" + this_object.valueOf() + "): Invalid selector - " + selector.join("|"));
      return undefined;
    } 
    //return this[selector.car()].__q(selector.cdr());
    return __q(this_object[selector.car()], selector.cdr());
  };


  /** 
   * Pull in remote content - TODO - merge refactors from help.group.html
   */
  function load_remote_content(base, uri, src_selector, dest_selector) {
    var ajax_proxy = "http://" + afrimesh.settings.hosts.dashboard_server + afrimesh.settings.ajax_proxy;
    $(dest_selector).load(ajax_proxy + base + "/" + uri + " " + src_selector,
                          null,
                          function() {
                              rewrite_urls(base, src_selector, dest_selector);
                            });
  };
  function rewrite_urls(base, src_selector, dest_selector) {
    $("a").click(function() {
        var uri = url_to_uri(this.href);
        console.debug("Rewrote url: " + base + "/" + uri);
        load_remote_content(base, uri, src_selector, dest_selector);
        return false;
      });
  };  
  function url_to_uri(url) {
    var file_uri = ("file:///");
    var http_uri = ("http://" + afrimesh.settings.hosts.dashboard_server + "/");
    var uri = url;
    if (uri.indexOf(file_uri) == 0) {
      uri = uri.replace(file_uri, "");
    } else if (uri.indexOf(http_uri) == 0) {
      uri = uri.replace(http_uri, "");
    }
    return uri;
  }

  
  
  /**
   * Dump the object contents to a string
   */
  function dump_object(o, follow_properties) {
    var s = "";
    s += o + "\n";
    for (var p in o) {
      s += p + " : " + o[p] + "\n";
    }
    return s;
  };
  function pretty_print(item) {
    var s = "";
    for (var p in item) {
      s += p + ": " + item[p] + "\t";
    }
    return s;
  }
  function pretty_print_json(item) {
    var s = "{ ";
    var first_property = true;
    for (var p in item) {
      s += (first_property ? "" : ", ") + p + ": " + item[p];
      first_property = false;
    }
    s += " }";
    return s;
  };
  
  
  /**
   * Generate a timestamp string
   */
  function timestamp(dated) {
    var now = new Date();
    var ret = "";
    if (dated) {
      ret = now.getDate() + "/" + now.getMonth()+1 + "/" + now.getFullYear() + " ";
    }
    ret += now.getHours() + ":" + now.getMinutes();
    return ret;
  };
  
  /**
   * Cross-platform console functions
   *
   * Also see: http://stackoverflow.com/questions/55633/where-is-the-console-api-for-webkitsafari
   *           http://code.google.com/p/fbug/source/browse/trunk/lite/firebug.js?r=366
   */
  //try { window; } catch (e) { var window = undefined; } // TODO - breaks firefox
  if (typeof window === "undefined") {  // running from the command line
    window = {};
    window.console = { };
    window.console.native_log   = print; 
    window.console.native_info  = print;
    window.console.native_warn  = print;
    window.console.native_error = print; 
  } else if (!window.console) { // not running in firebug, not safari
    // assuming firefox , TODO - we need to do some decent platform detection at some point - jquery ?
    window.console = { };
    try {
      netscape.security.PrivilegeManager.enablePrivilege('UniversalXPConnect'); // argh!
      var console_service = Components.classes['@mozilla.org/consoleservice;1'].getService(Components.interfaces.nsIConsoleService);
      window.console.native_log = function(message) {
        netscape.security.PrivilegeManager.enablePrivilege('UniversalXPConnect'); 
        console_service.logStringMessage(message);
      }; 
      window.console.native_error = function(message) {
        netscape.security.PrivilegeManager.enablePrivilege('UniversalXPConnect');
        Components.utils.reportError(message);
      };
      window.console.native_info = console.native_log;
      window.console.native_warn = console.native_log;
    } catch (e) { // firefox only lets us do this if we're a file:///
      window.console.native_log   = function() {};
      window.console.native_info  = console.native_log;
      window.console.native_warn  = console.native_log;
      window.console.native_error = console.native_log;
    }
  } else {  // running in safari, firebug or some other browser with console support
    window.console.native_log   = console.log;
    window.console.native_info  = console.info;
    window.console.native_warn  = console.warn;
    window.console.native_error = console.error;
  }
  window.console.debug_output = true;
  
  window.console.page = function(message) { // output the message to a page div with id=console
    var element = document.getElementById("console");
    if (element) {
      var s = element.innerHTML;
      s += ("<code>" + message + "</code><br>");
      element.innerHTML = s;
    }
  };
  
  window.console.log = function() {
    var message = "";// timestamp(true);
    message += "[log]\t";
    for (var arg = 0; arg < arguments.length; arg++) {
      message += arguments[arg];
    }
    return window.console.native_log(message);
  };
  
  window.console.info = function() {
    var message = "";// timestamp(true);
    message += "[info]\t";
    for (var arg = 0; arg < arguments.length; arg++) {
      message += arguments[arg];
    }
    return window.console.native_info(message);
  };
  
  window.console.warn = function() {
    var message = "";// timestamp(true);
    message += "[warn]\t";
    for (var arg = 0; arg < arguments.length; arg++) {
      message += arguments[arg];
    }
    return window.console.native_warn(message);
  };
  
  window.console.error = function() {
    var message = "";//timestamp();
    message += "[error]\t";
    for (var arg = 0; arg < arguments.length; arg++) {
      message += arguments[arg] + " ";
    }
    return window.console.native_error(message);
  };
  
  window.console.debug = function() {
    if (!console.debug_output) { return ""; }
    var message = "";//timestamp();
    message += "[debug]\t";
    for (var arg = 0; arg < arguments.length; arg++) {
      message += arguments[arg] + " ";
    }
    return window.console.native_log(message);
  };
  
//})();  
//if (!console && window.console) {
  console = window.console; //TODO CLEAN 
//}
console.debug("loaded afrimesh.utilities.js");

