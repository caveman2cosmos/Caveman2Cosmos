#!/bin/bash

# Elevate permissions
if [ $EUID != 0 ]; then
    sudo "$0" "$@"
    exit $?
fi

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

apt-get --quiet --assume-yes update || exit $?

apt-get --quiet --assume-yes install \
    python2.7 \
    python-pip \
    python3 \
    python3-pip ||
    exit $?

echo "Setup finished"
