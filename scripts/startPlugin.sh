#!/bin/bash

pgrep tmatePlugin > /dev/null

if [[ $? -ne 0 ]]; then
	$(cd "$(dirname "${BASH_SOURCE[0]}")/../" && pwd)/tmate_plugIN $(cd "$(dirname "${BASH_SOURCE[0]}")/../" && pwd)/config/plugin_config_tmate.json > /dev/null &
fi
