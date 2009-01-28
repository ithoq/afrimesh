
/**
 * Making Javascript a better Lisp
 */
Array.prototype.car = function() { return (this.length > 0) ? this[0]       : []; };
Array.prototype.cdr = function() { return (this.length > 1) ? this.slice(1) : []; };
Array.prototype.first = Array.prototype.car;
Array.prototype.rest  = Array.prototype.cdr;


/**
 * Making Javascript a better Javascript
 */
Object.prototype.isArray = function() {
  return this.constructor == Array;
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


/**
 * Query object graphs using simple selectors
 */
Object.prototype.Q = function(selector) {
  if (!selector.split) {
    console.warn("Q(" + this.valueOf() + "): Invalid type for selector - " + selector.valueOf());
    return undefined;
  }
  selector = selector.split(/ |\.|-|\|/);
  /** SUGAR drop the first selector element if it is the same as the object name */
  //var r = /[^function ][A-Z,0-9,$]*/i;
  //var this_name = this.constructor.toString().match(/[^function ][A-Z,0-9,$]*/i).toString();
  //console.log("name: |" + this_name + "|");
  if (selector.car().toLowerCase() == "afrimesh") { // TODO UDE we don't have a friendly way to get object names
    selector = selector.cdr();
  };
  return this.__q(selector);
};
Object.prototype.__q = function(selector) {
  //return (selector.length > 0) ? this[selector.car()].__q(selector.cdr()) : this;
  if (!selector.isArray()) {
    console.warn("__q(" + this.valueOf() + "): Invalid type for selector - " + selector.valueOf());
    return undefined;
  } else if (selector.length == 0) {
    return this;
   } else if (this[selector.car()] == undefined) {
    console.warn("__q(" + this.valueOf() + "): Invalid selector - " + selector.join("|"));
    return undefined;
  } 
  return this[selector.car()].__q(selector.cdr());
};


/**
 * Dump the object contents to a string
 */
function dump_object(o, follow_properties) {
  var s = "";
  s += o + "\n";
  for (var p in o) {
    s += p + " : " + p[o] + "\n";
  }
  console.info(s);
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
 */
if (typeof window === "undefined") {  // running from the command line
  var console = { };
  console.native_log   = print; 
  console.native_info  = print;
  console.native_warn  = print;
  console.native_error = print; 
} else if (window.console === undefined) { // not running in firebug, not safari
  // assuming firefox , TODO - we need to do some decent platform detection at some point - jquery ?
  var console = { };
  netscape.security.PrivilegeManager.enablePrivilege('UniversalXPConnect'); // argh!
  var console_service = Components.classes['@mozilla.org/consoleservice;1'].getService(Components.interfaces.nsIConsoleService);
  console.native_log = function(message) {
    netscape.security.PrivilegeManager.enablePrivilege('UniversalXPConnect'); 
    console_service.logStringMessage(message);
  }; 
  console.native_error = function(message) {
    netscape.security.PrivilegeManager.enablePrivilege('UniversalXPConnect');
    Components.utils.reportError(message);
  };
  console.native_info = console.native_log;
  console.native_warn = console.native_log;
} else {  // running in safari, firebug or some other browser with console support
  console.native_log   = console.log;
  console.native_info  = console.info;
  console.native_warn  = console.warn;
  console.native_error = console.error;
}
console.debug_output = true;

console.page = function(message) { // output the message to a page div with id=console
  var element = document.getElementById("console");
  if (element) {
    var s = element.innerHTML;
    s += ("<code>" + message + "</code><br>");
    element.innerHTML = s;
  }
};

console.log = function() {
  var message = "";// timestamp(true);
  message += "[log]\t";
  for (var arg = 0; arg < arguments.length; arg++) {
    message += arguments[arg];
  }
  console.native_log(message);
};

console.info = function() {
  var message = "";// timestamp(true);
  message += "[info]\t";
  for (var arg = 0; arg < arguments.length; arg++) {
    message += arguments[arg];
  }
  console.native_info(message);
};

console.warn = function() {
  var message = "";// timestamp(true);
  message += "[warn]\t";
  for (var arg = 0; arg < arguments.length; arg++) {
    message += arguments[arg];
  }
  console.native_warn(message);
};

console.error = function() {
  var message = "";//timestamp();
  message += "[error]\t";
  for (var arg = 0; arg < arguments.length; arg++) {
    message += arguments[arg] + " ";
  }
  console.native_error(message);
};

console.debug = function() {
  if (!console.debug_output) { return; }
  var message = "";//timestamp();
  message += "[debug]\t";
  for (var arg = 0; arg < arguments.length; arg++) {
    message += arguments[arg] + " ";
  }
  console.native_log(message);
};

/**
console.log("this is a log");
console.info("this is an info");
console.warn("this is a warn");
console.error("this is an error");
console.debug("this is a debug");
*/


console.debug("loaded afrimesh.util.js");
