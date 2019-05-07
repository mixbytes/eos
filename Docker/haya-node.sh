#!/bin/sh
cd /opt/haya/bin

if [ ! -d "/opt/haya/bin/data-dir" ]; then
    mkdir /opt/haya/bin/data-dir
fi

if [ -f '/opt/haya/bin/data-dir/config.ini' ]; then
    echo
  else
    cp /config.ini /opt/haya/bin/data-dir
fi

if [ -d '/opt/haya/bin/data-dir/contracts' ]; then
    echo
  else
    cp -r /contracts /opt/haya/bin/data-dir
fi

while :; do
    case $1 in
        --config-dir=?*)
            CONFIG_DIR=${1#*=}
            ;;
        *)
            break
    esac
    shift
done

if [ ! "$CONFIG_DIR" ]; then
    CONFIG_DIR="--config-dir=/opt/haya/bin/data-dir"
else
    CONFIG_DIR=""
fi

exec /opt/haya/bin/haya-node $CONFIG_DIR "$@"
