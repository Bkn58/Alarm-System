<?php

/* 
 * Выборка следующей порции данных со смещением delta от начала resultset
 */
    session_start();
    $delta = $_POST["delta"];
    $_SESSION["delta_row"] = $delta;
    $count = $_POST["count"];
    $_SESSION["count_row"] = $count;
    header("Location: select.php"); exit; 