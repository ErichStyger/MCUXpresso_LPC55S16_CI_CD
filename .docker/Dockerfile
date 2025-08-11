# Docker file used with GitHub actions
# See .docker/README_CI_CD.md

FROM ubuntu:24.04

# folder for downloaded files
WORKDIR /apps
RUN mkdir -p /apps

# Install prerequisites
RUN \
    apt update \
    && apt install -y git python3 wget unzip \
    && apt install -y cmake build-essential ninja-build \
    && apt install -y gcc-arm-none-eabi gdb-arm-none-eabi libnewlib-arm-none-eabi 

# Install nano editor
RUN apt-get install -y nano

# Install doxygen and graphviz
RUN apt-get install -y doxygen graphviz

# Install networking utilities
RUN apt-get update && apt-get install -y iputils-ping

# Install curl and others, needed to get SEGGER files
RUN apt-get install -y apt-transport-https ca-certificates curl software-properties-common
RUN apt-get install -y libx11-xcb1 libxcb-icccm4 libxcb-image0 libxcb-keysyms1 libxcb-randr0 libxcb-render-util0 libxcb-shape0 libxcb-sync1 libxcb-util1 libxcb-xfixes0 libxcb-xkb1 libxkbcommon-x11-0 libxkbcommon0 xkb-data
RUN apt-get install -y udev

# Get a specific version of the arm toolchain
RUN \
    cd /apps \
    && wget https://github.com/xpack-dev-tools/arm-none-eabi-gcc-xpack/releases/download/v13.2.1-1.1/xpack-arm-none-eabi-gcc-13.2.1-1.1-linux-x64.tar.gz -O gcc-arm-none-eabi.tar.gz \
    && mkdir /opt/gcc-arm-none-eabi-13.2.1-1.1 \
    && tar -xvzf gcc-arm-none-eabi.tar.gz -C /opt/gcc-arm-none-eabi-13.2.1-1.1 --strip-components 1 \
    && rm gcc-arm-none-eabi.tar.gz \
    && ln -s /opt/gcc-arm-none-eabi-13.2.1-1.1/bin/* /usr/local/bin

# for gcov, have to use the arm-none-eabi-gcov one
RUN \
    rm /usr/bin/gcov \
    && cd /usr/bin \
    && ln -s /opt/gcc-arm-none-eabi-13.2.1-1.1/bin/arm-none-eabi-gcov gcov

# Install gcovr (needs pip) and virtual environment
RUN \
  apt install -y python3-pip python3.12-venv

# Install SEGGER
  RUN \
      cd /apps \
      && curl -d "accept_license_agreement=accepted&submit=Download+software" -X POST -O "https://www.segger.com/downloads/jlink/JLink_Linux_V810g_x86_64.deb" 
  # in case issue with mismatch between J-Link version between host and container/image: use matching version
  # issue with udev, see https://forum.segger.com/index.php/Thread/8953-SOLVED-J-Link-Linux-installer-fails-for-Docker-containers-Error-Failed-to-update/
  RUN \
      cd /apps \
      && dpkg --unpack JLink_Linux_V810g_x86_64.deb \
      && rm -f /var/lib/dpkg/info/jlink.postinst \
      && dpkg --configure jlink \
      && apt install -yf
  
# Install LinkServer software: make sure you have the same version installed on your host!
# Linkserver download paths, e.g.
# Windows: https://www.nxp.com/lgfiles/updates/mcuxpresso/LinkServer_24.12.21.exe
# Linux: https://www.nxp.com/lgfiles/updates/mcuxpresso/LinkServer_24.12.21.x86_64.deb.bin
# MacOS Arch64: https://www.nxp.com/lgfiles/updates/mcuxpresso/LinkServer_24.12.21.aarch64.pkg
# MacOS: https://www.nxp.com/lgfiles/updates/mcuxpresso/LinkServer_24.12.21.x86_64.pkg
RUN \
    apt-get install -y whiptail libusb-1.0-0-dev dfu-util usbutils hwdata \
    cd /apps \
    && curl -O https://www.nxp.com/lgfiles/updates/mcuxpresso/LinkServer_24.12.21.x86_64.deb.bin \
    && chmod a+x LinkServer_24.12.21.x86_64.deb.bin \
    && ./LinkServer_24.12.21.x86_64.deb.bin --noexec --target linkserver \
    && cd linkserver \
    && dpkg --unpack LinkServer_24.12.21.x86_64.deb \
    && rm /var/lib/dpkg/info/linkserver_24.12.21.postinst \
    && dpkg --configure linkserver_24.12.21 \
    && dpkg --unpack LPCScrypt.deb \
    && rm /var/lib/dpkg/info/lpcscrypt.postinst \
    && apt-get install -yf

# create a directory for the project
RUN \
    mkdir -p /project/
    
# Copy project sources into image
COPY arm-none-eabi-gcc.cmake    /project/
COPY CMakePresets.json          /project/
COPY CMakeLists.txt             /project/
COPY arm-none-eabi-gcc.cmake    /project/
COPY sdk                        /project/sdk/
COPY src                        /project/src/
COPY McuLib                     /project/McuLib/
COPY .gitlab/*.sh               /project/

RUN \
    cd /project \
    && ls -al \
    && chmod +x build.sh \
    && chmod +x test.sh \
    && chmod +x release.sh

# Build project
RUN \
  cd /project \
  && cmake --preset Debug \
  && cmake --build --preset app-debug \
  && cmake --preset Release \
  && cmake --build --preset app-release \
  && cmake --preset Test \
  && cmake --build --preset app-test

# Command that will be invoked when the container starts
ENTRYPOINT ["/bin/bash"]
