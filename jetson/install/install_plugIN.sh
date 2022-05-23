#!/bin/bash

plugin_appguid=3ff0bf0a-17a0-47c0-b9f6-229191393182
plugin_service=plugIN-tmate.service
output_file="install_plugIN_$plugin_appguid.output"
sudo mkdir -p $ALLXON_PLUGIN_DIR

check_for_install() {
    # If users try to install this plugIN on non-Jetson devices, then it will be returned
    if [ ! -f "/etc/nv_tegra_release" ]; then
    sudo echo "Not Supported" > $output_file 2>&1
    sudo cp $output_file $ALLXON_PLUGIN_DIR/
    sudo rm $output_file
    exit 1
    fi
}

install_plugin_files() {
    sudo cp -r ./$plugin_appguid/* $ALLXON_PLUGIN_DIR
    sudo cp ./$plugin_service /etc/systemd/system/
}

install_plugin_complete() {
    sudo echo "Installed" > $output_file 2>&1

    sudo cp $output_file /opt/allxon/plugIN/

    sudo rm $output_file

    echo "plugIN is installed to $ALLXON_PLUGIN_DIR"
}

initial_plugin_service_in_system() {
    sudo systemctl daemon-reload

    sudo chmod 644 /etc/systemd/system/$plugin_service

    sudo systemctl enable --now $plugin_service
}

install_plugIN() {
    check_for_install

    install_plugin_files

    install_plugin_complete

    initial_plugin_service_in_system > /dev/null 2>&1

    sleep 1

    exit 0
}

install_plugIN


