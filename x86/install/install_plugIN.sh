#!/bin/bash

plugin_appguid=286b0652-c5ef-46c0-aa8c-7b617bbf6ab9
plugin_folder=plugIN-tmate

sudo mkdir -p $ALLXON_PLUGIN_DIR

output_file="install_plugIN_$plugin_appguid.output"

# If users try to install this plugIN on non-Ubuntu x86 devices, then it will be returned
if [ -r /etc/os-release ]; then
    lsb_dist="$(. /etc/os-release && echo "$ID")"
    if [ ! "$lsb_dist" == "ubuntu" ]; then
       sudo echo "Not Supported Distribution" > $output_file 2>&1
       sudo cp $output_file $ALLXON_PLUGIN_DIR/
       sudo rm $output_file
       exit 1
    fi
fi

arch=$(dpkg --print-architecture)
if [ ! "$arch" == "amd64" ] && [ ! "$arch" == "i386" ]; then
   sudo echo "Not Supported Architecture" > $output_file 2>&1
   sudo cp $output_file $ALLXON_PLUGIN_DIR/
   sudo rm $output_file
   exit 1
fi

sudo cp -r ./$plugin_appguid/* $ALLXON_PLUGIN_DIR

echo "plugIN is installed to $ALLXON_PLUGIN_DIR/"

sudo $ALLXON_PLUGIN_DIR/scripts/startPlugin.sh
