#!/bin/bash

$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/commands/stop.sh
pgrep plugIN-tmate | xargs -i kill -2 {}
