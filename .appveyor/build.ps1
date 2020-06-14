$ErrorActionPreference = "Stop"

Set-Location 'c:\projects\pecl-pcs'

$task = New-Item 'task.bat' -Force
Add-Content $task "call phpize 2>&1"
Add-Content $task "call configure --enable-pcs --enable-debug-pack 2>&1"
Add-Content $task "nmake /nologo 2>&1"
Add-Content $task "exit %errorlevel%"
& "c:\build-cache\php-sdk-$env:BIN_SDK_VER\phpsdk-$env:VC-$env:ARCH.bat" -t $task
if (-not $?) {
    throw "build failed with errorlevel $LastExitCode"
}

$source = ''
if ($env:ARCH -eq 'x64') {
    $source += 'x64\'
}
$source += 'Release';
if ($env:TS -eq '1') {
    $source += '_TS'
}

$file = Get-Command php | Select-Object -ExpandProperty Definition
$dest = (Get-Item $file).Directory.FullName

Copy-Item "$source\php_pcs.dll" "$dest\ext\php_pcs.dll"

$ini = New-Item "$dest\php-cli.ini" -Force
Add-Content $ini "extension_dir=$dest\ext"
Add-Content $ini 'extension=php_pcs.dll'
