<?php  include_once 'include/util.inc'; ?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html>
    <head>
        <title><?php echo($title); ?></title>
        <link rel="stylesheet" href="/static/style.css" />
    </head>
    <body>
        <div class="content">
            <div class='header'>
<ul id="navigation">
<?php  if (isLoggedIn()) : ?>
  <li class="left">Welcome, <?php echo(getName(current_user())); ?></li>
  <li class="right"><a href="/logout">Log out</a></li>
  <li class="right"><a href="/users/view/<?php echo(current_user()); ?>">My profile</a></li>
<?php  else : ?>
  <li class="right"><a href="/login">Log in</a></li>
  <li class="right"><a href="/users/add">Register</a></li>
<?php  endif; ?>
  <li class="right"><a href="/index">Home</a></li>
</ul>
            </div>
            <div style='clear: both;'>

<h1><?php echo($title); ?></h1>
<h2>Recent Questions</h2>
    <?php  foreach ($questions as $question) : ?>
    <div class='shortquestion'>
        <span class='title'><a href="/questions/view/<?php echo($question -> getId()); ?>"><?php echo($question -> getTitle()); ?></a></span>
        <span class='date'>Posted on: <?php echo(substr($question -> getInsertTs(),0,10)); ?> by <a href="/users/view/<?php echo($question -> getUserId()); ?>"><?php echo($question -> getUser() -> getFirstName()); ?> <?php echo($question -> getUser() -> getLastName()); ?></a></span>
        <?php  if($question -> getTags() !== null) : ?>
        <span class='tags'>Filed Under: <?php echo(buildTagList($question -> getTags())); ?></span>
        <?php  endif; ?>
        <?php  if(strlen($question -> getContent()) > 100) : ?>
          <span class='question'><?php echo( formatAsMarkdown(substr($question -> getContent(), 0, 100)."..."))?></span>
        <?php  else : ?>
          <span class='question'><?php echo( formatAsMarkdown($question -> getContent()) )?></span>
        <?php  endif; ?>
    </div>
    <?php  endforeach; ?>
<?php  if (isLoggedIn()) : ?>
  <a href="/questions/add">Add a question</a>
<?php  endif; ?>

        <div class="footer">
            Copyright &copy; 2017 Ryan Devlin
        </div>
        </div><!-- content div -->
    </body>
</html>

