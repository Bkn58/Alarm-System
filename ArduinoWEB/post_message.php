<?php 
echo htmlspecialchars($_SERVER["PHP_SELF"]). "<br>";
if ($_SERVER["REQUEST_METHOD"] == "POST") {
   $logF = fopen ("message.log", "a+") or die ("Невозможно открыть log-файл!");
	$str = date("Y-m-d H:i:s").";";
        $str .= $_SERVER["REMOTE_ADDR"].";";
	$str .= $_POST["datetime"].";";
        $str .= $_POST["ibutton"].";";
	$str .= $_POST["alarm"].";";
	$str .= $_POST["arm"].";";
	$str .= $_POST["door"].";";
	$str .= $_POST["power"].";";
	$str .= $_POST["accu"].";";
	$str .= $_POST["temp"].";";
	$str .= $_POST["humidity"].";";
	$str .= $_POST["CO"].";";
	$str .= $_POST["CO2"].";";
	$str .= $_POST["integ"].";\n";
	echo $str;
	fwrite ($logF,$str);
	fclose ($logF);

        // подключаем модули
        require_once 'config/config.php';
        require_once 'db_operation.php';
        
        $db = new db_operation ();     // создаем объект база данных
        $db_conn = $db->db_connect (); // подключаемся к БД
        
        // проверка входных значений на валидность
        if ($_POST["ibutton"]==NULL) $_POST["ibutton"] = 0;
        
        //формируем массив значений для записи в базу
        $values["servtime"] = date("Y-m-d H:i:s");
        $values["ip_addr"] = $_SERVER["REMOTE_ADDR"];
        $values["datetime"] = mysqli_real_escape_string($db_conn,$_POST["datetime"]);
        $values["ibutton"] = mysqli_real_escape_string($db_conn,$_POST["ibutton"]);
        $values["alarm"] = mysqli_real_escape_string($db_conn,$_POST["alarm"]);
        $values["arm"] = mysqli_real_escape_string($db_conn,$_POST["arm"]);
        $values["door"] = mysqli_real_escape_string($db_conn,$_POST["door"]);
        $values["power"] = mysqli_real_escape_string($db_conn,$_POST["power"]);
        $values["accu"] = mysqli_real_escape_string($db_conn,$_POST["accu"]);
        $values["temp"] = mysqli_real_escape_string($db_conn,$_POST["temp"]);
        $values["humidity"] = mysqli_real_escape_string($db_conn,$_POST["humidity"]);
        $values["CO"] = mysqli_real_escape_string($db_conn,$_POST["CO"]);
        $values["CO2"] = mysqli_real_escape_string($db_conn,$_POST["CO2"]);
        $values["integ"] = mysqli_real_escape_string($db_conn,$_POST["integ"]);
        
        $db->db_insert($db_conn, $values); // вставить строку в базу
        
        // переход на главную страницу - коментировать для отладки
//        header("Location: index.html"); exit; 
}
