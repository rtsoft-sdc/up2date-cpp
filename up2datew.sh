#!/bin/bash

if [ ! -f "client.conf" ] ; then
  echo "no client.conf in the working dir" >&2
  exit 1
fi

export CERT_PATH=/tmp/client.crt
jq -r '.certificate' client.conf > "$CERT_PATH"
export PROVISIONING_ENDPOINT="$(jq -r '.provisioning_endpoint' client.conf)"
export X_APIG_TOKEN="$(jq -r '.x_apig_token' client.conf)"

# graceful shutdown
_term() { 
  echo "SIGTERM" 
  kill -TERM "$child" 2>/dev/null
}

trap _term SIGTERM

/opt/up2date --nodaemon &

child=$! 
wait "$child"