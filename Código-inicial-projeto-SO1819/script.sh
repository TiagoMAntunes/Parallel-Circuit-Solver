#! /bin/bash

for input in inputs/*.txt
do
    echo
    echo === $input ===
    echo File: $input
    echo Number of lines: $(wc -l $input | grep -o -P "[0-9]+\s")
    echo Number of interconnections: $(cat $input | grep -P -e "p\s+[0-9]" | wc -l)
done