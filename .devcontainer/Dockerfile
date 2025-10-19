FROM mcr.microsoft.com/devcontainers/cpp:1-ubuntu-22.04

# Install specific g++ version and ensure proper user setup
RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
    && apt-get -y install --no-install-recommends \
        gcc-11 \
        g++-11 \
        build-essential \
        freeglut3-dev \
        libglm-dev\
    && update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-11 100 \
    && update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-11 100 \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# Ensure the vscode user has sudo access without password for file permission fixes
RUN echo "vscode ALL=(ALL) NOPASSWD:ALL" >> /etc/sudoers.d/vscode

RUN echo "PROMPT_COMMAND='PS1_CMD1=\$(git branch --show-current 2>/dev/null)'; PS1='\[\e[38;5;202m\][\[\e[38;5;208m\]\${PS1_CMD1}\[\e[38;5;202m\]]\[\e[0m\] \[\e[96;1m\]\u\[\e[22m\]@\[\e[38;5;111m\]\h\[\e[0m\]:\[\e[93;1m\]\w\n\[\e[0m\]\\$ '" >> /home/vscode/.bashrc

# Set the default user
USER vscode
