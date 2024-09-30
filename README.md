# MCUXpresso_LPC55S16_CI_CD
Project for the LPC55S16-EVK, using VS Code with CMake and Ninja, blinking the blue onboard LED.
This repository is used as an example how to do CI/CD with GitHub actions.
The setup is described in details in my blog article: https://mcuoneclipse.com/2023/10/02/ci-cd-for-embedded-with-vs-code-docker-and-github-actions/
See as well https://mcuoneclipse.com/2023/12/18/modern-on-target-embedded-system-testing-with-cmake-and-ctest/

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

# NXP LinkServer Debug
Prior launching, run LinkServer GDB Server:
```
c:\nxp\LinkServer_24.9.75\LinkServer gdbserver --keep-alive LPC55S16:LPCXpresso55S16
```

# NXP LinkServer Runner
Set Semihosting to LinkServer in IncludeMcuLibConfig.h:
```
#define McuSemihost_CONFIG_DEBUG_CONNECTION         McuSemihost_DEBUG_CONNECTION_LINKSERVER
```
```
c:\nxp\LinkServer_24.9.75\LinkServer --log-level 1 run --mode semihost lpc55s16 build\debug-test\LPC55S16_Blinky.elf


C:\NXP\LinkServer_24.9.75\LinkServer.exe "--log-level" "2" "run" "--mode" "semihost" "--exit-mark" "** PASSED **" "lpc55s16" "C:/Users/Erich Styger.N0007139/Data/GitRepos/MCUXpresso_LPC55S16_CI_CD/C:\NXP\LinkServer_24.9.75\LinkServer.exe --log-level 2 run --mode semihost --exit-mark "** PASSED **" lpc55s16 C:/Users/Erich Styger.N0007139/Data/GitRepos/MCUXpresso_LPC55S16_CI_CD/build/debug-test/LPC55S16_Blinky.elfbuild/debug-test/LPC55S16_Blinky.elf"
```

C:\NXP\LinkServer_24.9.75\LinkServer.exe "--log-level" "2" "run" "--mode" "semihost" "--exit-mark" "*STOP*" "--pass-mark" "*** PASSED ***" "--fail-mark" "*** FAILED ***" "--send" "Led_1" "lpc55s16" "C:/Users/Erich Styger.N0007139/Data/GitRepos/MCUXpresso_LPC55S16_CI_CD/build/debug-test/LPC55S16_Blinky.elf"

Works:
C:\NXP\LinkServer_24.9.75\LinkServer.exe "--log-level" "2" "run" "--mode" "semihost" "--send" "Led_2" "lpc55s16" "C:/Users/Erich Styger.N0007139/Data/GitRepos/MCUXpresso_LPC55S16_CI_CD/build/debug-test/LPC55S16_Blinky.elf"


C:\NXP\LinkServer_24.9.75\LinkServer.exe --log-level 2 run --mode semihost --exit-mark "*STOP*" --pass-mark "*** PASSED ***" --fail-mark "*** FAILED ***" --send "Led_x" lpc55s16 "build/debug-test/LPC55S16_Blinky.elf"

echo %errorlevel%  

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
ctest --verbose --output-on-failure --test-dir build/debug-test --timeout 15

ctest -T test --verbose --output-on-failure -R ^Led_1$
```

## Links
- How to create a GitHub action: https://github.blog/2021-11-04-10-github-actions-resources-basics-ci-cd/
- Solving 'permission denied' on config file: https://dev.to/aileenr/github-actions-fixing-the-permission-denied-error-for-shell-scripts-4gbl
- Unity testing framework: https://github.com/ThrowTheSwitch/Unity/blob/master/docs/UnityGettingStartedGuide.md
- Getting started with Docker: https://www.digikey.ch/en/maker/projects/getting-started-with-docker/aa0d4c708c274ffd975f3b427e5c0ce6
- Blog: https://mcuoneclipse.com/2023/12/18/modern-on-target-embedded-system-testing-with-cmake-and-ctest/
- Blog: https://mcuoneclipse.com/2023/10/02/ci-cd-for-embedded-with-vs-code-docker-and-github-actions/
