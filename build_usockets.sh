git submodule update --init --recursive

export script_path=`pwd`
mkdir -p $script_path/thirdparty/build 

cd $script_path/thirdparty/uWebSockets/uSockets
LDFLAGS="-L$script_path/thirdparty/build/lib" CFLAGS="-I$script_path/thirdparty/build/include" WITH_LIBUV=1 make
cd $script_path
