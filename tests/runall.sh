#/bin/bash
LOGFILE=runall.log
GOLDFILE=runall.gold
STEPS="step2_eval step3_env step4_if_fn_do step5_tco step6_file step7_quote step8_macros step9_try stepA_mal"

# FIXME -- determine python or python3
PYTHON=python3

cd mal

rm -f $LOGFILE
touch $LOGFILE

for STEP in ${STEPS}
do
    echo "============================================================" >> ${LOGFILE}
    echo ${STEP} >> ${LOGFILE}
    echo "============================================================" >> ${LOGFILE}
    env MAL_IMPL=tests/mal/ral RAW=1 \
      ${PYTHON} ./runtest.py  --deferrable --optional \
        --start-timeout 60 --test-timeout 120 \
        ./${STEP}.mal -- ./run >> ${LOGFILE}
done

#grep -A 2 -B 2 ": failing tests" ${LOGFILE}
diff ${GOLDFILE} ${LOGFILE}
