#!/bin/bash

if [[ $(git diff --shortstat 2> /dev/null | tail -n1) != "" ]] ; then
  echo "Cant export - would possibly override stuff"
  exit 0
fi

sed -i '/^%/ d' ch_potentials.tex
sed -i 's/bibliography{references}/input{ch_potentials.bbl}/' ch_potentials.tex

zip arxiv.zip \
ch_potentials.tex \
ch_potentials.bbl \
lipics-v2021.cls \
fig/* \
table/*

git checkout ch_potentials.tex
