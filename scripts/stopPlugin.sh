#!/bin/bash

$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/commands/stop.sh
pgrep tmate_plugIN | xargs -i kill -2 {}
