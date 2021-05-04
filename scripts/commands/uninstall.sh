#!/bin/bash

exec > "$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/$(basename "${BASH_SOURCE[0]%.*}").output"

which tmate > /dev/null || { echo "Not installed"; exit 0; }

"$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/stop.sh"

apt-get remove -y tmate

apt-get purge -y tmate

apt-get autoremove -y

userdel -r tmate
