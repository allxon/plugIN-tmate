#!/bin/bash

exec > "$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/$(basename "${BASH_SOURCE[0]%.*}").output"

id tmate > /dev/null 2>&1 || useradd -m tmate -s /bin/bash -g sudo 

which tmate > /dev/null && tmate -V && exit 0

which apt-get > /dev/null || { echo "Not supported!"; exit 1; }

apt-get update && apt-get install -y tmate
