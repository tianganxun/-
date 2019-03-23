<?php
/**
 * Created by PhpStorm.
 * User: tianganxun
 * Date: 2018/6/16
 * Time: 16:52
 */
header("content-type:text/html;charset=utf-8");

session_start();
$user=null;
if(isset($_SESSION['username']))
$user=$_SESSION['username'];
if($user==null){
  echo " <a href=\"javascript:openLogin();\">登录</a>| <a href=\"register.html\">注册</a>";
}
else
{
echo " <a id='m' name='$user' >$user</a>| <a href=\"zhuxiao1.php\">注销</a>";
}






?>