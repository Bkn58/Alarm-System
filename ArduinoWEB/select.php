<!DOCTYPE html>
<!--
Стартовая страница.
-->
<?php session_start(); ?>
<html>
    <head>
        <meta charset="UTF-8">
        <title></title>
    </head>
    <body>
        <form  method="POST" action="next.php">
            <p>Строк на странице
                <input type="number" name="count" min="1" step="1" size="4" maxlength="4" autofocus
                value="<?php echo $_SESSION['count_row'] ?>">
            Смещение
                <input type="number" name="delta" min="0" step="1" size="4" maxlength="4" autofocus
                value="<?php echo $_SESSION['delta_row'] ?>">
            <p>
            <input type= "submit" value="Select" name="next" width="20" height="10" />
            <?php
            // подключаем модули
            require_once 'config/config.php';
            require_once 'db_operation.php';

            $db = new db_operation ();     // создаем объект база данных
            $db_conn = $db->db_connect(); // подключаемся к БД
            $db->db_select();
            ?>
        </button>
    </form>
</body>
</html>
