<?php
/**
 * @file
 * WordPress migration utilities.
 *
 * Originally designed by Four Kitchens for the EW migration.
 */

/**
 * Implements hook_drush_command().
 */
function is_wordpress_migrate_drush_command() {
  $items = array();

  $items['is-wp-preimport'] = array(
    'description' => dt('Prepare WXR files for ingestion with WordPress Migrate.'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
    'arguments' => array(
      'source' => dt('Folder within sites/default/files/migration/wordpress/source'),
    ),
    'options' => array(
      'all_operations' => array(
        'description' => dt('By default, performs every operation. Set to 0 if you want to manually choose operations.'),
      ),
      'create_tags' => array(
        'description' => dt('If set, will create an additional file, tags.xml, containing all tags & categories.'),
      ),
      'remove_tags' => array(
        'description' => dt('If set, will remove tags & categories from each WXR file.'),
      ),
      'remove_comments' => array(
        'description' => dt('If set, will remove comments from each WXR file.'),
      ),
      'remove_authors' => array(
        'description' => dt('If set, will remove authors from each WXR file.'),
      ),
      'create_authors' => array(
        'description' => dt('If set, will create an additional file, authors.xml, containing all authors.'),
      ),
      'combine' => array(
        'description' => dt('If set, creates one monolithic WXR file.'),
      ),
      'is_image_extract' => array(
        'description' => dt('If set, creates is_image.json containing all metadata is_image tags.'),
      ),
      'img_src_extract' => array(
        'description' => dt('If set, creates images.json containing all img tags and metadata.'),
      ),
      'shortcodes' => array(
        'description' => dt('If set, replaces shortcodes with HTML.'),
      ),
    ),
  );

  $items['is-wp-tax-rule-export'] = array(
    'description' => dt('Export Taxonomy Term Rules for Migration into CSV format.'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_DATABASE,
    'options' => array(
      'skip-existing' => array(
        'description' => dt('If set, skips rules that have already been defined, which is useful for creating deltas.'),
      ),
    ),
  );

  return $items;
}

/**
 * Command validation callback for ew-wp-preimport.
 */
function drush_is_wordpress_migrate_is_wp_preimport_validate($source = FALSE) {
  $source_base_uri = 'public://migration/wordpress/source';

  // Check to see if source path exists.
  if (!file_exists(drupal_realpath($source_base_uri))) {
    return drush_set_error('EW_WP_IMPORT_NO_SOURCE', dt('@source_base_uri does not exist, please create it.', array(
      '@source_base_uri' => $source_base_uri,
    )));
  }

  // Build list of available folders within source.
  $dirs = glob(drupal_realpath($source_base_uri) . '/*', GLOB_ONLYDIR);
  $source_dirs = array();
  foreach ($dirs as $dir) {
    $dir_parts = explode('/', $dir);
    $source_dirs[array_pop($dir_parts)] = $dir;
  }

  // Abort if no available directories are found.
  if (empty($source_dirs)) {
    return drush_set_error('IS_WP_IMPORT_NO_SOURCE_DIRS', dt('@source_base_uri exists, but contains no directories.', array(
      '@source_base_uri' => $source_base_uri,
    )));
  }

  // Source was provided via argument; validate.
  if ($source) {
    if (!array_key_exists($source, $source_dirs)) {
      drush_set_error('IS_WP_IMPORT_SOURCE_NOT_FOUND', dt('@source was not found in the source directories.'));
      unset($source);
    }
  }
  // No valid source was found; ask user for choice.
  if (!$source) {
    $source = drush_choice($source_dirs, dt('Select the directory within @source_base_uri', array(
      '@source_base_uri' => $source_base_uri,
    )), '!key');
    if (!$source) {
      return drush_user_abort();
    }
  }

  // Valid source path found.
  $source_path = $source_dirs[$source];

  // Check for XML files in source.
  $source_xml_files = file_scan_directory($source_path, '/.*\.xml$/');
  if (empty($source_xml_files)) {
    return drush_set_error('IS_WP_IMPORT_SOURCE_NO_XML', dt('@source does not contain any XML files for processing; aborting.', array(
      '@source' => $source,
    )));
  }

  // Set up target.
  $target_uri = 'public://migration/wordpress/target/' . $source;

  // Create target if it doesn't exist.
  if (!file_exists(drupal_realpath($target_uri))) {
    file_prepare_directory($target_uri, FILE_CREATE_DIRECTORY);
    drush_log(dt('@target_uri created.', array(
      '@target_uri' => $target_uri,
    )), 'ok');
  }

  // Check to see if target already contains files.
  $iterator = new \FilesystemIterator(drupal_realpath($target_uri));
  if ($iterator->valid()) {
    $wipe_target = drush_confirm(dt('@target_uri is not empty, okay to wipe?', array(
      '@target_uri' => $target_uri,
    )));
    if (!$wipe_target) {
      return drush_user_abort();
    }
    file_scan_directory($target_uri, '/.*/', array(
      'callback' => 'file_unmanaged_delete',
      'nomask' => '/(\.\.?|CVS|images)$/',
    ));
    drush_log(dt('@target_uri wiped.', array(
      '@target_uri' => $target_uri,
    )), 'ok');
  }

  // Validation complete.
  drush_set_option('source_uri', $source_base_uri . '/' . $source);
  drush_set_option('target_uri', $target_uri);
  drush_set_option('source_path', $source_path);
  drush_set_option('source_xml_files', $source_xml_files);
}

/**
 * Command callback for ew-wp-preimport.
 */
function drush_is_wordpress_migrate_is_wp_preimport($source = FALSE) {
  if (drush_get_option('all_operations', TRUE)) {
    $commands = is_wordpress_migrate_drush_command();
    foreach (array_keys($commands['ew-wp-preimport']['options']) as $option) {
      if ($option != 'all_operations') {
        drush_set_option($option, TRUE);
      }
    }
  }
  $source_uri = drush_get_option('source_uri');
  $target_uri = drush_get_option('target_uri');
  $source_path = drush_get_option('source_path');
  $source_xml_files = drush_get_option('source_xml_files');

  $create_authors = drush_get_option('create_authors');
  $remove_authors = drush_get_option('remove_authors');

  $create_tags = drush_get_option('create_tags');
  $remove_tags = drush_get_option('remove_tags');

  $filter_content = drush_get_option('shortcodes') || drush_get_option('is_image_extract') || drush_get_option('img_src_extract');

  $tags_to_copy = array(
    'title',
    'link',
    'description',
    'pubDate',
    'language',
    'wp:wxr_version',
    'wp:base_site_url',
    'wp:base_blog_url',
    'generator',
  );

  if (drush_get_option('is_image_extract')) {
    $is_images = array();
  }

  if (drush_get_option('img_src_extract')) {
    $images = array();
  }

  // Colossal WordPress hack.
  if (drush_get_option('shortcodes')) {
    drush_log('  ' . dt('Filtering embedded shortcode elements.'), 'ok');
    define('ABSPATH', __DIR__);
    define('WPINC', '/contrib/wordpress/includes');
    require_once 'is_wordpress_migrate.mock.inc';

    // WordPress files.
    require_once 'contrib/wordpress/includes/functions.php';
    require_once 'contrib/wordpress/includes/plugin.php';
    require_once 'contrib/wordpress/includes/kses.php';
    require_once 'contrib/wordpress/includes/formatting.php';
    require_once 'contrib/wordpress/includes/shortcodes.php';
    require_once 'contrib/wordpress/includes/media.php';
    // Custom shortcodes and filters.
    require_once 'is_wordpress_legacy/shortcodes.php';
    require_once 'is_wordpress_legacy/shortcode.php';
  }

  foreach ($source_xml_files as $uri => $file_class) {
    $blog_name = is_wordpress_migrate_blog_name_from_path($file_class->filename);
    // Create target file.
    $target_xml_uri = $target_uri . '/' . $file_class->filename;
    $target_xml_path = drupal_realpath($target_xml_uri);
    file_unmanaged_copy($uri, $target_xml_uri);
    drush_log(dt('Processing @filename...', array(
      '@filename' => $file_class->filename,
    )), 'ok');

    if (drush_get_option('remove_comments')) {
      // Remove all comments.
      $remove_command = "sed -i '/<wp:comment>/,/<\/wp:comment>/d' " . $target_xml_path;
      $result = drush_shell_exec($remove_command);
      if ($result) {
        drush_log('  ' . dt('Removed comments.'), 'ok');
      }
      else {
        return drush_set_error('IS_WP_IMPORT_SED_FAIL', dt('Comment removal failed, aborting.'));
      }
    }

    // Author operations.
    if (drush_get_option('all_operations', TRUE) || $filter_content || $create_authors || $remove_authors || $create_tags || $remove_tags) {
      $target_xml = new DOMDocument();
      $target_xml->preserveWhiteSpace = FALSE;
      $target_xml->formatOutput = TRUE;
      $target_xml->load($target_xml_path);
      $target_xml_xpath = new DOMXPath($target_xml);
    }

    if ($create_authors) {
      $authors_xml = new DOMDocument('1.0', 'UTF-8');
      $authors_xml->preserveWhiteSpace = FALSE;
      $authors_xml->formatOutput = TRUE;

      $rss = $target_xml->getElementsByTagName('rss')->item(0);
      $rss_element = $authors_xml->createElement('rss');
      // Attributes.
      foreach ($rss->attributes as $attribute) {
        $rss_element->setAttribute($attribute->name, $attribute->value);
      }
      // Namespace attributes.
      foreach ($target_xml_xpath->query('namespace::*', $rss) as $node) {
        $rss_element->setAttribute($node->nodeName, $node->nodeValue);
      }
      $authors_xml->appendChild($rss_element);

      $channel_element = $authors_xml->createElement('channel');
      $rss_element->appendChild($channel_element);

      foreach ($tags_to_copy as $tag) {
        foreach ($target_xml_xpath->query('/descendant::' . $tag . '[1]') as $node) {
          $tag_element = $authors_xml->createElement($node->tagName, $node->nodeValue);
          $channel_element->appendChild($tag_element);
        }
      }
    }

    if ($create_tags) {
      $tags_xml = new DOMDocument('1.0', 'UTF-8');
      $tags_xml->preserveWhiteSpace = FALSE;
      $tags_xml->formatOutput = TRUE;

      $rss = $target_xml->getElementsByTagName('rss')->item(0);
      $rss_element = $tags_xml->createElement('rss');
      // Attributes.
      foreach ($rss->attributes as $attribute) {
        $rss_element->setAttribute($attribute->name, $attribute->value);
      }
      // Namespace attributes.
      foreach ($target_xml_xpath->query('namespace::*', $rss) as $node) {
        $rss_element->setAttribute($node->nodeName, $node->nodeValue);
      }
      $tags_xml->appendChild($rss_element);

      $tag_channel_element = $tags_xml->createElement('channel');
      $rss_element->appendChild($tag_channel_element);

      foreach ($tags_to_copy as $tag) {
        foreach ($target_xml_xpath->query('/descendant::' . $tag . '[1]') as $node) {
          $tag_element = $tags_xml->createElement($node->tagName, $node->nodeValue);
          $tag_channel_element->appendChild($tag_element);
        }
      }
    }

    if ($create_authors || $remove_authors) {
      foreach ($target_xml_xpath->query('/rss/channel/wp:author') as $node) {
        // Copy whole author node into author document.
        if ($create_authors) {
          $channel_element->appendChild($authors_xml->importNode($node, TRUE));
        }
        if ($remove_authors) {
          // Remove author node from target.
          $node->parentNode->removeChild($node);
        }
      }
    }
    if ($create_tags || $remove_tags) {
      foreach ($target_xml_xpath->query('//wp:tag') as $node) {
        // Copy whole tag node into tag document.
        if ($create_tags) {
          foreach ($node->getElementsByTagNameNS($node->namespaceURI, 'term_id') as $element) {
            $element->nodeValue = $blog_name . '_' . $element->nodeValue;
          }
          $tag_channel_element->appendChild($tags_xml->importNode($node, TRUE));
        }
        if ($remove_tags) {
          // Remove tag node from target.
          $node->parentNode->removeChild($node);
        }
      }
      foreach ($target_xml_xpath->query('//wp:category') as $node) {
        // Copy whole tag node into tag document.
        if ($create_tags) {
          foreach ($node->getElementsByTagNameNS($node->namespaceURI, 'term_id') as $element) {
            $element->nodeValue = $blog_name . '_' . $element->nodeValue;
          }
          $tag_channel_element->appendChild($tags_xml->importNode($node, TRUE));
        }
        if ($remove_tags) {
          // Remove tag node from target.
          $node->parentNode->removeChild($node);
        }
      }
    }

    // Scan content and perform operations, such as replacements.
    if ($filter_content) {
      drush_log('  ' . dt('Rewriting post_id values.'), 'ok');
      foreach ($target_xml_xpath->query('//item/wp:post_id') as $node) {
        $node->nodeValue = $blog_name . '_' . $node->nodeValue;
      }

      drush_log('  ' . dt('Removing all but published items.'), 'ok');
      foreach ($target_xml_xpath->query('//item/wp:status') as $node) {
        if ($node->nodeValue != 'publish') {
          $node->parentNode->removeChild($node);
        }
      }

      drush_log('  ' . dt('Removing items without a published status.'), 'ok');
      foreach ($target_xml_xpath->query('//item[not(wp:status)]') as $node) {
        $node->parentNode->removeChild($node);
      }

      drush_log('  ' . dt('Removing items with no title.'), 'ok');
      foreach ($target_xml_xpath->query('//item/title') as $node) {
        $title = trim($node->nodeValue);
        if (!$title) {
          $node->parentNode->removeChild($node);
        }
      }

      if (drush_get_option('is_image_extract')) {
        drush_log('  ' . dt('Extracting is_image tags.'), 'ok');
      }
      if (drush_get_option('img_src_extract')) {
        drush_log('  ' . dt('Extracting img tags.'), 'ok');
      }
      if (drush_get_option('shortcodes')) {
        drush_log('  ' . dt('Filtering embedded shortcode elements.'), 'ok');
      }
      foreach ($target_xml_xpath->query('//item/content:encoded') as $node) {
        $parent_node = $node->parentNode;
        // Determine the WordPress post ID.
        $post_id = $target_xml_xpath->query('.//wp:post_id', $parent_node)->item(0)->nodeValue;
        // Determine the post date.
        $post_date = $target_xml_xpath->query('.//wp:post_date', $parent_node)->item(0)->nodeValue;
        // Get the contents; $before is used to see if any changes occurred.
        $before = $value = $node->nodeValue;

        if (drush_get_option('shortcodes')) {
          $value = do_shortcode($value);
          $value = apply_filters('the_content', $value);

          require_once 'is_wordpress_migrate.shortcodes.inc';
          $all_functions = get_defined_functions();
          foreach ($all_functions['user'] as $function_name) {
            if (substr($function_name, 0, strlen('is_wp_shortcode_replace_')) == 'is_wp_shortcode_replace_') {
              $value = $function_name($value);
            }
          }
        }

        if (drush_get_option('img_src_extract') && $value) {
          $doc = new DOMDocument();
          $caption_image_tags = array();
          $plain_image_tags = array();
          $noncaption_image_tags = array();
          // Suppressing errors, because there's a lot of malformed content.
          // If there are ANY performance problems, look here first.
          @$doc->loadHTML($value);
          // Extract all images.
          $plain_image_tags = iterator_to_array($doc->getElementsByTagName('img'));
          // Get caption shortcodes.
          preg_match_all('/\[(caption)(.*?)(\/)?\](?(4)|(?:(.+?)\[\/\s*caption\s*\]))?/s', $value, $matches);
          // Extract images from caption shortcodes.
          if (!empty($matches[0])) {
            foreach ($matches[0] as $delta => $full_match) {
              $full_match_doc = new DOMDocument();
              @$full_match_doc->loadHTML($full_match);
              $caption_image_tags[$delta] = iterator_to_array($full_match_doc->getElementsByTagName('img'));
              $match = array();
              for ($i = 0; $i <= 6; $i++) {
                $match[$i] = $matches[0][$delta];
              }
              $caption_image_attributes[$delta] = is_shortcode_extract_attributes($match);
              // If caption attribute is filled in, extract credit (misused
              // caption) value.
              if (!empty($caption_image_attributes[$delta]['caption'])) {
                $caption_image_credit[$delta]['caption'] = $caption_image_attributes[$delta]['caption'];
              }
              // If credit is still empty, extract from full match.
              if (empty($caption_image_credit[$delta]['caption'])) {
                preg_match('/\/\>(?:\<\/a\>)? ?(.*?)\[\/caption\]/s', $full_match, $credit_matches);
                if (!empty($credit_matches[1])) {
                  $caption_image_credit[$delta]['caption'] = $credit_matches[1];
                }
              }
            }
          }
          // Build a unidimensional array of caption images.
          if (!empty($caption_image_tags)) {
            $caption_image_tags = call_user_func_array('array_merge', $caption_image_tags);
          }
          // Construct an array of image tags outside caption shortcodes.
          $noncaption_image_tags = array_udiff($plain_image_tags, $caption_image_tags, function ($tag_a, $tag_b) {
            $tag_a_src = $tag_a->attributes->getNamedItem('src')->value;
            $tag_b_src = $tag_b->attributes->getNamedItem('src')->value;
            $tag_src_diff = $tag_a_src === $tag_b_src ? 0 : -1;
            return $tag_src_diff;
          });
          // Extract attributes and information.
          $image_tags = array(
            'caption_images' => !empty($caption_image_tags) ? $caption_image_tags : NULL,
            'noncaption_images' => !empty($noncaption_image_tags) ? $noncaption_image_tags : NULL,
          );
          if (!empty($image_tags)) {
            foreach ($image_tags as $array_delta => $tag_array) {
              if (!empty($tag_array)) {
                foreach ($tag_array as $tag_delta => $tag) {
                  $image = array(
                    'post_id' => $post_id,
                    'post_date' => $post_date,
                  );
                  foreach ($tag->attributes as $attr) {
                    $image[$attr->nodeName] = $attr->nodeValue;
                  }

                  // If extracting a caption image,
                  if ($array_delta === 'caption_images' && empty($image['credit'])) {
                    // Give the image a credit from the caption shortcode.
                    $image['credit'] = $caption_image_credit[$tag_delta]['caption'];
                  }

                  // Filtering - white listing; must contain the following.
                  $whitelisted = FALSE;
                  $whitelisted_strings = array(
                    'wordpress.com',
                    'timeinc.net',
                  );
                  foreach ($whitelisted_strings as $string) {
                    if (strpos($image['src'], $string)) {
                      $whitelisted = TRUE;
                    }
                  }

                  // Filtering - black listing; cannot contain the following.
                  $blacklisted = FALSE;
                  $blacklisted_strings = array(
                    'wp-includes',
                  );
                  foreach ($blacklisted_strings as $string) {
                    if (strpos($image['src'], $string)) {
                      $blacklisted = TRUE;
                    }
                  }

                  // Only add if white-listed and not black-listed.
                  if ($whitelisted && !$blacklisted) {
                    $images[] = $image;
                  }
                }
              }
            }
          }

          // Shows.
          $show_tout_main = $target_xml_xpath->evaluate('string(.//wp:postmeta[wp:meta_key = "show_tout_main"]/wp:meta_value)', $parent_node);
          // Older style.
          if (!$show_tout_main) {
            $show_tout_main = $target_xml_xpath->evaluate('string(.//wp:postmeta[wp:meta_key = "show_tout175x175"]/wp:meta_value)', $parent_node);
          }
          if ($show_tout_main) {
            $image = array(
              'post_id' => $post_id,
              'src' => $show_tout_main,
              'post_date' => $post_date,
            );
            $sizes = _is_feature_image_tout_sizes();
            $field_name_base = 'field_legacy_tout_image_';
            foreach ($sizes as $size) {
              $field_name = $field_name_base . $size;
              $$field_name = $target_xml_xpath->evaluate('string(.//wp:postmeta[wp:meta_key = "show_tout' . $size . '"]/wp:meta_value)', $parent_node);
              if ($$field_name) {
                $image[$field_name] = $$field_name;
              }
            }
            $images[] = $image;
          }

          // Recaps.
          $recap_main_image = $target_xml_xpath->evaluate('string(.//wp:postmeta[wp:meta_key = "recap_main_image"]/wp:meta_value)', $parent_node);
          if ($recap_main_image) {
            $image = array(
              'post_id' => $post_id,
              'src' => $recap_main_image,
              'post_date' => $post_date,
            );
            $recap_main_caption = $target_xml_xpath->evaluate('string(.//wp:postmeta[wp:meta_key = "recap_main_caption"]/wp:meta_value)', $parent_node);
            if ($recap_main_caption) {
              $image['caption'] = trim($recap_main_caption);
            }
            $recap_main_credit = $target_xml_xpath->evaluate('string(.//wp:postmeta[wp:meta_key = "recap_main_credit"]/wp:meta_value)', $parent_node);
            if ($recap_main_credit) {
              $image['credit'] = trim($recap_main_credit);
            }
            $sizes = _is_feature_image_tout_sizes();
            $field_name_base = 'field_legacy_tout_image_';
            foreach ($sizes as $size) {
              $field_name = $field_name_base . $size;
              $$field_name = $target_xml_xpath->evaluate('string(.//wp:postmeta[wp:meta_key = "recap_tout' . $size . '"]/wp:meta_value)', $parent_node);
              if ($$field_name) {
                $image[$field_name] = $$field_name;
              }
            }
            $images[] = $image;
          }
        }

        if (drush_get_option('is_image_extract') && $value) {
          preg_match_all('/(.?)\[(is_image)(.*?)(\/)?\](?(4)|(?:(.+?)\[\/\s*\2\s*\]))?(.?)/s', $value, $matches);
          if (!empty($matches[0])) {
            foreach (array_keys($matches[0]) as $key) {
              $image = array(
                'post_id' => $post_id,
                'post_date' => $post_date,
              );
              $match = array();
              for ($i = 0; $i <= 6; $i++) {
                $match[$i] = $matches[$i][$key];
              }
              $attributes = is_shortcode_extract_attributes($match);
              foreach ($attributes as $attribute_name => $attribute_value) {
                $image[$attribute_name] = $attribute_value;
              }

              if (isset($image['url']) && filter_var($image['url'], FILTER_VALIDATE_URL)) {
                // Normalize.
                $image['src'] = $image['url'];
                unset($image['url']);

                // Determine original file name.
                $file = basename($image['src']);
                $file_info = pathinfo($file);
                $file_name = basename($file, '.' . $file_info['extension']);
                $file_name_components = explode('_', $file_name);

                // Look for variations.
                $sizes = _is_feature_image_tout_sizes();

                $field_name_base = 'field_legacy_tout_image_';
                foreach ($sizes as $size) {
                  $field_name = $field_name_base . $size;
                  $$field_name = $target_xml_xpath->evaluate('string(.//wp:postmeta[wp:meta_key = "post_media_' . $size . '"]/wp:meta_value)', $parent_node);
                  if ($$field_name) {
                    $legacy_tout_file = basename($$field_name);
                    $legacy_tout_file_components = explode('_', $legacy_tout_file);
                    $legacy_tout_file_info = pathinfo($legacy_tout_file);
                    $legacy_tout_file_name = basename($file, '.' . $legacy_tout_file_info['extension']);
                    $legacy_tout_file_name_components = explode('_', $legacy_tout_file_name);
                    // If the image starts with the same name...
                    if ($file_name_components[0] == $legacy_tout_file_name_components[0]
                      && $file_name_components[0] == $legacy_tout_file_components[0]) {
                      // It's a variation.
                      $image[$field_name] = $$field_name;
                    }
                  }
                }
                $is_images[] = $image;
              }
            }
          }
        }

        // Write changes, if any.
        if ($before != $value) {
          $new_child = $target_xml->createElement('content:encoded');
          $new_cdata = $target_xml->createCDATASection($value);
          $new_child->appendChild($new_cdata);
          $parent_node->replaceChild($new_child, $node);
        }
      }

      drush_log('  ' . dt('Removing attachments, pages and menu items.'), 'ok');
      $post_types_to_remove = array('attachment', 'nav_menu_item', 'page');
      foreach ($target_xml_xpath->query('//item/wp:post_type') as $node) {
        if (in_array($node->nodeValue, $post_types_to_remove)) {
          $node->parentNode->removeChild($node);
        }
      }
      drush_log('  ' . dt('Removing items with no type.'), 'ok');
      foreach ($target_xml_xpath->query('//item[not(wp:post_type)]') as $node) {
        $node->parentNode->removeChild($node);
      }
    }

    if ($create_authors) {
      $authors_xml->save(drupal_realpath($target_uri) . '/authors.xml');
      drush_log('  ' . dt('Created separate authors file.'), 'ok');
      // Only do this once.
      $create_authors = FALSE;
    }

    if ($create_tags) {
      $tags_xml->save(drupal_realpath($target_uri) . '/tags.xml');
      drush_log('  ' . dt('Created separate tags and categories file.'), 'ok');
      // Only do this once.
      $create_tags = FALSE;
    }

    if ($remove_authors) {
      $target_xml->save($target_xml_path);
      drush_log('  ' . dt('Removed authors.'), 'ok');
    }

    if ($remove_tags) {
      $target_xml->save($target_xml_path);
      drush_log('  ' . dt('Removed tags and categories.'), 'ok');
    }
  }

  if (drush_get_option('is_image_extract') && !empty($is_images)) {
    file_put_contents(drupal_realpath($target_uri) . '/is_images.json', json_encode($is_images));
    drush_log('  ' . dt('Created is_images.json with all is_image metadata.'), 'ok');
  }

  if (drush_get_option('img_src_extract') && !empty($images)) {
    file_put_contents(drupal_realpath($target_uri) . '/images.json', json_encode($images));
    drush_log('  ' . dt('Created images.json with all img metadata.'), 'ok');
  }

  if (drush_get_option('combine')) {
    drush_print(dt('Combining...'));
    // Get list of files to combine, which is anything except authors.xml.
    $target_xml_files = file_scan_directory($target_uri, '/.*\.xml$/', array(
      'nomask' => '/(\.\.?|CVS|authors.xml|tags.xml)$/',
    ));

    $combined_xml = new DOMDocument('1.0', 'UTF-8');
    $combined_xml->preserveWhiteSpace = FALSE;
    $combined_xml->formatOutput = TRUE;

    $prepare_combined = FALSE;

    foreach ($target_xml_files as $target_xml_path => $target_xml_file) {
      $target_xml = new DOMDocument();
      $target_xml->preserveWhiteSpace = FALSE;
      $target_xml->formatOutput = TRUE;
      $target_xml->load($target_xml_path);
      $target_xml_xpath = new DOMXPath($target_xml);

      if (!$prepare_combined) {
        $rss = $target_xml->getElementsByTagName('rss')->item(0);
        $rss_element = $combined_xml->createElement('rss');
        // Attributes.
        foreach ($rss->attributes as $attribute) {
          $rss_element->setAttribute($attribute->name, $attribute->value);
        }
        // Namespace attributes.
        foreach ($target_xml_xpath->query('namespace::*', $rss) as $node) {
          $rss_element->setAttribute($node->nodeName, $node->nodeValue);
        }
        $combined_xml->appendChild($rss_element);

        $channel_element = $combined_xml->createElement('channel');
        $rss_element->appendChild($channel_element);

        foreach ($tags_to_copy as $tag) {
          foreach ($target_xml_xpath->query('/descendant::' . $tag . '[1]') as $node) {
            $tag_element = $combined_xml->createElement($node->tagName, $node->nodeValue);
            $channel_element->appendChild($tag_element);
          }
        }

        if ($create_authors) {
          foreach ($target_xml_xpath->query('//wp:author') as $node) {
            $channel_element->appendChild($tags_xml->importNode($node, TRUE));
          }
        }

        if ($create_tags) {
          foreach ($target_xml_xpath->query('//wp:tag') as $node) {
            $channel_element->appendChild($tags_xml->importNode($node, TRUE));
          }
          foreach ($target_xml_xpath->query('//wp:category') as $node) {
            $channel_element->appendChild($tags_xml->importNode($node, TRUE));
          }
        }

        $prepare_combined = TRUE;
      }

      // Get all children.
      $node = $target_xml->getElementsByTagName('channel')->item(0);
      foreach ($node->childNodes as $child_node) {
        if (!in_array($node->tagName, $tags_to_copy)) {
          $channel_element->appendChild($combined_xml->importNode($child_node, TRUE));
        }
      }
    }

    $combined_xml_path = drupal_realpath($target_uri) . '/combined.xml';
    $combined_xml->save($combined_xml_path);
    drush_log(dt('Created combined file.'), 'ok');

    // Remove file when complete.
    foreach (array_keys($target_xml_files) as $target_xml_path) {
      unlink($target_xml_path);
    }
  }

  drush_print();
  drush_print(drupal_realpath($target_uri));
}

/**
 * Command callback for ew-wp-tax-rule-export.
 */
function drush_is_wordpress_migrate_is_wp_tax_rule_export() {
  $result = db_query('SELECT * FROM {migration_taxonomy_rules} ORDER BY source_term ASC');
  if ($result) {
    $target_uri = 'public://migration/wordpress/target/migration_taxonomy_rules.csv';
    $counter = $counter_skipped = 0;
    $fp = fopen(drupal_realpath($target_uri), 'w');
    $defaults = array(
      'rename_term' => '',
      'ignore_term' => '0',
      'person' => '0',
      'movie' => '0',
      'tv' => '0',
      'book' => '0',
      'music' => '0',
      'stage' => '0',
      'web_series' => '0',
      'video_game' => '0',
      'taxonomy_genre' => '0',
      'taxonomy_is_franchise' => '0',
      'taxonomy_franchise' => '0',
      'taxonomy_event' => '0',
      'taxonomy_freeform' => '0',
      'taxonomy_category' => '0',
    );
    $default_count = count($defaults);
    $skip_existing = drush_get_option('skip-existing');

    while ($row = $result->fetchAssoc()) {
      if ($counter++ == 0) {
        fputcsv($fp, array_keys($row));
      }
      $skip = TRUE;
      if ($skip_existing) {
        $default_elements = 0;
        foreach ($defaults as $key => $val) {
          if ($row[$key] == $val) {
            $default_elements++;
          }
        }
        if ($default_elements == $default_count) {
          $skip = FALSE;
        }
      }
      else {
        $skip = FALSE;
      }
      if ($skip) {
        $counter_skipped++;
      }
      else {
        fputcsv($fp, $row);
      }
    }
    fclose($fp);
    drush_log(dt('Wrote @counter rules (@skipped skipped) to @target', array(
      '@counter' => $counter,
      '@target' => drupal_realpath($target_uri),
      '@skipped' => $counter_skipped,
    )), 'success');
  }
  else {
    drush_log(dt('Unable to get migration taxonomy rules!'), 'error');
  }
}
