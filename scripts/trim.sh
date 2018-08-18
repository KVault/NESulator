#!/usr/bin/env bash

set -e

cd "$(dirname "$0")/.."

tr -d ' # \f\t' < logs/nestest.log > logs/nestest_trim.log
tr -d ' # \f\t' < logs/nesulator.log > logs/nesulator_trim.log

vimdiff logs/nesulator_trim.log logs/nestest_trim.log
