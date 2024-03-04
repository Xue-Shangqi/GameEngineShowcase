# About
This is a very simple space shooter game that contains the basics of what a space shooter game would have. There is a title screen with the title of the game and the gameplay starts when start button is pressed. For the gameplay, you need to dodge meteors that are falling from the top of the screen and the win condition would be to get a score of 1,000. To increase player's score, you would need to destroy meteors by hitting the space bar to fire laser. If the space ship hits a meteor, you would be presented the losing screen and would need to restart. For the movement, the space ship would warp around the screen if it goes out of bound, so be careful when to warp and when not to warp. Additionally, if the left control is held down, the maximum speed of the space ship would be increase, which is useful in tight situtations.

## Extra Credit 
For the extra credit part, I attempted:
* Audio
* Non player objects
* Title Screen
* Modifier
* Could I get extra credit for making multiple screens and win condition :pleading_face::point_right::point_left:	

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
cd as4
mkdir build
cd build
```
After the folder is made, run cmake and make and finally run the code:
```
cmake ..
make
./as4
```
# Controls
There are movement controls for this project. They are WASD or the arrow keys
* W - Moves the space ship forward
* S - Moves the space ship backward 
* A - Tilts the space ship left and moves left
* D - Tilts the space ship right and moves right
* Space - Fires laser 
* Left Ctrl - Increases the space ship's max speed 
