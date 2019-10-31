#!/usr/bin/env bash
set -e
set +x

PROJECT_NAME=$1
SRC_DIR="/mnt/src"
BUILD_DIR="/var/src"

DIST=$2
BUILD_TYPE=${3:-Release}
TESTS=$4

if [[ -d ${BUILD_DIR} ]]; then
  rm -rf ${BUILD_DIR}
  mkdir -p ${BUILD_DIR}
else
  mkdir -p ${BUILD_DIR}
fi

cp -arf ${SRC_DIR}/${PROJECT_NAME} ${BUILD_DIR}/${PROJECT_NAME}
cd "${BUILD_DIR}/${PROJECT_NAME}"
#git submodule update --init --recursive
cd scripts
./${PROJECT_NAME}_build.sh -y -f -n -o ${BUILD_TYPE}

if [ -x $(command -v ccache) ]; then
    echo "================CCACHE================="
    ccache -s
    echo "======================================="
fi

if [ $TESTS ]; then
    if [ $DIST == "centos" ]; then
        source /opt/rh/python33/enable
    fi
    cd "${BUILD_DIR}/${PROJECT_NAME}/build"
    /root/bin/ctest --output-on-failure -E mongo -R "$TESTS"
fi

cd "${BUILD_DIR}/${PROJECT_NAME}/build/packages/"
chmod +x generate_package.sh
mkdir -p ${SRC_DIR}/${PROJECT_NAME}/build/pkgs

case $DIST in
  ubuntu)
    cd "/var/src/${PROJECT_NAME}/build/packages/"
    bash "generate_package.sh" deb
    cp -av *.deb ${SRC_DIR}/${PROJECT_NAME}/build/pkgs
    cp -av *.tar.gz ${SRC_DIR}/${PROJECT_NAME}/build/pkgs
  ;;
  centos)
    cd "/var/src/${PROJECT_NAME}/build/packages/"
    bash "generate_package.sh" rpm
    cp -av *.rpm ${SRC_DIR}/${PROJECT_NAME}/build/pkgs
  ;;
esac
