RUN=./doTest.sh
THREADS=$1

if [ -z $THREADS ]; then
	echo "Usage: ./lightTest.sh <n threads>"
	exit 0
fi

for FILE in $(ls inputs/*.txt | grep -v 512); do
	$RUN $1 $FILE
done


