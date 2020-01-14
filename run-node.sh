#!/bin/bash

# TODO: check this file!

NODE_PATH=$HOME/.haya-node
NODE_IMAGE='mixbytes/haya:v0.4.0'
CONFIG_URL='https://explorer.daobet.org/config'

read -p "Node data dir: $NODE_PATH, do you want change it? (y/n) " resp
if [ $resp == "y" ]; then
    read -p "Enter new node data dir:"$'\n' resp
    NODE_PATH=$(realpath $resp)
    echo "New node data dir: $NODE_PATH"
fi

if [ -f $NODE_PATH/config/config.ini ]; then
    CONFIG_EXISTS=true
fi

if [ -d $NODE_PATH/state/blocks ]; then
    STATE_EXISTS=true
fi

if [ -d $NODE_PATH/wallet/ ]; then
    WALLET_EXISTS=true
fi

mkdir -p $NODE_PATH/config
mkdir -p $NODE_PATH/state
mkdir -p $NODE_PATH/wallet

function download-configs() {
    echo "Downloading configs..."
    curl -sk $CONFIG_URL/config.ini -o $NODE_PATH/config/config.ini
    curl -sk $CONFIG_URL/genesis.json -o $NODE_PATH/config/genesis.json
    curl -sk $CONFIG_URL/logger.json -o $NODE_PATH/config/logger.json
}

if [ $CONFIG_EXISTS ]; then
    read -p "Config file already exists, do you want rewrite it? (y/n) " resp
    if [ $resp == "y" ]; then
        download-configs
    fi
else
    download-configs
fi

if [ $STATE_EXISTS ]; then
    read -p "Blockchain state already exists, do you want delete it? (y/n) " resp
    if [ $resp == "y" ]; then
        echo "Deleting state..."
        sudo rm -rf $NODE_PATH/state/*
    fi
fi

if [ $WALLET_EXISTS ]; then
    read -p "Wallet file already exists, do you want delete it? (y/n) " resp
    if [ $resp == "y" ]; then
        echo "Deleting wallet files..."
        sudo rm -rf $NODE_PATH/wallet/*
    fi
fi


HAYA_NODE_EXISTS=$(docker container ls -qf name=haya-node)
HAYA_WALLET_EXISTS=$(docker container ls -qf name=haya-wallet)

function run-haya-node() {
    echo "Running haya-node container..."
    docker run -d --cap-add IPC_LOCK \
    --env EOSIO_ROOT=/opt/haya \
    --env LD_LIBRARY_PATH=/usr/local/lib \
    --env PATH=/opt/haya/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin \
    --name haya-node \
    --network host \
    --volume $NODE_PATH:/opt/haya:rw \
    $NODE_IMAGE \
    /usr/bin/haya-node \
    --config-dir=/opt/haya/config \
    --genesis-json=/opt/haya/config/genesis.json \
    --logconf=/opt/haya/config/logger.json \
    -d /opt/haya/state
}

function run-haya-wallet() {
    echo "Running haya-wallet container..."
    docker run -d --network host \
    --volume $NODE_PATH:/opt/haya:rw \
    --name haya-wallet $NODE_IMAGE \
    /usr/bin/haya-wallet \
    --http-server-address=127.0.0.1:8899 \
    --http-alias=localhost:8899 \
    --unlock-timeout=99999999 \
    --wallet-dir=/opt/haya/wallet
}

if [ HAYA_NODE_EXISTS ]; then
    read -p "Haya node docker contrainer already exists, do you want delete it? (y/n) " resp
    if [ $resp == "y" ]; then
        echo "Deleting old haya-node container..."
        docker stop haya-node
        docker rm haya-node
        run-haya-node
    else
        read -p "Do you want restart haya node container? (y/n) " resp
        if [ $resp == "y" ]; then
            echo "Restarting haya-node container..."
            docker restart haya-node
        fi
    fi
else
    run-haya-node
fi



if [ $HAYA_WALLET_EXISTS ]; then
    read -p "Haya wallet docker contrainer already exists, do you want delete it? (y/n) " resp
    if [ $resp == "y" ]; then
        echo "Deleting old haya-wallet container..."
        docker stop haya-wallet
        docker rm haya-wallet
        run-haya-wallet
    else
        read -p "Do you want restart haya wallet container? (y/n) " resp
        if [ $resp == "y" ]; then
            echo "Restarting haya-wallet container..."
            docker restart haya-wallet
        fi
    fi
else
    run-haya-wallet
fi
