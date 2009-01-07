/**
 * Afrimesh: easy management for B.A.T.M.A.N. wireless mesh networks
 * Copyright (C) 2008-2009 Meraka Institute of the CSIR
 * All rights reserved.
 *  
 * This software is licensed as free software under the terms of the
 * New BSD License. See /LICENSE for more information.
 */

var updateUpDown = function(sortable){
  $('dl:not(.ui-sortable-helper)', sortable)
  .removeClass('first').removeClass('last')
  .find('.up, .down').removeClass('disabled').end()
  .filter(':first').addClass('first').find('.up').addClass('disabled').end().end()
  .filter(':last').addClass('last').find('.down').addClass('disabled').end().end();
};
var emptyTrashCan = function(item){
  item.remove();
};
var sortableChange = function(e, ui){
  if(ui.sender){
    var w = ui.element.width();
    ui.placeholder.width(w);
    ui.helper.css("width",ui.element.children().width());
  }
};
var sortableUpdate = function(e, ui){
  if(ui.element[0].id == 'trashcan'){
    emptyTrashCan(ui.item);
  } else {
    updateUpDown(ui.element[0]);
    if(ui.sender)
      updateUpDown(ui.sender[0]);
  }
};


function mesh_internet_configure() {

  //$("#traffic-priority").tableDnD();

  var els = ['#traffic-priority'];
  var $els = $(els.toString());
  $els.sortable({
        items: '> dl',
        handle: 'dt',
        cursor: 'move',
        //cursorAt: { top: 2, left: 2 },
        //opacity: 0.8,
        //helper: 'clone',
        appendTo: 'body',
        //placeholder: 'clone',
        //placeholder: 'placeholder',
        connectWith: els,
        start: function(e,ui) {
          ui.helper.css("width", ui.item.width());
        },
        change: sortableChange,
        update: sortableUpdate
      });


  /*$("div#widget-internet-configure").html("loaded");*/

}
