FROM mcr.microsoft.com/devcontainers/cpp:1-ubuntu-22.04

# Install specific g++ version and ensure proper user setup
RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
    && apt-get -y install --no-install-recommends \
        gcc-11 \
        g++-11 \
        build-essential \
    && update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-11 100 \
    && update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-11 100 \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# Ensure the vscode user has sudo access without password for file permission fixes
RUN echo "vscode ALL=(ALL) NOPASSWD:ALL" >> /etc/sudoers.d/vscode

# Set the default user
USER vscode
