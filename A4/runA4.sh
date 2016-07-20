#!/bin/bash

for var in "$@"
do
    echo "running \"./A4 Assets/$var.lua\" ..."
    ./A4 Assets/"$var".lua
    echo "Done"
done

