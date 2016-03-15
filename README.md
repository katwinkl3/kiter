# kiter

Kiter is an iterative algorithm based on K-periodic scheduling to compute the throughput of a CSDFG.
It also includes several benchmarks like AGB5CSDF and IB5CSDF.


## Compile it.

Just ```make``` should do the job.
You may need to specify a new version for ```SDF3_BINARY_ROOT``` in the Makefile.

## Use it.

```
[toky@localhost kiter]$ ./release/bin/kiter -f benchmark/BlackScholes.xml -aKPeriodicThroughput
Run KPeriodicThroughput
Maximum throughput is 4.755863796e-09
Maximum period     is 210266745.000000
KPeriodicThroughput duration=0.000546
```

The throughput is computed using the same method as SDF3-ANALYSIS Version "27 September 2010".


## Known dependencies
* Boost
* LibXml2
