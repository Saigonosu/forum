<?php
include_once "include/util.inc";
include_once "include/authenticator.inc";
include_once "models/users.inc";
include_once "models/answers.inc";
include_once "models/group.inc";
include_once "models/questions.inc";
include_once "models/question_tags.inc";

define("REQUIRED_PARAMS", array('first_name', 'last_name', 'email', 'password'));



function requiredParamsPresent($params){
  $present = true;
  foreach(REQUIRED_PARAMS as $param){
    $present = empty($params[$param]) ? false : true;
    if(!$present){
      return $present;
    }
  }
  return $present;
}

function get_manage($params) {
    Authenticator::instance() -> ensure('admin_page');

    $users = User::findAll();
    // @formatter:off
    renderTemplate(
        "views/user_list.inc",
        array(
            "title" => "Manage users",
            "users" => $users
        )
    );
    // @formatter:on
}

function get_view($params) {
    $id = safeParam($params, 0, false);
    if ($id === false) {
        die("No user id specified");
    }
    $authenticated = authenticateUser($id);
    $user = User::findById($id);
    if (!$user) {
        die("No user with id $id found.");
    }

    $first_name = $user -> getFirstName();
    $last_name = $user -> getLastName();
    $questions = Question::findQuestionsByUserId($user -> getId());
    // @formatter:off
    renderTemplate(
        "views/user_view.inc",
        array(
            'title' => "$first_name $last_name",
            'user' => $user,
            'questions' => $questions,
            'auth' => $authenticated
        )
    );
    // @formatter:on
}

function get_list($params) {
}

function get_edit($params) {
  ensureLoggedIn();
    $id = safeParam($params, 0, false);
    if (!$id) {
        die("No user specified");
    }
    $authenticated = authenticateUser($id);
    $user = User::findById($id);
    if (!$user) {
        die("No user found.");
    }
    // @formatter:off
    if($authenticated){
      renderTemplate(
          "views/user_edit.inc",
          array(
              'title' => 'Editing User',
              'user' => $user,
          )
      );
    }
    else {
      redirectRelative('index');
    }
    // @formatter:on
}

function get_add($params) {
  // @formatter:off
  $user = new User($params);
  renderTemplate(
      "views/user_add.inc",
      array(
        'title' => 'Register!',
        'user' => $user
      )
  );
  // @formatter:on
}

function post_add($params) {
  $user = new User($params);
  $validator = $user -> validate();
  if($validator -> hasErrors()){
    renderTemplate(
      "views/user_add.inc",
      array (
        'title' => 'Add A User',
        'user' => $user,
        'errors' => $validator -> allErrors()
      )
    );
  }
  else {
    if($user -> userExists()){
      renderTemplate(
          "views/login_form.inc",
          array(
              'title' => 'Login',
              'errors' => array("Email $email already exists. Please Login")
          )
      );
    }
    else {
      $user -> insert();
      loadUser($user -> getEmail());
      $_SESSION['loggedin'] = true;
      if (isset($_SESSION['redirect'])) {
        $redirect = $_SESSION['redirect'];
        redirect($redirect);
        exit();
      }
      redirectRelative("index");
    }
  }
}

function validate_present($elements) {
    $errors = '';
    foreach ($elements as $element) {
        if (!isset($_POST[$element])) {
            $errors .= "Missing $element\n";
        }
    }
    return $errors;
}

function post_update($params) {
  ensureLoggedIn();
  $user = new User($params);
  $validator = $user -> validate();
  if($validator -> hasErrors()){
    renderTemplate(
      "views/user_edit.inc",
      array (
          'title' => 'Editing User',
          'user' => $user,
          'errors' => $validator -> allErrors()
        )
    );
  }
  else{
    $user -> update();
    $id = $user -> getId();
    redirectRelative("users/view/$id");
  }
}

function get_delete($params) {
    ensureLoggedIn();
    $id = safeParam($params, 0, false);
    if (!$id) {
        die("No user specified");
    }
    $user = User::findById($id);
    if (!$user) {
        die("No user found.");
    }
    $authenticated = authenticateUser($id);
    if($authenticated){
      Answer::deleteByUserId($user->getId());
      $questions = Question::findQuestionsByUserId($user->getId());
      foreach($questions as $question){
        resetTags($question->getId());
      }
      Question::deleteByUserId($user->getId());
      $user -> delete();
    }
    $_SESSION = array();
    session_destroy();
    redirectRelative("index");
}

function get_groups($params) {
    $id = safeParam($params, 0, false);
    if ($id === false) {
        die("No user id specified");
    }
    $user = User::findById($id);
    if (!$user) {
        die("No user with id $id found.");
    }
    $memberOf = Group::findByUserId($id);
    $notMemberOf = Group::findByNotUserId($id);
    renderTemplate(
        "views/user_groups.inc",
        array(
            "title" => "{$user->getFullName()}'s Groups",
            "memberOf" => $memberOf,
            "notMemberOf" => $notMemberOf,
            "user" => $user
        )
    );
}
?>
