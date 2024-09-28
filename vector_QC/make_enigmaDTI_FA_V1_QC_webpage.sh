#!/bin/bash
#$ -S /bin/bash

###### no need to edit this script
###### to run >> ./make_enigmaDTI_FA-V1_QC_webpage.sh ${dirO}  (where $dirO is the output directory specified for the Matlab script)

QC_directory=$1
cd $QC_directory
subject_list=`ls -d */`

echo "<html>" 												>  enigmaDTI_FA-V1_QC.html
echo "<head>"                                                   >> enigmaDTI_FA-V1_QC.html
echo "<style type=\"text/css\">"								>> enigmaDTI_FA-V1_QC.html
echo "*"                                                        >> enigmaDTI_FA-V1_QC.html
echo "{"														>> enigmaDTI_FA-V1_QC.html
echo "margin: 0px;"												>> enigmaDTI_FA-V1_QC.html
echo "padding: 0px;"											>> enigmaDTI_FA-V1_QC.html
echo "}"														>> enigmaDTI_FA-V1_QC.html
echo "html, body"												>> enigmaDTI_FA-V1_QC.html
echo "{"														>> enigmaDTI_FA-V1_QC.html
echo "height: 100%;"											>> enigmaDTI_FA-V1_QC.html
echo "}"														>> enigmaDTI_FA-V1_QC.html
echo "</style>"													>> enigmaDTI_FA-V1_QC.html
echo "</head>"													>> enigmaDTI_FA-V1_QC.html

echo "<body>" 													>>  enigmaDTI_FA-V1_QC.html


for sub in ${subject_list};
do
echo "<table cellspacing=\"1\" style=\"width:100%;background-color:#000;\">"				>> enigmaDTI_FA-V1_QC.html
echo "<tr>"																					>> enigmaDTI_FA-V1_QC.html
echo "<td> <FONT COLOR=WHITE FACE=\"Geneva, Arial\" SIZE=5> $sub </FONT> </td>"             >> enigmaDTI_FA-V1_QC.html
echo "</tr>"                                                                                >> enigmaDTI_FA-V1_QC.html
echo "<tr>"                                                                                 >> enigmaDTI_FA-V1_QC.html
echo "<td><a href=\"file:"$sub"/Axial_V1_check.png\"><img src=\""$sub"/Axial_V1_check_lowRez.png\" width=\"100%\" ></a></td>"	>> enigmaDTI_FA-V1_QC.html
echo "<td><a href=\"file:"$sub"/Coronal_V1_check.png\"><img src=\""$sub"/Coronal_V1_check_lowRez.png\" width=\"100%\" ></a></td>"	>> enigmaDTI_FA-V1_QC.html
echo "<td><a href=\"file:"$sub"/Sagittal_V1_check.png\"><img src=\""$sub"/Sagittal_V1_check_lowRez.png\" width=\"100%\" ></a></td>"	>> enigmaDTI_FA-V1_QC.html
echo "</tr>"																				>> enigmaDTI_FA-V1_QC.html
echo "</table>"                                                                             >> enigmaDTI_FA-V1_QC.html
done;

echo "</body>"                                                                              >> enigmaDTI_FA-V1_QC.html
echo "</html>"                                                                              >> enigmaDTI_FA-V1_QC.html


