#!/bin/bash

plugin_appguid=3ff0bf0a-17a0-47c0-b9f6-229191393182
plugin_folder=plugIN-tmate

sudo mkdir -p $ALLXON_PLUGIN_DIR/

output_file="install_plugIN_$plugin_appguid.output"

# If users try to install this plugIN on non-Jetson devices, then it will be returned
if [ ! -f "/etc/nv_tegra_release" ]; then
   sudo echo "Not Supported" > $output_file 2>&1
   sudo cp $output_file $ALLXON_PLUGIN_DIR/
   sudo rm $output_file
   exit 1
fi

sudo cp -r ./$plugin_appguid/* $ALLXON_PLUGIN_DIR

echo "plugIN is installed to $ALLXON_PLUGIN_DIR"

sudo $ALLXON_PLUGIN_DIR/scripts/startPlugin.sh