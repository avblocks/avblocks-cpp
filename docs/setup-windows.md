# Setup for Windows

> Setup instructions for Windows 10. All scripts are `PowerShell`.

## PowerShell

In PowerShell as Administrator:

```powershell
Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope LocalMachine
```

> This will allow you to execute PowerShell scripts locally on your machine.

## Chocolatey

In PowerShell as Administrator:

```powershell
Set-ExecutionPolicy AllSigned
Set-ExecutionPolicy Bypass -Scope Process -Force
iex ((New-Object System.Net.WebClient).DownloadString('https://chocolatey.org/install.ps1'))
```

Close and reopen PowerShell after that.

### Set cache directory

In PowerShell as Administrator:

```powershell
choco config set cacheLocation C:/ChocoCache
```

### Upgrade

```powershell
choco upgrade chocolatey
```

## Visual Studio 2022

This is needed for the C++ compiler. Install [Visual Studio 2022 Community Edition](https://visualstudio.microsoft.com/vs/older-downloads/).  

During installation select C++ desktop development and latest Windows 10 SDK. 

After installation open Visual Studio and check for updates. Install the latest 2022 version that is available. 

## CMake

Install via Chocolatey. In PowerShell as Administrator:

```bash
choco uninstall cmake
choco install cmake --installargs 'ADD_CMAKE_TO_PATH=System'
```

## Ninja

Install via Chocolatey. In PowerShell as Administrator:

```powershell
choco install ninja
```

## Environment

Update `$env:PSModulePath`  so that you can use PowerShell modules. 

In PowerShell as Administrator:

```powershell
[Environment]::SetEnvironmentVariable("PSModulePath", "$HOME/Documents/WindowsPowerShell/Modules;" + $env:PSModulePath, "Machine")
```

### VSSetup (Visual Studio Setup PowerShell Module)

In PowerShell as Administrator:

```powershell
Install-Module VSSetup -Scope CurrentUser
```

Test. Close and reopen PowerShell: 

```powershell
Get-VSSetupInstance `
| Select-VSSetupInstance -Version '[17.0,18.0]' `
| Select-Object -ExpandProperty InstallationPath
```

>  This finds the installation path for Visual Studio 2022.

See [Selecting an Instance](https://github.com/Microsoft/vssetup.powershell/wiki/Selecting-an-Instance)

## Python 3

This will become your system wide Python 3 installation. For projects use `venv` module (Python 3 ) to configure projects and install dependencies.

In PowerShell as Administrator:

```powershell
choco install python3 --force --params '"/InstallDir:C:\tools\python3"'
```

Install pip3 and easy_install:

```powershell
C:\tools\python3\python.exe -m ensurepip --default-pip
```

Create `python3.exe` symbolic link:

```powershell
New-Item -ItemType SymbolicLink -Path C:\tools\python3 -Name python3.exe -Target C:\tools\python3\python.exe
```

Add to PATH:

```powershell
[System.Environment]::SetEnvironmentVariable("Path", $env:Path + ";C:\tools\python3;C:\tools\python3\Scripts", "Machine")
```

Close and reopen PowerShell to refresh PATH.

## Visual Studio Code

Download and install from [Visual Studio Code](https://code.visualstudio.com/download) site.

Open Visual Studio Code and press `Ctrl + Shift + p`. Select `Shell Command: Install 'code' command in PATH`.

