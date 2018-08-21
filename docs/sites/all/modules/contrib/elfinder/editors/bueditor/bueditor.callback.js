/*
 * elFinder Integration
 *
 * Copyright (c) 2010-2018, Alexey Sukhotin. All rights reserved.
 */

function elfinder_bueditor_callback(url) {
  var img = jQuery('<img/>').attr('src', url).css('display', 'none').insertBefore(jQuery('#finder'));
  parent.jQuery("#bue-tgd-form input[name=attr_width]").val(img.width());
  parent.jQuery("#bue-tgd-form input[name=attr_height]").val(img.height());
  parent.BUE.imce.target.value = url;
  parent.BUE.popups['bue-imce-pop'].close();
}

function elfinder_bue_callback(url) {
  elfinder_bueditor_callback(url);
}


