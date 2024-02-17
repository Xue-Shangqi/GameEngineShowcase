# About
The purpose of this project is the understand how to load models from different files and change their transformation properties. Additionally I added models I found online and added simple animation like spin and moving planes to make the project more interesting.

# Set up
To run the code, first you need to git clone the CS-381 repository and then update submodules. Run the commands:
```
git init
git submodule add https://github.com/Xue-Shangqi/CS-381.git
git submodule init
git submodule update --init --recursive
```

# Build
After the submodules are updated, go into the CS-381 (if you are not already in it) and as1 folder and make a build folder:
```
cd CS-381
cd as1
mkdir build
cd build
```
After the folder is made, run cmake and make and finally run the code:
```
cmake ..
make
./as1
```
# Controls
Not applicable