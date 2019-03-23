<?php
/**
 * Created by PhpStorm.
 * User: tianganxun
 * Date: 2018/6/19
 * Time: 20:34
 */
header("content-type:text/html;charset=utf-8");
session_start();


$_SESSION = array();


if(isset($_COOKIE[session_name()]))
{
    setCookie(session_name(),'',time()-3600,'/');
}

session_destroy();
?>
<script type="text/javascript">
    alert("注销成功");
    window.location.href="zhuye.html";
</script>
?>