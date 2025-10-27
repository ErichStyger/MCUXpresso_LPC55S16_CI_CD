# Docker file for VS Code using DevContainer

FROM mcr.microsoft.com/devcontainers/cpp:dev-ubuntu24.04

# folder for downloaded files and virtual python environment
WORKDIR /apps
RUN mkdir -p /apps

# Install general prerequisites
RUN \
    apt update \
    && apt install -y git python3 python3-pip python3-setuptools wget unzip \
    && apt install -y cmake build-essential ninja-build \
    && apt install -y gcc-arm-none-eabi gdb-arm-none-eabi libnewlib-arm-none-eabi 

# Install utilities
RUN apt-get install -y nano iputils-ping udev curl

# Install doxygen and graphviz
RUN apt-get install -y doxygen graphviz

# Install west globally
RUN pip3 install --break-system-packages --upgrade west

# ARM compiler version:
# RUN curl -LO -k https://developer.arm.com/-/media/Files/downloads/gnu/13.2.rel1/binrel/arm-gnu-toolchain-13.2.rel1-x86_64-arm-none-eabi.tar.xz && \
#    tar xf arm-gnu-toolchain-13.2.rel1-x86_64-arm-none-eabi.tar.xz && \
#    rm arm-gnu-toolchain-13.2.rel1-x86_64-arm-none-eabi.tar.xz

# Get a specific version of the arm toolchain
# x86:              xpack-arm-none-eabi-gcc-13.2.1-1.1-linux-x64.tar.gz
# Apple silicon:    xpack-arm-none-eabi-gcc-13.2.1-1.1-linux-arm64.tar.gz
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
RUN \
    cd /apps \
    && python3 -m venv venv \
    && ./venv/bin/pip install gcovr

# Install SEGGER tools: make sure you have the same version installed on your host!
# for x86:           JLink_Linux_V810g_x86_64.deb
# for Apple silicon: JLink_Linux_V810g_arm64.deb
# in case issue with mismatch between J-Link version between host and container/image: use matching version
RUN \
    apt-get install -y \
        apt-transport-https ca-certificates software-properties-common \
        libx11-xcb1 libxcb-icccm4 libxcb-image0 libxcb-keysyms1 libxcb-randr0 \
        libxcb-render-util0 libxcb-shape0 libxcb-sync1 libxcb-util1 \
        libxcb-xfixes0 libxcb-xkb1 libxkbcommon-x11-0 libxkbcommon0 xkb-data \
    && cd /apps \
    && curl -d "accept_license_agreement=accepted&submit=Download+software" \
    -X POST -O "https://www.segger.com/downloads/jlink/JLink_Linux_V810g_x86_64.deb" \
    && dpkg --unpack JLink_Linux_V810g_x86_64.deb \
    && rm -f /var/lib/dpkg/info/jlink.postinst \
    && dpkg --configure jlink \
    && apt install -yf

# Install LinkServer software: make sure you have the same version installed on your host!
# Linkserver download paths, e.g.
# Windows: https://www.nxp.com/lgfiles/updates/mcuxpresso/LinkServer_25.7.33.exe
# Linux: https://www.nxp.com/lgfiles/updates/mcuxpresso/LinkServer_25.7.33.x86_64.deb.bin
# MacOS Arch64: https://www.nxp.com/lgfiles/updates/mcuxpresso/LinkServer_25.7.33.aarch64.pkg
# MacOS: https://www.nxp.com/lgfiles/updates/mcuxpresso/LinkServer_25.7.33.x86_64.pkg
#RUN \
#  	apt-get install -y whiptail libusb-1.0-0-dev dfu-util usbutils hwdata \
#    && cd /apps \
#    && curl -O https://www.nxp.com/lgfiles/updates/mcuxpresso/LinkServer_25.7.33.x86_64.deb.bin \
#    && chmod a+x LinkServer_25.7.33.x86_64.deb.bin \
#    && ./LinkServer_25.7.33.x86_64.deb.bin --noexec --target linkserver \
#    && cd linkserver \
#    && dpkg --unpack LinkServer_25.7.33.x86_64.deb \
#    && rm /var/lib/dpkg/info/linkserver_25.7.33.postinst \
#    && dpkg --configure linkserver_25.7.33 \
#    && dpkg --unpack LPCScrypt.deb \
#    && rm /var/lib/dpkg/info/lpcscrypt.postinst \
#    && apt-get install -yf

# which version to use, see https://mcuoneclipse.com/2025/08/11/streamlining-linkserver-installation-for-ci-cd/
# it gets installed into /usr/local/LinkServer_<version>
ARG LINKSERVER_VERSION="25.7.33"

# install dependencies: need to start udev manually, otherwise getting "Failed to send reload request: No such file or directory" during LinkServer installation
RUN \
    apt-get install -y whiptail libusb-1.0-0-dev dfu-util usbutils hwdata \
    && /lib/systemd/systemd-udevd --daemon

# install LinkServer software
RUN \
    curl -O https://www.nxp.com/lgfiles/updates/mcuxpresso/LinkServer_${LINKSERVER_VERSION}.x86_64.deb.bin \
    && chmod a+x LinkServer_${LINKSERVER_VERSION}.x86_64.deb.bin \
    && ./LinkServer_${LINKSERVER_VERSION}.x86_64.deb.bin acceptLicense skipIdeSelect

# Add LinkServer to PATH
ENV PATH="${PATH}:/usr/local/LinkServer_${LINKSERVER_VERSION}"

# Command that will be invoked when the container starts
ENTRYPOINT ["/bin/bash"]
