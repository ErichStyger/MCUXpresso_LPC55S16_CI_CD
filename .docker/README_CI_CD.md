# CI/CD with Docker
Project to build a CI/CD pipeline for an embedded device with docker.
To run it with docker, copy it to the project root first.

## Docker
- run docker daemon (or Docker Desktop on Windows), if not already running
- on the host, open a terminal and `cd` into the project root folder
- create docker image:
```
docker build -t cicd-image .
```
- create container from image:
```
docker create -i -t --name cicd-container cicd-image
```
- start container in interactive mode:
```
docker start -i cicd-container
```

## J-Link Debug Probe Serial Number
- Note: depending on network latency, probe speed and characteristics, tunneling might be unstable.
- Identify serial number of SEGGER J-Link debug probe with `JLinkConfig` or `JLinkGDBServerCL`.
- use serial number in VS Code launch.json:
```
"serialNumber": "801048148",
```
- for a tunnel connection:
```
"ipAddress": "tunnel:801048148"
```
- in the docker image, the J-Link files are here:
```
/opt/SEGGER/JLink/
```
- Settings for the J-Link connection serial number: check the `CMakeLists.txt` inside the `src/tests` folder.

## Run Tests with JRun inside Docker Container
We need to use a tunneling service (IP connection) to access the hardware outside docker.
Change the serial number `801048205` below to the one of your J-Link debug probe.
- Create a tunnel connection on the host:
```
JLinkRemoteServerCL -UseTunnel -TunnelBySN -select="801048148"
```
- Start the docker container in interactive mode:
```
docker start -i cicd-container
```
- change director to the project:
```
cd /project
```
- run a test with JRun:
```
JRun --verbose --device lpc55s16 --rtt -if SWD --ip "tunnel:801048148" --pc off --sp off --args "Led_1" build/Test/LPC55S16_Blinky.elf
```
- run a single test with CTest:
```
ctest --verbose --extra-verbose --test-dir build/Test -R Led_1
```
- to run all tests with CTest:
```
ctest --verbose --extra-verbose --test-dir build/Test
```
- Inspect the files in the container, then use 'exit'
- Check .github/workflows folder which defines GitHub actions

## Trigger Action with Commit
Trigger the action with a tag push:
```
git tag v0.0.1
git push origin v0.0.1
```
## Links
- https://wiki.segger.com/J-Link_Docker_Container
- https://mcuoneclipse.com/2017/02/05/remote-board-debugging-j-link-remote-server-with-eclipse/
- https://mcuoneclipse.com/2023/10/02/ci-cd-for-embedded-with-vs-code-docker-and-github-actions/
