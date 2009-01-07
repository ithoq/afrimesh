/**
 * Afrimesh: easy management for B.A.T.M.A.N. wireless mesh networks
 * Copyright (C) 2008-2009 Meraka Institute of the CSIR
 * All rights reserved.
 *  
 * This software is licensed as free software under the terms of the
 * New BSD License. See /LICENSE for more information.
 */

/* json commands */
var radius_radwho = { 
  package: "radius",  
  command: "who" 
};
var radius_list = { 
  package: "radius",  
  command: "list" 
};  
var radius_new = {
  package:  "radius",
  command:  "new",
  username: "",
  type:     "",
  seconds: 0
};
var radius_delete = {
  package: "radius",
  command: "delete",
  username: ""
};
var radius_modify = {
  package: "radius",
  command: "modify",
  username: "",
  new_username: "",
  new_password: "",
  new_type: ""
};



function mesh_customers_recharge() {
  $("div.button").hover(function() { $(this).addClass   ("hover"); },
                        function() { $(this).removeClass("hover"); });
  $("div.button-submit").hover(function() { if (!$("div.button").hasClass("selected")) return; else $(this).addClass   ("hover"); },
                               function() { $(this).removeClass("hover"); });
  $("div.button").click(function() {
      $("div.button-submit").removeClass("selected");
      $("div.button").removeClass("selected");
      $(this).addClass("selected");
    });
  $("div.button-submit").mousedown(function() {
      if (!$("div.button").hasClass("selected")) return;
      $(this).addClass("selected");
    });
  $("div.button-submit").mouseup(function() {
      $(this).removeClass("selected");
      // TODO - constructors for radius commands or maybe something else ?
      radius_new.username = "mesh";
      radius_new.type = "prepaid";
      radius_new.seconds = $("div#widget-customers-recharge div.button.selected").get(0).id;
      $.ajax({ 
          url: config.dashboard_host + "/cgi-bin/village-bus-radius",
            type: "POST", 
            contentType: "application/json", 
            processData: false,
            dataType: "json", data: $.toJSON(radius_new),
            success: 
          function(data) { 
            if (data.length < 1) { $("div#widget-customers-recharge").html("no data"); return; }
            var s = "";
            if (data[0].error) { s += "TODO: " + customer.error; }
            s += "<center><table>";
            s += "<tr><td><div class='voucher button'>";
            s += "<p><b>" + data[0].username + "</b><i>username</i></p>";
            s += "<p><b>" + data[0].password + "</b><i>password</i></p>";
            s += "</div></td></tr>";
            s += "<tr><td><div class='button-submit' id='menu-customers-recharge' style='float:center;'><p><b>print</b></p></div></td></tr>";
            s += "</table><center>";
            $("#widget-customers-recharge table").hide();
            $("#widget-customers-recharge").html(s);
            $(".voucher").corner("8px");
            $(".button-submit").corner("8px");
            $("div.button-submit").hover(function() { $(this).addClass   ("hover"); },
                                         function() { $(this).removeClass("hover"); });
            $("div.button-submit").click(function() { $("#menu-customers-recharge").click(); });
          }
        });
    });
}



function mesh_customers_watch() {
  $.ajax({ 
      url: config.dashboard_host + "/cgi-bin/village-bus-radius", 
        type: "POST", 
        contentType: "application/json", 
        processData: false,
        dataType: "json", data: $.toJSON(radius_radwho),
        success: 
      function(data) {
        if (data.length < 1) { $("div#widget-customers-watch").html("no data"); return; }
        var s = "";

        /* simple */
        s += "<table>";
        s += "<thead><tr><th>login</th><th>credit</th><th>station</th><th>status</th></tr></thead>";
        s += "<tbody>";
        for (id in data) {
          customer = data[id];
          s += "<tr class='" + customer.type + "'>";
          s += "<td>" + customer.username + "</td>";
          s += "<td>unlimited</td>";
          s += "<td>" + customer.caller_id + "</td>";
          s += "<td>" + ((customer.type == "login") ? "logged in" : "idle");
          s += " for " + Math.floor(customer.session_time / 60)  + " minutes</td>";
          s += "</tr>";
        }
        s += "</tbody>";
        s += "</table>";

        s += "<br/>";

        /* complete */
        /*s += "<table><thead><tr>";
        for (property in data[0]) {
          s += "<th>" + property + "</th>";
        }
        s += "</tr></thead>";
        for (id in data) {
          customer = data[id];
          if (customer.error) { s += "TODO: " + customer.error; continue; }
          s += "<tr>";
          for (property in customer) {
            s += "<td>" + customer[property] + "</td>";
          }
          s += "</tr>";
        }
        s += "</table>";*/

        $("div#widget-customers-watch").html(s);
        $("div#widget-customers-watch table").tablesorter(); 
      }
    });
}


/**
 *
 */


/**
 *
 */
function mesh_customers_manage() {
  this.on_customer_delete = function(username) {
    radius_delete.username = username;
    $.ajax({ 
        url: config.dashboard_host + "/cgi-bin/village-bus-radius", type: "POST", 
          contentType: "application/json",  dataType: "json", data: $.toJSON(radius_delete),
          success: 
        function(data) {
          if (data.length < 1) { $("div#widget-customers-manage").html("no data"); return; }
          $("#menu-customers-manage").click(); 
        }
        });
  };

  this.on_customer_password = function(id) {
    alert("pass:" + id);
  };

  this.on_customer_modify = function(value, settings) {
    //alert("id: " + this.id  + "\n" + settings.name + " : " + value);
    //alert(this.innerHTML + "\n" + this.innerText);

    // send update to village-bus-radius

    if (settings.name == "username") { // update other field ids to reflect new username
      $(".edit-type#" + this.id).get(0).id = value;
      this.id = value;
    }
    return value;
  };


  $.ajax({ 
      url: config.dashboard_host + "/cgi-bin/village-bus-radius", 
        type: "POST", 
        contentType: "application/json", 
        dataType: "json", data: $.toJSON(radius_list),
        success: 
      function(data) {
        if (data.length < 1) { $("div#widget-customers-manage").html("no data"); return; }
        var s = "<table>";
        s += "<thead><tr><th>username</th><th>type</th><th class='command'>command</th></tr></thead>";
        s += "<tbody>"
        for (id in data) {
          customer = data[id];
          s += "<tr>";
          s += "<td><div id='" + customer.username + "' class='edit-username'>" + customer.username + "</div></td>";
          s += "<td><div  id='" + customer.username + "' class='edit-type'>" + customer.type + "</div></td>";
          s += "<td class='command'><span onclick='on_customer_delete(\"" + customer.username + "\")' class='button'>delete</span>";
          s += "<span id='" + id + "' onclick='on_customer_password(id)' class='button'>password</span></td>";
          s += "</tr>";
        }
        s += "</tbody>";
        s += "<tfoot><tr id='new'><td colspan=3 class='command'><span id='new' class='button'>new</span></td></tr></tfoot>";
        s += "</table>";
        s += "<br/>";
        $("div#widget-customers-manage").html(s);
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
      }
    });
}



