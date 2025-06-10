#!/bin/bash


# if no args
if [ $# -eq 0 ]; then
    echo "Usage: $0 [clean|make]"
    exit 1
fi

MAKEFILES=$(find . -name Makefile)

if [ "$1" == "clean" ]; then
    
    for MAKEFILE in $MAKEFILES; do
        DIR=$(dirname "$MAKEFILE")
        
        cd "$DIR" || continue
        
        if [ -f compile_commands.json ]; then
            rm compile_commands.json
            echo "Removed compile_commands.json in $DIR"
        fi
        
        make clean > /dev/null
        echo "Cleaned $DIR"
        
        cd - > /dev/null || exit 1
    done
    
    exit 0


elif [ "$1" == "make" ]; then
  for MAKEFILE in $MAKEFILES; do
      DIR=$(dirname "$MAKEFILE")
      
      cd "$DIR" || continue
      
      bear -- make -j$(nproc)
      
      cd - > /dev/null || exit 1
  done
fi
