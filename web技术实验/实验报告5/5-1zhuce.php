<?php
header("content-type:text/html;charset=utf-8");
session_start();
$ckcode=$_POST['ckcode'];
if($ckcode==$_SESSION['rand']){
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
$r1=$_POST['1'];
$r2=$_POST['2'];
$r3=$_POST['3'];
$r4=$_POST['4'];
$r5=$_POST['5'];
$r6=$_POST['6'];

$rec_src=$filename;








echo "注册成功";

echo"$r1";

echo"$r2";
echo"$r3";
echo"$r4";
echo"$r5";
echo"$r6";


echo "<br />";}
else
{
    ?>
    <script type="text/javascript">
        alert("验证码输入错误");
        window.location.href="5-1zhuce.html";
    </script>
    <?php
}


?>