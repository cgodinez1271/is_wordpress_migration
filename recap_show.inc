<?php

/**
 * @file
 * WordPress Recap migration.
 */

class EwWpRecapShowGenericMigration extends XMLMigration {
  /**
   * Constructor.
   */
  public function __construct() {
    parent::__construct();
    // Map the source unique identifier for progress, rollbacks.
    $this->map = new MigrateSQLMap($this->machineName,
      array(
        'post_id' => array(
          'type' => 'varchar',
          'length' => 255,
          'not null' => TRUE,
          'description' => 'WordPress Post ID',
        ),
      ),
      MigrateDestinationFile::getKeySchema()
    );

    // Source fields.
    $fields = array(
      'title' => 'Show title',
      'content' => 'Description of the Show',
      'link' => 'Legacy URL',
      'post_date' => 'Date published',
    );

    $xml_folder = DRUPAL_ROOT . '/sites/default/files/migration/wordpress/target';
    $files = file_scan_directory($xml_folder, '/\.xml$/');
    foreach ($files as $key => $value) {
      if (in_array($value->name, array('authors', 'tags'))) {
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

    $this->source = new EwWordPressSourceXML($items_url, $item_xpath, $item_id_xpath, $fields, $source_options);
  }

  /**
   * Convert WordPress TV Show Genres into Taxonomy Terms.
   *
   * @param int $value
   *   The WordPress term ID.
   *
   * @return string
   *   The human readable term associated with the ID.
   * @throws MigrateException
   */
  protected function wpTvshowGenre($value) {
    switch ($value) {
      case '56265':
        return 'ActionAdventure';

      case '6234':
        return 'Comedy';

      case '5467':
        return 'Drama';

      case '7003':
        return 'Reality TV';

      case '5215':
        return 'Scifi';

      default:
        throw new MigrateException('Unknown show_genre "' . $value . '"');
    }
  }
}

class EwWpRecapShowMigration extends EwWpRecapShowGenericMigration {
  /**
   * Constructor.
   */
  public function __construct() {
    parent::__construct();

    $this->description = t('EW WordPress Recap Show');

    $this->destination = new MigrateDestinationNode('creative_work');

    $this->addFieldMapping('title', 'title');
    $this->addFieldMapping('promote')->defaultValue(0);
    $this->addFieldMapping('sticky')->defaultValue(0);
    $this->addFieldMapping('revision')->defaultValue(0);

    // Television.
    $this->addFieldMapping('field_cw_type')->defaultValue(3);

    $this->addFieldMapping('body', 'content');
    $this->addFieldMapping('body:format')
      ->defaultValue('full_html');
    $this->addFieldMapping('comment')
      ->defaultValue(0);

    $this->addFieldMapping('created', 'post_date');

    // Hard coding migrated flag.
    $this->addFieldMapping('field_migrated')
      ->defaultValue(1);
    $this->addFieldMapping('field_legacy_url', 'link');
    $this->addFieldMapping('field_legacy_id', 'post_id');

    $this->addFieldMapping('status', 'status')
      ->callbacks(array($this, 'wpPublishStatus'));

    $this->addFieldMapping('field_television_status', 'show_status')
      ->callbacks(array($this, 'wpTvshowStatus'));

    $this->addFieldMapping('field_genre:source_type')
      ->defaultValue('name');
    $this->addFieldMapping('field_genre:create_term')
      ->defaultValue(TRUE);
    $this->addFieldMapping('field_genre:ignore_case')
      ->defaultValue(FALSE);
    $this->addFieldMapping('field_genre', 'show_genre')
      ->callbacks(array($this, 'wpTvshowGenre'));

    $this->addFieldMapping('field_broadcaster:source_type')
      ->defaultValue('name');
    $this->addFieldMapping('field_broadcaster:create_term')
      ->defaultValue(TRUE);
    $this->addFieldMapping('field_broadcaster:ignore_case')
      ->defaultValue(FALSE);
    $this->addFieldMapping('field_broadcaster', 'show_network');

    $this->addFieldMapping('field_images', 'tout')
      ->callbacks('_ewImagesLegacyUrlToNid');

    $this->addUnmigratedSources(array(
      'pubDate',
      'guid',
      'description',
      'creator',
      'excerpt',
      'post_date_gmt',
      'comment_status',
      'ping_status',
      'post_name',
      'post_parent',
      'menu_order',
      'post_type',
      'post_password',
      'is_sticky',
      'show_sortname',
      'show_airday',
      'show_link_text1',
      'show_link_url1',
      'show_link_text2',
      'show_link_url2',
      'show_link_text3',
      'show_link_url3',
      'show_link_text4',
      'show_link_url4',
      'show_current_season',
      'show_related_maxitems',
      'show_related_feeds',
      'show_tout75x75',
    ));

    $this->addUnmigratedDestinations(array(
      'body:language',
      'body:summary',
      'changed',
      'field_comments:language',
      'field_comments',
      'field_cw_author',
      'field_cw_director',
      'field_cw_mpaa',
      'field_cw_release_date',
      'field_cw_run_dates',
      'field_cw_tvpgr',
      'field_distributor:create_term',
      'field_distributor:ignore_case',
      'field_distributor:source_type',
      'field_distributor',
      'field_embedded_content',
      'field_guest_performer',
      'field_lead_performer',
      'field_legacy_id:language',
      'field_legacy_url:language',
      'field_music_label:create_term',
      'field_music_label:ignore_case',
      'field_music_label:source_type',
      'field_music_label',
      'field_performer',
      'field_producer_group:create_term',
      'field_producer_group:ignore_case',
      'field_producer_group:source_type',
      'field_producer_group',
      'field_producer_person',
      'field_publisher:create_term',
      'field_publisher:ignore_case',
      'field_publisher:source_type',
      'field_publisher',
      'field_release_date_limited',
      'field_release_date_wide',
      'field_runtime',
      'field_television_seasons',
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
   * Prepare a row for import.
   *
   * @param object $row
   *   Standard object.
   *
   * @return bool
   *   FALSE if the row should be skipped.
   */
  public function prepareRow($row) {
    if ($row->post_type != 'tvr_tvshow' || !$row->title) {
      return FALSE;
    }

    $row->tout = isset($row->show_tout175x175) && $row->show_tout175x175 ? $row->show_tout175x175 : NULL;
    $row->tout = isset($row->show_tout_main) && $row->show_tout_main ? $row->show_tout_main : $row->tout;

    // See if the show already exists.
    $nid_creative_work = _ewFieldToNid(
      $row->title,
      'node',
      'creative_work',
      'title');
    if ($nid_creative_work) {
      $node = node_load($nid_creative_work);
      $wrapper = entity_metadata_wrapper('node', $node);
      $wrapper->body->value = $row->content;
      $wrapper->body->format->value = 'full_html';

      $field_legacy_url = $wrapper->field_legacy_url->value();
      $field_legacy_url[] = $row->link;
      $wrapper->field_legacy_url = $field_legacy_url;

      $field_legacy_id = $wrapper->field_legacy_id->value();
      $field_legacy_id[] = $row->post_id;
      $wrapper->field_legacy_id = $field_legacy_id;

      $wrapper->field_television_status = $this->wpTvshowStatus($row->show_status);
      $wrapper->save();
      return FALSE;
    }

    return parent::prepareRow($row);
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
   * Convert a WordPress Term into a numeric show status.
   *
   * @param mixed $value
   *   The value to be converted.
   *
   * @return int
   *   The numeric representation of the show status.
   * @throws MigrateException
   */
  protected function wpTvshowStatus($value) {
    switch ($value) {
      // Pending.
      case '29029':
        return 3;

      // On Hiatus.
      case '120701':
        return 2;

      // In Season.
      case '41352379':
        return 1;

      default:
        throw new MigrateException('Unknown show_status "' . $value . '"');
    }
  }
}

/**
 * Import show seasons.
 */
class EwWpRecapShowSeasonMigration extends EwWpRecapShowGenericMigration {
  /**
   * Constructor.
   */
  public function __construct() {
    parent::__construct();
    $this->softDependencies = array('EwWpRecapShow');
    $this->description = t('EW WordPress Recap Show Seasons');

    $table_name = 'recap_seasons';
    $this->map = new MigrateSQLMap($this->machineName,
      array(
        'post_id' => array(
          'type' => 'varchar',
          'length' => 255,
          'not null' => TRUE,
          'description' => 'WordPress Post ID',
        ),
      ),
      MigrateDestinationTable::getKeySchema($table_name)
    );

    $this->destination = new MigrateDestinationTable($table_name);

    $this->addFieldMapping('season_id', 'post_id');
    $this->addFieldMapping('season_number', 'season_number');
    $this->addFieldMapping('nid_creative_work', 'nid_creative_work');

    $this->addUnmigratedSources(array(
      'title',
      'content',
      'link',
      'post_date',
    ));
  }

  /**
   * Prepare a row for import.
   *
   * @param object $row
   *   Standard object.
   *
   * @return bool
   *   FALSE if the row should be skipped.
   */
  public function prepareRow($row) {
    if ($row->post_type != 'tvr_season' || $row->status == 'draft') {
      return FALSE;
    }

    // Fixing missing Seasons.
    if ($row->post_id == 'tvrecaps_2302') {
      $row->season_show = '2301';
    }
    elseif ($row->post_id == 'tvrecaps_2373') {
      $row->season_show = '2372';
    }
    elseif ($row->post_id == 'tvrecaps_27708') {
      $row->season_show = '27401';
    }
    elseif ($row->post_id == 'tvrecaps_33635') {
      $row->season_show = '33634';
    }
    elseif ($row->post_id == 'tvrecaps_40795') {
      $row->season_show = '39461';
    }
    elseif ($row->post_id == 'tvrecaps_43613') {
      $row->season_show = '43612';
    }
    elseif ($row->post_id == 'tvrecaps_43756') {
      $row->season_show = '43758';
    }
    elseif ($row->post_id == 'tvrecaps_44865' || $row->post_id == 'tvrecaps_44866') {
      $row->season_show = '44864';
    }

    if (!$row->season_show) {
      watchdog('ew_wordpress_migrate', "EwWpRecapShowSeasonMigration - season_show missing from !post_id",
        array('!post_id' => $row->post_id), WATCHDOG_ERROR);
      return FALSE;
    }

    $blog_name = ew_wordpress_migrate_blog_name_from_path($this->getSource()->getReader()->url);

    $field_legacy_id = $blog_name . '_' . $row->season_show;

    // Fixing bad data.
    if ($field_legacy_id == 'tvrecaps_169') {
      $field_legacy_id = 'tvrecaps_6297';
    }

    // Fixing missing data.
    try {
      $row->nid_creative_work = _ewFieldToNid(
        $field_legacy_id,
        'node',
        'creative_work',
        'field_legacy_id');
    }
    catch (MigrateException $e) {
      $node = new stdClass();
      if ($field_legacy_id == 'tvrecaps_20869') {
        $node->title = 'The New Normal';
      }
      elseif ($field_legacy_id == 'tvrecaps_39691') {
        $node->title = 'True Detective';
      }
      else {
        throw new MigrateException('EwWpRecapShowSeasonMigration::prepareRow - unknown field_legacy_id "' . $field_legacy_id . '" in ' . $row->post_id);
      }
      $node->type = 'creative_work';
      node_object_prepare($node);
      $node->status = 1;
      $node->promote = 0;
      $node->comment = 0;
      $node->language = LANGUAGE_NONE;
      $node->field_migrated[LANGUAGE_NONE][0]['value'] = 1;
      $node->field_legacy_id[LANGUAGE_NONE][0]['value'] = $field_legacy_id;
      $node = node_submit($node);
      node_save($node);
    }

    $row->nid_creative_work = _ewFieldToNid(
      $field_legacy_id,
      'node',
      'creative_work',
      'field_legacy_id');

    if (!$row->nid_creative_work) {
      watchdog('ew_wordpress_migrate', "EwWpRecapShowSeasonMigration - Creative work creative_work field_legacy_id !season_show not found.",
        array('!season_show' => $row->season_show), WATCHDOG_ERROR);
      return FALSE;
    }

    $row->season_number = (int) $row->season_number;

    $node = node_load($row->nid_creative_work);
    if (!isset($node->field_television_seasons[LANGUAGE_NONE])) {
      $node->field_television_seasons[LANGUAGE_NONE] = array();
    }
    // Avoid duplicates.
    foreach ($node->field_television_seasons[LANGUAGE_NONE] as $key => $season) {
      if ($season['value'] == $row->season_number) {
        unset($node->field_television_seasons[LANGUAGE_NONE][$key]);
      }
    }
    $node->field_television_seasons[LANGUAGE_NONE][] = array(
      'value' => $row->season_number,
    );
    node_save($node);

    $row->post_id = substr($row->post_id, strlen($blog_name) + 1);

    return parent::prepareRow($row);
  }
}
