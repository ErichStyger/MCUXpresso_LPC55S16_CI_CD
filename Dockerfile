# Docker file used with GitHub actions
# Fetch ubuntu image
FROM ubuntu:24.04

# Install prerequisites
RUN \
    apt update && \
    apt install -y cmake gcc-arm-none-eabi libnewlib-arm-none-eabi build-essential ninja-build

# Install nano editor, used to edit files inside container
RUN apt-get install -y nano

# Install doxygen and graphviz
RUN apt-get install -y doxygen graphviz

# Install networking utilities
RUN apt-get install -y iputils-ping

# Install curl and others, needed to get SEGGER
RUN apt-get install -y apt-transport-https ca-certificates curl software-properties-common udev
RUN apt-get install -y libx11-xcb1 libxcb-icccm4 libxcb-image0 libxcb-keysyms1 libxcb-randr0 libxcb-render-util0 libxcb-shape0 libxcb-sync1 libxcb-util1 libxcb-xfixes0 libxcb-xkb1 libxkbcommon-x11-0 libxkbcommon0 xkb-data

# Install SEGGER: binaries will be in /usr/bin
RUN \
    mkdir -p /project \
    && cd /project \
    && curl -d "accept_license_agreement=accepted&submit=Download+software" -X POST -O "https://www.segger.com/downloads/jlink/JLink_Linux_V810g_x86_64.deb" 
# in case issue with mismatch between J-Link version between host and container/image: use matching version
# issue with udev, see https://forum.segger.com/index.php/Thread/8953-SOLVED-J-Link-Linux-installer-fails-for-Docker-containers-Error-Failed-to-update/
RUN \
    cd /project \
    && dpkg --unpack JLink_Linux_V810g_x86_64.deb \
    && rm -f /var/lib/dpkg/info/jlink.postinst \
    && dpkg --configure jlink \
    && apt install -yf

# Install LinkServer software: make sure you have the same version installed on your host!
RUN \
  apt-get install -y whiptail libusb-1.0-0-dev dfu-util usbutils hwdata
# Linkserver download paths, e.g.
# Windows: https://www.nxp.com/lgfiles/updates/mcuxpresso/LinkServer_24.9.75.exe
# Linux: https://www.nxp.com/lgfiles/updates/mcuxpresso/LinkServer_24.9.75.x86_64.deb.bin
# MacOS Arch64: https://www.nxp.com/lgfiles/updates/mcuxpresso/LinkServer_24.9.75.aarch64.pkg
# MacOS: https://www.nxp.com/lgfiles/updates/mcuxpresso/LinkServer_24.9.75.x86_64.pkg
RUN \
    cd /project \
    && curl -O https://www.nxp.com/lgfiles/updates/mcuxpresso/LinkServer_24.9.75.x86_64.deb.bin \
    && chmod a+x LinkServer_24.9.75.x86_64.deb.bin \
    && ./LinkServer_24.9.75.x86_64.deb.bin --noexec --target linkserver \
    && cd linkserver \
    && dpkg --unpack LinkServer_24.9.75.x86_64.deb \
    && rm /var/lib/dpkg/info/linkserver_24.9.75.postinst \
    && dpkg --configure linkserver_24.9.75 \
    && dpkg --unpack LPCScrypt.deb \
    && rm /var/lib/dpkg/info/lpcscrypt.postinst \
    && apt-get install -yf

# Binary: /usr/local/LinkServer_24.9.75/LinkServer
# https://stackoverflow.com/questions/66353515/mcuxpresso-eclipse-project-with-c-automatic-headless-build

# create a directory for the project
RUN \
    pwd \
    && mkdir -p /project/
    
# Copy project sources into image
COPY CMakeLists.txt             /project/
COPY CMakePresets.json          /project/
COPY arm-none-eabi-gcc.cmake    /project/
COPY sdk                        /project/sdk/
COPY src                        /project/src/

RUN \
    cd /projeect && \
    git clone https://github.com/ErichStyger/McuLib

# Build project
#RUN \
#    ls /home/runner/work/MCUXpresso_LPC55S16_CI_CD/MCUXpresso_LPC55S16_CI_CD \
#    cp /home/runner/work/MCUXpresso_LPC55S16_CI_CD/MCUXpresso_LPC55S16_CI_CD/McuLib /project/McuLib

RUN \
    cd /project && \
    ls -al && \
    cmake --preset Debug && \
    cmake --build --preset app-debug && \
    cmake --preset Release && \
    cmake --build --preset app-releasecmake --preset Release
    
# Command that will be invoked when the container starts
ENTRYPOINT ["/bin/bash"]
