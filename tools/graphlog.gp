#!/usr/bin/gnuplot --persist

# Requires a text file named "epoch-log.txt" in the log directory

set term png size 2000, 400
set output "./images/log.png"

# Left Y axis gets scaled to the max survivors.
# Right Y axis gets scaled to 0..255.
#   1:2 Survivors 0..N        => 0..N
#   1:3 Genome length 0..50   => 0..255
#   1:4 Diversity 0..0.7      => 0..255
#   1:5 Anxiety 0..255        => 0..255

# Get the maximum number of population from the textfile
XMAX = system("cat ./logs/population-range.txt")

set mxtics
set ytics autofreq nomirror tc lt 2
set yrange [ 0:XMAX/4 ]
set y2range [ 0:1 ]
set y2tics autofreq nomirror tc lt 1
set grid
set key lmargin

ScaleSurvivors(s) = s
ScaleGenomeLength(y) = y*2
ScaleDiversity(d) = d
ScaleDiversityMice(dm) = dm
ScaleDiversityCats(dc) = dc
ScaleSurvivedMice(sm) = sm
ScaleSurvivedCats(sc) = sc
#ScaleMurders(m) = m

plot "./logs/epoch-log.txt" \
       using 1:(column("Survivors")) with lines lw 2 linecolor 2 title "Survivors" axes x1y1, \
    "" using 1:(column("Diversity")) with lines lw 2 linecolor 1 title "Diversity" axes x1y2, \
    "" using 1:(column("DiversityMice")) with lines lw 2 linecolor 5 title "DiversityMice" axes x1y2, \
    "" using 1:(column("DiversityCats")) with lines lw 2 linecolor 6 title "DiversityCats" axes x1y2, \
    "" using 1:(column("SurvivedMice")) with lines lw 2 linecolor 3 title "SurvivedMice" axes x1y1, \
    "" using 1:(column("SurvivedCats")) with lines lw 2 linecolor 4 title "SurvivedCats" axes x1y1

