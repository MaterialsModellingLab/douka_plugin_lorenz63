#!/usr/bin/env bash
# Copyright (c) 2025 Materials Modelling Lab, The University of Tokyo
# SPDX-License-Identifier: Apache-2.0

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
BASE="${SCRIPT_DIR}/.."

cd $BASE

NAME="lorenz63"

douka obsgen \
  --param        param/${NAME}.obsgen.json \
  --plugin       ${NAME} \
  --plugin_param param/${NAME}.json \
  --output       output/obs

