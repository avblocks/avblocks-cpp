# get script dir
$script_dir=$PSScriptRoot

# create build dirs
New-Item -Force -ItemType Directory $script_dir/build/debug_x64
New-Item -Force -ItemType Directory $script_dir/build/release_x64

New-Item -Force -ItemType Directory $script_dir/build/debug_demo_x64
New-Item -Force -ItemType Directory $script_dir/build/release_demo_x64

# setup Visual C++ environment
# see: https://docs.microsoft.com/en-us/cpp/build/building-on-the-command-line?view=vs-2022

$vs_install_dir = $(Get-VSSetupInstance | Select-VSSetupInstance -Version '[17.0,18.0]' | Select-Object -ExpandProperty InstallationPath)
$vs_common_tools = "${vs_install_dir}/Common7/Tools/"

./scripts/Invoke-CmdScript.ps1 "$vs_common_tools/VsDevCmd.bat" "-arch=amd64 -host_arch=amd64"

# Python
Write-Output "Python ..."

# create virtual environment
C:\tools\python3\python.exe -m venv .venv

# activate virtual environment
.venv\Scripts\Activate.ps1

# install development packages
pip install -r dev/requirements.txt
