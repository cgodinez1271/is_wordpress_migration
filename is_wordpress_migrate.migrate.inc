<?php
/**
 * @file
 * Describe the WordPress migration to the Migrate API.
 */

/**
 * Implements hook_migrate_api().
 */
function is_wordpress_migrate_migrate_api() {
  $api = array(
    'api' => 2,
    'field handlers' => array(
      'DateMigrateFieldHandler',
    ),
    'migrations' => array(
      'ISWpAuthor' => array(
        'class_name' => 'ISWpAuthorMigration',
      ),
      'ISWpArticleNews' => array(
        'class_name' => 'ISWpArticleNewsMigration',
      ),
    ),
  );
  return $api;
}
