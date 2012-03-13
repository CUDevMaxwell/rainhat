<?php

/**
 * @file
 * Callbacks include file for the Fedora Connection entity. 
 */

/**
 * List callback.
 */
function rainhat_fedoraconnection_list() {
  $elements = array();

  // entity_load() called with one argument loads all entities of the specified type.
  $fedora_connections = entity_load('rainhat_fedora_connection');

  $header = array(t('Name'), t('Connection URL (Links to the Fedora Admin Interface)'));

  if(user_access('administer fedora connections collections')) {
    $header[] = t('Administer');
  }

  // Sort the topics by date, since entity_load() returns them unsorted.
  uasort($fedora_connections, create_function('$a,$b', 'return strcmp($a->name, $b->name);'));

  $rows = array();
  foreach($fedora_connections as $id => $fedora_connection) {
    $row = array();
    $row[] = check_plain($fedora_connection->name);
    $row[] = l(check_plain($fedora_connection->url), $fedora_connection->url . "/admin") ;

    if(user_access('administer fedora connections collections')) {
      $act = l(t('Edit'), "admin/structure/fedora/connections/$id/edit");
      $act .= ', ';
      $act .= l(t('Delete'), "admin/structure/fedora/connections/$id/delete");
      $row[] = $act;
    }

    $rows[] = $row;
  }
  $elements['fedpra_connections'] = array(
    '#theme' => 'table',
    '#title' => t('Fedora Connections'),
    '#header' => $header,
    '#rows' => $rows,
  );
  return $elements;
}

/**
 * Add Fedora Connection callback.
 */
function rainhat_fedora_connection_add() {
  // Invoke the create() method we added to the controller to get a new entity with defaults.
  $fedora_connection = entity_get_controller('rainhat_fedora_connection')->create();

  return drupal_get_form('rainhat_fedora_connection_form', $fedora_connection);
}

/**
 * Fedora Connection add/edit form callback.
 */
function rainhat_fedora_connection_form($form, &$form_state, $fedora_connection) {
  $form['#fedora_connection'] = $fedora_connection;

  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#default_value' => check_plain($fedora_connection->name),
    '#size' => 50,
    '#maxlength' => 255,
    '#required' => TRUE,
  );
  $form['url'] = array(
    '#type' => 'textfield',
    '#title' => t('Connection URL'),
    '#default_value' => check_plain($fedora_connection->url),
    '#size' => 50,
    '#maxlength' => 255,
  );
  $form['admin_username'] = array(
    '#type' => 'textfield',
    '#title' => t('Username'),
    '#default_value' => check_plain($fedora_connection->admin_username),
    '#size' => 60,
    '#maxlength' => 120,
    '#required' => TRUE,
  );

  $form['admin_password'] = array(
    '#type' => 'textfield',
    '#title' => t('Password'),
    '#default_value' => check_plain($fedora_connection->admin_password),
    '#size' => 60,
    '#maxlength' => 120,
    '#required' => TRUE,
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}

function rainhat_fedora_connection_form_validate($form, &$form_state){
  
  //Ensure that the fedora connection is unique
  $duplicates = db_select('rainhat_fedora_connection', 'f')
    ->fields('f')
    ->where('name=:name OR url=:url', array('name' => $form_state['values']['name'], 'url' => $form_state['values']['url']))
    ->execute();
    
  foreach ($duplicates as $duplicate){
    form_set_error('', t('The name or url is not unique. @name has @url', array('@name' => $duplicate->name, '@url' => $duplicate->url)));
  }

}

/**
 * Fedora Connection add/edit form submit callback.
 */
function rainhat_fedora_connection_form_submit($form, &$form_state) {
  $fedora_connection = $form['#fedora_connection'];

  $fedora_connection->name = $form_state['values']['name'];
  $fedora_connection->url = $form_state['values']['url'];
  $fedora_connection->admin_username = $form_state['values']['admin_username'];
  $fedora_connection->admin_password = $form_state['values']['admin_password'];

  // Invoke the save() method that we added to the controller.
  entity_get_controller('rainhat_fedora_connection')->save($fedora_connection);
  drupal_set_message(t('Fedora Connection saved.'));
  $form_state['redirect'] = 'admin/structure/fedora/connections';
}

function rainhat_fedora_connection_delete($form, &$form_state, $fedora_connection) {
  $form['#fedora_connection'] = $fedora_connection;
  $form['id'] = array(
    '#type' => 'value',
    '#value' => $fedora_connection->id,
  );
  return confirm_form($form,
    t('Are you sure you want to delete @f?', array('@f' => $fedora_connection->name)),
    '<front>',
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}

function rainhat_fedora_connection_delete_submit($form, &$form_state) {
  if($form_state['values']['confirm']) {
    $fedora_connection = rainhat_fedora_connection_load($form_state['values']['id']);

    // Invoke the delete() method that we added to the controller.
    entity_get_controller('rainhat_fedora_connection')->delete(array($fedora_connection->id));
    drupal_set_message(t('Fedora Connection "@f" has been deleted.', array('@f' => $fedora_connection->name)));
  }
  if(isset($_GET['destination'])) {
    $destination = drupal_get_destination();
    unset($_GET['destination']);
  }
  else {
    $destination = array('destination' => '<front>');
  }
  $form_state['redirect'] = $destination['destination'];
}