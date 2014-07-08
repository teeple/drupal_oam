#!/usr/bin/php
<?php

require_once('config.inc');

date_default_timezone_set('Asia/Seoul');
global $verbose, $mysqli;

// format
// cron_ftp.inc -u user -p password -s source -d destination_dir -n nid
$verbose = FALSE;

$opts = getopt("u:p:s:d:n:m:v", array( "postfix:"));
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
    case 'n':  // scheduler node nid
      $nid = $v;
      break;
    case 'm' :  // mode: list, transfer
      $mode = $v;
      break;
    case 'postfix':
      $postfix = $v;
      break;
  }
}

if ( empty( $src_host) || empty( $user) || empty( $pass) || empty( $src_dir) || empty( $src_file) || empty( $nid) || empty($mode)) {
  printf("Invalid parameters. \nUsage: %s -u user -p password -s host -d directory -n nid -m mode\n", $argv[0]);
  exit(1) ;
}

// connect mysql
if ( ! _connect_mysql()) {
  if ( $verbose) {
    printf("Failed to connect MySQL.\n");
    exit(1);
  }
}
// Open FTP

// set up basic connection
if ( $verbose) {
  printf("Open FTP connection to '%s' with user '%s'\n", $src_host, $user);
}

$stmt_ins = $mysqli->prepare("insert into ftp_scheduler_file_list ( scheduler_nid, reg_date, file_name ) values( ?,?,?)");
$stmt_upd = $mysqli->prepare("update ftp_scheduler_file_list set end_date = ? where serial_no = ?");


$conn_id = _connect_ftp($src_host, $user, $pass, $verbose);
if ( $conn_id ) {

  if ( $mode == 'list') {
    $files = _get_file_list( $conn_id, $src_dir, $src_file, $postfix, $verbose);
    foreach( $files as $f) {
      _insert_file( $stmt_ins, $nid, $f );
      $source_file = $src_dir .'/'.$f;
      ftp_rename( $conn_id, $source_file, $source_file.$postfix['list']);  // add 'LST' postfix
    }
    if ( count($files) == 0) {
      printf("FTP files (%s/%s) are not found.\n", $src_dir, $src_file);
    }
  }
  else {  // transfer
    $source_file = $src_dir . '/' . $src_file;
    $local_file = $dest . '/' . $src_file;
    if ( $verbose) {
      printf("FTP get '%s' to '%s'\n", $source_file, $local_file);
    }
    if ( ftp_get( $conn_id, $local_file, $source_file, FTP_BINARY)) {
      if ( $verbose) {
        printf("FTP get finished: '%s' to '%s'\n", $source_file, $local_file);
      }
      _update_file( $stmt_upd, $nid);
      ftp_rename( $conn_id, $source_file, $source_file.$postfix['transfered']);
    }
    else {
      if ( $verbose) {
        printf("FTP get failed: '%s' to '%s'\n", $source_file, $local_file);
      }
    }
  }
}

// close the connection
ftp_close($conn_id);

// close DB sessions
foreach( array( $stmt_ins, $stmt_upd) as $s) {
  $s->close();
}
_disconnect_mysql();
exit(0);


function _insert_file( $stmt, $nid, $file )
{
  global $verbose;

  $date = date( 'Y-m-d H:i:s');
  $stmt->bind_param('dss', $nid, $date, $file);

  /* Select queries return a resultset */
  /* execute prepared statement */
  $stmt->execute();

  if ( $verbose) {
    printf("MYSQL: %d Row inserted.\n", $stmt->affected_rows);
  }
}

function _update_file( $stmt, $serialno)
{
  global $verbose;

  $date = date( 'Y-m-d H:i:s');
  $stmt->bind_param('sd', $date, $serialno);

  /* Select queries return a resultset */
  /* execute prepared statement */
  $stmt->execute();

  if ( $verbose) {
    printf("MYSQL: %d Row updated.\n", $stmt->affected_rows);
  }
}


function _connect_ftp( $src_host, $user, $pass, $verbose = false)
{
  $conn_id = ftp_connect($src_host, 0, 5);
  if ( $conn_id ) {
    // login with username and password
    $r = ftp_login($conn_id, $user, $pass);

    if ( $verbose) {
      printf("FTP connected.\n");
    }
  }
  else {
    printf("FTP connection failed.\n");
  }
  return $conn_id;
}

function _get_file_list( $conn_id, $src_dir, $src_file, $postfix, $verbose = false)
{
  // get files list
  if ( ftp_chdir($conn_id, $src_dir)) {
    if ( $verbose) {
      printf("FTP change directory to '%s'.\n", $src_dir);
    }

    $files = ftp_nlist($conn_id, $src_file);
    foreach( $files as $idx => $f) {
      // remove files with given postfix
      foreach( $postfix as $p) {
        if ( substr($f, strlen($f) - strlen($p)) == $p) {
          unset( $files[$idx]);
        }
      }
    }
    $files = array_merge( $files);
    
    if ( $verbose) {
      foreach( $files as $f) {
        printf("%s\n", $f);
      }
    }

    return $files;
  }
  else {
    printf("ERROR: FTP failed to change directory to '%s'.\n", $src_dir);
  }

  return NULL;
}

/**
 * @return boolean
 */
function _connect_mysql()
{
  global $verbose, $mysqli, $database;

  $mysqli = new mysqli("localhost", $database['user'], $database['passwd'], $database['dbname']);

  /* check connection */
  if ($mysqli->connect_errno) {
    printf("MYSQL: Connect failed: %s\n", $mysqli->connect_error);
    exit();
  }
  if ( $verbose) {
    echo 'MySQL connected successfully';
  }

  return true;
}

function _disconnect_mysql()
{
  global $mysqli;

  $mysqli->close();
}