# MCUXpresso_LPC55S16_CI_CD
Project for the LPC55S16-EVK, using VS Code with CMake and Ninja, blinking the blue onboard LED.
The project is setup for both J-Link and NXP LinkServer to run automated on-target tests.
This repository is used as an example how to do CI/CD with GitHub actions.
The setup is described in details in my blog article: https://mcuoneclipse.com/2023/10/02/ci-cd-for-embedded-with-vs-code-docker-and-github-actions/

The repository of this project is available on GitHub here: https://github.com/ErichStyger/MCUXpresso_LPC55S16_CI_CD

See following articles for details:
- https://mcuoneclipse.com/2023/12/18/modern-on-target-embedded-system-testing-with-cmake-and-ctest/
- https://mcuoneclipse.com/2025/02/08/optimizing-embedded-development-with-vs-code-and-devcontainer/
- https://mcuoneclipse.com/2025/02/18/remote-debugging-with-devcontainer-and-vs-code/
- https://mcuoneclipse.com/2025/08/11/streamlining-linkserver-installation-for-ci-cd/

## Cloning the repository
This repository is using the McuLib repository as sub-repo. 

Cloning the repo with submodules:
```
git clone --recurse-submodules git@github.com:ErichStyger/MCUXpresso_LPC55S16_CI_CD.git
```
If missed, update it with:
```
git submodule update --init --recursive
```

## UART to on-board Debug Probe (VCOM)
VCOM through debug probe (e.g. with on-board LinkServer probe):
- JP9 (lower left corner, near PMod connector): *open*
- JP12 (left of JP3/UART Header): *closed*

VOM through J3 UART header (e.g. with external MCU-Link debug probe):
- JP9 *closed*
- JP12 *open*

See https://mcuoneclipse.com/2021/05/15/using-fatfs-and-minini-with-the-nxp-lpc55s16-evk/

## Build
To initialize the build files, use
```
cmake --preset Debug
```
To build it:
```
cmake --build --preset app-debug
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
LinkServer gdbserver --keep-alive LPC55S16:LPCXpresso55S16
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
JRun --verbose --device LPC55S16 --rtt -if SWD --args "led1" "build/debug-test/LPC55S16_Blinky.elf"
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
Inside the test CMakeLists.txt, select either semihosting or UART/VCOM connection with corresponding COM port on your machine:
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
LinkServer --log-level 1 run --mode semihost lpc55s16 --send "led1" build\Test\LPC55S16_Blinky.elf
```

Semihosting example with more options:
```
LinkServer --log-level 2 run --mode semihost --exit-mark "*STOP*" --pass-mark "*** PASSED ***" --fail-mark "*** FAILED ***" --send "led1" lpc55s16 "build/Test/LPC55S16_Blinky.elf"
```

UART example:
```
LinkServer --log-level 1 run --mode serial:COM57:115200 --exit-mark "*STOP*" --pass-mark "*** PASSED ***" --fail-mark "*** FAILED ***" --args-mark "*ARGS*" --send "led1" lpc55s16 "build/Test/LPC55S16_Blinky.elf"
```

## Unit Tests
Enable 'PL_CONFIG_USE_UNIT_TESTS' in platform.h

Manual test runs:
```
ctest --verbose --output-on-failure --test-dir build/Test --timeout 15 -R Led_1
```

## Notes
[ctest] No coverage info files for CMake project C:/Users/Erich Styger.N0007139/Data/GitRepos/GitHub_MCUXpresso_LPC55S16_CICD/MCUXpresso_LPC55S16_CI_CD. No coverage data will be analyzed for this project.
https://gcovr.com/en/stable/output/lcov.html
gcovr --lcov build/Test/coverage.info

settings.json:
    "cmake.coverageInfoFiles": [
        "${workspaceFolder}/build/coverage.info"
    ],  

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
