#!/bin/bash

set -eu
set -x

#~. "${BASH_SOURCE[0]%/*}/../scripts/utils.sh"

readonly os="${1:?}"
readonly project="${2:?}"
readonly build_type="${3:?}"
readonly tests="${4:-}"

readonly root="/work/$project"
readonly build_dir="$root/build"

case "$os" in
(ubuntu) pkg_extension=deb ;;
(centos) pkg_extension=rpm ;;
(*)      echo "Bad os: $os." ; exit 1 ;;
esac

rm -rf $build_dir/*
mkdir -p $build_dir

#~cp -arf ${SRC_DIR}/${PROJECT_NAME} ${build_dir}/${PROJECT_NAME}
#~cd "${build_dir}/${PROJECT_NAME}"

"$root"/scripts/build.sh --build-type "$build_type"

if [[ -x "$( command -v ccache &>/dev/null )" ]]; then
  ccache -s
fi

if [[ -n "$tests" ]]; then
  if [[ "$os" == centos ]]; then
    set +u
    source /opt/rh/python33/enable
    set -u
  fi
  pushd "$build_dir"
    ctest --output-on-failure -E mongo -R "$tests"
  popd
fi

pushd "$build_dir"/packages
  #~mkdir -p ${SRC_DIR}/${PROJECT_NAME}/build/pkgs

  ./generate_package.sh "$pkg_extension"
  ls -la

  #~ cp -av *.deb *.tar.gz *.rpm ../pkgs/
popd
