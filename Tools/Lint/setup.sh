#!/bin/bash

# Elevate permissions
if [ $EUID != 0 ]; then
    sudo "$0" "$@"
    exit $?
fi

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

apt-get --quiet --assume-yes update || exit $?

apt-get --quiet --assume-yes install \
    curl \
    python2 \
    python3 \
    python3-pip ||
    exit $?

TEMP_DIR=$(mktemp --tmpdir --directory install-pip-for-python-2-XXXX) || exit $?

cd "$TEMP_DIR" || exit $?

GET_PIP_SCRIPT="get-pip.py"

curl https://bootstrap.pypa.io/get-pip.py --output "$GET_PIP_SCRIPT" || exit $?
python2 "$GET_PIP_SCRIPT" || exit $?

cd - || exit $?
rm --recursive "$TEMP_DIR" || exit $?

echo "Done setting up pylint environment"