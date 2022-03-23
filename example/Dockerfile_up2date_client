ARG VARIANT="bullseye"
FROM debian:${VARIANT}

RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
    && apt-get -y install libssl-dev ca-certificates jq \
    && apt-get autoremove -y && apt-get clean -y && rm -rf /var/lib/apt/lists/*

COPY build/example/up2date_client/up2date_cli /opt/up2date
COPY up2datew.sh /opt/

WORKDIR /opt
ENTRYPOINT ["/opt/up2datew.sh"]
