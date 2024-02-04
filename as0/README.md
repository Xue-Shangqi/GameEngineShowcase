# Set up
To run the code, first you need to git clone the CS-381 repository and then update submodules. Run the commands:
```
git init
git submodule add https://github.com/Xue-Shangqi/CS-381.git
git submodule init
git submodule update --init --recursive
```

# Build
After the submodules are updated, go into the CS-381 (if you are not already in it) and as0 folder and make a build folder:
```
cd CS-381
cd as0
mkdir build
cd build
```
After the folder is made, run cmake and make and finally run the code:
```
cmake ..
make
./as0
```