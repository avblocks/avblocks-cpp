# Setup for Linux

> Scripts are `bash`

## Ubuntu

### Compilers

```bash
sudo apt install build-essential
```

### Libraries

These libraries are required by AVBlocks:

#### Ubuntu 24.04 / 22.04 / 20.04

```bash
sudo apt-get install libjpeg8 libtiff5 libpng16-16 libtbb2
```

#### Ubuntu 18.04 / 16.04

```bash
sudo apt-get install libjpeg8 libtiff5 libpng12-0 libtbb2
```

### CMake

#### Manual Installation (preferred)

Remove `cmake` that comes with Ubuntu 20.04 or 18.04:

```bash
sudo apt remove cmake
sudo apt purge --auto-remove cmake
```

Install dependencies:

```bash
sudo apt install libssl-dev
```

Download `cmake 3.19`:

```bash
version=3.19
build=1

mkdir -p ~/temp
cd ~/temp

wget https://cmake.org/files/v$version/cmake-$version.$build.tar.gz
tar -xzvf cmake-$version.$build.tar.gz
```

Build and install the extracted source by running:

```bash
cd ~/temp/cmake-$version.$build/

./bootstrap
make -j 4

sudo make install
```

To verify close and reopen Terminal:

```bash
cmake --version
```

### ninja

```bash
sudo apt install ninja-build
```

### pyenv

Install Python Dependencies:

```bash
sudo apt-get install libreadline-gplv2-dev libncursesw5-dev libssl-dev libsqlite3-dev tk-dev libgdbm-dev libc6-dev libbz2-dev
```

Install `pyenv`:

```bash
git clone https://github.com/pyenv/pyenv.git ~/.pyenv
echo 'export PYENV_ROOT="$HOME/.pyenv"' >> ~/.bashrc
echo 'export PATH="$PYENV_ROOT/bin:$PATH"' >> ~/.bashrc
```

Enable:

```bash
source ~/.bashrc
```

### Visual Studio Code

Install via Snap Store:

```bash
sudo snap install --classic code
```
