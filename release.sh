#!/bin/bash
TARGET_VERSION=$1

update_version() {
    local TARGET_FILENAME=$1
    local REPLACE_FROM_REGEX=$2
    local REPLACE_TO_REGEX=$3
    local TEMP_FILE=$(mktemp)

    # Find version part
    cat ${TARGET_FILENAME} | while read LINE; do
        if [[ $LINE =~ $REPLACE_FROM_REGEX ]]; then
            echo ${BASH_REMATCH[1]} >${TEMP_FILE}
        fi
    done

    local CURRENT_VERSION=$(cat ${TEMP_FILE})
    if [[ $CURRENT_VERSION == "" ]]; then
        echo "Can't found version"
        rm ${TEMP_FILE}
        exit 1
    fi

    if [[ $CURRENT_VERSION == $TARGET_VERSION ]]; then
        echo "Version already matched, don't need to update"
        rm ${TEMP_FILE}
        return
    else
        echo "Current version: $CURRENT_VERSION replace to Target version: $TARGET_VERSION"
        sed -i -E "s/$REPLACE_FROM_REGEX/$REPLACE_TO_REGEX/g" $TARGET_FILENAME
    fi

    rm ${TEMP_FILE}
}

update_version Makefile "BUILD_VERSION := '(.+)'" "BUILD_VERSION := '$TARGET_VERSION'"
update_version jetson/config/tmatePluginUpdate.json "\"version\": \"(.+)\"," "\"version\": \"$TARGET_VERSION\","
update_version x86/config/tmatePluginUpdate.json "\"version\": \"(.+)\"," "\"version\": \"$TARGET_VERSION\","