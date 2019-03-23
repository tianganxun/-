


<?php
/**
 * Created by PhpStorm.
 * User: tianganxun
 * Date: 2018/6/16
 * Time: 16:52
 */
header("content-type:text/html;charset=utf-8");
$q=$_GET["q"];


$con=mysqli_connect("localhost","root","efd98a2622");
if (!$con) {
    die('数据库连接失败');
}
mysqli_select_db($con,"dazuoye");
switch ($q)
{
    case "remen":
        $rs=mysqli_query($con,"SELECT * FROM rec order by rec_count desc limit 8");
  break;
    case "zuixin":
        $rs=mysqli_query($con,"SELECT * FROM rec  order by rec_id desc limit 8");
        break;
    case "zipai":
        $rs=mysqli_query($con,"SELECT * FROM rec where rec_class= '自拍' order by rec_count desc limit 8");

        break;
    case "jingwu":
        $rs=mysqli_query($con,"SELECT * FROM rec where rec_class= '景物' order by rec_count desc limit 8");
        break;
    case "xiaochi":
        $rs=mysqli_query($con,"SELECT * FROM rec where rec_class= '小吃' order by rec_count desc limit 8");
        break;
    case "techan":
        $rs=mysqli_query($con,"SELECT * FROM rec where rec_class= '特产' order by rec_count desc limit 8");
        break;
    default:

}
//$rs=mysqli_query($con,"SELECT * FROM rec");



if(!$rs){die("Va testlid result!");}

while($row = mysqli_fetch_object($rs))

echo "<div style='box-shadow:  0 2px 5px -3px #808080; height: 200px;margin: 13px 80px 20px 5px '>

<div style='float: left;margin-top: 0;margin-left: 6px;padding-right: 10px '><img src='$row->rec_src' height='180' width='270'></div>
<div style='float: left '>

<div style='height: 150px;width: 600px'>
<div><p style='word-wrap: normal;font-size: 20px'><span style='color: red'>#$row->rec_tag</span></br></br>$row->rec_text</p></div>
<div style='float: bottom'>
 
 
 <p style='color: grey'>&nbsp;来自&nbsp;&nbsp;$row->rec_user_name &nbsp;  &nbsp; &nbsp; &nbsp; &nbsp; $row->rec_date 
  <span style='float: right'> <img  src=\"d1.png\"  height=\"16\" width=\"16\" onclick=\"javascript:closeLogin();\"/> &nbsp;$row->rec_count</span></p></div>


</div>


</div>

</div>";






mysqli_close($con);
?>