<?php
/**
 * @file
 * WordPress shortcode replacement functions.
 */

/**
 * VodPod shortcode replacement.
 *
 * @param string $subject
 *   The content.
 *
 * @return string
 *   Content with shortcode replacement.
 */
function is_wp_shortcode_replace_vodpod($subject) {
  $pattern = '|\[vodpod\040id\=(.*)\&amp\;w\=(.*)\&amp\;h\=(.*)\&amp\;fv\=(.*)\]|i';
  $replacement = '<embed src="http://widgets.vodpod.com/w/video_embed/$1" type="application/x-shockwave-flash" AllowScriptAccess="sameDomain" pluginspage="http://www.macromedia.com/go/getflashplayer" wmode="transparent" flashvars=" width="$2" height="$3" />';
  $ret_val = preg_replace($pattern, $replacement, $subject);
  return $ret_val;
}
