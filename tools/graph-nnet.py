#!/usr/bin/python3

import igraph
import argparse

# create parser to access arguments
parser = argparse.ArgumentParser(description="Read in input file and output location.")

parser.add_argument("netDir", type=str, help="Path to the directory, from which file is taken.")
parser.add_argument("netFilename", type=str, help="Name of the file containing the connections.")
parser.add_argument("outputDir", type=str, help="Name of the directory, where the svg is stored.")

args = parser.parse_args()

# load data into a graph
g = igraph.Graph.Read_Ncol(f"./{args.netDir}/{args.netFilename}", names=True, weights=True)

for v in g.vs:
	v['size'] = 35
	v['label'] = v['name']
	if v['name'] in ['Lx', 'Ly', 'EDx', 'EDy', 'ED', 'Bfd', 'Blr', 'Gen', 'LMx', 'LMy', 'LPf', 'LPb', 'Pop', 'Pfd', 'Plr', 'Osc', 'Age', 'Rnd', 'Sg', 'Sfd', 'Slr', 'LPfi', 'LPbi', 'LPs', 'LPsi', 'LPfo', 'LPfoi', 'LPm', 'LPmi', 'LPc', 'LPci', 'Bfdi', 'Blri', 'SAfd', 'SAfdi', 'SAlr', 'SAlri', 'FOfd', 'FOfdi', 'FOlr', 'FOlri', 'Popm', 'Popc', 'Pfdi', 'Pfdm', 'Pfdmi', 'Pfdc', 'Pfdci', 'Plri', 'Plrm', 'Plrmi', 'Plrc', 'Plrci']:
		v['color'] = 'lightblue'
	elif v['name'] in ['MvX', 'MvY', 'MvE', 'MvW', 'MvN', 'MvS', 'Mfd', 'MvL', 'MvR', 'MRL', 'Mrv', 'Mrn', 'OSC', 'LPD', 'Res', 'SG', 'Klf' ]:
		v['color'] = 'lightpink'
	else:
		v['color'] = 'lightgrey'

# convert edge weights to color and size
for e in g.es:
	#print(e['weight'])
	if e['weight'] < 0:
		e['color'] = 'lightcoral'
	elif e['weight'] == 0:
		e['color'] = 'grey'
	else:
		e['color'] = 'green'

	width = abs(e['weight'])
	e['width'] = 1 + 1.25 * (width / 8192.0)


# plot graph

print(len(g.vs))

if len(g.vs) < 6:
    bbox = (300,300)
    layout = 'fruchterman_reingold'
elif len(g.vs) < 12:
    bbox = (400,400)
    layout = 'fruchterman_reingold'
elif len(g.vs) < 18:
    bbox = (500,500)
    layout = 'fruchterman_reingold'
elif len(g.vs) < 24:
    bbox = (520,520)
    layout = 'fruchterman_reingold'
elif len(g.vs) < 26:
    bbox = (800,800)
    layout = 'fruchterman_reingold'
elif len(g.vs) < 50:
    bbox = (1000,1000)
    layout = 'fruchterman_reingold'
elif len(g.vs) < 130:
    bbox = (1200,1000)
    layout = 'fruchterman_reingold'
elif len(g.vs) < 150:
    bbox = (4000,4000)
    layout = 'fruchterman_reingold'
    for v in g.vs:
        v['size'] = v['size'] * 1.5
elif len(g.vs) < 200:
    bbox = (4000,4000)
    layout = 'kamada_kawai'
    for v in g.vs:
        v['size'] = v['size'] * 2
else:
    bbox = (8000,8000)
    layout = 'fruchterman_reingold'

outputFilename = args.netFilename.split(".txt")[0]
igraph.plot(g, f"./{args.outputDir}" + outputFilename + ".svg", edge_curved=True, bbox=bbox, margin=64, layout=layout)


