# UP2DATE CLIENT C++ Example

> see [UP2DATE C++ Client Library](../README.md) on how to prepare building environment and configure the example

## QUICK START

> example docker container with RITMS update client:
```shell   
docker build -f example/Dockerfile_up2date_client . -t up2date_client:0.1   
```

> example docker container with DPS only client:
```shell   
docker build -f example/Dockerfile_ritms_auth . -t up2date_client:0.1   
```

> start client from a container
```shell   
docker run --rm -v "$(pwd)/client.conf:/opt/client.conf" up2date_client:0.1
```
