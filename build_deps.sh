git submodule update --init --recursive

export script_path=`pwd`
mkdir -p $script_path/thirdparty/build 

cd $script_path/thirdparty/libuv
mkdir build
cmake -B build -DCMAKE_BUILD_TYPE=Release -DLIBUV_BUILD_SHARED=OFF
cmake --build build --config Release
cmake --install build --prefix $script_path/thirdparty/build
cd $script_path

# build usockets 
sh build_usockets.sh 