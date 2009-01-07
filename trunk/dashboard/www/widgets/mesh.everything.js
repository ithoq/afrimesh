/**
 * Afrimesh: easy management for B.A.T.M.A.N. wireless mesh networks
 * Copyright (C) 2008-2009 Meraka Institute of the CSIR
 * All rights reserved.
 *  
 * This software is licensed as free software under the terms of the
 * New BSD License. See /LICENSE for more information.
 */


var traffic_graph_max_in  = 512; /* TODO pull from uci - max observed is 430 */
var traffic_graph_max_out = 256; /* TODO pull from uci - max observed is 228 */
var traffic_graph_convert = 9.4;  /* convert from octets to kbps allowing for layer-2 headers etc. */
var traffic_graph_points = 30; /* TODO What would be really cool would be able to add these as props to traffic_graph - can we do this in js ? */
var traffic_graph_update_interval = 5000;
var traffic_graph_in  = [];
var traffic_graph_out = [];
for (var i = 0; i < traffic_graph_points; i++) {
  traffic_graph_in.push(0);
  traffic_graph_out.push(0);
}
traffic_graph_in[0] = traffic_graph_max_in; 
traffic_graph_out[0] = traffic_graph_max_out; 
var last_time = new Date();
var last_in = 0;
var last_out = 0;
var traffic_graph_timer;

/**

  TEST:

  export REQUEST_METHOD="GET" ; export QUERY_STRING="{ \"address\" : \"192.168.20.1\", \"community\" : \"public\" }"; ./village-bus-snmp

 */


var update_traffic_graph = function() {
  
  clearTimeout(traffic_graph_timer);
  $.ajax({ 
      url: config.router_host + "/cgi-bin/village-bus-snmp", 
        type: "POST", 
        contentType: "application/json", 
        processData: false,
        dataType: "json", data: $.toJSON(snmp_traffic),
        success: 
      function(data) {
        if (data.length != 1) { $("div#traffic-internet").html("no data"); return; }
        if (data[0].error) { 
          $("#traffic-internet-busy").html(data[0].error);
          return;
        }
        var snmp = data[0];
        /*var foo = ""; for (key in snmp) { foo+= key + "|" + snmp[key] + "\n"; } alert(foo);*/
        var now_time = new Date();
        if (last_in == 0 || 
            last_time.getTime() > now_time.getTime() || 
            last_in  > snmp.iso_3_6_1_2_1_2_2_1_10_4 || 
            last_out > snmp.iso_3_6_1_2_1_2_2_1_16_4) {
          last_time = now_time;
          last_in = snmp.iso_3_6_1_2_1_2_2_1_10_4;
          last_out = snmp.iso_3_6_1_2_1_2_2_1_16_4;
          return;
        }
        var interval = now_time.getTime() - last_time.getTime();
        var traffic_in = ((snmp.iso_3_6_1_2_1_2_2_1_10_4 - last_in) / interval) * traffic_graph_convert;
        var traffic_out = ((snmp.iso_3_6_1_2_1_2_2_1_16_4 - last_out) / interval) * traffic_graph_convert;
        last_time = now_time;
        last_in = snmp.iso_3_6_1_2_1_2_2_1_10_4;
        last_out = snmp.iso_3_6_1_2_1_2_2_1_16_4;
        traffic_graph_in.push(traffic_in);
        traffic_graph_out.push(traffic_out);
        if (traffic_graph_in.length > traffic_graph_points ) {
          traffic_graph_in.splice(2, traffic_graph_in.length - traffic_graph_points); // leave 0 for scaling
        }
        if (traffic_graph_out.length > traffic_graph_points) {
          traffic_graph_out.splice(2, traffic_graph_out.length - traffic_graph_points);
        }
        
        $("#traffic-internet").sparkline(traffic_graph_in,  { composite: false, lineColor: "black", spotColor: false, minSpotColor: false, maxSpotColor: false, fillColor: false });
        $("#traffic-internet").sparkline(traffic_graph_out, { composite: true,  lineColor: "gray",  spotColor: false, minSpotColor: false, maxSpotColor: false, fillColor: false });
        $("#traffic-internet-in").html("" + Math.floor(traffic_in));
        $("#traffic-internet-out").html("" + Math.floor(traffic_out));
        $("#traffic-internet-in-max").html(traffic_graph_max_in);
        $("#traffic-internet-out-max").html(traffic_graph_max_out);
        
        var n = Math.max(traffic_in / traffic_graph_max_in, traffic_out / traffic_graph_max_out);
        var busy = ""; //"n: " + n + "  ";
        if (n < 0.2) {
          busy += "<span style=\"color:green;\">relaxed</span>";
        } else if (n < 0.75) {
          busy += "<span style=\"color:green;\">normal</span>";
        } else if (n < 0.90) {
          busy += "<span style=\"color:orange;\">busy</span>";
        } else {
          busy += "<span style=\"color:red;\">full</span>";
        }
        $("#traffic-internet-busy").html(busy);
        
      }
    });
  traffic_graph_timer = setTimeout(update_traffic_graph, traffic_graph_update_interval);
}
  
