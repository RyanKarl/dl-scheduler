#1/bin/bash
UTIL=0.5
RUNTIME=1000
DELTA=1000
NUMSETS=1
NUMRUNS=16
#Set this to 1 to enable looping for more results
START=1

for i in `seq $START $NUMRUNS`
do
  echo "Runtime is now $RUNTIME"
  for j in `seq 1 $NUMSETS`
  do
    echo "$j sets"
    python ./simso/test_rand.py -r $RUNTIME -u $UTIL -n $j -t FILE -s EDF > ./results/EDF/file_$i-$j.txt
    ./evaluations/eval < ./results/EDF/file_$i-$j.txt > ./results/EDF/points_file_$i-$j.txt
    python ./simso/test_rand.py -r $RUNTIME -u $UTIL -n $j -t BASE -s EDF > ./results/EDF/base_$i-$j.txt
    ./evaluations/eval < ./results/EDF/base_$i-$j.txt > ./results/EDF/points_base_$i-$j.txt
  done  
  let "RUNTIME += DELTA"
done
