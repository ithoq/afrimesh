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
    afrimesh.customers().map(function(customer) {
        s += "<tr>";
        s += "<td><div id='" + customer.username + "' class='edit-username'>" + customer.username + "</div></td>";
        s += "<td><div id='" + customer.username + "' class='edit-type'>" + customer.type + "</div></td>";
        s += "<td class='command'><span onclick='on_customer_delete(\"" + customer.username + "\")' class='button'>delete</span>";
        s += "<span id='" + customer.username + "' onclick='on_customer_password(id)' class='button'>password</span></td>";
        s += "</tr>";
      });
    s += "</tbody>";
    s += "<tfoot><tr id='new'><td colspan=3 class='command'><span id='new' class='button'>new</span></td></tr></tfoot>";
    s += "</table>";
    s += "<br/>";
    $("div#widget-customers-manage").html(s)


    $(".edit-username").editable(on_customer_modify, { name : 'username',
          type   : "text",
          onblur : "submit",
          select : true,
          indicator : '<img src="widgets/indicator.gif">'  });
    $(".edit-type").editable(on_customer_modify, { name : 'type',
          type   : "select",
          //data   : "{'disabled':'disabled','metered':'metered','prepaid':'prepaid','flatrate':'flatrate'}",
          data   : "{'disabled':'disabled','prepaid':'prepaid','flatrate':'flatrate'}",
          indicator : '<img src="widgets/indicator.gif">',
          onblur : "submit",
          style  : "inherit" });
    $("div#widget-customers-manage table").tablesorter(); 

    /* button logic */
    $(".button").corner("4px");
    $(".button").mousedown(function() { $(this).addClass("selected"); });
    $(".button").mouseup(function() { $(".button").removeClass("selected"); });

    $("div#widget-customers-manage .button#new").click(function() { 
        $("div#widget-customers-manage table tr#new").html($("#widget-customers-new table tr#save").html());
        $("div#widget-customers-manage input#username").focus();
        $(".button").corner("4px");
        $(".button").mousedown(function() { $(this).addClass("selected"); });
        $(".button").mouseup(function() { $(".button").removeClass("selected"); });
        $("div#widget-customers-manage .button#new").click(function() { 
            radius_new.username = $("#widget-customers-manage input#username").val();
            radius_new.type = "flatrate";
            $.ajax({ 
                url: config.dashboard_host + "/cgi-bin/village-bus-radius",
                  type: "POST", 
                  contentType: "application/json", 
                  processData: false,
                  dataType: "json", data: $.toJSON(radius_new),
                  success: 
                function(data) { 
                  /*alert(data[0].foo);*/
                  $("#menu-customers-manage").click(); 
                }
              });
          });
      });
  }; /* populate customers */

  function on_customer_modify(value, settings) {
    //alert("id: " + this.id  + "\n" + settings.name + " : " + value);
    //alert(this.innerHTML + "\n" + this.innerText);
    // send update to village-bus-radius
    if (settings.name == "username") { // update other field ids to reflect new username
      $(".edit-type#" + this.id).get(0).id = value;
      this.id = value;
    }
    return value;
  };


  
  
})();