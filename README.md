# Kiter

Kiter is an iterative algorithm based on K-periodic scheduling to compute the throughput of a CSDFG (see [Bodin2016](https://dl.acm.org/doi/abs/10.1145/2897937.2898056)).

Build status:
[![Build Status](https://travis-ci.org/bbodin/kiter.svg?branch=master)](https://travis-ci.org/bbodin/kiter)
[![Linter Status](https://github.com/bbodin/kiter/workflows/Super-Linter/badge.svg)](https://github.com/marketplace/actions/super-linter)

## Quick start

Using `make release` you can compile the project.

Then you can test the Kiter algorithm on standard SDF3 XML files, for example:

```bash
./Release/bin/kiter -f benchmarks/IB5CSDF/BlackScholes.xml -aKPeriodicThroughput
Maximum throughput is      2.377931898e-08
Maximum period     is      42053349.000000
Execution Time     is             0.625631
```

There are a few requirements to compile Kiter, instructions follow.

## Requirement

* Boost
* LibXml2
* (GLPK)


### OSX

`homebrew install gplk`

### Ubuntu

`apt-get install git make cmake gcc g++ libboost-all-dev libxml2-dev libglpk-dev glpk-utils`

### Fedora

`yum install -y glpk-devel cmake findutils unzip gcc make gcc-c++ libxml2-devel  boost-devel git  wget`

## Compile it

By running ```make release``` you can compile the project.

## Use it

The command is :

```bash
./Release/bin/kiter -f <input-file> -a KPeriodicThroughput
```

```input-file``` is an SDF3-like XML file.

Instead of `KPeriodicThroughput`, more actions are available, a list is available using:

```bash
./Release/bin/kiter -h
```

### Example


```bash
./Release/bin/kiter -f benchmarks/IB5CSDF/BlackScholes.xml -aKPeriodicThroughput
Maximum throughput is      2.377931898e-08
Maximum period     is      42053349.000000
Execution Time     is             0.625631
```

The throughput value should be interpreted similarly to SDF3-ANALYSIS output Version "27 September 2010".

## Run the benchmark

To compare with SDF3 you can downdload the SDF3 benchmarks and tools doing `make sdf3_benchmarks` and `make sdf3`.
Then ```make test``` should run most of the tests.

## Included benchmarks

This repos also includes several benchmarks like AGB5CSDF and IB5CSDF. You can also download the SDF3 benchmarks using `make sdf3_benchmarks`.

```text
benchmarks/AGB5CSDF:
autogen1.xml  autogen2.xml  autogen3.xml  autogen4.xml  autogen5.xml

benchmarks/IB5CSDF:
BlackScholes_sized.xml  Echo_sized.xml  H264_sized.xml  JPEG2000_sized.xml  PDectect_sized.xml
BlackScholes.xml        Echo.xml        H264.xml        JPEG2000.xml        PDectect.xml
```


## Related publications

```TeX
@inproceedings{DBLP:conf/dac/BodinKD16,
  author    = {Bruno Bodin and
               Alix Munier Kordon and
               Beno{\^{\i}}t Dupont de Dinechin},
  title     = {Optimal and fast throughput evaluation of {CSDF}},
  booktitle = {Proceedings of the 53rd Annual Design Automation Conference, {DAC}
               2016, Austin, TX, USA, June 5-9, 2016},
  pages     = {160},
  year      = {2016},
  crossref  = {DBLP:conf/dac/2016},
  url       = {http://doi.acm.org/10.1145/2897937.2898056},
  doi       = {10.1145/2897937.2898056},
  timestamp = {Fri, 27 May 2016 09:17:23 +0200},
  biburl    = {http://dblp.uni-trier.de/rec/bib/conf/dac/BodinKD16},
  bibsource = {dblp computer science bibliography, http://dblp.org}
}
```


## Possible compilation Error

 - undefined reference to `GRBModel::addVar : you'll need to compile gurobi C++ interface `make -C /path/to/gurobi/build/src/`

## TODOs (to check)

- Add type for actors in XML and Dataflow
- Save input and output port names for reetrancy ?
- For duration specify a processor type
- print a channel properties XML even if empty
- In the print mapping, if task are on the same mapping, spread then X,Y slightly
- Add new task and a few more function really slow, need to speed them up.