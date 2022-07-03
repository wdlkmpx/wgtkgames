#!/bin/sh

if command -v ffmpeg-static ; then
    ffmpeg='ffmpeg-static'
elif command -v ffmpeg ; then
    ffmpeg='ffmpeg'
else
    echo 'ffmpeg is not installed'
    exit 1
fi

echo

for dir in $(find . -mindepth 1 -maxdepth 1 -type d)
do
    cd ${dir}
    #--
    pixmaps=$(ls *.xpm 2>/dev/null)
    if [ -z "$pixmaps" ] ; then
        echo "$dir doesn't contain .xpm files"
        cd ..
        continue
    fi
    for i in ${pixmaps}
    do
        base=${i%.xpm}
        ${ffmpeg} -i ${i} -compression_level 100 -y ${base}.png
    done
    sed -i 's%xpm%png%g' themerc
    sync
    rm -f *.xpm
    #--
    cd ..
done

