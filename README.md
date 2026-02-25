# Bizaco Load Balancer -- Project 3
Author: Tiernan Lindauer, UIN 134003853

For CSCE 412, Project 3 - LB

## Build/Run:

```bash
make
```

```bash
./loadbalancer --servers 10 --runtime 10000 --log logs/run_log_10servers_10000cycles.txt
```
(you can also use `--seed` to specify a seed; I used seed 42 for the submitted log, for reproducability)


```bash
./loadbalancer --switch --runtime 10000 --log logs/switch_10000cycles.txt
```


```
include/     Headers: Config, Request, RequestQueue, WebServer, IPBlocker, LoadBalancer
src/         Sources and main.cpp
docs/        Doxygen output (generate with doxygen Doxyfile)
logs/        Log files (created automatically)
config.cfg   Configuration file
Makefile     Build
Doxyfile     Doxygen doxyfile to use
```