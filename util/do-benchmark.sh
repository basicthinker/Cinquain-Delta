#! /bin/bash

if [ $# != 3 ]; then
  echo "USAGE: $0 DiffProgramFile DiffCommand DeltaList DataDir"
  echo "Note: delta list has reference and version file names"
  exit
fi

CMD=$1
LIST=(`cat $2`)
DIR=$3

for ((i=0; i<${#LIST[@]}; i+=2))
do
#  echo -e "${LIST[i+1]}\t"
  $CMD $DIR/${LIST[i]} $DIR/${LIST[i+1]}
done

