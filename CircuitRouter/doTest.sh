#!/bin/bash
SEQ=CircuitRouter-SeqSolver/CircuitRouter-SeqSolver
PAR=CircuitRouter-ParSolver/CircuitRouter-ParSolver
THREADS=$1
FILENAME=$2
OUTFILE=${FILENAME}.speedups.csv

if [ -z $FILENAME ]; then
    echo "Invalid arguments."
    echo "Usage: ./doTest.sh <max threads> <inputfile>"
    exit 0
fi

if [ ! -f $SEQ ] || [ ! -f $PAR ]; then
	echo "Please run make command first"
	exit 0
fi

$SEQ $FILENAME
SEQTIME=$(cat ${FILENAME}.res | grep -P "time" | grep -o -P "\d+.\d+")

echo \#threads,exec_time,speedup > $OUTFILE
echo 1S,${SEQTIME},$(echo ${SEQTIME}/${SEQTIME} | bc) >> $OUTFILE

for i in $(seq 1 ${THREADS});
do
    $(${PAR} -t $i ${FILENAME})
    TIME=$(cat ${FILENAME}.res | grep -P "time" | grep -o -P "\d+.\d+")
    echo $i,${TIME},$(echo "scale=6; ${SEQTIME}/${TIME}" | bc) >> $OUTFILE
done

if [ ! -d results ]; then
	mkdir results
fi

mv $FILENAME.speedups.csv results/
