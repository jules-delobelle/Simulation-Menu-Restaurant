$dir = (wsl wslpath -u ($PSScriptRoot -replace '\\', '//')).Trim()

$cmd1 = "wsl.exe bash -c 'cd " + $dir + " && ./lecteur_final ; read'"
$cmd2 = "wsl.exe bash -c 'cd " + $dir + " && ./lecteur_relais ; read'"
$cmd3 = "wsl.exe bash -c 'cd " + $dir + " && ./client ; read'"

Start-Process powershell -ArgumentList "-NoExit", "-Command", "`$host.UI.RawUI.WindowTitle = 'Lecteur Final' ; $cmd1"
Start-Sleep -Milliseconds 500
Start-Process powershell -ArgumentList "-NoExit", "-Command", "`$host.UI.RawUI.WindowTitle = 'Relais' ; $cmd2"
Start-Sleep -Milliseconds 500
Start-Process powershell -ArgumentList "-NoExit", "-Command", "`$host.UI.RawUI.WindowTitle = 'Client' ; $cmd3"