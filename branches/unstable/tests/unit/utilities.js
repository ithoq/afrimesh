

/** - Utilities ----------------------------------------------------------- */
function dump_object(o) {
  print("\n+================================================");
  print("| " + o);
  print("+- properties -----------------------------------");
  for (var p in o) {
    try {
      print("| " + p + " : " + o[p]);
    } catch (e) {
      print("| " + p + " : " + e);
    }
  }
  if (o.prototype) {
    print("+- prototype ------------------------------------");
    for (var p in o.prototype) {
      print("| prototype." + p + " : " + o.prototype[p]);
    }
  }
  print("+================================================\n");
};




/** 
 * Wanted:
 *
 *   . memoization  - http://blog.stevenlevithan.com/archives/timed-memoization
 *   . functors     - http://dev.zeraweb.com/functors-in-javascript
 */
function memoize (functor, expiration) {
  var memo = {};
  return function () {
    var key = Array.prototype.join.call(arguments, "§");
    if (key in memo)
      return memo[key];
    if (expiration)
      setTimeout(function () {delete memo[key];}, expiration);
    return memo[key] = functor.apply(this, arguments);
  };
};

