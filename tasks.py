# see: http://docs.pyinvoke.org/en/stable/
from invoke import task

import platform

os = platform.system()

@task
def build(c, type="debug"):
    """Builds the library and the tests
    """
    if "Darwin" == os:     
        c.run(
            f"./scripts/build.sh --type {type}", 
            pty=True
        )
    elif "Linux" == os:     
        c.run(
            f"./scripts/build.sh --type {type}", 
            pty=True
        )
    elif "Windows" == os:
        c.run(
            f"powershell -ExecutionPolicy ByPass \
            ./scripts/build.ps1 -type {type}"
        )
    else:
        print("Unsupported platform!")


@task
def clean(c, type="debug"):
    """Build cleanup
    """
    if "Darwin" == os:     
        c.run(
            f"./scripts/clean.sh --type {type}", 
            pty=True
        )
    elif "Linux" == os:     
        c.run(
            f"./scripts/clean.sh --type {type}", 
            pty=True
        )
    elif "Windows" == os:
        c.run(
            f"powershell -ExecutionPolicy ByPass \
            ./scripts/clean.ps1 -type {type}"
        )
    else:
        print("Unsupported platform!")    

@task
def test(c, type="debug", filter=""):
    """Runs all tests. Use the filter to specify individual tests.
    """
    if "Darwin" == os:     
        c.run(
            f"./scripts/test.sh --type {type} --filter '{filter}'", 
            pty=True
        )
    elif "Linux" == os:     
        c.run(
            f"./scripts/test.sh --type {type} --filter '{filter}'", 
            pty=True
        )
    elif "Windows" == os:
        c.run(
            f"powershell -ExecutionPolicy ByPass \
            ./scripts/test.ps1 -type {type} -filter '{filter}'"
        )
    else:
        print("Unsupported platform!")

@task
def watch(c, type="debug", filter=""):
    """Runs all tests and watches for changes. Use the filter to specify individual tests.
    """
    if "Darwin" == os:     
        c.run(
            f"./scripts/watch.sh --type {type} --filter '{filter}'", 
            pty=True
        )
    elif "Linux" == os:     
        c.run(
            f"./scripts/watch.sh --type {type} --filter '{filter}'", 
            pty=True
        )
    elif "Windows" == os:
        if 0 == len(filter):
            filter = '*'
            
        c.run(
            f"powershell -ExecutionPolicy ByPass \
            ./scripts/watch.ps1 -type {type} -filter '{filter}'"
        )
    else:
        print("Unsupported platform!")
