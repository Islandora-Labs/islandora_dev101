<?php

/**
 * @file
 * Contains all the functions required for adding derivatives for the Delftware SP.
 */

/**
 * Creates and adds a thumbnail (TN) datastream.
 *
 * @param AbstractObject $object
 *   The object to add the deriviative datastream to.
 * @param bool $force
 *   Whether the derivative generation is being forced or not.
 *
 * @return array|NULL
 *   An array describing the outcome of the thumbnail creation or NULL if no
 *   derivatives are to be created or updated.
 *
 * @see hook_islandora_derivative()
 */
function islandora_delftware_add_tn_derivative(AbstractObject $object, $force = FALSE) {
  if ($force || !isset($object['TN'])) {
    $size = 200;
    $file_uri = islandora_delftware_get_derivative_source_as_unmanaged_file($object);
    $results = islandora_delftware_add_jpg_derivative($object, $file_uri, 'TN', $size, $force);
    file_unmanaged_delete($file_uri);
    return $results;
  }
}
/**
 * Creates a larger JPG deriviative for the BLUE datastream.
 *
 * @param AbstractObject $object
 *   The object to add the derivative datastreams to.
 * @param bool $force
 *   Whether the derivative generation is being forced or not.
 *
 * @return array|NULL
 *   An array describing the outcome of the thumbnail creation or NULL if no
 *   derivatives are to be created or updated.
 *
 * @see hook_islandora_derivative()
 */
function islandora_delftware_add_blue_derivative(AbstractObject $object, $force = FALSE) {
  if ($force || !isset($object['BLUE'])) {
    $size = 525;
    $file_uri = islandora_delftware_get_derivative_source_as_unmanaged_file($object);
    $results = islandora_delftware_add_jpg_derivative($object, $file_uri, 'BLUE', $size, $force);
    file_unmanaged_delete($file_uri);
    return $results;
  }
}

/**
 * Copies the source datastream to the filesystem as an unmanaged file.
 *
 * @param AbstractObject $object
 *   The object to generate/add the deriviative datastreams to.
 *
 * @return string
 *   The file path to the unmanaged derivative source file.
 */
function islandora_delftware_get_derivative_source_as_unmanaged_file(AbstractObject $object) {
  $mime_detect = new MimeDetect();
  $ext = $mime_detect->getExtension($object['OBJ']->mimeType);
  $file_name = str_replace(':', '-', $object->id);
  $file_uri = file_create_filename("{$file_name}.OBJ.{$ext}", 'temporary://');
  $object['OBJ']->getContent($file_uri);
  return $file_uri;
}

/**
 * Creates and adds a jpeg derivative to an object.
 *
 * @param AbstractObject $object
 *   The object to add the deriviative datastreams to.
 * @param string $file_uri
 *   The URI to the image file the derivative will be generated from.
 * @param string $dsid
 *   The datastream ID to use for the derived datastream.
 * @param int $width
 *   The width to make the derived datastream.
 * @param int $height
 *   The height to make the derived datastream.
 * @param bool $force
 *   Whether the derivative generation is being forced or not.
 *
 * @return array|NULL
 *   An array describing the outcome of adding the JPG derivative or NULL if no
 *   derivatives are to be created or updated.
 *
 * @see hook_islandora_derivative()
 */
function islandora_delftware_add_jpg_derivative(AbstractObject $object, $file_uri, $dsid, $size, $force) {
  if (!isset($object[$dsid]) || (isset($object[$dsid]) && $force === TRUE)) {

    // Create the derivative file according to the passed in parameters.
    $derivative_file_uri = islandora_delftware_create_jpg_derivative($file_uri, $size);
    file_unmanaged_delete($file_uri);

    // Check that we got back a valid file. If not, throw an error.
    if (!is_array($derivative_file_uri) && file_valid_uri($derivative_file_uri)) {
      $success = islandora_delftware_add_datastream($object, $dsid, $derivative_file_uri);
      file_unmanaged_delete($derivative_file_uri);
      if ($success === TRUE) {
        return array(
          'success' => TRUE,
          'messages' => array(
            array(
              'message' => t('Created JPG derivative for @dsid (@pid).'),
              'message_sub' => array(
                '@dsid' => $dsid,
                '@pid' => $object->id,
              ),
              'type' => 'dsm',
            ),
          ),
        );
      }
      else {
        return array(
          'success' => FALSE,
          'messages' => array(
            array(
              'message' => t('Failed to create JPG derivative for @dsid (@pid). Error message: @message'),
              'message_sub' => array(
                '@dsid' => $dsid,
                '@pid' => $object->id,
                '@message' => $success,
              ),
              'type' => 'watchdog',
              'severity' => WATCHDOG_ERROR,
            ),
          ),
        );
      }
    }
    else {
      return $derivative_file_uri;
    }
  }
}

/**
 * Creates a jpeg derivative from the given image file.
 *
 * @param string $file_uri
 *   The URI to the image file the derivative will be generated from.
 * @param string $dsid
 *   Used to identify the generated file.
 * @param int $width
 *   The width to make the derived datastream.
 * @param int $height
 *   The height to make the derived datastream.
 *
 * @return string|array
 *   A URI to the generated derivative if successful, error message otherwise.
 *
 * @see hook_islandora_derivative()
 */
function islandora_delftware_create_jpg_derivative($file_uri, $size ) {
  $source = drupal_realpath($file_uri);
  // Get the base name of the source file.
  $base = pathinfo($source, PATHINFO_FILENAME);

  // Get the smallest dimension on the file.
  $width = exec("identify -format %w " . escapeshellarg($source));
  $height = exec("identify -format %h " . escapeshellarg($source));
  $square_size = min($width, $height);

  // Make an intermediary derivative: square, greyscale, high contrast.
  $temp = file_create_filename("$base-temp.jpg", 'temporary://');
  $temp_path = drupal_realpath($temp);
  $args['colorspace'] = '-set colorspace RGB';
  $args['gray'] = '-colorspace gray';
  $args['contrast'] = '-sigmoidal-contrast 10,40%';
  $args['crop'] = '-gravity center -crop ' . escapeshellarg("{$square_size}x{$square_size}+0+0" );
  $context = array(
    'source' => $source,
    'destination' => $temp_path,
  );
  drupal_alter('imagemagick_arguments', $args, $context);
  // To make use of ImageMagick 6's parenthetical command grouping we need to
  // make the $source image the first parameter and $dest the last.
  // See http://www.imagemagick.org/Usage/basics/#cmdline
  $command = escapeshellarg($source) . ' ' . implode(' ', $args) . ' ' . escapeshellarg($temp_path);
  _imagemagick_convert_exec($command, $output, $ret);


  // Make final derivative, blue and resized
  $dest = file_create_filename("$base-dest.jpg", 'temporary://');
  $dest_path = drupal_realpath($dest);
  $args = array();
  $args['color'] = '-fill RoyalBlue4 -tint 100';
  $args['size'] = '-resize ' . escapeshellarg("{$size}x{$size}");
  $context = array(
    'source' => $temp_path,
    'destination' => $dest_path,
  );
  drupal_alter('imagemagick_arguments', $args, $context);
  $command = escapeshellarg($temp_path) . ' ' . implode(' ', $args) . ' ' . escapeshellarg($dest_path);


  $output = '';
  $ret = -1;
  if (_imagemagick_convert_exec($command, $output, $ret) !== TRUE) {
    return array(
      'success' => FALSE,
      'messages' => array(
        array(
          'message' => t('ImageMagick failed to create derivatives for @pid<br/>Error: @ret<br/>Command: @command<br/>Output: @output'),
          'message_sub' => array(
            '@ret' => $ret,
            '@command' => $command,
            '@output' => $output,
          ),
          'type' => 'watchdog',
          'severity' => WATCHDOG_ERROR,
        ),
      ),
    );
  }
  return $dest;
}

/**
 * Adds the file as a datastream to the object using the DSID to identify it.
 *
 * @param AbstractObject $object
 *   The object to add the datastream to.
 * @param string $datastream_id
 *   The datastream id of the added datastream.
 * @param string $file_uri
 *   A URI to the file containing the content for the datastream.
 *
 * @return bool|string
 *   TRUE if successfully added/modified the datastream, error message if not.
 */
function islandora_delftware_add_datastream($object, $datastream_id, $file_uri) {
  $return = TRUE;
  try {
    $ingest = !isset($object[$datastream_id]);
    $mime_detector = new MimeDetect();
    if ($ingest) {
      $ds = $object->constructDatastream($datastream_id, "M");
      $ds->label = $datastream_id;
    }
    else {
      $ds = $object[$datastream_id];
    }
    if ($ds->mimetype != $mime_detector->getMimetype($file_uri)) {
      $ds->mimetype = $mime_detector->getMimetype($file_uri);
    }
    $ds->setContentFromFile(drupal_realpath($file_uri), FALSE);
    if ($ingest) {
      $object->ingestDatastream($ds);
    }
  }
  catch (exception $e) {
    $return = $e->getMessage();
  }
  file_unmanaged_delete($file_uri);
  return $return;
}
