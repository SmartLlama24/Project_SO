#!/bin/bash

hunt_id="$1"
file_output=$(./treasure_manager --list "$hunt_id" 2>/dev/null)

if [ -z "$file_output" ]; then
    echo "No treasures or invalid hunt: $hunt_id"
    exit 1
fi

# We'll parse block by block
echo "$file_output" | awk '
    /^Treasure Id:/ {
        username = "";
        value = 0;
    }
    /^Username:/ {
        gsub(/^Username: /, "", $0);
        username = $0;
    }
    /^Value:/ {
        gsub(/^Value: /, "", $0);
        value = $0;
        score[username] += value;
    }
    END {
        for (u in score)
            if (u != "")
                printf("%s: %d points\n", u, score[u]);
    }
'

