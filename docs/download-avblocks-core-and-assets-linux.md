# Download AVBlocks Core and Assets on Linux

Change to the directory where you cloned this repository:

```bash
cd avblocks-cpp
```

## AVBlocks Core

In the script below, change the tag to the release that you need. For the available versions check the [AVBlocks Core](https://github.com/avblocks/avblocks-core/releases) releases.   

```bash
# select version and platform
tag="v3.0.0-demo.1"
platform="linux"

# download
mkdir -p ./sdk
curl \
  --location \
  --output ./sdk/avblocks_$tag-$platform.tar.gz \
  https://github.com/avblocks/avblocks-core/releases/download/$tag/avblocks_$tag-$platform.tar.gz
  
# unzip
pushd sdk
tar -xvf avblocks_$tag-$platform.tar.gz
popd
```

## Assets

These demo audio and video assets are used as input for the AVBlocks samples.

```bash
mkdir -p ./assets
curl \
  --location \
  --output ./assets/avblocks_assets_v1.zip \
  https://github.com/avblocks/avblocks-assets/releases/download/v1/avblocks_assets_v1.zip
  
# unzip
pushd assets
unzip avblocks_assets_v1.zip
popd
```

