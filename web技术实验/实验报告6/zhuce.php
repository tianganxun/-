<?php
/**
 * Created by PhpStorm.
 * User: tianganxun
 * Date: 2018/6/13
 * Time: 12:04
 */
header("content-type:text/html;charset=utf-8");
date_default_timezone_set('prc');

$username=$_GET['username'];
$password=$_GET['password'];
$repassword=$_GET['repassword'];

$con=mysqli_connect("localhost","root","tgx140455");
if (!$con) {
    die('数据库连接失败');
}

mysqli_select_db($con,"dazuoye");
$dbusername=null;
$dbpassword=null;
$t=time();

$d=date("m月d日 H:i",$t);

$newdbdate=$d;
$result=mysqli_query($con,"select * from user_info where username ='{$username}'");
while ($row=mysqli_fetch_array($result)) {
    $dbusername=$row["username"];
    $dbpassword=$row["password"];





}
if (!is_null($dbusername)) {
    ?>
    <script type="text/javascript">
        alert("用户名已经存在");
        window.location.href="注册.html";
    </script>
    <?php
}
else {
    if ($repassword!=$password){
        ?>
        <script type="text/javascript">
            alert("密码不一致");
            window.location.href="注册.html";
        </script>
        <?php
    }
    else {
        mysqli_query($con,"insert into user_info (username,password) values('{$username}','{$password}')");
        mysqli_query ($con, "update user_info set 访问次数='1' where username='{$username}'" );
        mysqli_query ($con, "update user_info set 推荐数='10' where username='{$username}'" );
        mysqli_query ($con, "update user_info set 点赞数='100' where username='{$username}'" );
        mysqli_query ($con, "update user_info set 被点赞数='0' where username='{$username}'" );
        mysqli_query ($con, "update user_info set 被推荐数='0' where username='{$username}'" );
        mysqli_query ($con, "update user_info set 上次访问时间='{$d}' where username='{$username}'" );

        ?>
        <script type="text/javascript">
            alert("注册成功，请重新登录");
            window.location.href="denglu.html";
        </script>
        <?php
    }
}
mysqli_close($con);
?>