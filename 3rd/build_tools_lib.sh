#!/bin/bash

script_dir=$(cd $(dirname $0) && pwd)

cd ${script_dir}

./the_essence_of_computing/build_lib.sh 

if [ $? -ne 0 ]; then
    echo "build tools lib filed"
    exit -1
fi

if [ -d tools_lib ]; then
    rm -r tools_lib
fi


mv the_essence_of_computing/tools_lib . 

