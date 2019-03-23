<?php
header("content-type:text/html;charset=utf-8");
session_start();
date_default_timezone_set('prc');
if($_FILES["file"]["error"])
{
    echo $_FILES["file"]["error"];
}
else
{
    //控制上传文件的类型，大小
    if(($_FILES["file"]["type"]=="image/jpeg" || $_FILES["file"]["type"]=="image/png") && $_FILES["file"]["size"]<1024000)
    {
        //找到文件存放的位置
        $filename = "./file/".date("YmdHis").$_FILES["file"]["name"];

        //转换编码格式
        $filename = iconv("UTF-8","gb2312",$filename);

        //判断文件是否存在
        if(file_exists($filename))
        {
            echo "该文件已存在！";
        }
        else
        {
            //保存文件
            move_uploaded_file($_FILES["file"]["tmp_name"],$filename);

            echo $filename;

            echo "<img src=$filename>";

        }
    }
    else
    {
        echo "文件类型不正确！";
    }
}
$rec_text=$_POST['rec_text'];
$rec_tag=$_POST['rec_tag'];
$rec_class=$_POST['rec_class'];
$rec_src=$filename;
$rec_user=$_SESSION['username'];
$t1=time();

$d1=date("m月d日 H:i",$t1);

$rec_date=$d1;


$con=mysqli_connect("localhost","root","efd98a2622");
if (!$con) {
    die('数据库连接失败');
}


mysqli_select_db($con,"dazuoye");
mysqli_query($con,"insert into rec (rec_class,rec_tag,rec_text,rec_src,rec_date,rec_user_name,rec_count) values('$rec_class','$rec_tag','$rec_text','$rec_src','$rec_date','$rec_user','0')");


//mysqli_query($con,"insert into rec (rec_class,rec_tag,rec_text,rec_src,rec_count) values('$rec_class','$rec_tag','$rec_text','$rec_src','0')");
//mysqli_query($con,"insert into rec () values()");


//mysqli_query($con,"insert into rec (rec_src,rec_user_name,rec_text,rec_date,rec_tag,rec_class) values('{$rec_src}','{$rec_user}'，'{$rec_text}','{$rec_date}'，'{$rec_tag}','{$rec_class}')");
echo "发布成功";
$link2='zhuye.html';
echo" <a href='{$link2}'>返回主页»</a>";
echo "<br />";
$link3='fabu.html';
echo" <a href='{$link3}'>继续发布»</a>";

mysqli_close($con);
?>