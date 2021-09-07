#!/bin/bash

plugin_appguid=286b0652-c5ef-46c0-aa8c-7b617bbf6ab9

echo "Stop running plugIN-tmate."
sudo /opt/allxon/plugIN/$plugin_appguid/scripts/stopPlugin.sh
sudo rm -rf /opt/allxon/plugIN/$plugin_appguid