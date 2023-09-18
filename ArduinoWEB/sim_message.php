<?php 
session_start();
echo htmlspecialchars($_SERVER["PHP_SELF"]). "<br>";
if ($_SERVER["REQUEST_METHOD"] == "GET") {
   $logF = fopen ("message.log", "a+") or die ("Невозможно открыть log-файл!");
	$str = date("Y-m-d H:i:s").";";
        $str .= $_SERVER["REMOTE_ADDR"].";";
	$str .= $_GET["y"].";"; //datetime
        $str .= $_GET["b"].";";  //ibutton
	$str .= $_GET["a"].";";    //alarm
	$str .= $_GET["r"].";";      //arm
	$str .= $_GET["d"].";";     //door
	$str .= $_GET["s"].";";     //sens
	$str .= $_GET["p"].";";    //power
	$str .= $_GET["c"].";";     //accu
	$str .= $_GET["t"].";";     //temp
	$str .= $_GET["h"].";"; //humidity
	$str .= $_GET["u"].";";       //CO
	$str .= $_GET["o"].";";      //CO2
	$str .= $_GET["g"].";\n";  //integ
	echo $str;
	fwrite ($logF,$str);
	fclose ($logF);
        
        $_SESSION['login'] = $_GET["l"];       //login
        $_SESSION['password'] = "bkn" . $_GET["w"] . "$"; //password

        // подключаем модули
        require_once 'config/config.php';
        require_once 'db_operation.php';
        
        $db = new db_operation ();     // создаем объект база данных
        $db_conn = $db->db_connect (); // подключаемся к БД
        
        // проверка входных значений на валидность
        if ($_GET["y"]==NULL) $_GET["y"] = 0; //datetime
        if ($_GET["b"]==NULL) $_GET["b"] = 0;  //ibutton
        if ($_GET["a"]==NULL) $_GET["a"] = 0;    //alarm
        if ($_GET["r"]==NULL) $_GET["r"] = 0;      //arm
        if ($_GET["d"]==NULL) $_GET["d"] = 0;     //door
        if ($_GET["s"]==NULL) $_GET["s"] = 0;     //sens
        if ($_GET["p"]==NULL) $_GET["p"] = 0;    //power
        if ($_GET["c"]==NULL) $_GET["c"] = 0;     //accu
        if ($_GET["t"]==NULL) $_GET["t"] = 0;     //temp
        if ($_GET["h"]==NULL) $_GET["h"] = 0; //humidity
        if ($_GET["u"]==NULL) $_GET["u"] = 0;       //CO
        if ($_GET["o"]==NULL) $_GET["o"] = 0;      //CO2
        if ($_GET["g"]==NULL) $_GET["g"] = 0;    //integ
        
        //формируем массив значений для записи в базу
        $values["servtime"] = date("Y-m-d H:i:s");
        $values["ip_addr"] = $_SERVER["REMOTE_ADDR"];
        $values["datetime"] = mysqli_real_escape_string($db_conn,$_GET["y"]);//datetime
        $values["ibutton"] = mysqli_real_escape_string($db_conn,$_GET["b"]); //ibutton
        $values["alarm"] = mysqli_real_escape_string($db_conn,$_GET["a"]);   //alarm
        $values["arm"] = mysqli_real_escape_string($db_conn,$_GET["r"]);     //arm
        $values["door"] = mysqli_real_escape_string($db_conn,$_GET["d"]);    //door
        $values["sens"] = mysqli_real_escape_string($db_conn,$_GET["s"]);    //sens
        $values["power"] = mysqli_real_escape_string($db_conn,$_GET["p"]);   //power
        $values["accu"] = mysqli_real_escape_string($db_conn,$_GET["c"]);    //accu
        $values["temp"] = mysqli_real_escape_string($db_conn,$_GET["t"]);    //temp
        $values["humidity"] = mysqli_real_escape_string($db_conn,$_GET["h"]);//humidity
        $values["CO"] = mysqli_real_escape_string($db_conn,$_GET["u"]);      //CO
        $values["CO2"] = mysqli_real_escape_string($db_conn,$_GET["o"]);     //CO2
        $values["integ"] = mysqli_real_escape_string($db_conn,$_GET["g"]);   //integ
        
        $db->db_insert($db_conn, $values); // вставить строку в базу
        
        // переход на главную страницу - коментировать для отладки
//        header("Location: index.html"); exit; 
}
