<?php

function template_preprocess_islandora_delftware(array &$variables) {
  drupal_add_js('misc/form.js');
  drupal_add_js('misc/collapse.js');
  $islandora_object = $variables['islandora_object'];
  module_load_include('inc', 'islandora', 'includes/datastream');
  module_load_include('inc', 'islandora', 'includes/utilities');
  module_load_include('inc', 'islandora', 'includes/metadata');

  $variables['islandora_object_label'] = $islandora_object->label;
  $variables['parent_collections'] = islandora_get_parents_from_rels_ext($islandora_object);
  $variables['metadata'] = islandora_retrieve_metadata_markup($islandora_object);
  $variables['description'] = islandora_retrieve_description_markup($islandora_object);

  // Display the full-size since we don't have derivatives.
  if (isset($islandora_object['OBJ']) && islandora_datastream_access(ISLANDORA_VIEW_OBJECTS, $islandora_object['OBJ'])) {
    $full_size_url = url("islandora/object/{$islandora_object->id}/datastream/OBJ/view");
    $params = array(
      'title' => $islandora_object->label,
      'path' => $full_size_url,
    );
    $variables['islandora_content'] = theme('image', $params);
  }

}
