<?php
/**
 * @file
 * WordPress tags and category migration.
 */

abstract class EwWpGenericTaxonomyMigration extends XMLMigration {
  /**
   * Constructor.
   */
  public function __construct() {
    parent::__construct();

    // Map the source unique identifier for progress, rollbacks.
    $this->map = new MigrateSQLMap($this->machineName,
      array(
        'id' => array(
          'type' => 'varchar',
          'length' => 255,
          'not null' => TRUE,
          'description' => 'Migration Taxonomy Rules ID',
        ),
      ),
      MigrateDestinationFile::getKeySchema()
    );

    $this->addUnmigratedDestinations(array(
      'rename_term',
      'ignore_term',
      'person',
      'movie',
      'tv',
      'book',
      'music',
      'stage',
      'web_series',
      'video_game',
      'taxonomy_genre',
      'taxonomy_ew_franchise',
      'taxonomy_franchise',
      'taxonomy_event',
      'taxonomy_freeform',
      'taxonomy_category',
    ));

    $table_name = 'migration_taxonomy_rules';
    $this->destination = new MigrateDestinationTable($table_name);

    // Update only.
    $this->systemOfRecord = Migration::DESTINATION;
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
    if (isset($row->tag_name) && in_array($row->tag_name, array('=', "'"))) {
      return FALSE;
    }
    // The blog name is the folder where the data came from.
    $row->blog_name = ew_wordpress_migrate_blog_name_from_path($this->getSource()->getReader()->url);

    // Determine existing row's ID, if any.
    $row->id = ew_wordpress_migrate_term_legacy_to_id($row->term_id, $row->source_type);
    // If it does not exist...
    if (!$row->id) {
      // Create it.
      $rule = new stdClass();
      $rule->field_legacy_id = $row->term_id;
      $rule->blog_name = $row->blog_name;
      $rule->source_type = $row->source_type;
      $rule->source_term = $row->source_type == 'tag' ? $row->tag_name : $row->cat_name;
      $rule->rename_term = '';
      drupal_write_record('migration_taxonomy_rules', $rule);
      $row->id = $rule->id;
    }
    return parent::prepareRow($row);
  }
}

class EwWpTagMigration extends EwWpGenericTaxonomyMigration {
  /**
   * Constructor.
   */
  public function __construct() {
    parent::__construct();

    $this->description = t('EW WordPress Tags Import');

    // Source fields.
    $fields = array(
      'term_id' => 'Term ID',
      'tag_slug' => 'Tag Slug',
      'tag_name' => 'Tag Name',
      'blog_name' => 'Blog Name',
      'source_type' => 'Source Type',
    );

    $xml_folder = DRUPAL_ROOT . '/sites/default/files/migration/wordpress/target';
    $files = file_scan_directory($xml_folder, '/\.xml$/');
    foreach ($files as $key => $value) {
      if ($value->name != 'tags') {
        unset($files[$key]);
      }
    }
    $items_url = array_keys($files);

    $item_xpath = '/rss/channel/tag';
    $item_id_xpath = 'wp:term_id';

    $source_options = array(
      'reader_class' => 'WordPressXMLReader',
      'cache_counts' => TRUE,
    );

    $this->source = new EwWordPressSourceXML($items_url, $item_xpath, $item_id_xpath, $fields, $source_options);

    $this->addFieldMapping('id', 'id');
    $this->addFieldMapping('field_legacy_id', 'term_id');
    $this->addFieldMapping('source_term', 'tag_name');
    $this->addFieldMapping('blog_name', 'blog_name');
    $this->addFieldMapping('source_type', 'source_type');

    $this->addUnmigratedSources(array(
      'tag_slug',
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
    $row->source_type = 'tag';
    return parent::prepareRow($row);
  }
}

class EwWpCategoryMigration extends EwWpGenericTaxonomyMigration {
  /**
   * Constructor.
   */
  public function __construct() {
    parent::__construct();

    $this->description = t('EW WordPress Category Import');

    // Source fields.
    $fields = array(
      'term_id' => 'Term ID',
      'category_nicename' => 'Category Slug',
      'category_parent' => 'Category Parent',
      'cat_name' => 'Category Name',
      'blog_name' => 'Blog Name',
      'source_type' => 'Source Type',
    );

    $xml_folder = DRUPAL_ROOT . '/sites/default/files/migration/wordpress/target';
    $files = file_scan_directory($xml_folder, '/\.xml$/');
    foreach ($files as $key => $value) {
      if ($value->name != 'tags') {
        unset($files[$key]);
      }
    }
    $items_url = array_keys($files);

    $item_xpath = '/rss/channel/category';
    $item_id_xpath = 'wp:category_id';

    $source_options = array(
      'reader_class' => 'WordPressXMLReader',
      'cache_counts' => TRUE,
    );

    $this->source = new EwWordPressSourceXML($items_url, $item_xpath, $item_id_xpath, $fields, $source_options);

    $this->addFieldMapping('id', 'id');
    $this->addFieldMapping('field_legacy_id', 'term_id');
    $this->addFieldMapping('source_term', 'cat_name');
    $this->addFieldMapping('blog_name', 'blog_name');
    $this->addFieldMapping('source_type', 'source_type');

    $this->addUnmigratedSources(array(
      'category_nicename',
      'category_parent',
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
    $row->source_type = 'category';
    return parent::prepareRow($row);
  }
}

class EwWpTaxonomyRuleMigration extends Migration {
  /**
   * Constructor.
   */
  public function __construct() {
    parent::__construct();

    $this->description = t('EW Taxonomy Rules Import');
    // Update only.
    $this->systemOfRecord = Migration::DESTINATION;

    // Map the source unique identifier for progress, rollbacks.
    $this->map = new MigrateSQLMap($this->machineName,
      array(
        'id' => array(
          'type' => 'int',
          'not null' => TRUE,
          'unsigned' => TRUE,
          'description' => 'Migration Taxonomy Rules ID',
        ),
      ),
      MigrateDestinationFile::getKeySchema()
    );

    $columns = array(
      array('id', 'ID'),
      array('field_legacy_id', 'Legacy ID'),
      array('blog_name', 'Blog Name'),
      array('source_type', 'Source Type'),
      array('source_term', 'Source Term'),
      array('rename_term', 'Rename Term'),
      array('ignore_term', 'Ignore Term'),
      array('person', 'Person'),
      array('movie', 'Movie'),
      array('tv', 'TV'),
      array('book', 'Book'),
      array('music', 'Music'),
      array('stage', 'Stage'),
      array('web_series', 'Web Series'),
      array('video_game', 'Video Game'),
      array('taxonomy_genre', 'Taxonomy: Genre'),
      array('taxonomy_ew_franchise', 'Taxonomy: EW Franchise'),
      array('taxonomy_franchise', 'Taxonomy: Franchise'),
      array('taxonomy_event', 'Taxonomy: Event'),
      array('taxonomy_freeform', 'Taxonomy: Freeform'),
      array('taxonomy_category', 'Taxonomy: Category'),
    );

    $file = drupal_get_path('module', 'ew_wordpress_migrate') . '/migration_taxonomy_rules.csv';

    $source_options = array(
      'cache_counts' => TRUE,
    );

    $this->source = new MigrateSourceCSV($file, $columns, array(
      'header_rows' => 1,
      'cache_counts' => TRUE,
    ));

    $table_name = 'migration_taxonomy_rules';
    $this->destination = new MigrateDestinationTable($table_name);

    foreach ($columns as $column) {
      $this->addFieldMapping($column[0], $column[0]);
    }
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
    if (!$row->id) {
      watchdog('ew_wordpress_migrate', "EwWpTaxonomyRuleMigration - missing id - !id",
        array('!id' => $row->id), WATCHDOG_ERROR);
      return FALSE;
    }
    if (!$row->field_legacy_id) {
      watchdog('ew_wordpress_migrate', "EwWpTaxonomyRuleMigration - missing field_legacy_id - !field_legacy_id",
        array('!field_legacy_id' => $row->field_legacy_id), WATCHDOG_ERROR);
      return FALSE;
    }
    if (!$row->source_term) {
      watchdog('ew_wordpress_migrate', "EwWpTaxonomyRuleMigration - missing source_term - !source_term",
        array('!source_term' => $row->source_term), WATCHDOG_ERROR);
      return FALSE;
    }

    // Make sure that there aren't multiple mappings for a single term.
    $mapped_values = array();
    foreach (array(
      'ignore_term',
      'person',
      'movie',
      'tv',
      'book',
      'music',
      'stage',
      'web_series',
      'video_game',
      'taxonomy_genre',
      'taxonomy_ew_franchise',
      'taxonomy_franchise',
      'taxonomy_event',
      'taxonomy_freeform',
      'taxonomy_category',
    ) as $binary_field) {
      if (!in_array($row->$binary_field, array(0, 1))) {
        watchdog('ew_wordpress_migrate', "EwWpTaxonomyRuleMigration - invalid range for !id - !field - !value", array(
          '!id' => $row->id,
          '!field' => $binary_field,
          '!value' => $row->$binary_field,
        ), WATCHDOG_ERROR);
        return FALSE;
      }
      if ($row->$binary_field == 1) {
        $mapped_values[] = $binary_field;
      }
    }
    if (count($mapped_values) > 1) {
      watchdog('ew_wordpress_migrate', "EwWpTaxonomyRuleMigration - multiple mappings for !id - !fields", array(
        '!id' => $row->id,
        '!fields' => implode(',', $mapped_values),
      ), WATCHDOG_ERROR);
      return FALSE;
    }

    $id = ew_wordpress_migrate_term_legacy_to_id($row->field_legacy_id, $row->source_type);
    // If it does not exist...
    if (!$id) {
      // Create it.
      drupal_write_record('migration_taxonomy_rules', $row);
    }
    return parent::prepareRow($row);
  }
}

class EwWordPressSourceXML extends WordPressSourceXML {
  /**
   * Get the protected reader for introspection.
   *
   * @return MigrateXMLReader
   *   Protected reader.
   */
  public function getReader() {
    return $this->reader;
  }
}
