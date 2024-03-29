#!/bin/bash

if [[ $(git diff --shortstat 2> /dev/null | tail -n1) != "" ]] ; then
  echo "Cant export - would possibly override stuff"
  exit 0
fi

sed -i '/^%/ d' ch_potentials.tex

zip jea.zip \
ch_potentials.tex \
references.bib \
fig/* \
table/*

git checkout ch_potentials.tex
