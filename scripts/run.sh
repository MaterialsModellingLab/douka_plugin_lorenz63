#!/usr/bin/env bash
# Copyright (c) 2025 Materials Modelling Lab, The University of Tokyo
# SPDX-License-Identifier: Apache-2.0

set -e

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
BASE="${SCRIPT_DIR}/.."
NAME="lorenz63"

cd $BASE

if [ ! -d "output/obs" ]; then
  echo "Observation does not exist. Run obsgen.sh first."
  exit 1
fi

if [ ! -d "output/init" ]; then
  echo "Initial state does not exist. Run init.sh first."
  exit 1
fi

# If previous state exists, archive it
if [ -d "output/state" ]; then
  tar -zcf output/state$(date +%Y%m%d_%H%M%S).tar.gz -C output state
  rm -rf output/state/*
else
  mkdir -p output/state
fi

# Copy initial state
cp output/init/* output/state

# T is file number - 1 of ./output/obs directory
T=$(ls output/obs/${NAME}*.json | wc -l | awk '{print $1-1}')
# N is file number of ./output/init directory
N=$(ls output/init/${NAME}*.json | wc -l)

for (( t = 0; t < T; t++)); do
  SYS_TIM=$(printf "%06d" $t)
  OBS_TIM=$(printf "%06d" $t)

  echo "Predict ${OBS_TIM}"
  for (( i = 0; i < N; i++)); do
    STATE_FILE=$(printf ${NAME}_%04d_${SYS_TIM}_${OBS_TIM}.json $i)
    douka predict \
      --state        output/state/${STATE_FILE} \
      --param        param/${NAME}.predict.json \
      --plugin       ${NAME} \
      --plugin_param param/${NAME}.json \
      --output       output/state \
      >> output/${NAME}.log &
  done
  wait

  SYS_TIM=$(printf "%06d" $(( t + 1 )))
  STATE_FILE=${NAME}_%04d_${SYS_TIM}_${OBS_TIM}.json

  echo "Filter  ${OBS_TIM}"
  douka filter \
      --state  output/state/${STATE_FILE} \
      --param  param/${NAME}.filter-enkf.json \
      --obs    output/obs/${NAME}_obs_${SYS_TIM}.json \
      --output output/state \
      >> output/${NAME}.log
done

