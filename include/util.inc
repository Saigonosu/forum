<?php
function formatAsMarkdown($str) {
    $str = htmlspecialchars(ltrim($str),ENT_QUOTES);
    $str = preg_replace('/\*\*(.+)\*\*/u', '<b>$1</b>', $str);
    $str = preg_replace('/\*([^\*]+)\*/u', '<i>$1</i>', $str);
    $str = preg_replace('/#### ([^\n]*)\n/', "<h4>$1</h4>\n", $str);
    $str = preg_replace('/### ([^\n]*)\n/', "<h3>$1</h3>\n", $str);
    $str = preg_replace('/## ([^\n]*)\n/', "<h2>$1</h2>\n", $str);
    $str = preg_replace('/# ([^\n]*)\n/', "<h1>$1</h1>\n", $str);
    $str = preg_replace('/\[([^\]]+)\]\(([^\)]+)\)/', '<a href=\'$2\'>$1</a>', $str);
    $str = preg_replace('/([^\n\r]{2,})(?:(\r\n){2,}|\r{2,}|\n{2,}|$)/u', "<p>$1</p>\n\n", $str);
    return $str;
}

function safeParam($arr, $index, $default) {
    if ($arr && isset($arr[$index])) {
        return $arr[$index];
    }
    return $default;
}

function resetTags($question_id){
  QuestionTag::deleteByQuestionId($question_id);
}

function buildQuestionTagBridge($question, $tags_array){
  foreach($tags_array as $tag){
    $params = array(
      'question_id' => $question -> getId(),
      'tag_id' => $tag -> getId()
    );
    $question_tag = new QuestionTag($params);
    $question_tag -> insert();
  }
}

function insertTags($tags){
  $rows = array();
  foreach($tags as $tag){
    $rows[] = Tag::findOrCreateByName(array('name' => $tag));
  }
  return $rows;
}

function splitTags($tag_list){
  $tags = explode(' ', $tag_list);
  return array_filter($tags);
}

function buildTagList($tags){
  $links = array();
  $tags = explode(' ', $tags);
  foreach($tags as $tag){
    $links[] = "<a href='/forum/tags/list/$tag'>$tag</a>";
  }
  return implode(' ', $links);
}

function getName($id){
  $user = User::findById($id);
  $first_name = $user -> getFirstName();
  $last_name = $user -> getLastName();
  return "$first_name $last_name";
}

function authenticateUser($id){
  return current_user() === $id;
}

function validateChanges($params, $redirect_string, $opts = array()){
    $missing_params = array();
    foreach(REQUIRED_PARAMS as $param){
      $capitalized_param = ucfirst($param);
      empty($params[$param]) ? array_push($missing_params, "$captialized_param is missing") : 'Do Nothing';
    }
    $opts['errors'] = $missing_params;
    renderTemplate( $redirect_string, $opts);
  }

function loadUser($email) {
  $user = User::findUserByEmail($email);
  $_SESSION['current_user'] = $user -> getId();
}

function current_user(){
  return $_SESSION['current_user'];
}

function isLoggedIn() {
    $inSession = session_id();
    if (!empty($inSession)) {
        if (isset($_SESSION['loggedin'])) {
            return $_SESSION['loggedin'];
        }
    }
    return false;
}

function ensureLoggedIn() {
    if (!isLoggedIn()) {
        $_SESSION['redirect'] = $_SERVER['REQUEST_URI'];
        redirectRelative('login');
        exit();
    }
}

function debug($something) {
    echo "<div class='debug'>\n";
    print_r($something);
    echo "\n</div>\n";
}

function redirect($url) {
    session_write_close();
    header("Location: $url");
    exit();
}

function redirectRelative($url) {
    redirect(relativeURL($url));
}

function relativeUrl($url) {
    $requestURI = explode('/', $_SERVER['REQUEST_URI']);
    $scriptName = explode('/', $_SERVER['SCRIPT_NAME']);

    $dir = array();
    for ($i = 0; $i < sizeof($scriptName); $i++) {
        if ($requestURI[$i] == $scriptName[$i]) {
            $dir[] = $requestURI[$i];
        } else {
            break;
        }
    }
    return implode('/', $dir) . '/' . $url;
}

function __importTemplate($matches) {
    $fileName = $matches[1];
    if (!file_exists($fileName)) {
        die("Template $fileName doesn't exist.");
    }
    $contents = file_get_contents($fileName);
    $contents = preg_replace_callback('/%%\s*(\S*)\s*%%/', '__importTemplate', $contents);
    return $contents;
}

function __resolveRelativeUrls($matches) {
    return relativeUrl($matches[1]);
}

function __cacheName($view) {
    $cachePath = explode('/', $view);
    $idx = sizeof($cachePath) - 1;
    $cachePath[$idx] = 'cache_' . $cachePath[$idx];
    return implode('/', $cachePath);
}

function renderTemplate($view, $params) {
    if (!file_exists($view)) {
        die("File $view doesn't exist.");
    }
    # do we have a cached version?
    clearstatcache();
    $cacheName = __cacheName($view);
    if (false && file_exists($cacheName) && (filemtime($cacheName) >= filemtime($view))) {
        $contents = file_get_contents($cacheName);
    } else {
        # we need to build the file (doesn't exist or template is newer)
        $contents = __importTemplate(array('unused', $view));

        $contents = preg_replace_callback('/@@\s*(.*)\s*@@/U', '__resolveRelativeUrls', $contents);

        // @formatter:off
        $patterns = array(
            array('src' => '/{{/', 'dst' => '<?php echo('),
            array('src' => '/}}/', 'dst' => '); ?>'),
            array('src' => '/\[\[/', 'dst' => '<?php '),
            array('src' => '/\]\]/', 'dst' => '?>')
        );
        // @formatter:on
        foreach ($patterns as $pattern) {
            $contents = preg_replace($pattern['src'], $pattern['dst'], $contents);
        }
        file_put_contents($cacheName, $contents);
    }
    extract($params);
    ob_start();
    eval("?>" . $contents);
    $result = ob_get_contents();
    ob_end_clean();
    echo $result;
}
?>
