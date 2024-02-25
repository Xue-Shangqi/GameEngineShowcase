# About
This purpose of this project is to create a raylib program that allows you to control different planes and have the other plane continue their movement. The project also make it so turning is not instantly and gradually approch the targeted angle/speed.

## Extra Credit 
For the extra credit part, I attempted:
* No IsKeyPressed for plane movement
* Custom mesh off the internet
* Camera movement
* Fly movement
* Engine noise

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
cd as3
mkdir build
cd build
```
After the folder is made, run cmake and make and finally run the code:
```
cmake ..
make
./as3
```
# Controls
There are movement controls for this project. They are WASD or the arrow keys
* W - Increases the selected plane's velocity
* S - Decreases the selected plane's velocity 
* A - Increases the selected plane's heading 
* D - Decreases the selected plane's heading 
* Q - Ascend the selected plane
* E - Descend the selected plane
