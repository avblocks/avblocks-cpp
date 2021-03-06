param(
    # build type
    # one of: "debug", "release"
    [String]$type=""
)

# switch to parent directory	
pushd "$PSScriptRoot/.."

echo "Running build.ps1 ..."

if (! $type) {
    echo "Usage:"    
    echo '.\build.ps1 -type [debug, release, debug_demo, release_demo]'
    popd; exit 1
}

if (! @("debug", "release", "debug_demo", "release_demo").Contains($type.ToLower())) {
    echo "Usage:"    
    echo '.\build.ps1 -type [debug, release, debug_demo, release_demo]'
    popd; exit 1
}

echo "type = $type"

if ($type -ieq "debug") {
    New-Item -Force -Path ./build/debug_x64 -ItemType Directory 
    pushd ./build/debug_x64
        cmake -G 'Ninja' -DCMAKE_BUILD_TYPE=debug -DPLATFORM=x64 ../..
        ninja
    popd  
}

if ($type -ieq "debug_demo") {
    New-Item -Force -Path ./build/debug_demo_x64 -ItemType Directory 
    pushd ./build/debug_demo_x64
        cmake -G 'Ninja' -DCMAKE_BUILD_TYPE=debug -DPLATFORM=x64 -DDEMO=YES ../..
        ninja
    popd  
}

if ($type -ieq "release") {
    New-Item -Force -Path ./build/release_x64 -ItemType Directory 
    pushd ./build/release_x64
        cmake -G 'Ninja' -DCMAKE_BUILD_TYPE=release -DPLATFORM=x64 ../..
        ninja
    popd  
}

if ($type -ieq "release_demo") {
    New-Item -Force -Path ./build/release_demo_x64 -ItemType Directory 
    pushd ./build/release_demo_x64
        cmake -G 'Ninja' -DCMAKE_BUILD_TYPE=release -DPLATFORM=x64 -DDEMO=YES ../..
        ninja
    popd  
}

popd