# UP2DATE CLIENT C++ Library

## INTRODUCTION

[RITMS UP2DATE](https://ritms.online) is a cloud ready solution for unified software and firmware management. Use this for implementing lifecycle management for the full stack of drivers and firmware of connected devices.

RITMS UP2DATE is based on open and worldwide adopted building blocks, the most important is [Eclipse Hawkbit](https://www.eclipse.org/hawkbit/) which provides open and flexible Direct Device Integration (DDI) API and Management API.

RITMS UP2DATE extends Eclipse Hawkbit API with zero-cost maintenance device provisioning based on X509 certificates. The Public Key Infrastructure deployed to cloud governs digital certificates to secure end-to-end communications. Devices are automatically provisioned to connect the update service in a secure way.

This C++ client library is a reference implementation of RITMS UP2DATE API for connected constrained devices.

## QUICK START

1. Contact [RITMS UP2DATE](https://ritms.online) to get access and keys for a tenant service deployed in a cloud.
2. Using provided (or generated) `client.conf`
```shell   
docker run --rm -v "$(pwd)/client.conf:/opt/client.conf" rtsoft/up2date_cpp:latest
```
> in Windows Command Line (cmd):
```shell   
docker run --rm -v "%cd%/client.conf:/opt/client.conf" rtsoft/up2date_cpp:latest
```

3. Log in UP2DATE Management UI, upload a firmware bundle and assign it to the device. 

## BUILDING CUSTOMIZED CLIENT

```shell   
git clone --recurse-submodules https://github.com/rtsoft-gmbh/up2date-cpp.git
```

> *Did you just check out / clone a repository and forget `--recursive / --recurse-submodules` ? Well, do this:*
```shell   
git submodule update --init --recursive
```

> ***Use [Visual Studio Code Remote-Containers](README-vscode.md) to build and develop in dedicated pre-configured contanerized environment.*** 

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
