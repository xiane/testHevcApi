# How to use testHevcApi

## Build the binary and libvphevcodec.so library.

```
git clone https://github.com/xiane/testHevcApi
cd testHevcApi
git submodule init
git submodule update
make
```

## Apply library path.

Add libvphevcodec.so path by belove command, or install it.

### export library path
```
(go to testHevcApi path)
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${PWD}/hevc_enc/
```

### install the library
```
(go to testHevcApi path)
cd hevc_enc
sudo make install
```

### How to use it.

```
Amlogic HEVC Encode API
 usage: output [srcfile][outfile][width][height][gop][framerate][bitrate][num]
  options  :
  srcfile  : yuv data url in your root fs
  outfile  : stream url in your root fs
  width    : width
  height   : height
  gop      : I frame refresh interval
  framerate: framerate
   bitrate  : bit rate
   num      : encode frame count
   fmt      : encode input fmt 1:nv12, 2:nv21
```
