#!/bin/bash
set -eu -o pipefail

# This script creates QPianoMusicBooks.zip and generates music_blank.ts
# License: same with QPiano
# Author: Alexey Loginov <alexl@mageia.org>

if [ ! -f "/usr/bin/markdown" ]
then
  echo "Program '/usr/bin/markdown' was not found. Please install 'discount' package."
  exit 0
fi

if [ ! -f "/usr/bin/zip" ]
then
  echo "Program '/usr/bin/zip' was not found. Please install 'zip' package."
  exit 0
fi

pushd ../

echo "Determining MUSIC_RELEASE..."
version=`cat src/version.txt|grep 'MUSIC_RELEASE'|cut -d " " --fields=3`
echo "Done for determining MUSIC_RELEASE"
echo "MUSIC_RELEASE=$version"

echo "Preparing temporary directory..."
rm -rf music/QPianoMusicBooks$version
echo "Done for preparing temporary directory"

echo "Creating subdirectories in temporary directory..."
mkdir -p music/QPianoMusicBooks$version/BeginnerCourse/InfoPages
mkdir -p music/QPianoMusicBooks$version/BoosterMusic/InfoPages
echo "Done for creating subdirectories in temporary directory"

echo "Copying MIDI files..."
cp -f doc/courses/BeginnerCourse/*.mid music/QPianoMusicBooks$version/BeginnerCourse/
cp -f doc/courses/BoosterMusic/*.mid music/QPianoMusicBooks$version/BoosterMusic/
echo "Done for copying MIDI files"

echo "Generating HTML files from MD files..."
for file in `find ./music/src -name ??-*.md`
do
  file_html=`echo $file|sed "s|_en.md|_en.html|g"|sed "s|./music/src|music/QPianoMusicBooks$version|g"|sed "s|/0|/InfoPages/0|g"`
  markdown $file > $file_html
  awk 'NR>1{printf "\n"} {printf "%s", $0}' $file_html > "$file_html"_tmp
  mv -f "$file_html"_tmp $file_html
  sed -i 's|<p><strong>Hint:|<font color="#ff0000"><p><strong>Hint:|g' $file_html
  printf "</font>" >> $file_html
  printf "</body>" >> $file_html
  sed -i '1s/^/<body bgcolor="#FFFFC0">/' $file_html
  sed -i "s|&rsquo;|'|g" $file_html
  sed -i "s|&hellip;|. . .|g" $file_html
done
echo "Done for generating HTML files from MD files"

echo "Renaming directories..."
mv -f "music/QPianoMusicBooks$version/BeginnerCourse" "music/QPianoMusicBooks$version/Beginner Course"
mv -f "music/QPianoMusicBooks$version/BoosterMusic" "music/QPianoMusicBooks$version/Booster Music"
echo "Done for renaming directories"

echo "Creating ZIP file..."
pushd music
rm -f QPianoMusicBooks.zip
zip -r QPianoMusicBooks.zip QPianoMusicBooks$version
popd
echo "Done for creating ZIP file"

popd

echo "Generating music_blank.ts by conv_html_to_ts.pl..."
./conv_html_to_ts.pl ../music/QPianoMusicBooks$version
mv music_blank.ts ../translations/
echo "Done for generating music_blank.ts by conv_html_to_ts.pl"

echo "Deleting temporary directory..."
rm -rf ../music/QPianoMusicBooks$version
echo "Done for deleting temporary directory"
