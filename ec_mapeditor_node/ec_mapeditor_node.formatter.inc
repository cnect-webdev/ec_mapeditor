<?php

/**
 * @file
 * Node field formatter functions.
 */

/**
 * Implements hook_field_formatter_info().
 */
function ec_mapeditor_node_field_formatter_info() {
  $formatters = array();
  if (module_exists('geofield') || module_exists('nexteuropa_geofield')) {

    // Fetches default settings.
    $settings = new EcMapeditorMapSettings();

    // @todo. how to prevent this?
    $settings = $settings->settings;

    // Defines settings available to this formatter.
    $formatters['ec_webtools_map'] = array(
      'label' => t('EC webtools map'),
      'field types' => array('geofield', 'geofield_geojson'),
      'settings' => array(
        'tiles' => $settings['tiles'],
        'height' => $settings['height'],
        'zoom' => $settings['zoom'],
        'icon' => $settings['icon'],
        'center' => $settings['center'],
        'fields_in_popup' => $settings['fields_in_popup'],
      ),
    );
  }
  return $formatters;
}

/**
 * Implements hook_field_formatter_settings_form().
 */
function ec_mapeditor_node_field_formatter_settings_form($field, $instance, $view_mode, $form, &$form_state) {
  $display = $instance['display'][$view_mode];
  $settings = $display['settings'];

  // Defines formatter settings form by using shared form elements.
  $elements = array(
    'tiles',
    'height',
    'zoom',
    'icon',
    'center',
    'fields_in_popup',
  );
  return _ec_mapeditor_custom_map_form_elements($elements, $settings);
}

/**
 * Implements hook_field_formatter_settings_summary().
 */
function ec_mapeditor_node_field_formatter_settings_summary($field, $instance, $view_mode) {
  $display = $instance['display'][$view_mode];
  $settings = $display['settings'];
  $summary = '';
  if ($display['type'] == 'ec_webtools_map') {
    $summary = t('@height px, @map, @icon', array(
      '@height' => $settings['height']['height'],
      '@map' => $settings['tiles']['tiles'],
      '@icon' => $settings['icon']['icon'],
    ));
  }
  return $summary;
}

/**
 * Implements hook_field_formatter_view().
 */
function ec_mapeditor_node_field_formatter_view($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
  if (count($items) == 0) {
    return '';
  }
  else {
    if ($display['type'] == 'ec_webtools_map') {
      $elements = array();

      // Defines the layer settings.
      $layer_settings = array();

      // Sets defaults that are irrelevant for the field formatter.
      $layer_settings['clustering']['cluster_markers'] = FALSE;
      $layer_settings['control'] = array(
        'enabled' => TRUE,
        'show_in_control' => FALSE,
      );

      // Fetches layers settings from display settings.
      $layer_settings['icon'] = $display['settings']['icon'];

      // Returns GeoJSON formatted geofield data. Uses same approach as
      // map content type and view. _ec_mapeditor_node_prepare_features
      // expects multiple items that hold multiple features.
      $field_items = array(
        array(
          'field_items' => $items,
        ),
      );

      // Processes geo data from geofield.
      if ($field['type'] == 'geofield') {

        // Display popup content on node page EC_mapeditor field formatter.
        $fields_in_popup = $display['settings']['fields_in_popup']['fields_in_popup'];
        if (NULL == $fields_in_popup) {
          $layer_settings['popup']['show_popup'] = FALSE;
        }
        else {

          $popup_content = "<div class='node-page-popup-content-wrapper'>";
          $popup_field = explode(',', $fields_in_popup);

          foreach ($popup_field as $field_name) {
            if ($field_name == 'title') {
              $popup_content .= '<h2 class="node-page-popup-title">' . $entity->title . '</h2>';
            }
            else {
              $field_view = field_view_field($entity_type, $entity, trim($field_name), 'full');
              $popup_content .= drupal_render($field_view);
            }
          }

          $popup_content .= '</div>';
          // Those fields ([title],[popup]) are mandatory to display a popup.
          $field_items[0]['title'] = 'Node popup';
          $field_items[0]['popup'] = $popup_content;

          // Show popin window.
          $layer_settings['popup']['show_popup'] = TRUE;
          $layer_settings['popup']['popin'] = TRUE;
        }

        $features = _ec_mapeditor_node_prepare_features($field_items);
        // Disables popup for the field formatter because there is no pop up
        // information in the geofield.
      }

      // Processes geo data from nexteuropa geofield.
      elseif ($field['type'] == 'geofield_geojson') {

        // Enables popup for MS geofield.
        $layer_settings['popup'] = array(
          'show_popup' => TRUE,
          'popin' => TRUE,
        );
        $features = $field_items[0]['field_items'][0]['geofield_geojson'];

        // Decodes geofield_geojson field items. They are already in json,
        // but. But they need to be decoded so they can be treated the same as
        // geofield items.
        $features = drupal_json_decode($features);
      }

      // Prepares layers to be attached as JavaScript setting.
      $layers[] = array(
        'layer_settings' => $layer_settings,
        'label' => 'No label',
        'features' => $features,
        'id' => 'no_label',
      );

      // Attaches layers as JavaScript setting.
      $elements['#attached']['js'][] = array(
        'data' => array(
          'node_layers' => $layers,
        ),
        'type' => 'setting',
      );

      // Defines the map settings.
      $map_settings = new EcMapeditorMapSettings();

      // Fetches map settings from display settings.
      $custom_map_settings = array(
        'tiles' => $display['settings']['tiles'],
        'height' => $display['settings']['height'],
        'zoom' => $display['settings']['zoom'],
        'center' => $display['settings']['center'],
      );

      // Sets the center of the map to the first map item if the center
      // coordinates are left empty.
      if (empty($display['settings']['center']['lat']) && empty($display['settings']['center']['lon'])) {
        $custom_map_settings['center']['lat'] = $items[0]['lat'];
        $custom_map_settings['center']['lon'] = $items[0]['lon'];
      }

      $map_settings->customize($custom_map_settings);

      // @todo. how to prevent this?
      $map_settings = $map_settings->settings;

      // Creates the render array for the map.
      $map = new EcMapeditorMapBuild($map_settings);

      // Defines the custom JavaScripts for the webtools JS map library.
      $node_layer_js = base_path() . drupal_get_path('module', 'ec_mapeditor_node') . "/js/node_layer.js?v=" . rand(0, 33333);
      $custom_js[] = $node_layer_js;
      $map_layer_js = base_path() . drupal_get_path('module', 'ec_mapeditor_layer') . "/js/map_layer.js?v=" . rand(0, 33333);
      $custom_js[] = $map_layer_js;
      $ec_mapeditor_js = base_path() . drupal_get_path('module', 'ec_mapeditor') . "/js/map.js?v=" . rand(0, 33333);
      $custom_js[] = $ec_mapeditor_js;
      $map->setMoreCustomJs($custom_js);
      $elements[0]['map'] = $map->build;
      return $elements;
    }
  }
}
