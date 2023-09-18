<!DOCTYPE html>
<!--

-->
<?php
session_start();

    $_SESSION['login'] = $_POST["login"];
    $_SESSION['password'] = $_POST["password"];
    $_SESSION['delta_row'] = 0;    // сдвиг на кол-во строк от начала select
    $_SESSION['count_row'] = 10;   // количество строк на странице
header("Location: select.php"); exit; 
?>

