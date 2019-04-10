#!/usr/bin/ksh

# This script will kill all the children of a pid

if (( $# < 1 )); then
  echo "killpidtree <-self> pid1 pid2 ... will kill all the child processes of each pid"
  exit -1
fi

killself=0

if [[ $1 = "-self" ]]; then
  killself=1
  shift
fi

while (( $# ))
do
  pids=`ps -ef | awk '{if ($3 == '$1') print $2}'`

  if [[ -z $pids ]]; then
    echo "no children of $1 found"
  else
    echo "killing pids: $pids"
    kill -9 $pids
  fi
  if (( killself )); then
    echo "killing parent: $1"
    kill -9 $1
  fi
    
  shift
done
