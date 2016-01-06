#!/bin/bash

find "$1" -type f -printf '%n %p\n' | awk '$1 > 1{$1="";print}'
