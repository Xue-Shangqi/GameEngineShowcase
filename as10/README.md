# About
The project is a very simple game that is multiplayer where the value goes up  or down depending on which button the player press. First one player needs to host the game for the game to work correctly.

## Extra Credit 
None

# Set up
The better and faster way to run the game is to just download and import it straight into godot since it doesn't require you to build and make the game. To import the game, first drag and drop into res:// folder the follow files:
* Networking.gd
* RemoteScript.gd
* World.tscn
* Player.tscn
and then go into Godot engine and select Project -> Project Settings... -> Run -> Set the main scene to World.tscn


However if you really want to build and make the game, here are the commands:
To run the code, first you need to git clone the CS-381 repository and then update submodules. Run the commands:
```bash
git init
git submodule add https://github.com/Xue-Shangqi/CS-381.git
git submodule init
git submodule update --init --recursive
```
After the submodules are updated, go into the CS-381 (if you are not already in it) and Godot folder and make a build folder:
```bash
cd CS-381
cd as10
mkdir build
cd build
```
After the folder is made, run cmake and make and finally run the code:
```bash
cmake ..
make
./as10
```
# Controls
The only control is the incremenet and decrement button. There is also the host and join button but that is only for joining or setting up server 