#!/bin/bash

exec > "$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/$(basename "${BASH_SOURCE[0]%.*}").output"

which tmate > /dev/null || { echo "Not installed"; exit 0; }

test -e /tmp/tmate.sock > /dev/null || { echo "Stopped"; exit 0; }

test -e /tmp/tmate.sock > /dev/null && pgrep tmate$ > /dev/null || rm -f /tmp/tmate.sock

pgrep tmate$ > /dev/null && pgrep tmate$ | xargs kill && echo "Stopped"
