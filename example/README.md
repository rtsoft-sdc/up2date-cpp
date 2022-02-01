# UP2DATE CLIENT C++ Example

> see [UP2DATE C++ Client Library](../README.md) on how to prepare building environment and configure the example

## QUICK START

> example docker container:
```shell   
docker build -f example/Dockerfile . -t up2date_client:0.1   
```

> start client from a container
```shell   
docker run --rm -v "$(pwd)/client.conf:/opt/client.conf" up2date_client:0.1
```
