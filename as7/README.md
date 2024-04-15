# About
This custom game is a continuation of my scuffed game series. For this one, the rule is very simple, just dont let any fruit fall out of the map. If you do not catch all the fruit, you will lose. There are two version of the game, one being the normal difficulty and the other one is the "funny" (hard) difficulty. For the normal difficulty, the win condition is to score 100 points and for the funny difficulty, the win condition is to "survive". The background music for the normal mode is made by [pear8737](https://www.youtube.com/channel/UCuWfAq9BTpHx9Waz3R1RjNw) and [Nj0820](https://www.youtube.com/watch?v=kDZvuxlHFJA). The background music for the hard mode is made by [Casey Edwards](https://www.youtube.com/c/CaseyEdwards) and [Victor Borba](https://www.youtube.com/channel/UCgKUpNOPIIdOA0E_jeQaA-g).

## Extra Credit 
For the extra credit part, I attempted:
* Mute Button 
* Extra Library (time.h library for tracking time)
* Make a lot of things? Maybe idk

# Set up
To run the code, first you need to git clone the CS-381 repository and then update submodules. Run the commands:
```bash
git init
git submodule add https://github.com/Xue-Shangqi/CS-381.git
git submodule init
git submodule update --init --recursive
```

# Build
After the submodules are updated, go into the CS-381 (if you are not already in it) and as1 folder and make a build folder:
```bash
cd CS-381
cd as7
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
* A - Move left  
* D - Move right 