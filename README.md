# MCUXpresso_LPC55S16_CI_CD
Project for the LPC55S16-EVK, using VS Code with CMake and Ninja, blinking the blue onboard LED.
This repository is used as an example how to do CI/CD with GitHub actions.

## Environment
The root folder contains several batch files as helper.
In order to have compiler and J-Link not hardcoded in the project itself, rund the setenv script file.

setenv example for Windows:
```
set TOOLCHAIN_PREFIX=C:/Raspy/arm-none-eabi-gcc-12.2.1-1.2
set SEGGER_PATH=C:/Program Files/SEGGER/JLink
```
Note that the commands have to be set up *before* starting VS Code, to have the environment set.
Alternatively, these variables can be set on user level.

## CMake commands
There are a set of CMake and build related commands you can use:

init:
```
cmake -G"Ninja" . -B build
```

build:
```
cmake --build build
```

clean:
```
cmake --build build --target clean
```

CleanClean:
```
rmdir /S /Q build 2>nul
```

## Links
- How to create a GitHub action: https://www.incredibuild.com/blog/using-github-actions-with-your-c-project https://github.blog/2021-11-04-10-github-actions-resources-basics-ci-cd/
- Solving 'permission denied' on config file: \href{https://dev.to/aileenr/github-actions-fixing-the-permission-denied-error-for-shell-scripts-4gbl}{permission denied}
- Unity testing framework: https://github.com/ThrowTheSwitch/Unity/blob/master/docs/UnityGettingStartedGuide.md
- Getting started with Docker: https://www.digikey.ch/en/maker/projects/getting-started-with-docker/aa0d4c708c274ffd975f3b427e5c0ce6

