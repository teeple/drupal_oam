<?php 
require_once('../SysMon.class.php');
date_default_timezone_set('Asia/Seoul');

function test_sysmon_cpu()
{
	printf("OS: %s\n", PHP_OS);
  $cpu = new SysMonCPU();
	echo "dataToSave\n";
  print_r( $cpu->getDataToSave());
	echo "Result\n";
	print_r( $cpu->getResult());
	echo "cmd\n";
	print_r( $cpu->getCmd());
	echo "measure\n";
	print_r( $cpu->measure());
}

test_sysmon_cpu();

printf("\nEND\n");
