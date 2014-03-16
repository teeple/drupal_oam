<?php 

function _check_cpuload()
{
	$rst = exec('sar 1 1');
	// return format
	// 01:57:48  %usr  %nice   %sys   %idle
	// Average:      7      0      4     90
	$r = preg_split( "/[\s]+/", $rst);
	array_shift($r);
	$r[3] = 100 - intval($r[3]);
	return array(
			'key' => array( 'usr', 'nice', 'sys', 'idle'),
			'value' => array_map('intval', $r));
}

/**
 * Simple page function to display system info
 */
function checksys_page()
{
	$r = _check_cpuload();
	$page = array(
			'#type' => 'markup',
			'#markup' => t('Check sys CPU Load:' . implode(',',$r)),
	);
	return $page;
}


function ajax_mon_handler($mode)
{
	$os = PSI_OS;
	$sysinfo = new $os();

	ctools_include('ajax');

	$data = null;
	switch( $mode) {
		case 'cpu' :
			$data['cpu_load'] = _check_cpuload();
			$sys = $sysinfo->getSys();
			$data['cpu_loadavg'] = array(
					'key' => array( '1min', '5min', '15min'),
					'value' => array_map( 'floatval', explode(' ',$sys->getLoad()))
			);
			break;
		default:
			$data = t('Invalid mode @a', array( '@a' => $mode));
			break;
	}

	print json_encode(array( 'data' => $data, 'mode' => $mode));
	exit;
}
