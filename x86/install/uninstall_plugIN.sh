#!/bin/bash

plugin_appguid=286b0652-c5ef-46c0-aa8c-7b617bbf6ab9
plugin_service=plugIN-tmate.service

disable_plugin_service() {
    if [ -f "/etc/systemd/system/${plugin_service}" ]; then
        echo "Find the service ${plugin_service} from system, disable the service first."
        systemctl stop ${plugin_service}
        systemctl disable ${plugin_service}  
        rm /etc/systemd/system/${plugin_service} 
    fi
}

remove_plugin() {
    sudo rm -rf /opt/allxon/plugIN/$plugin_appguid
}

uninstall_plugIN() {

    disable_plugin_service

    remove_plugin

    exit 0
}

uninstall_plugIN