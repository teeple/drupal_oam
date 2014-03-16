<?php 

/* for phpsysinfo lib */
define('APP_ROOT', dirname(__FILE__) );

/**
 * internal xml or external
 * external is needed when running in static mode
 *
 * @var boolean
 */
define('PSI_INTERNAL_XML', false);

if (version_compare("5.2", PHP_VERSION, ">")) {
	die("PHP 5.2 or greater is required!!!");
}

require_once APP_ROOT . '/includes/autoloader.inc.php';
require_once APP_ROOT . '/config.php';



