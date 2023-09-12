# Get the base image
FROM ubuntu:22.04

LABEL Remarks="Dockerfile for ABCg on Ubuntu 22.04"

# Install build tools and dependencies
RUN apt-get update && apt-get install -y \
  build-essential \
  ca-certificates \
  cmake \
  python3 \
  pip \
  git \
  # GLEW, SDL, SDL_image
  libglew-dev \
  libsdl2-dev \
  libsdl2-image-dev \
  # glvnd and X dependencies for interop with NVIDIA Container Toolkit
  libglx0 \
  libxext6 libx11-6 \
  libglvnd0 libglvnd-dev \
  libgl1 libgl1-mesa-dev \
  libegl1 libegl1-mesa-dev \
  # Clean out cache
  && rm -rf /var/lib/apt/lists/*

# Install Emscripten
RUN git clone https://github.com/emscripten-core/emsdk.git \
  && cd emsdk \
  && git pull \
  && ./emsdk install latest \
  && ./emsdk activate latest \
  && echo 'source /emsdk/emsdk_env.sh > /dev/null 2>&1' >> ~/.bashrc

# Expose http ports
EXPOSE 8080

# Set environment variables
ENV HOME /root
ENV NVIDIA_VISIBLE_DEVICES all
ENV NVIDIA_DRIVER_CAPABILITIES all

# Set working directory
WORKDIR /abcg

# Copy data to image
COPY . /abcg

# Define a command to be run after the docker is up
CMD ["bash", "-l"]
