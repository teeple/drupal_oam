<?php 


/**
 * Simple page function to display system info
 */
function checksys_page()
{
	$rst = exec('sar 1 1');
	dpm( $rst, 'sar');
	// return format 
	// 01:57:48  %usr  %nice   %sys   %idle
	// Average:      7      0      4     90
	
	$r = preg_split( "/[\s]+/", $rst);
	array_shift($r);
	dpm( $r, 'rst4');
	
	$page = array(
		'#type' => 'markup',
		'#markup' => t('Check sys CPU Load:' . implode(',',$r)),
	);
	return $page;
}