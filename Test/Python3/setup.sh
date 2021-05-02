#!/bin/bash

# Elevate permissions
if [ $EUID != 0 ]; then
    sudo "$0" "$@"
    exit $?
fi

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

apt-get --quiet --assume-yes update || exit $?

apt-get --quiet --assume-yes install \
    python3 \
    python3-pip ||
    exit $?

python3 -m pip install --requirement "$SCRIPT_DIR/requirements.txt" || exit $?

echo "Setup finished"
