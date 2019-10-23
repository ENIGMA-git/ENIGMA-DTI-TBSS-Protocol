#!/bin/bash
#$ -S /bin/bash


Table=./ALL_Subject_Info.txt
subjectIDcol=subjectID
subjectList=./subjectList.csv
outTable=./combinedROItable.csv
Ncov=2
covariates="Age;Sex"
Nroi="all" #2
rois="IC;EC"

#R binary
Rbin=/usr/local/R-2.9.2_64bit/bin/R

#Run the R code
${Rbin} --no-save --slave --args ${Table} ${subjectIDcol} ${subjectList} ${outTable} ${Ncov} ${covariates} ${Nroi} ${rois} <  ./combine_subject_tables.R  