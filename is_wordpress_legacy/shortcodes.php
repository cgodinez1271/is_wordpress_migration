<?php
// @codingStandardsIgnoreFile

function pftv_brightcove_filter( $content ) {
	return preg_replace('#\[ewbrightcove "(\d+)", "(\d+)", "(\d+)", "(\d+)"]#', pftv_brightcove_script("$1", "$2", "$3", "$4"), $content);
}
function pftv_brightcove_script($playerid, $videoid, $width, $height) {
	if ($playerid == '2777811655001') $playerid = '2777811652001';
	return '<div style="display:none"></div>
	<script src="http://admin.brightcove.com/js/BrightcoveExperiences.js"></script>
	<div id="bcPlayerV' . esc_attr($videoid) . '">
	<object id="myExperience" class="BrightcoveExperience" style="width: '. esc_attr( $width ) .'px; height: '. esc_attr( $height ) . 'px; ">
	<param name="bgcolor" value="#FFFFFF" />
	<param name="width" value="' . esc_attr( $width ) .'" />
	<param name="height" value="' . esc_attr( $height ) .'" />
	<param name="playerID" value="' . esc_attr( $playerid ) .'" />
	<param name="publisherID" value="219646971"/>
	<param name="isVid" value="true" />
	<param name="isUI" value="true" />
	<param name="wmode" value="transparent" />
	<param name="@videoPlayer"  value="' . $videoid .'" />
	</object>
	</div>';
}
add_filter( 'the_content', 'pftv_brightcove_filter' );


function pftv_hulu_filter($content) {
	return preg_replace('#\[ewhulu "([a-zA-Z0-9]*)"]#', pftv_hulu_script("$1"), $content);
}
function pftv_hulu_script($id) {
	return '<object width="512" height="296"><param name="movie" value="' . esc_url( 'http://www.hulu.com/embed/' . $id ) . '"></param><param name="allowFullScreen" value="true"></param><embed src="' . esc_url( 'http://www.hulu.com/embed/' . $id ) . '" type="application/x-shockwave-flash" allowFullScreen="true"  width="512" height="296"></embed></object>';
}
add_filter( 'the_content', 'pftv_hulu_filter' );


function ew_playlist_filter($content) {
	return preg_replace('#\[ew_playlist "([a-zA-Z0-9\/\.\:\_]*)"]#', ew_playlist_script("$1"), $content);
}
function ew_playlist_script($id) {
	wp_enqueue_script('swfobject', '', array(), '');
	global $post;
	$text = '
	<div id="flashPlayer' . $post->ID .'">
	</div>
	<script src="http://ajax.googleapis.com/ajax/libs/swfobject/2.2/swfobject.js"></script>
	<script>
		var flashvars = {wmode:"transparent",autoplay:"no", playlistPath: "'. esc_js( $id ) . '"};
		var params = {};
		var attributes = {};
		swfobject.embedSWF("http://www.ew.com/ew/static/mp3s/playerMultipleList.swf", "flashPlayer' . $post->ID .'", "295", "200", "9.0.0", "#FFFFFF", flashvars);	
	</script>';
	return $text;
}
//add_filter( 'the_content', 'ew_playlist_filter' );

function coveritlive_shortcode( $atts, $content = null ) {
   extract(shortcode_atts(array(
		'code' => '',
		'name' => 'Coveritlive Event'
      ), $atts ) );
 	return '<iframe src="http://www.coveritlive.com/index2.php/option=com_altcaster/task=viewaltcast/altcast_code=' . $atts[code] . '/height=550/width=470" scrolling="no" height="550px" width="470px" frameBorder ="0" ><a href="http://www.coveritlive.com/mobile.php?option=com_mobile&task=viewaltcast&altcast_code=' . $atts[code] .'" >' . $atts[name] . '</a></iframe>';

}
add_shortcode('ew_coveritlive', 'coveritlive_shortcode');

function ew_mp3_shortcode( $atts, $content = null ) {
   extract(shortcode_atts(array(
		'mp3' => '',
      ), $atts ) );
 	global $post;
	$text = '
	<div id="singleflashPlayer' . $post->ID .'">
	</div>
	<script src="http://ajax.googleapis.com/ajax/libs/swfobject/2.2/swfobject.js"></script>
	<script>
		var flashvars = {soundPath:"' . esc_js( $atts[mp3] ) .'", autoplay:"no"};
		var params = {};
		var attributes = {};
		swfobject.embedSWF("http://www.ew.com/ew/static/mp3/singleplay/playerSingle.swf", "singleflashPlayer' . $post->ID .'", "192", "67", "9.0.0", "#FFFFFF", flashvars);
	</script>';
	return $text;
}
add_shortcode('ew_mp3', 'ew_mp3_shortcode');

function ew_twitter_shortcode($atts, $content = null) {
	extract(shortcode_atts(array(
		'search' => '',
		'title' => '',
		'subject' => '',
		'loop' => 'false'
      ), $atts ) );
	
	$text = '
	<div id="twtr-search-widget"></div>
	<script src="http://widgets.twimg.com/j/1/widget.js"></script>
	<link href="http://widgets.twimg.com/j/1/widget.css" rel="stylesheet">
	<script>
		new TWTR.Widget({ search: \''. esc_js($search) . '\', id: \'twtr-search-widget\', loop: '. esc_attr($loop) .', title: \'' . esc_js($title) .'\', subject: \''. esc_js($subject) .'\', width: 500, height: 300, theme: { shell: { background: \'#d4ebf7\', color: \'#2e7bbf\' }, tweets: { background: \'#ffffff\', color: \'#050214\', links: \'#1985b5\' } } }).render().start();
	</script>';
	return $text;

}
add_shortcode('ew_twitter', 'ew_twitter_shortcode');


function ew_hunch_shortcode($atts, $content = null) {
	extract(shortcode_atts(array(
			'width' => '500',
			'height' => '375',
			'size' => 'm',
			'topic_id' => ''
		), $atts) );
	$text = '<iframe width="'. esc_attr( $width ) .'" height="' . esc_attr( $height ) .'" frameBorder="0" marginHeight="0" marginWidth="0" scrolling="no" src="' . esc_url( 'http://api.hunch.com/api/widget/?size=' . $size .'&amp;border=1&amp;topicId=' . $topic_id ) .'"></iframe>';
	return $text;
}
add_shortcode('ew_hunch', 'ew_hunch_shortcode');

add_action('edit_category', 'category_updater');

function ew_nbc_video_shortcode( $atts, $content = null ) {
   extract(shortcode_atts(array(
		'width' => '500',
		'height' => '370',
		'url' => '',
      ), $atts ) );
	  
	$the_url = esc_attr($url);	
	$the_host = parse_url($the_url);
	$the_host = $the_host['host'];	
	$host_check = "nbc.com";
	
	if( strpos($the_host, $host_check) === false ) {
		$text = '';
	} else {		
		$the_url = str_replace("&amp;", "&", $the_url); //because NBC doesn't like "&amp;" in their URLs	
		$text = '
		<object classid="clsid:d27cdb6e-ae6d-11cf-96b8-444553540000" width="'. esc_attr($width) .'" height="'. esc_attr($height) .'" align="middle">
			<param name="allowScriptAccess" value="always" />
			<param name="allowFullScreen" value="true" />
			<param name="movie" value="'. $the_url .'"/>
			<param name="quality" value="high" />
			<param name="bgcolor" value="#000000" />
			<embed src="'. $the_url .'" quality="high" bgcolor="#000000" width="'. esc_attr($width) .'" height="'. esc_attr($height) .'" align="middle" allowFullScreen="true" allowScriptAccess="always" type="application/x-shockwave-flash"></embed>
		</object>';
	}
	return $text;
}
add_shortcode('ew_nbc_video', 'ew_nbc_video_shortcode');



/*
 * Amazon Affiliate Link Shortcode
 * Ex: [ew_amazon linktext="Buy the DVD" amazon_id="B003ZKBELG" style="bold italic"]
 */
function ew_amazon_aff_shortcode( $atts, $content = null ) {
	extract(shortcode_atts(array(
		'linktext' => 'Purchase this on Amazon',
		'amazon_id' => '',
		'style' => '',
	), $atts ) );
	
	$text = '<a href="http://www.amazon.com/dp/'. esc_attr($amazon_id) .'?tag=ewcom-20" class="affiliate-amazon '. esc_attr($style) .'" target="_blank">'. esc_attr($linktext) .'</a>';	
	return $text;	
}
add_shortcode('ew_amazon', 'ew_amazon_aff_shortcode');

/*
 * iTunes Affiliate Link Shortcode
 * Ex: [ew_itunes linktext="Rent the movie" url="http://itunes.apple.com/WebObjects/kj2f2f2f" style="bold italic"]
 */
function ew_itunes_aff_shortcode( $atts, $content = null ) {
	extract(shortcode_atts(array(
		'linktext' => 'Check this out on iTunes',
		'url' => '',
		'style' => '',
	), $atts ) );
	
	$the_url = $atts[url];
	$partner_id = 'partnerId=30&siteID=vqp0qzBz5hY';
	$partner_id = ( strpos($the_url, '?') === false ) ? '?'.$partner_id : '&'.$partner_id;
	
	$text = '<a href="'. $the_url . $partner_id .'" class="affiliate-itunes '. esc_attr($style) .'" target="_blank">'. esc_attr($linktext) .'</a>';	
	return $text;
}
add_shortcode('ew_itunes', 'ew_itunes_aff_shortcode');

/*
 * VYou embed Shortcode
 * Ex: [ew_vyou user="sleonard" display_name="Scott Leonard" size="full"]
 * *setting the size to "full" will change the layout and dimensions
 */
function ew_vyou_shortcode( $atts, $content = null ) {
	extract(shortcode_atts(array(
		'user' => '',
		'display_name' => '',
		'size' => '',
		'width' => '500',
		'height' => '702',
	), $atts ) );
	
	$display = ( esc_attr($display_name) == '' ) ? esc_attr($user) : esc_attr($display_name);
	$the_url = 'virtual_user='. esc_attr($user) .'&display_name='. $display .'&embed=true&player_style=vyouStyleSkinny01.swf';	
	$the_width = (int) $width;
	$the_height = (int) $height;
	
	if( esc_attr($size) === 'full' ) {
		$the_url = str_replace( '&player_style=vyouStyleSkinny01.swf', '', $the_url );
		$the_width = 710;
		$the_height = 740;
	}
	
	$text = '
	<object width="'. $the_width .'" height="'. $the_height .'">
		<param name="name" value="vyouPlayer" />
		<param name="movie" value="http://vyou.com/player/reg001" />
		<param name="FlashVars"	value="'. $the_url .'" />
		<param name="allowFullScreen" value="true" />
		<param name="allowscriptaccess" value="always" />
		<embed src="http://vyou.com/player/reg001" type="application/x-shockwave-flash" FlashVars="'. $the_url .'" allowscriptaccess="always" allowfullscreen="true" name="vyouPlayer" id="vyouPlayer" width="'. $the_width .'" height="'. $the_height .'"></embed>
	</object>';
	return $text;
}
add_shortcode('ew_vyou', 'ew_vyou_shortcode');




/*
 * Infomous tag cloud Shortcode
 * Ex: [infomous id="78" width="400" height="300" caption="true"]
 */
function infomous_shortcode( $atts, $content = null ) {
	$atts = shortcode_atts(array(
		'id' => '',
		'width' => '400',
		'height' => '300',
		'caption' => 'false'
	), $atts );
	
	$text = '<script src="http://infomous.com/cloud_widget/'. intval($atts['id']) .'?width='. intval($atts['width']) .'&height='. intval($atts['height']) .'&caption='. esc_js($atts['caption']) .'"></script>';
	
	return $text;
}
add_shortcode('infomous', 'infomous_shortcode');




/*
 * EW Kicker Link Shortcode
 * Ex: [ew_kicker_link kicker="More info" linktext="Go to our homepage" style="italic" link="http://ew.com"]
 */
function ew_kicker_link_shortcode( $atts, $content = null ) {
	extract($atts);	
	$text = '<p class="kicker-link"><span>'. esc_html($kicker) .'</span>: <a href="'. esc_url($link) .'" class="'. esc_attr($style) .'">'. esc_html($linktext) .'</a></p>';	
	return $text;	
}
add_shortcode('ew_kicker_link', 'ew_kicker_link_shortcode');



/*
 * IGN video embed Shortcode
 * Ex: [ew_ign_video url="http://www.ign.com/videos/2011/11/01/metal-gear-solid-hd-video-preview"]
 * only URL is required
 */
function ew_ign_video_shortcode( $atts, $content = null ) {
   extract(shortcode_atts(array(
		'custom_id' => 'vid_4eaf20f3132c9a5f6100003a',
		'width' => '480',
		'height' => '270',
		'url' => ''
    ), $atts ) );
	  
	$the_url = esc_attr($url);	
	$the_host = parse_url($the_url);
	$the_host = $the_host['host'];	
	$host_check = "ign.com";
	
	
	if( substr($the_host, -7) != $host_check ){ //if URL fails the host check then revert to old URL
		$text = '';
	} else {		
		$the_url = str_replace("&amp;", "&", $the_url);
		$text = '
		<object id="'. esc_attr($custom_id) .'" class="ign-videoplayer" width="'. intval($width) .'" height="'. intval($height) .'" data="http://media.ign.com/ev/prod/embed.swf" type="application/x-shockwave-flash">
			<param name="movie" value="http://media.ign.com/ev/prod/embed.swf" />
			<param name="allowfullscreen" value="true" />
			<param name="allowscriptaccess" value="always" />
			<param name="bgcolor" value="#000000" />
			<param name="flashvars" value="url='. $the_url .'"/>
		</object>';
	}
	return $text;
}
add_shortcode('ew_ign_video', 'ew_ign_video_shortcode');


/*
 * Storify embed Shortcode
 * Ex: [ew_storify src="EWstorify/madonna-super-bowl-halftime-show.js"]
 */
function ew_storify_shortcode( $atts, $content = null ) {
   extract(shortcode_atts(array(
		'src' => ''
    ), $atts ) );	
	
	if( $src == '' ){
		return;
	} else {
		$src = str_replace( "http://storify.com/", "", $src ); // in case the editors left the domain in the src we strip it out
		$link = str_replace( ".js", "", $src );
		$text = '<script src="'. esc_url( 'http://storify.com/'. $src ) .'"></script>
			<noscript>[<a href="'. esc_url( 'http://storify.com/'. $link ) .'" target="_blank">View this story on Storify</a>]</noscript>';
		
		return $text;
	}	
}
add_shortcode('ew_storify', 'ew_storify_shortcode');

/*
 * NBC video 2012
 * Ex: [ew_nbc id="1397382"]
 */
function ew_nbc_video_shortcode2( $atts, $content = null ) {
   extract(shortcode_atts(array(
		'width' => '510',
		'height' => '347',
		'id' => '',
      ), $atts ) );
	
	$text = '<iframe id="nbc-video-widget" width="'. intval( $width ) .'" height="'. intval( $height ) .'" src="http://www.nbc.com/assets/video/widget/widget.html?vid='. intval( $id ) .'" frameborder="0"></iframe>';
	
	return $text;
}
add_shortcode('ew_nbc', 'ew_nbc_video_shortcode2');


/*
 * HULU 2012
 * Ex: [ew_hulu id="YXBSTOmrHQlnrImuYvkbdw"]
 */
function ew_hulu_shortcode( $atts, $content = null ) {
   extract(shortcode_atts(array(
		'width' => '510',
		'height' => '288',
		'id' => '',
      ), $atts ) );
	
	$text = '<div class="ew-video-embed ew-video-hulu">
		<object width="512" height="288">
			<param name="movie" value="http://www.hulu.com/embed/'. esc_attr( $id ) .'"></param>
			<param name="allowFullScreen" value="true"></param>
			<param name="wmode" value="transparent"></param>	
			<embed src="http://www.hulu.com/embed/'. esc_attr( $id ) .'" type="application/x-shockwave-flash" wmode="transparent" width="'. intval( $width ) .'" height="'. intval( $height ) .'" allowFullScreen="true"></embed>
		</object></div>';
	
	return $text;
}
add_shortcode('ew_hulu', 'ew_hulu_shortcode');


/*
 * Funny or Die video 2012
 * Ex: [ew_funnyordie id="7f070b8b36"]
 */
function ew_funnyordie_shortcode( $atts, $content = null ) {
   extract(shortcode_atts(array(
		'width' => '510',
		'height' => '328',
		'id' => '',
      ), $atts ) );
	
	$text = '<iframe src="http://www.funnyordie.com/embed/'. esc_attr( $id ) .'" width="'. intval( $width ) .'" height="'. intval( $height ) .'" frameborder="0"></iframe>';
	
	return $text;
}
add_shortcode('ew_funnyordie', 'ew_funnyordie_shortcode');


/*
 * Bravo video 2012
 * Ex: [ew_bravo id="_vid18481241"]
 */
function ew_bravo_shortcode( $atts, $content = null ) {
   extract(shortcode_atts(array(
		'width' => '510',
		'height' => '290',
		'id' => '',
      ), $atts ) );
	
	$text = '<iframe src="http://www.bravotv.com/video/embed/?/'. esc_attr( $id ) .'" width="'. intval( $width ) .'" height="'. intval( $height ) .'" frameborder="0" scrolling="no"></iframe>';
	
	return $text;
}
add_shortcode('ew_bravo', 'ew_bravo_shortcode');


/*
 * VYou player 2012
 * Ex: [ew_vyou_player id="embed/profile/widget/single/nid/1597245"]
 */
function ew_vyou_player_shortcode( $atts, $content = null ) {
   extract(shortcode_atts(array(
		'width' => '510',
		'height' => '700',
		'id' => '',
      ), $atts ) );
	
	$text = '<iframe src="http://vyou.com/'. esc_attr( $id ) .'/width/'. intval( $width ) .'/height/'. intval( $height ) .'" width="'. intval( $width ) .'" height="'. intval( $height ) .'" frameborder="0" scrolling="no" marginheight="0" marginwidth="0"></iframe>';
	$text = '';
	
	return $text;
}
add_shortcode('ew_vyou_player', 'ew_vyou_player_shortcode');


/*
 * MTV video 2012
 * Ex: [ew_mtv id="mgid:uma:video:mtv.com:785886"]
 */
function ew_mtv_shortcode( $atts, $content = null ) {
   extract(shortcode_atts(array(
		'width' => '510',
		'height' => '288',
		'id' => '',
      ), $atts ) );
	
	$text = '<iframe src="http://media.mtvnservices.com/'. esc_attr( $id ) .'" width="'. intval( $width ) .'" height="'. intval( $height ) .'" frameborder="0" scrolling="no" marginheight="0" marginwidth="0"></iframe>';
	
	return $text;
}
add_shortcode('ew_mtv', 'ew_mtv_shortcode');


/*
 * Trailer Addict 2012
 * Ex: [ew_trailer_addict id="57477"]
 */
function ew_trailer_addict_shortcode( $atts, $content = null ) {
   extract(shortcode_atts(array(
		'width' => '510',
		'height' => '280',
		'id' => '',
      ), $atts ) );
	
	$text = '<div class="ew-video-embed ew-video-trailer-addict"><object width="'. intval( $width ) .'" height="'. intval( $height ) .'">
			<param name="movie" value="http://www.traileraddict.com/emd/'. esc_attr( $id ) .'"></param>
			<param name="allowscriptaccess" value="always">
			<param name="wmode" value="transparent">
			</param><param name="allowFullScreen" value="true"></param>
			<embed src="http://www.traileraddict.com/emd/'. esc_attr( $id ) .'" width="'. intval( $width ) .'" height="'. intval( $height ) .'" type="application/x-shockwave-flash" allowscriptaccess="always" wmode="transparent" allowFullScreen="true"></embed>
		</object></div>';
	
	return $text;
}
add_shortcode('ew_trailer_addict', 'ew_trailer_addict_shortcode');


/*
 * TMZ video 2012
 * Ex: [ew_tmz id="index.php/kwidget/wid/0_bnom7t7w/uiconf_id/6740162"]
 */
function ew_tmz_shortcode( $atts, $content = null ) {
   extract(shortcode_atts(array(
		'width' => '510',
		'height' => '336',
		'id' => '',
      ), $atts ) );
	
	$text = '<div class="ew-video-embed ew-video-tmz"><object width="'. intval( $width ) .'" height="'. intval( $height ) .'" data="http://cdnapi.kaltura.com/'. esc_attr( $id ) .'" type="application/x-shockwave-flash" allowScriptAccess="always" allowNetworking="all" allowFullScreen="true">
			<param name="movie" value="http://cdnapi.kaltura.com/'. esc_attr( $id ) .'" />
			
			<param name="allowScriptAccess" value="always" />
			<param name="allowNetworking" value="all" />
			<param name="allowFullScreen" value="true" />
			<param name="bgcolor" value="#000000" />
			<param name="flashVars" value=""/>
			<param name="wmode" value="transparent" />
		</object></div>';
	
	return $text;
}
add_shortcode('ew_tmz', 'ew_tmz_shortcode');


/*
 * College Humor 2012
 * Ex: [ew_college_humor id="6774659"]
 */
function ew_college_humor_shortcode( $atts, $content = null ) {
   extract(shortcode_atts(array(
		'width' => '510',
		'height' => '320',
		'id' => '',
      ), $atts ) );
	
	$text = '<iframe src="http://www.collegehumor.com/e/'. esc_attr( $id ) .'" width="'. intval( $width ) .'" height="'. intval( $height ) .'" frameborder="0" webkitAllowFullScreen allowFullScreen></iframe>';
	
	return $text;
}
add_shortcode('ew_college_humor', 'ew_college_humor_shortcode');


/*
 * CW video 2012
 * Ex: [ew_cw id="mediaKey=bd1ee4a5-8191-4ee1-9733-f7b5f24e5c2d&config=embed.xml"]
 */
function ew_cw_shortcode( $atts, $content = null ) {
   extract(shortcode_atts(array(
		'width' => '510',
		'height' => '270',
		'id' => '',
      ), $atts ) );
	
	$text = '<div class="ew-video-embed ew-video-cw"><object width="'. intval( $width ) .'" height="'. intval( $height ) .'" id="embed" align="middle">
			<param name="flashVars" value="'. esc_attr( $id ) .'" />
			<param name="allowScriptAccess" value="always" />
			<param name="allowFullScreen" value="true" />
			<param name="movie" value="http://pdl.warnerbros.com/cwtv/digital-smiths/production_player/vsplayer.swf" />
			<param name="quality" value="high" />
			<param name="bgcolor" value="#ffffff" />
			<param name="wmode" value="transparent" />
			<embed flashVars="'. esc_attr( $id ) .'" width="'. intval( $width ) .'" height="'. intval( $height ) .'" name="embed" src="http://pdl.warnerbros.com/cwtv/digital-smiths/production_player/vsplayer.swf" align="middle" allowScriptAccess="always" allowFullScreen="true"></embed>
		</object></div>';
	
	return $text;
}
add_shortcode('ew_cw', 'ew_cw_shortcode');


/*
 * Comedy Central video 2012
 * Ex: [ew_comedy_central id="mgid:cms:video:colbertnation.com:414517"]
 */
function ew_comedy_central_shortcode( $atts, $content = null ) {
   extract(shortcode_atts(array(
		'width' => '510',
		'height' => '288',
		'id' => '',
      ), $atts ) );
	
	$text = '<iframe src="http://media.mtvnservices.com/embed/'. esc_attr( $id ) .'" width="'. intval( $width ) .'" height="'. intval( $height ) .'" frameborder="0"></iframe>';
	
	return $text;
}
add_shortcode('ew_comedy_central', 'ew_comedy_central_shortcode');


/*
 * Yahoo video 2012
 * Ex: [ew_yahoo id="29533555"]
 */
function ew_yahoo_shortcode( $atts, $content = null ) {
   extract(shortcode_atts(array(
		'width' => '510',
		'height' => '306',
		'id' => '',
      ), $atts ) );
	
	$text = '<iframe width="'. intval( $width ) .'" height="'. intval( $height ) .'" src="http://d.yimg.com/nl/vyc/site/player.html#vid='. esc_attr( $id ) .'" frameborder="0"></iframe>';
	
	return $text;
}
add_shortcode('ew_yahoo', 'ew_yahoo_shortcode');


/*
 * HBO 2012
 * Ex: [ew_hbo id="1260199"]
 */
function ew_hbo_shortcode( $atts, $content = null ) {
   extract(shortcode_atts(array(
		'width' => '510',
		'height' => '288',
		'id' => '',
      ), $atts ) );
	
	$text = '<div class="ew-video-embed ew-video-hbo"><object width="'. intval( $width ) .'" height="'. intval( $height ) .'">
			<param name="movie" value="http://www.hbo.com/bin/hboPlayerV2.swf?vid='. esc_attr( $id ) .'"></param>
			<param name="FlashVars" value="domain=http://www.ew.com"></param>
			<param name="allowFullScreen" value="true"></param>
			<param name="allowscriptaccess" value="always"></param>
			<param name="wmode" value="transparent" />
			<embed src="http://www.hbo.com/bin/hboPlayerV2.swf?vid='. esc_attr( $id ) .'" FlashVars="domain=http://www.hbo.com&videoTitle=Ep. 9: Preview&copyShareURL=http%3A//www.hbo.com/video/video.html/%3Fautoplay%3Dtrue%26vid%3D1260199%26filter%3Dgirls%26view%3Dnull" type="application/x-shockwave-flash" allowscriptaccess="always" allowfullscreen="true"  width="'. intval( $width ) .'" height="'. intval( $height ) .'"></embed>
		</object></div>';
	
	return $text;
}
add_shortcode('ew_hbo', 'ew_hbo_shortcode');


/*
 * Showtime video 2012
 * Ex: [ew_showtime id="1622097218001"]
 */
function ew_showtime_shortcode( $atts, $content = null ) {
   extract(shortcode_atts(array(
		'width' => '510',
		'height' => '440',
		'id' => '',
      ), $atts ) );
	
	$text = '<div class="ew-video-embed ew-video-showtime"><object width="'. intval( $width ) .'" height="'. intval( $height ) .'" classid="clsid:D27CDB6E-AE6D-11cf-96B8-444553540000" codebase="http://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=9,0,47,0">
			<param name="movie" value="http://c.brightcove.com/services/viewer/federated_f9?isVid=1" />
			<param name="bgcolor" value="#FFFFFF" />
			<param name="flashVars" value="videoId='. esc_attr( $id ) .'&playerID=29474209001&playerKey=AQ~~,AAAAAAAA9pg~,GnOHJwU2r3sFsJRSf1bvZ_iPYmWg8io0&domain=embed&dynamicStreaming=true" />
			<param name="base" value="http://admin.brightcove.com" />
			<param name="seamlesstabbing" value="false" />
			<param name="allowFullScreen" value="true" />
			<param name="swLiveConnect" value="true" />
			<param name="allowScriptAccess" value="always" />
			<param name="wmode" value="transparent" />
			<embed width="'. intval( $width ) .'" height="'. intval( $height ) .'" flashVars="videoId='. esc_attr( $id ) .'&playerID=29474209001&playerKey=AQ~~,AAAAAAAA9pg~,GnOHJwU2r3sFsJRSf1bvZ_iPYmWg8io0&domain=embed&dynamicStreaming=true" base="http://admin.brightcove.com" name="flashObj" src="http://c.brightcove.com/services/viewer/federated_f9?isVid=1" bgcolor="#FFFFFF" seamlesstabbing="false" type="application/x-shockwave-flash" allowFullScreen="true" swLiveConnect="true" allowScriptAccess="always" pluginspage="http://www.macromedia.com/shockwave/download/index.cgi?P1_Prod_Version=ShockwaveFlash"></embed>
		</object></div>';
	
	return $text;
}
add_shortcode('ew_showtime', 'ew_showtime_shortcode');



/*
 * E! Online 2012
 * Ex: [ew_eonline id="TFLT8NVCWgLjreQHRnPvY2c1yAUyB_NG"]
 * Warning: at dev time this embed autoplays
 */
function ew_eonline_shortcode( $atts, $content = null ) {
   extract(shortcode_atts(array(
		'width' => '510',
		'height' => '290',
		'id' => '',
      ), $atts ) );
	
	$text = '<div class="ew-video-embed ew-video-eonline">
		<object width="'. intval( $width ) .'" height="'. intval( $height ) .'" classid="clsid:d27cdb6e-ae6d-11cf-96b8-444553540000" codebase="http://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=10,0,0,0" id="CEGEmbed">
			<param name="allowScriptAccess" value="always" />
			<param name="allowFullScreen" value="true" />
			<param name="movie" value="http://www.eonline.com/static/videoplayer/platform_players/swf/CEGDynamicPlayer.swf" />
			<param name="quality" value="high" />
			<param name="bgcolor" value="#000000" />
			<param name="wmode" value="transparent" />
			<param name="flashvars" value="width='. intval( $width ) .'&height='. intval( $height ) .'&ID=Embed&releasePID='. esc_attr( $id ) .'&playerId=Embed&skinUrl=http://www.eonline.com/static/videoplayer/platform_players/swf/skinCEGPlayer.swf&locId=US" />
			<embed width="'. intval( $width ) .'" height="'. intval( $height ) .'" flashvars="width='. intval( $width ) .'&height='. intval( $height ) .'&ID=Embed&releasePID='. esc_attr( $id ) .'&playerId=Embed&skinUrl=http://www.eonline.com/static/videoplayer/platform_players/swf/skinCEGPlayer.swf&locId=US" allowScriptAccess="always" allowfullscreen="true" wmode="transparent" quality="high" bgcolor="#000000" name="CEGEmbed" id="CEGEmbed" src="http://www.eonline.com/static/videoplayer/platform_players/swf/CEGDynamicPlayer.swf" type="application/x-shockwave-flash"/>
		</object></div>';
	
	return $text;
}
add_shortcode('ew_eonline', 'ew_eonline_shortcode');



/*
 * Huffpo 2012
 * Ex: [ew_huffpo id="517387904"]
 */
function ew_huffpo_shortcode( $atts, $content = null ) {
   extract(shortcode_atts(array(
		'width' => '510',
		'height' => '310',
		'id' => '',
      ), $atts ) );
	
	$text = '<div class="ew-video-embed ew-video-huffpo"><object width="'. intval( $width ) .'" height="'. intval( $height ) .'" id="FiveminPlayer" classid="clsid:d27cdb6e-ae6d-11cf-96b8-444553540000">
			<param name="allowfullscreen" value="true"/>
			<param name="allowScriptAccess" value="always"/>
			<param name="movie" value="http://embed.5min.com/'. esc_attr( $id ) .'/"/>
			<param name="wmode" value="transparent" />
			<embed src="http://embed.5min.com/'. esc_attr( $id ) .'/" width="'. intval( $width ) .'" height="'. intval( $height ) .'" name="FiveminPlayer" type="application/x-shockwave-flash" allowfullscreen="true" allowScriptAccess="always" wmode="opaque">
			</embed>
		</object></div>';
	
	return $text;
}
add_shortcode('ew_huffpo', 'ew_huffpo_shortcode');



/*
 * Celebuzz 2012
 * Ex: [ew_celebuzz id="1673029729001"]
 */
function ew_celebuzz_shortcode( $atts, $content = null ) {
   extract(shortcode_atts(array(
		'width' => '510',
		'height' => '400',
		'id' => '',
      ), $atts ) );
	
	$text = '<div class="ew-video-embed ew-video-celebuzz">
		<object width="'. intval( $width ) .'" height="'. intval( $height ) .'" classid="clsid:D27CDB6E-AE6D-11cf-96B8-444553540000" codebase="http://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=9,0,47,0">
			<param name="movie" value="http://c.brightcove.com/services/viewer/federated_f9?isVid=1&isUI=1" />
			<param name="bgcolor" value="#FFFFFF" />
			<param name="flashVars" value="videoId='. esc_attr( $id ) .'&playerID=111285850001&playerKey=AQ~~,AAAAFOjEdNk~,Ccf1BJmYkhML2OA3FEsF4-tPam_YW9WW&domain=embed&dynamicStreaming=true" />
			<param name="base" value="http://admin.brightcove.com" />
			<param name="seamlesstabbing" value="false" />
			<param name="allowFullScreen" value="true" />
			<param name="swLiveConnect" value="true" />
			<param name="allowScriptAccess" value="always" />
			<param name="wmode" value="transparent" />
			<embed width="'. intval( $width ) .'" height="'. intval( $height ) .'" flashVars="videoId='. esc_attr( $id ) .'&playerID=111285850001&playerKey=AQ~~,AAAAFOjEdNk~,Ccf1BJmYkhML2OA3FEsF4-tPam_YW9WW&domain=embed&dynamicStreaming=true" base="http://admin.brightcove.com" name="flashObj" src="http://c.brightcove.com/services/viewer/federated_f9?isVid=1&isUI=1" bgcolor="#FFFFFF" seamlesstabbing="false" type="application/x-shockwave-flash" allowFullScreen="true" allowScriptAccess="always" swLiveConnect="true" pluginspage="http://www.macromedia.com/shockwave/download/index.cgi?P1_Prod_Version=ShockwaveFlash"></embed>
		</object></div>';
	
	return $text;
}
add_shortcode('ew_celebuzz', 'ew_celebuzz_shortcode');


/*
 * IFC 2012
 * Ex: [ew_ifc id="1666728230001"]
 */
function ew_ifc_shortcode( $atts, $content = null ) {
   extract(shortcode_atts(array(
		'width' => '510',
		'height' => '300',
		'id' => '',
      ), $atts ) );
	
	$text = '<div class="ew-video-embed ew-video-ifc">
		<object width="'. intval( $width ) .'" height="'. intval( $height ) .'" classid="clsid:D27CDB6E-AE6D-11cf-96B8-444553540000" codebase="http://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=9,0,47,0">
			<param name="movie" value="http://c.brightcove.com/services/viewer/federated_f9?isVid=1&isUI=1" />
			<param name="bgcolor" value="#FFFFFF" />
			<param name="flashVars" value="videoId='. esc_attr( $id ) .'&playerID=88218671001&playerKey=AQ~~,AAAAAAAn_zM~,B6LaFUvNnt2RhwK5cjOvZ4hHQyd5XXC9&domain=embed&dynamicStreaming=true" />
			<param name="base" value="http://admin.brightcove.com" /><param name="seamlesstabbing" value="false" />
			<param name="allowFullScreen" value="true" />
			<param name="swLiveConnect" value="true" />
			<param name="allowScriptAccess" value="always" />
			<param name="wmode" value="transparent" />
			<embed width="'. intval( $width ) .'" height="'. intval( $height ) .'" flashVars="videoId='. esc_attr( $id ) .'&playerID=88218671001&playerKey=AQ~~,AAAAAAAn_zM~,B6LaFUvNnt2RhwK5cjOvZ4hHQyd5XXC9&domain=embed&dynamicStreaming=true" src="http://c.brightcove.com/services/viewer/federated_f9?isVid=1&isUI=1" bgcolor="#FFFFFF" base="http://admin.brightcove.com" name="flashObj" seamlesstabbing="false" type="application/x-shockwave-flash" allowFullScreen="true" allowScriptAccess="always" swLiveConnect="true" pluginspage="http://www.macromedia.com/shockwave/download/index.cgi?P1_Prod_Version=ShockwaveFlash"></embed>
		</object></div>';
	
	return $text;
}
add_shortcode('ew_ifc', 'ew_ifc_shortcode');


/*
 * AMC 2012
 * Ex: [ew_amc id="1677080827001"]
 */
function ew_amc_shortcode( $atts, $content = null ) {
   extract(shortcode_atts(array(
		'width' => '510',
		'height' => '300',
		'id' => '',
      ), $atts ) );
	
	$text = '<div class="ew-video-embed ew-video-amc">
		<object width="'. intval( $width ) .'" height="'. intval( $height ) .'" classid="clsid:D27CDB6E-AE6D-11cf-96B8-444553540000" codebase="http://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=9,0,47,0">
			<param name="movie" value="http://c.brightcove.com/services/viewer/federated_f9?isVid=1" />
			<param name="bgcolor" value="#FFFFFF" />
			<param name="flashVars" value="videoId='. esc_attr( $id ) .'&playerID=83327935001&playerKey=AQ~~,AAAAAAuyCbQ~,-gfAmfm8njJ8S-9E4q2UfzG931rvkxuP&domain=embed&dynamicStreaming=true" />
			<param name="base" value="http://admin.brightcove.com" />
			<param name="seamlesstabbing" value="false" />
			<param name="allowFullScreen" value="true" />
			<param name="swLiveConnect" value="true" />
			<param name="allowScriptAccess" value="always" />
			<param name="wmode" value="transparent" />
			<embed width="'. intval( $width ) .'" height="'. intval( $height ) .'" flashVars="videoId='. esc_attr( $id ) .'&playerID=83327935001&playerKey=AQ~~,AAAAAAuyCbQ~,-gfAmfm8njJ8S-9E4q2UfzG931rvkxuP&domain=embed&dynamicStreaming=true" src="http://c.brightcove.com/services/viewer/federated_f9?isVid=1" bgcolor="#FFFFFF" base="http://admin.brightcove.com" name="flashObj" seamlesstabbing="false" type="application/x-shockwave-flash" allowFullScreen="true" swLiveConnect="true" allowScriptAccess="always" pluginspage="http://www.macromedia.com/shockwave/download/index.cgi?P1_Prod_Version=ShockwaveFlash"></embed>
		</object></div>';
	
	return $text;
}
add_shortcode('ew_amc', 'ew_amc_shortcode');


/*
 * ABC 2012
 * Ex: [ew_abc id="m/vp2/sfp/prod/v1.0.3/xml/abc/conf_embed.xml?&configId=406732&playlistId=PL55179004&clipId=VD55208872&showId=SH015427390000&gig_lt=1339108768187&gig_pt=1339108770421&gig_g=2"]
 */
function ew_abc_shortcode( $atts, $content = null ) {
   extract(shortcode_atts(array(
		'width' => '510',
		'height' => '300',
		'id' => '',
      ), $atts ) );
	
	$text = '<div class="ew-video-embed ew-video-abc">
		<object width="'. intval( $width ) .'" height="'. intval( $height ) .'" classid="clsid:D27CDB6E-AE6D-11cf-96B8-444553540000" codebase="http://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=9,0,124,0" id="ABCESNWID">
			<param name="movie" value="http://a.abc.com/media/_global/swf/embed/2.6.13/SFP_Walt.swf" />
			<param name="quality" value="high" />
			<param name="allowScriptAccess" value="always" />
			<param name="allowNetworking" value="all" />
			<param name="flashvars" value="configUrl=http://ll.static.abc.com/'. esc_attr( $id ) .'" />
			<param name="allowfullscreen" value="true" />
			<param name="wmode" value="transparent" />
			<embed width="'. intval( $width ) .'" height="'. intval( $height ) .'" flashvars="configUrl=http://ll.static.abc.com/'. esc_attr( $id ) .'" src="http://a.abc.com/media/_global/swf/embed/2.6.13/SFP_Walt.swf" quality="high" allowScriptAccess="always" allowNetworking="all" allowfullscreen="true" pluginspage="http://www.adobe.com/shockwave/download/download.cgi?P1_Prod_Version=ShockwaveFlash" type="application/x-shockwave-flash" name="ABCESNWID"></embed>
		</object></div>';
	
	return $text;
}
add_shortcode('ew_abc', 'ew_abc_shortcode');


/*
 * MSN 2012
 * Ex: [ew_msn id="embed/83a49316-634b-48b7-9b41-998dd2d1dd19/?vars=bWt0PWVuLXVzJmJyYW5kPXY1JTVlNTQ0eDMwNiZsaW5rb3ZlcnJpZGUyPWh0dHAlM2ElMmYlMmZ3d3cuYmluZy5jb20lMmZ2aWRlb3MlMmZicm93c2UlM2Zta3QlM2Rlbi11cyUyNnZpZCUzZCU3YjAlN2QlMjZmcm9tJTNkZW4tdXNfbXNuaHAmbGlua2JhY2s9aHR0cCUzYSUyZiUyZnd3dy5iaW5nLmNvbSUyZnZpZGVvcyUyZmJyb3dzZSZjb25maWdDc2lkPU1TTlZpZGVvJmNvbmZpZ05hbWU9c3luZGljYXRpb25wbGF5ZXImc3luZGljYXRpb249dGFnJnBsYXllci5mcj1zaGFyZWVtYmVkLXN5bmRpY2F0aW9u"]
 */
function ew_msn_shortcode( $atts, $content = null ) {
   extract(shortcode_atts(array(
		'width' => '510',
		'height' => '300',
		'id' => '',
      ), $atts ) );
	
	$text = '<div class="ew-video-embed ew-video-msn">
			<iframe width="'. intval( $width ) .'" height="'. intval( $height ) .'" src="http://hub.video.msn.com/'. esc_attr( $id ) .'" frameborder="0" marginwidth="0" marginheight="0" scrolling="no" ></iframe>
		</div>';
	
	return $text;
}
add_shortcode('ew_msn', 'ew_msn_shortcode');


/*
 * CMT 2012
 * Ex: [ew_cmt id="mgid:uma:video:cmt.com:788111/cp~series%3D1955%26id%3D1686802%26vid%3D788111%26uri%3Dmgid%3Auma%3Avideo%3Acmt.com%3A788111"]
 */
function ew_cmt_shortcode( $atts, $content = null ) {
   extract(shortcode_atts(array(
		'width' => '510',
		'height' => '300',
		'id' => '',
      ), $atts ) );
	
	$text = '<div class="ew-video-embed ew-video-msn">
			<embed src="http://media.mtvnservices.com/'. esc_attr( $id ) .'" width="'. intval( $width ) .'" height="'. intval( $height ) .'" type="application/x-shockwave-flash" allowFullScreen="true" allowScriptAccess="always" base="." flashVars=""></embed>
		</div>';
	
	return $text;
}
add_shortcode('ew_cmt', 'ew_cmt_shortcode');


/*
 * Springboard 2012
 * Ex: [ew_springboard id="mediaplayer/springboard/video/sh007/81/504055/"]
 */
function ew_springboard_shortcode( $atts, $content = null ) {
   extract(shortcode_atts(array(
		'width' => '510',
		'height' => '330',
		'id' => '',
      ), $atts ) );
	
	$text = '<div class="ew-video-embed ew-video-cmt">
		<object width="'. intval( $width ) .'" height="'. intval( $height ) .'" id="sbPlayer" type="application/x-shockwave-flash" classid="clsid:D27CDB6E-AE6D-11cf-96B8-444553540000">
			<param name="movie" value="http://www.springboardplatform.com/'. esc_attr( $id ) .'"></param>
			<param name="allowFullScreen" value="true"></param>
			<param name="allowscriptaccess" value="always"></param>
			<param name="wmode" value="transparent"></param>
			<embed width="'. intval( $width ) .'" height="'. intval( $height ) .'" src="http://www.springboardplatform.com/'. esc_attr( $id ) .'" type="application/x-shockwave-flash" allowscriptaccess="always" allowfullscreen="true" wmode="transparent"></embed>
		</object></div>';
	
	return $text;
}
add_shortcode('ew_springboard', 'ew_springboard_shortcode');

/*
 * MSNBC 2012
 * Ex: [ew_msnbc id="47828591"]
 */
function ew_msnbc_shortcode( $atts, $content = null ) {
   extract(shortcode_atts(array(
		'width' => '510',
		'height' => '300',
		'id' => '',
		'playerid' => '32545640'
      ), $atts ) );
	
	$text = '<div class="ew-video-embed ew-video-abc">
		<object width="'. intval( $width ) .'" height="'. intval( $height ) .'" id="msnbc2f5f33" classid="clsid:D27CDB6E-AE6D-11cf-96B8-444553540000" codebase="http://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=10,0,0,0">
			<param name="movie" value="http://www.msnbc.msn.com/id/'. esc_attr( $playerid ) .'" />
			<param name="FlashVars" value="launch='. esc_attr( $id ) .'&amp;width='. intval( $width ) .'&amp;height='. intval( $height ) .'" />
			<param name="allowScriptAccess" value="always" />
			<param name="allowFullScreen" value="true" />
			<param name="wmode" value="transparent" />
			<embed name="msnbc2f5f33" src="http://www.msnbc.msn.com/id/'. esc_attr( $playerid ) .'" width="'. intval( $width ) .'" height="'. intval( $height ) .'" FlashVars="launch='. esc_attr( $id ) .'&amp;width='. intval( $width ) .'&amp;height='. intval( $height ) .'" allowscriptaccess="always" allowFullScreen="true" wmode="transparent" type="application/x-shockwave-flash" pluginspage="http://www.adobe.com/shockwave/download/download.cgi?P1_Prod_Version=ShockwaveFlash"></embed>
		</object></div>';
	
	return $text;
}
add_shortcode('ew_msnbc', 'ew_msnbc_shortcode');


/*
 * CBS 2012
 * Ex: [ew_cbs id="e/t_TWlyS3M70eCFz01XuddVrVeN__gCT8/cbs/1/"]
 */
function ew_cbs_shortcode( $atts, $content = null ) {
   extract(shortcode_atts(array(
		'width' => '510',
		'height' => '300',
		'id' => ''
      ), $atts ) );
	
	$text = '<div class="ew-video-embed ew-video-cbs">
		<object width="'. intval( $width ) .'" height="'. intval( $height ) .'">
			<param name="movie" value="http://www.cbs.com/'. esc_attr( $id ) .'" /></param>
			<param name="allowFullScreen" value="true"></param>
			<param name="allowScriptAccess" value="always"></param>
			<param name="wmode" value="transparent"></param>
			<embed width="'. intval( $width ) .'" height="'. intval( $height ) .'" src="http://www.cbs.com/'. esc_attr( $id ) .'" allowFullScreen="true" allowScriptAccess="always" type="application/x-shockwave-flash"></embed>
		</object></div>';
	
	return $text;
}
add_shortcode('ew_cbs', 'ew_cbs_shortcode');



/*
 * CNN 2012
 * Ex: [ew_cnn id="cnn/.element/apps/cvp/3.0/swf/cnn_416x234_embed.swf?context=embed&contentId=bestoftv/2012/07/20/early-batman-shooting-cell-video.cnn"]
 */
function ew_cnn_shortcode( $atts, $content = null ) {
   extract(shortcode_atts(array(
		'width' => '416',
		'height' => '374',
		'id' => ''
      ), $atts ) );
	
	$text = '<div class="ew-video-embed ew-video-cbs">
		<object width="'. intval( $width ) .'" height="'. intval( $height ) .'" classid="clsid:D27CDB6E-AE6D-11cf-96B8-444553540000" id="ep_1456">
			<param name="allowfullscreen" value="true" />
			<param name="allowscriptaccess" value="always" />
			<param name="wmode" value="transparent" />
			<param name="movie" value="http://i.cdn.turner.com/'. esc_attr( $id ) .'" />
			<param name="bgcolor" value="#000000" />
			<embed src="http://i.cdn.turner.com/'. esc_attr( $id ) .'" type="application/x-shockwave-flash" bgcolor="#000000" allowfullscreen="true" allowscriptaccess="always" wmode="transparent" width="'. intval( $width ) .'" height="'. intval( $height ) .'"></embed>
		</object></div>';
	
	return $text;
}
add_shortcode('ew_cnn', 'ew_cnn_shortcode');



/*
 * Fox News 2012
 * Ex: [ew_foxnews id="1783616097001"]
 */
function ew_foxnews_shortcode( $atts, $content = null ) {
   extract(shortcode_atts(array(
		'width' => '466',
		'height' => '263',
		'id' => ''
      ), $atts ) );
	
	$text = '<div class="ew-video-embed ew-video-fox-news">
			<script src="http://video.foxnews.com/v/embed.js?id='. esc_attr( $id ) .'&w='. intval( $width ) .'&h='. intval( $height ) .'"></script>
		</div>';
	
	return $text;
}
add_shortcode('ew_foxnews', 'ew_foxnews_shortcode');



/*
 * Us Weekly 2012
 * Ex: [ew_usweekly id="RrdWhtNTqOV7oKdb55c0z_m-v5v22ZZe"]
 */
function ew_usweekly_shortcode( $atts, $content = null ) {
   extract(shortcode_atts(array(
		'width' => '510',
		'height' => '300',
		'id' => ''
      ), $atts ) );
	
	$text = '<div class="ew-video-embed ew-video-us-weekly">
			<script src="http://player.ooyala.com/player.js?width='. intval( $width ) .'&height='. intval( $height ) .'&embedCode='. esc_attr( $id ) .'"></script>
		</div>';
	
	return $text;
}
add_shortcode('ew_usweekly', 'ew_usweekly_shortcode');



/*
 * OWN Network 2012
 * Ex: [ew_own id="37406"]
 */
function ew_own_shortcode( $atts, $content = null ) {
   extract(shortcode_atts(array(
		'width' => '510',
		'height' => '300',
		'id' => ''
      ), $atts ) );
	
	$text = '<div class="ew-video-embed ew-video-own">
			<iframe width="'. intval( $width ) .'" height="'. intval( $height ) .'" src="http://www.oprah.com/common/omplayer_embed.html?article_id='. esc_attr( $id ) .'" frameborder="0" scrolling="no"></iframe>
		</div>';
	
	return $text;
}
add_shortcode('ew_own', 'ew_own_shortcode');



/*
 * Vulture 2012
 * Ex: [ew_vulture id="Late-Night-Jeff-Daniels-Slammed"]
 */
function ew_vulture_shortcode( $atts, $content = null ) {
   extract(shortcode_atts(array(
		'width' => '510',
		'height' => '380',
		'id' => ''
      ), $atts ) );
	
	$text = '<div class="ew-video-embed ew-video-vulture">
			<iframe src="http://video.vulture.com/video/'. esc_attr( $id ) .'/player?layout=compact&read_more=1" width="'. intval( $width ) .'" height="'. intval( $height ) .'" frameborder="0" scrolling="no"></iframe>
		</div>';
	
	return $text;
}
add_shortcode('ew_vulture', 'ew_vulture_shortcode');



/*
 * GMA 2012
 * Ex: [ew_gma id="kwidget/wid/0_szveq7f6/uiconf_id/6501142"]
 */
function ew_gma_shortcode( $atts, $content = null ) {
   extract(shortcode_atts(array(
		'width' => '510',
		'height' => '300',
		'id' => ''
      ), $atts ) );
	
	$text = '<div class="ew-video-embed ew-video-gma">
			<object width="'. intval( $width ) .'" height="'. intval( $height ) .'" data="http://cdnapi.kaltura.com/index.php/'. esc_attr( $id ) .'" name="kaltura_player_1345038087" id="kaltura_player_1345038087" type="application/x-shockwave-flash" allowScriptAccess="always" allowNetworking="all" allowFullScreen="true">
				<param name="allowScriptAccess" value="always"/>
				<param name="allowNetworking" value="all"/>
				<param name="allowFullScreen" value="true"/>
				<param name="bgcolor" value="#000000"/>
				<param name="wmode" value="transparent" />
				<param name="movie" value="http://cdnapi.kaltura.com/index.php/'. esc_attr( $id ) .'"/>
				<param name="flashVars" value="referer=http://www.ew.com&autoPlay=false"/>
			</object>
		</div>';
	
	return $text;
}
add_shortcode('ew_gma', 'ew_gma_shortcode');



/*
 * Ustream video 2012
 * Ex: [ew_ustream id="29533555" live="true"]
 */
function ew_ustream_shortcode( $atts, $content = null ) {
   extract(shortcode_atts(array(
		'width' => '510',
		'height' => '321',
		'id' => '',
		'live' => 'false'
      ), $atts ) );
	
	$livestr = ($live == 'true') ? '':'recorded/';
	$text = '<iframe width="'. intval( $width ) .'" height="'. intval( $height ) .'" src="'.esc_url('http://www.ustream.tv/embed/'.$livestr.$id .'?v=3&amp;wmode=direct').'" scrolling="no" frameborder="0" style="border: 0px none transparent;"></iframe>';
	
	return $text;
}
add_shortcode('ew_ustream', 'ew_ustream_shortcode');
?>
