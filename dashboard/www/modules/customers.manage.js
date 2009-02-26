/*
 * Afrimesh: easy management for B.A.T.M.A.N. wireless mesh networks
 * Copyright (C) 2008-2009 Meraka Institute of the CSIR
 * All rights reserved.
 *  
 * This software is licensed as free software under the terms of the
 * New BSD License. See /LICENSE for more information.
 */


var populate_control = undefined;
(function() {
  
  /** 
   * Query the villagebus interface to radius for all customers and populate the
   * customer management interface with the result.
   */
  populate_control = function() {
    var s = "<table>";
    s += "<thead><tr><th>username</th><th>type</th><th class='command'>command</th></tr></thead>";
    s += "<tbody>";
    afrimesh.customers.select().map(function(customer) {
        s += "<tr>";
        s += "<td><div id='" + customer.username + "' class='edit-username'>" + customer.username + "</div></td>";
        s += "<td><div id='" + customer.username + "' class='edit-type'>" + customer.type + "</div></td>";
        s += "<td class='command'>";
        s += "<span id='" + customer.username + "' class='button delete'>delete</span>";
        //s += "<span id='" + customer.username + "' class='button password'>password</span>";
        s += "</td>";
        s += "</tr>";
      });
    s += "</tbody>";
    s += "<tfoot><tr id='new'><td colspan=3 class='command'><span id='new' class='button'>new</span></td></tr></tfoot>";
    s += "</table>";
    s += "<br/>";
    $("div#widget-customers-manage").html(s)

    /** wire event handlers */
    $(".edit-username").editable(on_customer_update, { name : 'username',
          type   : "text",
          onblur : "submit",
          select : true,
          indicator : '<img src="images/indicator.gif">'  });
    $(".edit-type").editable(on_customer_update, { name : 'type',
          type   : "select",
          //data   : "{'disabled':'disabled','metered':'metered','prepaid':'prepaid','flatrate':'flatrate'}",
          data   : "{'disabled':'disabled','prepaid':'prepaid','flatrate':'flatrate'}",
          indicator : '<img src="images/indicator.gif">',
          onblur : "submit",
          style  : "inherit" });
    $(".button.delete").click(function()   { on_customer_remove(this.id);   });
    $(".button.password").click(function() { on_customer_password(this.id); });
    $("#new .button").click(on_customer_new);
    $("div#widget-customers-manage table").tablesorter(); 

    /** button logic - TODO - move to afrimesh.ui.js */
    $(".button").corner("4px");
    $(".button").mousedown(function() { $(this).addClass("selected"); });
    $(".button").mouseup(function() { $(".button").removeClass("selected"); });

  }; /* populate customers */


  /**
   * Create the ui for entering a new customer record
   */
  function on_customer_new(username) {
    $("div#widget-customers-manage table tr#new").html($("#widget-customers-new table tr#new_active").html());
    $("div#widget-customers-manage input#username").focus();
    $("#save").click(on_customer_save);
    $(".button").corner("4px"); // TODO - json 3 added functionality to only have to do this once 
    $(".button").mousedown(function() { $(this).addClass("selected"); });
    $(".button").mouseup(function() { $(".button").removeClass("selected"); });
    console.debug("display_customer_insert");
  };

  /**
   * Callback to insert new customer
   */
  function on_customer_save() {
    var username = $("#widget-customers-manage input#username").val();
    var ret = afrimesh.customers.generate(username, "flatrate", 0);
    console.debug("Inserted: " + ret);
    $("ul#menu ul#customers li#manage").click();
  };


  /**
   * Callback to commit modified customer information
   */
  function on_customer_update(new_value, settings) {
    var username = this.id;
    if (settings.name == "username") { 
      $("#" + username).each(function(e) {  // update all elements to new username - TODO this needs to be tested
          this.id = new_value; 
        });
      afrimesh.customers.update.username(username, new_value);
    } else if (settings.name == "type") {
      afrimesh.customers.update.type(username, new_value);
    }
    return new_value;
  };


  /**
   * Callback to remove customer 
   */
  function on_customer_remove(username) {
    console.debug("Removing: " + username);
    var ret = afrimesh.customers.remove(username);
    console.debug("Removed: " + ret);
    $("ul#menu ul#customers li#manage").click();
  };
 

  /**
   * Callback to change customer password
   */
  function on_customer_password(username) {
    console.debug("Changing password for: " + username);
  };
  
})();