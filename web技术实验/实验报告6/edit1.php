<?php
$link=mysqli_connect("localhost","root","efd98a2622");

mysqli_select_db($link,"dazuoye");




$a1 = $_POST['rec_id'];
$a2 = $_POST['rec_class'];
$a3 = $_POST['rec_user_name'];
$a4 = $_POST['rec_text'];
$a5 = $_POST['rec_count'];
$a6 = $_POST['rec_tag'];

//mysqli_query($con,"insert into rec (rec_class,rec_tag,rec_text,rec_src,rec_date,rec_user_name,rec_count) values('$rec_class','$rec_tag','$rec_text','$rec_src','$rec_date','$rec_user','0')");
mysqli_query($link,"UPDATE rec SET rec_id='$a1',rec_class='$a2',rec_user_name='$a3',rec_text='$a4',rec_count='$a5',rec_tag='$a6' WHERE rec_id=$a1") or die('修改数据出错：'.mysqli_error());
header("Location:guanli.php");