import sys
import os
import pandas as pd
import csv
import matplotlib.pyplot as plt


# name of the directory which contains the log files
outputDir = "outputTest"


# count the number of replicate folder
def countReplicates():
    numberReplicates = 0
    for replicateDir in os.scandir():
        if replicateDir.is_dir():
            numberReplicates = numberReplicates + 1
    return numberReplicates


# get data necessary for creating an epoch-log which contains the mean values for each position
# for each generation for each category in the log
def getData():
    print(os.getcwd())
    os.chdir(outputDir)

    # iterate over the experiment dirs and create a logfile with mean values in each of them
    for experimentDir in os.scandir():

        if experimentDir.is_dir():
            # change into the experiment folder
            os.chdir(experimentDir.name)
            isFirstReplicate = True
            replicateCount = 0
            numberOfReplicates = countReplicates()

            # iterate over all replicates in the experiment directory
            for replicateDir in os.scandir():
                if replicateDir.is_dir():
                    # change into the replicate folder
                    os.chdir(replicateDir.name)
                    replicateCount = replicateCount + 1

                    if isFirstReplicate:
                        # copy the content of the textfile into the new textfile
                        isFirstReplicate = False
                        with open("logs/epoch-log.txt") as logFile:
                            with open ("../epoch-log-mean.txt", "w") as meanLogFile:
                                meanLogFile.writelines(logFile.readlines())

                        print(os.getcwd())


                        dataFileMean = pd.read_csv("../epoch-log-mean.txt", sep=r"\s+")

                        # add the replicatename to all categories of the textfile
                        dataFileMean.columns = [dataFileMean.columns[0]] + [f"{col}{replicateDir.name}" for col in dataFileMean.columns[1:]]

                        # add another column with the mean value of all other values except generation
                        dataFileMean["mean" + replicateDir.name] = dataFileMean.iloc[:, 1:].mean(axis=1)

                        dataFileMean.to_csv("../epoch-log-mean.txt", sep=" ", index=False)
                    else:
                        dataFileMean = pd.read_csv("../epoch-log-mean.txt", sep=r"\s+")
                        dataFileCurrent = pd.read_csv("logs/epoch-log.txt", sep=r"\s+")
                        dataFileCurrent = dataFileCurrent.iloc[:, 1:]

                        # add the replicatename to all categories of the textfile
                        dataFileCurrent.columns = [f"{col}{replicateDir.name}" for col in dataFileCurrent.columns[0:]]

                        # add another column with the mean value of all other values except generation
                        dataFileCurrent["mean" + replicateDir.name] = dataFileCurrent.iloc[:, 0:].mean(axis=1)

                        ergebnis = pd.concat([dataFileMean, dataFileCurrent], axis=1)

                        ergebnis.to_csv("../epoch-log-mean.txt", sep=" ", index=False)

                    os.chdir("..")

            # plot a single line graph for an experiment
            dataFileMean = pd.read_csv("./epoch-log-mean.txt", sep=r"\s+")

            x = dataFileMean["generation"]
            y_columns = dataFileMean.columns[1:]

            plt.figure(figsize=(10,6))

            for col in y_columns:
                if col.startswith("mean"):
                    plt.plot(x, dataFileMean[col], label=f"{col}")
                else:
                    plt.plot(x, dataFileMean[col], label=f"{col}", color='grey', alpha=0.5)


            plt.xlabel("generation")
            plt.ylabel("values")
            plt.title("Graph for " + experimentDir.name)
            plt.legend(fontsize='xx-small')
            #plt.grid(True)

            plt.show()


            os.chdir("..")










def main():
    os.chdir(os.path.dirname(os.path.abspath(__file__)))
    os.chdir("..")
    getData()

if __name__ == "__main__":
    main()