<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>个人主页</title>



<?php
/**
 * Created by PhpStorm.
 * User: tianganxun
 * Date: 2018/6/16
 * Time: 16:52
 */

header("content-type:text/html;charset=utf-8");
session_start();
$user="未登录";
$tiaoshu=0;
if(isset($_SESSION['username']))
$user=$_SESSION['username'];


$con=mysqli_connect("localhost","root","efd98a2622");
if (!$con) {
    die('数据库连接失败');
}
mysqli_select_db($con,"dazuoye");
$rs=mysqli_query($con,"SELECT * FROM rec where rec_user_name='$user' ");

$tiaoshu=mysqli_num_rows($rs);






?>
    <style>

        footer{width:100%;height:20px;background-color: rgba(0, 0, 0, 0);text-align: center}

        a{
            margin-right: 5px;
            margin-left: 5px;

            text-decoration: none;
        }
        a:link{

            color: black;
        }

        a:visited{

            color: black;
        }
        a:hover {

            color: #FFB941 !important;
        }

        .main_left_dh p:hover{
            color: white;
        }

        i {
            color: red;
            margin-right: 5px;
        }
        .img_logo{
            margin-top: 11px;
            margin-left: 80px;
            width:28px;
            height:28px;
        }
        .main_radius_bg {
            width: 600px;
            height: 400px;

            background-color: #fff;
            border-radius: 6px;
            box-shadow: 1px 2px 3px #98b4c4;
            -moz-box-shadow:1px 2px 3px #98b4c4;
            -webkit-box-shadow: 1px 2px 3px #98b4c4;
        }
        .input_tgx {
            border: 1px solid #ccc;
            border-radius: 2px;
            height: 28px;
            width: 220px;
            vertical-align: middle;
            color: #808080;
            -webkit-box-shadow: 0px 1px 1px 0px #eaeaea inset;
            -moz-box-shadow: 0px 1px 1px 0px #eaeaea inset;
            box-shadow: inset 0px 1px 1px 0px #eaeaea;
            height: 20px\9;
            padding: 8px 0 0 2px\9;


        }

        .inputs {
            -moz-user-select: none;


            border: 1px solid rgba(0, 0, 0, 0.1);
            border-radius: 2px 2px 2px 2px;
            color: #fff;
            cursor: default;
            font-family: arial,sans-serif;
            font-size: 16px;

            font-weight: bold;
            height: 40px;
            width: 224px;
            background: #FFA00A;
            margin: 1px 100px 100px 112px;
            min-width: 54px;
            padding: 0 8px;
            text-align: center;

        }
        input:focus {
            border-color:#FFB941 !important;
            background: #fff;
        }
        .main_left_dh {
            position:fixed;
            left:180px;
            top:60px ;
            margin: 0;
            padding: 0;
            width: 8%;
            height: 50vh;
            _display: inline;


        }

        .main_right_dh {
            position:absolute;
            left:1300px;
            top: 45px;
            margin: 50px;
            width: 15%;

            _display: inline;


        }

        .zhanshi{
            position:absolute;
            left:280px;
            top: 19px;
            padding-left: 40px;
            margin: 50px;
            width: 60%;

            _display: inline;

        }
        .zhanshi_part{
            background: #d2695f;
            height: 400px;
            margin-top: 13px;
            margin-right: 80px;
            margin-left: 80px;
            margin-bottom: 5px;
        }

        .input_main {
            margin: 30px 15px;
        }


        .main_head1 {

            height:8%;
            width: 100%;

            _display: inline;
        }


        .main_left {
            float:left;

            width: 100%;

            _display: inline;


        }

        .img_close{
            position:absolute;
            right:1px;
            top:1px;
        }

        .img_yanzhen{
            position:absolute;
            left:470px;
            top:204px;
        }
        .nav {
            width: 100%;
            height:40px;
            box-shadow: 0px 3px 5px -3px #808080;
            padding-bottom: 7px;
            position:fixed;
            top: 0;
            z-index:12;
            background: white;
        }
        .nav_right{
            float: right;
            font-family:"Comic Sans MS", cursive, sans-serif;
            margin-top: 13px;
            margin-right: 80px;
            margin-bottom: 5px;
            font-size: 16px;

        }

        .component-topNav-input {
            position:absolute;
            top:9px;
            left:150px;
            width: 15%;

            border:1px solid rgba(6, 6, 6, 0.19);
            margin-left: 20px;
            color: #000;
            height: 27px;
            padding-right: 1.07rem;

            outline: rgba(0, 0, 0, 0.08) solid 1px;

            background: #eaeaea;
        }
        ul{
            height:100%;
            margin: 0;
            padding: 0;

        }
        li{
            width: 100%;
            height: 66px;
            background: rgba(0, 0, 0, 0.07);
            margin: 1px;
            list-style-type: none;
            border-radius:5px;
            font-size: 20px;
            text-align: center;
            line-height: 66px;
            font-family:"Comic Sans MS", cursive, sans-serif;
        }
        li:hover{
            background: red;
        }

        li.active{
            background: #5ff5ff;
        }
        #footer1{
            width: 99%;
            height:100px;
            border-top:1px solid #CCC;
            position:absolute;
            margin-top:700px;

        }
        .foot-let{

            height:80px;


        }

        .button {
            background-color: rgba(6, 6, 6, 0.45); /* Green */
            border: none;
            color: white;
            margin-top: 10px;
            padding: 10px 29px;
            text-align: center;
            text-decoration: none;
            display: inline-block;
            font-size: 16px;
            border-radius:4px ;
        }

        .button1 {
            background-color: #fff; /* Green */
            border: none;
            color: black;
            margin-top: 10px;
            padding: 10px 10px;
            text-align: center;
            text-decoration: none;
            display: inline-block;
            font-size: 21px;
        }


 .img1{border-style: solid;border-width:1px; }
    </style>
</head>
<body onload=" zhuxiao()">
<script>

    function createXMLHttpRequest(){
        if(window.ActiveXObject)
            xmlHttp = new ActiveXObject("Microsoft.XMLHTTP");
        else if(window.XMLHttpRequest)
            xmlHttp = new XMLHttpRequest();
    }
    function zhuxiao(){
        createXMLHttpRequest();
        var zurl="zhuxiao.php";

        xmlHttp.open("GET",zurl,true);

        xmlHttp.onreadystatechange = function(){

            if(xmlHttp.readyState == 4 && xmlHttp.status == 200)
                x.innerHTML=xmlHttp.responseText;

        }

        xmlHttp.send(null);
    }</script>
<div class="nav">

    <div id=win style="display:none; POSITION:fixed; left:50%; top:50%; width:600px; height:400px; margin-left:-300px; margin-top:-200px; text-align:center">
        <div  class=" main_radius_bg">

            <div class=" main_head1">

            </div>

            <img class="img_close" src="1.png" height="16" width="16" onclick="javascript:closeLogin();"/>


            <br>
            <div class=" main_left">
                <form action="denglu.php" method="get" name="form_register">

                    <div class=input_main ><label for="username" ><i>*</i>您的昵称：</label>
                        <input class=input_tgx type="text" id="username" name="username"  placeholder="请输入用户名" ></div>

                    <div class=input_main > <label for="password" ><i>*</i>您的密码：</label>
                        <input class=input_tgx id="password" name="password"  placeholder="请输入密码"></div>

                    <div class=input_main > <label for="ckcode" ><i>*</i>输验证码：</label>
                        <input class=input_tgx id="ckcode" name="ckcode"  placeholder="请输入验证码 点击可换"></div>
                    <img src="yanzhen.php" class="img_yanzhen" height="32"  style="cursor:pointer" onclick="refreshcode(this,this.src)">

                    <script language="javascript" type="text/javascript">
                        function refreshcode(obj,url){
                            obj.src=url+"?nowtime="+Math.random();


                        }

                    </script>

                    <br>
                    <div> <input class="inputs" type="submit" value="登录"></div>

                </form>
            </div>








        </div>
    </div>
    <script>
        function openLogin(){
            document.getElementById("win").style.display="";
        }
        function closeLogin(){

            document.getElementById("win").style.display="none";
        }
        function zhuye() {

            window.location.href="zhuye.html";
        }

    </script>


    <img class="img_logo"  style= "display:inline " src="lv.png" onclick="zhuye()">

    <div style= "display:inline ">
        <input class="component-topNav-input" type="text"></div>
    <div id="x" class="nav_right" style= "display:inline ">

        <a href="javascript:openLogin();">登录</a>| <a href="register.html">注册</a></div>
</div>
















<div >
    <img class="img1" style="position: absolute; top: 80px;left: 320px; height: 130px;width: 130px" src="100.jpg">
<?php
echo "
<div>
   <p style='position: absolute; top: 60px;left: 520px;font-size: 28px;color: rgba(0,0,0,0.63)'> $user
   </br>
   <span style='font-size:19px '>共 $tiaoshu 条动态</span>
    </br>
    <button class='button' onclick=' window.open(\"fabu.html\")'>发布动态</button>
   </p></div>
     <div style='padding-top: 250px;padding-left: 320px;padding-bottom: 40px'>
     
     <p><button class='button1'>动态</button>|<button class='button1'>点赞</button>|<button class='button1'>信息</button></p>
</div>
"
?>
</div>
<div>

    <?php


    while($row = mysqli_fetch_object($rs))
    {
    echo "<div style='box-shadow:  0 2px 5px -3px #808080; height: 200px;margin: 13px 80px 20px 5px ;width: 60%;float: left;margin-left: 300px'>

        <div style='float: left;margin-top: 0;margin-left: 6px;padding-right: 10px '><img src='$row->rec_src' height='180' width='270'></div>
        <div style='float: left '>

            <div style='height: 150px;width: 600px'>
                <div><p style='word-wrap: normal;font-size: 20px'><span style='color: red'>#$row->rec_tag</span></br></br>$row->rec_text</p></div>
                <div style='float: bottom'>


                    <p style='color: grey'>&nbsp;来自&nbsp;&nbsp;$row->rec_user_name &nbsp;  &nbsp; &nbsp; &nbsp; &nbsp; $row->rec_date
                        <span style='float: right'> <img  src=\"d1.png\"  height=\"16\" width=\"16\" onclick=\"javascript:closeLogin();\"/> &nbsp;$row->rec_count</span></p></div>
               

            </div>


        </div>

    </div>";}
    mysqli_close($con);
    ?>
</div>
<div id="footer1" >
    <div class="foot-let">
        <p style="line-height:35px; height:35px; color:#666; font-size:14px; margin-left:8px;text-align: center">同济大学电子信息学院计科专业软件工程课程设计</p>
        <p style="line-height:35px; height:35px; color:#666; font-size:14px; margin-left:8px;text-align: center">&copy;2018 tgxjzh </p>
    </div>

</div>
</body>


</html>
