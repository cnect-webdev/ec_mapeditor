<?php
/**
 * @file
 * Define helper functions for install file.
 */

/**
 * Creates the URL map layer bundle.
 */
function _ec_mapeditor_url_create_layer() {
  $bundle = new stdClass();
  $bundle->type = 'url_layer';
  $bundle->label = t('URL layer');
  $bundle->is_new = TRUE;
  $bundle->description = t("Map layer from URLs");
  $controller = new EcMapeditorLayerTypeController('map_layer_type');
  $controller->save($bundle);
}

/**
 * Defines and creates the Map URL base and instance field.
 */
function _ec_mapeditor_url_create_url_field() {
  if (field_info_field('map_url')) {
    watchdog('url_layer', 'Field Map URL not created because it exists already', array(), WATCHDOG_NOTICE, NULL);
  }
  else {
    $array_fields = array(
      'map_url' => array(
        'active' => 1,
        'cardinality' => -1,
        'deleted' => 0,
        'entity_types' => array(),
        'indexes' => array(),
        'locked' => 1,
        'module' => 'link',
        'settings' => array(
          'attributes' => array(
            'class' => '',
            'rel' => '',
            'target' => 'default',
          ),
          'display' => array(
            'url_cutoff' => 80,
          ),
          'enable_tokens' => 1,
          'title' => 'optional',
          'title_maxlength' => 128,
          'title_value' => '',
          'url' => 0,
        ),
        'translatable' => 0,
        'type' => 'link_field',
        'field_name' => 'map_url',
      ),
    );
    foreach ($array_fields as $field) {
      field_create_field($field);
    }
  }

  // Defines the Map URL field for the CSV map layer bundle.
  $t = get_t();
  $array_instances = array(
    'map_url' => array(
      'entity_type' => 'map_layer',
      'bundle' => 'url_layer',
      'default_value' => NULL,
      'deleted' => 0,
      'display' => array(
        'default' => array(
          'label' => 'above',
          'settings' => array(),
          'type' => 'hidden',
          'weight' => 0,
        ),
        'map_layer' => array(
          'label' => 'above',
          'settings' => array(),
          'type' => 'hidden',
          'weight' => 0,
        ),
      ),
      'field_name' => 'map_url',
      'label' => $t('URL'),
      'description' => 'URL where to find map data. You can link to json or kml files. The URLs must be publicly accessible.',
      'required' => 0,
      'settings' => array(
        'absolute_url' => 1,
        'attributes' => array(
          'class' => '',
          'configurable_class' => 0,
          'configurable_title' => 0,
          'rel' => '',
          'target' => 'default',
          'title' => '',
        ),
        'display' => array(
          'url_cutoff' => 80,
        ),
        'enable_tokens' => 1,
        'rel_remove' => 'default',
        'title' => 'none',
        'title_label_use_field_label' => 0,
        'title_maxlength' => 128,
        'title_value' => '',
        'url' => 0,
        'user_register_form' => FALSE,
        'validate_url' => 1,
      ),
      'widget' => array(
        'active' => 0,
        'module' => 'link',
        'settings' => array(),
        'type' => 'link_field',
        'weight' => 4,
      ),
    ),
  );
  foreach ($array_instances as $instance) {
    field_create_instance($instance);
  }
}

/**
 * Changes map layer field.
 *
 * Adds url layer as target bundles.
 */
function _ec_mapeditor_url_change_map_layers_field() {
  $field_info = field_read_field('map_layers');

  // Adds url layer as target bundles.
  $field_info['settings']['handler_settings']['target_bundles']['url_layer'] = 'url_layer';
  field_update_field($field_info);
}
