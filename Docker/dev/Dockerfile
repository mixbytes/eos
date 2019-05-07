ARG registry
FROM ${registry}/haya-builder:latest as builder
ARG symbol=SYS

ADD . haya
WORKDIR haya

RUN git submodule init && git submodule update --init --recursive

RUN echo "$(git rev-parse --abbrev-ref HEAD):$(git rev-parse HEAD)" > /etc/haya-version \
    && cmake -H. -B"/opt/haya" -GNinja -DCMAKE_BUILD_TYPE=Release -DWASM_ROOT=/opt/wasm -DCMAKE_CXX_COMPILER=clang++ \
       -DCMAKE_C_COMPILER=clang -DCMAKE_INSTALL_PREFIX=/opt/haya -DBUILD_MONGO_DB_PLUGIN=true -DCORE_SYMBOL_NAME=$symbol \
    && cmake --build /opt/haya --target install \
    && cp /haya/Docker/config.ini / && ln -s /opt/haya/contracts /contracts && cp /haya/Docker/nodeosd.sh /opt/haya/bin/nodeosd.sh && ln -s /haya/tutorials /tutorials

RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get -y install openssl ca-certificates vim psmisc python3-pip libcurl4-openssl-dev libusb-1.0 && rm -rf /var/lib/apt/lists/*
RUN pip3 install numpy
ENV EOSIO_ROOT=/opt/haya
RUN chmod +x /opt/haya/bin/nodeosd.sh
ENV LD_LIBRARY_PATH /usr/local/lib
ENV PATH /opt/haya/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin
