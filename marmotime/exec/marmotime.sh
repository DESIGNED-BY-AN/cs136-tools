#!/bin/bash

if [ $# -ne 5 ]; then
  echo ""
  echo "+-----------------------------------------------------------------------------------------------------------------------------------+"
  echo "+ Usage	               : ./marmotime.sh [main_file] [student_submission] [optimal_solution] [input_file] [leniency]"
  echo "+-----------------------------------------------------------------------------------------------------------------------------------+"
  echo "+ Purpose              : check if the student_submission can match the optimal_solution in execution speed"
  echo "+                        exit with code 0 if true, and non-zero otherwise"
  echo "+-----------------------------------------------------------------------------------------------------------------------------------+"
  echo "+ Restriction          : does not check for output accuracy"
  echo "+-----------------------------------------------------------------------------------------------------------------------------------+"
  echo "+ Parameters           [main_file]          : the name of the file containing the runner main function, put -e if it doesn't exist"
  echo "+                      [student_submission] : the name of the file containing the student's submission"
  echo "+                      [optimal_solution]   : the name of the file containing the professor's solution"
  echo "+                      [input_file]         : the name of the file containing the standard input" 
  echo "+                      [leniency]           : the higher the more lenient (min. 10)"
  echo "+-----------------------------------------------------------------------------------------------------------------------------------+"
  echo "+ Author               : An Le (Spring 2018)" 
  echo "+-----------------------------------------------------------------------------------------------------------------------------------+"
  echo ""
  exit 1;
fi

#==============================================================================
# > ENVIRONMENT VARIABLES
#==============================================================================
DEBUG=0 #set to 1 to see the result visually (0 - GOOD, 1 - BAD), 2 to see all the error messages
LENIENCY=$5 #allows the student_solution to be $LENIENCY times slower than the optimal_solution, 
	    #do not set to anything lower than 10
if [ $LENIENCY -lt 10 ]; then
  LENIENCY=10
fi
CODE=0 #the all-important return code, assumed innocent until proven guilty
       #also protective feature, if this code errors out, the student will get some "free" marks
NO_MAIN=0
ERR_ADDR="/dev/null"
SOLUTION=$3.c

FIRST_WRAP=$1
SECOND_WRAP=$1
FIRST_MOVE=$2
SECOND_MOVE=$3.c
#==============================================================================
# > HELPER FUNCTIONS
#==============================================================================
echoerr() { echo "$@" 1>&2; }

# $1 - PID, $2 - input, $3 - err_addr
execute() {
  ls > file_list 2>$3
  while read -r line
  do
    name="$line"
    length=${#name}
    let "index = $length - 2"
    ext=${name:$index:2}
    if [ "$ext" = ".c" ]; then
      gcc -c $name -o ${name:0:$index}.o 1>&2 2>$3
    fi
  done < file_list
  gcc *.o -o $1
  chmod u+x $1 1>&2 2>$3
  ./$1 <$2 1>$3 2>$1.err
  rm file_list
}

# $1 - PID, $2 - err_addr
cleanup(){
  rm $1* 1>&2 2>$2
  rm *.o 1>&2 2>$2
}

#==============================================================================
# > MAIN LOGIC
#==============================================================================
mv $3 $3.c

if [ $DEBUG -eq 2 ]; then
  ERR_ADDR=$$.log
fi
if [ "$1" = "-e" ]; then
  NO_MAIN=1
fi

if [ $NO_MAIN -eq 1 ]; then
  FIRST_WRAP=$3.c
  SECOND_WRAP=$2
fi

./xchrono $FIRST_WRAP $$.c 1>&2 2>${ERR_ADDR}
mv $FIRST_WRAP $$x1 1>&2 2>${ERR_ADDR}
mv $FIRST_MOVE $$x2 1>&2 2>${ERR_ADDR}

execute $$ $4 $ERR_ADDR
LINE=$(grep "marmoset_time:" $$.err)
TIME=${LINE:14}

mv $$x1 $FIRST_WRAP 1>&2 2>${ERR_ADDR}
mv $$x2 $FIRST_MOVE 1>&2 2>${ERR_ADDR}
cleanup $$ $ERR_ADDR

let "EXTENDED_TIME = $TIME * $LENIENCY"
./chrono $SECOND_WRAP $$.c $EXTENDED_TIME 1>&2 2>${ERR_ADDR}
mv $SECOND_WRAP $$x3 1>&2 2>${ERR_ADDR}
mv $SECOND_MOVE $$x4 1>&2 2>${ERR_ADDR}

execute $$ $4 $ERR_ADDR
grep "timeout" $$.err > $$.errx 2>${ERR_ADDR}
if [ -s $$.errx ]; then
  CODE=1  
fi

mv $$x3 $SECOND_WRAP 1>&2 2>${ERR_ADDR}
mv $$x4 $SECOND_MOVE 1>&2 2>${ERR_ADDR}
cleanup $$ $ERR_ADDR

if [ $CODE -eq 0 ] && [ $DEBUG -eq 1 ]; then
  echoerr "GOOD"
elif [ $DEBUG -eq 1 ]; then
  echoerr "BAD" 
fi

if [ $DEBUG -eq 2 ] && [ -s $$.errx ]; then
  echoerr "ERROR LOG: "
  echoerr ""
  cat ${ERR_ADDR} 1>&2
  rm ${ERR_ADDR}
fi

exit $CODE
