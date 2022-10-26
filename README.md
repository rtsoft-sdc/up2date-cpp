[![ghcr.io](https://github.com/rtsoft-gmbh/up2date-cpp/actions/workflows/ci.yaml/badge.svg)](https://github.com/rtsoft-gmbh/up2date-cpp/actions/workflows/ci.yaml)
[![OpenSSF Scorecard](https://img.shields.io/ossf-scorecard/github.com/rtsoft-gmbh/up2date-cpp?label=openssf%20scorecard&style=flat)](https://api.securityscorecards.dev/projects/github.com/rtsoft-gmbh/up2date-cpp)

# UP2DATE CLIENT C++ Library

## INTRODUCTION

[RITMS UP2DATE](https://up2date.ritms.online) is a cloud ready solution for unified software and firmware management. Use this for implementing lifecycle management for the full stack of drivers and firmware of connected devices.

RITMS UP2DATE is based on open and worldwide adopted building blocks, the most important is [Eclipse Hawkbit](https://www.eclipse.org/ddi/) which provides open and flexible Direct Device Integration (DDI) API and Management API.

RITMS UP2DATE extends Eclipse Hawkbit API with zero-cost maintenance device provisioning based on X509 certificates. The Public Key Infrastructure deployed to cloud governs digital certificates to secure end-to-end communications. Devices are automatically provisioned to connect the update service in a secure way.

This C++ client library is a reference implementation of RITMS UP2DATE API for connected constrained devices.

## QUICK START

1. Contact [RITMS UP2DATE](https://ritms.online) to get access and keys for a tenant service deployed in a cloud.
2. Using provided (or generated) `client.conf`
```shell   
docker run --rm -v "$(pwd)/client.conf:/opt/client.conf" ghcr.io/rtsoft-gmbh/up2date_cpp:latest
```
> in Windows Command Line (cmd):
```shell   
docker run --rm -v "%cd%/client.conf:/opt/client.conf" ghcr.io/rtsoft-gmbh/up2date_cpp:latest
```

3. Log in UP2DATE Management UI, upload a firmware bundle and assign it to the device. 

## BUILDING CUSTOMIZED CLIENT

```shell   
git clone --recurse-submodules https://github.com/rtsoft-gmbh/up2date-cpp.git
```

***Finally use [Visual Studio Code Remote-Containers](README-vscode.md) to build and develop in dedicated pre-configured contanerized environment.*** 

> if you do not use Visual Studio Code environment install dependencies and then run cmake:

> For UNIX systems default http client implementation requires **libssl-dev**. ```apt install libssl-dev```

```shell 
cd up2date-cpp 
mkdir build
cmake -B build -S . -DBUILD_EXAMPLES=ON
cd build && make
```

### CMake flags

**BUILD_EXAMPLES** - *(default OFF)* build example projects (up2date_client, ritms_ott, standalone_client, ritms_auth).
**U2D_HTTP_CLIENT_IMPL** - *(default AUTO)* detects and apply http client implementation. Can be AUTO, WIN, UNIX, CUSTOM.   
>NOTE: If **U2D_HTTP_CLIENT_IMPL=CUSTOM**  cmake flags *CUSTOM_HTTP_CLIENT_SRC*, *CUSTOM_HTTP_CLIENT_LINK_LIBRARIES*, *CUSTOM_HTTP_CLIENT_INCLUDE_DIRS* should be carefully set.
For usage check include scripts: [example flag usage](modules/src/unix/CMakeLists.txt), [build script](modules/CMakeLists.txt)

## CONFIGURATION

To connect RITMS UP2DATE cloud service the device must be configured with
1. X509 Device Provisioning Certificate.
2. Provisioning endpoint (API URL and API token)

The [example](example/README.md) provides a ready to start implementation of a client simulator, it uses a single `client.conf` containing all configuration data:

> To prepare `client.conf` file put device certificate to `cert` directory and run:
```shell
./mkconfig.sh -c cert/my-controller.crt > client.conf
```

### For Tenant Owners
(assume `tenant.crt`, `tenant.key`, `password` are in `cert`) create certificates for each device:
```shell   
./gen_cert.sh -n my-controller-1
./gen_cert.sh -n my-controller-2
...
```   

delete `.key` files and deliver `my-controller-X.crt` to each device.

 > WARNING: Keep tenant key-pair (`tenant.crt`, `tenant.key`) in a secret place and do not store `password` permanently along with key-pair!
