#!/bin/bash

plugin_appguid=3ff0bf0a-17a0-47c0-b9f6-229191393182
plugin_folder=plugIN-tmate

sudo mkdir -p /opt/allxon/plugIN/

sudo cp -r /var/lib/SyncAgent/download/tmp/$plugin_folder/$plugin_appguid /opt/allxon/plugIN/
echo "plugIN is installed to /opt/allxon/plugIN/$plugin_appguid/"

sudo rm -rf /var/lib/SyncAgent/download/tmp
sudo /opt/allxon/plugIN/$plugin_appguid/scripts/startPlugin.sh
