#/*
# * combine_subject_tables.R
# * 
# * Created by Neda Jahanshad - neda.jahanshad@ini.usc.edu
# * ENIGMA_DTI
# */

#################
cmdargs = commandArgs(trailingOnly=T);
TableFile=cmdargs[1]
subjectIDcol=cmdargs[2]
subjectList	=cmdargs[3]
outTable=cmdargs[4]
Ncov=as.numeric(cmdargs[5]);
covariates=cmdargs[6]
Nroi=as.numeric(cmdargs[7]);
NroiC=cmdargs[7];
rois=cmdargs[8]
#################
#Table<-"./ALL_INFO.txt"
#subjectIDcol="subjectID"
#subjectList="./subjectList.csv"
#outTable="./FULL_ROItable.csv"
#Ncov=2
#ovariates="Age;Sex"
#roi=2
#NroiC="2"
#rois="IC;EC"
################
if (substring(TableFile,nchar(TableFile)-3,nchar(TableFile))==".csv"){
	tokT=",";
} else {tokT="\t"} 
if (substring(subjectList,nchar(subjectList)-3,nchar(subjectList))==".csv"){
	tokS=",";
} else {tokS="\t"}

###################
##filter out table to include only variables of interest
###################
Table<-read.table(TableFile,sep=tokT,header=T,blank.lines.skip = TRUE)
columnnames<-colnames(Table)
#print(columnnames)
#print(subjectIDcol)
DesignMatrix<-data.frame(subjectID=Table[,which(columnnames==subjectIDcol)])
#print(DesignMatrix$subjectID)

if (Ncov > 0) {
	parsedCov=parse(text=covariates)
	for (nc in 1:Ncov) {
		covName<-as.character(parsedCov[nc])
		print(covName)
		origcolnames = colnames(DesignMatrix);
		DesignMatrix[,length(DesignMatrix)+1] = Table[,which(columnnames==covName)]
		colnames(DesignMatrix)<-c(origcolnames,covName)
	}
}

iNA=which(apply(DesignMatrix,1,function(x)any(is.na(x))));
if (length(iNA > 0)) {
	DesignMatrix<-DesignMatrix[-which(apply(DesignMatrix,1,function(x)any(is.na(x)))),]
}


##############
### match subjects in subjectList
##############

### match file for subjects 
subjList<-data.frame(read.table(subjectList,sep=tokS,header=FALSE,blank.lines.skip = TRUE))
files=subjList[,2]
matchind = match(DesignMatrix$subjectID,subjList[,1]);
#print(as.character(subjList[,1]))

### for each subject ID, read file and add columns corresponding to ROIs
done=0;
for (i in 1:length(as.vector(matchind)) ) {
	f_indx = matchind[i]
	### read file
	Rfile=as.character(files[f_indx]);

	if (file.exists(Rfile)==1) {
		if(done==0) {
		if (NroiC=="all") {
			ROImatrix<-data.matrix(read.table(as.character(Rfile),sep=",",header=T,blank.lines.skip = TRUE,na.strings = "NaN",row.names=1))
			rownames=row.names(ROImatrix)
			for (r in 1:length(rownames)) {
			origcolnames = colnames(DesignMatrix);
			DesignMatrix[,length(DesignMatrix)+1] = rep(NA,length(as.vector(matchind)))
			colnames(DesignMatrix)<-c(origcolnames,rownames[r])
			}
#DesignMatrix$Tract=NULL
		} else if (Nroi > 0) {
			parsedROIs=parse(text=rois)
			for (nc in 1:Nroi) {
				roiName<-as.character(parsedROIs[nc])
				print(roiName)
				origcolnames = colnames(DesignMatrix);
				DesignMatrix[,length(DesignMatrix)+1] = rep(NA,length(as.vector(matchind)))
				colnames(DesignMatrix)<-c(origcolnames,roiName)
			}
		}
		done=1;
	}
	
	ROImatrix<-data.matrix(read.table(as.character(Rfile),sep=",",header=T,blank.lines.skip = TRUE,na.strings = "NaN",row.names=1))
	rownames=row.names(ROImatrix)
		origcolnames = colnames(DesignMatrix);
	if (NroiC=="all") {
		rownames=row.names(ROImatrix)
		for (j in 1:length(rownames)) {
		roiName<-as.character(rownames[j])
#if (roiName !="Tract") {
				DesignMatrix[i,which(origcolnames==roiName)] = ROImatrix[which(rownames==roiName),1]
#}
		}
	} else if (Nroi > 0) {
		parsedROIs=parse(text=rois)
		for (nc in 1:Nroi) {
			roiName<-as.character(parsedROIs[nc])
			DesignMatrix[i,which(origcolnames==roiName)] = ROImatrix[which(rownames==roiName),1]
		}
	}
	}
}

iNA=which(apply(DesignMatrix,1,function(x)any(is.na(x))));
if (length(iNA > 0)) {
	DesignMatrix<-DesignMatrix[-which(apply(DesignMatrix,1,function(x)any(is.na(x)))),]
}

write.table(DesignMatrix,outTable,sep=",",row.names=F,col.names=T,quote=F);
