
# Adaptive FogMon

_A self-adaptive P2P monitoring tool for the Cloud-to-Thing continuum infrastructures._

> V. Colombo, A. Tundo, M. Ciavotta, and L. Mariani, “Towards Self-Adaptive Peer-to-Peer Monitoring for Fog Environments,” in Proceedings of the 17th Symposium on Software Engineering for Adaptive and Self-Managing Systems (SEAMS), 2022.

## Synopsys

Monitoring is a critical component in Fog environments: it promptly provides insights about the behavior of systems, reveals Service Level Agreements (SLAs) violations, enables the autonomous orchestration of services and platforms, calls for the intervention of operators, and triggers self-healing actions. <br>
In such environments, monitoring solutions have to cope with the heterogeneity of the devices and platforms present in the Fog, the limited resources available at the edge of the network, and the high dynamism of the whole Cloud-to-Thing continuum. <br>
This work addresses the challenge of accurately and efficiently monitoring the Fog with a self-adaptive peer-to-peer (P2P) monitoring solution that can opportunistically adjust its behavior according to the collected data exploiting a lightweight rule-based expert system ([CLIPS](http://www.clipsrules.net/)). <br>
Empirical results show that adaptation can improve monitoring accuracy, while reducing network and power consumption at the cost of higher memory consumption.

> This self-adaptive prototype is an extension of [FogMon 2.0](https://github.com/di-unipi-socc/FogMon/tree/liscio-2.0).
> Please refer to the upstream repository and the following articles for the original version:
> 
> Stefano Forti, Marco Gaglianese, Antonio Brogi <br>
> [**Lightweight self-organising distributed monitoring of Fog infrastructures**](https://doi.org/10.1016/j.future.2020.08.011), Future Generation Computer Systems, 2020.
> 
> Antonio Brogi, Stefano Forti, Marco Gaglianese <br>
> [**Measuring the Fog, Gently**](https://doi.org/10.1007/978-3-030-33702-5_40), 17th International Conference on Service-Oriented Computing (ICSOC), 2019.



## Quickstart

### Docker setup
```
git submodule init
git submodule update
```

There is a Docker image for easily running the node.

```
docker build --tag fogmon .
```

Then run a _Leader_ with the command
```
docker run -it --net=host fogmon --leader
```
and the other nodes with
```
docker run -it --net=host fogmon -C ip_leader
```
All the other parameters are visible reading the main.cpp, they can set up all FogMon parameters (time between tests, time between checks etc).

Some ports needs to be open for incoming connections, by default they are:

5555/TCP fogmon

5201/TCP iperf

8366/TCP assolo

8365/UDP assolo

### Example execution on 5 nodes
Let's call the 5 nodes A, B, C, D, E and their ip IP_A, IP_B,..., IP_E.

First run one of the nodes (A) as a default _Leader_
```
docker run -it --net=host fogmon --leader
```
then connect all the other nodes to A
```
docker run -it --net=host fogmon -C IP_A
```
After 5 rounds the parameter "--time-propagation" a _Leader_ selection happens, and 2 new leaders are selected. This is because the number of _Leaders_ is too low to sustain 5 nodes.


### Compile outside docker

The tool can be compiled outside Docker (not advised) as follows:

```
cmake .
```
```
make
```
dependencies:

sqlite3 for development

libstdc++ for static linking

libserialport

sigar:

https://github.com/hyperic/sigar

it needs libtool for compiling

and if it does not compile then try with:
```
./autogen.sh && ./configure && make && sudo make install
```
and
```
make clean
./autogen.sh && ./configure && make CFLAGS=-fgnu89-inline && sudo make install
```

## How to specify adaptation rules
Examples of adaptation rules are available with the [rules-example.clp](src/follower/rules-example.clp) and [leader-rules-example.clp](src/leader/leader-rules-example.clp) files.
