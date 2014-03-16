<?php 

function _check_cpuload()
{
	$rst = exec('sar 1 1');
	// return format
	// 01:57:48  %usr  %nice   %sys   %idle
	// Average:      7      0      4     90
	$r = preg_split( "/[\s]+/", $rst);
	array_shift($r);
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
		case 'cpu_load' :
			$data = _check_cpuload();
			break;
		case 'cpu_loadavg' :
			$sys = $sysinfo->getSys();
			$data = $sys->getLoad();
			break;
		default:
			$data = t('Invalid mode @a', array( '@a' => $mode));
			break;
	}

	print json_encode(array( 'data' => $data, 'mode' => $mode));
	exit;
}
