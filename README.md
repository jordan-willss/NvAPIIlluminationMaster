﻿# NvAPIIlluminationMaster

> **Warning** <br>
> If you do not know C/C++, please do NOT attempt to use this project. It is incredibly easy to "fuck around and find out", and brick your GPU by exploring register addresses of GPUs. Just don't risk it, wait a bit and it'll be ready in a releasable state :)
> 
> I (Jordan Wills) as an individual will not be held liable for any damages or loss you incur by using any of the code in this repository. Any comments or claims made either in this repository or elsewhere pertaining to the effectiveness, usability, or stability of the contents of this repository shall not officially apply and is not actionable in any event. You use this repository at your own risk.

> **Note** <br>
> This repository is *at best* an SFP (shitty first prototype) for a Windows service to control GPU illumination using the NvAPI protocols, rather than i2c. It "works" for me, but really this requires substantial amounts of work still and a better external integration before being properly released into the world.

<br>

This is a project that allows users to control the RGB illumination of certain NVIDIA GPUs by creating a Windows service that the user can directly communicate with.

## To-do list

* Find a better way to communicate with the service
* Set up a way to specify RGB values to set
* Add a check to only accept the correct illumination-setting function based on the zone type
* Add an "uninstall service" thing
* Create an ACTUAL install/uninstall
* Add a check to see whether the NvAPI protocol we're using will even work on the device
* Conquer the world

## What are the projects/solutions? (why are there three?)

* NVAPIIlluminationControl - This is the actual functionality, but this solution runs as a console application to allow for debugging of the NvAPI usage
* NVAPIIlluminationControlLibrary - This is a shell solution that simply imports the main.cpp from the *NVAPIIlluminationControl* project, then, it is built into a *.dll* and *.lib* file for the *NVAPIIlluminationControlService* project to use
* NVAPIIlluminationControlService - This project is used to build a *.exe* Window service which integrates directly with Windows and then, allows the user to communicate to the service to control the GPU illumination

<br>

## How do I get setup? (what is all this junk)

1. In your C: drive, create a folder called "NvAPIIlluminationController" (C:/NvAPIIlluminationController)

2. Copy the file and folder from ~/NVAPIIlluminationMaster to your newly created folder from step 1

3. After running a build of the *NVAPIIlluminationControlService* project successfully, a *.exe* file will be generated in ~/NVAPIIlluminationMaster/NVAPIIlluminationControlService/x64/Release. Alongside this file, you'll also find the *NVAPIIlluminationControlLibrary.dll*. Copy both of these to the folder from step 1

4. Open a terminal, and run the following command: "C:/NvAPIIlluminationController/NVAPIIlluminationControlService.exe install"

5. Open *Services* (the Windows application), scroll to "NvAPIIlluminationController", and start the service

> **Note** <br>
> I got a little tired of copy/pasting the *.exe* and *.dll* so I made this .bat (Windows batch file) script to do it for me:
> ```
> @echo off
> cd /d "%~dp0"
> 
> set "executable=NVAPIIlluminationControlService.exe"
> set "library=NVAPIIlluminationControlLibrary.dll"
> set "destinationDirectory=C:\NvAPIIlluminationController"
> copy "%executable%" "%destinationDirectory%"
> copy "%library%" "%destinationDirectory%"
> 
> start "" "NVAPIIlluminationControlService.exe" install
> ```
> This will ONLY work if it's in the same folder as where the *NVAPIIlluminationControlService* project is building to. But, it might save you a headache or two

<br>

## Why are my runs/builds failing? (help! it's all broken!)

As mentioned in the previous section, each solution requires something from the last, which is very likely the main reason for the projects not to run/build. Here is a list of what is required to be added as an "Existing Item" to each project:

* main.cpp (NVAPIIlluminationControl) -> NVAPIIlluminationControlLibrary
* NVAPIIlluminationControlLibrary.dll (NVAPIIlluminationControlLibrary) -> NVAPIIlluminationControlService
* NVAPIIlluminationControlLibrary.lib (NVAPIIlluminationControlLibrary) -> NVAPIIlluminationControlService

> **Note** <br>
> You should only use "Release" files with "Release" projects and "Debug" with "Debug"
