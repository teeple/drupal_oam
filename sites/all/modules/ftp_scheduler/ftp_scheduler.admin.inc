<?php 

/**
 * Provide admin settings form
*/
function ftp_scheduler_admin_config($form, &$form_state) {
  $form = array();

  $form['ftp'] = array(
      '#type' => 'fieldset',
      '#title' => t('FTP configuration'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
  );

  $form['ftp']['ftp_transfered_postfix'] = array(
      '#type' => 'textfield',
      '#title' => t('Postfix for transfered file'),
      '#default_value' => variable_get('ftp_transfered_postfix', array()),
  );
  
  return system_settings_form($form);
}

function ftp_scheduler_admin_config_submit( $form, &$form_state)
{
  variable_set( 'ftp_transfered_postfix', $form_state['values']['ftp_transfered_postfix']);
}
