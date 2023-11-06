#!/bin/bash
# Author : Ransi De Silva
# 

set -e
set -u

WRITEFILE=.
WRITESTR=""

if [ $# -lt 2 ]
then
  echo "Need to provide WRITEFILE and WRITESTR parameters"
  exit 1
else 
  WRITEFILE=$1
  WRITESTR=$2
fi

DIR="$(dirname "${WRITEFILE}")"
FILE="$(basename "${WRITEFILE}")"

mkdir -p "${DIR}" && touch "${DIR}/${FILE}"
echo "${WRITESTR}" > "${DIR}/${FILE}"

if [ ! -f "${DIR}/${FILE}" ];
then
  echo "${D1IR}/${FILE} Could not be created." 
  exit 1
fi
