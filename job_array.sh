#!/bin/bash
# Using the array job feature of SLURM to submit a serial farm.
# The input file table.dat or table_postprocess.dat contains individual cases - one case per line
# A simple test table can be generated with
# for ((i=1; i<=4; i++)); do echo "sleep $(($RANDOM % 30))"; done > table.dat

# The table now will be given as commandline argument

# "sbatch job_array.sh table.dat" or "sbatch job_array.sh table_postprocess.dat" for global execution
# "sh job_array.sh table.dat" or "sh sbatch job_array.sh table_postprocess.dat" for local execution
export TABLE=$1

# Total number of cases (= number of jobs to submit):
N_cases=$(cat "$TABLE" | wc -l)

# Testing if sbatch is available in PATH
if command -v sbatch &> /dev/null; then

  # Submitting an array job to the scheduler:
  sbatch --nice=1000 --array=1-$N_cases%5 run_many.sh

else

  # Read lines sequentially and locally execute them one by one
  while IFS= read -r zeile; do
    eval $zeile
  done < $TABLE
fi