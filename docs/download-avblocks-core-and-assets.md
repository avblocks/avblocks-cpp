# Download AVBlocks Core and Assets

## AVBlocks Core

In the script below, change the tag to the release that you need. For the available versions go to https://github.com/avblocks/avblocks-core/releases.   

```bash
# select version and platform
tag="v2.3.5-alpha.1"
platform="darwin"

# download
mkdir -p ./sdk
curl \
  --location \
  --output ./sdk/avblocks_$tag-$platform.zip \
  https://github.com/avblocks/avblocks-core/releases/download/$tag/avblocks_$tag-$platform.zip
  
# unzip
pushd sdk
unzip avblocks_$tag-$platform.zip
popd
```

## Assets

These demo audio and video assets are used as input by the AVBlocks samples.

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

### 