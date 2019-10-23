/*
 *  singleSubject_FA_ROI.cpp
 *  Created by Peter Kochunov <pkochunov@gmail.com>
 *  and Neda Jahanshad <neda.jahanshad@loni.ucla.edu>
 *  2012 ENIGMA-DTI
 *
 */



#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
\
#include "niftiio/nifti1_io.h" 
#include "fslio/fslio.h"
#include "znzlib/znzlib.h"
#include "fslio/dbh.h"
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

#include <math.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <vector>
#include <set>

	//

using namespace std;
int nx,ny,nz,nv;
	//float ***mask, ***skeleton, ***data;

	//http://www.linuxselfhelp.com/HOWTO/C++Programming-HOWTO-7.html
int  tokenize(const string& str, string *tokens, const string& delimiters = " ")
{
		// Skip delimiters at beginning.
    string::size_type lastPos = str.find_first_not_of(delimiters, 0);
		// Find first "non-delimiter".
    string::size_type pos     = str.find_first_of(delimiters, lastPos);
    int i=0;
    while (string::npos != pos || string::npos != lastPos)
    {
			// Found a token, add it to the vector.
        tokens[i]=str.substr(lastPos, pos - lastPos);
			// Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
			// Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
        i++;
        if (i>=9) return i;
    }
	return i;
}

void Intersection (const set<long>& A, const set<long>&B, set<long>& C) {
	C.clear();
	set<long>::iterator si;
	for (si=A.begin(); si !=A.end(); si++){
		if (B.count(*si)>0) {
			C.insert(*si);
		}
	}
}

string int2string(float tmp)
{
	ostringstream oss;
	oss<<tmp;
	return oss.str();
	
}


	//void read_reshape3D (char file_name[], float ***vol3D) {
	float*** read_reshape3D (char file_name[]) {
		// read in image -- need to do this 3 times
		float*** vol3D;
			//float *buffer;
		void *buffer;
		double dx1;
		//double image;
		int nvols=1;
		short nxs,nys,nzs,nvs,t;
		size_t voxsize,bufsz;
		FSLIO *fslio;
		fslio = FslOpen(file_name,"rb");
		//cout<<"opened file"<< file_name <<endl;
		//... can now access header info via the FslGet calls ...
		FslGetDim(fslio, &nxs, &nys, &nzs, &nvs);
		nx=nxs; ny=nys; nz=nzs;nv=nvs;
		//cout<<"found dementions " << nx << " "<< ny << " "<< nz <<endl;
		voxsize = FslGetDataType(fslio, &t) / 8;  
		bufsz = nxs * nys * nzs * voxsize;
		//	cout<<"found voxsize " << voxsize <<endl;
		//	cout<<"found bufsz " << bufsz <<endl;
			//buffer = (float *) calloc(bufsz, voxsize);
		buffer =calloc(bufsz, voxsize);
			//cout <<sizeof(float)<<endl;
			//buffer = calloc(bufsz, sizeof(float));
			//... allocate room for buffer ...
		FslReadVolumes(fslio,buffer,nvols);
			//buffer =(float *) buffer;
	int i, j, k;
    vol3D = new float **[nx];	
    for(i = 0; i < nx; i++){
        vol3D[i] = new float*[ny];
        for(j = 0; j < ny; j++){
            vol3D[i][j] = new float[nz];    
        }
    }
		//nifti_image image = fslio.niftiptr;
    for( k = 0; k < nz; k++){ 
        for( j = 0; j < ny; j++){
            for( i = 0; i < nx; i++)    
            {
                long index = (k * ny * nx) + (j * nx) + i;
				
				if (voxsize==4){
					dx1 = ((float*)buffer)[index]; 
				} else if (voxsize ==2) {
					dx1 = ((short*)buffer)[index];
				} else if (voxsize ==1) {
					dx1 = ((char*)buffer)[index];
				} else  {cerr<<"Wrong image format."<<endl;exit(1);}
					//dx1 = buffer[index];
					vol3D[i][j][k] = dx1; 
					//vol3D[i][j][k] = niftiI<-nifti_image.data[index];
            }
        }    
    }
	
		FslClose(fslio);
free (buffer);
		//cout<<"closed"<<endl;
		return (vol3D);
}

float* average_skeleton_fa(float ***skeleton, float ***data,int nx, int ny, int nz)
{
	float* pointer;
	float info[1];
	pointer=info;
	float average=0, count=0;
	for (int i=0; i<nx;i ++)
        for(int j=0; j<ny;j++)
			for (int k=0;k<nz;k++)
			{
                if (skeleton[i][j][k]>0 && data[i][j][k]>0 ) {
					average+= data[i][j][k];
					count++;
				}
			}
		//cout<<"Count,"<<count<<endl;
	info[0]=average/count;
	info[1]=count;
		//cout<<"Average,"<<average<<endl;
		//cout<<"AverageFA,"<<info[0]<<","<<info[1]<<endl;
	if (count>0) return pointer;
	else {cerr<<"No values were found for this track"<<endl; }
	
}

float* average_tract_fa(int track_code, float ***mask, float ***skeleton, float ***data, int nx, int ny, int nz)
{
	float* pointer;
	float info[1];
	pointer=info;
	float average=0, count=0;
	for (int i=0; i<nx;i ++)
        for(int j=0; j<ny;j++)
			for (int k=0;k<nz;k++)
			{
                if (mask[i][j][k] ==track_code && data[i][j][k]>0  && skeleton[i][j][k]>0) {
					average+= data[i][j][k];
					count++;
				}
			}
	info[0]=average/count;
	info[1]=count;
	
	if (count>0) return pointer;
	else {cerr<<"No values were found for this track"<<endl; }
}

float* average_ROI_fa(int track_code, float ***mask, float ***data, int nx, int ny, int nz)
{
	float* pointer;
	float info[1];
	pointer=info;
	float average=0, count=0;
	for (int i=0; i<nx;i ++)
        for(int j=0; j<ny;j++)
			for (int k=0;k<nz;k++)
			{
                if (mask[i][j][k] ==track_code && data[i][j][k]>0) {
					average+= data[i][j][k];
					count++;
				}
			}
	info[0]=average/count;
	info[1]=count;
	
	if (count>0) return pointer;
	else {cerr<<"No values were found for this track"<<endl; }
}

void create_subj_table (string OUTputTable, float ***skeleton, float ***mask,  float ***data, int *label_code, string *label_label, int total_labels)

{
	ifstream fin(OUTputTable.c_str());
    if (fin)
    {
		cout << "Output file already exists, deleting..." << endl;
		remove (OUTputTable.c_str());
		fin.close();
    }
	
	
	ofstream table1;
	table1.open (OUTputTable.c_str(),ios::out | ios::app);
	table1 << "Tract," << "Average," << "nVoxels\n" ;
		//cout << "Tract," << "Average," << "nVoxels" << endl;
	float *FAinfo =average_skeleton_fa(skeleton, data,nx, ny, nz);
		//cout<<"AverageFA,"<<FAinfo[0]<<","<<FAinfo[1]<<endl;
	table1<<"AverageFA,"<<FAinfo[0]<<","<<FAinfo[1]<<"\n";
	for (int i=1; i<total_labels; i++) {
		float *tractinfo =average_tract_fa(label_code[i],mask,skeleton,data,nx, ny, nz);
		table1 << label_label[i]<<","<<tractinfo[0]<<","<<tractinfo[1]<<"\n"; 
			//cout<< label_label[i]<<","<<tractinfo[0]<<","<<tractinfo[1]<<endl;
	}	
	table1.close();
}


void usage()
{
	cerr<<"meanROIs_exe LabelsFile MASK.nii ROIAtlas.nii OutputCSV InputImage "<<endl;
	cerr<<"This code will print out the average FA value along a skeleton and ROI values on that skeleton according to an ROI atlas, for example the JHU-ROI atlas (provided with this code)."<<endl;
	cerr<<"The look up table for the atlas should be tab-delimited --  the first column should refer to the voxel value within the image and the second column should refer to the desired label of the region."<<endl;
	
	cerr<<"./singleSubjROI_exe look_up_table.txt skeleton.nii.gz JHU-WhiteMatter-labels-1mm.nii.gz OutputfileName Subject_FAskeleton.nii.gz"<<endl;
	
	cerr<<"example -- ./singleSubjROI_exe ENIGMA_look_up_table.txt mean_FA_skeleton.nii.gz JHU-WhiteMatter-labels-1mm.nii.gz Subject*_ROIout Subject*_FA.nii.gz"<<endl;
	exit(1);
}


int main(int argc, char **argv)
{
	if (argc<6 || argc>6) usage();
	
		/////////////////////
		/// Look up file ////
		/////////////////////
	int label_code[70];
	string label_label[70];
	cerr<<"Reading Look-up file " << argv[1]<<endl;
	ifstream h2_in(argv[1]);
	string tmp("");
	int label_count=0;
	label_code[label_count]=0;
	label_label[label_count++]="WholeSkeleton";
	string OutputCSV=argv[4];
	
	while ( getline(h2_in, tmp) && label_count<70)
	{
			//cerr<<"read in "<<tmp<<endl;
		string tokens[10];
		int number=0;
		number = tokenize(tmp, tokens, "\t");
		if (number>2)
		{
			label_code[label_count]=atoi(tokens[0].c_str());
			label_label[label_count]=tokens[1];
			
				//	cerr<<label_code[label_count]<<"\t"<<label_label[label_count]<<endl;
			label_count++;
		}
		else {cerr<<"Wrong table format. Only found "<<number<<endl;exit(1);}
	}
	
	cerr<<"Read a total of "<<label_count<<" labels"<<endl;
	
		/////////////////////
		/// Read in Label Atlas and Skeleton ////
		/////////////////////
	float ***mask, ***skeleton, ***data;
	cerr<<"Reading in Skeleton " <<argv[2]<<endl;
		//read_reshape3D (argv[2], mask) ;
	skeleton=read_reshape3D (argv[2]);
	cerr<<"Reading in Label Atlas/mask " <<argv[3]<<endl;
		//read_reshape3D (argv[3], skeleton) ;
	mask=read_reshape3D (argv[3]);
	cerr<<"Reading in subject image " <<argv[5]<<endl;
		//read_reshape3D (argv[5], data) ;
	data=read_reshape3D (argv[5]);
	
//	cout<< "some mask value" <<mask[90][90][90]<<endl;
//	cout<< "some skel value" <<skeleton[90][90][90]<<endl;
//	cout<< "some data value" <<data[90][90][90]<<endl;
		/////////////////////
		/// Find ROI values ////
		/////////////////////
		//int N_vols=argc-5;
	int N_col_per_table= label_count;
		// lets see how many voxels will have the result
	int N_col_total=  label_count+1;
//	cerr<<"There will be total of "<<N_col_total<<" labels"<<endl;
//	if (N_col_total<1) {cerr<<" Please change the threshold" <<endl;exit(1);}
		//mask.write_volume("tmp");
		//cerr<<"Allocating memory for this dataset ...";
		//table_column columns[N_col_total*2];
		//for (int i=0; i< N_col_total;i++){ columns[i].init(N_vols*2); for (int j=0;j<N_vols*2;j++) columns[i].array[j]=0;}
		//cerr<<"   done"<<endl;
	
	
		/////////////////////
		/// Make Table ////
		/////////////////////
	cerr<<"Creating table " << OutputCSV <<".csv"<<endl;
		//for (int i=0; i<N_col_total;i+= N_col_per_table)
			//{
        string f_name=OutputCSV+".csv";
        cerr<<"Writing "<<f_name<<endl;

		create_subj_table(f_name, skeleton, mask, data, label_code, label_label,label_count);
		
		//}	
}


