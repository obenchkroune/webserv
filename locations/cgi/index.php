<?php
if (isset($_GET['id']))
{
    echo "hello your id is " . $_GET['id'] . ".";
}
else
{
    echo "hello your id is uknown.";
}
?>