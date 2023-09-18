<?php
 session_start();
 //echo session_id(); // идентификатор сессии
/* 
 * операции с базой данных
 */
final class db_operation {
    
    private $conn = null; // текущее подключение
    
    /*
     * Выполняет подключение к базе данных на основе config.ini
     * @return $conn - текущее подключение
     */
    function db_connect() {
        $config = Config::getConfig("db");
        // Создание подключения
        //echo  $config["servername"] . " " . $_SESSION['login']." ". $_SESSION['password'] . " " . $config["database"];
        //$this->conn = new mysqli($config["servername"], $config["username"], $config["password"], $config["database"]);
        $this->conn = new mysqli($config["servername"], $_SESSION['login'], $_SESSION['password'], $config["database"]);

        // Проверяем соединение
        if ($this->conn->connect_error) {
            die("Ошибка подключения: "); // . $this->conn->connect_error);
        }
        //echo "Подключение успешно";
        return $this->conn;
    }
    /**
     * выполняет insert в базу данных
     * @param $db_conn - текущее подключение
     *        $values - массив значений
     */
    function db_insert ($db_conn, array $values) {
        if ($this->conn == NULL) {
            echo "Не создано текущее подключение к базе\n";
            return;
        }
        $sql = "INSERT INTO log ";
        $sql .= "(servtime,ip_adr,cntrtime,ibutton,alarm,arm,door,sens,power,accu,temp,hum,CO,CO2,integ)";
        $sql .= "VALUES (";
        $sql .= "\"" . $values["servtime"] . "\",";
        //$sql .= "now(),";
        $sql .= "\"" . $values["ip_addr"] . "\",";
        $sql .= "\"" . $values["datetime"] . "\",";
        $sql .= $values["ibutton"] . ",";
        $sql .= "\"" . $values["alarm"] . "\",";
        $sql .= "\"" . $values["arm"] . "\",";
        $sql .= "\"" . $values["door"] . "\",";
        $sql .= "\"" . $values["sens"] . "\",";
        $sql .= "\"" . $values["power"] . "\",";
        $sql .= "\"" . $values["accu"] . "\",";
        $sql .= $values["temp"] . ",";
        $sql .= $values["humidity"] . ",";
        $sql .= $values["CO"] . ",";
        $sql .= $values["CO2"] . ",";
        $sql .= $values["integ"] . ")";
        if(!mysqli_query($this->conn, $sql)){
           echo "<br>" . "\nERROR: Не удалось выполнить " . "<br>" . $sql . "<br>" . mysqli_error($db_conn);
        }

    }
    /**
     * select с вставкой в таблицу
     * @param $db_conn - текущее подключение
     */
    function db_select() {
        $row_cnt=0;
        $delta_row = $_SESSION["delta_row"]; // сколько строк надо пропустить
        if ($this->conn == NULL) {
            echo "Не создано текущее подключение к базе\n";
            return;
        }
        $sql = "SELECT * FROM log ORDER BY servtime DESC";
        if ($result = $this->conn->query($sql)) {
            if ($result->num_rows > 0) {
                echo "<table border=э'1'>";
                echo "<tr>";
                echo "<th>id</th>";
                echo "<th>servtime</th>";
                echo "<th>ip_addr</th>";
                echo "<th>datetime</th>";
                echo "<th>ibutton</th>";
                echo "<th>alarm</th>";
                echo "<th>arm</th>";
                echo "<th>door</th>";
                echo "<th>sens</th>";
                echo "<th>power</th>";
                echo "<th>accu</th>";
                echo "<th>temp</th>";
                echo "<th>humidity</th>";
                echo "<th>CO</th>";
                echo "<th>CO2</th>";
                echo "<th>integ</th>";
                echo "</tr>";
                while ($row = $result->fetch_array()) {
                    if ($delta_row==0) {
                    echo "<tr>";
                    echo "<td>" . $row['id'] . "</td>";
                    echo "<td>" . $row['servtime'] . "</td>";
                    echo "<td>" . $row['ip_adr'] . "</td>";
                    echo "<td>" . $row['cntrtime'] . "</td>";
                    echo "<td>" . $row['ibutton'] . "</td>";
                    if( $row['alarm']!= "alarm")
                        echo "<td style=background:white>" . $row['alarm'] . "</td>";
                    else
                        echo "<td style=background:red>" . $row['alarm'] . "</td>";
                    echo "<td>" . $row['arm'] . "</td>";
                    if ($row['door']!="close"){
                        echo "<td style=background:red>" . $row['door'] . "</td>"; 
                    }
                    else {
                        echo "<td style=background:white>" . $row['door'] . "</td>"; 
                    }
                    if ($row['sens']!="0"){
                        echo "<td style=background:red>" . $row['sens'] . "</td>"; 
                    }
                    else {
                        echo "<td style=background:white>" . $row['sens'] . "</td>"; 
                    }
                    if ($row['power']!="fault") {
                        echo "<td style=background:white>" . $row['power'] . "</td>";
                    }
                    else {
                        echo "<td style=background:red>" . $row['power'] . "</td>";
                    }
                    $volt = 0.0 + substr ($row['accu'],0,4);
                    if ($volt < 3.8){
                        echo "<td style=background:red>" . $row['accu'] . "</td>";
                    }
                    else{
                        echo "<td style=background:white>" . $row['accu'] . "</td>";
                    }
                    if($row['temp']>40) {
                        echo "<td style=background:red>" . $row['temp'] . "</td>";
                    }
                    else {
                        echo "<td style=background:white>" . $row['temp'] . "</td>";
                    }
                    echo "<td>" . $row['hum'] . "</td>";
                    echo "<td>" . $row['CO'] . "</td>";
                    echo "<td>" . $row['CO2'] . "</td>";
                    if ($row['integ']>300) {
                      echo "<td style=background:red>"  . $row['integ'] . "</td>";
                    }
                    else {
                      echo "<td style=background:white>"  . $row['integ'] . "</td>";  
                    }
                    echo "</tr>";
                    $row_cnt++;
                    if ($row_cnt == $_SESSION['count_row']) break;
                    }
                    else {
                        $delta_row  -= 1;
                    }
                }
                echo "</table>";
                //echo "<input type= \"button\" value=\"Next\" width=\"10\" height=\"10\" onclick=\"next.php\"/>";
                //echo "Records = $row_cnt";
                // Доступный набор результатов
                $result->free();
            } else {
                echo "Записей, соответствующих вашему запросу, не найдено.";
            }
            } else {
                echo "ОШИБКА: не удалось выполнить запрос. " . $mysqli->error;
            }
    }
        public function __destruct() {
        // close db connection
        $this->conn = null;
        //echo "<br>" . "close db connection";
    }

}