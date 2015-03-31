<?php
/**
 * @file
 * WordPress Author migration.
 */

class ISWpAuthorMigration extends XMLMigration {
  /**
   * Constructor.
   */
  public function __construct($argument) {
    parent::__construct($argument);
    $this->description = t('Migration of Authors from WordPress.');

    // Map the source unique identifier for progress, rollbacks.
    $this->map = new MigrateSQLMap($this->machineName,
      array(
        'author_login' => array(
          'type' => 'varchar',
          'not null' => TRUE,
          'length' => 254,
        ),
      ),
      MigrateDestinationTerm::getKeySchema()
    );

    $fields = array(
      // Source fields.
      'author_id' => t('Unique WordPress ID of the author'),
      'author_login' => t('Username (for login) of the author'),
      'author_email' => t('Author email address'),
      'author_display_name' => t('Displayed author name'),
	  'author_first_name' => t('First Name'),
	  'author_last_name' => t('Last Name'),
      // Synthetic fields.
      'field_fullname' => t('Full Name'),
    );

    $xml_folder = DRUPAL_ROOT . '/sites/default/files/migration/wordpress/target/';
    $files = file_scan_directory($xml_folder, '/\.xml$/');
    foreach ($files as $key => $value) {
      if ($value->name != 'authors') {
        unset($files[$key]);
      }
    }
    $items_url = array_keys($files);

    $source_options = array(
		'reader_class' => 'WordPressXMLReader',
		'cache_counts' => TRUE,
    );
	
    $this->source = new WordPressSourceXML($items_url, '/rss/channel/wp:author', 'wp:author_login', $fields, $source_options);
    $this->destination = new MigrateDestinationTerm('author');

    $this->addFieldMapping('name', 'field_fullname');

    $this->addUnmigratedSources(array(
      'author_id',
      'author_email',
    ));
  }

  /**
   * Prepare row for import.
   *
   * @param object $row
   *   Standard object.
   *
   * @return bool
   *   FALSE if the row should be skipped.
   */
  public function prepareRow($row) {

	$first_name = $this->xpathValue($row->xml->xpath('wp:author_first_name')); 
	$last_name = $this->xpathValue($row->xml->xpath('wp:author_last_name')); 
	$display_name = $this->xpathValue($row->xml->xpath('wp:author_display_name'));

    // If we don't have a first and last name, just use the display name.
    if ($first_name && $last_name) {
      $row->field_fullname = $first_name . ' ' . $last_name;
    }
    else {
      $row->field_fullname = $display_name;
    }
	# drush_print_r($row);
	if ($row->field_fullname ) {
    	return TRUE;
	} else {
    	return FALSE;
	}
  }

  public function xpathValue($result) {
	return (string)reset($result);
  }
}
