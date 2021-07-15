#!/bin/bash

pgrep plugIN-tmate > /dev/null

if [[ $? -ne 0 ]]; then
	$(cd "$(dirname "${BASH_SOURCE[0]}")/../" && pwd)/plugIN-tmate $(cd "$(dirname "${BASH_SOURCE[0]}")/../" && pwd)/config/plugin_config_tmate.json > /dev/null &
fi
