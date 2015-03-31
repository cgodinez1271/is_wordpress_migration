<?php
// @codingStandardsIgnoreFile
/*

Template name: WAP RSS

*/
include_once('theme_settings.php');
remove_shortcode('tiImage');
global $shortcode_image;
global $shortcode_thumb;
global $shortcode_medium;
global $shortcode_credit;
$shortcode_image = '';
$shortcode_thumb = '';
$shortcode_medium = '';
$shortcode_credit = '';
function wap_image_shortcode($atts, $content = null) {

	//echo "Hello!";
	extract(shortcode_atts(array(
			'url' => '',
			'alt' => '',
			'align' => 'left',
			'credit' => '',
			'caption' => '',
			'title' => ''
		), $atts) );
	global $shortcode_image;
	global $shortcode_thumb;
	global $shortcode_medium;
	global $shortcode_credit;
	$split_url = split("/", $url);
	$x = sizeof($split_url);
	$x = $x - 1;
	$alt_str = $split_url[$x];
	$alt_words = split("_", $alt_str);
	$x = sizeof($alt_words);
	$x = $x - 1;
	$size = str_replace(".jpg", '', $alt_words[$x]);
	if (is_integer($width))
	{
		$width = $size . 'px';
	}
	$alt_str = str_replace("_" . $size . ".jpg", '', $alt_str);

	if ($alt == '')
	{
		$alt = $alt_str;
	}
	if ($align == 'right')
	{
		$class .= ' alignright';
	}
	if ($align == 'center')
	{
		$class .= 'aligncenter';
	}
	if ($align == 'left')
	{
		$class .= ' alignleft';
	}
	if ($credit != '')
	{
		$class .= ' wp-caption';
		$shortcode_credit = $credit;
	}	
	if ($shortcode_image == '')
	{
		$shortcode_image = $url;
		$shortcode_thumb = str_replace("_" . $size . ".jpg", "_75.jpg", $url);
		$shortcode_medium = str_replace("_" . $size . ".jpg", "_175.jpg", $url);
	}
}
if ($channel_category == 'newsbriefs')
{
	$args = array('posts_per_page' => 25,
				  'numberposts' => 5,
				  'orderby' => 'date',
				  'order' => 'DESC',
				  'status' => 'publish');
}
else
{
	$args = array('posts_per_page' => 25,
				  'numberposts' => 5,
				  'orderby' => 'date',
				  'order' => 'DESC',
				  'category_name' => 'News',
				  'status' => 'publish');
}
$posts = query_posts($args);
$output = 'json';

	remove_all_filters('the_content');

	add_filter('the_content', 'wpautop');
	add_filter('the_content', 'ew_remove_shortcodes');
	add_filter('the_content', 'ew_remove_js');
	add_filter('the_content', 'ew_remove_images');
	add_filter('the_content', 'ew_remove_links');
	//add_filter('the_content', 'ew_remove_html');
	add_filter('the_content', 'ew_convert_smart_quotes');
if ($_GET[output] == 'rss')
{
	$output = 'rss';
}
if ($output == 'json')
{
	foreach($posts as $post)
	{
		setup_postdata($post);
		$content = get_the_content();
		//$image_content = $post->content;
		//$image = do_shortcode($image_content);
		$content = strip_shortcodes($content);
		$cleaned_content = apply_filters('the_content', $content);
		if (has_post_thumbnail())
		{
			$post_image_id = get_post_thumbnail_id($post->ID);
			//$post_array[$i]['image_thumb'] = get_the_post_thumbnail($post->ID);
			$image_thumb = wp_get_attachment_image_src($post_image_id, 'default', false, $attr );
			$shortcode_image = $image_thumb[0];		}
		else
		{
			$image_content = $content;
			add_shortcode('tiImage', 'wap_image_shortcode');
			$image_content = do_shortcode($image_content);
			remove_shortcode('tiImage');
		}
		//echo $image_content;
		//echo $shortcode_image;
		//$cleaned_content = $content;
		$wap_feed[] = array('blog_title' => get_bloginfo('name') . ' ' .get_bloginfo('description'),
							'desc' => get_bloginfo('description'),
							'url' => get_bloginfo('url'),
							'timestamp' => get_the_time('U'),
							'author' => get_the_author(),
							'post_title' => '<![CDATA[ ' . get_the_title() .' ]]>',
							'permalink' => get_permalink(),
							'content' => '<![CDATA[ ' . $cleaned_content . ' ]]>',
							'image' => array('default' => $shortcode_image, 'thumb' => $shortcode_thumb, 'medium' => $shortcode_medium),
							'credit' => '<![CDATA[ ' .$shortcode_credit . ' ]]>'
							);
							//the_content();
		$shortcode_image = '';
		$shortcode_thumb = '';
		$shortcode_medium = '';
		$shortcode_credit = '';
	}
	$as_json = json_encode($wap_feed);
	echo $as_json;
}
if ($output == 'rss')
{
header('Content-Type: ' . feed_content_type('rss') . '; charset=' . get_option('blog_charset'), true);
$more = 1;

echo '<?xml version="1.0" encoding="UTF-8"?>';
echo '<!-- generator="WordPress.com" -->';
?><rss version="0.92">
<channel>
	<title><?php bloginfo_rss('name'); wp_title_rss(); ?></title>
	<link><?php bloginfo_rss('url') ?></link>
	<description><?php bloginfo_rss('description') ?></description>
	<lastBuildDate><?php echo mysql2date('Y-m-d\TH:i:s\Z', get_lastpostmodified('blog'), false); ?></lastBuildDate>
	<docs>http://backend.userland.com/rss092</docs>

	<language>en</language>
	<image><title><?php bloginfo_rss('name'); wp_title_rss(); ?></title><url>http://img2.timeinc.net/ew/i/logoEW.gif</url><link><?php bloginfo_rss('url') ?></link><width>208</width><height>77</height><description><?php bloginfo_rss('description') ?></description></image>
	<?php
foreach($posts as $post)
{
	setup_postdata($post);
?>
		<item>
		<title><?php the_title_rss() ?></title>
		<description><![CDATA[<?php the_content('', 0, '') ?>]]></description>
		<link><?php the_permalink_rss() ?></link>
		<link><?php the_permalink_rss() ?></link>
		</item>
<?php
}
?>
</channel>
</rss>
<?php
}
?>
