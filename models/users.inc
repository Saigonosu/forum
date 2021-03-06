<?php
include_once 'models/model.inc';

class User extends Model {
    private $first_name;
    private $last_name;
    private $email;
    private $password;
    private $validator;

    //public function __construct($id, $description, $done = false) {
    public function __construct($fields = array(), $validator = null){
        parent::__construct($fields);
        $this -> setFirstName(safeParam($fields, 'first_name'));
        $this -> setLastName(safeParam($fields, 'last_name'));
        $this -> setEmail(safeParam($fields, 'email'));
        $this -> setPassword(safeParam($fields, 'password'));
        if(!isset($validator)){
          $this -> validator = new Validator();
        }
        else {
          $this -> validator = $validator;
        }
    }

    public function validate($throw = false) {
        $this -> validator -> required('first_name', $this -> first_name, "First Name is required");
        $this -> validator -> required('last_name', $this -> last_name, "Last Name is required");
        $this -> validator -> required('email', $this -> email, "Email is required");
        $this -> validator -> required('password', $this -> password, "Password is required");
        if ($throw && $this -> validator -> hasErrors()) {
            throw new IllegalStateException(implode(", ", $this -> validator -> allErrors()));
        }
        return $this -> validator;
    }

    private function clean() {
        $this -> first_name = htmlentities($this -> first_name, ENT_QUOTES);
        $this -> last_name = htmlentities($this -> last_name, ENT_QUOTES);
        $this -> email = htmlentities($this -> email, ENT_QUOTES);
        $this -> password = htmlentities($this -> password, ENT_QUOTES);
    }

    public function getFirstName() {
        return $this -> first_name;
    }

    public function setFirstName($first_name) {
        $this -> first_name = $first_name;
        return $this;
    }

    public function getLastName() {
        return $this -> last_name;
    }

    public function setLastName($last_name) {
        $this -> last_name = $last_name;
        return $this;
    }

    public function getFullName() {
        return $this -> first_name . ' ' . $this -> last_name;
    }

    public function getEmail() {
        return $this -> email;
    }

    public function setEmail($email) {
        $this -> email = $email;
        return $this;
    }

    public function getPassword() {
        return $this -> password;
    }

    public function setPassword($password) {
        $this -> password = $password;
        return $this;
    }

    static function findAll() {
        $db = Db::getDb();
        $st = $db -> prepare('SELECT * FROM users');
        $st -> execute();
        $rows = $st -> fetchAll(PDO::FETCH_ASSOC);
        return self::fromRows($rows);
    }

    static function findById($id) {
        $db = Db::getDb();
        $st = $db -> prepare('SELECT * FROM users WHERE id = :id');
        $st -> bindParam(':id', $id);
        $st -> execute();
        $row = $st -> fetch(PDO::FETCH_ASSOC);
        return new self($row);
    }

    static function findUserByEmail($email) {
        $db = Db::getDb();
        $st = $db -> prepare('SELECT * FROM users WHERE email = :email');
        $st -> bindParam(':email', $email);
        $st -> execute();
        $row = $st -> fetch(PDO::FETCH_ASSOC);
        return new self($row);
    }

    static function isValid($email, $password) {
        $db = Db::getDb();
        $st = $db -> prepare('SELECT * FROM users WHERE email = :email');
        $st -> bindParam(':email', $email);
        $st -> execute();
        $row = $st -> fetch(PDO::FETCH_ASSOC);
        $user = new self($row);
        if (isset($user)) {
          return $user -> getPassword() === $password;
        }
        else {
          return false;
        }
    }
    public function userExists() {
        $db = Db::getDb();
        $st = $db -> prepare('SELECT count(*) as cnt FROM users WHERE email = :email');
        $st -> bindParam(':email', $this -> email);
        $st -> execute();
        $count = $st -> fetch(PDO::FETCH_ASSOC);
        if (isset($count) ) {
          return $count['cnt'] == 1;
        }
        else {
          return false;
        }
    }

    function insert() {
        $this -> validate(true);
        $this -> clean();
        $db = Db::getDb();
        $statement = $db -> prepare("INSERT INTO users (first_name, last_name, email, password) values (:first_name, :last_name, :email, :password)");
        $statement -> bindParam(':first_name', $this -> first_name);
        $statement -> bindParam(':last_name', $this -> last_name);
        $statement -> bindParam(':email', $this -> email);
        $statement -> bindParam(':password', $this -> password);
        $statement -> execute();
        $this -> id = $db -> lastInsertId();
        return $this;
    }

    function update() {
        $this -> validate(true);
        $this -> clean();
        $db = Db::getDb();
        $statement = $db -> prepare("UPDATE users SET first_name = :first_name, last_name = :last_name, email = :email, password = :password WHERE id = :id");
        $statement -> bindParam(':first_name', $this -> first_name);
        $statement -> bindParam(':last_name', $this -> last_name);
        $statement -> bindParam(':email', $this -> email);
        $statement -> bindParam(':password', $this -> password);
        $statement -> bindParam(':id', $this -> id);
        $statement -> execute();
        return $this;
    }

    function delete() {
        $db = Db::getDb();
        $statement = $db -> prepare("DELETE FROM users WHERE id = :id");
        $statement -> bindParam(':id', $this -> id);
        $statement -> execute();
    }
}
