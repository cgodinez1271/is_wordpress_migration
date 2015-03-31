<?php
/**
 * @file
 * WordPress fakery.
 */
// @codingStandardsIgnoreFile
function wp_cache_get($a, $b) {}
function wp_cache_add($a, $b, $c) {}
function suppress_errors() {
  return TRUE;
}
function current_theme_supports($a) {
  return TRUE;
}
global $wpdb;
class fakeWdb {
  public function suppress_errors() {}
  public function get_results() {
    return array();
  }
}
$wpdb = new fakeWdb();

class fakePost {
  public $post_parent;
  public $ID;
}
function get_posts() {
  return array();
}
function get_post() {
  return new FakePost();
}
function get_children($a) {
  return array();
}
function is_admin() {
  return FALSE;
}
function get_the_ID() {}
function wp_enqueue_script($a, $b, $c, $d, $e) {}
function get_template_directory_uri() {}
