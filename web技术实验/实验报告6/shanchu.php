<?php



$link=mysqli_connect("localhost","root","tgx140455");
// 选择数据库
mysqli_select_db($link,"dazuoye");
$id=null;
$id = $_GET['id'];
echo $id;
//删除指定数据
mysqli_query($link,"DELETE FROM rec WHERE rec_id={$id}") or die('删除数据出错：'.mysqli_error());
// 删除完跳转到新闻页
header("Location:guanli.php");