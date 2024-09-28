# Preprocessing Quality Control
#### Protocol for FA and vector alignment QC analysis for ENIGMA-DTI 
The following steps will allow you to visualize your raw FA images before registration to the ENIGMA-DTI template, and to see if your principle direction vectors are appropriately aligned to the white matter tracts.

These protocols are offered with an unlimited license and without warranty. However, if you find these protocols useful in your research, please provide a link to the ENIGMA website in your work: www.enigma.ini.usc.edu

Highlighted portions of the instructions may require you to make changes so that the commands work on your system and data. 

* _Prerequisites_
    * [Matlab](http://www.mathworks.com/products/matlab/) installed
    * Diffusion-weighted images preprocessed using [FSL’s DTIFIT](http://fsl.fmrib.ox.ac.uk/fsl/fsl4.0/fdt/fdt_dtifit.html) or equivalent. This requires the creation of FA maps and eigenvectors comprising of three volumes, the first being the x-component of the eigenvector, the second being the y-component and the third being the z-component.

##### Step 1 – Download the utility packages
* Download the matlab package for Step 3 made available in this repository titled "enigmaDTI_QC"
* Download the script(mac: make_enigmaDTI_FA_V1_QC_webpage_mac.sh), linux: make_enigmaDTI_FA_V1_QC_webpage.sh) to build the QC webpage for Step 4

##### Step 2 – Build a text file defining the location of subject files
* Create a three column tab-delimited text file (e.g. Subject_Path_Info.txt):
    * subjectID: subject ID
    * FAimage: full path to original FA image.
    * V1image: full path to original V1 image. This is a 4D volume that represents the primary eigenvector of the diffusion tensors at every voxel (i.e. x-component of the eigenvector).

      |  subjectID    |          FAimage              |            V1image            |
      | ------------- |:-----------------------------:| -----------------------------:|
      | USC_01        | /path/to/FA/USC_01_FA.nii.gz  | /path/to/V1/USC_01_V1.nii.gz  |
      | USC_02        | /path/to/FA/USC_02_FA.nii.gz  | /path/to/V1/USC_02_V1.nii.gz  |
      | USC_03        | /path/to/FA/USC_03_FA.nii.gz  | /path/to/V1/USC_03_V1.nii.gz  |
      
#### Step 3 – Run Matlab script to make QC images
* Unzip the Matlab scripts from Step 1 and change directories to that folder with the required Matlab *.m scripts. For simplicity, we assume you are working on a Linux machine with the base directory /enigmaDTI/QC_ENIGMA/.

* Make a directory to store all of the QC output:

        mkdir /enigmaDTI/QC_ENIGMA/QC_FA_V1/ 

* Start Matlab:

         /usr/local/matlab/bin/matlab

Next we will run the func_QC_enigmaDTI_FA_V1.m script that reads the Subject_Path_Info.txt file to create subdirectories in a specified output_directory for each individual subjectID, then create an axial, coronal and sagittal image of the FA_image with vectors from the V1_image overlaid on top. The threshold (0 to ~0.3, default 0.2) overlay the V1 information only on voxels with FA of the specified threshold or greater. Increasing the threshold above 0.1 will run the script faster and is recommended for groups with many subjects.

* In the Matlab command window paste and run:
     
        TXTfile='/enigmaDTI/QC_ENIGMA/Subject_Path_Info.txt';
        output_directory='/enigmaDTI/QC_ENIGMA/QC_FA_V1/';
        thresh=0.2;
        [subjs,FAs,VECs]=textread(TXTfile,'%s %s %s','headerlines',1)
        
        for s = 1:length(subjs) 
        subj=subjs(s);
        Fa=FAs(s);
        Vec=VECs(s);
        try
        % reslice FA
        [pathstrfa,nameniifa,gzfa] = fileparts(Fa{1,1});
        [nafa,namefa,niifa] = fileparts(nameniifa);
        newnamegzfa=[pathstrfa,'/',namefa,'_reslice.nii.gz'];
        newnamefa=[pathstrfa,'/',namefa,'_reslice.nii'];
        copyfile(Fa{1,1},newnamegzfa);
        gunzip(newnamegzfa);
        delete(newnamegzfa);
        reslice_nii(newnamefa,newnamefa);
        
        % reslice V1
        [pathstrv1,nameniiv1,gzv1] = fileparts(Vec{1,1});
        [nav1,namev1,niiv1] = fileparts(nameniiv1);
        newnamegzv1=[pathstrv1,'/',namev1,'_reslice.nii.gz'];
        newnamev1=[pathstrv1,'/',namev1,'_reslice.nii'];
        copyfile(Vec{1,1},newnamegzv1);
        gunzip(newnamegzv1);
        delete(newnamegzv1);
        reslice_nii(newnamev1,newnamev1);
        
        % qc
        func_QC_enigmaDTI_FA_V1(subj,newnamefa,newnamev1, output_directory);
        
        close(1)
        close(2)
        close(3)
        
        % delete
        delete(newnamefa)
        delete(newnamev1)
        end
        
        display(['Done with subject: ', num2str(s), ' of ', num2str(length(subjs))]);
        
        end

For troubleshooting individual subjects that func_QC_enigmaDTI_FA_V1.m script can be run in the command console with the following parameters:
``` func_QC_enigmaDTI_FA_V1('subjectID', 'FA_image_path', 'V1_image_path','output_directory') ```

##### Step 4 - Make the QC webpage
Within a terminal session go to the /enigmaDTI/QC_ENIGMA/ directory where you stored the script make_enigmaDTI_FA_V1_QC_webpage.sh and ensure it is executable:
    
    chmod 777 make_enigmaDTI_FA_V1_QC_webpage.sh

or for mac:
    
    chmod 777 make_enigmaDTI_FA_V1_QC_webpage_mac.sh
    
Run the script, specifying the full path to the directory where you stored the Matlab QC output files:

    ./make_enigmaDTI_FA_V1_QC_webpage.sh /enigmaDTI/QC_ENIGMA/QC_FA_V1/ 
    
or for mac:
    
    sh make_enigmaDTI_FA_V1_QC_webpage_mac.sh /enigmaDTI/QC_ENIGMA/QC_FA_V1/
    
This script will create a webpage called enigmaDTI_FA_V1_QC.html in the same folder as your QC output. To open the webpage in a browser in a Linux environment type:

    firefox /enigmaDTI/QC_ENIGMA/QC_FA_V1/enigmaDTI_FA_V1_QC.html
    
Scroll through each set of images to check that the vector directions are correct. For closer inspection, clicking on a subject’s preview image will provide a larger image. If you want to check the segmentation on another computer, you can just copy over the whole /enigmaDTI/QC_ENIGMA/QC_FA_V1/ output folder to your computer and open the webpage from there.

Congrats! Now you should have all you need to make sure your FA images turned out OK and their vector fields are aligned!

