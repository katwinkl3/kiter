# kiter

Kiter is an iterative algorithm based on K-periodic scheduling to compute the throughput of a CSDFG.



## Compile it.

Just ```make``` should do the job.

## Use it.

The command is :

```
./release/bin/kiter -f <input-file> -a <algorithm>
```
```input-file``` is an SDF3-like XML file, and ```algorithm``` the algorithm to apply.

Available algorithms are automatically listed if no algorithm provided :
```
[toky@zebulon kiter]$ ./release/bin/kiter -f AGB5CSDF/autogen1.xml 
 Unsupported algorithm (-a NAME), list of supported algorithms is 
 - 1PeriodicThroughput : Optimal 1-Periodic Throughput evaluation of CSDF by K-Periodic scheduling method.
 - 2PeriodicThroughput : Optimal 1-Periodic Throughput evaluation of CSDF by K-Periodic scheduling method.
 - NKPeriodicThroughput : Optimal Throughput evaluation of CSDF by using N-periodic method.
 - NPeriodicThroughput : Optimal Throughput evaluation of SDF by using Munier1993 method.
 - NCleanPeriodicThroughput : Optimal Throughput evaluation of SDF by using Munier1993 method combined with deGroote2012 reduction.
 - KPeriodicThroughput : Optimal Throughput evaluation of CSDF by K-Periodic scheduling method 1.
 - KBisPeriodicThroughput : Optimal Throughput evaluation of CSDF by K-Periodic scheduling method 2.
 - KTerPeriodicThroughput : Optimal Throughput evaluation of CSDF by K-Periodic scheduling method 3.
 - PrintInfos : Just print some graph informations.
 - deGrooteThroughput : Throughput analysis from deGroote2012 paper except event graph reduction.
 - deGrooteCleanThroughput : Throughput analysis from deGroote2012 paper.
```

### Example


```
[toky@localhost kiter]$ ./release/bin/kiter -f benchmark/BlackScholes.xml -aKPeriodicThroughput
Run KPeriodicThroughput
Maximum throughput is 4.755863796e-09
Maximum period     is 210266745.000000
KPeriodicThroughput duration=0.000546
```

The throughput is computed using the same method as SDF3-ANALYSIS Version "27 September 2010".


## Run the benchmark

To compare with SDF3 you may need to specify a new value for ```SDF3_BINARY_ROOT``` in the Makefile. 
Then ```make benchmark``` should do the job.

## Included benchmarks

This repos also includes several benchmarks like AGB5CSDF and IB5CSDF.

```
AGB5CSDF:
autogen1.xml  autogen2.xml  autogen3.xml  autogen4.xml  autogen5.xml

benchmark:
21.xml  BlackScholes.xml  Echo.xml  H264.xml  JPEG2000.xml  new_benchmark.xml  Pdectect.xml  sample.xml

benchmark_sized:
Black-scholes.xml  Echo.xml  H264.xml  JPEG2000.xml  Pdectect.xml
```


## Known dependencies
* Boost
* LibXml2
