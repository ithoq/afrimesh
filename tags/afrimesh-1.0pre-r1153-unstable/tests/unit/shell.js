/**
 * xpcUnit: execute javascript unit tests from the command line
 * Copyright (C) 2007 http://7degrees.co.za
 * All rights reserved.
 *  
 * This software is licensed as free software under the terms of the
 * New BSD License. See /LICENSE for more information.
 */


/** 
 * Helper functions for running browser dependent Javascript from the command
 * line. 
 */
var Shell = {

  /** Replacement for the browser's main event loop. ---------------------- 
   *
   * See: http://mlblog.osdir.com/mozilla.devel.netlib/2005-02/msg00030.shtml
   *      http://archives.devshed.com/forums/mozilla-98/sleep-wait-from-xpcshell-1762836.html
   */
  process_events : true,
  queue : undefined,
  init_queue : function() {
    event_queue_service = Components.classes["@mozilla.org/event-queue-service;1"].getService(Components.interfaces.nsIEventQueueService);
    event_queue_service.createMonitoredThreadEventQueue();
    this.queue = event_queue_service.getSpecialEventQueue(event_queue_service.CURRENT_THREAD_EVENT_QUEUE);
  },
  main : function() {
    if (!this.queue) {
      this.init_queue();
    }
    try {
      this.process_events = true;
      //this.queue.eventLoop();
      while (this.process_events)
        this.queue.processPendingEvents();
      this.queue.processPendingEvents();
      this.queue.stopAcceptingEvents();
      this.queue.processPendingEvents();
      event_queue_service.destroyThreadEventQueue(this.queue);
    } catch (e) {
      print("Exception: ", e);
    }
  },


  /** Idle function ------------------------------------------------------- */
  sleep : function() { 
    if (!Shell.queue) {
      Shell.init_queue();
    }
    Shell.queue.processPendingEvents(); 
  },


  /** Terminate event loop ------------------------------------------------ */
  exit : function() {
    this.process_events = false;
  },


  /** XMLHttpRequest ------------------------------------------------------ 
   *
   *   State  Description
   *     0    The request is not initialized
   *     1    The request has been set up
   *     2    The request has been sent
   *     3    The request is in process
   *     4    The request is complete   
   */
  XMLHttpRequest : function () {
    /** obtain XMLHttpRequest class and create an instance */
    //var request = Components.classes["@mozilla.org/xmlextras/xmlhttprequest;1"].getService(Components.interfaces.nsIXMLHttpRequest);
    var request = Components.classes["@mozilla.org/xmlextras/xmlhttprequest;1"].createInstance(Components.interfaces.nsIXMLHttpRequest);
    /** force xpconnect to query object interface */
    //request instanceof Components.interfaces.nsIJSXMLHttpRequest;
    request.QueryInterface(Components.interfaces.nsIJSXMLHttpRequest).onload = null;
    return request;
  },


  /** setTimeout ---------------------------------------------------------- */
  setTimeout : function(callback, delay) {
    var timer = Components.classes["@mozilla.org/timer;1"].createInstance(Components.interfaces.nsITimer);
    // TODO test multiple concurrent timers
    timer.initWithCallback({ notify : function(timer) { callback(timer); } },
                           delay,
                           timer.TYPE_ONE_SHOT);
    return timer;
  },

}; /** Shell */





