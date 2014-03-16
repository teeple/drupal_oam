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

  $form['cpu'] = array(
    '#type' => 'fieldset',
    '#title' => t('CPU monitoring settings'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['cpu']['cpu_interval'] = array(
    '#type' => 'select',
    '#title' => t('Measuring interval'),
    '#default_value' => variable_get('cpu_interval', array()),
    '#options' => array(
			3 => t('3 sec'),
			5 => t('5 sec'),
			10 => t('10 sec'),
			),
    '#description' => t('CPU measuring intervals'),
  );
  $form['cpu']['cpu_chart_width'] = array(
  		'#type' => 'textfield',
  		'#title' => t('Chart width'),
  		'#default_value' => variable_get('cpu_chart_width', array()),
  );
  $form['cpu']['cpu_chart_height'] = array(
  		'#type' => 'textfield',
  		'#title' => t('Chart height'),
  		'#default_value' => variable_get('cpu_chart_height', array()),
  );
  
  return system_settings_form($form);
}

function system_monitor_admin_config_validate( $form, &$form_state)
{
	$values = $form_state['values'];
	if ( ! is_numeric( $values['cpu_chart_width'])) {
		form_set_error('cpu_chart_width', 'Invalid chart width');
	}
	if ( ! is_numeric( $values['cpu_chart_height'])) {
		form_set_error('cpu_chart_height', 'Invalid chart height');
	}
}

function system_monitor_admin_config_submit( $form, &$form_state)
{
	variable_set( 'cpu_interval', $form_state['values']['cpu_interval']);
	variable_set( 'cpu_chart_width', $form_state['values']['cpu_chart_width']);
	variable_set( 'cpu_chart_height', $form_state['values']['cpu_chart_height']);
}
