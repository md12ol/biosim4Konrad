#!/usr/bin/python3

import os
from tests.pylib.config import TestConfig
from itertools import product
import shutil

# global parameters which influence the folder structure and the config files
outputDir = "outputScript"

# global parameters which influence the config files and data table
# (the functionality of each parameter is described in "biosim4.ini")
staticMicePositions = ["false"]
staticCatPositions = ["false"]
sizeX = ["128"]
sizeY = ["128"]
population = ["100", "200"]
miceRatio = ["0.5", "0.75"]
dynamicPopulation = ["true"]
stepsPerGeneration = ["300"]
maxGenerations = ["100"]
genomeInitialLengthMin = ["24"]
genomeInitialLengthMax = ["24"]
genomeMaxLength = ["300"]
maxNumberNeurons = ["5"]
killEnable = ["true"]
sexualReproduction = ["true"]
chooseParentsByFitness = ["true"]
pointMutationRate = ["0.001"]
responsivenessCurveKFactor = ["2"]
populationSensorRadius = ["2.5"]
longProbeDistance = ["16"]
shortProbeBarrierDistance = ["4"]
shortProbeSafeAreaDistance = ["4"]
shortProbeFoodAreaDistance = ["4"]
signalSensorRadius = ["2.0"]
genomeMiceTextFile = ["none"]
genomeCatsTextFile = ["none"]
challengeMice = ["20"]
challengeCats = ["19"]
foodAreaType = ["6"]
barrierType = ["0"]
safeAreaType = ["6"]
spawnMiceInSafeAreas = ["false"]
deterministic = ["false"]
RNGSeed = ["12345678"]


# modifies a single, specified line in the given config file
def modify_ini(exp_ini, key, value):
    print("Modifying", key, value)
    exp_ini.write_config_setting("DEFAULT", key, value)


# create the needed structure for a single experiment
def create_folders(index):
    if not os.path.isdir(outputDir):
        os.mkdir(outputDir)
    os.chdir(outputDir)
    os.mkdir("experiment" + str(index))
    os.chdir("experiment" + str(index))
    os.mkdir("genomes")
    os.mkdir("graphs")
    os.mkdir("heatmaps")
    os.mkdir("images")
    os.mkdir("logs")
    os.mkdir("nets")

    os.chdir("logs")
    with open("epoch-log.txt", "w"):
        pass
    with open("population-range.txt", "w") as file:
        file.write("0")
    with open("population-range-final.txt", "w") as file:
        file.write("0")

    os.chdir("..")
    os.chdir("..")
    os.chdir("..")



def create_dat_table_entry(path, index):
    if index == 1:
        with open("table.dat", "w") as datfile:
            datfile.write("./bin/Release/biosim4 " + path + "\n")
    else:
        with open("table.dat", "a") as datfile:
            datfile.write("./bin/Release/biosim4 " + path + "\n")



# use itertools to create all possible combinations of given parameter arrays, each combination being an experiment
def create_config_files():
    index = 1
    for a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z, ab, ac, ad, ae, af, ag, ah, ai in product(staticMicePositions, staticCatPositions,
    sizeX, sizeY, population, miceRatio, dynamicPopulation, stepsPerGeneration, maxGenerations, genomeInitialLengthMin, genomeInitialLengthMax,
    genomeMaxLength, maxNumberNeurons, killEnable, sexualReproduction, chooseParentsByFitness, pointMutationRate,
    responsivenessCurveKFactor, populationSensorRadius, longProbeDistance, shortProbeBarrierDistance,
    shortProbeSafeAreaDistance, shortProbeFoodAreaDistance, signalSensorRadius, genomeMiceTextFile, genomeCatsTextFile,
    challengeMice, challengeCats, foodAreaType, barrierType, safeAreaType,
    spawnMiceInSafeAreas, deterministic, RNGSeed):

        # Create the needed folder structure for the experiment
        create_folders(index)

        shutil.copyfile("biosim4.ini", "biosim4experiment.ini")

        # Create needed default section in duplicate
        with open("biosim4experiment.ini", "r", encoding="utf-8") as file:
            content = file.read()

        with open("biosim4experiment.ini", "w", encoding="utf-8") as file:
            file.write("[DEFAULT]\n" + content)

        # Load the modified configuration
        config = TestConfig("biosim4experiment.ini")

        # Modify parameters for the experiment
        config.load()
        modify_ini(config, "staticMicePositions", a)
        modify_ini(config, "staticCatPositions", b)
        modify_ini(config, "sizeX", c)
        modify_ini(config, "sizeY", d)
        modify_ini(config, "population", e)
        modify_ini(config, "miceRatio", f)
        modify_ini(config, "dynamicPopulation", g)
        modify_ini(config, "stepsPerGeneration", h)
        modify_ini(config, "maxGenerations", i)
        modify_ini(config, "genomeInitialLengthMin", j)
        modify_ini(config, "genomeInitialLengthMax", k)
        modify_ini(config, "genomeMaxLength", l)
        modify_ini(config, "maxNumberNeurons", m)
        modify_ini(config, "killEnable", n)
        modify_ini(config, "sexualReproduction", o)
        modify_ini(config, "chooseParentsByFitness", p)
        modify_ini(config, "pointMutationRate", q)
        modify_ini(config, "responsivenessCurveKFactor", r)
        modify_ini(config, "populationSensorRadius", s)
        modify_ini(config, "longProbeDistance", t)
        modify_ini(config, "shortProbeBarrierDistance", u)
        modify_ini(config, "shortProbeSafeAreaDistance", v)
        modify_ini(config, "shortProbeFoodAreaDistance", w)
        modify_ini(config, "signalSensorRadius", x)
        modify_ini(config, "genomeMiceTextFile", y)
        modify_ini(config, "genomeCatsTextFile", z)
        modify_ini(config, "challengeMice", ab)
        modify_ini(config, "challengeCats", ac)
        modify_ini(config, "foodAreaType", ad)
        modify_ini(config, "barrierType", ae)
        modify_ini(config, "safeAreaType", af)
        modify_ini(config, "spawnMiceInSafeAreas", ag)
        modify_ini(config, "deterministic", ah)
        modify_ini(config, "RNGSeed", ai)

        # Modify output and experiment dir
        modify_ini(config, "outputDir", outputDir)
        modify_ini(config, "experimentDir", "experiment" + str(index))

        # Safe the configuration
        config.save()

        # Move the file to the corresponding experiment folder
        shutil.move("biosim4experiment.ini", outputDir + "/experiment" + str(index))

        # Create table entry for specific experiment
        create_dat_table_entry('"' + outputDir + '/experiment' + str(index) + '/biosim4experiment.ini"', index)

        index = index + 1


def main():
    create_config_files()



if __name__ == '__main__':
    main()


