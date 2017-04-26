<?php

/**
 * @file
 * Hooks related to Imgix styles and effects.
 */

/**
 * @addtogroup hooks
 * @{
 */

/**
 * Respond to Imgix style updating.
 *
 * This hook enables modules to update settings that might be affected by
 * changes to a style. For example, updating a module specific variable to
 * reflect a change in the Imgix style's name.
 *
 * @param $style
 *   The Imgix style array that is being updated.
 */
function hook_imgix_style_save($style) {
  // If a module defines an Imgix style and that style is renamed by the user
  // the module should update any references to that style.
  if (isset($style['old_name']) && $style['old_name'] == variable_get('mymodule_imgix_style', '')) {
    variable_set('mymodule_imgix_style', $style['name']);
  }
}

/**
 * Respond to Imgix style deletion.
 *
 * This hook enables modules to update settings when a Imgix style is being
 * deleted. If a style is deleted, a replacement name may be specified in
 * $style['name'] and the style being deleted will be specified in
 * $style['old_name'].
 *
 * @param $style
 *   The Imgix style array that being deleted.
 */
function hook_imgix_style_delete($style) {
  // Administrators can choose an optional replacement style when deleting.
  // Update the modules style variable accordingly.
  if (isset($style['old_name']) && $style['old_name'] == variable_get('mymodule_imgix_style', '')) {
    variable_set('mymodule_imgix_style', $style['name']);
  }
}

/**
 * Modify any Imgix styles provided by other modules or the user.
 *
 * This hook allows modules to modify, add, or remove Imgix styles. This may
 * be useful to modify default styles provided by other modules or enforce
 * that a specific effect is always enabled on a style. Note that modifications
 * to these styles may negatively affect the user experience, such as if an
 * effect is added to a style through this hook, the user may attempt to remove
 * the effect but it will be immediately be re-added.
 *
 * The best use of this hook is usually to modify default styles, which are not
 * editable by the user until they are overridden, so such interface
 * contradictions will not occur. This hook can target default (or user) styles
 * by checking the $style['storage'] property.
 *
 * If your module needs to provide a new style (rather than modify an existing
 * one) use hook_imgix_default_styles() instead.
 *
 * @see hook_imgix_default_styles()
 */
function hook_imgix_styles_alter(&$styles) {
  // Check that we only affect a default style.
  if ($styles['thumbnail']['storage'] == IMGIX_STORAGE_DEFAULT) {
    // Add an additional effect to the thumbnail style.
    $styles['thumbnail']['configuration']['h'] = 100;
  }
}

/**
 * Provide module-based Imgix styles for reuse throughout Drupal.
 *
 * This hook allows your module to provide Imgix styles. This may be useful if
 * you require images to fit within exact dimensions. Note that you should
 * attempt to re-use the default styles provided by Imgix module whenever
 * possible, rather than creating Imgix styles that are specific to your module.
 *
 * You may use this hook to more easily manage your site's changes by moving
 * existing Imgix styles from the database to a custom module. Note however that
 * moving Imgix styles to code instead storing them in the database has a
 * negligible effect on performance, since custom Imgix styles are loaded
 * from the database all at once. Even if all styles are pulled from modules,
 * Imgix module will still perform the same queries to check the database for
 * any custom styles.
 *
 * @return
 *   An array of Imgix styles, keyed by the style name.
 * @see imgix_imgix_default_styles()
 */
function hook_imgix_default_styles() {
  $styles = array();

  $styles['mymodule_preview'] = array(
    'label' => 'My module preview',
    'configuration' => array(
      'h' => 100,
      'w' => 100,
    ),
  );

  return $styles;
}

 /**
  * @} End of "addtogroup hooks".
  */
