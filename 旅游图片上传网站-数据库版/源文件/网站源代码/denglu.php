<?php
/**
 * Created by PhpStorm.
 * User: tianganxun
 * Date: 2018/6/13
 * Time: 12:04
 */
header("content-type:text/html;charset=utf-8");
date_default_timezone_set('prc');
session_start();
$username=$_GET['username'];
$password=$_GET['password'];
$ckcode=$_GET['ckcode'];
if($ckcode==$_SESSION['rand']){
$con=mysqli_connect("localhost","root","efd98a2622");
if (!$con) {
    die('数据库连接失败');
}
mysqli_select_db($con,"dazuoye");
$dbusername=null;
$dbpassword=null;
$dbcishu=0;
$dbdate=null;
$result=mysqli_query($con,"select * from user_info where username ='{$username}'");
while ($row=mysqli_fetch_array($result)) {
    $dbusername=$row["username"];
    $dbpassword=$row["password"];
    $dbcishu=$row["访问次数"];
    $newdbcishu=$dbcishu+1;
    $dbdate=$row["上次访问时间"];
    $t=time();
    $d=date("m月d日 H:i",$t);
    $newdbdate=$d;
}
if (is_null($dbusername)) {
    ?>
    <script type="text/javascript">
        alert("用户名不存在");
        window.location.href="zhuye.html";
    </script>
    <?php
}
else {
    if ($dbpassword!=$password){
        ?>
        <script type="text/javascript">
            alert("密码错误");
            window.location.href="zhuye.html";
        </script>
        <?php
    }
    else {

        mysqli_query ($con, "update user_info set 访问次数='{$newdbcishu}' where username='{$username}'" );
        mysqli_query ($con, "update user_info set 上次访问时间='{$newdbdate}' where username='{$username}'" );

        $_SESSION['username']=$dbusername;
        echo("您是第");
        echo $dbcishu;
        echo("次访问该网页");
        echo "<br />";
        echo("上次访问时间为");
        echo($dbdate);
        echo "<br />";
        if ($username=="田甘迅")
        {    echo "<br />";
             $link='guanli.html';
             echo" <a href='{$link}'>进入管理界面»</a>";


        }
        else
            {
                echo "<br />";
                $link2='zhuye.html';
                echo" <a href='{$link2}'>返回主页»</a>";
                echo "<br />";
                $link1='geren.html';
                echo" <a href='{$link1}'>个人页面»</a>";
                echo "<br />";
                $link3='fabu.html';
                echo" <a href='{$link3}'>前去发布»</a>";
                }
    }


}
mysqli_close($con);}
else
{
    ?>
    <script type="text/javascript">
        alert("验证码输入错误");
        window.location.href="zhuye.html";
    </script>
    <?php
}
?>