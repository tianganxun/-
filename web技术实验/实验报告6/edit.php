<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <title>修改动态</title>
</head>
<body>
<?php
$link=mysqli_connect("localhost","root","efd98a2622");

mysqli_select_db($link,"dazuoye");

$id = $_GET['id'];
$sql = mysqli_query($link," SELECT * FROM rec WHERE rec_id=$id" );
$sql_arr = mysqli_fetch_assoc($sql);

?>

<form action="edit1.php" method="post">

    <label>ID: </label><input type="text" name="rec_id" value="<?php echo $sql_arr['rec_id']?>" readonly="readonly">
    <label>发布者：</label><input type="text" name="rec_user_name" value="<?php echo $sql_arr['rec_user_name']?>">
    <label>描述：</label><input type="text" name="rec_text" value="<?php echo $sql_arr['rec_text']?>">
    <label>被赞数：</label><input type="text" name="rec_count" value="<?php echo $sql_arr['rec_count']?>">
    <label>关键词：</label><input type="date" name="rec_tag" value="<?php echo $sql_arr['rec_tag']?>">
    <label>类别：</label><input type="text" name="rec_class" value="<?php echo $sql_arr['rec_class']?>">
    <input type="submit" value="提交">
</form>

</body>
</html>