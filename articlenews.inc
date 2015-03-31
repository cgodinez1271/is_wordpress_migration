<?php
/**
 * @file
 * WordPress Post migration.
 */

class ISWpArticleNewsMigration extends XMLMigration {
  public function __construct($arguments) {
    parent::__construct($arguments);

    // Source fields.
    $fields = array(
      'title' => 'Item title',
      'link' => 'WordPress URL of the item',
      'pubDate' => 'Published date',
      'dc:creator' => 'WordPress username of the item author',
      'guid' => 'Alternate URL of the item (?)',
      'description' => 'Blank',
      'content:encoded' => 'Body of the item',
      'excerpt:encoded' => 'Teaser for the item',
      'wp:post_id' => 'Unique ID of the item within the blog',
      'wp:post_date' => 'Date posted (author\s timezone?)',
      'wp:post_date_gmt' => 'Date posted (GMT)',
      'wp:comment_status' => 'Whether comments may be posted to this item (open/closed)',
      'wp:ping_status' => '?',
      'wp:post_name' => 'Trailing component of link',
      'wp:status' => 'Item status (publish/draft/inherit)',
      'wp:post_parent' => 'Parent item ID (?)',
      'wp:menu_order' => 'Equivalent to Drupal weight?',
      'wp:post_type' => 'Item type (post/page/attachment)',
      'wp:post_password' => '?',
      'wp:is_sticky' => 'Equivalent to Drupal sticky flag',
      'category' => 'Categories (as nicename) assigned to this item',
	  'wp:postmeta' => 'Are containers not been given their own WXR tags',
    );

    // Map the source unique identifier for progress, rollbacks.
    $this->map = new MigrateSQLMap($this->machineName,
      array(
        'wp:post_id' => array(
          'type' => 'varchar',
          'length' => 255,
          'not null' => TRUE,
          'description' => 'WordPress Post ID',
        ),
      ),
      MigrateDestinationNode::getKeySchema()
    );

    $xml_folder = DRUPAL_ROOT . '/sites/default/files/migration/wordpress/target';
    $files = file_scan_directory($xml_folder, '/\.xml$/');
    foreach ($files as $key => $value) {
      if (in_array($value->name, array('authors'))) {
        unset($files[$key]);
      }
    }
    $items_url = array_keys($files);
    $item_xpath = '/rss/channel/item';
    $item_id_xpath = 'wp:post_id';

    $source_options = array(
      'reader_class' => 'WordPressXMLReader',
      'cache_counts' => TRUE,
    );

    $this->source = new WordPressSourceXML($items_url, $item_xpath, $item_id_xpath, $fields, $source_options);
    $this->destination = new MigrateDestinationNode('article');

    $this->addFieldMapping('title', 'title')
		->xpath('title');
    $this->addFieldMapping('field_headline', 'title')
		->xpath('title');

    #$this->addFieldMapping('field_authors', 'creator')
    #  ->callbacks('_ewVignetteAuthorToUid');
	$this->addFieldMapping('field_edit_date', 'wp:post_date')
		->xpath('wp:post_date');

    $this->addFieldMapping('body', 'content:encoded')
		->xpath('content:encoded');
    $this->addFieldMapping('body:format')
      ->defaultValue('full_html');

    #$this->addFieldMapping('status', 'status')
   	#   ->callbacks(array($this, 'wpPublishStatus'));

    $this->addFieldMapping('field_legacy_url', 'link')
		->xpath('link');
    $this->addFieldMapping('field_legacy_id', 'wp:post_id')
		->xpath('wp:post_id');

    $this->addUnmigratedSources(array(
      'guid',
      'description',
      'menu_order',
      'excerpt:encoded',
      'post_date_gmt',
      'wp:comment_status',
      'wp:ping_status',
      'wp:post_parent',
      'wp:post_password',
	  'wp:is_sticky',
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
	// drush_print_r($row);

	#$wp_row = $row->xml->children($this->arguments['namespaces']['wp']);
	#$content_row = $row->xml->children($this->arguments['namespaces']['content']);

	$xml_data = $row->xml->children();
	$row->title = (string) $xml_data->title;

    if (!$row->xml->title) {
      return FALSE;
    }

	//$first_name = $this->xpathValue($row->xml->xpath('wp:author_first_name'));c
	//foreach ($row->xml->components->field_images->field_image as $field_image) {}
	foreach (this->xpathValue($row->xml->xpath('wp:postmeta')) as $postmeta) { 
		drush_print_r($postmeta);	
	}
  }

  /**
   * Convert a WordPress publish status to Drupal.
   *
   * @param mixed $value
   *   The value to be converted.
   *
   * @return int
   *   Drupal status value.
   */
  protected function wpPublishStatus($value) {
    if ($value == 'publish') {
      return 1;
    }
    return 0;
  }

  /**
   * Act on each row upon its successful migration.
   *
   * @param object $entity
   *   The entity which has just been created.
   * @param object $row
   *   The source migration data.
   */
  public function complete($entity, stdClass $row) {
    if ($row->status) {
      $state_data = new stdClass();
      $state_data->vid = $entity->vid;
      $state_data->nid = $entity->nid;
      $state_data->state = 'published';
      $state_data->timestamp = strtotime($row->post_date);
      $state_data->status = 1;
      drupal_write_record('node_revision_states', $state_data, array('vid'));
    }
  }

  public function xpathValue($result) {
    return (string)reset($result);
  }
}
