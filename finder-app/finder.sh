#!/bin/sh
# Author : Ransi De Silva
# 

set -e
set -u

FILESDIR=.
SEARCHSTR=""

if [ $# -lt 2 ]
then
  echo "Need to set FILEDIR and SEARSTR parameters"
  exit 1
else 
  FILESDIR=$1
  SEARCHSTR=$2
fi

if [ ! -d "$FILESDIR" ];
then
  echo ""$FILESDIR" Does not exist."
  exit 1 
fi

FILECOUNT=$(find "$FILESDIR" -type f | wc -l)
LINECOUNT=$(grep -r "$SEARCHSTR" "$FILESDIR" | wc -l)

echo "The number of files are ${FILECOUNT} and the number of matching lines are ${LINECOUNT}"
