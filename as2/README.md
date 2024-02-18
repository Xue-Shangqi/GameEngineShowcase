# About
The purpose of this project to is understand how to create a skybox and ground. In addition to that, the project is also aimed towards creating basic physics so that the plane (player) can move. 

## Question 7 - Why not simplify plane movement with camera remapping?
Although remapping the camera might seem like a simply solution, it is not used in this project because the motion of the airplane needs to correspond to the camera's perspective in a 3D space. Additionally, remapping the camera direction might not accurately reflect the intended motion from the camera's perspective. Lastly, it is easy to tell if the plane is moving or the camera is moving based on the skybox and the ground. If the camera is moving, then the plane would be at the same place relative to the skybox and ground.

## Extra Credit 
For the extra credot part, I attempted:
    * No IsKeyPressed for plane movement
    * Custom mesh off the internet
    * Arrow keys for control
    * Camera movement
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
cd as2
mkdir build
cd build
```
After the folder is made, run cmake and make and finally run the code:
```
cmake ..
make
./as2
```
# Controls
There are movement controls for this project. They are WASD or the arrow keys
    * W and Up arrow - Moves the plane away from the camera
    * S and Down arrow - Moves the plane towards the camera
    * A and Left arrow - Moves the plane left 
    * D and Right arrow - Moves the plane right
    * Q - Ascend the plane
    * E - Descend the plane
