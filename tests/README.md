# Ral Tests

Run tests that were used during development.

./runall.sh

# Performance

time python3 perf1.py
time ../bin/ral perf1.ral

## puddle linux
python:
real    0m0.099s
user    0m0.089s
sys     0m0.010s
ral:
real    0m1.999s
user    0m1.999s
sys     0m0.000s

# using profiler

mkdir gprof_build && cd gprof_build
cmake \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_FLAGS=-pg -DCMAKE_EXE_LINKER_FLAGS=-pg -DCMAKE_SHARED_LINKER_FLAGS=-pg \
  ..
  
../bin/gprof_ral perf1.ral # makes gmon.out
gprof ../bin/gprof_ral gmon.out > perf1.stats