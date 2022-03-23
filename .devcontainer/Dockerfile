# See here for image contents: https://github.com/microsoft/vscode-dev-containers/tree/v0.217.4/containers/cpp/.devcontainer/base.Dockerfile

# [Choice] Debian / Ubuntu version (use Debian 11, Ubuntu 18.04/21.04 on local arm64/Apple Silicon): debian-11, debian-10, ubuntu-21.04, ubuntu-20.04, ubuntu-18.04
ARG VARIANT="bullseye"
FROM mcr.microsoft.com/vscode/devcontainers/cpp:0-${VARIANT}

ENV VCPKG_ROOT /usr/local/vcpkg

RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
     && apt-get -y install --no-install-recommends libssl-dev doxygen

ENV PATH "${VCPKG_ROOT}:${PATH}"

RUN vcpkg integrate install
