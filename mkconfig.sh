#!/bin/bash

usage() { echo "Usage:"; echo "$0 -c <certificate path> [-p <provisioning endpoint> -x <token>]" 1>&2; exit 1; }

p="https://dps.ritms.online/provisioning"
x="xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"

while getopts ":c:p:x:" o; do
  case "${o}" in
    c)
        c=${OPTARG}
        ;;
    p)
        p=${OPTARG}
        ;;
    x)
        x=${OPTARG}
        ;;
    *)
        usage
        ;;
  esac
done
shift $((OPTIND-1))

if [ -z "${c}" ]; then
    usage
fi

if [ ! -f "$c" ]; then
  echo "certificate file not exist"
  exit 3
fi

certificate=$(sed -z 's/\n/\\n/g' < "$c")

cat << EOF
{
  "x_apig_token": "${x}",
  "provisioning_endpoint": "${p}",
  "certificate": "${certificate}"
}
EOF

