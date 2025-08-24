# Download Sorter
This is a download sorter written in C++ that sorts all files in a folder into customizable subcatagories based on their file extension. It should support Firefox, Safari, and Chromium browsers, but if it doesn't work with yours, you can submit an issue and I'll get it working as soon as I can.
Only compatible with mac and linux for now, only tested on an M1 MacBook Air, but should work fine on most unix systems. 

**Windows support is in development.**

## Initial Setup

### This must go within a "DownloadSorter" folder in your home directory

If you're on an M series mac, you can download the ```sorter-mac-arm64```, ```configTEMPLATE.json```, and ```entrScriptTEMPLATE``` from the latest release. Otherwise, you will need to clone or download the repository, and build from source by running ```./build``` which will use clang to build the sorter file. This command will only work for Mac, and likely Linux, but not Windows. Windows users will need to port the app to windows, and if you do succsesfully do that, please submit a PR so this repo can be more complete. I can not make a version myself as I do not have a windows PC to test on.

Your file tree should look like this. If you're not using entr, ignore the entrScript file:
```
|- HOMEDIR
   |- DownloadSorter
      |- sorter
      |- config.json
      |- entrScript (if using entr)
```

The sorter executable expects your config to be HOMEDIR/DownloadSorter/config.json. so make sure it's either there, or you have changed the code to be a full file path, and built that.


## Config

First, rename "configTEMPLATE.json" to "config.json"

*note: git should ignore config.json to avoid publishing personal information*

Open up config.json, and now we can start setting values.

## config

In the "config" section, two values must be set. The download directory, and the miscellaneous directory. The downloads directory is wherever your browser (or other app) downloads files to. I would recommend making a "RawDownloads" folder somewhere, and making that the download location. The miscellaneous directory is where files not specified go, so if you don't have a location for a .bf file, it will go here.
It should look like this

```json
{
  "config": [
    {"Download Directory": "/PATH/TO/DOWNLOADS"},
    {"Misc Directory": "/PATH/TO/OTHER"}
  ]
}
```

## sorting

In the "sorting" section, there are some premade categories, however you may add, remove, or change as you like. This README will explain how to make one category, sinc the process is the same

Each category will have it's directory, and an array of file extensions that go there. For example, an images section:

```json
[
  // Path to this category's destination folder
  "HOMEDIR/downloads/images",
  [
  // List of file extensions to sort into this category
    "png", "jpeg", "jpg", "webp", "bmp", "gif", "heic", "tiff", "avif", "svg", "icns"
  ]
],
```

HOMEDIR is there as a replacment for my home directory, you should input the full file path.
The comma after the last "]" should only be there if there is another category after if in the file.

## Automation

The script should work fine on it's own, but if you want it to run automatically on downloads there are a few ways. The way I used and tested is with [Entr](https://github.com/eradman/entr) (Note: This only works on BSD, Mac, and Linux. Windows users will need to find another tool)

Once you download, build, and install entr, you can rename the "entrScriptTEMPLATE" to "entrScript" or whatever other name you'd like, and open it in a text editor.
Replace "/PATH/TO/DOWNLOADS" with the same raw download directory used by your browser and config. This is the directory that will scan for changes, and run the script when a change in that directory is found. Then replace "PATH/TO/sorter" with the path to the sorter executable. If you don't have this yet, go to the bottom section and build from source, then come back and enter its full file path.

Then you can set this script to run on startup of your computer.

## Building From Source

This shouldn't be too hard, clone the repository, install clang if you don't already have it, and on mac you can run ./build in the root of the repo. If you have any issues, look in the build script and main.cpp files and try and fix it. If you can't fix it, go ahead and submit an issue with your problem, and machine details. Feel free to submit a PR if you find and fix an issue too!

The clang command used to build was: ```clang++ -std=c++23 -Iinclude main.cpp -o sorter```

By default, the script will rerun the directory scanning and moving portion every 2 seconds if a file is still downloading, that way it will wait until it is done downloading, and then moving it. This can be disabled by adding the "NO_REPEAT" flag while building.
For example: ```clang++ -std=c++23 -D NO_REPEAT=1 -Iinclude main.cpp -o sorter```

## Windows Port
I am currently working on a windows port, please be patient.

Function Calls:
getenv("HOME"); (Gets the path of the user's home directory, used to find DownloadSorter in that directory
sleep(2); (From the unistd.h header, replacement exists in windows.h, as "Sleep(2);" I believe.

Im not certain if the file path system works the same on windows, so that will need testing.

# FAQ

### Q: How do I fix scripts not having permission to execute?
### A: On Mac and Linux, you can run ```chmod +x [file]``` in your terminal to give it execution permissions.
