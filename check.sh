#!/bin/bash
set -e
thisdir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

$thisdir/cppcheck.sh

modes='Debug Release'

function run_many_times() {
    echo "+++ Running tests many times ..."
    count=0
    while [ $count -lt 100 ]; do
         ./gem_test --use-colour no --order rand --rng-seed 'time' > /dev/null
        let count+=1
    done
    echo "Tests OK"
}

for mode in $modes; do
    echo "+++ Checking $mode ..."
    dir=$thisdir/check_build_$mode
    rm -rf $dir
    mkdir $dir
    cd $dir
    echo "Building ..."
    cmake -DCMAKE_BUILD_TYPE=$mode .. > /dev/null
    make -j4 > /dev/null
    echo "Running ..."
    ctest --verbose
    run_many_times
    $thisdir/valgrind.sh $dir/transwarp_test --use-colour no
    cd $thisdir    
    rm -rf $dir
done

echo
echo "+++ All Good!"
