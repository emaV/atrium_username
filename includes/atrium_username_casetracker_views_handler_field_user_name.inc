<?php

/**
 * Field handler to show Selective state.
 *
 * @ingroup views_field_handlers
 */
class atrium_username_casetracker_views_handler_field_user_name extends views_handler_field {

  function render($values) {
    $uid = $values->{$this->field_alias};
    if($uid==0) {
      $assign_to = t('Unassigned');
    }
    else {
      $assign_to = atrium_username_get_name($uid) ? atrium_username_get_name($uid) : casetracker_get_name($uid);
    }
    return $assign_to;
  }
}
