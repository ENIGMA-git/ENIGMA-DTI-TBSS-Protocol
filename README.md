### Table of Contents
- [Protocol for TBSS analysis using the ENIGMA-DTI template](#protocol-for-tbss-analysis-using-the-enigma-dti-template)
- [Protocol for ROI analysis using the ENIGMA-DTI template](#protocol-for-roi-analysis-using-the-enigma-dti-template)
- [Protocol for applying TBSS skeletonizations from FA analysis to diffusivity and obtaining ROI measures using the ENIGMA-DTI template](#protocol-for-applying-tbss-skeletonizations-from-fa-analysis-to-diffusivity-and-obtaining-roi-measures-using-the-enigma-dti-template)


# Protocol for TBSS analysis using the ENIGMA-DTI template
##### (Last update April 2014)

Neda Jahanshad, Emma Sprooten, Peter Kochunov
neda.jahanshad@ini.usc.edu, emma.sprooten@yale.edu

The following steps will allow you to register and skeletonize your FA images to the DTI atlas being used for ENIGMA-DTI for tract-based spatial statistics (TBSS; Smith et al., 2006).

Here we assume preprocessing steps including motion/Eddy current correction, masking, tensor calculation, and creation of FA maps has already been performed, along with quality control.

Further instructions for using FSL, particularly TBSS can be found on the website: http://www.fmrib.ox.ac.uk/fsl/tbss/index.html

1. Download a copy of the ENIGMA-DTI template FA map, edited skeleton, masks and corresponding distance map from this repository (folder titled [enigmaDTI](enigmaDTI/)). The downloaded archive will have the following files:
    - ENIGMA_DTI_FA.nii.gz
    - ENIGMA_DTI_FA_mask.nii.gz
    - ENIGMA_DTI_FA_skeleton.nii.gz
    - ENIGMA_DTI_FA_skeleton_mask.nii.gz
    - ENIGMA_DTI_FA_skeleton_mask_dst.nii.gz

2. Copy all FA images into a folder
            
        cp /subject*_folder/subject*_FA.nii.gz /enigmaDTI/TBSS/run_tbss/

3. cd into directory and erode images slightly with FSL
        
        cd /enigmaDTI/TBSS/run_tbss/
        tbss_1_preproc *.nii.gz
        
This will create a ./FA folder with all subjects eroded images and place all original ones in a ./origdata folder

4. Register all subjects to template. Can choose registration method that works best for your data (_as a default use TBSS_)

        tbss_2_reg -t ENIGMA_DTI_FA.nii.gz
        tbss_3_postreg -S
        
Make sure to QC images to ensure good registration! ***if any maps are poorly registered, move them to another folder

        mkdir /enigmaDTI/TBSS/run_tbss/BAD_ REGISTER/
        mv FA_didnt_pass_QC* /enigmaDTI/TBSS/run_tbss/BAD_REGISTER/
        
**NOTE:** If your field of view is different from the ENIGMA template – (example, you are missing some cerebellum/temporal lobe from your FOV) or you find that the ENIGMA mask is somewhat larger than your images, please follow Steps 5 and 6 to remask and recreate the distance map. Otherwise, continue to use the distance map provided

5. Make a new directory for your edited version:

         mkdir /enigmaDTI/TBSS/ENIGMA_targets_edited/
        
Create a common mask for the specific study and save as:

         /enigmaDTI/TBSS/ENIGMA_targets_edited/mean_FA_mask.nii.gz
    
One option to create a common mask for your study (in ENIGMA space) is to combine all well registered images and see where most subjects (here 90%) have brain tissue using FSL tools and commands:

        cd /enigmaDTI/TBSS/run_tbss/

        ${FSLPATH}/fslmerge –t ./all_FA_QC ./FA/*FA_to_target.nii.gz
        ${FSLPATH}/fslmaths ./all_FA_QC -bin -Tmean –thr 0.9 /enigmaDTI/TBSS/ENIGMA_targets_edited/mean_FA_mask.nii.gz

Mask and rename ENIGMA_DTI templates to get new files for running TBSS:

        ${FSLPATH}/fslmaths /enigmaDTI/TBSS/ENIGMA_targets/ENIGMA_DTI_FA.nii.gz –mas /enigmaDTI/TBSS/ENIGMA_targets_edited/mean_FA_mask.nii.gz /enigmaDTI/TBSS/ENIGMA_targets_edited/mean_FA.nii.gz

        ${FSLPATH}/fslmaths /enigmaDTI/TBSS/ENIGMA_targets/ENIGMA_DTI_FA_skeleton.nii.gz –mas /enigmaDTI/TBSS/ENIGMA_targets_edited/mean_FA_mask.nii.gz /enigmaDTI/TBSS/ENIGMA_targets_edited/mean_FA_skeleton.nii.gz

Your folder should now contain:

        /enigmaDTI/TBSS/ENIGMA_targets_edited/mean_FA.nii.gz
        /enigmaDTI/TBSS/ENIGMA_targets_edited/mean_FA_mask.nii.gz
        /enigmaDTI/TBSS/ENIGMA_targets_edited/mean_FA_skeleton.nii.gz
        
6. cd into directory where you have newly masked ENIGMA target and skeleton to create a distance map

        tbss_4_prestats -0.049
        
The distance map will be created but the function will return an error because the all_FA is not included here. This is ok! The skeleton has already been thresholded here so we do not need to select a higher FA value (ex 0.2) to threshold.

It will output:

        /enigmaDTI/TBSS/ENIGMA_targets_edited/mean_FA_skeleton_mask_dst

Your folder should now contain at least:

        /enigmaDTI/TBSS/ENIGMA_targets_edited/mean_FA.nii.gz
        /enigmaDTI/TBSS/ENIGMA_targets_edited/mean_FA_mask.nii.gz
        /enigmaDTI/TBSS/ENIGMA_targets_edited/mean_FA_skeleton.nii.gz
        /enigmaDTI/TBSS/ENIGMA_targets_edited/mean_FA_skeleton_mask.nii.gz
        /enigmaDTI/TBSS/ENIGMA_targets_edited/mean_FA_skeleton_mask_dst.nii.gz

**NOTE:** For the following steps, if you use the ENIGMA mask and distance map as provided, in the commands for steps 7 and 8 replace: 

`/enigmaDTI/TBSS/ENIGMA_targets_edited/mean_FA_mask.nii.gz` with `/enigmaDTI/TBSS/ENIGMA_targets/ENIGMA_DTI_FA.nii.gz` 

and `/enigmaDTI/TBSS/ENIGMA_targets_edited/mean_FA_skeleton_mask_dst` with `/enigmaDTI/TBSS/ENIGMA_targets/ENIGMA_DTI_FA_skeleton_mask_dst`


7. For faster processing or parallelization, it is helpful to run the projection on one subject at a time. Move each subject FA image into its own directory and (if masking was necessary as in steps 5 and 6 above) mask with common mask. This can be parallelized on a multiprocessor system if needed.

        cd /enigmaDTI/TBSS/run_tbss/
        
        for subj in subj_1 subj_2 … subj_N
        do
        
        mkdir -p ./FA_individ/${subj}/stats/
        mkdir -p ./FA_individ/${subj}/FA/
        
        cp ./FA/${subj}_*.nii.gz ./FA_individ/${subj}/FA/
        
        ####[optional/recommended]####
        ${FSLPATH}/fslmaths ./FA_individ/${subj}/FA/${subj}_*FA_to_target.nii.gz -mas /enigmaDTI/TBSS/ENIGMA_targets_edited/mean_FA_mask.nii.gz ./FA_individ/${subj}/FA/${subj}_masked_FA.nii.gz
        
        done
        
8. Skeletonize images by projecting the ENIGMA skeleton onto them:

        cd /enigmaDTI/TBSS/run_tbss/
        for subj in subj_1 subj_2 … subj_N
        do

        ${FSLPATH}/tbss_skeleton -i ./FA_individ/${subj}/FA/${subj}_masked_FA.nii.gz -p 0.049 /enigmaDTI/TBSS/ENIGMA_targets_edited/mean_FA_skeleton_mask_dst ${FSLPATH}/data/standard/LowerCingulum_1mm.nii.gz ./FA_individ/${subj}/FA/${subj}_masked_FA.nii.gz ./FA_individ/${subj}/stats/${subj}_masked_FAskel.nii.gz -s /enigmaDTI/TBSS/ENIGMA_targets_edited/mean_FA_skeleton_mask.nii.gz

        done
        
Congrats! Now you have all your images in the ENIGMA-DTI space with corresponding projections.

All your skeletons are:

        /enigmaDTI/TBSS/run_tbss/FA_individ/${subj}/stats/${subj}_masked_FAskel.nii.gz
        
# Protocol for ROI analysis using the ENIGMA-DTI template
Neda Jahanshad, Rene Mandl, Peter Kochunov
neda.jahanshad@ini.usc.edu

The following steps will allow you to extract relevant ROI information from the skeletonized FA images that have been registered and skeletonized according to the ENIGMA-DTI template, and keep track of them in a spreadsheet.
* Can be a tab-delimited text file, or a .csv  
* Ex) MetaDataSpreadsheetFile.csv :
* The following is an example of a data spreadsheet with all variables of interest. This spreadsheet is something you may already have to keep track of all subject information. It will be used later to extract only information of interest in Step 6

     
   |  subjectID    |   Age     | Diagnosis  |   Sex   |   ...   |
   | ------------- |:---------:|-----------:|:-------:|:-------:|
   |    USC_01     |    23     |      1     |    1    |   ...   |
   |    USC_02     |    45     |      1     |    2    |   ...   |
   |    USC_03     |    56     |      1     |    1    |   ...   | 
   |    USC_04     |    27     |      1     |    1    |   ...   |
   |    USC_05     |    21     |      1     |    1    |   ...   |
   |    USC_06     |    44     |      2     |    2    |   ...   |
   |    USC_07     |    35     |      1     |    1    |   ...   |
   |    USC_08     |    31     |      1     |    2    |   ...   |
   |    USC_09     |    50     |      1     |    1    |   ...   |
   |    USC_10     |    29     |      1     |    2    |   ...   |
   
* An example is provided - ALL_Subject_Info.txt

## Instructions

1. Download and install [R](http://cran.r-project.org/)
2. Download a copy of the scripts and executables from the folder titled [ROIextraction_info](ROIextraction_info)

Bash shell scripts and compiled versions of the code **bold** have been made available to run on Linux -based workstations. Raw code is also provided in the case re-compilation is needed.
The downloaded archive will have the following files:
 * [**run_ENIGMA_ROI_ALL_script.sh**](ROIextraction_info/run_ENIGMA_ROI_ALL_script.sh)
 * [**singleSubjROI_exe**](ROIextraction_info/singleSubjROI_exe)
 * [singleSubject_FA_ROI.cpp](ROIextraction_info/singleSubject_FA_ROI.cpp)
 * [**averageSubjectTracts_exe**](ROIextraction_info/averageSubjectTracts_exe)
 * [average_subj_tract_info.cpp](ROIextraction_info/average_subj_tract_info.cpp)
 * [**run_combineSubjectROI_script.sh**](ROIextraction_info/run_combineSubjectROI_script.sh)
 * [combine_subject_tables.R](ROIextraction_info/combine_subject_tables.R)
 
_necessary files_
 * [ENIGMA_look_up_table.txt](ROIextraction_info/ENIGMA_look_up_table.txt)
 * [JHU-WhiteMatter-labels-1mm.nii.gz](ROIextraction_info/JHU-WhiteMatter-labels-1mm.nii.gz)
 * [mean_FA_skeleton.nii.gz](ROIextraction_info/mean_FA_skeleton.nii.gz)

 _example files_
 * [ALL_Subject_Info.txt](ROIextraction_info/ALL_Subject_Info.txt)
 * [subjectList.csv](ROIextraction_info/subjectList.csv)
 * [Subject1_FAskel.nii.gz](ROIextraction_info/Subject1_FAskel.nii.gz) #not named the same as download
 * [Subject7_FAskel.nii.gz](ROIextraction_info/Subject7_FAskel.nii.gz) #not named the same as download

 _example outputs_
 * [Subject1_ROIout.csv](ROIextraction_info/ENIGMA_ROI_part1/Subject1_ROIout.csv)
 * [Subject1_ROIout_avgs.csv](ROIextraction_info/ENIGMA_ROI_part2/Subject1_ROIout_avgs.csv) #not named the same as download
 * [Subject7_ROIout.csv](ROIextraction_info/ENIGMA_ROI_part1/Subject7_ROIout.csv)
 * [Subject7_ROIout_avgs.csv](ROIextraction_info/ENIGMA_ROI_part2/Subject7_ROIout_avgs.csv) #not named the same as download
 * [combinedROItable.csv](ROIextraction_info/combinedROItable.csv)

3. **run_ENIGMA_ROI_ALL_script.sh** provides an example shell script on how to run all the pieces in series.
    * This can be modified to run the first two portions in parallel if desired.

4. The first command - **singleSubjROI_exe** uses the atlas and skeleton to extract ROI values from the JHU-atlas ROIs as well as an average FA value across the entire skeleton
    * It is run with the following inputs:
    * ./singleSubjROI_exe look_up_table.txt skeleton.nii.gz JHU-WhiteMatterlabels-1mm.nii.gz OutputfileName Subject_FA_skel.nii.gz
    * example -- ./singleSubjROI_exe ENIGMA_look_up_table.txt mean_FA_skeleton.nii.gz JHU-WhiteMatter-labels-1mm.nii.gz Subject1_ROIout Subject1_FAskel.nii.gz
    * The output will be a .csv file called Subject1_ROIout.csv with all mean FA values of ROIs listed in the first column and the number of voxels each ROI contains in the second column (see **ENIGMA_ROI_part1/Subject1_ROIout.csv** for example output)

5. The second command - **averageSubjectTracts_exe** uses the information from the first output to average relevant (example average of L and R external capsule) regions to get an average value weighted by volumes of the regions.
    * It is run wiht the following inputs
    * ./averageSubjectTracts_exe inSubjectROIfile.csv outSubjectROIfile_avg.csv
    * where the first input is the ROI file obtained from **Step 4** and the second input is the name of the desired output file.
    * The output will be a .csv file called _outSubjectROIfile_avg.csv_ with all mean FA values of the new ROIs listed in the first column and the number of voxels each ROI contains in the second column (see **ENIGMA_ROI_part2/Subject1_ROIout_avg.csv** for example output)

6. The final portion of this analysis is an ‘R’ script combine_subject_tables.R that takes into account all ROI files and creates a spreadsheet which can be used for GWAS or other association tests. It matches desired subjects to a meta-data spreadsheet, adds in desired covariates, and combines any or all desired ROIs from the individual subject files into individual columns.
    * Input arguments as shown in the bash script are as follows:
        * Table=./ALL_Subject_Info.txt –
            * A meta-data spreadsheet file with all subject information and any and all covariates
        * subjectIDcol=subjectID
            * the header of the column in the meta-data spreadsheet referring to the subject IDs so that they can be matched up accordingly with the ROI files
        * subjectList=./subjectList.csv
            * a two column list of subjects and ROI file paths
            * this can be created automatically when creating the average ROI .csv files – see **run_ENIGMA_ROI_ALL_script.sh** on how that can be done
        * outTable=./combinedROItable.csv
            * the filename of the desired output file containing all covariates and ROIs of interest
        * Ncov=2
            * The number of covariates to be included from the meta-data spreadsheet
            * At least age and sex are recommended
        * covariates="Age;Sex"
            * the column headers of the covariates of interest
            * these should be separated by a semi-colon ‘;’ and no spaces
        * Nroi="all" #2
            * The number of ROIs to include
            * Can specify “all” in which case all ROIs in the file will be added to the spreadsheet
            * Or can specify only a certain number, for example 2 and write out the 2 ROIs of interest in the next input
        * rois= “all” #"IC;EC"
            * the ROIs to be included from the individual subject files
            * this can be “all” if the above input is “all”
            * or if only a select number (ex, 2) ROIs are desired, then the names of the specific ROIs as listed in the first column of the ROI file
                * these ROI names should be separated by a semi-colon ‘;’ and no spaces for example if Nroi=2, rois="IC;EC" to get only information for the internal and external capsules into the output .csv file   
    * (see **combinedROItable.csv** for example output)

Congrats! Now you should have all of your subjects ROIs in one spreadsheet with only relevant covariates ready for association testing!

 ![picture](images/enigma_tbss.png)
 
 
 
# Protocol for applying TBSS skeletonizations from FA analysis to diffusivity and obtaining ROI measures using the ENIGMA-DTI template

The following steps will allow you to skeletonize diffusivity measures including mean, axial and radial diffusivity (denoted by MD, L1, and RD respectively) and extract relevant ROI information from them according to the ENIGMA-DTI template, and keep track of them in a spreadsheet.

***Before you get started, you must perform all the ENIGMA- FA analyses!! This protocol will follow the same naming conventions ***

Make sure you have already performed the FA analyses here:
- [Protocol for TBSS analysis using the ENIGMA-DTI template](#protocol-for-tbss-analysis-using-the-enigma-dti-template)
and
- [Protocol for ROI analysis using the ENIGMA-DTI template](#protocol-for-roi-analysis-using-the-enigma-dti-template)

## Instructions

1. Setup 
    - From the previous TBSS protocol (linked above), we will assume the parent directory is: `/enigmaDTI/TBSS/run_tbss/`,
which we will define this through the variable parentDirectory but you should modify this according to where your images are stored.

    Also as before, we will assume your ENIGMA template files are:

        /enigmaDTI/TBSS/ENIGMA_targets/ENIGMA_DTI_FA.nii.gz
        /enigmaDTI/TBSS/ENIGMA_targets/ENIGMA_DTI_FA_mask.nii.gz
        /enigmaDTI/TBSS/ENIGMA_targets/ENIGMA_DTI_FA_skeleton.nii.gz
        /enigmaDTI/TBSS/ENIGMA_targets/ENIGMA_DTI_FA_skeleton_mask.nii.gz
        /enigmaDTI/TBSS/ENIGMA_targets/ENIGMA_DTI_FA_skeleton_mask_dst.nii.gz
        
    *Note: if you had to re-mask the template your paths will be to the edited versions, so remember to use these instead!*
    
        /enigmaDTI/TBSS/ENIGMA_targets_edited/mean_FA.nii.gz
        /enigmaDTI/TBSS/ENIGMA_targets_edited/mean_FA_mask.nii.gz
        /enigmaDTI/TBSS/ENIGMA_targets_edited/mean_FA_skeleton.nii.gz
        /enigmaDTI/TBSS/ENIGMA_targets_edited/mean_FA_skeleton_mask.nii.gz
        /enigmaDTI/TBSS/ENIGMA_targets_edited/mean_FA_skeleton_mask_dst.nii.gz
        
2. Copy all necessary diffusivity images (from TBSS’s DTIFIT, for example) into designated directories in your run_tbss/ folder.

    - We will assume your diffusivity files are located in dtifit_folder but make sure to correct this to reflect your naming convention
    - Mean and axial diffusivities are output as part of DTIFIT, but here we will compute the radial diffusivity as a mean of the second and third eigenvalue images.
    - The following is written in a loop so all subjects run in series, however this can be parallelized
    - Use the latest FSL version 5.0.7 if you have it, but the same code will work for older versions as well.

```
FSLDIR=/usr/local/fsl-5.0.7/
 
ENIGMAtemplateDirectory=/enigmaDTI/TBSS/ENIGMA_targets/
parentDirectory=/enigmaDTI/TBSS/run_tbss/
dtifit_folder=/enigmaDTI/DTIFIT/
 
mkdir ${parentDirectory}/MD/
mkdir ${parentDirectory}/AD/
mkdir ${parentDirectory}/RD/
 
cd $parentDirectory
 
 
for subj in subj_1 subj_2 … subj_N
do
   cp ${dtifit_folder}/${subj}*_MD.nii.gz ${parentDirectory}/MD/${subj}_MD.nii.gz
   cp ${dtifit_folder}/${subj}*_L1.nii.gz ${parentDirectory}/AD/${subj}_AD.nii.gz
   $FSLDIR/bin/fslmaths ${dtifit_folder}/${subj}*_L2.nii.gz –add ${dtifit_folder}/${subj}*_L3.nii.gz \\
       -div 2 ${parentDirectory}/RD/${subj}_RD.nii.gz
 
 
   for DIFF in MD AD RD
   do
   mkdir -p ${parentDirectory}/${DIFF}/origdata/
   mkdir -p ${parentDirectory}/${DIFF}_individ/${subj}/${DIFF}/
   mkdir -p ${parentDirectory}/${DIFF}_individ/${subj}/stats/
 
   $FSLDIR/bin/fslmaths ${parentDirectory}/${DIFF}/${subj}_${DIFF}.nii.gz -mas \\
      ${parentDirectory}/FA/${subj}_FA_FA_mask.nii.gz \\ 
      ${parentDirectory}/${DIFF}_individ/${subj}/${DIFF}/${subj}_${DIFF}
 
   $FSLDIR/bin/immv ${parentDirectory}/${DIFF}/${subj} ${parentDirectory}/${DIFF}/origdata/
 
   $FSLDIR/bin/applywarp -i ${parentDirectory}/${DIFF}_individ/${subj}/${DIFF}/${subj}_${DIFF} -o \\ 
      ${parentDirectory}/${DIFF}_individ/${subj}/${DIFF}/${subj}_${DIFF}_to_target -r \\ 
      $FSLDIR/data/standard/FMRIB58_FA_1mm -w ${parentDirectory}/FA/${subj}_FA_FA_to_target_warp.nii.gz
 
##remember to change ENIGMAtemplateDirectory if you re-masked the template
 
  $FSLDIR/bin/fslmaths ${parentDirectory}/${DIFF}_individ/${subj}/${DIFF}/${subj}_${DIFF}_to_target -mas \\
       ${ENIGMAtemplateDirectory}/ENIGMA_DTI_FA_mask.nii.gz \\
       ${parentDirectory}/${DIFF}_individ/${subj}/${DIFF}/${subj}_masked_${DIFF}.nii.gz	
 
   $FSLDIR/bin/tbss_skeleton -i ./FA_individ/${subj}/FA/${subj}_masked_FA.nii.gz -p 0.049 \\
       ${ENIGMAtemplateDirectory}/ENIGMA_DTI_FA_skeleton_mask_dst.nii.gz $FSLDIR/data/standard/LowerCingulum_1mm.nii.gz \\         
       ${parentDirectory}/FA_individ/${subj}/FA/${subj}_masked_FA.nii.gz  \\ 
       ${parentDirectory}/${DIFF}_individ/${subj}/stats/${subj}_masked_${DIFF}skel -a \\ 
       ${parentDirectory}/${DIFF}_individ/${subj}/${DIFF}/${subj}_masked_${DIFF}.nii.gz -s \\ 
       ${ENIGMAtemplateDirectory}/ENIGMA_DTI_FA_skeleton_mask.nii.gz
 
 
   done
done

```
**Important note:** There was a discrepancy in previous versions of the script where the input `-i` into the `$FSLDIR/bin/tbss_skeleton` command was `${ENIGMAtemplateDirectory}/ENIGMA_DTI_FA.nii.gz.` To remain consistent with the FA projections, we've changed this to `./FA_individ/${subj}/FA/${subj}_masked_FA.nii.gz` in the code above.

Now you should have your diffusivity skeletons!
- Check to make sure all skeletons cover the identical set of voxels, for example:

```
FSLDIR=/usr/local/fsl-5.0.7/
 
${FSLDIR}/bin/fslview  ${parentDirectory}/MD_individ/${subj}/stats/subj_1_masked_MDskel.nii.gz \\
     ${parentDirectory}/FA_individ/subj_1/stats/subj_1_masked_FAskel.nii.gz \\
     ${ENIGMAtemplateDirectory}/ENIGMA_DTI_FA_skeleton.nii.gz
```

Now we can extract ROI measures from these skeletons! Remember you have already done this with FA. Refer to steps 3-6 in [this section](#protocol-for-roi-analysis-using-the-enigma-dti-template).

Below, the runDirectory represents the directory where all your downloaded scripts and codes are located.

```
parentDirectory=/enigmaDTI/TBSS/run_tbss/
runDirectory=/enigmaDTI/TBSS/run_tbss/
 
for DIFF in MD AD RD
do
   mkdir ${parentDirectory}/${DIFF}_individ/${DIFF}_ENIGMA_ROI_part1
   dirO1=${parentDirectory}/${DIFF}_individ/${DIFF}_ENIGMA_ROI_part1/
 
   mkdir ${parentDirectory}/${DIFF}_individ/${DIFF}_ENIGMA_ROI_part2
   dirO2=${parentDirectory}/${DIFF}_individ/${DIFF}_ENIGMA_ROI_part2/
 
   for subject in subj_1 subj_2 … subj_N
     do
 
     ${runDirectory}/singleSubjROI_exe ${runDirectory}/ENIGMA_look_up_table.txt \\ 
         ${runDirectory}/mean_FA_skeleton.nii.gz ${runDirectory}/JHU-WhiteMatter-labels-1mm.nii.gz \\ 
         ${dirO1}/${subject}_${DIFF}_ROIout ${parentDirectory}/${DIFF}_individ/${subject}/stats/${subject}_masked_${DIFF}skel.nii.gz
 
     ${runDirectory}/averageSubjectTracts_exe ${dirO1}/${subject}_${DIFF}_ROIout.csv ${dirO2}/${subject}_${DIFF}_ROIout_avg.csv
 
# can create subject list here for part 3!
     echo ${subject},${dirO2}/${subject}_${DIFF}_ROIout_avg.csv >> ${parentDirectory}/${DIFF}_individ/subjectList_${DIFF}.csv
   done
 
   Table=${parentDirectory}/ROIextraction_info/ALL_Subject_Info.txt 
   subjectIDcol=subjectID
   subjectList=${parentDirectory}/${DIFF}_individ/subjectList_${DIFF}.csv
   outTable=${parentDirectory}/${DIFF}_individ/combinedROItable_${DIFF}.csv
   Ncov=3  #2 if no disease
   covariates="Age;Sex;Diagnosis" # Just "Age;Sex" if no disease
   Nroi="all" 
   rois="all"
 
#location of R binary 
  Rbin=/usr/local/R-2.9.2_64bit/bin/R
 
#Run the R code
  ${Rbin} --no-save --slave --args ${Table} ${subjectIDcol} ${subjectList} ${outTable} \\ 
         ${Ncov} ${covariates} ${Nroi} ${rois} < ${runDirectory}/combine_subject_tables.R
done
```
Congrats! Now you should have all of your subjects ROIs in one spreadsheet per diffusivity measure with only relevant covariates ready for association testing!

