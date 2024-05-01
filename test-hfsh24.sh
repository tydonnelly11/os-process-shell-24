#! /bin/bash

if ! [[ -x hfsh24 ]]; then
    echo "hfsh24 executable does not exist"
    exit 1
fi

../tester/run-tests.sh $*


