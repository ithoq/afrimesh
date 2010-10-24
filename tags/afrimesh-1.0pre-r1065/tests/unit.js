/**
 * xpcUnit: execute javascript unit tests from the command line
 * Copyright (C) 2007 http://7degrees.co.za
 * All rights reserved.
 *  
 * This software is licensed as free software under the terms of the
 * New BSD License. See /LICENSE for more information.
 */


/**
 * Unit testing
 *
 * See: http://clarkware.com/articles/JUnitPrimer.html
 */
var Unit = {

  /** Logging ------------------------------------------------------------- */
  log_buffer : "",
  reset_log : function() { this.log_buffer = ""; },
  dump_log : function() { print(this.log_buffer); },
  log : function() {
    //print("\tLOG: " + arguments.callee + " " + arguments.join(" ")); // TODO fix
    this.log_buffer += "\t";
    for (arg = 0; arg < arguments.length; arg++) {
      this.log_buffer += arguments[arg] + "";
    }
    this.log_buffer += "\n"; 
  },

  /** TestRunner ---------------------------------------------------------- */
  TestRunner : {
    count_suites  : 0,
    count_tests   : 0,
    count_success : 0,
    run : function() {
      for (i = 0; i < arguments.length; i++) {
        this.count_suites++;
        suite = arguments[i];
        for (test in suite) {
          //print("[", suite, "] Testing: ", test); // UDE We can't get object names when they are passed as arguments to a function
          //print("\t[", test, "]"); // UDE We can't get object names when they are passed as arguments to a function
          dump("[" + test + "]");
          this.count_tests++;
          try {
            Unit.reset_log();
            if (!suite[test]()) {
              print(" -> FAIL");
            } else {
              print(" -> SUCCESS");
              this.count_success++;
            }
          } catch (e) {
            print(" -> EXCEPTION: '", e + "'");
          }
          Unit.dump_log();
        }
      }
      print("\nPASSED " + this.count_success + "/" + this.count_tests + " TEST CASES IN " + this.count_suites + " TEST SUITES."); 
    }
  }
     

}; /** Unit */


