# About
This game is a DOECS implementation of ships and planes. It is similar to how AS6 works but the architecture used is DOECS. There are 5 ships and 5 planes each with different rotation speed, max speed, acceleration, etc. all picked at random when the data component is initialized.

## Extra Credit 
* None 

# Set up
To run the code, first you need to git clone the CS-381 repository and then update submodules. Run the commands:
```bash
git init
git submodule add https://github.com/Xue-Shangqi/CS-381.git
git submodule init
git submodule update --init --recursive
```

# Build
After the submodules are updated, go into the CS-381 (if you are not already in it) and as8 folder and make a build folder:
```bash
cd CS-381
cd as8
mkdir build
cd build
```
After the folder is made, run cmake and make and finally run the code:
```bash
cmake ..
make
./as7
```
# Controls
There are basic movement controls in the game:
* W - Increase selected entity's speed 
* S - Decrease selected entity's speed 
* A - Increase selected entity's heading/yaw
* D - Decrease selected entity's heading/yaw
* Q - Increase selected entity's pitch (plane only)
* E - Decrease selected entity's pitch (plane only)
* R - Increase selected entity's roll (plane only)
* F - Decrease selected entity's roll (plane only)
* Space Bar - Set speed to 0
* Tab - Switch selected entity