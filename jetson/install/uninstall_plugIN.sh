#!/bin/bash

plugin_appguid=3ff0bf0a-17a0-47c0-b9f6-229191393182

echo "Stop running plugIN-tmate."
currentShDirectory=$(dirname ${BASH_SOURCE})
sudo $currentShDirectory/scripts/stopPlugin.sh
sudo rm -rf $currentShDirectory