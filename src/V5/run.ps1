$dir = (wsl wslpath -u ($PSScriptRoot -replace '\\', '//')).Trim()

# On compile les 3 versions distinctes
wsl.exe bash -c "cd $dir && make cleanall && make"

# 1. Lancer le Dataserveur (Fenêtre 1)
$cmd1 = "wsl.exe bash -c 'cd $dir && ./dataserveur ; read'"
Start-Process powershell -ArgumentList "-NoExit", "-Command", "`$host.UI.RawUI.WindowTitle = 'DATASERVEUR' ; $cmd1"
Start-Sleep -Seconds 1 # Attend que le DS crée son pipe

# 2. Lancer le Routeur (Fenêtre 2)
$cmd2 = "wsl.exe bash -c 'cd $dir && ./routeur ; read'"
Start-Process powershell -ArgumentList "-NoExit", "-Command", "`$host.UI.RawUI.WindowTitle = 'ROUTEUR' ; $cmd2"
Start-Sleep -Seconds 1 # Attend que le Routeur soit prêt pour le client

# 3. Lancer le Client (Fenêtre 3)
$cmd3 = "wsl.exe bash -c 'cd $dir && ./client ; read'"
Start-Process powershell -ArgumentList "-NoExit", "-Command", "`$host.UI.RawUI.WindowTitle = 'CLIENT' ; $cmd3"