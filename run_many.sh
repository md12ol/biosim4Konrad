#!/bin/bash
# Here you should provide the sbatch arguments to be used in all jobs in this serial farm
#SBATCH --time=24:00:00
#SBATCH --mem=2G
#SBATCH --account=bunzenth
#SBATCH --cpus-per-task=4
#SBATCH --partition=all
#SBATCH --output=/home/bunzenth/biosim4Konrad/joboutput/%x_%j.out
#SBATCH --error=/home/bunzenth/biosim4Konrad/joboutput/%x_%j.err

# Case number is given by $SLURM_ARRAY_TASK_ID environment variable:
i=$SLURM_ARRAY_TASK_ID

# Extracing the $i-th line from file $TABLE:
LINE=`sed -n ${i}p "$TABLE"`
# Echoing the command (optional), with the case number prepended:
echo "$i; $LINE"
# Executing the command:
eval "$LINE"