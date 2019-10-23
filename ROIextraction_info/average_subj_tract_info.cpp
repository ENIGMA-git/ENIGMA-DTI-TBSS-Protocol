/*
 *  Written by Neda Jahanshad <neda.jahanshad@ini.usc.edu>
 *  2012 ENIGMA-DTI
 *  edited in 2013 to revisit additional ROIs
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

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
#include <map>
#include <stdarg.h>


	/// will match images/text files with subject information from big spreadsheet
	/// will columnize rows/columns of interest from text files
	/// perform weighted averages of files and add in columns

	// http://www.linuxselfhelp.com/HOWTO/C++Programming-HOWTO-7.html
using namespace std;


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


void map_individual_file (char file_path[],map<string,float> &ROImap_u, map<string,float> &ROImap_w) 
{
		// split columns up
	float label_avg[70], label_wt[70];
	string label_label[70];
	cout <<"Reading subject ROI file " << file_path<<endl;
	ifstream h2_in(file_path);
	string tmp("");
	int label_count=0;
	
		//map<string,float> ROImap_u, ROImap_w;
	map<string,float> ::iterator it;
	
	while ( getline(h2_in, tmp) && label_count<70)
	{
		if (label_count ==0) {
			label_count++;
		} else {
	
		float* pointer;
		float info[1];
		info[0]=0.0;info[1]=1.0;
		string tokens[10];
		int number=0;
		number = tokenize(tmp, tokens, ",");
		if (number>2)
		{
			info[0]=atof(tokens[1].c_str());
			info[1]=atof(tokens[2].c_str());
				//pointer=info;
			ROImap_u[tokens[0]]=info[0];
			ROImap_w[tokens[0]]=info[1];
				//cout<< label_count <<" "<<info[0]<<" 2) "<<info[1] <<endl;
			label_count++;
				}
		else {cerr<<"Wrong table format. Only found "<<number<< " columns!"<<endl;exit(1);}
		} }
	cerr<<"Read a total of "<<label_count<<" labels"<<endl;	
		//
		//for ( it=ROImap_u.begin() ; it != ROImap_u.end(); it++ )
		//cout << (*it).first << " => " << (*it).second << endl;
}

void wt_avg_roi_values (map<string,float> ROImap_u, map<string,float> ROImap_w,float &averageROI, float &nvtotal, int nROIs, string ROI1,string ROI2, ...)
{
		//float* pointer;
		//float info[1];
		//pointer=info;
	float sumROI; //,averageROI,nvtotal;
	va_list vl;
	string ROI_rest;
	
		//cout << "means" << ROImap_u.find(ROI1)->second << " "<< ROImap_u.find(ROI2)->second << endl;
		//cout << "weights" << ROImap_w.find(ROI1)->second << " "<< ROImap_w.find(ROI2)->second << endl;
	sumROI=ROImap_u.find(ROI1)->second * ROImap_w.find(ROI1)->second + ROImap_u.find(ROI2)->second * ROImap_w.find(ROI2)->second;
	nvtotal=ROImap_w.find(ROI1)->second + ROImap_w.find(ROI2)->second;
	
	if (nROIs >2) {
		va_start(vl,ROI2);
		for (int i = 0 ; i < (nROIs -2); i++) {
			ROI_rest=(string)va_arg(vl,char*);
				//cout << ROI_rest << endl;
			
			sumROI=sumROI + ((ROImap_u.find(ROI_rest)->second) * (ROImap_w.find(ROI_rest)->second));
			nvtotal=nvtotal + ROImap_w.find(ROI_rest)->second;
			
				//	cout << "add mean " << ROImap_u.find(ROI_rest)->second << endl;
				//	cout << "add weight " << ROImap_w.find(ROI_rest)->second << endl;
			
		}
		va_end (vl);
	}
	
	
	averageROI = sumROI/nvtotal;

}


using namespace std;
int nx,ny,nz,nv;
	//float ***mask, ***skeleton, ***data;

string int2string(float tmp)
{
	ostringstream oss;
	oss<<tmp;
	return oss.str();
	
}


void create_subj_table (string OUTputTable, map<string,float> &ROImap_u, map<string,float> &ROImap_w )

{
	map<string,float> ::iterator it1, it2;
	
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
	
	it2=ROImap_w.begin() ;
	for ( it1=ROImap_u.begin() ; it1 != ROImap_u.end(); it1++ ) {
		table1 << (*it1).first << "," << (*it1).second << "," << (*it2).second<< endl;
		it2++;
	}
	
	table1.close();
}



void usage()
{
	cerr<<"program_exe inSubjectROIfile.csv outSubjectROIfile.csv "<<endl;
	exit(1);
}

int main(int argc, char **argv)
{
	if (argc<3 || argc>3) usage();
	
		/////////////////////
		/// Look up file ////
		/////////////////////
		//int label_code[70];
		//string label_label[70];
	float info[1];
	float* avgInfo;
		//info=avgInfo;
	
	map<string,float> ROImap_u,ROImap_w;
	map<string,float>::iterator it;
	map<string,float>::key_compare mycomp;
	mycomp = ROImap_u.key_comp();

	map_individual_file (argv[1], ROImap_u, ROImap_w);
	 
		//float rICm=ROImap_u.find("ALIC-R")->second;
		//float rICw=ROImap_w.find("ALIC-R")->second;
	
		////// erase small ROIs
	
	ROImap_w.erase ( "ML-R" );ROImap_w.erase ( "ML-L" );
	ROImap_w.erase ( "ICP-R" );ROImap_w.erase ( "ICP-L" );
	ROImap_w.erase ( "SCP-R" );ROImap_w.erase ( "SCP-L" );
	ROImap_w.erase ( "CP-R" );ROImap_w.erase ( "CP-L" );
//	ROImap_w.erase ( "CGH-R" );ROImap_w.erase ( "CGH-L" );
//	ROImap_w.erase ( "FX/ST-R" );ROImap_w.erase ( "FX/ST-L" );
	
	ROImap_u.erase ( "ML-R" );ROImap_u.erase ( "ML-L" );
	ROImap_u.erase ( "ICP-R" );ROImap_u.erase ( "ICP-L" );
	ROImap_u.erase ( "SCP-R" );ROImap_u.erase ( "SCP-L" );
	ROImap_u.erase ( "CP-R" );ROImap_u.erase ( "CP-L" );
//	ROImap_u.erase ( "CGH-R" );ROImap_u.erase ( "CGH-L" );
//	ROImap_u.erase ( "FX/ST-R" );ROImap_u.erase ( "FX/ST-L" );
	
	
	float new_u,new_w;
	
		////// internal capsule
	cout <<" Averaging ALIC-R PLIC-R RLIC-R"<< endl;
		//avgInfo = wt_avg_roi_values(ROImap_u,ROImap_w,3,"ALIC-R","PLIC-R","RLIC-R");
	wt_avg_roi_values(ROImap_u,ROImap_w,new_u,new_w,3,"ALIC-R","PLIC-R","RLIC-R");
		//cout <<" we get " << avgInfo[0] << "; Nvox= " << avgInfo[1] << endl;
		//new_u=(float)avgInfo[0]; new_w=(float)avgInfo[1]; 
	ROImap_u.insert ( pair<string,float>("IC-R",new_u) );
	ROImap_w.insert ( pair<string,float>("IC-R",new_w) );
	
	cout <<" Averaging ALIC-L PLIC-L RLIC-L"<< endl;
	wt_avg_roi_values(ROImap_u,ROImap_w,new_u,new_w,3,"ALIC-L","PLIC-L","RLIC-L");
		//cout <<" we get " << avgInfo[0] << "; Nvox= " << avgInfo[1] << endl;
		//new_u=avgInfo[0]; new_w=avgInfo[1]; 
	ROImap_u.insert ( pair<string,float>("IC-L",new_u) );
	ROImap_w.insert ( pair<string,float>("IC-L",new_w) );
    
    cout <<" Averaging ALIC-L ALIC-R"<< endl;
	wt_avg_roi_values(ROImap_u,ROImap_w,new_u,new_w,2,"ALIC-L","ALIC-R");
    //cout <<" we get " << avgInfo[0] << "; Nvox= " << avgInfo[1] << endl;
    //new_u=avgInfo[0]; new_w=avgInfo[1];
	ROImap_u.insert ( pair<string,float>("ALIC",new_u) );
	ROImap_w.insert ( pair<string,float>("ALIC",new_w) );
    
    cout <<" Averaging PLIC-L PLIC-R"<< endl;
	wt_avg_roi_values(ROImap_u,ROImap_w,new_u,new_w,2,"PLIC-L","PLIC-R");
    //cout <<" we get " << avgInfo[0] << "; Nvox= " << avgInfo[1] << endl;
    //new_u=avgInfo[0]; new_w=avgInfo[1];
	ROImap_u.insert ( pair<string,float>("PLIC",new_u) );
	ROImap_w.insert ( pair<string,float>("PLIC",new_w) );
    
    cout <<" Averaging RLIC-L RLIC-R"<< endl;
	wt_avg_roi_values(ROImap_u,ROImap_w,new_u,new_w,2,"RLIC-L","RLIC-R");
    //cout <<" we get " << avgInfo[0] << "; Nvox= " << avgInfo[1] << endl;
    //new_u=avgInfo[0]; new_w=avgInfo[1];
	ROImap_u.insert ( pair<string,float>("RLIC",new_u) );
	ROImap_w.insert ( pair<string,float>("RLIC",new_w) );
    
	
	cout <<" Averaging ALIC PLIC RLIC R and L"<< endl;
	wt_avg_roi_values(ROImap_u,ROImap_w,new_u,new_w,6,"ALIC-R","PLIC-R","RLIC-R","ALIC-L","PLIC-L","RLIC-L");
		//cout <<" we get " << avgInfo[0] << "; Nvox= " << avgInfo[1] << endl;
		//new_u=avgInfo[0]; new_w=avgInfo[1]; 
	ROImap_u.insert ( pair<string,float>("IC",new_u) );
	ROImap_w.insert ( pair<string,float>("IC",new_w) );
	
		/////// corona radiata
	cout <<" Averaging ACR-R SCR-R PCR-R"<< endl;
	wt_avg_roi_values(ROImap_u,ROImap_w,new_u,new_w,3,"ACR-R","SCR-R","PCR-R");
		//cout <<" we get " << avgInfo[0] << "; Nvox= " << avgInfo[1] << endl;
		//new_u=avgInfo[0]; new_w=avgInfo[1]; 
	ROImap_u.insert ( pair<string,float>("CR-R",new_u) );
	ROImap_w.insert ( pair<string,float>("CR-R",new_w) );
    
    /////// CC
	cout <<" Averaging BCC GCC SCC"<< endl;
	wt_avg_roi_values(ROImap_u,ROImap_w,new_u,new_w,3,"BCC","GCC","SCC");
    //cout <<" we get " << avgInfo[0] << "; Nvox= " << avgInfo[1] << endl;
    //new_u=avgInfo[0]; new_w=avgInfo[1];
	ROImap_u.insert ( pair<string,float>("CC",new_u) );
	ROImap_w.insert ( pair<string,float>("CC",new_w) );
	
	cout <<" Averaging ACR-L SCR-L PCR-L"<< endl;
	wt_avg_roi_values(ROImap_u,ROImap_w,new_u,new_w,3,"ACR-L","SCR-L","PCR-L");
		//cout <<" we get " << avgInfo[0] << "; Nvox= " << avgInfo[1] << endl;
		//new_u=avgInfo[0]; new_w=avgInfo[1]; 
	ROImap_u.insert ( pair<string,float>("CR-L",new_u) );
	ROImap_w.insert ( pair<string,float>("CR-L",new_w) );
    
    cout <<" Averaging ACR-L ACR-R"<< endl;
	wt_avg_roi_values(ROImap_u,ROImap_w,new_u,new_w,2,"ACR-L","ACR-R");
    //cout <<" we get " << avgInfo[0] << "; Nvox= " << avgInfo[1] << endl;
    //new_u=avgInfo[0]; new_w=avgInfo[1];
	ROImap_u.insert ( pair<string,float>("ACR",new_u) );
	ROImap_w.insert ( pair<string,float>("ACR",new_w) );
    
    cout <<" Averaging SCR-L SCR-R"<< endl;
	wt_avg_roi_values(ROImap_u,ROImap_w,new_u,new_w,2,"SCR-L","SCR-R");
    //cout <<" we get " << avgInfo[0] << "; Nvox= " << avgInfo[1] << endl;
    //new_u=avgInfo[0]; new_w=avgInfo[1];
	ROImap_u.insert ( pair<string,float>("SCR",new_u) );
	ROImap_w.insert ( pair<string,float>("SCR",new_w) );
    
    cout <<" Averaging PCR-L PCR-R"<< endl;
	wt_avg_roi_values(ROImap_u,ROImap_w,new_u,new_w,2,"PCR-L","PCR-R");
    //cout <<" we get " << avgInfo[0] << "; Nvox= " << avgInfo[1] << endl;
    //new_u=avgInfo[0]; new_w=avgInfo[1];
	ROImap_u.insert ( pair<string,float>("PCR",new_u) );
	ROImap_w.insert ( pair<string,float>("PCR",new_w) );
	
	cout <<" Averaging ACR SCR PCR R and L"<< endl;
	wt_avg_roi_values(ROImap_u,ROImap_w,new_u,new_w,6,"ACR-R","SCR-R","PCR-R","ACR-L","SCR-L","PCR-L");
		//cout <<" we get " << avgInfo[0] << "; Nvox= " << avgInfo[1] << endl;
		//new_u=avgInfo[0]; new_w=avgInfo[1];
	ROImap_u.insert ( pair<string,float>("CR",new_u) );
	ROImap_w.insert ( pair<string,float>("CR",new_w) );
	
		/////// CST
	cout <<" Averaging CST-L CST-R"<< endl;
	wt_avg_roi_values(ROImap_u,ROImap_w,new_u,new_w,2,"CST-L","CST-R");
		//cout <<" we get " << avgInfo[0] << "; Nvox= " << avgInfo[1] << endl;
		//new_u=avgInfo[0]; new_w=avgInfo[1]; 
	ROImap_u.insert ( pair<string,float>("CST",new_u) );
	ROImap_w.insert ( pair<string,float>("CST",new_w) );	
	
		/////// PTR
	cout <<" Averaging PTR-L PTR-R" << endl;
	wt_avg_roi_values(ROImap_u,ROImap_w,new_u,new_w,2,"PTR-L","PTR-R");
		//cout <<" we get " << avgInfo[0] << "; Nvox= " << avgInfo[1] << endl;
		//new_u=avgInfo[0]; new_w=avgInfo[1]; 
	ROImap_u.insert ( pair<string,float>("PTR",new_u) );
	ROImap_w.insert ( pair<string,float>("PTR",new_w) );	
	
		/////// SS
	cout <<" Averaging SS-L SS-R" << endl;
	wt_avg_roi_values(ROImap_u,ROImap_w,new_u,new_w,2,"SS-L","SS-R");
		//cout <<" we get " << avgInfo[0] << "; Nvox= " << avgInfo[1] << endl;
		//new_u=avgInfo[0]; new_w=avgInfo[1]; 
	ROImap_u.insert ( pair<string,float>("SS",new_u) );
	ROImap_w.insert ( pair<string,float>("SS",new_w) );	
	
		/////// EC
	cout <<" Averaging EC-L EC-R" << endl;
	wt_avg_roi_values(ROImap_u,ROImap_w,new_u,new_w,2,"EC-L","EC-R");
		//cout <<" we get " << avgInfo[0] << "; Nvox= " << avgInfo[1] << endl;
		//new_u=avgInfo[0]; new_w=avgInfo[1]; 
	ROImap_u.insert ( pair<string,float>("EC",new_u) );
	ROImap_w.insert ( pair<string,float>("EC",new_w) );
	
		/////// CGC
	cout <<" Averaging CGC-L CGC-R" << endl;
	wt_avg_roi_values(ROImap_u,ROImap_w,new_u,new_w,2,"CGC-L","CGC-R");
		//cout <<" we get " << avgInfo[0] << "; Nvox= " << avgInfo[1] << endl;
		//new_u=avgInfo[0]; new_w=avgInfo[1]; 
	ROImap_u.insert ( pair<string,float>("CGC",new_u) );
	ROImap_w.insert ( pair<string,float>("CGC",new_w) );
    
    	/////// CGH
	
    cout <<" Averaging CGH-L CGH-R" << endl;
	wt_avg_roi_values(ROImap_u,ROImap_w,new_u,new_w,2,"CGH-L","CGH-R");
    //cout <<" we get " << avgInfo[0] << "; Nvox= " << avgInfo[1] << endl;
    //new_u=avgInfo[0]; new_w=avgInfo[1];
	ROImap_u.insert ( pair<string,float>("CGH",new_u) );
	ROImap_w.insert ( pair<string,float>("CGH",new_w) );
    
		/////// SLF
	cout <<" Averaging SLF-L SLF-R" << endl;
	wt_avg_roi_values(ROImap_u,ROImap_w,new_u,new_w,2,"SLF-L","SLF-R");
		//cout <<" we get " << avgInfo[0] << "; Nvox= " << avgInfo[1] << endl;
		//new_u=avgInfo[0]; new_w=avgInfo[1]; 
	ROImap_u.insert ( pair<string,float>("SLF",new_u) );
	ROImap_w.insert ( pair<string,float>("SLF",new_w) );
	
		/////// SFO
	cout <<" Averaging SFO-L SFO-R" << endl;
	wt_avg_roi_values(ROImap_u,ROImap_w,new_u,new_w,2,"SFO-L","SFO-R");
		//cout <<" we get " << new_u << "; Nvox= " << new_w << endl;
		//new_u=avgInfo[0]; new_w=avgInfo[1]; 
	ROImap_u.insert ( pair<string,float>("SFO",new_u) );
	ROImap_w.insert ( pair<string,float>("SFO",new_w) );
	

		/////// IFO
	cout <<" Averaging IFO-L IFO-R" << endl;
	wt_avg_roi_values(ROImap_u,ROImap_w,new_u,new_w,2,"IFO-L","IFO-R");
		//info[0]=avgInfo[0];info[1]=avgInfo[1];
		//new_u=info[0]; new_w=info[1]; 
	ROImap_u.insert ( pair<string,float>("IFO",new_u) );
	ROImap_w.insert ( pair<string,float>("IFO",new_w) );
	
    /////// FX/ST
	cout <<" Averaging FX/ST-L FX/ST-R" << endl;
	wt_avg_roi_values(ROImap_u,ROImap_w,new_u,new_w,2,"FX/ST-L","FX/ST-R");
    //info[0]=avgInfo[0];info[1]=avgInfo[1];
    //new_u=info[0]; new_w=info[1];
	ROImap_u.insert ( pair<string,float>("FXST",new_u) );
	ROImap_w.insert ( pair<string,float>("FXST",new_w) );
	
	    ///////UNC
	cout <<" Averaging UNC-L UNC-R" << endl;
	wt_avg_roi_values(ROImap_u,ROImap_w,new_u,new_w,2,"UNC-L","UNC-R");
    //info[0]=avgInfo[0];info[1]=avgInfo[1];
    //new_u=info[0]; new_w=info[1];
	ROImap_u.insert ( pair<string,float>("UNC",new_u) );
	ROImap_w.insert ( pair<string,float>("UNC",new_w) );
	
	create_subj_table (argv[2], ROImap_u,ROImap_w );
	
	return 0;
}

