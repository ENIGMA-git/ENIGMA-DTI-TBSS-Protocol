# Protocol for TBSS analysis using the ENIGMA-DTI template
##### (Last update April 2014)

Neda Jahanshad, Emma Sprooten, Peter Kochunov
neda.jahanshad@ini.usc.edu, emma.sprooten@yale.edu

The following steps will allow you to register and skeletonize your FA images to the DTI atlas being used for ENIGMA-DTI for tract-based spatial statistics (TBSS; Smith et al., 2006).

Here we assume preprocessing steps including motion/Eddy current correction, masking, tensor calculation, and creation of FA maps has already been performed, along with quality control.

Further instructions for using FSL, particularly TBSS can be found on the website: http://www.fmrib.ox.ac.uk/fsl/tbss/index.html

1. Download a copy of the ENIGMA-DTI template FA map, edited skeleton, masks and corresponding distance map from this repository (folder titled _enigmaDTI_). The downloaded archive will have the following files:
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

        tbss_2_reg –t ENIGMA_DTI_FA.nii.gz
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

**NOTE:** For the following steps, if you use the ENIGMA mask and distance map as provided, in the commands for steps 7 and 8 replace: `/enigmaDTI/TBSS/ENIGMA_targets_edited/mean_FA_mask.nii.gz` with
`/enigmaDTI/TBSS/ENIGMA_targets/ENIGMA_DTI_FA.nii.gz` and `/enigmaDTI/TBSS/ENIGMA_targets_edited/mean_FA_skeleton_mask_dst` with `/enigmaDTI/TBSS/ENIGMA_targets/ENIGMA_DTI_FA_skeleton_mask_dst`


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