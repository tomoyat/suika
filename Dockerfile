FROM gcc:13.2

RUN apt-get update \
 && DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
    build-essential \
    netcat-openbsd \
    git \
    iproute2 \
    iputils-ping \
    cmake \
    gdb \
    sudo \
 && apt-get -y clean \
 && rm -rf /var/lib/apt/lists/*
