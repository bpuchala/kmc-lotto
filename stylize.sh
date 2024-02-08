#!/bin/bash

echo "Stylizing declarations..."
for file in $(git diff --name-only --staged | grep ".hpp$"); do
    clang-format -style=google -i $file
done

echo "Stylizing actual programs..."
for file in $(git diff --name-only --staged | grep ".cpp$"); do
    clang-format -style=google -i $file
done

touch .is_stylized.txt
