Lightship
=========

An online multiplayer arcade game.

Building
--------

The recommended directory structure to use is as follows:

```
lightship
├── lightship-build
├── lightship-git
├── urho3d-build
├── urho3d-git
└── urho3d-install
```

Create the project directory and cd into it:
```sh
mkdir lightship
cd lightship
```

Clone Urho3D into the folder ```urho3d-git```
```sh
git clone https://github.com/urho3d/urho3d urho3d-git
```

Compile and install Urho3D with the following settings:
```sh
mkdir urho3d-build
cd urho3d-build
cmake -DCMAKE_INSTALL_PREFIX=$(pwd)/../urho3d-install -DURHO3D_SAMPLES=OFF -DURHO3D_LIB_TYPE=SHARED ../urho3d-git
make -j32
make install
```

Next, clone lightship into ```lightship-git```
```sh
git clone https://github.com/thecomet/lightship lightship-git
```

Need to fetch the game assets, which are added as a submodule:
```sh
cd lightship-git
git submodule init
git submodule update
cd ..
```

Create the build directory and compile:
```sh
mkdir lightship-build
cd lightship-build
cmake -DURHO3D_HOME=$(pwd)../urho3d-install ../lightship-git
make -j32
```

Running
-------

If everything goes smoothly, there will be a number of executables placed in ```lightship-build/bin```. The server can be started with:
```sh
./bin/lightship-server
```

The client can be started with:
```sh
./bin/lightship-client
```

