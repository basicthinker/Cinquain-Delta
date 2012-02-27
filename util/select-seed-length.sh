#! /bin/bash

MIN_SEED=16
MAX_SEED=96
STEP=16

if [ $# != 3 ]; then
  echo "USAGE: $0 DiffCommand DeltaList DataDir"
  echo "Note: delta list has reference and version file names"
  exit
fi

for ((i=$MIN_SEED; i<=$MAX_SEED; i+=$STEP))
do
  ./do-benchmark.sh "$1 $i" $2 $3 > seed-$i.tmp 2>err-$i.tmp
done

