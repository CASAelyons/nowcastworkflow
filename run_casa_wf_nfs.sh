#!/usr/bin/env bash

old_dir=`pwd`

cd $NOWCAST_WORKFLOW_DIR

dax_name=$(python daxgen.py -o dax_outputs -f $@)
echo $dax_name;
./plan_nfs.sh ${dax_name}

cd $old_dir
