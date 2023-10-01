# Fetch ubuntu image
FROM ubuntu:22.04

# Install prerequisites
RUN \
    apt update && \
    apt install -y cmake gcc-arm-none-eabi libnewlib-arm-none-eabi build-essential
    
# Clone our project repo
RUN \
    mkdir -p /project/src/ && \
    cd /project/
#    mkdir -p /project/src/ && \
#    cd /project/ && \
#    git clone https://github.com/ErichStyger/MCUXpresso_LPC55S16_CI_CD.git --branch master
    
# Set the compiler path
#ENV PICO_SDK_PATH=/project/pico-sdk/

# Copy in our source files
COPY CMakeLists.txt             /project/
COPY arm-none-eabi-gcc.cmake    /project/
COPY sdk                        /project/sdk/
COPY src                        /project/src/
COPY McuLib                     /project/McuLib/

# Build project
RUN \
    cd /project && \
    cmake -G"Unix Makefiles" . -B build && \
    cmake --build build
    
# Command that will be invoked when the container starts
ENTRYPOINT ["/bin/bash"]
