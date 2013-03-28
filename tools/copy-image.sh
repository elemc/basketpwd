#!/bin/sh

names=$*
folders="16x16 22x22 32x32 48x48"
base_dir="/usr/share/icons/oxygen"
part_dir="actions"

rcc_add=""

for name in $names; do
    for size in $folders; do
        dest_dir=icons/${size}/
        if [ ! -d ${dest_dir} ]; then
            mkdir -p ${dest_dir}
        fi
        cp -R ${base_dir}/${size}/${part_dir}/${name}.png icons/${size}/
        rcc_add="$rcc_add\n<file alias=\"${size}/${name}.png\">icons/${size}/${name}.png</file>"
    done
done
echo -e $rcc_add
