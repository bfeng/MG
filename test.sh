/!/bin/bash

for sleeptime in 0 1 10
do
  for number_of_global_size in 48 96 336 432
  do
    echo time ./MG $number_of_global_size 48 1 $sleeptime
  done
done

