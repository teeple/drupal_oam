<?php 

interface SysMonTarget
{
  public function getDataToSave();
}

class SysMonCPU implements SysMonTarget {
  private $cmd = '';
  private $value = '';
  
  function __construct() {
    $etime = new DateTime();
    $this->cmd = sprintf('sar -f /tmp/smon/sa%s | tail -n 60', $etime->format('d'));   
    if ( PHP_OS == 'Darwin') {
      $this->idx = array_flip( array( 'usr','nice','sys','idle'));
    } 
    else {
      $this->idx = array_flip( array( 'usr','nice','sys','idle'));
    }
  }
  
  // convert line buf to value array
  private function convert( $line)
  {
    $r = preg_split( "/[\s]+/", $line);
    array_shift( $r);
    $v = array_map( 'intval', $r);
    return array( $v[$this->idx['usr']], $v[$this->idx['nice']], $v[$this->idx['sys']], 100 - $v[$this->idx['idle']]);
  }
  
  public function measure( $mode = null) {
    $rst = exec( $this->cmd, $lines);
    $this->value = $rst;
    
    $value = array();
    if ( $mode != 'all') {
      $lines = array( $lines[count($lines)-2]);   // get the last line
    }
    foreach( $lines as $l) {
      $value[] = $this->convert($l);
    }
    return $value;    
  }
  
  
  public function getDataToSave() {
    $rst = exec( $this->cmd, $lines);
    $this->value = $rst;
    $value = $this->convert($lines);
    
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

  public function getCmd() {
    return $this->cmd;
  }

  public function getResult() {
    return $this->value;
  }
}

