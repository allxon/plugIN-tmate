#!/bin/bash

cd /var/lib/SyncAgent/download/tmp/tmate_plugIN
sudo mv 286b0652-c5ef-46c0-aa8c-7b617bbf6ab9 /opt/allxon/plugIN/
echo "plugIN is installed to /opt/allxon/plugIN/286b0652-c5ef-46c0-aa8c-7b617bbf6ab9/"
cd /var/lib/SyncAgent/download/
rm -rf tmp
sudo /opt/allxon/plugIN/286b0652-c5ef-46c0-aa8c-7b617bbf6ab9/scripts/startPlugin.sh