# BinaryDataConverter

A tool to convert tabular binary game data into CSV and back. 
It has been designed to be used with "Digimon Survive", but it can be configured to work with other games as well.

This is a very early version. Documentation, features and stability will be fairly lacking. ;)

# Usage

## Digimon Survive

To use the tool with Digimon Survive, first extract the DB files from `gamesystem/game/systemdata` using [AssetStudio](https://github.com/Perfare/AssetStudio) or a similar program and copy the files into the `gameFiles` folder of the tool. Make sure the file extensions (.bytes and .txt respectively) stay in tact.

Then just run the `unpackAll.bat` and it will unpack tables into the `userFiles` folder.

If you run `packAll.bat` it will reverse the process and convert all the CSV in `userFiles` back to the game format.

Alternatively you can run the tool from the command line. The basic syntax is
```
BinaryDataConverter <pathToStructureFile>
```

You can also drag & drop a structure file onto the .exe. However, be aware that all file paths are relative to the structure file in that case.

For further command line options, run `BinaryDataConverter --help`.


# Building

This project uses CMake in combination [CPM.cmake](https://github.com/cpm-cmake/CPM.cmake) for dependency management.

Building the project should be a simple

```
$ git clone git@github.com:Operation-Decoded/BinaryDataConverter.git
$ cd <project dir>
$ cmake .
$ make install```

## Important Notice
By default CPM.cmake will download all the dependencies, which includes Boost. This can take up to 3 GiB of disk space and take a while.
You can modify and optimize this behavior by configuring CPM environment variables. Please refer to their [documentation](https://github.com/cpm-cmake/CPM.cmake#Options).


# Contact
* Discord: SydMontague#8056, or in either the [Digimon Modding Community](https://discord.gg/cb5AuxU6su) or [Digimon Discord Community](https://discord.gg/0VODO3ww0zghqOCO)
* directly on GitHub
* E-Mail: sydmontague@web.de
* Reddit: [/u/Sydmontague](https://reddit.com/u/sydmontague)