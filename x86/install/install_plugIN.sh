#!/bin/bash

plugin_appguid=286b0652-c5ef-46c0-aa8c-7b617bbf6ab9
plugin_folder=plugIN-tmate

sudo mkdir -p /opt/allxon/plugIN/

output_file="install_plugIN_$plugin_appguid.output"

# If users try to install this plugIN on non-Ubuntu x86 devices, then it will be returned
if [ -r /etc/os-release ]; then
    lsb_dist="$(. /etc/os-release && echo "$ID")"
    if [ ! "$lsb_dist" == "ubuntu" ]; then
       sudo echo "Not Supported Distribution" > $output_file 2>&1
       sudo cp $output_file /opt/allxon/plugIN/
       sudo rm $output_file
       exit 1
    fi
fi

arch=$(dpkg --print-architecture)
if [ ! "$arch" == "amd64" ] && [ ! "$arch" == "i386" ]; then
   sudo echo "Not Supported Architecture" > $output_file 2>&1
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
