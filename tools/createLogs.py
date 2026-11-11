import sys
import os
import pandas as pd
import csv
import matplotlib.pyplot as plt

# name of the directory which contains the log files
outputDir = "outputTest"


# get data necessary for creating an epoch-log which contains the mean values for each position
# for each generation for each category in the log
def getData(logName):

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

                        if logName == 'Survivors':
                            dataSurvivors = dataFileCurrent[['generation', 'Survivors']].rename(columns={'Survivors': 'Survivors' + replicateDir.name})
                            dataSurvivorsBoth = dataFileCurrent[['generation', 'SurvivedMice', 'SurvivedCats']].rename(columns={'SurvivedMice' : 'SurvivedMice' + replicateDir.name, 'SurvivedCats' : 'SurvivedCats' + replicateDir.name})
                            dataSurvivors.to_csv("../epoch-log-survivors.txt", sep = " ", index=False)
                            dataSurvivorsBoth.to_csv("../epoch-log-survivorsboth.txt", sep = " ", index=False)
                        elif logName == 'Diversity':
                            dataDiversity = dataFileCurrent[['generation', 'Diversity']].rename(columns={'Diversity': 'Diversity' + replicateDir.name})
                            dataDiversityBoth = dataFileCurrent[['generation', 'DiversityMice', 'DiversityCats']].rename(columns={'DiversityMice' : 'DiversityMice' + replicateDir.name, 'DiversityCats' : 'DiversityCats' + replicateDir.name})
                            dataDiversity.to_csv("../epoch-log-diversity.txt", sep = " ", index=False)
                            dataDiversityBoth.to_csv("../epoch-log-diversityboth.txt", sep = " ", index=False)
                        else:
                            data = dataFileCurrent[['generation', logName]].rename(columns={logName: logName + replicateDir.name})
                            data.to_csv("../epoch-log-" + logName + ".txt", sep =" ", index=False)

                    else:
                        dataFileCurrent = pd.read_csv("logs/epoch-log.txt", sep=r"\s+")
                        if logName == 'Survivors':
                            dataFileCurrentSurvivors = dataFileCurrent[['Survivors']].rename(columns={'Survivors': 'Survivors' + replicateDir.name})
                            dataFileCurrentSurvivorsBoth = dataFileCurrent[['SurvivedMice', 'SurvivedCats']].rename(columns={'SurvivedMice': 'SurvivedMice' + replicateDir.name, 'SurvivedCats' : 'SurvivedCats' + replicateDir.name})
                            dataSurvivors = pd.read_csv("../epoch-log-survivors.txt", sep=r"\s+")
                            dataSurvivorsBoth = pd.read_csv("../epoch-log-survivorsboth.txt", sep=r"\s+")
                            ergebnisSurvivors = pd.concat([dataSurvivors, dataFileCurrentSurvivors], axis=1)
                            ergebnisSurvivorsBoth = pd.concat([dataSurvivorsBoth, dataFileCurrentSurvivorsBoth], axis=1)

                            ergebnisSurvivors.to_csv("../epoch-log-survivors.txt", sep=" ", index=False)
                            ergebnisSurvivorsBoth.to_csv("../epoch-log-survivorsboth.txt", sep=" ", index=False)
                        elif logName == 'Diversity':
                            dataFileCurrentDiversity = dataFileCurrent[['Diversity']].rename(columns={'Diversity': 'Diversity' + replicateDir.name})
                            dataFileCurrentDiversityBoth = dataFileCurrent[['DiversityMice', 'DiversityCats']].rename(columns={'DiversityMice': 'DiversityMice' + replicateDir.name, 'DiversityCats' : 'DiversityCats' + replicateDir.name})
                            dataDiversity = pd.read_csv("../epoch-log-diversity.txt", sep=r"\s+")
                            dataDiversityBoth = pd.read_csv("../epoch-log-diversityboth.txt", sep=r"\s+")
                            ergebnisSurvivors = pd.concat([dataDiversity, dataFileCurrentDiversity], axis=1)
                            ergebnisSurvivorsBoth = pd.concat([dataDiversityBoth, dataFileCurrentDiversityBoth], axis=1)
                            ergebnisSurvivors.to_csv("../epoch-log-diversity.txt", sep=" ", index=False)
                            ergebnisSurvivorsBoth.to_csv("../epoch-log-diversityboth.txt", sep=" ", index=False)
                        else:
                            dataFileCurrentData = dataFileCurrent[[logName]].rename(columns={logName: logName + replicateDir.name})
                            dataFileData = pd.read_csv("../epoch-log-" + logName + ".txt", sep=r"\s+")
                            ergebnis = pd.concat([dataFileData, dataFileCurrentData], axis=1)
                            ergebnis.to_csv("../epoch-log-" + logName + ".txt", sep=" ", index=False)

                    os.chdir("..")

            # plot a single line graph for an experiment
            if logName == 'Survivors':

                # add the missing mean values
                dataFileSurvivors = pd.read_csv("./epoch-log-survivors.txt", sep=r"\s+")
                dataFileSurvivorsBoth = pd.read_csv("./epoch-log-survivorsboth.txt", sep=r"\s+")
                dataFileSurvivors["mean"] = dataFileSurvivors.iloc[:, 1:].mean(axis=1)
                dataFileSurvivorsBoth["meanMice"] = dataFileSurvivorsBoth.filter(regex='^SurvivedMice').mean(axis=1)
                dataFileSurvivorsBoth["meanCats"] = dataFileSurvivorsBoth.filter(regex='^SurvivedCats').mean(axis=1)

                # then create two plots, one for general number of survivors and one for mice and cats
                x_Survivors = dataFileSurvivors["generation"]
                x_SurvivorsBoth = dataFileSurvivorsBoth["generation"]
                y_Diversity = dataFileSurvivors.columns[1:]
                y_SurvivorsBoth = dataFileSurvivorsBoth.columns[1:]

                # create two plots sequentially
                plt.figure(figsize=(10,6))
                for col in y_Diversity:
                    if(col.startswith("mean")):
                        plt.plot(x_Survivors, dataFileSurvivors[col], label=f"{col}")
                    else:
                        plt.plot(x_Survivors, dataFileSurvivors[col], label=f"{col}", color="grey")
                plt.xlabel("generation")
                plt.ylabel("values")
                plt.title("Survivors for " + experimentDir.name)
                plt.legend(fontsize='xx-small')
                plt.savefig(logName + ".png")
                #plt.show()
                plt.close()

                plt.figure(figsize=(10,6))
                for col in y_SurvivorsBoth:
                    if(col.startswith("mean")):
                        plt.plot(x_SurvivorsBoth, dataFileSurvivorsBoth[col], label=f"{col}")
                    elif(col.startswith("SurvivedMice")):
                        plt.plot(x_SurvivorsBoth, dataFileSurvivorsBoth[col], label=f"{col}", color="grey", alpha=0.5)
                    else:
                        plt.plot(x_SurvivorsBoth, dataFileSurvivorsBoth[col], label=f"{col}", color="grey")
                plt.xlabel("generation")
                plt.ylabel("values")
                plt.title("Survivors for " + experimentDir.name)
                plt.legend(fontsize='xx-small')
                plt.savefig(logName + "both.png")
                #plt.show()
                plt.close()

            elif logName == 'Diversity':

                # add the missing mean values
                dataFileDiversity = pd.read_csv("./epoch-log-diversity.txt", sep=r"\s+")
                dataFileDiversityBoth = pd.read_csv("./epoch-log-diversityboth.txt", sep=r"\s+")
                dataFileDiversity["mean"] = dataFileDiversity.iloc[:, 1:].mean(axis=1)
                dataFileDiversityBoth["meanMice"] = dataFileDiversityBoth.filter(regex='^DiversityMice').mean(axis=1)
                dataFileDiversityBoth["meanCats"] = dataFileDiversityBoth.filter(regex='^DiversityCats').mean(axis=1)

                # then create two plots, one for general number of survivors and one for mice and cats
                x_Diversity = dataFileDiversity["generation"]
                x_DiversityBoth = dataFileDiversityBoth["generation"]
                y_Diversity = dataFileDiversity.columns[1:]
                y_DiversityBoth = dataFileDiversityBoth.columns[1:]

                # create two plots sequentially
                plt.figure(figsize=(10,6))
                for col in y_Diversity:
                    if(col.startswith("mean")):
                        plt.plot(x_Diversity, dataFileDiversity[col], label=f"{col}")
                    else:
                        plt.plot(x_Diversity, dataFileDiversity[col], label=f"{col}", color="grey")
                plt.xlabel("generation")
                plt.ylabel("values")
                plt.title("Diversity for " + experimentDir.name)
                plt.legend(fontsize='xx-small')
                plt.savefig(logName + ".png")
                #plt.show()
                plt.close()

                plt.figure(figsize=(10,6))
                for col in y_DiversityBoth:
                    if(col.startswith("mean")):
                        plt.plot(x_DiversityBoth, dataFileDiversityBoth[col], label=f"{col}")
                    elif(col.startswith("DiversityMice")):
                        plt.plot(x_DiversityBoth, dataFileDiversityBoth[col], label=f"{col}", color="grey", alpha=0.5)
                    else:
                        plt.plot(x_DiversityBoth, dataFileDiversityBoth[col], label=f"{col}", color="grey")
                plt.xlabel("generation")
                plt.ylabel("values")
                plt.title("Diversity for " + experimentDir.name)
                plt.legend(fontsize='xx-small')
                plt.savefig(logName + "both.png")
                #plt.show()
                plt.close()

            else:
                # add the missing mean values
                dataFile = pd.read_csv('./epoch-log-' + logName + '.txt', sep=r"\s+")
                dataFile['mean'] = dataFile.iloc[:, 1:].mean(axis=1)

                x = dataFile['generation']
                y_columns = dataFile.columns[1:]

                plt.figure(figsize=(10,6))
                for col in y_columns:
                    if(col.startswith("mean")):
                        plt.plot(x, dataFile[col], label=f"{col}")
                    else:
                        plt.plot(x, dataFile[col], label=f"{col}", color="grey")
                plt.xlabel("generation")
                plt.ylabel("values")
                plt.title("Values for " + logName + " (" + experimentDir.name + ")")
                plt.legend(fontsize='xx-small')
                plt.savefig(logName + ".png")
                #plt.show()
                plt.close()

            os.chdir("..") # change into experiment folder
    os.chdir("..") # change into biosim folder


def main(argv):
    os.chdir(os.path.dirname(os.path.abspath(__file__)))
    os.chdir("..")

    for logName in argv:
        if argv.index(logName) > 0:
            getData(logName)

if __name__ == "__main__":
    main(sys.argv)