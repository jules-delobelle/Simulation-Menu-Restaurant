$dir = wsl wslpath -u ($PSScriptRoot -replace '\\', '/')

Start-Process powershell -ArgumentList "-NoExit", "-Command", "`$host.UI.RawUI.WindowTitle = 'Lecteur Final' ; wsl.exe bash -c 'cd $dir ; ./test_lecteur_final ; read'"
Start-Sleep -Milliseconds 500
Start-Process powershell -ArgumentList "-NoExit", "-Command", "`$host.UI.RawUI.WindowTitle = 'Relais' ; wsl.exe bash -c 'cd $dir ; ./test_lecteur_relais ; read'"
Start-Sleep -Milliseconds 500
Start-Process powershell -ArgumentList "-NoExit", "-Command", "`$host.UI.RawUI.WindowTitle = 'Client' ; wsl.exe bash -c 'cd $dir ; ./test_client ; read'"
