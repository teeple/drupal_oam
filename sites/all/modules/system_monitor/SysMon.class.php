<?php 

interface SysMonTarget
{
  public function getDataToSave();
}

class SysMonCPU implements SysMonTarget {
  private $cmd = '';
  
  function __construct() {
    $etime = new DateTime();
    $this->cmd = sprintf('sar -f /tmp/smon/sa%s | tail -n 60', $etime->format('d'));
  }
  
  public function getDataToSave() {
    $rst = exec( $this->cmd, $lines);
    $r = preg_split( "/[\s]+/", $rst);
    array_shift( $r);
    $r[3] = 100 - intval($r[3]);
    $value = array_map( 'intval', $r);
    // get time
    $r = preg_split( "/[\s]+/", $lines[count($lines)-2]);
    $value['date'] = date( 'Y-m-d ') . $r[0];
    return $value;
  }  
  
  public function save() {
    $data = $this->getDataToSave();
    if ( !empty($data)) {
      $r = db_insert('sysmon_stat_cpu', array('return' => Database::RETURN_INSERT_ID))
      ->fields( array(
          'date' => $data['date'],
          'usr' => $data[0],
          'nice' => $data[1],
          'sys' => $data[2],
          'cpu_load' => $data[3]
      ));
      $r-> execute();
    }
  }
}