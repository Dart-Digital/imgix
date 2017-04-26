<?php

/**
 * @file
 * Administration pages for Imgix settings.
 */

/**
 * Form builder; Imgix settings form.
 *
 * @ingroup forms
 */
function imgix_settings_form($form, &$form_state) {

  // Show the Imgix Style label.
  $form['imgix_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Imgix URL'),
    '#default_value' => variable_get('imgix_url', ''),
    '#required' => FALSE,
    '#description' => t('Enter the Imgix URL that maps to your source folder. Set up a "Web Folder" source and give it the URL of your public files directory as the source (%url).', array('%url' => file_create_url('public://'))),
  );

  return system_settings_form($form);
}

/**
 * Menu callback; Listing of all current Imgix styles.
 */
function imgix_style_list() {
  $page = array();

  $styles = imgix_styles();
  $page['imgix_style_list'] = array(
    '#markup' => theme('imgix_style_list', array('styles' => $styles)),
    '#attached' => array(
      'css' => array(drupal_get_path('module', 'imgix') . '/imgix.admin.css' => array()),
    ),
  );

  return $page;
}

/**
 * Form builder; Edit an Imgix style name and effects order.
 *
 * @param $form_state
 *   An associative array containing the current state of the form.
 * @param $style
 *   An Imgix style array.
 * @ingroup forms
 * @see imgix_style_form_submit()
 */
function imgix_style_form($form, &$form_state, $style) {
  $title = t('Edit %name style', array('%name' => $style['label']));
  drupal_set_title($title, PASS_THROUGH);

  // Adjust this form for styles that must be overridden to edit.
  $editable = (bool) ($style['storage'] & IMGIX_STORAGE_EDITABLE);

  if (!$editable && empty($form_state['input'])) {
    drupal_set_message(t('This Imgix style is currently being provided by a module. Click the "Override defaults" button to change its settings.'), 'warning');
  }

  $form_state['imgix_style'] = $style;
  $form['#tree'] = TRUE;
  $form['#attached']['css'][drupal_get_path('module', 'imgix') . '/imgix.admin.css'] = array();

  // Show the thumbnail preview.
  $form['preview'] = array(
    '#type' => 'item',
    '#title' => t('Preview'),
    '#markup' => theme('imgix_style_preview', array('style' => $style)),
  );

  // Show the Imgix Style label.
  $form['label'] = array(
    '#type' => 'textfield',
    '#title' => t('Imgix style name'),
    '#default_value' => $style['label'],
    '#disabled' => !$editable,
    '#required' => TRUE,
  );

  // Allow the name of the style to be changed, unless this style is
  // provided by a module's hook_default_imgix_styles().
  $form['name'] = array(
    '#type' => 'machine_name',
    '#size' => '64',
    '#default_value' => $style['name'],
    '#disabled' => !$editable,
    '#description' => t('Use only lowercase alphanumeric characters, underscores (_), and hyphens (-).'),
    '#required' => TRUE,
    '#machine_name' => array(
      'exists' => 'imgix_style_load',
      'source' => array('label'),
      'replace_pattern' => '[^0-9a-z_\-]',
      'error' => t('Please only use lowercase alphanumeric characters, underscores (_), and hyphens (-) for style names.'),
    ),
  );

  $configuration = $style['configuration'];

  $form['configuration'] = array(
    '#title' => t('Configuration'),
    '#type' => 'fieldset',
    '#tree' => TRUE,
  );

  $form['configuration']['fluid'] = array(
    '#type' => 'checkbox',
    '#title' => t('Fluid'),
    '#description' => t('Use the <a href="!url" target="_blank">imgix-js</a> to make this image scale automatically on different devices.', array('!url' => 'https://www.imgix.com/imgix-js')),
    '#default_value' => isset($configuration['fluid']) ? $configuration['fluid'] : 1,
    '#required' => FALSE,
  );

  $form['configuration']['lossless'] = array(
    '#type' => 'checkbox',
    '#title' => t('Lossless'),
    '#description' => t('<a href="!url" target="_blank">Read documentation</a>', array('!url' => 'https://www.imgix.com/docs/reference/format#param-lossless')),
    '#default_value' => isset($configuration['lossless']) ? $configuration['lossless'] : 0,
    '#required' => FALSE,
  );

  $form['configuration']['fm'] = array(
    '#type' => 'select',
    '#title' => t('Format'),
    '#options' => array(
      '' => t('Original Image Format'),
      'gif' => t('GIF'),
      'jp2' => t('JPEG 2000'),
      'jpg' => t('JPEG'),
      'json' => t('JSON'),
      'jxr' => t('JPEG JXR'),
      'pjpg' => t('Progressive JPEG'),
      'mp4' => t('MP4'),
      'png' => t('PNG'),
      'png8' => t('PNG 8-bit'),
      'png32' => t('PNG 32-big'),
      'webp' => t('WebP'),
    ),
    '#default_value' => isset($configuration['fm']) ? $configuration['fm'] : '',
    '#description' => t('<a href="!url" target="_blank">Read documentation</a>', array('!url' => 'https://www.imgix.com/docs/reference/format#param-fm')),
    '#required' => FALSE,
  );

  $form['configuration']['auto'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Automatic adjustments'),
    '#options' => array(
      'format' => t('Format'),
      'enhance' => t('Enhance'),
      'redeye' => t('Redeye'),
    ),
    '#default_value' => isset($configuration['auto']) ? $configuration['auto'] : array('format'),
    '#description' => t('<a href="!url" target="_blank">Read documentation</a>', array('!url' => 'https://www.imgix.com/docs/reference/automatic#param-auto')),
    '#required' => FALSE,
  );

  $form['configuration']['q'] = array(
    '#type' => 'textfield',
    '#title' => t('Quality'),
    '#description' => t('<a href="!url" target="_blank">Read documentation</a>', array('!url' => 'https://www.imgix.com/docs/reference/format#param-q')),
    '#default_value' => isset($configuration['q']) ? $configuration['q'] : '75',
    '#required' => FALSE,
  );

  $form['configuration']['w'] = array(
    '#type' => 'textfield',
    '#title' => t('Width'),
    '#description' => t('Values between 0.0 and 1.0 are interpreted as a ratio in relation to the source image size. Values greater than 1 are interpreted as pixels.'),
    '#default_value' => isset($configuration['w']) ? $configuration['w'] : '1',
    '#required' => FALSE,
  );

  $form['configuration']['h'] = array(
    '#type' => 'textfield',
    '#title' => t('Height'),
    '#description' => t('Values between 0.0 and 1.0 are interpreted as a ratio in relation to the source image size. Values greater than 1 are interpreted as pixels.'),
    '#default_value' => isset($configuration['h']) ? $configuration['h'] : '1',
    '#required' => FALSE,
  );

  $form['configuration']['fit'] = array(
    '#type' => 'select',
    '#title' => t('Fit'),
    '#options' => array(
      'clip' => t('Clip'),
      'clamp' => t('Clamp'),
      'crop' => t('Crop'),
      'facearea' => t('Facearea'),
      'fill' => t('Fill'),
      'max' => t('Max'),
      'min' => t('Min'),
      'scale' => t('Scale'),
    ),
    '#default_value' => isset($configuration['fit']) ? $configuration['fit'] : 'clip',
    '#description' => t('<a href="!url" target="_blank">Read documentation</a>', array('!url' => 'https://www.imgix.com/docs/reference/size#param-fit')),
    '#required' => FALSE,
  );

  // Show the Override or Submit button for this style.
  $form['actions'] = array('#type' => 'actions');
  $form['actions']['override'] = array(
    '#type' => 'submit',
    '#value' => t('Override defaults'),
    '#validate' => array(),
    '#submit' => array('imgix_style_form_override_submit'),
    '#access' => !$editable,
  );
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Update style'),
    '#access' => $editable,
  );

  return $form;
}

/**
 * Submit handler for overriding a module-defined style.
 */
function imgix_style_form_override_submit($form, &$form_state) {
  drupal_set_message(t('The %style style has been overridden, allowing you to change its settings.', array('%style' => $form_state['imgix_style']['label'])));
  $style = $form_state['imgix_style'];
  $style['configuration'] = $form_state['values']['configuration'];
  imgix_default_style_save($style);
}

/**
 * Submit handler for saving an Imgix style.
 */
function imgix_style_form_submit($form, &$form_state) {
  // Update the Imgix style.
  $style = $form_state['imgix_style'];
  $style['name'] = $form_state['values']['name'];
  $style['label'] = $form_state['values']['label'];
  $style['configuration'] = $form_state['values']['configuration'];

  imgix_style_save($style);
  if ($form_state['values']['op'] == t('Update style')) {
    drupal_set_message(t('Changes to the style have been saved.'));
  }
  $form_state['redirect'] = 'admin/config/media/imgix-styles/edit/' . $style['name'];
}

/**
 * Form builder; Form for adding a new Imgix style.
 *
 * @ingroup forms
 * @see imgix_style_add_form_submit()
 */
function imgix_style_add_form($form, &$form_state) {
  $form['label'] = array(
    '#type' => 'textfield',
    '#title' => t('Style name'),
    '#default_value' => '',
    '#required' => TRUE,
  );
  $form['name'] = array(
    '#type' => 'machine_name',
    '#description' => t('Use only lowercase alphanumeric characters, underscores (_), and hyphens (-).'),
    '#size' => '64',
    '#required' => TRUE,
    '#machine_name' => array(
      'exists' => 'imgix_style_load',
      'source' => array('label'),
      'replace_pattern' => '[^0-9a-z_\-]',
      'error' => t('Please only use lowercase alphanumeric characters, underscores (_), and hyphens (-) for style names.'),
    ),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Create new style'),
  );

  return $form;
}

/**
 * Submit handler for adding a new Imgix style.
 */
function imgix_style_add_form_submit($form, &$form_state) {
  $style = array(
    'name' => $form_state['values']['name'],
    'label' => $form_state['values']['label'],
    'configuration' => $form_state['values']['configuration'],
  );
  $style = imgix_style_save($style);
  drupal_set_message(t('Style %name was created.', array('%name' => $style['label'])));
  $form_state['redirect'] = 'admin/config/media/imgix-styles/edit/' . $style['name'];
}

/**
 * Form builder; Form for deleting an Imgix style.
 *
 * @param $style
 *   An Imgix style array.
 *
 * @ingroup forms
 * @see imgix_style_delete_form_submit()
 */
function imgix_style_delete_form($form, &$form_state, $style) {
  $form_state['imgix_style'] = $style;

  $replacement_styles = array_diff_key(imgix_style_options(TRUE, PASS_THROUGH), array($style['name'] => ''));
  $form['replacement'] = array(
    '#title' => t('Replacement style'),
    '#type' => 'select',
    '#options' => $replacement_styles,
    '#empty_option' => t('No replacement, just delete'),
  );

  return confirm_form(
    $form,
    t('Optionally select a style before deleting %style', array('%style' => $style['label'])),
    'admin/config/media/imgix-styles',
    t('If this style is in use on the site, you may select another style to replace it.'),
    t('Delete'),  t('Cancel')
  );
}

/**
 * Submit handler to delete an Imgix style.
 */
function imgix_style_delete_form_submit($form, &$form_state) {
  $style = $form_state['imgix_style'];

  imgix_style_delete($style, $form_state['values']['replacement']);
  drupal_set_message(t('Style %name was deleted.', array('%name' => $style['label'])));
  $form_state['redirect'] = 'admin/config/media/imgix-styles';
}

/**
 * Confirmation form to revert a database style to its default.
 */
function imgix_style_revert_form($form, &$form_state, $style) {
  $form_state['imgix_style'] = $style;

  return confirm_form(
    $form,
    t('Revert the %style style?', array('%style' => $style['label'])),
    'admin/config/media/imgix-styles',
    t('Reverting this style will delete the customized settings and restore the defaults provided by the @module module.', array('@module' => $style['module'])),
    t('Revert'),  t('Cancel')
  );
}

/**
 * Submit handler to convert an overridden style to its default.
 */
function imgix_style_revert_form_submit($form, &$form_state) {
  drupal_set_message(t('The %style style has been reverted to its defaults.', array('%style' => $form_state['imgix_style']['label'])));
  imgix_default_style_revert($form_state['imgix_style']);
  $form_state['redirect'] = 'admin/config/media/imgix-styles';
}

/**
 * Returns HTML for the page containing the list of Imgix styles.
 *
 * @param $variables
 *   An associative array containing:
 *   - styles: An array of all the Imgix styles returned by imgix_get_styles().
 *
 * @see imgix_get_styles()
 * @ingroup themeable
 */
function theme_imgix_style_list($variables) {
  $styles = $variables['styles'];

  $header = array(t('Style name'), t('Settings'), array('data' => t('Operations'), 'colspan' => 3));
  $rows = array();
  foreach ($styles as $style) {
    $row = array();
    $row[] = l($style['label'], 'admin/config/media/imgix-styles/edit/' . $style['name']);
    $link_attributes = array(
      'attributes' => array(
        'class' => array('imgix-style-link'),
      ),
    );
    if ($style['storage'] == IMGIX_STORAGE_NORMAL) {
      $row[] = t('Custom');
      $row[] = l(t('edit'), 'admin/config/media/imgix-styles/edit/' . $style['name'], $link_attributes);
      $row[] = l(t('delete'), 'admin/config/media/imgix-styles/delete/' . $style['name'], $link_attributes);
    }
    elseif ($style['storage'] == IMGIX_STORAGE_OVERRIDE) {
      $row[] = t('Overridden');
      $row[] = l(t('edit'), 'admin/config/media/imgix-styles/edit/' . $style['name'], $link_attributes);
      $row[] = l(t('revert'), 'admin/config/media/imgix-styles/revert/' . $style['name'], $link_attributes);
    }
    else {
      $row[] = t('Default');
      $row[] = l(t('edit'), 'admin/config/media/imgix-styles/edit/' . $style['name'], $link_attributes);
      $row[] = '';
    }
    $rows[] = $row;
  }

  if (empty($rows)) {
    $rows[] = array(array(
      'colspan' => 4,
      'data' => t('There are currently no styles. <a href="!url">Add a new one</a>.', array('!url' => url('admin/config/media/imgix-styles/add'))),
    ));
  }

  return theme('table', array('header' => $header, 'rows' => $rows));
}

/**
 * Returns HTML for a preview of an Imgix style.
 *
 * @param $variables
 *   An associative array containing:
 *   - style: The Imgix style array being previewed.
 *
 * @ingroup themeable
 */
function theme_imgix_style_preview($variables) {
  $style = $variables['style'];

  $sample_image = 'public://imgix/sample.png';
  $sample_width = 160;
  $sample_height = 160;

  if (!file_exists($sample_image)) {
    $source = drupal_get_path('module', 'imgix') . '/sample.png';

    $directory = 'public://imgix';
    file_prepare_directory($directory, FILE_CREATE_DIRECTORY);
    // Copy the preview image to the public:// folder so it's available for use.
    file_unmanaged_copy($source, $sample_image, FILE_EXISTS_REPLACE);
  }

  // Set up original file information.
  $original_path = $sample_image;
  $original_image = image_get_info($original_path);
  if ($original_image['width'] > $original_image['height']) {
    $original_width = min($original_image['width'], $sample_width);
    $original_height = round($original_width / $original_image['width'] * $original_image['height']);
  }
  else {
    $original_height = min($original_image['height'], $sample_height);
    $original_width = round($original_height / $original_image['height'] * $original_image['width']);
  }
  $original_attributes = array_intersect_key($original_image, array('width' => '', 'height' => ''));
  $original_attributes['style'] = 'width: ' . $original_width . 'px; height: ' . $original_height . 'px;';

  // Set up preview file information.
  $preview_file = imgix_style_url($style['name'], $original_path);
  $preview_image = imgix_get_info('http:' . $preview_file);
  if ($preview_image['width'] > $preview_image['height']) {
    $preview_width = min($preview_image['width'], $sample_width);
    $preview_height = round($preview_width / $preview_image['width'] * $preview_image['height']);
  }
  else {
    $preview_height = min($preview_image['height'], $sample_height);
    $preview_width = round($preview_height / $preview_image['height'] * $preview_image['width']);
  }
  $preview_attributes = array_intersect_key($preview_image, array('width' => '', 'height' => ''));
  $preview_attributes['style'] = 'width: ' . $preview_width . 'px; height: ' . $preview_height . 'px;';

  $output = '<div class="imgix-style-preview preview clearfix">';

  // Build the preview of the original image.
  $original_url = file_create_url($original_path);
  $output .= '<div class="preview-image-wrapper">';
  $output .= t('original') . ' (' . l(t('view actual size'), $original_url) . ')';
  $output .= '<div class="preview-image original-image" style="' . $original_attributes['style'] . '">';
  $output .= '<a href="' . $original_url . '">' . theme('image', array('path' => $original_path, 'alt' => t('Sample original image'), 'title' => '', 'attributes' => $original_attributes)) . '</a>';
  $output .= '<div class="height" style="height: ' . $original_height . 'px"><span>' . $original_image['height'] . 'px</span></div>';
  $output .= '<div class="width" style="width: ' . $original_width . 'px"><span>' . $original_image['width'] . 'px</span></div>';
  $output .= '</div>'; // End preview-image.
  $output .= '</div>'; // End preview-image-wrapper.

  // Build the preview of the Imgix style.
  $preview_url = file_create_url($preview_file);
  $output .= '<div class="preview-image-wrapper">';
  $output .= check_plain($style['label']) . ' (' . l(t('view actual size'), file_create_url($preview_file) . '?' . time()) . ')';
  $output .= '<div class="preview-image modified-image" style="' . $preview_attributes['style'] . '">';
  $output .= '<a href="' . file_create_url($preview_file) . '?' . time() . '">' . theme('image', array('path' => $preview_url, 'alt' => t('Sample modified image'), 'title' => '', 'attributes' => $preview_attributes)) . '</a>';
  $output .= '<div class="height" style="height: ' . $preview_height . 'px"><span>' . $preview_image['height'] . 'px</span></div>';
  $output .= '<div class="width" style="width: ' . $preview_width . 'px"><span>' . $preview_image['width'] . 'px</span></div>';
  $output .= '</div>'; // End preview-image.
  $output .= '</div>'; // End preview-image-wrapper.

  $output .= '</div>'; // End imgix-style-preview.

  return $output;
}
