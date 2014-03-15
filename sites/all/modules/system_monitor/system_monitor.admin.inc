<?php
/**
 * @file
 * Provides settings for analytices dashboard page.
 */

/**
 * Provide admin settings form
 */
function system_monitor_admin_config($form, &$form_state) {
  $form = array();

  // Generate default charts selection
  $default = variable_get('cpu_interval', array());

  $form['cpu'] = array(
    '#type' => 'fieldset',
    '#title' => t('CPU monitoring settings'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['cpu']['cpu_interval'] = array(
    '#type' => 'select',
    '#title' => t('Measuring interval'),
    '#default_value' => $default,
    '#options' => array(
			3 => t('3 sec'),
			5 => t('5 sec'),
			10 => t('10 sec'),
			),
    '#description' => t('CPU measuring intervals'),
  );

  return system_settings_form($form);
}

function system_monitor_admin_config_submit( $form, &$form_state)
{
	variable_set( 'cpu_interval', $form_state['values']['cpu_interval']);
}
