import sys
import os
import pandas as pd
import csv
import matplotlib.pyplot as plt

# create the plots for the corresponding logName
def createPlots(outputDir, logName):

    os.chdir(outputDir)

    # iterate over the experiment dirs and create a logfile with mean values in each of them
    for experimentDir in os.scandir():

        if experimentDir.is_dir():
            # change into the experiment folder
            os.chdir(experimentDir.name)
            isFirstReplicate = True

            # iterate over all replicates in the experiment directory
            for replicateDir in os.scandir():
                if replicateDir.is_dir():
                    # change into the replicate folder
                    os.chdir(replicateDir.name)

                    if isFirstReplicate:
                        # copy the content of the textfile into the new textfile
                        isFirstReplicate = False
                        dataFileCurrent = pd.read_csv("logs/epoch-log.txt", sep=r"\s+")
                        data = dataFileCurrent[['generation', logName]].rename(columns={logName: logName + replicateDir.name})
                        dataBoth = dataFileCurrent[['generation', logName + 'Mice', logName + 'Cats']].rename(columns={logName + 'Mice' : logName + 'Mice' + replicateDir.name, logName + 'Cats' : logName + 'Cats' + replicateDir.name})
                        data.to_csv("../epoch-log-" + logName + ".txt", sep = " ", index=False)
                        dataBoth.to_csv("../epoch-log-" + logName + "both.txt", sep = " ", index=False)

                    else:
                        dataFileCurrent = pd.read_csv("logs/epoch-log.txt", sep=r"\s+")
                        dataFileCurrentValues = dataFileCurrent[[logName]].rename(columns={logName : logName + replicateDir.name})
                        dataFileCurrentValuesBoth = dataFileCurrent[[logName + 'Mice', logName + 'Cats']].rename(columns={logName + 'Mice' : logName + 'Mice' + replicateDir.name, logName + 'Cats' : logName + 'Cats' + replicateDir.name})
                        data = pd.read_csv("../epoch-log-" + logName + ".txt", sep=r"\s+")
                        dataBoth = pd.read_csv("../epoch-log-" + logName + "both.txt", sep=r"\s+")
                        ergebnis = pd.concat([data, dataFileCurrentValues], axis=1)
                        ergebnisBoth = pd.concat([dataBoth, dataFileCurrentValuesBoth], axis=1)
                        ergebnis.to_csv("../epoch-log-" + logName + ".txt", sep=" ", index=False)
                        ergebnisBoth.to_csv("../epoch-log-" + logName + "both.txt", sep=" ", index=False)

                    os.chdir("..")

            # plot a single line graph for an experiment

            # add the missing mean values
            dataFile = pd.read_csv("./epoch-log-" + logName + ".txt", sep=r"\s+")
            dataFileBoth = pd.read_csv("./epoch-log-" + logName + "both.txt", sep=r"\s+")
            dataFile["mean"] = dataFile.iloc[:, 1:].mean(axis=1)
            dataFileBoth["meanMice"] = dataFileBoth.filter(regex="^" + logName + "Mice").mean(axis=1)
            dataFileBoth["meanCats"] = dataFileBoth.filter(regex="^" + logName + "Cats").mean(axis=1)

            # then create two plots, one for general number and one for mice and cats
            x = dataFile["generation"]
            x_both = dataFileBoth["generation"]
            y_columns = dataFile.columns[1:]
            y_columns_both = dataFileBoth.columns[1:]

            # create two plots sequentially
            plt.figure(figsize=(10,6))
            for col in y_columns:
                if col.startswith("mean"):
                    plt.plot(x, dataFile[col], label=f"{col}")
                else:
                    plt.plot(x, dataFile[col], label=f"{col}", color="grey")
            plt.xlabel("generation")
            plt.ylabel("values")
            plt.title(logName + " for " + experimentDir.name)
            plt.legend()
            plt.savefig(logName + ".png")
            # plt.show()
            plt.close()


            plt.figure(figsize=(10, 6))
            for col in y_columns_both:
                if col.startswith("mean"):
                    plt.plot(x_both, dataFileBoth[col], label=f"{col}")
                elif col.startswith(logName + "Mice"):
                    plt.plot(x_both, dataFileBoth[col], label=f"{col}", color="grey", alpha=0.5)
                else:
                    plt.plot(x_both, dataFileBoth[col], label=f"{col}", color="grey")

            plt.xlabel("generation")
            plt.ylabel("values")
            plt.title(logName + " for " + experimentDir.name)
            plt.legend()
            plt.savefig(logName + "both.png")
            #plt.show()
            plt.close()

            os.chdir("..") # change into experiment folder
    os.chdir("..") # change into biosim folder


def main(argv):
    os.chdir(os.path.dirname(os.path.abspath(__file__)))
    os.chdir("..")

    # name of the directory which contains the log files
    outputDir = "out"

    for logName in argv:
        if argv.index(logName) == 1:
            outputDir = logName
        if argv.index(logName) > 1:
            createPlots(outputDir, logName)

if __name__ == "__main__":
    main(sys.argv)