#!/bin/bash

thau0=${1:-41}
thau1=${2:-0.94}
thau2=${3:-0.39}
thauz=${4:-2000}
thauc=${5:-0.00001}
# Establecemos que el código de retorno de un pipeline sea el del último programa con código de retorno
# distinto de cero, o cero si todos devuelven cero.
set -o pipefail

# Put here the program (maybe with path)
GETF0="get_pitch -1 $thau0 -2 $thau1 -3 $thau2 -4 $thauz -5 $thauc"

for fwav in pitch_db/train/*.wav; do
    ff0=${fwav/.wav/.f0}
    echo "$GETF0 $fwav $ff0 ----"
	$GETF0 $fwav $ff0 > /dev/null || ( echo -e "\nError in $GETF0 $fwav $ff0" && exit 1 )
done

pitch_evaluate pitch_db/train/*.f0ref

exit 0
