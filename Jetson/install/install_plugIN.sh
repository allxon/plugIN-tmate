#!/bin/bash

plugin_appguid=3ff0bf0a-17a0-47c0-b9f6-229191393182
plugin_folder=plugIN-tmate

sudo mkdir -p /opt/allxon/plugIN/

output_file="install_plugIN_$plugin_appguid.output"

# If users try to install this plugIN on non-Jetson devices, then it will be returned
if [ ! -f "/etc/nv_tegra_release" ]; then
   sudo echo "Not Supported" > $output_file 2>&1
   sudo cp $output_file /opt/allxon/plugIN/
   sudo rm $output_file
   exit 1
fi

# Determine the case belongs to OTA installation or Install by Preload installer
if [ -d "/var/lib/SyncAgent/download/tmp/$plugin_folder/$plugin_appguid" ]; then
   sudo cp -r /var/lib/SyncAgent/download/tmp/$plugin_folder/$plugin_appguid /opt/allxon/plugIN/
   sudo rm -rf /var/lib/SyncAgent/download/tmp
else
   sudo cp -r ./$plugin_appguid /opt/allxon/plugIN/
fi

echo "plugIN is installed to /opt/allxon/plugIN/$plugin_appguid/"

sudo /opt/allxon/plugIN/$plugin_appguid/scripts/startPlugin.sh
