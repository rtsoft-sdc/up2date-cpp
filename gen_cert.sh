#!/bin/bash

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"

usage() { echo "Usage:"; echo "$0 -n <name>" 1>&2; exit 1; }

while getopts ":c:k:t:n:" o; do
  case "${o}" in
    n)
        n=${OPTARG}
        ;;
    *)
        usage
        ;;
  esac
done
shift $((OPTIND-1))

if [ -z "${n}" ]; then
    usage
fi

CA_CRT="$SCRIPT_DIR/cert/tenant.crt"
CA_KEY="$SCRIPT_DIR/cert/tenant.key"

if [ ! -f "$CA_CRT" ] || [ ! -f "$CA_KEY" ]; then
  echo "key or certificate not exist"
  exit 3
fi

if [[ ! $n =~ ^([a-z](-?[a-z0-9])*)+$ ]]; then
    echo "$n is not valid subdomain"
    exit 1
fi

docker run -it --rm -v "$SCRIPT_DIR/cert":/home/step/certs smallstep/step-cli step certificate create "$n" \
            "certs/${n}.crt" "certs/${n}.key" --profile leaf  \
            --ca certs/tenant.crt --ca-key certs/tenant.key \
            --ca-password-file certs/password --kty RSA --size 2048 --no-password --insecure \
            --not-after 43830h
