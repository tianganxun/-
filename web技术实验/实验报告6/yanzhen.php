<?php
session_start();
//生成验证码图片
Header("Content-type: image/PNG");
$rand ="";
for($i=0;$i<4;$i++){
    $rand.=dechex(rand(0,15));
}

$im = imagecreate(100,30); //制定图片背景大小

$black = ImageColorAllocate($im, 0,0,0); //设定三种颜色
$white = ImageColorAllocate($im, 255,255,255);
$gray = ImageColorAllocate($im, 200,200,200);
for($i=0;$i<3;$i++){
    $te=imagecolorallocate($im,rand(0,255),rand(0,255),rand(0,255));
}
imagefill($im,0,0,$gray); //采用区域填充法，设定（0,0）



while(($rand=rand()%100000)<10000);
//将四位整数验证码绘入图片
$_SESSION['rand']=$rand;

imagestring($im, 5, 30, 10, $rand, $te);
// 用 col 颜色将字符串 s 画到 image 所代表的图像的 x，y 座标处（图像的左上角为 0, 0）。
//如果 font 是 1，2，3，4 或 5，则使用内置字体

for($i=0;$i<200;$i++) //加入干扰象素
{
    $randcolor = ImageColorallocate($im,rand(0,255),rand(0,255),rand(0,255));
    imagesetpixel($im, rand()%100 , rand()%50 , $randcolor);
}
imageline($im,0,rand(0,100),100,rand(0,15),$randcolor);
imageline($im,0,rand(0,100),100,rand(0,15),$randcolor);
imageline($im,0,rand(0,100),100,rand(0,15),$randcolor);
ImagePNG($im);
ImageDestroy($im);

?>