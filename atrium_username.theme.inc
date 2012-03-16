<?php
/**
 * Override of theme('username').
 * Most of the code comes from tao_username,
 * it uses atrium_username_get_name to get the name to display.
 */
function atrium_username_theme_username($object) {

  if (!empty($object->name)) {

    // Get username from user profile (if not set yet).
    if(empty($object->atrium_username)) {
      $atrium_username = atrium_username_get_name($object->uid);
      $object->atrium_username = $atrium_username ? $atrium_username : $object->name;
    }
    $name = $object->atrium_username;

    // Shorten the name when it is too long or it will break many tables.
    $shortname = drupal_strlen($object->atrium_username) > 20 ? drupal_substr($object->atrium_username, 0, 15) . '...' : $object->atrium_username;
    $shortname = check_plain($shortname);

    // Default case -- we have a real Drupal user here.
    if ($object->uid && user_access('access user profiles')) {
      return l($shortname, 'user/' . $object->uid, array('attributes' => array('class' => 'username', 'title' => t('View user profile.'))));
    }
    // Handle cases where user is not registered but has a link or name available.
    else if (!empty($object->homepage)) {
      return l($shortname, $object->homepage, array('attributes' => array('class' => 'username', 'rel' => 'nofollow')));
    }
    // Produce an unlinked username.
    else {
      return "<span class='username'>{$name}</span>";
    }

  }
  return "<span class='username'>" . variable_get('anonymous', t('Anonymous')) . "</span>";
}

/**
 * Override notifications_team_form (notification options) theme function.
 */
function atrium_username_theme_notifications_team_form($form) {
  if ($form['options']['#value']) {
    $options = '';
    foreach ($form['options']['#value'] as $uid => $name) {
      $atrium_username = atrium_username_get_name($uid);
      $form['options']['#value'][$uid] = $atrium_username ? $atrium_username : $name;
    }
  }
  return theme_notifications_team_form($form);
}

/**
 * Override casetracker_case_form ('Assign to' options) theme function.
 */
function atrium_username_theme_casetracker_case_form_common($form) {
  $assign_to = $form['assign_to']['#options'];
  foreach($assign_to as $key => $value) {
    $uid = casetracker_get_uid($key);
    $atrium_username =  atrium_username_get_name($uid);
    $name = $atrium_username ? $atrium_username : $value;
    $form['assign_to']['#options'][$key] = $name;
    $form['assign_to'][$key]['#title'] = $name;
  }
  return theme_casetracker_case_form_common($form);
}

/**
 * Override theme_casetracker_comment_changes.
 * Unfortunately we have to rewrite everything as the lines
 * to be changed are inside a switch/case with no other options.
 * Only the case 'assign_to' is modified.
 *
 * Displays the changes a comment has made to the case fields.
 *
 * @param $case_data
 *   An array of both 'old' and 'new' objects that contains
 *   the before and after values this comment has changed.
 */
function atrium_username_theme_casetracker_comment_changes($old, $new) {
  $rows = array();

  $fields = array(
    'pid' => t('Project'),
    'title' => t('Title'),
    'case_status_id' => t('Status'),
    'assign_to' => t('Assigned'),
    'case_priority_id' => t('Priority'),
    'case_type_id' => t('Type'),
  );
  foreach ($fields as $field => $label) {
    if ($new->{$field} != $old->{$field}) {
      switch ($field) {
        case 'pid':
          $old_title = db_result(db_query("SELECT title FROM {node} WHERE nid = %d", $old->pid));
          $new_title = db_result(db_query("SELECT title FROM {node} WHERE nid = %d", $new->pid));
          $old->{$field} = l($old_title, "node/{$old->pid}");
          $new->{$field} = l($new_title, "node/{$new->pid}");
          break;
        case 'case_status_id':
          $old->{$field} = check_plain(casetracker_case_state_load($old->{$field}, 'status'));
          $new->{$field} = check_plain(casetracker_case_state_load($new->{$field}, 'status'));
          break;
        case 'assign_to':
          $old->{$field} = atrium_username_get_name($old->{$field}) ? atrium_username_get_name($old->{$field}) : check_plain(casetracker_get_name($old->{$field}));
          $new->{$field} = atrium_username_get_name($new->{$field}) ? atrium_username_get_name($new->{$field}) : check_plain(casetracker_get_name($new->{$field}));
          break;
        case 'case_priority_id':
          $old->{$field} = check_plain(casetracker_case_state_load($old->{$field}, 'priority'));
          $new->{$field} = check_plain(casetracker_case_state_load($new->{$field}, 'priority'));
          break;
        case 'case_type_id':
          $old->{$field} = check_plain(casetracker_case_state_load($old->{$field}, 'type'));
          $new->{$field} = check_plain(casetracker_case_state_load($new->{$field}, 'type'));
          break;
      }
      $rows[] = array(t('@label: !old &raquo; !new', array('@label' => $label, '!old' => $old->{$field}, '!new' => $new->{$field})));
    }
  }

  if (!empty($rows)) {
    return theme('table', NULL, $rows, array('class' => 'case_changes'));
  }
}

/**
 * Implementation of hook_preprocess_block().
 * - atrium-account: override block title using atrium_username
 *                   check _atrium_block_account()
 *                   in /profiles/openatrium/modules/atrium_features/atrium/atrium.blocks.inc
 */
function atrium_username_preprocess_block(&$variables) {
  // atrium-account
  if($variables['block']->bid == "atrium-account") {
    global $user;
    if ($user->uid) {
      $atrium_username = atrium_username_get_name($uid);
      $user_name = $atrium_username ? $atrium_username : check_plain($user->name);
      $variables['block']->subject = theme('user_picture', $user) . $user_name;
    }
  }
}

