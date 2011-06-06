

console.log("loaded: common");

exports.fail = function(message, module, status) {
  if (module) {
    console.error("module " + module + ": " + message);
  } else {
    console.log(message);
  }
  return { error : message, 
           status : status ? status : 200,
           module : module ? module : "unknown" };
};

