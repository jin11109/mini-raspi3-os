#!/bin/bash

# Add all tools in tools directory to PATH
for d in "$(dirname "$BASH_SOURCE")"/tools/*/bin; do
    if [ -d "$d" ] && [[ ":$PATH:" != *":$d:"* ]]; then
        PATH="$d:$PATH"
    fi
done

export PATH