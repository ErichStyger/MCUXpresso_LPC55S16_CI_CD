# Fetch ubuntu image
FROM ubuntu:22.04

# Install prerequisites
RUN \
    apt update && \
    apt install -y cmake gcc-arm-none-eabi libnewlib-arm-none-eabi build-essential ninja-build
    
# create a directory for the project
RUN \
    mkdir -p /project/
    
# Copy project sources into image
COPY CMakeLists.txt             /project/
COPY arm-none-eabi-gcc.cmake    /project/
COPY sdk                        /project/sdk/
COPY src                        /project/src/
COPY McuLib                     /project/McuLib/

# Build project
RUN \
    cd /project && \
    cmake -G"Ninja" . -B build && \
    cmake --build build
    
# Command that will be invoked when the container starts
ENTRYPOINT ["/bin/bash"]
