ARG registry
FROM ${registry}/haya:latest-builder as builder
ARG symbol=SYS

RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get -y install ccache

ADD . haya
WORKDIR haya
ENV CCACHE_DIR /haya/.ccache

RUN git submodule init && git submodule update --init --recursive

RUN echo "$(git rev-parse --abbrev-ref HEAD):$(git rev-parse HEAD)" > /etc/haya-version \
    && cmake -H. -B"/tmp/build" -GNinja -DCMAKE_BUILD_TYPE=Release -DWASM_ROOT=/opt/wasm -DCMAKE_CXX_COMPILER=clang++ \
       -DCMAKE_C_COMPILER=clang -DCMAKE_INSTALL_PREFIX=/tmp/build -DBUILD_MONGO_DB_PLUGIN=true -DCORE_SYMBOL_NAME=$symbol \
    && cmake --build /tmp/build --target install


FROM ubuntu:18.04

RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get -y install openssl ca-certificates libcurl4-openssl-dev libusb-1.0 && rm -rf /var/lib/apt/lists/*
COPY --from=builder /usr/local/lib/* /usr/local/lib/
COPY --from=builder /tmp/build/bin /opt/haya/bin
COPY --from=builder /haya/Docker/config.ini /
COPY --from=builder /etc/haya-version /etc
COPY --from=builder /haya/Docker/haya-node.sh /opt/haya/bin/haya-node.sh
ENV EOSIO_ROOT=/opt/haya
RUN chmod +x /opt/haya/bin/haya-node.sh
ENV LD_LIBRARY_PATH /usr/local/lib
ENV PATH /opt/haya/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin
