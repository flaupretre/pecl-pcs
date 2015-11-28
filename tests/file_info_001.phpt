--TEST--
Retrieve information about virtual files
--FILE--
<?php

var_dump(PCS\Mgr::fileCount());

print_r(PCS\Mgr::fileInfos());

?>
===DONE===
--EXPECTF--
int(3)
Array
(
    [0] => Array
        (
            [flags] => 3
            [load] => 3
            [size] => %d
            [path] => internal/parser/ParserInterface.php
        )

    [1] => Array
        (
            [flags] => 3
            [load] => 3
            [size] => %d
            [path] => internal/parser/StringParser.php
        )

    [2] => Array
        (
            [flags] => 3
            [load] => 3
            [size] => %d
            [path] => internal/tools/embed.php
        )

)
===DONE===
