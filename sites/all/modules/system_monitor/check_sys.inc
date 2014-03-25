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
      'system_monitor_cronjob_save_stat' => 'Save statistics data in 5 min',
      'system_monitor_cronjob_ftp' => 'FTP files',
      );

    case 'rule':
      switch($job) {
        case 'system_monitor_cronjob_cpu':
          return '0 0 * * *';
        case 'system_monitor_cronjob_save_stat':
          return '*/5 * * * *';
        case 'system_monitor_cronjob_ftp':
          return '* * * * *';
      }

    case 'execute':
      switch($job) {
        case 'system_monitor_cronjob_cpu':
          system_monitor_cronjob_cpu();
          break;
        case 'system_monitor_cronjob_save_stat':
          system_monitor_cronjob_save_stat();
          break;
        case 'system_monitor_cronjob_ftp':
          system_monitor_cronjob_ftp();
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
  $cmd = DRUPAL_ROOT . '/' . drupal_get_path( 'module', 'system_monitor') . '/script/system_monitor.sh';
  $r = exec($cmd);
  watchdog( 'system_monitor', 'Run "@cmd". result:(@r)', array( '@cmd' => $cmd, '@r' => $r), WATCHDOG_INFO);
}

/*
 * save stat data
*/
function system_monitor_cronjob_save_stat()
{
  $cpu = new SysMonCPU();
  $data = $cpu->save();
  watchdog( 'system_monitor', 'Run "@cmd". result:(@r)', array( '@cmd' => $cpu->getCmd(), '@r' => $cpu->getResult()), WATCHDOG_INFO);
}

/*
 * FTP
 */
function system_monitor_cronjob_ftp()
{
  $now = new DateTime();
  // fetch scheduled ftp
  $nids = sysmon_get_nids_by_cond('md_ftp_schedule', array(
      array('field_enabled', 'value', '1', '='))
      );
  foreach( $nids as $nid) {
    $node = node_load( $nid);
    if ( ! isset( $node->field_next_run_date['und']) || $node->field_next_run_date['und'][0]['value'] <= $now->format('Y-m-d H:i:s')) {
      // set next run time
      $next = new DateTime( isset( $node->field_last_run_date['und']) ? $node->field_last_run_date['und'][0]['value'] : "now");
      $next->add( new DateInterval('PT'.$node->field_interval['und'][0]['value'].'M'));
      $node->field_next_run_date['und'][0]['value'] = $next->format('Y-m-d H:i:s');

      // update run time
      $node->field_last_run_date['und'][0]['value'] = $now->format('Y-m-d H:i:s');
      node_save( $node);
      
      // run now
      $src = node_load($node->field_source_server['und'][0]['target_id']);
      $dst = node_load($node->field_target_server['und'][0]['target_id']);
      $cmd = t('@root/@path/script/cron_ftp.sh @src @s_dir @file @target @t_dir',
          array( '@root' => DRUPAL_ROOT,
              '@path' => drupal_get_path( 'module', 'system_monitor'),
              '@src' => $src->field_ipaddr['und'][0]['value'],
              '@s_dir' => $node->field_source_dir['und'][0]['value'],
              '@file' => $node->field_file_name['und'][0]['value'],
              '@target' => $dst->field_ipaddr['und'][0]['value'],
              '@t_dir' => $node->field_target_dir['und'][0]['value']));
      $r = exec($cmd);
      watchdog( 'system_monitor', 'Run "@cmd"', array( '@cmd' => $cmd), WATCHDOG_INFO);
    }
//     else {
//       watchdog( 'system_monitor', 'No Run', array(), WATCHDOG_INFO);
//     }
  }
//  watchdog( 'system_monitor', 'No Run 2', array(), WATCHDOG_INFO);
} 