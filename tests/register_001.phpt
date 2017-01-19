--TEST--
Register/unregister/list functions
--FILE--
<?php
function foo ($class) {
        echo "Here\n";
}

echo("1-----\n");

var_dump(spl_autoload_functions());

echo("2-----\n");

var_dump(spl_autoload_register("foo"));

echo("3-----\n");

var_dump(spl_autoload_functions());

echo("4-----\n");

var_dump(spl_autoload_unregister("foo"));

echo("5-----\n");

var_dump(spl_autoload_functions());
?>
===DONE===
--EXPECTF--
1-----
bool(false)
2-----
bool(true)
3-----
array(1) {
  [0]=>
  string(3) "foo"
}
4-----
bool(true)
5-----
array(0) {
}
===DONE===
