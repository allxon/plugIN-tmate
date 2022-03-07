#!/bin/bash

plugin_appguid=286b0652-c5ef-46c0-aa8c-7b617bbf6ab9

echo "Stop running plugIN-tmate."
currentShDirectory=$(dirname ${BASH_SOURCE})
sudo $currentShDirectory/scripts/stopPlugin.sh
sudo rm -rf $currentShDirectory