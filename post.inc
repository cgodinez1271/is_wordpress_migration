<?php
/**
 * @file
 * WordPress Post migration.
 */

class EwWpPostMigration extends XMLMigration {
  // Setting a lower memory limit.
  protected $memoryThreshold = 0.5;

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
      'creator' => 'WordPress Username',
      'post_id' => 'WordPress Post ID',
      'title' => 'Item title',
      'content' => 'Body of the item',
      'post_date' => 'Post Date',
      'post_date_gmt' => 'Post Date GMT',
      'link' => 'Legacy URL',
      'status' => 'WordPress Publish Status',
      'comment_status' => 'Comment Status',
      'field_comments_id' => 'Disqus Comments Id',
      'mt_seo_title' => 'SEO Title / Headline',
      'field_display_ads' => 'Publishing Option - Display Ads',
      'description' => 'WordPress Post Description',
      'post_password' => 'Post Password',
      'is_sticky' => 'Sticky flag',
      'menu_order' => 'Menu Order',
      'post_parent' => 'Post Parent',
      'guid' => 'GUID',
      'excerpt' => 'Excerpt',
      'ping_status' => 'Ping Status',
      'disqus_switch' => 'Disqus Switch',
      'pkg_label' => 'Package Label',
      'module_title' => 'Module Title',
      'module_classes' => 'Module Classes',
      'module_more_link' => 'Module More Link',
      '_publicize_pending' => 'Publicize Pending (?)',
      'tagazine-media' => 'Tagazine Media (?)',
      'sharing_disabled' => 'Sharing Disabled Flag',
      'geo_public' => 'Geo Public Flag (?)',
      '_ew_wp_meta' => 'EW WP Meta (?)',
      'field_images' => 'Images associated with the Post',
      'field_article_creative_work' => t('Creative Work'),
      'field_article_people' => t('People'),
      'field_recap_short_headline' => t('Recap Short Headline'),
      'field_recap_episode_season' => t('Recap Episode Season'),
      'field_recap_episode' => t('Recap Episode Number'),
      'field_recap_episode_airdate' => t('Recap Episode Airdate'),
      'field_recap_episode_type' => t('Recap Episode Type'),
      'field_deck' => t('Deck'),
      'field_related_content' => t('Related Content'),
      'field_category' => t('Category'),
      'field_subcategory' => t('Subcategory'),
      'field_genre' => t('Genre'),
      'field_ew_franchise' => t('EW Franchise'),
      'field_franchise' => t('Franchise'),
      'field_event' => t('Event'),
      'field_tags' => t('Tags'),
      // Arbitrary fields for facilitating migration.
      'post_type_int' => t('Post Type represented by an integer.'),
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

    $this->description = t('EW WordPress Post');

    $this->destination = new MigrateDestinationNode('article');

    $this->softDependencies = array('EwWpRecapShowSeason');

    $this->addFieldMapping('title', 'title');
    $this->addFieldMapping('promote')->defaultValue(0);
    $this->addFieldMapping('sticky')->defaultValue(0);
    $this->addFieldMapping('revision')->defaultValue(0);
    $this->addFieldMapping('field_migration_source')->defaultValue('wordpress');

    $this->addFieldMapping('body', 'content');
    $this->addFieldMapping('body:format')
      ->defaultValue('full_html');

    $this->addFieldMapping('created', 'post_date');
    $this->addFieldMapping('changed', 'post_date');

    $this->addFieldMapping('path', 'path');
    $this->addFieldMapping('pathauto', 'pathauto');

    $this->addFieldMapping('field_authors', 'creator')
      ->callbacks('_ewVignetteAuthorToUid');

    $this->addFieldMapping('status', 'status')
      ->callbacks(array($this, 'wpPublishStatus'));

    $this->addFieldMapping('field_tout_deck', 'description');
    $this->addFieldMapping('field_tout_deck:format')
      ->defaultValue('full_html');

    $this->addFieldMapping('field_display_comments', 'comment_status')
      ->callbacks(array($this, 'wpCommentStatus'));

    $this->addFieldMapping('field_legacy_url', 'link');

    // Hard coding migrated flag.
    $this->addFieldMapping('field_migrated')->defaultValue(1);

    // Legacy ID.
    $this->addFieldMapping('field_legacy_id', 'post_id');

    $this->addFieldMapping('field_comments_id', 'field_comments_id');

    $this->addFieldMapping('field_recap_short_headline', 'field_recap_short_headline');

    $this->addFieldMapping('field_html_title_override', 'mt_seo_title');

    $this->addFieldMapping('field_headline', 'title');

    $this->addFieldMapping('field_headline_social', 'mt_seo_title');

    $this->addFieldMapping('field_display_ads', 'field_display_ads');

    $this->addFieldMapping('field_images', 'field_images');

    $this->addFieldMapping('field_article_people', 'field_article_people');

    $this->addFieldMapping('field_article_type', 'post_type_int');

    $this->addFieldMapping('field_article_creative_work', 'field_article_creative_work');
    $this->addFieldMapping('field_recap_episode_season', 'field_recap_episode_season');
    $this->addFieldMapping('field_recap_episode', 'field_recap_episode');
    $this->addFieldMapping('field_recap_episode_airdate', 'field_recap_episode_airdate');
    $this->addFieldMapping('field_recap_episode_type', 'field_recap_episode_type');
    $this->addFieldMapping('field_deck', 'field_deck');
    $this->addFieldMapping('field_related_content', 'field_related_content');

    $this->addFieldMapping('field_category:source_type')->defaultValue('name');
    $this->addFieldMapping('field_category:create_term')->defaultValue(TRUE);
    $this->addFieldMapping('field_category:ignore_case')->defaultValue(FALSE);
    $this->addFieldMapping('field_category', 'field_category');

    $this->addFieldMapping('field_subcategory:source_type')->defaultValue('name');
    $this->addFieldMapping('field_subcategory:create_term')->defaultValue(TRUE);
    $this->addFieldMapping('field_subcategory:ignore_case')->defaultValue(FALSE);
    $this->addFieldMapping('field_subcategory', 'field_subcategory');

    $this->addFieldMapping('field_genre:source_type')->defaultValue('name');
    $this->addFieldMapping('field_genre:create_term')->defaultValue(TRUE);
    $this->addFieldMapping('field_genre:ignore_case')->defaultValue(FALSE);
    $this->addFieldMapping('field_genre', 'field_genre');

    $this->addFieldMapping('field_ew_franchise:source_type')->defaultValue('name');
    $this->addFieldMapping('field_ew_franchise:create_term')->defaultValue(TRUE);
    $this->addFieldMapping('field_ew_franchise:ignore_case')->defaultValue(FALSE);
    $this->addFieldMapping('field_ew_franchise', 'field_ew_franchise');

    $this->addFieldMapping('field_event:source_type')->defaultValue('name');
    $this->addFieldMapping('field_event:create_term')->defaultValue(TRUE);
    $this->addFieldMapping('field_event:ignore_case')->defaultValue(FALSE);
    $this->addFieldMapping('field_event', 'field_event');

    $this->addFieldMapping('field_franchise:source_type')->defaultValue('name');
    $this->addFieldMapping('field_franchise:create_term')->defaultValue(TRUE);
    $this->addFieldMapping('field_franchise:ignore_case')->defaultValue(FALSE);
    $this->addFieldMapping('field_franchise', 'field_franchise');

    $this->addFieldMapping('field_tags:source_type')->defaultValue('name');
    $this->addFieldMapping('field_tags:create_term')->defaultValue(TRUE);
    $this->addFieldMapping('field_tags:ignore_case')->defaultValue(FALSE);
    $this->addFieldMapping('field_tags', 'field_tags');

    $this->addUnmigratedSources(array(
      'post_password',
      'is_sticky',
      'menu_order',
      'post_parent',
      'post_date_gmt',
      'guid',
      'excerpt',
      'ping_status',
      'disqus_switch',
      'pkg_label',
      'module_title',
      'module_classes',
      'module_more_link',
      '_publicize_pending',
      'tagazine-media',
      'sharing_disabled',
      'geo_public',
      '_ew_wp_meta',
    ));

    $this->addUnmigratedDestinations(array(
      'body:language',
      'body:summary',
      'comment',
      'field_comments_id:language',
      'field_deck:format',
      'field_deck:language',
      'field_description',
      'field_description:format',
      'field_description:language',
      'field_embedded_content',
      'field_eyebrow',
      'field_eyebrow:format',
      'field_eyebrow:language',
      'field_grade',
      'field_headline:format',
      'field_headline:language',
      'field_headline_social:language',
      'field_html_title_override:language',
      'field_legacy_id:language',
      'field_legacy_url:language',
      'field_not_searchable',
      'field_print_issue',
      'field_print_issue:language',
      'field_print_issue_date',
      'field_print_issue_date:rrule',
      'field_print_issue_date:timezone',
      'field_print_issue_date:to',
      'field_recap_episode_airdate:rrule',
      'field_recap_episode_airdate:timezone',
      'field_recap_episode_airdate:to',
      'field_recap_short_headline:language',
      'field_syndication',
      'field_target_platform',
      'field_tout_deck:language',
      'field_tout_headline',
      'field_tout_headline:language',
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
      'metatag_og:video',
      'metatag_og:video:height',
      'metatag_og:video:secure_url',
      'metatag_og:video:type',
      'metatag_og:video:width',
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
      'metatag_twitter:creator',
      'metatag_twitter:creator:id',
      'metatag_twitter:data1',
      'metatag_twitter:data2',
      'metatag_twitter:description',
      'metatag_twitter:image',
      'metatag_twitter:image0',
      'metatag_twitter:image1',
      'metatag_twitter:image2',
      'metatag_twitter:image3',
      'metatag_twitter:image:height',
      'metatag_twitter:image:width',
      'metatag_twitter:label1',
      'metatag_twitter:label2',
      'metatag_twitter:player',
      'metatag_twitter:player:height',
      'metatag_twitter:player:stream',
      'metatag_twitter:player:stream:content_type',
      'metatag_twitter:player:width',
      'metatag_twitter:site',
      'metatag_twitter:site:id',
      'metatag_twitter:title',
      'metatag_twitter:url',
      'nid',
      'revision_uid',
      'tnid',
      'translate',
      'uid',
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
    if (!$row->title) {
      return FALSE;
    }

    // The blog name is the folder where the data came from.
    $blog_name = ew_wordpress_migrate_blog_name_from_path($this->getSource()->getReader()->url);

    switch ($row->post_type) {
      // Should be in its own import or ignored.
      case 'tvr_tout':
      case 'tvr_tvshow':
      case 'tvr_season':
      case 'attachment':
      case 'nav_menu_item':
        return FALSE;

      case 'tvr_recap':
        $row->post_type_int = 2;
        if ($row->recap_show) {
          $row->field_article_creative_work = _ewFieldToNid(
            $blog_name . '_' . $row->recap_show,
            'node',
            'creative_work',
            'field_legacy_id'
          );
          if (!$row->field_article_creative_work) {
            watchdog('ew_wordpress_migrate', "EwWpPostMigration - Creative work field_legacy_id !recap_show not found.",
              array('!recap_show' => $row->recap_show), WATCHDOG_ERROR);
          }
          $row->field_recap_episode_season = ew_wordpress_migrate_lookup_season_number_from_id($row->recap_season);
          $row->field_recap_episode = (int) $row->recap_episode;
          $row->field_recap_episode_airdate = $row->recap_airdate;
          $row->field_deck = $row->recap_deck;
          if (!isset($row->recap_seo_title)) {
            $row->recap_seo_title = trim($row->title);
          }
          $row->field_html_title_override = $row->recap_seo_title;
          if (!isset($row->recap_episode_type)) {
            $row->recap_episode_type = NULL;
          }
          $row->field_recap_episode_type = ew_wordpress_migrate_recap_episode_type_to_int($row->recap_episode_type);
          // [Show Name] [Episode Type] recap:
          $creative_work_node = node_load($row->field_article_creative_work);
          $headline_start = $creative_work_node->title;
          if (isset($row->field_recap_episode_type) && $row->field_recap_episode_type) {
            $headline_start .= ' ' . strtolower(ew_wordpress_migrate_episode_type_id_to_name($row->field_recap_episode_type));
          }
          $headline_start .= ' recap: ';
          $row->field_recap_short_headline = trim($row->title);
          $row->title = $headline_start . trim($row->title);

          $recap_seo_title_exploded = explode(':', $row->recap_seo_title);
          if (count($recap_seo_title_exploded) > 1) {
            unset($recap_seo_title_exploded[0]);
            $row->recap_seo_title = implode(':', $recap_seo_title_exploded);
          }

          $row->mt_seo_title = $headline_start . trim($row->recap_seo_title);
        }
        break;

      default:
        $row->post_type_int = 1;
    }

    if (!isset($row->field_comments_id) || !$row->field_comments_id) {
      $parsed_blog_link = parse_url($row->guid);
      $host_exploded = explode('.', $parsed_blog_link['host']);
      $post_id_exploded = explode('_', $row->post_id);

      // Ensure that prefix for comment ID is correct.
      $prefix = $host_exploded[0];
      if ($post_id_exploded[0] == 'tvrecaps') {
        $prefix = 'tvr';
      }

      $row->field_comments_id = $prefix . '_' . $post_id_exploded[1];
    }
    $row->field_display_ads = 1;
    if (isset($row->no_ads_switch)) {
      $row->field_display_ads = $row->no_ads_switch ? 0 : 1;
    }

    // Images.
    $row->field_images = array();

    $query = new EntityFieldQuery();
    $result = $query
      ->entityCondition('entity_type', 'node')
      ->entityCondition('bundle', 'image')
      ->fieldCondition('field_legacy_id', 'value', $row->post_id, '=')
      ->execute();
    if (isset($result['node'])) {
      $row->field_images = array_keys($result['node']);
    }

    if ($row->post_type == 'tvr_recap' && isset($row->recap_show)) {
      if (isset($row->recap_main_image)) {
        $image = _ewImagesLegacyUrlToNid($row->recap_main_image);
        if ($image === FALSE) {
          watchdog('ew_wordpress_migrate', "EwWpPostMigration - cannot find imported recap_main_image for !recap_main_image",
            array('!recap_main_image' => $row->recap_main_image), WATCHDOG_ERROR);
        }
        else {
          $row->field_images[] = $image;
        }
      }
    }

    $row->field_images = array_unique($row->field_images);

    // Remove emoticons.
    $row->content = preg_replace('/[\x{10000}-\x{10FFFF}]/u', '', $row->content);
    $row->content = nl2br($row->content);

    $row->field_subcategory = ew_wordpress_migrate_blog_name_to_label($blog_name);

    if (isset($row->tag_value) && count($row->tag_value) > 0) {
      foreach ($row->tag_value as $tag_value) {
        // Categories store their values differently.
        if (isset($row->category)) {
          $category_position = array_search($tag_value, $row->category);
          if ($category_position !== FALSE) {
            $tag_value = $row->category_value[$category_position];
          }
        }
        $result = ew_wordpress_migrate_taxonomy_to_drupal($tag_value, $blog_name);
        if ($result && !$result['ignore']) {
          if ($result['nid']) {
            if ($result['type'] == 'creative_work') {
              $row->field_article_creative_work[] = $result['nid'];
            }
            elseif ($result['type'] == 'person') {
              $row->field_article_people[] = $result['nid'];
            }
          }
          elseif ($result['vocabulary']) {
            if ($result['vocabulary'] == 'genre') {
              $row->field_genre[] = $result['tag_value'];
            }
            elseif ($result['vocabulary'] == 'ew_franchise') {
              $row->field_ew_franchise[] = $result['tag_value'];
            }
            elseif ($result['vocabulary'] == 'franchise') {
              $row->field_franchise[] = $result['tag_value'];
            }
            elseif ($result['vocabulary'] == 'event') {
              $row->field_event[] = $result['tag_value'];
            }
            elseif ($result['vocabulary'] == 'freeform') {
              $row->field_tags[] = $result['tag_value'];
            }
            // There can only be one category.
            elseif ($result['vocabulary'] == 'category') {
              $row->field_category = $result['tag_value'];
            }
          }
        }
      }
    }

    // Explicit business rules that supersede any mapping.
    switch ($blog_name) {
      case 'popstyle':
        $row->field_genre = 'Style';
        break;

      case 'thefamilyroom':
        $row->field_genre = 'Family Entertainment';
        break;

      case 'watchingtv':
      case 'tvrecaps':
      case 'insidetv':
        $row->field_category = 'TV';
        break;

      case 'insidemovies':
        $row->field_category = 'Movies';
        break;

      case 'musicmix':
        $row->field_category = 'Music';
        break;

      case 'shelflife':
        $row->field_category = 'Books';
        break;

      case 'popwatch':
        $row->field_category = 'News';
        break;

    }

    $blogs_with_logic = array('newsbriefs', 'thefamilyroom', 'popstyle');
    if (in_array($blog_name, $blogs_with_logic)) {
      $category_priority_order = array(
        'TV',
        'Movies',
        'Music',
        'Books',
        'News',
      );
      $category_default = 'News';
      if (!isset($row->field_category) || empty($row->field_category)) {
        $row->field_category = $category_default;
      }
      else {
        if (!is_array($row->field_category)) {
          $row->field_category = array(
            $row->field_category,
          );
        }
        $new_field_category = '';
        // If multiple categories exist, consolidate.
        foreach ($row->field_category as $existing_category) {
          foreach ($category_priority_order as $category) {
            if (!$new_field_category && $existing_category == $category) {
              $new_field_category = $category;
            }
          }
        }
        // Set default if none exist.
        if (!$new_field_category) {
          $new_field_category = $category_default;
        }
        $row->field_category = $new_field_category;
      }
    }

    if (!isset($row->mt_seo_title)) {
      $row->mt_seo_title = $row->title;
    }

    // Fix for bad data.
    $created_time = strtotime($row->post_date);
    if ($created_time < 1) {
      if (isset($row->recap_airdate)) {
        $row->post_date = date('c', strtotime($row->recap_airdate) + 86400);
      }
      else {
        $row->post_date = date('c');
      }
    }

    // Get the slug from the link; it should be there.
    $path = parse_url(rtrim($row->link, '/'), PHP_URL_PATH);
    $path_fragments = explode('/', $path);
    $slug = end($path_fragments);

    // Interesting, check the guid for the slug.
    if (!$slug) {
      $path = parse_url(rtrim($row->guid, '/'), PHP_URL_PATH);
      $path_fragments = explode('/', $path);
      $slug = end($path_fragments);
    }

    // Set the path.
    if ($slug) {
      $row->pathauto = 0;
      if ($row->post_type_int == 2) {
        $row->path = 'recap/' . $slug;
      }
      else {
        $row->path = 'article/' . date('Y/m/d', strtotime($row->post_date)) . '/' . $slug;
      }
    }
    else {
      $row->pathauto = 1;
    }

    return parent::prepareRow($row);
  }

  /**
   * Convert a WordPress comment status to Drupal.
   *
   * @param string $value
   *   The value to be converted.
   *
   * @return int
   *   The converted value.
   */
  protected function wpCommentStatus($value) {
    if ($value == 'open') {
      return 1;
    }
    return 0;
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
}
