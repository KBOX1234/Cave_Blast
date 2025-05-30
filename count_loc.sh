#!/bin/bash

# Ensure .gitignore exists
if [ ! -f .gitignore ]; then
    echo ".gitignore not found in the current directory."
    exit 1
fi

# Check if git is available
if ! command -v git &> /dev/null; then
    echo "git command is required but not found."
    exit 1
fi

# Generate the list of files tracked by git or not ignored
files=$(git ls-files --cached --others --exclude-standard)

# Filter for actual source code files (optional: you can refine extensions)
code_files=$(echo "$files" | grep -E '\.(c|cpp|h|py|js|ts|go|java|sh|rb|rs|php|html|css)$')

# Count lines
total_lines=0
for file in $code_files; do
    if [ -f "$file" ]; then
        lines=$(wc -l < "$file")
        total_lines=$((total_lines + lines))
    fi
done

echo "Total lines of code (excluding .gitignore files): $total_lines"
