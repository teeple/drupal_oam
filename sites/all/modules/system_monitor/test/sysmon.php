<?php 
require_once('../SysMon.class.php');
date_default_timezone_set('Asia/Seoul');

function test_sysmon_cpu()
{
  $cpu = new SysMonCPU();
  print_r( $cpu->getDataToSave());
	print_r( $cpu->getResult());
}

test_sysmon_cpu();
