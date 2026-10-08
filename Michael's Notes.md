# Michael's Notes

## Framing the Work

A biosim4 fork in which two species co-evolve:
- **Cats** (predators) must kill to reproduce.
- **Mice** (prey) must eat from food tiles and reach goal areas, and can use refuges that cats cannot enter.
- Both species are controlled by small neural networks encoded in their genomes.

Paper-worthy aspects:
- Predator–prey coevolution with species-specific sensors and actions (only cats can express the kill neuron)
- How refuges and food layout affect the strategies that evolve
- Population dynamics over 100,000+ generations
- How the networks' wiring changes over evolutionary time, tracked with the connection heatmaps

## Candidate Venues

Confirm 2027 deadlines and locations on each venue's site.

### Conferences
1. **ALIFE** (International Conference on Artificial Life, run by ISAL): the most natural fit; open-access proceedings through MIT Press.
2. **GECCO**: Complex Systems (Artificial Life) track or Neuroevolution track. Competitive; reviewers expect solid statistics across replicates.
3. **EvoStar / EvoApplications**: European and friendly to student work; has a track for applying evolution to biological and ecological modelling.
4. **IEEE SSCI** (ALIFE symposium) or **IEEE CEC**: good if you want an IEEE venue.

### Journals
- **Artificial Life** (MIT Press): the top journal in the field.
- **Adaptive Behavior** (SAGE): suits a focus on evolved behaviour (refuge use, pursuit and evasion).
- **Journal of the Royal Society Interface**: fits a biological framing ("what ecological conditions make refuge use evolve?").
- **Ecological Modelling** or **JASSS**: fit if the population dynamics are the main result.
- **BioSystems**: a reasonable mid-tier fallback.

## Related Papers

- **Main reading for the student:** Olson, Hintze, Dyer, Knoester & Adami (2013), "Predator confusion is sufficient to evolve swarming behaviour," *Journal of the Royal Society Interface* 10(85): 20130305. https://doi.org/10.1098/rsif.2013.0305 (free preprint: https://arxiv.org/abs/1209.3330)
  - Predators and prey in a 2D world, each controlled by an evolved controller encoded in its genome (Markov Brains, i.e. evolved logic-gate networks).
  - Shows how to argue "factor X is sufficient to evolve behaviour Y," the same argument this project would make about refuges or food placement.
- Olson, Hintze, Dyer, Moore & Adami (2016), "Exploring the coevolution of predator and prey morphology and behavior," ALIFE 2016 proceedings. https://arxiv.org/abs/1602.08802 (an example of a paper at the ALIFE conference)
- Gras, Devaurs, Wozniak & Aspinall (2009), EcoSim, *Artificial Life*: a large predator–prey simulation where individuals have food, energy and evolved behaviour.
- Li, Li & Zhao (2023), "Predator–prey survival pressure is sufficient to evolve swarming behaviors," *New Journal of Physics*. https://arxiv.org/abs/2308.12624 (the same kind of result with reinforcement learning instead of evolution; useful contrast for related work)

## What We Need for a Paper

We need one clear research question with evidence behind it; the simulator is the tool, not the result.
Test: we should be able to fill in "We show that ___ causes ___ to evolve."

### 1. A central question (main contribution)
Pick one ecological variable and show how it shapes what evolves:
- **Refuges:** Do safe areas cause mice to evolve hiding? Does that hold cats back, or push them to wait outside the refuges? Compare no refuges, few, and many.
- **Food layout:** Does clustered versus scattered food (fixed or random) change mouse movement and how often mice get caught?
- **Mouse-to-cat ratio / population size that changes during the run:** What keeps both species alive, and when does the system collapse or swing back and forth?

Note: the currently planned sweep (static positions, max neurons, mutation rate) is mostly tuning settings. That's useful for choosing parameters, but reviewers won't see it as a contribution. Make one ecological factor the experimental axis and fix the tuning settings at sensible values.

### 2. Controlled experiments with statistics
- 10 or more independent runs per condition; 20–30 is better for GECCO or the ALIFE conference.
- A baseline condition (e.g., no refuges, or a "cats can't kill" control).
- Results over evolutionary time (mean with confidence bands) and at the end of each run (box plots).
- Significance tests (Mann–Whitney U or Kruskal–Wallis) with a correction for multiple comparisons.

### 3. Evidence of an arms race between the species
- **Cross-generation tests:** pit cats from generation *i* against mice from generation *j* (supported by the Mice Testing Code, which loads saved genomes from files). If later cats beat earlier mice and vice versa, that shows real adaptation.
- **Single-species controls:** evolve one species against a fixed opponent and compare.

### 4. Behavioural analysis: what actually evolved
Describe the evolved strategies and back each one with a measurement:
- Mice: time spent in refuges, distance kept from cats, time on food.
- Cats: pursuit versus ambush (waiting near refuges or food), kills per cat.
- A few representative snapshots or frames.

### 5. Network analysis
Use the connection heatmaps: which sensors and actions get wired together as evolution proceeds, and whether that differs between cats and mice or between conditions (e.g., "mice with refuges evolve strong links from the refuge sensor to movement"). Especially convincing if a wiring change is linked to a behaviour change.

### 6. Reproducibility
Release the code and configuration files.

### How much is enough
- **Conference paper (ALIFE or EvoApplications, about 8 pages):** items 1, 2, and 4 for one ecological factor, plus a bit of 5.
- **GECCO or a journal:** add item 3, two ecological factors or their interaction, and a fuller network analysis.

**Next step:** decide the one-sentence claim, since it determines which experiments go into the next cluster batch.
