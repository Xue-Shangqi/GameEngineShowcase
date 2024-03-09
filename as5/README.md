# About
This project is a very simple GUI that controls three different sound with a slider. Additionally, there is a ping button to test out the SFX when pressed.

## Extra Credit 
For the extra credit part, I attempted:
* Custom Audio
* Dark theme GUI

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
cd as5
mkdir build
cd build
```
After the folder is made, run cmake and make and finally run the code:
```
cmake ..
make
./as5
```
# Controls
The only controls for this project are the space bar and the interactive GUI. When space bar is pressed, a ping sound will play and the interactive GUI slider will adjust volume.
