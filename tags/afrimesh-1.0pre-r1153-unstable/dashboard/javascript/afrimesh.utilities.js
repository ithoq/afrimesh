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
  function car(array) { return (array.length > 0) ? array[0]       : []; };
  function cdr(array) { return (array.length > 1) ? array.slice(1) : []; };
  //Array.prototype.rac = function() { return (this.length > 0) ? this[this.length() - 1] : [] };
  //Array.prototype.rdc = function() { return (this.length > 1) ? this.slice(0, this.length() - 2) : [] };
  //Array.prototype.first = Array.prototype.car;
  //Array.prototype.rest  = Array.prototype.cdr;
  //Array.prototype.head  = Array.prototype.car;
  //Array.prototype.tail  = Array.prototype.cdr;
  //Array.prototype.init  = Array.prototype.rdc;
  //Array.prototype.last  = Array.prototype.rac;


  /** - Make Javascript a better Javascript ------------------------------- */
  function isObject(object) {
    return typeof object === "object";
  };
  // See: http://thinkweb2.com/projects/prototype/instanceof-considered-harmful-or-how-to-write-a-robust-isarray/
  //      http://ajaxian.com/archives/isarray-why-is-it-so-bloody-hard-to-get-right
  //      http://erik.eae.net/archives/2005/06/06/22.13.54/
  // Also: Consider a 'Objekt' that extends Object that I can derive my own classes from
  function isArray(object) {
    //return object.constructor == Array;
    //return toString.call(object) === "[object Array]";
    return Object.prototype.toString.call(object) === "[object Array]";
  };
  function isFunction(object) {
    return typeof object == 'function';
  };
  function isNumber(object) {
    return object instanceof Number;
  };
  function isDate(object) {
    return object instanceof Date;
  };
  function isString(object) {
    return typeof object === "string";
  };
  function isStringURL(object) {
    return isString(object) && 
        ((object.indexOf("http://") === 0) ||
         (object.indexOf("https://") === 0));
  };
  function isNetworkAddress(object) { // TODO - really cheap hack
    var ret = isString(object) && (object.split(".").length > 1);
    console.log("isNetworkAddress: " + object.split(".") + " " + ret);
    return ret;
  };
  function type(object) {
    if (isArray(object)) {
      return "Array";
    } else if (isFunction(object)) {
      return "Function";
    } else if (isNumber(object)) {
      return "Number";
    } else if (isDate(object)) {
      return "Date";
    } else if (isStringURL(object)) {
      return "URL";
    } else if (isString(object)) {
      return "String";
    } else if (isObject(object)) {
      return "Object";
    }
    return "Unknown";
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


  /**
   * { href, hostname, pathname }
   */
  function parseURL(url) {
    var a = document.createElement("a");
    a.href = url;
    return a;
  };

  
  /** - Query object graphs using simple selectors ------------------------ */
  var regex_nested_selector = new RegExp(/\([\w_\|\@\-\[\]]+\)|[\|\w\@\-\[\]]+/g);  // a|(nested|selector)|pattern -> (nested|selector)
  var regex_parse_nested    = new RegExp(/[\w\|\@\-\[\]]+/g);                       // (nested|selector)           -> nested|selector
  var regex_array_selector  = new RegExp(/^\@[\w\-]+\[\d+\]/g);                     // foo|@some-name[0]|bar       -> @some-name
  var regex_parse_selector  = new RegExp(/[\w\-\_]+/g);                             // @some-name[0]               -> some-name,0


  function Qsplit(this_object, selector, root) {
    // Evaluate nested selectors e.g.  network|(wireless|@wifi-iface[0]|device)|ipaddr 
    var selectors = selector.match(regex_nested_selector);
    selector = "";
    for (var i = 0; i < selectors.length; i++) { 
      var sub = selectors[i];
      if (sub.indexOf("(") == 0 && sub.indexOf(")") == sub.length-1) {
        var parsed = sub.match(regex_parse_nested);
        if (parsed.length == 1 && parsed[0] != undefined) {
          var result = Q(this_object, parsed[0], root);
          sub = result;
        } else {
          console.warn("Invalid sub selector: " + sub);
        }
      }
      selector += sub;
    }
    return selector.split(/\|/);
  };

  function Qsugar(selector, root) {     // SUGAR drop the first selector element if it is the same as the object name:
    if (car(selector).toLowerCase() == (root ? root : "afrimesh")) { // TODO UDE we don't have an automagic way to get root object names
      selector = cdr(selector);
    };
    return selector;
  };

  function Q(this_object, selector, root) {
    //console.debug("Q(" + this_object + ", " + selector + ", " + root + ")");
    if (!this_object) {
      return "NONE";
    } else  if (!selector.split) {
      console.warn("Q(" + this_object.valueOf() + "): Invalid type for selector - " + selector.valueOf());
      return undefined;
    }
    selector = Qsplit(this_object, selector, root);
    selector = Qsugar(selector, root);
    return __q(this_object, selector);
  };  

  function __q(this_object, selector) {
    if (!isArray(selector)) {
      console.warn("__q(" + this_object.valueOf() + "): Invalid type for selector - " + selector.valueOf());
      return undefined;
    } else if (selector.length == 0) {
      return this_object;
    } 
    var head = car(selector);
    if (regex_array_selector.test(head)) {               // test for @some-name[0]
      head = resolve_array_selector(this_object, head); 
    } 
    if (this_object[head] == undefined) {
      //console.warn("__q(" + this_object.valueOf() + "): Invalid selector - " + selector.join("|"));
      return undefined;
    } 
    return __q(this_object[head], cdr(selector));
  };

  function resolve_array_selector(this_object, selector) {
    var pair = selector.match(regex_parse_selector);
    if (pair.length != 2) {
      console.warn("Invalid array selector: " + selector);
      return undefined;
    }
    var type   = pair[0];
    var index  = parseInt(pair[1]);
    var count = 0;
    for (var entry in this_object) {
      candidate = this_object[entry];
      if (candidate._sectiontype == type && index == count) {
        return entry;
      } else if (candidate._sectiontype == type) {
        count++;
      }
    }
    console.warn("Unknown array selector: " + selector);
    return undefined;
  };

  function Qset(this_object, selector, value, root) {
    if (!selector.split) {
      console.warn("Qset(" + this_object.valueOf() + "): Invalid type for selector - " + selector.valueOf());
      return undefined;
    }
    selector = Qsplit(this_object, selector, root);
    selector = Qsugar(selector, root); // TODO - fix this, really, just fix it.
    var last = selector.pop();
    for (var i = 0; i < selector.length; i++) { // node in selector) {
      node = selector[i];
      if (!this_object[node]) {
        //console.warn("Qset(" + this_object.valueOf() + "): Creating node '" + node + "' for selector '" + selector.join("|") + "'");
        this_object[node] = {};
      }
      this_object = this_object[node];
    }
    if (value) {
      this_object[last] = value;
    }
    return this_object[last];
  };



  /** 
   * Pull in remote content - TODO - merge refactors from help.group.html
   */
  function load_remote_content(base, uri, src_selector, dest_selector) {
    var ajax_proxy = afrimesh.villagebus.ajax_proxy();
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
    var http_uri = ("http://" + afrimesh.settings.address + "/");
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
  function show(item) {
    var s = "";
    for (var p in item) {
      s += p + ": " + item[p] + "\t";
    }
    return s;
  }
  function rshow(item) {
    var s = "";
    for (var property in item) {
      if (isObject(item[property])) {
        s += property + " : { " + rshow(item[property]) + " } \n";
      } else {
        s += property + " : |" + item[property] + "|\t";
      }
    }
    return s;
  }
  function show_json(item) {
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
   * Various pretty printers
   */
  function pretty_bytes(bytes) {
    var onegig = Math.pow(1024, 3);
    var onemeg = Math.pow(1024, 2);
    var onekay = 1024;
    bytes = parseFloat(bytes);
    if (bytes > onegig) {
      return (bytes / onegig).toFixed(1) + " GB";
    } else if (bytes > onemeg) {
      return (bytes / onemeg).toFixed(1) + " MB";
    }
    return (bytes / onekay).toFixed(1) + " KB";
  };
  function pretty_seconds(seconds) {
    var minute = 60.0;
    var hour   = 60.0 * minute;
    var day    = hour * 24.0;
    if (seconds > day) {
      return Math.floor(seconds / day) + " days";
    } else if (seconds > hour) {
      return Math.floor(seconds / hour) + " hours";
    } else if (seconds > minute) {
      return Math.floor(seconds / minute) + " minutes";
    } 
    return Math.floor(seconds) + " seconds";
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
  } else if (typeof netscape != "undefined") { // not running in firebug, not safari - maybe netscape ?
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
  
  window.console.log = function(message) {
    /*message = "";// timestamp(true);
    message += "[log]\t";
    for (var arg = 0; arg < arguments.length; arg++) {
      message += arguments[arg];
    }*/
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


  /** - Make jQuery a usuable looped animations ------------------------- */
  jQuery.fn.looped = function(start, end, duration) {
    function loopfn(element, start, end, duration) {
      return $(element).animate(start, duration, "swing", function() {
          $(this).animate(end, duration, "swing", function() {
              //setTimeout(loopfn, duration);
              loopfn(element, start, end, duration);
            }); 
        });
    }
    return this.each(function() {
        return loopfn(this, start, end, duration);
      });
  };

  /** - http://remysharp.com/2007/01/25/jquery-tutorial-text-box-hints/ - */
  jQuery.fn.hint = function (blurClass) {
    if (!blurClass) { 
      blurClass = 'blur';
    }
    return this.each(function () {
      var $input = jQuery(this);
      var title  = $input.attr('title');
      var $form  = jQuery(this.form);
      var $win   = jQuery(window);
      function remove() {
        if ($input.val() === title && $input.hasClass(blurClass)) {
          $input.val('').removeClass(blurClass);
        }
      }
      if (title) {                // only apply logic if the element has the attribute
        $input.blur(function () { // on blur, set value to title attr if text is blank
          if (this.value === '') {
            $input.val(title).addClass(blurClass);
          }
        }).focus(remove).blur(); // now change all inputs to title
        $form.submit(remove);    // clear the pre-defined text when form is submitted
        $win.unload(remove);     // handles Firefox's autocomplete
      }
    });
  };

  // TODO - use form.submit to save forms so that fn.hint can do this for you
  jQuery.fn.hintval = function (value) {
    var $input = jQuery(this);
    if (!value) {
      var title  = $input.attr('title');
      var value  = $input.val();
      return (title == value) ? "" : value;
    }
    $input.val(value);
    $input.removeClass("blur");
  };


console.debug("loaded afrimesh.utilities.js");

