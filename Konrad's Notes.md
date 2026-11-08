# Konrad's Notes

## Predator Confusion is sufficient to evolve swarming behaviour

### What problem or question is this paper trying to answer, and why does it matter?

The paper tries to answer the question, how significant the selective force of predator confusion
for the formation of swarms is. Previous works have already provided insight into swarming behaviour
but most have not isolated the evolutionary pressures which favor the formation of swarms and
none have explored the coevolution of predator and prey behaviour. The paper also explores, how
the evolutionary effect of predator confusion on prey in turn exerts pressure on the predators visual
field, so that they in turn adapt to detect prey in swarms. Finally, the paper shows the changes of
the functional response curve of consumption rate when the swarm density increases. All in all the paper
tries to prove, that the concept of predator confusion has significant influence on the ecological
interaction between predators and prey. The paper is does matter, because the understanding how pervasive 
mechanisms such as predator confusion affect functional response relationships is important in order to 
model the dynamics of predator-prey interactions over ecological and evolutionary time.

### How does the simulation work (the world, the predator, the prey, what they can sense and do, evolution)?


#### The world
The world is a two-dimensional virtual environment, consisting of agents (predator and prey).
Agents decisions are based on a Markov Network (stochastic state machine, which makes control decisions
based sensors and internal states). Certain predator and prey behaviour is constrained (relative movement speed,
turning agility and for predators maximal consumption rate) modelling the differences between predators and prey
observed in nature. Size is 512 x 512 units.


#### The predator
Predator agents can detect prey but no other predators. Their visual field allows them to detect a distance
of 200 virtual meters in front of them. Predators have a rudimentary pre-evolved Markov Network, which 
allows them to track prey. 4 actions (stay still, move forward, turn right 6 degrees and move forward,
turn left 6 degrees and move forward). Automatically attacks prey within 5 meters, consuming it if 
the attack is successful. One possible attack within 10 simulation steps (handling time). Capture
success attack decreased by predator confusion, caused by more prey agents within 30 meters
the predators visual field.


#### The prey
Prey agents can both detect predators and prey. Their visual field allows them to detect a distance
of 100 virtual meters in front of them. Preys Markov Networks are randomly generated based on seeds.
4 actions (stay still, move forward, turn right 8 degrees and move forward,
turn left 8 degrees and move forward).


#### Both
Both predator and prey agents share the same Markov Networks, allowing them a visual field of 180 degrees.
They also have the same moving options (forward, left, right). Retina sensors are organized into 12 layers.
Only the presence of agents in a layer can be detected but not the number. However, number of activated
layers can be count.


#### Evolution
Coevolution of Markov Networks of predators and prey with a genetic algorithm, selecting predators
which are good at hunting prey and prey which shows high survivability. Fitness is determined by
selecting pairs of predator and prey genomes, putting them into a simulation with 2000 steps, 50 prey agents
and 1 predator agent. In each step a fitness value for this step is calculated. At the end of a simulation
the sum of all single fitness values is calculated, determining the general fitness of predator and prey.
Asexually reproduction in a Moran process meaning that no recombination is applied.

### What are the main findings?

#### Finding 1: Without predator confusions prey agents disperse
In scenarios where no predator confusion occurs, the prey agents try to disperse as much as possible
evolving no swarm behaviour in a single replicate. The predator tries to hunt for the nearest prey agent
(Figure 3A). The survivability of prey increased over the generations.

#### Finding 2: With predator confusion prey evolve swarming behaviour
In 70% of the replicates, prey exhibited cohesive swarming behaviour. The predator evolved a mechanism
which allows him, to hunt for prey on the outer edges of the swarm (Figure 3B and 3C). He only uses
the two center retina slices in order to avoid predator confusion and constantly turns its direction until
prey enters one of his retinal slices. The survivability of prey significantly increases by evolving 
swarming behaviour as shown in Figure 4C.

#### Finding 3: Prey can evolve swarming behaviour without detecting the predator
The prey from generation 1200 ignored the presence of predators and made their movement decision based on
the position of their conspecifics. 

### What is one thing the paper does well, and one limitation or open question?

#### Advantage: Investigates both evolution of prey and predators
The paper investigates the coevolution of predator and prey, instead of focussing only on the prey.
It also explores the interactions between predators and prey.

#### Disadvantage: Only a single predator 
The paper only explores the swarming of prey. Certain predators like wolves hunt in packs in order to
increase their success chance to capture prey. 

### How is this paper similar to and different from our cats-and-mice simulation, and what is one idea from it, we could use in our work?
The simulation of the paper consists of both predators and prey, similar to our cats and mice scenario.
There are however many differences to our scenario:

- we allow multiple cats to exist in a simulation
- genomes can be inherited by two parents or one parent
- individual cats and mice can have different genomes in the same simulation
- there are a lot more sensors which also individual aspects into account (AGE, OSC1, GENETIC_SIM_FWD...)
- neurons are used as memory, storing information from sensors or other neurons
- movement works differently (combination of sensors/neurons decides the final movement into one of eight directions)
- there are a lot more actions which also affect individual aspects (SET_LONGPROBE_DISTANCE, SET_RESPONSIVENESS...)
- individuals can also affect their environment with pheromone (EMIT_SIGNAL0)
- genomes are coded as hexadecimal strings
- there are multiple evolution scenarios with different goals for individuals in order to survive
- we do not have a predator confusion in our scenario

One idea which we could take over from the paper is the handling time of predators. This would make it harder
for the cats to hunt for mice, but it could be a too big advantage for the mice. Another idea we could
take over is developing another visual sensor, which takes the density of individuals 180 degrees in front 
of an individual into account, although we already have a density sensor. 

## Behavioural response of prey to repeated attacks by non-coordinating predators

### What problem or question is this paper trying to answer, and why does it matter?

The paper tries to answer the question, how the attack of multiple predators, which chase the prey persistently,
influences the behavioural answer of the prey. Previous works have already studied predator prey systems.
However, few have considered the presence of more than one predator and the usage of more than a predator 
strategy. The paper shows also that a higher intensity of prey coordination leads to a faster decay of prey numbers
under successive predator attacks.

### How does the simulation work (the world, the predator, the prey, what they can sense and do, evolution)?

#### The world
The world is a two-dimensional virtual environment, consisting of agents (predator and prey).
A Langevin approach is used in order to decide the motions of the agents. 

#### The predators
The predators are agents which rely only on a visual system with a blind spot, influencing their motion. 
A predator has a capture zone R<sub>c</sub>. If a prey agent enters the capture zone it will be marked 
as dead (μ=0). Based on the living prey scanned within the detection zone R<sub>d</sub>, the agent
can choose to chase the nearest prey (NP-Approach) or the most central prey agent of the group (MCP-Approach).
The governing Langevin equation of the predator agent is: $$m_{pd}\ddot{x}_{pd} = F_{s,pd} + F_{f,pd} + F_{p,pd}$$

m<sub>pd</sub> and x<sub>pd</sub> represent the mass and position of the predator. F<sub>s,pd</sub> represents
the separation force of the predator, which acts in the separation zone of the predator. F<sub>f,pd</sub> 
represents the frictional force, acts opposite to the direction of motion of the predator and depends
on the medium (stationary fluid medium in the simulation). F<sub>p,pd<s/sub> represents the propulsion force
of the predator and depends on the state of the predator. If the predator is in satisfied (after capturing a prey
agent) or in refocus state (predator does not capture prey within given time limit) his own velocity influences
the propulsion force. If the predator is in pursuit state, the last known prey agent position influences
its propulsion (towards the prey agent, based on predator strategy), also allowing the predator to accelerate
during pursuit. Predators have higher mass than prey, lowering their manoeuvrability, but they have also
higher speed.

#### The prey
The prey are agents which also rely on a visual system with a blind spot, influencing their motion. 
The governing Langevin equation of the prey agent is:
$$m_{pr}\ddot{x}_{pr} = F_{s,pr} + F_{f,pr} + μ(F_{p,pr} + F_{c,pr} + F_{a,pr})$$

The separation force F<sub>s,pr</sub> and the frictional force F<sub>f,pr</sub> have the same function as
for the predator agent. The separation force serves to prevent an overlap between agents in the simulation.
F<sub>p,pr</sub> represents the propulsion force of the prey agent. If the prey agent is in cruising state
(no predator agent within prey vision), the own velocity influences the propulsion force. If the prey agent
is detecting a predator, the propulsion force acts towards the direction away of the predators position.
F<sub>c,pr</sub> represents the alignment force, driving the prey agents adjusting their velocity to that
of their neighbours and therefore supports their swarming behaviour. F<sub>a,pr</sub> represents the 
attraction force, also supporting swarming behaviour by driving prey agents towards their conspecifics
in the attraction zone around the respective prey agent. 

#### Both
Separation force and frictional force work the same way for predator and prey agents.


#### Evolution
There is no evolution within the simulation. Instead, the results depend on parameters set
before a simulation (predator strategy, relative angle of predators attack, prey coordination strength...).

### What are the main findings?

#### Finding 1: Prey agents can stay cohesive or split into smaller groups
The collective response of the prey agents under attack of a predator is mostly staying within a group
while moving away from the predator or splitting up into smaller groups, which then escape independently.
The paper also cites species in which such a behaviour was observed.

#### Finding 2: Prey agent groups can merge 
The merge of two prey groups can be caused by their attraction force. The merging of groups leading to 
a higher cohesive group can reduce the predation risk according to the many-eyes hypothesis (the larger 
the group, the lesser the individual burden of vigilance against predators, and the faster the response to
perturbations). The presence of multiple predators however, can lead to a subsequent split of the prey groups,
which highlights the dynamic nature of the prey response. The paper shows, that the merge of two prey groups
leads to confusion in which the order and speed of the prey agents is negatively affected until the order in 
their group is recovered.

#### Finding 3: Going for the nearest prey agent is the most successfully predator strategy
The paper shows, that using the NP strategy has the most success rate in capturing prey agents. The
predator agents also show a bias hunting for smaller groups. This also shows the advantage of using the MCP 
strategy intermittently to force the prey to split into smaller groups, since the success rate in hunting
prey agents in smaller groups is above the success rate hunting prey agents in larger groups.

#### Finding 4: The presence of multiple predators can benefit them
The presence of multiple predators reduces the coordination benefits of prey agents. The prey split
into smaller groups or stay cohesive depending on the angle of the predators attack. A phase difference
in the predators attack timings can even cause the group response of the prey agents to the first predator
benefitting the second one.

### What is one thing the paper does well, and one limitation or open question?

#### Advantage: Considers multiple predators and physical forces
One advantage of the paper is, that it considers the effect of multiple predators on the group response
of prey and also acceleration during an escape or attack attempt. 

#### Disadvantage: Does not consider group attacks
The paper only considers non-coordinating predators. In nature there are multiple species like african lions
which use group hunting tactics in order to enhance their success chance. In the work, the agents also
solely rely on visual system in order to decide their movements.

## The evolution of representation of simple cognitive networks

### What problem or question is this paper trying to answer, and why does it matter?

The paper tries to explain internal models of the environment as representations, measure them as R
and give a formal definition of representations based on information theory. The representation is used as
a measure to evaluate the long-term adaptive success of an agent. In order to measure how R changes over
the generations, two types of networks (Artificial Neuronal Network ANN and Markov Brains MB) 
are evolved to solve a categorization task using a genetic algorithm. The problem does matter, because
it shows the significance of AI systems to be able to use past knowledge and context knowledge
to solve complex tasks. The paper argues, that the representation is a valuable tool to investigate
the organization of evolved cognitive systems.

### How does the simulation work?

#### The world

Agents have the task to categorize visual objects into discrete categories based on their appearance. 
Agents have the task to catch falling small blocks and avoid large blocks either falling left or right. 
There are four input units (sensor) to detect falling blocks, ten internal states (neurons) and two actuators 
(actions; encodings 01 10 11 and 00) which control whether the agent stands still (00 and 11), 
moves to the left (10) or to the right (01). Start codon of two loci mark beginning of neuronal net, 
two other loci the number of inputs and outputs while two further loci specify which inputs
feed into which outputs. In one simulation only one brick falls at the time. The agent has a length of
6 units with two sensors located at the left and right of the agent detecting the block falling from above
with no sensors in the middle of the agent. Sensors can be 0 or 1. A large brick is 4 units long while a small
brick is 2 units long. Each brick falls within 20 time steps (arena size 20x20). Bricks are caught if agent 
and brick overlap at least in one unit.

#### Artificial Neuronal Network
In the network there are 4 sensors, 10 internal nodes and 2 output nodes. Sensors can have state -1 or 1.
Action combinations are 00, 01, 10, 11. Internal nodes can have continuous between -1 and 1. Neuronal gates 
are used, which can connect any types of nodes (even action to action or sensor to sensor). The output values 
of the neuronal gates are calculated as 
$$m(t+1)=tanh(\sum_{k=1}^{n}\sum_{j=1}w_{j}^{(k)}m_{j}^{(k)}(t))$$

m<sub>j</sub><sup>(k)</sup> represents a value of an incoming node via gate k multiplied with the associated
weight w<sub>j</sub><sup>(k)</sup>. m is the output for the timestep t.
Output values of neuronal gates are propagated to every node which the gate feeds.
Two loci mark the beginning of a neuronal gate, the two subsequent loci encode number of inputs and outputs
and the last two loci give the specific nodes which feed into the NG and to which nodes the NG feeds.
After this information n weights for of an n-input NG are encoded. The number of neuronal gates can
change during the evolution.

#### Markov Brains
Types of nodes are the same as for the artificial neuronal networks. Instead of neuronal gates, hidden markov
gates (HMG) are used to connect nodes to each other. A HMG can be understood as a finite state machine, defined
by its input-output structure and a state transition table.
The probability of an output node firing is determined by the state transition table. Each HMG can receive up
to 4 inputs and distribute signals up to 4 nodes with minimum of one input and one output node. Only deterministic
HMGs are considered. 




