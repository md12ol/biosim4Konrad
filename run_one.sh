#!/bin/bash
# Here you should provide the sbatch arguments to be used in all jobs in this serial farm
#SBATCH --time=24:00:00
#SBATCH --mem=2G
#SBATCH --account=bunzenth
#SBATCH --cpus-per-task=4
#SBATCH --partition=all
#SBATCH --output=/home/bunzenth/biosim4Konrad/joboutput/%x_%j.out
#SBATCH --error=/home/bunzenth/biosim4Konrad/joboutput/%x_%j.err

# First save the config file in a variable
CONFIG_FILE=$1

# If a seed was given change the rngseed of the configfile
if [ "$#" -ge 2 ]; then
  RNGSeed=$2
  sed -i "s/^rngseed[[:space:]]*=.*/rngseed = $RNGSeed/" $CONFIG_FILE
fi

# Lastly execute the simulation with arguments
eval "./bin/Release/biosim4 $CONFIG_FILE"
