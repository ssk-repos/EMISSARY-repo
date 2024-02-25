# EMISSARY : Enhanced Miss-Awareness Replacement Policy For L2 Instruction Caching
For decades, architects have designed cache replacement policies to reduce cache misses. Since not all cache misses affect processor performance equally, researchers have also proposed cache replacement policies focused on reducing the total miss cost rather than the total miss count. However, all prior cost-aware replacement policies have been proposed specifically for data caching and are either inappropriate or unnecessarily complex for instruction caching. This paper presents EMISSARY, the first cost-aware cache replacement family of policies specifically designed for instruction caching. Observing that modern architectures entirely tolerate many instruction cache misses, EMISSARY resists evicting those cache lines whose misses cause costly decode starvations.

## Project Outline 
The primary goal of this project is to implement and assess EMISSARY, the first cost-aware cache replacement policy designed specifically for instruction caching. Since the EMISSARY policy prioritizes instruction lines whose miss caused a decode starvation by preserving these lines in L2 upon eviction from L1I, this mainly involves implementing a method to identify intructions which cause decode starvation and communicating the priority of such high-priority instructions to L2 cache so that they can be preserved for fetching by L1I in the future and implementing a cache replacement policy for L2 cache that performs eviction based on line priority.

## Directory Structure
```
EMISSARY
|_ tools
|_ zsim
    |_ configs
      |_ EMISSARY
      |_ LRU
    |_ sim_results
      |_ configuration_1
      |_ configuration_2
      |_ configuration_3
    |_ misc
    |_ src
      |_ emissary_repl.h
    |_ tests
|_ README.md
|_ benchmarks.zip
|_ benchmarks.z01
|_ benchmarks.z02
|_ benchmarks.z03
|_ cse_server.patch
|_ setup_env
 
```

## Simulator Setup and Benchmark Execution
  - Simulator: ZSim ![Source](https://github.tamu.edu/HPCL/casim-csce614)   
  - Dependencies: ```gcc >= 4.6, pin, scons, libconfig, libhdf5, libelfg0```
  - Setup and Compilation
     - ```git apply cse_server.patch```
     - ```zip -F benchmarks.zip --out single-benchmark.zip && unzip single-benchmark.zip && mkdir benchmarks/parsec-2.1/inputs/streamcluster```
     - ```source setup_env```
     - ```cd zsim```
     - ```scons -j4```
  - Run Command
    After compiling the simulator, the benchmarks can be executed using the following command:
    ``` ./runscript <Suite> <Benchmark> <Replacement Policy>```\
    Note: The configurations for each benchmark are located inside ```zsim/configs/``` and can be modified to change cache configurations and EMISSARY replacement policy parameters (high priority threshold, pseudo-random selection probability)
  - Outputs
    The simulation output results will be generated inside ```zsim/outputs/<Replacement_Policy>/<Benchmark>/```. The two main files of interest are ```zsim.out``` and ```<Benchmark>.log```.\
  Note: Simulation results used for analysis are available inside ```zsim/sim_results```
  
## Project Source/Reference Paper
Nayana Prasad Nagendra, Bhargav Reddy Godala, Ishita Chaturvedi, Atmn Patel, Svilen Kanev, Tipp Moseley, Jared Stark, Gilles A. Pokam, Simone Campanoni, and David I. August. 2023. EMISSARY: Enhanced Miss Awareness Replacement Policy for L2 Instruction Caching. In Proceedings of the 50th Annual International Symposium on Computer Architecture (ISCA ’23), June 17–21, 2023, Orlando, FL, USA. ACM, New York, NY, USA, 13 pages.![Link](https://doi.org/10.1145/3579371.3589097)


## Acknowledgements
  - ![Dr. Eun Jung (EJ) Kim](https://engineering.tamu.edu/cse/profiles/ejkim.html) - Professor, Department of Computer Science & Engineering, Texas A & M University
  - Sabuj Laskar, Teaching Assistant (CSCE 614 Fall 2023)
