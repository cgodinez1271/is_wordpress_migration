<?php
// @codingStandardsIgnoreFile
class add_tiImage_button {
	var $pluginname = "tiImageShortcode";
	
	function add_tiImage_button()  {
		add_filter('tiny_mce_version', array (&$this, 'change_tinymce_version') );
		
		add_action('init', array (&$this, 'addbuttons') );
	}

	function addbuttons() {	
		if ( !current_user_can('edit_posts') && !current_user_can('edit_pages') ) return;		
		if ( get_user_option('rich_editing') == 'true') {		 
			add_filter("mce_external_plugins", array (&$this, "add_tinymce_plugin" ), 5);
			add_filter('mce_buttons', array (&$this, 'register_button' ), 5);
		}
	}
	
	function register_button($buttons) {	
		array_push($buttons, "separator", 'tiImageShortcode' );
		return $buttons;
	}
	
	function add_tinymce_plugin($plugin_array) {	
		$plugin_array[$this->pluginname] = get_bloginfo('stylesheet_directory') . '/plugins/tiImage/editor_plugin.js';		
		return $plugin_array;
	}
	
	function change_tinymce_version($version) {
		return ++$version;
	}
	
}

$tinymce_button2 = new add_tiImage_button ();

/* Legacy shortcode */
function ti_image_shortcode($atts, $content = null) {
	extract(shortcode_atts(array(
			'url' => '',
			'alt' => '',
			'align' => 'left',
			'credit' => '',
			'caption' => '',
			'title' => ''
		), $atts) );
	global $post;
	$class = 'tiImage';
	if ($alt == '')
	{
		$split_url = split("/", $url);
		$x = sizeof($split_url);
		$x = $x - 1;
		$alt = $split_url[$x];
		$alt_words = split("_", $alt);
		$x = sizeof($alt_words);
		$x = $x - 1;
		$size = str_replace(".jpg", '', $alt_words[$x]);
		if (is_numeric($size))
		{
			$width = $size . 'px';
			$new_size = $size + 10;
			$style_extra = ' style="width: ' . $size . 'px;" ';
		}
		$alt = str_replace("_" . $size . ".jpg", '', $alt);
	}
	if ($align == 'right')
	{
		$class .= ' alignright';
	}
	if ($align == 'center')
	{
		$class .= ' aligncenter';
	}
	if ($align == 'left')
	{
		$class .= ' alignleft';
	}
	if ($align == 'none')
	{
		$class .= ' alignnone';
	}
	if ($credit != '')
	{
		$class .= ' wp-caption';
	}
	$text = '<span class="' . esc_attr($class) .'"';
	if ($width != '') 
	{
		//$text .= ' style="width: ' . esc_attr($width) . '"';
		$text .= $style_extra;
	}
	$text .= '><img src="' . esc_attr($url) .'" alt="' . esc_attr($alt) .'" />';
	if ($credit != '')
	{
		$text .= '<span ' . $style_extra . ' class="wp-credit-text">Image Credit: ' . esc_attr($credit) . '</span>';
	}
	$text .= '</span>';
	return $text;
}
add_shortcode('tiImage', 'ti_image_shortcode');



//Handler functions
function ti_image_shortcode_handler($wp) {	
	if ( is_user_logged_in() && current_user_can('edit_posts')) {
		?>
		<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
		<html xmlns="http://www.w3.org/1999/xhtml">
		<head>
			<title>TI Image Shortcode</title>
			<script type="text/javascript" src="<?php echo get_bloginfo('siteurl') ?>/wp-includes/js/jquery/jquery.js"></script>
			<script type="text/javascript" src="<?php echo get_bloginfo('siteurl') ?>/wp-includes/js/tinymce/tiny_mce_popup.js"></script>
			<script type="text/javascript" src="<?php echo get_bloginfo('siteurl') ?>/wp-includes/js/tinymce/utils/mctabs.js"></script>
			<script type="text/javascript" src="<?php echo get_bloginfo('siteurl') ?>/wp-includes/js/tinymce/utils/form_utils.js"></script>
			<script type="text/javascript" src="<?php echo get_bloginfo('stylesheet_directory') ?>/plugins/tiImage/js/tiImage.js"></script>
			<link href="<?php echo get_bloginfo('stylesheet_directory') ?>/plugins/tiImage/css/tiImage.css" rel="stylesheet" type="text/css" />
			<script type="text/javascript">
				function init() {
				tinyMCEPopup.resizeToInnerSize();
			}
			</script>
			<base target="_self" />
		</head>
		<body>
			<form id="tiImageForm">
			<div id="urlHolder">
				<label for="url">URL:</label>
				<input type="text" name="url" id="url"/>
			</div>
			<div id="altHolder">
				<label for="alt">Alt:</label>
				<input type="text" name="alt" id="alt"/>
			</div>
			<div id="creditHolder">
				<label for="credit">Credit:</label>
				<input type="text" name="credit" id="credit"/>
			</div>
			<div id="captionHolder">
				<label for="caption">Caption:</label>
				<input type="text" name="caption" id="caption"/>
			</div>
			<div id="titleHolder">
				<label for="title">Title:</label>
				<input type="text" name="title" id="title"/>
			</div>
			<div id="alignmentHolder">
				<div id="alignmentInput">
					<label for="align">Align:</label>
					<div class="left"><span>Left: </span><input type="radio" name="imageAlign" value="left" checked/></div>
					<div class="right"><span>Right: </span><input type="radio" name="imageAlign" value="right"/></div>
					<div class="center"><span>Center: </span><input type="radio" name="imageAlign" value="center"/></div>
					<div class="none"><span>None: </span><input type="radio" name="imageAlign" value="none"/></div>
				</div>
			</div>
			<button name="insert" id="insert">Insert</button>
			</form>
			<div id="errors"></div>
			<div id="preview"></div>
		</body>
		</html>
		<?php
		exit();
   	}	
}
add_action('wp_ajax_ti_image_shortcode', 'ti_image_shortcode_handler');


function ti_image_shortcode_vars($vars) {
    $vars[] = 'ti_image_shortcode';
    return $vars;
}
add_filter('query_vars', 'ti_image_shortcode_vars');

?>
