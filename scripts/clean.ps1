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

echo "Running clean.ps1 ..."

$usage = '.\clean.ps1 -type [debug, release, debug_demo, release_demo] [-platform x64|arm64]'

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

echo "type = $type"
echo "platform = $platform"

$build_dir = "./build/$($type.ToLower())_$($platform.ToLower())"

if (Test-Path $build_dir) {
    pushd $build_dir
        ninja clean
        $ret = $LASTEXITCODE
    popd
} else {
    echo "Build directory does not exist: $build_dir"
    $ret = 0
}

popd
exit $ret
