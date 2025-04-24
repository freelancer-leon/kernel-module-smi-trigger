#!/usr/bin/env bash
set -x

SMI_TRIGGER="/dev/smitrigger"

if [ -c $SMI_TRIGGER ]; then
   ./segment_fault &
   sleep 5
   echo $! > $SMI_TRIGGER
fi
