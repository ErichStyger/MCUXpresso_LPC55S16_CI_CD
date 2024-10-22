# MCUXpresso_LPC55S16_CI_CD
Project for the LPC55S16-EVK, using VS Code with CMake and Ninja, blinking the blue onboard LED.
The project is setup for both J-Link and NXP LinkServer to run automated on-target tests.
This repository is used as an example how to do CI/CD with GitHub actions.
The setup is described in details in my blog article: https://mcuoneclipse.com/2023/10/02/ci-cd-for-embedded-with-vs-code-docker-and-github-actions/

See as well https://mcuoneclipse.com/2023/12/18/modern-on-target-embedded-system-testing-with-cmake-and-ctest/

## UART to on-board Debug Probe (VCOM)
VCOM through debug probe (e.g. with on-board LinkServer probe):
- JP9 (lower left corner, near PMod connector): *open*
- JP12 (left of JP3/UART Header): *closed*

VOM through J3 UART header (e.g. with external MCU-Link debug probe):
- JP9 *closed*
- JP12 *open*

See https://mcuoneclipse.com/2021/05/15/using-fatfs-and-minini-with-the-nxp-lpc55s16-evk/


## Environment
In order to have compiler and J-Link not hardcoded in the project itself, rund the setenv script file.

setenv example for Windows:
```
set TOOLCHAIN_PREFIX=C:/Raspy/arm-none-eabi-gcc-13.2.1-1.1
set SEGGER_PATH=C:/Program Files/SEGGER/JLink
set LINKSERVER_PATH=C:/NXP/LinkServer
```
Note that the commands have to be set up *before* starting VS Code, to have the environment set.
Alternatively, these variables can be set on user or system level.

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
- To delete a docker image:
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
- To exit the container:
```
exit
```

## Debugging with NXP LinkServer
Prior launching, run LinkServer GDB Server:
```
C:\NXP\LinkServer\LinkServer.exe gdbserver --keep-alive LPC55S16:LPCXpresso55S16
```

## J-Run Test runner
See https://www.segger.com/products/debug-probes/j-link/tools/j-run/

To run the tests with J-Run, set runner in src/platform.h:
```
#define PL_CONFIG_USE_RUNNER_JLINK        (1)
#define PL_CONFIG_USE_RUNNER_LINKSERVER   (0)
```
in src/tests/CMakeLists.txt, at the end, switch the runner part and enable the J-Link runner part.

Example to run a test directly with JRun:
```
"C:\Program Files\SEGGER\JLink\JRun.exe" --verbose --device LPC55S16 --rtt -if SWD --args "led1" "build/debug-test/LPC55S16_Blinky.elf"
```

## NXP LinkServer Runner
From LinkServer release v24.9.75 on, the tools include a LinkServer Runner, supporting both UART and semihosting.
Get help with
```
LinkServer run -h
```

Set LinkServer runner in src/platform.h:
```
#define PL_CONFIG_USE_RUNNER_JLINK        (0)
#define PL_CONFIG_USE_RUNNER_LINKSERVER   (1)
```
Inside the test CMakeLists.txt, select either semihosting or UART/VCOM connection:
```
set (RUNNER_CTEST_MODE  --mode semihost)
#set (RUNNER_CTEST_MODE  --mode serial:COM57:115200)
```
Additionally, configure it in src\platform.h:
```
#define PL_CONFIG_USE_SEMIHOSTING         (1 && PL_CONFIG_USE_RUNNER_LINKSERVER)   /* LinkServer only: using semihosting for log and test output */
#define PL_CONFIG_USE_SHELL_UART          (0 && PL_CONFIG_USE_RUNNER_LINKSERVER)   /* LinkServer only: using UART for log and test output */
```

To run the Linkserver runner manually (semihost mode):
```
C:\NXP\LinkServer\LinkServer --log-level 1 run --mode semihost lpc55s16 build\debug-test\LPC55S16_Blinky.elf
```

Semihosting example with more options:
```
C:\NXP\LinkServer\LinkServer.exe --log-level 2 run --mode semihost --exit-mark "*STOP*" --pass-mark "*** PASSED ***" --fail-mark "*** FAILED ***" --send "1" lpc55s16 "build/debug-test/LPC55S16_Blinky.elf"
```

UART example:
```
C:\NXP\LinkServer\LinkServer.exe --log-level 1 run --mode serial:COM57:115200 --exit-mark "*STOP*" --pass-mark "*** PASSED ***" --fail-mark "*** FAILED ***" --args-mark "*ARGS*" --send "Led_1" lpc55s16 "build/debug-test/LPC55S16_Blinky.elf"
```

## Unit Tests
Enable 'PL_CONFIG_USE_UNIT_TESTS' in platform.h

CTest: add the following in the main CMakeLists.txt:
```
enable_testing()
add_test(NAME ${CMAKE_PROJECT_NAME} COMMAND JRun.exe)
```
Manual test runs:
```
ctest --verbose --output-on-failure --test-dir build/debug-test --timeout 15
ctest -T test --verbose --output-on-failure -R ^Led_1$
```

## Links
- NXP LinkServer: https://www.nxp.com/design/design-center/software/development-software/mcuxpresso-software-and-tools-/linkserver-for-microcontrollers:LINKERSERVER
- How to create a GitHub action: https://github.blog/2021-11-04-10-github-actions-resources-basics-ci-cd/
- Solving 'permission denied' on config file: https://dev.to/aileenr/github-actions-fixing-the-permission-denied-error-for-shell-scripts-4gbl
- Unity testing framework: https://github.com/ThrowTheSwitch/Unity/blob/master/docs/UnityGettingStartedGuide.md
- Getting started with Docker: https://www.digikey.ch/en/maker/projects/getting-started-with-docker/aa0d4c708c274ffd975f3b427e5c0ce6
- Testing with CMake and CTest: https://mcuoneclipse.com/2023/12/18/modern-on-target-embedded-system-testing-with-cmake-and-ctest/
- CI/CD with Docker and GitHub: https://mcuoneclipse.com/2023/10/02/ci-cd-for-embedded-with-vs-code-docker-and-github-actions/
- J-Link J-Run with arguments: https://mcuoneclipse.com/2024/09/23/automated-on-target-testing-with-j-runs-args-feature/
- LinkServer Flashing with GUI: https://mcuoneclipse.com/2024/09/26/nxp-linkserver-24-9-75-new-gui-for-flash-programming/
