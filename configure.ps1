# get script dir
$script_dir=$PSScriptRoot

# setup Visual C++ environment
# see: https://docs.microsoft.com/en-us/cpp/build/building-on-the-command-line?view=vs-2017
# see: https://docs.microsoft.com/en-us/cpp/build/building-on-the-command-line?view=vs-2019

$vs_install_dir = $(Get-VSSetupInstance | Select-VSSetupInstance -Version '[15.0,16.0]' | Select-Object -ExpandProperty InstallationPath)
$vs_common_tools = "${vs_install_dir}/Common7/Tools/"

./scripts/Invoke-CmdScript.ps1 "$vs_common_tools/VsDevCmd.bat" "-arch=amd64 -host_arch=amd64"

# create build dirs
New-Item -Force -ItemType Directory $script_dir/build/debug_x64
New-Item -Force -ItemType Directory $script_dir/build/release_x64

New-Item -Force -ItemType Directory $script_dir/build/debug_demo_x64
New-Item -Force -ItemType Directory $script_dir/build/release_demo_x64

# Python
Write-Output "Build tools ..."
Write-Output "Python ..."

# create virtual environment
C:\tools\python3\python.exe -m venv .venv

# activate virtual environment
.venv\Scripts\Activate.ps1

# upgrade pip
# python -m pip install --upgrade wheel
# python -m pip install --upgrade pip

# install development packages
pip install -r dev/requirements.txt
