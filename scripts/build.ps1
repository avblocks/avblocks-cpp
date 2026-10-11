param(
    # build type
    # one of: "debug", "release", "debug_demo", "release_demo"
    [String]$type="",

    # platform
    # one of: "x64", "arm64". Defaults to the host architecture.
    [String]$platform=""
)

# switch to parent directory
pushd "$PSScriptRoot/.."

echo "Running build.ps1 ..."

$usage = '.\build.ps1 -type [debug, release, debug_demo, release_demo] [-platform x64|arm64]'

if (! $type) {
    echo "Usage:"
    echo $usage
    popd; exit 1
}

if (! @("debug", "release", "debug_demo", "release_demo").Contains($type.ToLower())) {
    echo "Usage:"
    echo $usage
    popd; exit 1
}

if (! $platform) {
    # PROCESSOR_ARCHITEW6432 is set when PowerShell runs emulated (e.g. x64 PowerShell on ARM64).
    $host_arch = if ($env:PROCESSOR_ARCHITEW6432) { $env:PROCESSOR_ARCHITEW6432 } else { $env:PROCESSOR_ARCHITECTURE }
    $platform = if ($host_arch -eq "ARM64") { "arm64" } else { "x64" }
}

if (! @("x64", "arm64").Contains($platform.ToLower())) {
    echo "Usage:"
    echo $usage
    popd; exit 1
}

$type = $type.ToLower()
$platform = $platform.ToLower()

echo "type = $type"
echo "platform = $platform"

$build_type = if ($type.StartsWith("debug")) { "debug" } else { "release" }
$demo_flag = if ($type.EndsWith("_demo")) { "-DDEMO=YES" } else { "" }

$build_dir = "./build/${type}_${platform}"

New-Item -Force -Path $build_dir -ItemType Directory
pushd $build_dir
    cmake -G 'Ninja' -DCMAKE_BUILD_TYPE=$build_type -DPLATFORM=$platform $demo_flag ../..
    if ($LASTEXITCODE -eq 0) { ninja }
    $ret = $LASTEXITCODE
popd

popd
exit $ret
