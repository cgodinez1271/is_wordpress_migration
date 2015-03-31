<?php
/**
 * @file
 * WordPress Image migration.
 */

class EwWpImageMigration extends Migration {
  protected $testMode = FALSE;
  protected $testImagePath = 'public://test.png';

  /**
   * Constructor.
   */
  public function __construct() {
    parent::__construct();
    $this->description = t('Migration of Images from WordPress.');

    // Map the source unique identifier for progress, rollbacks.
    $this->map = new MigrateSQLMap($this->machineName,
      array(
        'src' => array(
          'type' => 'varchar',
          'length' => 255,
          'not null' => TRUE,
          'description' => 'WordPress Image URL',
        ),
      ),
      MigrateDestinationTerm::getKeySchema()
    );

    $fields = array(
      'alt' => 'Image Alt Text',
      'post_id' => 'WordPress Post ID',
      'src' => 'URL of the image',
      'created' => 'Time created',
      'destination_dir' => 'Destination directory',
      'destination_file' => 'Destination file',
      'title' => 'Title',
      'credit' => 'Credit',
      'caption' => 'Image',
    );

    $source_folder = DRUPAL_ROOT . '/sites/default/files/migration/wordpress/target';
    $files = file_scan_directory($source_folder, '/\.json/');
    foreach ($files as $key => $value) {
      if (!in_array($value->name, array('images', 'ew_images'))) {
        unset($files[$key]);
      }
    }
    $items_url = array_keys($files);

    $source_options = array(
      'cache_counts' => TRUE,
    );

    $this->source = new MigrateSourceJSON($items_url, 'src', $fields, $source_options);

    $this->destination = new MigrateDestinationNode('image');

    $this->addFieldMapping('title', 'title');

    $this->addFieldMapping('field_legacy_id', 'post_id');
    $this->addFieldMapping('field_legacy_url', 'src');

    if ($this->testMode) {
      $this->addFieldMapping('field_image', 'testImagePath');
    }
    else {
      $this->addFieldMapping('field_image', 'src');
    }
    $this->addFieldMapping('field_image:destination_file', 'destination_file');
    $this->addFieldMapping('field_image:alt', 'alt');
    $this->addFieldMapping('field_image:destination_dir', 'destination_dir');

    foreach (_ew_feature_image_tout_sizes() as $size) {
      $this->addFieldMapping('field_legacy_tout_image_' . $size, 'field_legacy_tout_image_' . $size);
      $this->addFieldMapping('field_legacy_tout_image_' . $size . ':destination_dir', 'destination_dir');
      $this->addFieldMapping('field_legacy_tout_image_' . $size . ':alt', 'alt');
      $this->addUnmigratedDestinations(array(
        'field_legacy_tout_image_' . $size . ':file_class',
        'field_legacy_tout_image_' . $size . ':destination_file',
        'field_legacy_tout_image_' . $size . ':file_replace',
        'field_legacy_tout_image_' . $size . ':preserve_files',
        'field_legacy_tout_image_' . $size . ':source_dir',
        'field_legacy_tout_image_' . $size . ':title',
        'field_legacy_tout_image_' . $size . ':language',
      ));
    }

    $this->addFieldMapping('field_caption', 'caption');
    $this->addFieldMapping('field_caption:format')
      ->defaultValue('full_html');
    $this->addFieldMapping('field_credit', 'credit');

    $this->addFieldMapping('created', 'created');
    $this->addFieldMapping('changed', 'created');
    $this->addFieldMapping('field_migrated')->defaultValue(1);
    $this->addFieldMapping('status')->defaultValue(1);
    $this->addFieldMapping('promote')->defaultValue(0);
    $this->addFieldMapping('sticky')->defaultValue(0);
    $this->addFieldMapping('revision')->defaultValue(0);
    $this->addFieldMapping('comment')->defaultValue(0);

    $this->addUnmigratedDestinations(array(
      'field_caption:language',
      'field_comments:language',
      'field_comments',
      'field_credit:language',
      'field_headline:format',
      'field_headline:language',
      'field_headline',
      'field_image_rights:create_term',
      'field_image_rights:ignore_case',
      'field_image_rights:source_type',
      'field_image_rights',
      'field_image:file_class',
      'field_image:file_replace',
      'field_image:language',
      'field_image:preserve_files',
      'field_image:source_dir',
      'field_image:title',
      'field_legacy_id:language',
      'field_legacy_url:language',
      'field_metadata_notes:language',
      'field_metadata_notes',
      'is_new',
      'language',
      'log',
      'metatag_abstract',
      'metatag_author',
      'metatag_canonical',
      'metatag_copyright',
      'metatag_description',
      'metatag_fb:admins',
      'metatag_fb:app_id',
      'metatag_generator',
      'metatag_image_src',
      'metatag_keywords',
      'metatag_news_keywords',
      'metatag_og:country-name',
      'metatag_og:description',
      'metatag_og:email',
      'metatag_og:fax_number',
      'metatag_og:image',
      'metatag_og:latitude',
      'metatag_og:locality',
      'metatag_og:longitude',
      'metatag_og:phone_number',
      'metatag_og:postal-code',
      'metatag_og:region',
      'metatag_og:site_name',
      'metatag_og:street-address',
      'metatag_og:title',
      'metatag_og:type',
      'metatag_og:url',
      'metatag_og:video:height',
      'metatag_og:video:secure_url',
      'metatag_og:video:type',
      'metatag_og:video:width',
      'metatag_og:video',
      'metatag_original-source',
      'metatag_publisher',
      'metatag_robots',
      'metatag_shortlink',
      'metatag_title',
      'metatag_twitter:app:country',
      'metatag_twitter:app:id:googleplay',
      'metatag_twitter:app:id:ipad',
      'metatag_twitter:app:id:iphone',
      'metatag_twitter:app:url:googleplay',
      'metatag_twitter:app:url:ipad',
      'metatag_twitter:app:url:iphone',
      'metatag_twitter:card',
      'metatag_twitter:creator:id',
      'metatag_twitter:creator',
      'metatag_twitter:data1',
      'metatag_twitter:data2',
      'metatag_twitter:description',
      'metatag_twitter:image:height',
      'metatag_twitter:image:width',
      'metatag_twitter:image',
      'metatag_twitter:image0',
      'metatag_twitter:image1',
      'metatag_twitter:image2',
      'metatag_twitter:image3',
      'metatag_twitter:label1',
      'metatag_twitter:label2',
      'metatag_twitter:player:height',
      'metatag_twitter:player:stream:content_type',
      'metatag_twitter:player:stream',
      'metatag_twitter:player:width',
      'metatag_twitter:player',
      'metatag_twitter:site:id',
      'metatag_twitter:site',
      'metatag_twitter:title',
      'metatag_twitter:url',
      'nid',
      'path',
      'revision_uid',
      'tnid',
      'translate',
      'uid',
    ));
  }

  /**
   * Called from the source plugin upon first pulling raw data.
   *
   * @param object $row
   *   Object containing raw source data.
   *
   * @return bool
   *   TRUE to process this row, FALSE to have the source skip it.
   * @throws MigrateException
   *   When the src cannot be parsed for a date.
   */
  public function prepareRow($row) {
    $row->src = trim($row->src);

    $parsed_src = parse_url($row->src);
    $parsed_src['path'] = str_replace('//', '/', $parsed_src['path']);
    $path_exploded = explode('/', $parsed_src['path']);
    $year = $month = $day = '';

    if ($path_exploded[1] == 'ew') {
      if ($path_exploded[2] == 'i') {
        if (is_numeric($path_exploded[3])) {
          $year = $path_exploded[3];
          $month = is_numeric($path_exploded[4]) ? $path_exploded[4] : '00';
          $day = is_numeric($path_exploded[5]) ? $path_exploded[5] : '00';
        }
      }
      elseif ($path_exploded[2] == 'dynamic' && $path_exploded[3] == 'imgs') {
        $path_length = count($path_exploded);

        $date_index = $path_length - 2;

        $year = '20' . substr($path_exploded[$date_index], 0, 2);
        $month = substr($path_exploded[$date_index], 2, 2);
        $day = substr($path_exploded[$date_index], 4, 2);

        // Try another variation.
        if (!is_numeric($year) || !is_numeric(($month)) || !is_numeric($day)) {
          $date_index = $path_length - 3;

          $year = '20' . substr($path_exploded[$date_index], 0, 2);
          $month = substr($path_exploded[$date_index], 2, 2);
          $day = substr($path_exploded[$date_index], 4, 2);
        }
      }
      elseif ($path_exploded[2] == 'covergallery' && $path_exploded[3] == 'img') {
        $months = array(
          'jan',
          'feb',
          'mar',
          'apr',
          'may',
          'jun',
          'jul',
          'aug',
          'sep',
          'oct',
          'nov',
          'dec',
        );
        $year = $path_exploded[4];
        $month = str_pad((string) array_search(substr($path_exploded[5], 0, 3), $months) + 1, 2, '0', STR_PAD_LEFT);
        $day = substr($path_exploded[5], 3, 2);
      }
    }
    elseif (is_numeric($path_exploded[1])) {
      $year = $path_exploded[1];
      $month = is_numeric($path_exploded[2]) ? $path_exploded[2] : '00';
      $day = is_numeric($path_exploded[3]) ? $path_exploded[3] : '00';
    }
    elseif ($path_exploded[1] == 'files' && is_numeric($path_exploded[2])) {
      $year = $path_exploded[2];
      $month = is_numeric($path_exploded[3]) ? $path_exploded[2] : '00';
      $day = is_numeric($path_exploded[4]) ? $path_exploded[3] : '00';
    }
    elseif ($path_exploded[1] == 'photos' && $path_exploded[2] == 'uncategorized') {
      $year = $path_exploded[3];
      $month = $path_exploded[4];
      $day = $path_exploded[5];
    }
    // Give up.
    if (!is_numeric($year) || !is_numeric(($month)) || !is_numeric($day)) {
      $post_date = strtotime($row->post_date);
      if ($post_date < 1) {
        $post_date = time();
      }
      $day = date('d', $post_date);
      $month = date('m', $post_date);
      $year = date('Y', $post_date);
    }

    $row->destination_dir = "public://i/$year/$month/$day";
    $row->created = strtotime("$year/$month/$day");

    // Extreme fallback.
    if ($row->created > time() || $row->created < 0) {
      $post_date = time();
      $day = date('d', $post_date);
      $month = date('m', $post_date);
      $year = date('Y', $post_date);
      $row->destination_dir = "public://i/$year/$month/$day";
      $row->created = strtotime("$year/$month/$day");
    }

    $row->testImagePath = $this->testImagePath;
    $row->destination_file = basename($row->src);

    if (!isset($row->alt)) {
      $file = basename($row->src);
      $file_info = pathinfo($file);
      $file_name = basename($file, '.' . $file_info['extension']);
      $file_name_components = explode('_', $file_name);
      $row->alt = ucwords(trim(str_replace('-', ' ', $file_name_components[0])));
    }

    if ((!isset($row->title) || !$row->title)) {
      $row->title = $row->alt;
    }

    if (!$row->title) {
      $row->title = $row->destination_file;
    }

    return parent::prepareRow($row);
  }
}
