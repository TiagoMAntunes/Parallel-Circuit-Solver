RUN=./doTest.sh
THREADS=$1

if [ -z $THREADS ]; then
	echo "Usage: ./heavyTest.sh <n threads>"
	exit 0
fi

for FILE in $(ls heavyInputs/*.txt); do
	$RUN $1 $FILE
done


