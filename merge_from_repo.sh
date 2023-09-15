#!/bin/bash

# Check for the correct number of arguments
if [ "$#" -ne 3 ]; then
    echo "Usage: $0 <source_repo_url> <source_repo_branch> <merge_or_rebase>"
    echo "Example: $0 https://github.com/username/sourceRepo.git master merge"
    exit 1
fi

# Get the arguments
SOURCE_REPO_URL="$1"
SOURCE_BRANCH="$2"
MERGE_OR_REBASE="$3"

# Name the remote as 'temp_remote_for_merge' to avoid conflicts with existing remote names
REMOTE_NAME="temp_remote_for_merge"

# Function to cleanup and exit
cleanup_and_exit() {
    echo "Cleaning up..."
    git remote remove $REMOTE_NAME
    exit $1
}

# Add the remote
git remote add $REMOTE_NAME $SOURCE_REPO_URL
if [ $? -ne 0 ]; then
    echo "Error adding remote."
    exit 1
fi

# Fetch the commits from the added remote
git fetch $REMOTE_NAME
if [ $? -ne 0 ]; then
    echo "Error fetching from remote."
    cleanup_and_exit 1
fi

# Merge or Rebase
if [ "$MERGE_OR_REBASE" == "merge" ]; then
    git merge $REMOTE_NAME/$SOURCE_BRANCH --allow-unrelated-histories
    if [ $? -ne 0 ]; then
        echo "Error merging. Resolve conflicts and continue manually."
        cleanup_and_exit 1
    fi
elif [ "$MERGE_OR_REBASE" == "rebase" ]; then
    git rebase $REMOTE_NAME/$SOURCE_BRANCH
    if [ $? -ne 0 ]; then
        echo "Error rebasing. Resolve conflicts and continue manually."
        cleanup_and_exit 1
    fi
else
    echo "Invalid option for merge_or_rebase. Choose 'merge' or 'rebase'."
    cleanup_and_exit 1
fi

# Cleanup
cleanup_and_exit 0
