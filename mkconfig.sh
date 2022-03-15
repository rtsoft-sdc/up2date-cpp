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

info=`docker run -it --rm -v "$(realpath ${c})":/home/step/certs/data.crt smallstep/step-cli step certificate inspect --format json certs/data.crt`
issuer=`echo ${info} | jq -r .issuer.common_name[0] | tr -d '\n'`
controller=`echo ${info} | jq -r .subject.common_name[0] | tr -d '\n'`

cat << EOF
{
  "x_apig_token": "${x}",
  "provisioning_endpoint": "${p}",
  "certificate": "${certificate}",
  "tenant": "${issuer}",
  "controllerId": "${controller}"
}
EOF

