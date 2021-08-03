#!/bin/bash

plugin_appguid=286b0652-c5ef-46c0-aa8c-7b617bbf6ab9
plugin_folder=plugIN-tmate

sudo mkdir -p /opt/allxon/plugIN/

sudo cp -r /var/lib/SyncAgent/download/tmp/$plugin_folder/$plugin_appguid /opt/allxon/plugIN/
echo "plugIN is installed to /opt/allxon/plugIN/$plugin_appguid/"

sudo rm -rf /var/lib/SyncAgent/download/tmp
sudo /opt/allxon/plugIN/$plugin_appguid/scripts/startPlugin.sh
