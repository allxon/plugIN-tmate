#!/bin/bash

exec > "$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/$(basename "${BASH_SOURCE[0]%.*}").output"

which tmate > /dev/null || { echo "Not installed"; exit 0; }

test -e /tmp/tmate.sock > /dev/null || { echo "Stopped"; exit 0; }

test -e /tmp/tmate.sock > /dev/null && pidof tmate > /dev/null || rm -f /tmp/tmate.sock

pidof tmate > /dev/null && pidof tmate | xargs kill && echo "Stopped"
