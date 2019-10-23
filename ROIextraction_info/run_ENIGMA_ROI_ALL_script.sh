#!/bin/bash
#$ -S /bin/bash


## neda.jahanshad@ini.usc.edu ##
## ENIGMA-DTI ##

#######
## part 1 - loop through all subjects to create a subject ROI file 
#######

#make an output directory for all files
mkdir ENIGMA_ROI_part1
dirO1=./ENIGMA_ROI_part1/

for subject in Subject1 Subject7

do
./singleSubjROI_exe ENIGMA_look_up_table.txt mean_FA_skeleton.nii.gz JHU-WhiteMatter-labels-1mm.nii.gz ${dirO1}/${subject}_ROIout ${subject}_masked_FAskel.nii.gz

done


#######
## part 2 - loop through all subjects to create ROI file 
##			removing ROIs not of interest and averaging others
#######

#make an output directory for all files
mkdir ENIGMA_ROI_part2
dirO2=./ENIGMA_ROI_part2/

# you may want to automatically create a subjectList file 
#    in which case delete the old one
#    and 'echo' the output files into a new name
rm ./subjectList.csv

for subject in Subject1 Subject7

do
./averageSubjectTracts_exe ${dirO1}/${subject}_ROIout.csv ${dirO2}/${subject}_ROIout_avg.csv


# can create subject list here for part 3!
echo ${subject},${dirO2}/${subject}_ROIout_avg.csv >> ./subjectList.csv
done


#######
## part 3 - combine all 
#######
Table=./ALL_Subject_Info.txt
subjectIDcol=subjectID
subjectList=./subjectList.csv
outTable=./combinedROItable.csv
Ncov=2
covariates="Age;Sex"
Nroi="all" #2
rois="IC;EC"

#location of R binary 
Rbin=/usr/local/R-2.9.2_64bit/bin/R

#Run the R code
${Rbin} --no-save --slave --args ${Table} ${subjectIDcol} ${subjectList} ${outTable} ${Ncov} ${covariates} ${Nroi} ${rois} <  ./combine_subject_tables.R  