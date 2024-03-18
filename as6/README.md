# About
This project uses Object Oriented Entity Component Architecture to make assignment three more interesting.

## Extra Credit 
For the extra credit part, I attempted:
* Custom Model
* Howling Wind Audio

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
cd as6
mkdir build
cd build
```
After the folder is made, run cmake and make and finally run the code:
```
cmake ..
make
./as6
```
# Controls
There are basic movement controls in the game:
* W - Increases the selected plane's velocity
* S - Decreases the selected plane's velocity 
* A - Increases the selected plane's heading 
* D - Decreases the selected plane's heading 
* Tab - Changes the selected object
* Spacebar - Sets the selected objects speed to 0