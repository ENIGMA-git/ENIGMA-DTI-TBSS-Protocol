#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
#                        %%%  ENIGMA DTI %%%
#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
#%% This is a function to print out images for Quality Control
#%% of DTI_ENIGMA FA images with TBSS (FSL) skeltons overlaid
#%% as well as JHU atlas ROIs
#%%
#%% Please QC your images to make sure they are
#%% correct FA maps and oriented and aligned properly
#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
#%% Writen by Neda Jahanshad / Derrek Hibar
#%%   last update June 2014
#%%           Questions or Comments??
#%% neda.jahanshad@ini.usc.edu / derrek.hibar@ini.usc.edu
#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

cmdargs = commandArgs(trailingOnly=T);
site=cmdargs[1];  

outD = '.QC_ENIGMA/'
CSVfile ='combinedROItable.csv'
Nrois = 63;
rois = "AverageFA;BCC;GCC;SCC;CC;CGC;CGH;CR;EC;FX;FXST;IC;IFO;PTR;SFO;SLF;SS;UNC;CST;ACR;ALIC;PCR;PLIC;RLIC;SCR;ACR.L;ACR.R;ALIC.L;ALIC.R;CGC.L;CGC.R;CGH.L;CGH.R;CR.L;CR.R;CST.L;CST.R;EC.L;EC.R;FX.ST.L;FX.ST.R;IC.L;IC.R;IFO.L;IFO.R;PCR.L;PCR.R;PLIC.L;PLIC.R;PTR.L;PTR.R;RLIC.L;RLIC.R;SCR.L;SCR.R;SFO.L;SFO.R;SLF.L;SLF.R;SS.L;SS.R;UNC.L;UNC.R"
outPDF = 'ENIGMA_DTI_allROI_histograms.pdf'
outTXT = 'ENIGMA_DTI_allROI_stats.txt'

###########################################################################

dir.create(outD)

Table <- read.csv(CSVfile,header=T);
colTable = names(Table);
print(colTable)
## assigning all rows that have a value of "x" or "X" to "NA"
for (m in seq(1,length(colTable)))
{
	ind = which(Table[,m]=="x");
	ind2 = which(Table[,m]=="X");
	Table[ind] = "NA"
	Table[ind2] = "NA"
}

##get rid of all rows with NAs in them
INDX=which(apply(Table,1,function(x)any(is.na(x))));

##get rid of all rows with NAs in them
if (length(INDX) >0 )
{
	Table<-Table[-which(apply(Table,1,function(x)any(is.na(x)))),]
}

## parsing through the inputted list of ROIs
if (Nrois > 0)
{
	pdf(file=outPDF);
	
	parsedROIs = parse(text=rois);
    print(parsedROIs)
    write("Structure\tNumberIncluded\tMean\tStandDev\tMaxValue\tMinValue\tMaxSubject\tMinSubject\t5StDev_Off", file = outTXT);
    
	for (x in seq(1,Nrois,1))
	{
		
	ROI <- as.character(parsedROIs[x]);
	
	DATA = Table[ROI];
	DATA = unlist(DATA);
	DATA = as.numeric(as.vector(DATA));
	
	mu = mean(DATA);
	sdev = sd(DATA);
	N = length(DATA);
    
    outliers<-"";
	
    hbins = 20; #floor(N/10);
	
	maxV = max(DATA);
	minV = min(DATA);
	
	i =which(DATA==maxV)
	maxSubj = as.character(Table[i,1])
	
	j =which(DATA==minV)
	minSubj = as.character(Table[j,1])
    
    minO=which(DATA < mu - 5*sdev)
    maxO=which(DATA > mu + 5*sdev)
    
    if (length(minO) + length(maxO) > 0 ) {
        outliers<-"Outliers (5-sd):";
        for (ii in 1:length(minO)){
        outliers=paste(outliers,as.character(Table[minO[ii],1]),sep=",")
        }
        for (ii in 1:length(maxO)){
        outliers=paste(outliers,as.character(Table[maxO[ii],1]),sep=",")
        }}
    
    outliers=as.character(outliers)
	
	stats = c(ROI, N, mu, sdev, maxV, minV, maxSubj, minSubj,outliers);
	
	
	write.table(t(as.matrix(stats)),file = outTXT, append=T, quote=F, col.names=F,row.names=F, sep="\t");
		
	hist(DATA, breaks = hbins, main = paste(site,ROI));
	
	## uncomment the following 3 lines if you want to output individual histogram PNGs for each inputted ROI
        # png(paste(outD,ROI,"hist_data.png"));
        # hist(DATA, breaks = hbins, main = ROI);
        # dev.off()
	}
dev.off()
}
