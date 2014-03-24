<?php 

function _unset_menu_items(&$form, $formId) {
	unset($form['menu']);
	unset($form['path']);
	unset($form['author']);
	unset($form['comment_settings']);
	unset($form['options']);
	unset($form['additional_settings']);
	unset($form['actions']['preview']);
	unset($form['revision_information']);
}

/*
 * alter form
*/
function system_monitor_form_alter(&$form, &$form_state, $form_id) {

	if ((isset($form['#attributes']) && isset($form['#attributes']['class'])) && gettype($form['#attributes']['class']) == 'array') {
		$form['#attributes']['class'][] = 'myform';
	}

	switch( $form_id) {
		case'md_target_server_node_form':
		case 'md_ftp_schedule_node_form':
			_unset_menu_items($form, $form_id);
			break;
	}
}