<?php

$sum = 0;               //保存总得分
!empty($_POST['s1']) ? $s1 = $_POST['s1'] : $s1 = '';
!empty($_POST['s2']) ? $s2 = $_POST['s2'] : $s2 = '';
!empty($_POST['s3']) ? $s3 = $_POST['s3'] : $s3 = '';
!empty($_POST['s4']) ? $s4 = $_POST['s4'] : $s4 = '';
if ($s1=='A') {
    echo '第1题正确！';
    echo"</br>";
    $sum =  $sum+10;
}
else  { echo '第1题错误！答案为A';    echo"</br>";}
if ($s2=='B') {
    echo '第2题正确！';    echo"</br>";
    $sum =  $sum+10;}
else  { echo '第2题错误！答案为B';    echo"</br>";}

if ($s3=='C') {
    echo '第3题正确！';    echo"</br>";
    $sum =  $sum+10;}
else  { echo '第3题错误！答案为C';    echo"</br>";}
if ($s4=='D') {
    echo '第4题正确！';    echo"</br>";
    $sum =  $sum+10;}
else  { echo '第4题错误！答案为D';    echo"</br>";}



!empty($_POST['program']) ? $program = $_POST['program'] : $program = '';
!empty($_POST['p2']) ? $p2 = $_POST['p2'] : $p2 = '';
!empty($_POST['p3']) ? $p3 = $_POST['p3'] : $p3 = '';
if ($program) {

    if (in_array(2, $program)&&in_array(4,$program)&&in_array(5,$program)&&!in_array(1,$program)&&!in_array(3,$program)) {
        echo '第5题正确';    echo"</br>";
        $sum =  $sum+20;
    }
    else if (!in_array(1,$program)&&!in_array(3,$program)) {

        if(!in_array(2, $program) || !in_array(4,$program) || !in_array(5,$program)){
            echo '第5题回答不全!答案为BDE';    echo"</br>";
            $sum =  $sum+10;
        }


    }else{
        echo '第5题错误！答案为BDE';    echo"</br>";
    }
}else{
    echo '第5题未答！答案为BDE';    echo"</br>";
}

if ($p2) {

    if (in_array(2, $p2)&&in_array(4,$p2)&&!in_array(1,$p2)&&in_array(3,$p2)) {
        echo '第6题正确';    echo"</br>";
        $sum =  $sum+20;
    }
    else if (!in_array(1,$p2)) {

        if(!in_array(2, $p2) || !in_array(4,$p2)|| !in_array(3,$p2) ){
            echo '第6题回答不全!答案为BCD';    echo"</br>";
            $sum =  $sum+10;
        }


    }else{
        echo '第6题错误！答案为BCD';    echo"</br>";
    }
}else{
    echo '第6题未答！答案为BCD';    echo"</br>";
}

if ($p3) {

    if (in_array(2, $p3)&&in_array(4,$p3)&&in_array(1,$p3)&&in_array(3,$p3)) {
        echo '第7题正确';    echo"</br>";
        $sum =  $sum+20;
    }


        if(!in_array(2, $p3) || !in_array(4,$p3) || !in_array(3,$p3)|| !in_array(1,$p3)){
            echo '第7题回答不全!答案为ABCD';    echo"</br>";
            $sum =  $sum+10;
        }

}else{
    echo '第7题未答！答案为ABCD';    echo"</br>";
}

echo '总分为';
echo $sum;

?>