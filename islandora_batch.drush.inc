<?php

/**
 * Implements hook_drush_command().
 */
function islandora_batch_drush_command() {
  $items = array();

  $items['islandora_batch_ingest'] = array(
    'aliases' => array('ibpro', 'ibi'),
    'description' => 'Process and ingest preprocessed entries.',
    'drupal dependencies' => array('islandora_batch'),
    'options' => array(
      'timeout' => array(
        'description' => 'The max amount of time (in seconds) for which we ' .
          'should process.  When the time is exceeded, the current object ' .
          'will complete before terminating execution. If not provided, ' .
          'should process until all available preprocessed objects are ' .
          'exhausted.',
        'required' => FALSE,
      ),
    ),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_LOGIN,
  );

  $items['islandora_batch_scan_preprocess'] = array(
    'aliases' => array('ibsp'),
    'description' => 'Preprocess assets by scanning either a directory or a ' .
      'ZIP archive.',
    'drupal dependencies' => array('islandora_batch'),
    'options' => array(
      'type' => array(
        'description' => 'Either "directory" or "zip".',
        'required' => TRUE,
      ),
      'target' => array(
        'description' => 'The target to directory or zip file to scan.',
        'required' => TRUE,
      ),
      'content_models' => array(
        'description' => 'A comma-separated list of content models to assign ' .
          'to the objects.',
        'value' => 'optional',
        //'required' => FALSE,
      ),
    ),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_LOGIN,
  );

  return $items;
}

/**
 * Implements drush_hook_COMMAND().
 *
 * Builds a preprocessor, and passes it off to a preprocessor handler.
 */
function drush_islandora_batch_scan_preprocess() {
  // Build one of our preprocessors.
  $connection = islandora_get_tuque_connection();
  $parameters = array(
    'type' => drush_get_option('type'),
    'target' => drush_get_option('target'),
    'content_models' => drush_get_option('content_models', array()),
  );
  // Could use a drush_multiple_choice() or whatever to get the list of
  //   content models, if none were provided?
  $preprocessor = new IslandoraScanBatch($connection, $parameters);

  // Pass the preprocessor off to run.
  $preprocessed = islandora_batch_handle_preprocessor($preprocessor);
}

/**
 * Implements drush_hook_COMMAND().
 *
 * Ingests for the specified amount of time, if there is anything already
 * preprocessed.
 */
function drush_islandora_batch_ingest($args) {
  $parameters = array(
    'timeout' => drush_get_option('timeout', NULL),
  );

  $parameters = array_filter($parameters);

  islandora_batch_set_operations($parameters);

  drush_backend_batch_process();
}