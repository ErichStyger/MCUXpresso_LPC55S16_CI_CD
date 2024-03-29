# MCUXpresso_LPC55S16_CI_CD
Project for the LPC55S16-EVK, using VS Code with CMake and Ninja, blinking the blue onboard LED.
This repository is used as an example how to do CI/CD with GitHub actions.
The setup is described in details in my blog article: https://mcuoneclipse.com/2023/10/02/ci-cd-for-embedded-with-vs-code-docker-and-github-actions/

## Environment
In order to have compiler and J-Link not hardcoded in the project itself, rund the setenv script file.

setenv example for Windows:
```
set TOOLCHAIN_PREFIX=C:/Raspy/arm-none-eabi-gcc-13.2.1-1.1
set SEGGER_PATH=C:/Program Files/SEGGER/JLink
```
Note that the commands have to be set up *before* starting VS Code, to have the environment set.
Alternatively, these variables can be set on user level.

## Build
To initialize the build files, use
```
cmake --preset debug
```
To build it:
```
cmake --build --preset debug
```

## Docker Quick-Steps
- Installed Docker Desktop on Windows
- Test it first with a local docker image
- 'cd' to the Dockerfile
- Build docker image with the instructions in the Dockerfile (-t tags the image)
```
docker build -t lpc55s16-image .
```
- To view the actual images:
```
docker images
```
- To deleta docker image:
```
docker rmi <image>
```

- To test the image, create a container from the Image (-i for interactive mode, -t to add a pseudo terminal for interaction with the container, --entrypoint so we can log into the container)
```
docker create -i -t --name lpc55s16-container lpc55s16-image
```
- To list the container:
```
docker container ls -a
```
- To remove a container:
```
docker rm <container>
```
- To copy a file into the container:
```
docker cp <file> <container>:<pathInContainer>
```
- Start the container in interactive mode -i:
```
docker start -i lpc55s16-container
```
- To exit the container
```
exit
```
## J-Run
See https://www.segger.com/products/debug-probes/j-link/tools/j-run/

## Unit Tests
Enable 'PL_CONFIG_USE_UNIT_TESTS' in platform.h

Run JRun manually:
```
JRun --device LPC55S16 build/debug-test/LPC55S16_Blinky.elf
```
CTest: add the following in the main CMakeLists.txt:
```
enable_testing()
add_test(NAME ${CMAKE_PROJECT_NAME} COMMAND JRun.exe)
```
Manual test run:
```
ctest --test-dir build/debug-test --timeout 120
```

## Links
- How to create a GitHub action: https://github.blog/2021-11-04-10-github-actions-resources-basics-ci-cd/
- Solving 'permission denied' on config file: https://dev.to/aileenr/github-actions-fixing-the-permission-denied-error-for-shell-scripts-4gbl
- Unity testing framework: https://github.com/ThrowTheSwitch/Unity/blob/master/docs/UnityGettingStartedGuide.md
- Getting started with Docker: https://www.digikey.ch/en/maker/projects/getting-started-with-docker/aa0d4c708c274ffd975f3b427e5c0ce6

