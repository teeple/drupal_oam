#!/usr/bin/php
<?php
global $verbose;

// format
// cron_ftp.inc -u user -p password -s source -d destination_dir
$verbose = FALSE;

$opts = getopt("u:p:s:d:v");
foreach( $opts as $k => $v) {
  switch( $k) {
    case 'u' :   // user
      $user = $v;
      break;
    case 'p' :  // password
      $pass = $v;
      break;
    case 's' :  // source. <host>:<directory>/<file>
      $s = explode(':', $v);
      if ( preg_match( "/^([\w\.-]+):([\/\w\.\*~-]*)\/([\w\*\.-]+)?$/", $v, $matches)) {
        print_r( $matches);
        $src_host = $matches[1];
        $src_dir = $matches[2];
        $src_file = $matches[3];
      }
      else {
        printf("Invalid source '%s'. Required source format is '<host>:<directory>/<file>'.\n", $v);
      }
      break;
    case 'd' : // target directory
      $dest = $v;
      break;
    case 'v' :
      $verbose = TRUE;
      break;
  }  
}

if ( empty( $src_host) || empty( $user) || empty( $pass) || empty( $src_dir) || empty( $src_file)) {
  printf("Invalid parameters. \nUsage: %s -u user -p password -s host -d directory\n");
  exit;
}

if ( ! connect_mysql()) {
  if ( $verbose) {
    printf("Failed to connect MySQL.\n");
    exit;
  }
}
// Open FTP

// set up basic connection
if ( $verbose) {
  printf("Open FTP connection to '%s' with user '%s'\n", $src_host, $user);
}

$conn_id = ftp_connect($src_host, 0, 5);
if ( $conn_id ) {
  // login with username and password
  $r = ftp_login($conn_id, $user, $pass);
  
  if ( $verbose) {
    printf("FTP connected.\n");
  }
  
  // get files list
  $files = ftp_nlist($conn_id, $src_dir . '/' . $src_file);
  if ( $verbose) {
    print_r( $files);
    if ( count($files) == 0) {
      printf("Files not found.\n");
    }
  }
  
  foreach( $files as $f) {
    $v = explode('/', $f);
    $local_file = $dest . '/' . $v[count($v)-1];
    if ( $verbose) {
      printf("FTP get '%s' to '%s'\n", $f, $local_file);
    }
    if ( ftp_get( $conn_id, $local_file, $f, FTP_BINARY)) {
      if ( $verbose) {
        printf("FTP get finished: '%s' to '%s'\n", $f, $local_file);
      }
    }
    else {
      if ( $verbose) {
        printf("FTP get failed: '%s' to '%s'\n", $f, $local_file);
      }
    }
  }
  
  // close the connection
  ftp_close($conn_id);  
}
else {
  printf("FTP connection failed.\n");
}

function connect_mysql()
{
  $link = mysql_connect('localhost', 'root', 'root.123');
  if (!$link) {
    die('Could not connect: ' . mysql_error());
    return false;
  }
  if ( $verbose) {
    echo 'MySQL connected successfully';
  }
  mysql_close($link);
  return true;
}