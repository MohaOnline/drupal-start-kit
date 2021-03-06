<?php
/**
 * elFinder Integration
 *
 * Copyright (c) 2010-2020, Alexey Sukhotin. All rights reserved.
 */

/**
 * @file
 * file manager admin settings page
 */


/**
 * Settings form callback
 */
function elfinder_admin() {
  $obj = drupal_get_form('elfinder_d7_admin_form');
  return drupal_render($obj);
}

/**
 * Settings form definition for D7
 */
function elfinder_d7_admin_form($form_id, &$form_state) {
  return elfinder_admin_form($form_state);
}


function elfinder_admin_profile_links($profile_name) {
  $links = l(t('Edit'), 'admin/config/media/elfinder/profile/' . $profile_name . '/edit') . ' ' . l(t('Delete'), 'admin/config/media/elfinder/profile/' . $profile_name . '/delete');
  return $links;
}


/**
 * Settings form definition
 */
function elfinder_admin_form(&$form_state) {
  global $user, $language;
  $path = drupal_get_path('module', 'elfinder');
  // require_once $path .'/inc/' . 'elfinder.admin.profiles.inc';

  $langCode = isset($language->language) ? $language->language : 'en';

  $form['profiles'] = array(
    '#type' => 'fieldset',
    '#title' => t('Profiles'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  $profiles = elfinder_admin_profile_get();

  $roles = user_roles();

  $rows = array();

  if ($profiles) {
    if (is_array($profiles)) {
      foreach ($profiles as $profile) {
        $rows[] = array($profile->pid, $profile->name, $profile->description, implode(', ', array_intersect_key($roles, $profile->settings['profile_role'])), elfinder_admin_profile_links($profile->name));
      }
    } else {
      $profile = $profiles;
      $rows[] = array($profile->pid, $profile->name, $profile->description, implode(', ', array_intersect_key($roles, $profile->settings['profile_role'])), elfinder_admin_profile_links($profile->name));
    }

  }

  $profile_table = theme('table', array(
    'header' => array(t('Id'), t('Name'), t('Description'), t('Role'), ''),
    'rows' => $rows
  ));

  $addprofile = l(t('Add profile'), 'admin/config/media/elfinder/profile');

  $form['profiles']['profile_list'] = array(
    '#type' => 'markup',
    '#markup' => "$profile_table<br/>$addprofile",
  );

  $form['filesystem_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('File system settings'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );


  $form['filesystem_settings']['filesystem_public_root_label'] = array(
    '#prefix' => '<div class="custom-container">',
    '#suffix' => '</div>',
    '#type' => 'textfield',
    '#title' => t('Public files root directory label'),
    '#default_value' => variable_get('elfinder_settings_filesystem_public_root_label', ''),
    '#description' => t('Root directory label in directory tree'),
  );

  $form['filesystem_settings']['filesystem_private_root_label'] = array(
    '#prefix' => '<div class="custom-container">',
    '#suffix' => '</div>',
    '#type' => 'textfield',
    '#title' => t('Private files root directory label'),
    '#default_value' => variable_get('elfinder_settings_filesystem_private_root_label', ''),
    '#description' => t('Root directory label in directory tree'),
  );

  $form['filesystem_settings']['filesystem_unmanaged_root_label'] = array(
    '#prefix' => '<div class="custom-container">',
    '#suffix' => '</div>',
    '#type' => 'textfield',
    '#title' => t('Unmanaged files root directory label'),
    '#default_value' => variable_get('elfinder_settings_filesystem_unmanaged_root_label', ''),
    '#description' => t('Root directory label in directory tree'),
  );

  $form['filesystem_settings']['filesystem_root_custom'] = array(
    '#prefix' => '<div class="custom-container">',
    '#suffix' => '</div>',
    '#type' => 'textfield',
    '#title' => t('Custom unmanaged files path'),
    '#default_value' => variable_get('elfinder_settings_filesystem_root_custom', ''),
    '#description' => t('Custom filesystem root path.') . '<br/>' . t('Available tokens: <code>%files</code> (base path, eg: <code>/</code>), <code>%name</code> (current username, eg: <code>@u</code>, <b>NOTE:</b> it is not unique - users can have same username, so better to combine it with user id value), <code>%uid</code> (current user id, eg: <code>@uid</code>), <code>%lang</code> (current language code, eg: <code>@lang</code>), plus all tokens provided by token module', array('@u' => $user->name, '@uid' => $user->uid, '@lang' => $langCode)),
  );

  $form['filesystem_settings']['filesystem_url_custom'] = array(
    '#prefix' => '<div class="custom-container">',
    '#suffix' => '</div>',
    '#type' => 'textfield',
    '#title' => t('Custom unmanaged files URL'),
    '#default_value' => variable_get('elfinder_settings_filesystem_url_custom', ''),
    '#description' => t('Custom filesystem url.') . '<br/>' . t('Available tokens: <code>%files</code> (base path, eg: <code>/</code>), <code>%name</code> (current username, eg: <code>@u</code>, <b>NOTE:</b> it is not unique - users can have same username, so better to combine it with user id value), <code>%uid</code> (current user id, eg: <code>@uid</code>), <code>%lang</code> (current language code, eg: <code>@lang</code>), plus all tokens provided by token module', array('@u' => $user->name, '@uid' => $user->uid, '@lang' => $langCode)),
  );

  $form['filesystem_settings']['mime_detect'] = array(
    '#type' => 'radios',
    '#title' => t('File type detection'),
    '#default_value' => variable_get('elfinder_settings_filesystem_mimedetect', 'auto'),
    '#options' => array(
      'auto' => t('Automatical detection'),
    ),
  );

  $form['filesystem_settings']['filesystem_allowed_extensions'] = array(
    '#prefix' => '<div class="custom-container">',
    '#suffix' => '</div>',
    '#type' => 'textfield',
    '#title' => t('Allowed file extensions'),
    '#maxlength' => 1000,
    '#default_value' => variable_get('elfinder_settings_filesystem_allowed_extensions', ''),
    '#description' => t('Separate extensions with a space or comma and do not include the leading dot.'),
  );

  if (function_exists('finfo_open')) {
    $form['filesystem_settings']['mime_detect']['#options']['finfo'] = t('php finfo');
  }

  if (function_exists('mime_content_type')) {
    $form['filesystem_settings']['mime_detect']['#options']['php'] = t('php mime_content_type()');
  }

  $form['filesystem_settings']['mime_detect']['#options']['linux'] = t('file -ib (linux)');
  $form['filesystem_settings']['mime_detect']['#options']['bsd'] = t('file -Ib (bsd)');
  $form['filesystem_settings']['mime_detect']['#options']['internal'] = t('By file extension (built-in)');
  $form['filesystem_settings']['mime_detect']['#options']['drupal'] = t('Drupal API');


  $form['filesystem_settings']['filesystem_inline_preview'] = array(
    '#type' => 'radios',
    '#title' => t('Preview'),
    '#default_value' => variable_get('elfinder_settings_filesystem_inline_preview', 'default'),
    '#description' => t('File types allowed to display in preview'),
    '#options' => array(
      'default' => t('Default - images, video, audio, pdf, text'),
      'all' => t('All supported for preview files - additional: md, psd, html, archives, swf, sharecad.org, MS Office Online, Google Docs - be careful'),
      'custom' => t('Custom regex'),
      'disabled' => t('Disabled'),
    ),
  );

  $form['filesystem_settings']['filesystem_inline_preview_custom'] = array(
    '#prefix' => '<div class="custom-container">',
    '#suffix' => '</div>',
    '#type' => 'textfield',
    '#title' => t('Custom preview match regex'),
    '#default_value' => variable_get('elfinder_settings_filesystem_inlinepreviewcustom', '^(?:(?:image|video|audio)|application/(?:x-mpegURL|dash\\+xml)|(?:text/plain|application/pdf)$)'),
    '#description' => t('Custom mime type match regex for preview'),
  );

  $form['filesystem_settings']['filesystem_external_preview'] = array(
    '#type' => 'radios',
    '#title' => t('External Service Preview'),
    '#default_value' => variable_get('elfinder_settings_filesystem_external_preview', 'disabled'),
    '#description' => t('Use Microsoft, Google and other online services to preview some office documents. <b>Warning!</b> By previewing document with external services <b>YOU ARE ULOADING</b> the document to them. Google, Microsoft and other service owners usually <b>TRACK</b> your activity and <b>share it with Sales, CIA, FSB (KGB), FBI, governors, etc.</b>'),
    '#options' => array(
      'default' => t('Use Microsoft Office and Google Docs for preview'),
      'disabled' => t('Disabled'),
    ),
  );

  $form['filesystem_settings']['file_url_type'] = array(
    '#type' => 'radios',
    '#title' => t('Selected file url type'),
    '#default_value' => variable_get('elfinder_settings_filesystem_fileurl', 'true'),
    '#options' => array(
      'true' => t('Absolute'),
      'false' => t('Relative'),
    ),
  );

  $form['filesystem_settings']['file_perm'] = array(
    '#type' => 'textfield',
    '#title' => t('Created file permissions'),
    '#default_value' => variable_get('elfinder_settings_filesystem_fileperm', '0666'),
    '#size' => 4,
  );

  $form['filesystem_settings']['dir_perm'] = array(
    '#type' => 'textfield',
    '#title' => t('Created directory permissions'),
    '#default_value' => variable_get('elfinder_settings_filesystem_dirperm', '0777'),
    '#size' => 4,
  );


  $form['filesystem_settings']['max_filesize'] = array(
    '#type' => 'textfield',
    '#title' => t('Maximum upload size'),
    '#default_value' => variable_get('elfinder_settings_filesystem_maxfilesize', ''),
    '#description' => t('Enter a value like "512" (bytes), "80 KB" (kilobytes) or "50 MB" (megabytes) in order to restrict the allowed file size. If left empty the file sizes will be limited only by PHP\'s maximum post and file upload sizes (current limit <strong>%limit</strong>).', array('%limit' => format_size(file_upload_max_size()))),
    '#size' => 10,
    '#weight' => 5,
  );

  $form['filesystem_settings']['max_archivesize'] = array(
    '#type' => 'textfield',
    '#title' => t('Maximum archive size'),
    '#default_value' => variable_get('elfinder_settings_filesystem_maxarchivesize', ''),
    '#description' => t('Enter a value like "512" (bytes), "80 KB" (kilobytes) or "50 MB" (megabytes) in order to restrict the allowed file size. If left empty the archive file file sizes will not be checked during extraction.'),
    '#size' => 10,
    '#weight' => 5,
  );

  $form['filesystem_settings']['max_filecount'] = array(
    '#type' => 'textfield',
    '#title' => t('Maximum folder size'),
    '#default_value' => variable_get('elfinder_settings_filesystem_maxfilecount', ''),
    '#description' => t('The maximum number of files allowed in a directory. 0 for unlimited.'),
    '#size' => 10,
    '#weight' => 5,
  );

  $form['filesystem_settings']['handleprivate'] = array(
    '#type' => 'radios',
    '#title' => t('Handle private downloads'),
    '#default_value' => variable_get('elfinder_settings_filesystem_handleprivate', 'true'),
    '#options' => array(
      'true' => t('Yes'),
      'false' => t('No'),
    ),
    '#description' => t('Use elFinder to handle private file downloads'),
  );

  $form['thumbnail_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Image settings'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  $form['thumbnail_settings']['tmbsize'] = array(
    '#type' => 'textfield',
    '#title' => t('Thumbnail size'),
    '#default_value' => variable_get('elfinder_settings_thumbnail_size', '48'),
    '#size' => 4,
  );

  $form['thumbnail_settings']['tmbdirname'] = array(
    '#type' => 'textfield',
    '#title' => t('Thumbnail directory name'),
    '#default_value' => variable_get('elfinder_settings_thumbnail_dirname', 'tmb'),
    '#size' => 10,
  );

  $form['thumbnail_settings']['imglib'] = array(
    '#type' => 'radios',
    '#title' => t('Image manipulation library'),
    '#default_value' => variable_get('elfinder_settings_thumbnail_imglib', 'auto'),
    '#options' => array(
      'auto' => t('Automatical detection'),
      'imagick' => t('Image Magick'),
      'gd' => t('GD'),
    ),
  );

  $form['thumbnail_settings']['tmbcrop'] = array(
    '#type' => 'radios',
    '#title' => t('Image crop'),
    '#default_value' => variable_get('elfinder_settings_thumbnail_tmbcrop', 'true'),
    '#options' => array(
      'true' => t('Yes'),
      'false' => t('No'),
    ),
    '#description' => t('Crop image to fit thumbnail size. Yes - crop, No - scale image to fit thumbnail size.'),
  );

  $form['misc_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Miscellaneous settings'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  $form['misc_settings']['rememberlastdir'] = array(
    '#type' => 'radios',
    '#title' => t('Remember last opened directory'),
    '#default_value' => variable_get('elfinder_settings_misc_rememberlastdir', 'true'),
    '#options' => array(
      'true' => t('Yes'),
      'false' => t('No'),
    ),
    '#description' => t('Creates a cookie. Disable if you have issues with caching.'),
  );

  $form['misc_settings']['usesystemjquery'] = array(
    '#type' => 'radios',
    '#title' => t('Use system jQuery'),
    '#default_value' => variable_get('elfinder_settings_misc_usesystemjquery', 'true'),
    '#options' => array(
      'true' => t('Yes'),
      'false' => t('No'),
    ),
    '#description' => t('Use system jQuery and jQuery UI when possible. If set to \'No\' jQuery hosted at Google will be uses.'),
  );

  $form['misc_settings']['manager_width'] = array(
    '#type' => 'textfield',
    '#title' => t('File manager width'),
    '#default_value' => variable_get('elfinder_settings_misc_manager_width', ''),
    '#size' => 4,
  );

  $form['misc_settings']['manager_height'] = array(
    '#type' => 'textfield',
    '#title' => t('File manager height'),
    '#default_value' => variable_get('elfinder_settings_misc_manager_height', ''),
    '#size' => 4,
  );

  $form['misc_settings']['hidden_folders'] = array(
    '#type' => 'textfield',
    '#title' => t('Hidden folders'),
    '#maxlength' => 1000,
    '#default_value' => variable_get('elfinder_settings_misc_hidden_folders', ''),
    '#description' => t('Folders listed here will be hidden. Separate with a comma. Example: "color, css_injector, ctools, styles".'),
  );

  $form['submit'] = array('#type' => 'submit', '#value' => t('Save configuration'));
  $form['#submit'][] = 'elfinder_admin_submit';

  return $form;
}

/**
 * Save form data
 */
function elfinder_admin_submit($form, &$form_state) {
  variable_set('elfinder_settings_thumbnail_size', $form_state['values']['tmbsize']);
  variable_set('elfinder_settings_thumbnail_dirname', $form_state['values']['tmbdirname']);

  if ($form_state['values']['filesystem_root_custom'] != '') {
    variable_set('elfinder_settings_filesystem_root_custom', $form_state['values']['filesystem_root_custom']);
  }

  variable_set('elfinder_settings_filesystem_url_custom', $form_state['values']['filesystem_url_custom']);


  variable_set('elfinder_settings_filesystem_mimedetect', $form_state['values']['mime_detect']);
  variable_set('elfinder_settings_filesystem_fileurl', $form_state['values']['file_url_type']);
  variable_set('elfinder_settings_thumbnail_imglib', $form_state['values']['imglib']);
  variable_set('elfinder_settings_filesystem_fileperm', $form_state['values']['file_perm']);
  variable_set('elfinder_settings_filesystem_dirperm', $form_state['values']['dir_perm']);
  variable_set('elfinder_settings_misc_rememberlastdir', $form_state['values']['rememberlastdir']);
  variable_set('elfinder_settings_misc_usesystemjquery', $form_state['values']['usesystemjquery']);
  variable_set('elfinder_settings_thumbnail_tmbcrop', $form_state['values']['tmbcrop']);
  variable_set('elfinder_settings_filesystem_maxfilesize', $form_state['values']['max_filesize']);
  variable_set('elfinder_settings_filesystem_maxarchivesize', $form_state['values']['max_archivesize']);
  variable_set('elfinder_settings_filesystem_maxfilecount', $form_state['values']['max_filecount']);
  variable_set('elfinder_settings_filesystem_handleprivate', $form_state['values']['handleprivate']);
  variable_set('elfinder_settings_filesystem_public_root_label', $form_state['values']['filesystem_public_root_label']);
  variable_set('elfinder_settings_filesystem_private_root_label', $form_state['values']['filesystem_private_root_label']);
  variable_set('elfinder_settings_filesystem_unmanaged_root_label', $form_state['values']['filesystem_unmanaged_root_label']);
  variable_set('elfinder_settings_misc_manager_width', $form_state['values']['manager_width']);
  variable_set('elfinder_settings_misc_manager_height', $form_state['values']['manager_height']);
  variable_set('elfinder_settings_misc_hidden_folders', $form_state['values']['hidden_folders']);
  variable_set('elfinder_settings_filesystem_allowed_extensions', $form_state['values']['filesystem_allowed_extensions']);

  if ($form_state['values']['filesystem_inline_preview'] == 'default') {
    variable_set('elfinder_settings_filesystem_inlinepreviewregex', '^(?:(?:image|video|audio)|application/(?:x-mpegURL|dash\\+xml)|(?:text/plain|application/pdf)$)');
  } else if ($form_state['values']['filesystem_inline_preview'] == 'all') {
    variable_set('elfinder_settings_filesystem_inlinepreviewregex', '.');
  } else if ($form_state['values']['filesystem_inline_preview'] == 'custom') {
    variable_set('elfinder_settings_filesystem_inlinepreviewregex', variable_get('elfinder_settings_filesystem_inlinepreviewcustom', '^$'));
  } else {
    variable_set('elfinder_settings_filesystem_inlinepreviewregex', '^$');
  }

  variable_set('elfinder_settings_filesystem_inline_preview', $form_state['values']['filesystem_inline_preview']);
  variable_set('elfinder_settings_filesystem_inlinepreviewcustom', $form_state['values']['filesystem_inline_preview_custom']);
  variable_set('elfinder_settings_filesystem_external_preview', $form_state['values']['filesystem_external_preview']);

  drupal_set_message(t('Changes have been saved.'));
}

/**
 * Validate form data
 */
function elfinder_admin_form_validate($form, &$form_state) {
  $tmbsize = $form_state['values']['tmbsize'];
  $mwidth = $form_state['values']['manager_width'];
  $mheight = $form_state['values']['manager_height'];

  if ($form_state['values']['filesystem_root'] == 'custom') {
    $customroot = $form_state['values']['filesystem_root_custom'];

    if ($customroot == '') {
      form_set_error('filesystem_root_custom', t('Filesystem root cannot be empty'));
    }

    $customroot = elfinder_parse_path_tokens($customroot);

    // checking directory accessibility by creating test file
    if (!elfinder_prepare_directory($customroot)) {
      form_set_error('filesystem_root_custom', t('Filesystem root is not writable'));
    }
  }

  if (!preg_match('/^\d{3,4}$/', $form_state['values']['file_perm'])) {
    form_set_error('file_perm', t('Created file permissions should be a numeric'));
  }

  if (!preg_match('/^\d{3,4}?$/', $form_state['values']['dir_perm'])) {
    form_set_error('dir_perm', t('Created directory permissions should be a numeric'));
  }

  if (!empty($form_state['values']['max_filesize']) && !is_numeric(parse_size($form_state['values']['max_filesize']))) {
    form_set_error('max_filesize', t('The file size option must contain a valid value. You may either leave the text field empty or enter a string like "512" (bytes), "80 KB" (kilobytes) or "50 MB" (megabytes).'));
  }

  if (!empty($form_state['values']['max_archivesize']) && !is_numeric(parse_size($form_state['values']['max_archivesize']))) {
    form_set_error('max_archivesize', t('The file size option must contain a valid value. You may either leave the text field empty or enter a string like "512" (bytes), "80 KB" (kilobytes) or "50 MB" (megabytes).'));
  }

  if ($mwidth && !is_numeric($mwidth)) {
    form_set_error('manager_width', t('Manager width size should be a number'));
  }

  if ($mheight && !is_numeric($mheight)) {
    form_set_error('manager_height', t('Manager height size should be a number'));
  }

  if ($tmbsize && !is_numeric($tmbsize)) {
    form_set_error('tmbsize', t('Thumbnail size should be a number'));
  }
}
