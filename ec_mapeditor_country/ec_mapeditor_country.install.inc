<?php
/**
 * @file
 * Define helper functions for install file.
 */

/**
 * Creates the country map layer bundle.
 */
function _ec_mapeditor_country_create_country_layer() {
  $bundle = new stdClass();
  $bundle->type = 'country_layer';
  $bundle->label = t('Country layer');
  $bundle->is_new = TRUE;
  $bundle->description = t("Map layer with country outlines");
  $controller = new EcMapeditorLayerTypeController('map_layer_type');
  $controller->save($bundle);
}

/**
 * Defines and creates map countries field.
 */
function _ec_mapeditor_country_create_map_countries_field() {
  if (field_info_field('map_countries')) {
    watchdog('country_layer', 'Field Map Countries not created because it exists already', array(), WATCHDOG_NOTICE, NULL);
  }
  else {
    $array_fields = array(
      'map_countries' => array(
        'active' => 1,
        'cardinality' => 1,
        'deleted' => 0,
        'entity_types' => array(),
        'indexes' => array(
          'format' => array(
            0 => 'format',
          ),
        ),
        'locked' => 1,
        'module' => 'text',
        'settings' => array(),
        'translatable' => 0,
        'field_name' => 'map_countries',
        'translatable' => 0,
        'type' => 'text_long',
      ),
    );
    foreach ($array_fields as $field) {
      field_create_field($field);
    }
  }

  // Defines the map country field for the country layer bundle. Creates
  // multiple line textarea field for CSV data.
  $t = get_t();
  $array_instances = array(
    'map_countries' => array(
      'field_name' => 'map_countries',
      'label' => $t('Countries data'),
      'settings' => array(
        'text_processing' => 0,
        'user_register_form' => FALSE,
      ),
      'widget' => array(
        'active' => 1,
        'module' => 'text',
        'settings' => array(
          'rows' => 12,
        ),
        'type' => 'text_textarea',
        'weight' => 3,
      ),
      'display' => array(
        'default' => array(
          'label' => 'above',
          'module' => 'text',
          'settings' => array(),
          'type' => 'text_default',
          'weight' => 11,
        ),
        'map_layer' => array(
          'label' => 'above',
          'module' => 'text',
          'settings' => array(),
          'type' => 'hidden',
          'weight' => 11,
        ),
      ),
      'entity_type' => 'map_layer',
      'bundle' => 'country_layer',
      'default_value' => NULL,
      'deleted' => 0,
      'description' => t('Countries to be displayed on the map. Enter comma separated values (CSV) in this format: country code, link, description, NUTS level, color.'),
    ),
  );
  foreach ($array_instances as $instance) {
    field_create_instance($instance);
  }
}

/**
 * Defines and map country list field.
 *
 * Creates single textfield for list of country codes.
 */
function _ec_mapeditor_country_create_map_country_list_field() {
  if (field_info_field('map_country_list')) {
    watchdog('country_layer', 'Field Map country list not created because it exists already', array(), WATCHDOG_NOTICE, NULL);
  }
  else {
    $array_fields = array(
      'map_country_list' => array(
        'active' => 1,
        'cardinality' => 1,
        'deleted' => 0,
        'entity_types' => array(),
        'indexes' => array(
          'format' => array(
            0 => 'format',
          ),
        ),
        'locked' => 1,
        'module' => 'text',
        'settings' => array(),
        'translatable' => 0,
        'field_name' => 'map_country_list',
        'translatable' => 0,
        'type' => 'text',
      ),
    );
    foreach ($array_fields as $field) {
      field_create_field($field);
    }
  }

  // Defines the map country list field for the country layer bundle.
  $t = get_t();
  $array_instances = array(
    'map_countries' => array(
      'field_name' => 'map_country_list',
      'label' => $t('Country list'),
      'settings' => array(
        'text_processing' => 0,
        'user_register_form' => FALSE,
      ),
      'widget' => array(
        'active' => 1,
        'module' => 'text',
        'settings' => array(
          'size' => 100,
        ),
        'type' => 'text_textfield',
        'weight' => 3,
      ),
      'display' => array(
        'default' => array(
          'label' => 'above',
          'module' => 'text',
          'settings' => array(),
          'type' => 'text_default',
          'weight' => 11,
        ),
        'map_layer' => array(
          'label' => 'above',
          'module' => 'text',
          'settings' => array(),
          'type' => 'hidden',
          'weight' => 11,
        ),
      ),
      'entity_type' => 'map_layer',
      'bundle' => 'country_layer',
      'default_value' => NULL,
      'deleted' => 0,
      'description' => t('Countries to be displayed on the map. Enter two digit country codes separated by spaces. For example "BE NL LU"'),
    ),
  );

  // Creates the field instance on the entity.
  foreach ($array_instances as $instance) {
    field_create_instance($instance);
  }
}

/**
 * Changes map layer field.
 *
 * Adds country layer as target bundles.
 */
function _ec_mapeditor_country_change_map_layers_field() {
  $field_info = field_read_field('map_layers');

  // Adds country layer as target bundles.
  $field_info['settings']['handler_settings']['target_bundles']['country_layer'] = 'country_layer';
  field_update_field($field_info);
}
