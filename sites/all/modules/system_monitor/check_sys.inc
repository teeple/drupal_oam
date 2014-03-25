<?php 

function _check_cpuload($mode = NULL)
{
  // return format
  // 01:57:48  %usr  %nice   %sys   %idle
  // Average:      7      0      4     90
  $etime = new DateTime();
  $stime = new DateTime();
  $stime->sub( new DateInterval('PT500S'));   // 5*100 = 500 seconds
  $sec5 = new DateTime();
  $sec5->sub(new DateInterval('PT5S'));  // 5 seconds

  $last_data = &drupal_static(__FUNCTION__);
  if ( ! isset($last_data) && $mode != 'all') {
    if ( $cache = cache_get('system_monitor_cpu_data')) {
      $last_data = $cache->data;
      //dpm( $last_data['time']->format('H:i:s'), 'last');
      //dpm( $sec5->format('H:i:s'), 'new');
      if ( $last_data['time'] >= $sec5) {
        // return cached data
        return array(
            'key' => array( 'usr', 'nice', 'sys', 'idle'),
            'value' => $last_data['value'],
            'cache' => true,
            //'last' => $last_data,
        );
      }
    }
  }

  // -s option is not working
  $cmd = sprintf('sar -f /tmp/smon/sa%s -s %s | tail -n 100', $etime->format('d'), $stime->format('H:i:s'));
  $rst = exec( $cmd, $lines);
  $value = array();
  if ( $mode != 'all') {
    $lines = array( $lines[count($lines)-2]);   // get the last line
  }
  foreach( $lines as $l) {
    $r = preg_split( "/[\s]+/", $l);
    array_shift( $r);
    $r[3] = 100 - intval($r[3]);
    $value[] = array_map( 'intval', $r);
  }

  cache_set('system_monitor_cpu_data', array( 'time' => $etime, 'value' => $value), 'cache');
  return array(
      'key' => array( 'usr', 'nice', 'sys', 'idle'),
      'value' => $value,
      //'last' => $last_data,
  );
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

function _check_cpu_loadavg($mode)
{
  $etime = new DateTime();
  $sec5 = new DateTime();
  $sec5->sub(new DateInterval('PT5S'));  // 5 seconds

  $last_data = &drupal_static(__FUNCTION__);
  if ( ! isset($last_data) ) {
    if ( $cache = cache_get('system_monitor_cpu_loadavg')) {
      $last_data = $cache->data;
      //dpm( $last_data['time']->format('H:i:s'), 'last');
      //dpm( $sec5->format('H:i:s'), 'new');
      if ( $last_data['time'] >= $sec5) {
        // return cached data
        return array(
            'key' => array( '1min', '5min', '15min'),
            'value' => $last_data['value'],
            'cache' => true,
            //'last' => $last_data,
        );
      }
    }
  }

  $os = PSI_OS;
  $sysinfo = new $os();
  $sys = $sysinfo->getSys();
  $value = array( array_map( 'floatval', explode(' ',$sys->getLoad())));

  cache_set('system_monitor_cpu_loadavg', array( 'time' => $etime, 'value' => $value), 'cache');
  return array(
      'key' => array( '1min', '5min', '15min'),
      'value' => $value,
      //'last' => $last_data,
  );
}

function ajax_mon_handler( $part, $mode)
{
  ctools_include('ajax');

  $data = null;
  switch( $part) {
    case 'cpu' :
      $data['cpu_load'] = _check_cpuload( $mode);
      $data['cpu_loadavg'] = _check_cpu_loadavg($mode);
      break;
    default:
      $data = t('Invalid mode @a', array( '@a' => $part));
      break;
  }

  print json_encode(array( 'data' => $data, 'mode' => $mode));
  exit;
}


// hook cron_api
function system_monitor_cronapi($op, $job = NULL)
{
  switch($op) {
    case 'list':
      return array(
      'system_monitor_cronjob_cpu' => 'Check CPU activity',
      );

    case 'rule':
      switch($job) {
        case 'system_monitor_cronjob_cpu':
          return '0,5,10,15,20,25,30,35,40,45,50,55 * * * *';
      }

    case 'execute':
      switch($job) {
        case 'system_monitor_cronjob_cpu':
          system_monitor_cronjob_cpu();
          break;
      }

  }

}

function system_monitor_background_procss_sar()
{
  $cmd = drupal_get_path( 'module', 'system_monitor') . '/system_monitor.sh';
  watchdog( 'system_monitor', $cmd, array(), WATCHDOG_INFO);
  exec($cmd);
}

/*
 * run system monitoring shell every day
*/
function system_monitor_cronjob_cpu()
{
  $cmd = DRUPAL_ROOT . '/' . drupal_get_path( 'module', 'system_monitor') . '/system_monitor.sh';
  $r = exec($cmd);
  watchdog( 'system_monitor', 'Run @cmd. result=@r', array( '@cmd' => $cmd, '@r' => $r), WATCHDOG_INFO);
}