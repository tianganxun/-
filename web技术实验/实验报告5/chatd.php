<?php
/**
 * Created by PhpStorm.
 * User: tianganxun
 * Date: 2018/7/1
 * Time: 16:04
 */
session_start();
$username=$_GET['username'];
$password=$_GET['password'];
$_SESSION['username']=$username;
$link1='chat.html';
echo" <a href='{$link1}'>进入聊天室»</a>";
echo"$username";
