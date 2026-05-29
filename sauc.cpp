/* sauc.cpp  -- Search of Alternate Unit Cells
 (C) Copyright Keith McGill 2013
 
 rev 27 Mar 2013 -- HJB
 rev 26 May 2013 -- LCA
 rev 27 May 2013 -- HJB
 rev 24 Jun 2013 -- HJB
 rev  6 Jul 2013 -- HJB
 rev 22 Apr 2014 -- HJB
 rev 20 Jan 2015 -- HJB
 rev 12 Nov 2016 -- HJB
 rev 19 Sep 2022 -- HJB
 
 *******************************************************
 You may redistribute this program under the terms
 of the GPL.
 
 Alternatively you may redistribute this functions
 and subroutines of this program as an API under the
 terms of the LGPL
 *******************************************************
 *************************** GPL NOTICES ******************************
 *                                                                    *
 * This program is free software; you can redistribute it and/or      *
 * modify it under the terms of the GNU General Public License as     *
 * published by the Free Software Foundation; either version 2 of     *
 * (the License, or (at your option) any later version.               *
 *                                                                    *
 * This program is distributed in the hope that it will be useful,    *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of     *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the      *
 * GNU General Public License for more details.                       *
 *                                                                    *
 * You should have received a copy of the GNU General Public License  *
 * along with this program; if not, write to the Free Software        *
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA           *
 * 02111-1307  USA                                                    *
 *                                                                    *
 **********************************************************************
 
 ************************* LGPL NOTICES *******************************
 *                                                                    *
 * This library is free software; you can redistribute it and/or      *
 * modify it under the terms of the GNU Lesser General Public         *
 * License as published by the Free Software Foundation; either       *
 * version 2.1 of the License, or (at your option) any later version. *
 *                                                                    *
 * This library is distributed in the hope that it will be useful,    *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of     *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU  *
 * Lesser General Public License for more details.                    *
 *                                                                    *
 * You should have received a copy of the GNU Lesser General Public   *
 * License along with this library; if not, write to the Free         *
 * Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,    *
 * MA  02110-1301  USA                                                *
 *                                                                    *
 **********************************************************************
 
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <ctype.h>
#include <iostream>
#include <cstring>
#include "TNear.h"
#include "triple.h"
#include "rhrand.h"
#include <fstream>
#include <string>
#include <sstream>
#include "G6.h"
#include "V7.h"
#include "Cell.h"
#include "Reducer.h"
#include "Mat66.h"
#include "pststrmgr.h"
#include <vector>

#include "sauc_psm_files_create.h"
#include "S6M_SellingReduce.h"

char *fgetln(FILE *stream, size_t *len);

/* Template for an array class so we can have 6D vectors
 of arrays */

template< typename T > class sixarray {
private:
    size_t length;
    T      xxarray[6];
public:
    // constructors
    sixarray ( void) {
        xxarray[0] = xxarray[1] = xxarray[2]
        = xxarray[3] = xxarray[4] = xxarray[5] = 0;
    }
    
    sixarray ( T a, T b, T c, T d, T e, T f) {
        xxarray[0] = a;
        xxarray[1] = b;
        xxarray[2] = c;
        xxarray[3] = d;
        xxarray[4] = e;
        xxarray[5] = f;
    }
    
    T operator[] ( const size_t index ) {return xxarray[index];}
};


/* Forward Declaration */
void SphereResults( std::ostream& out,
                   const std::vector<unitcell>&  myvector,
                   const std::vector<size_t>&    myindices,
                   const std::vector<double>&    mydistances,
                   const unitcell&               unknownCell,
                   const int&			 dont_sort_by_family);

using namespace std;

/* Escape database-sourced free text for safe HTML embedding.
   Order matters: & must be replaced first to avoid double-escaping. */
static std::string html_escape(const std::string &s) {
    std::string result;
    result.reserve(s.size());
    for (char c : s) {
        switch (c) {
            case '&':  result += "&amp;";  break;
            case '<':  result += "&lt;";   break;
            case '>':  result += "&gt;";   break;
            case '"':  result += "&quot;"; break;
            default:   result += c;        break;
        }
    }
    return result;
}

CNearTree <unitcell> * cellTree[6] = {NULL,NULL,NULL,NULL,NULL,NULL};
CNearTree <unitcell>::iterator cellTree_itend[6];
string sauc_NT_ckp_names[7] =
{
    "sauc_NT_L1_ckp.dmp",
    "sauc_NT_L2_ckp.dmp",
    "sauc_NT_NCDist_ckp.dmp",
    "sauc_NT_V7_ckp.dmp",
    "sauc_NT_D7_ckp.dmp",
    "sauc_NT_S6_ckp.dmp",
    "sauc_NT_DC7unsrt_ckp.dmp"
};

PSM_localpsm_handle PDBentries=NULL;
PSM_localpsm_handle PDBcells=NULL;
PSM_localpsm_handle CSDcells=NULL;
PSM_localpsm_handle CODentries=NULL;


const int PDB_DBTYPE = '\1';
const int CSD_DBTYPE = '\2';
const int COD_DBTYPE = '\3';


string PDB_loaded;
string CSD_loaded;
string COD_loaded;
vector <char> cellDB;
vector <sixarray<double> > cellDArray;
vector <std::string> spaceArray;
vector <int> zArray;
vector <std::string> idArray;
double probeArray[7];
G6 primredprobe;
string probelattice;
double avglen=1.;
double crootvol;
G6 primcell;
G6 redprimcell;
int nearest = 0, numRow = 0;
int quitAlgorithm = 0, quitSimilar = 0, quitContinue = 0, endProgram = 0, choiceAlgorithm = 0, choiceSimilar = 0, choiceContinue = 0, goBack = 0,
priorAlgorithm = -1;
double result0 = 0, result1 = 0, result2 = 0, result3 = 0, result4 = 0, result5 = 0, resultSumTemp = 0, resultSum = 0, numRange = 0;
double numRangeA, numRangeB, numRangeC, numRangeAlpha, numRangeBeta, numRangeGamma, sphereRange;
string valueDump;
int sauc_batch_mode = 0;
int sauc_javascript = 0;


//*****************************************************************************/
double convertToDouble(string value)
{
    double number;
    istringstream issvalue(value);
    if (!(issvalue >> number)) number = 0;
    return number;
}

//*****************************************************************************/
int convertToInt(string zvalue)
{
    int number = 0;
    istringstream issvalue(zvalue);
    if (!(issvalue >> number)) number = 0;
    return number;
}

inline long hashvalue(string str) {
    unsigned int ii;
    unsigned long hash;
    const char * strasstr = str.c_str();
    hash = 0;
    for(ii=0;ii < PSM_HASHTABLE_MAXSTR && ii<str.length();ii++) {
        hash = ((hash << 4)| (hash>>28))^((unsigned long)(tolower(strasstr[ii]))-32);
    }
    return (long) (hash&0x1FFFF);
}

vector<string> split(const string& str, char delim) {
    vector<string> values;
    const char * strasstr = str.c_str();
    unsigned int istart,iend;
    istart=0;
    while (istart < str.length()){
        for (iend=istart;iend<str.length();iend++){
            if (strasstr[iend] == delim) {
                values.push_back(str.substr(istart,iend-istart));
                istart = iend+1;
                break;
            }
        }
        if (iend < str.length()) continue;
        values.push_back(str.substr(istart,iend-istart));
        istart = iend+1;
        break;
    }
    return values;
}

inline std::string PSM_getstringfield(PSM_localpsm_handle handle, PSM_string field) {
    if (field.offset < 0 || field.offset >= handle->chars_len
        || field.offset+field.length-1 < 0 || field.offset+field.length-1 >= handle->chars_len)
        return std::string("__UNK__");
    return std::string(handle->chars+field.offset,field.length);
}


/* Make an mmap file from a PDB, COD or CSD text file,
 or pick up one that is already there*/

//*****************************************************************************
PSM_localpsm_handle make_psmfile(const char * mmapfilename, const char* textfilename,
                                 size_t numhash, size_t * fieldnumbers,
                                 size_t skip_lines, char sepchar, int CSDflag)
{
    FILE * infile;
    char * nextline;
    size_t linelen;
    char buffer [1040];
    PSM_localpsm_handle localpsm_handle;
    size_t ii;
    
    if ((localpsm_handle = PSM_mmap_file(mmapfilename))) {
        
        return localpsm_handle;
        
    }
    
    
    localpsm_handle = PSM_localpsm_handle_create_wh(numhash,
                                                    fieldnumbers,
                                                    sepchar );
    infile = fopen(textfilename,"rb");
    
    if (!infile) return 0;
    
    for (ii = 0; ii < skip_lines; ii++) {
        nextline = fgetln(infile,&linelen);
    }
    while ((nextline = fgetln(infile,&linelen))) {
        if (!CSDflag) {
            PSM_addstrn(localpsm_handle,nextline,linelen);
        } else {
            /* for CSD add an extra copy of the first 6 characters
             of the refcode as a family ID */
            size_t llen;
            llen = linelen;
            if (linelen > 1023) llen = 1023;
            strncpy(buffer,nextline,llen);
            buffer[llen++] = sepchar;
            for (ii = 0; ii < 6 && buffer[ii]!=sepchar && buffer[ii]!=0; ii++) {
                buffer[llen++] = buffer[ii];
            }
            buffer[llen] = '\0';
            PSM_addstrn(localpsm_handle,buffer,llen);
        }
    }
    
    fclose(infile);
    
    if (PSM_write_file(localpsm_handle,mmapfilename)) {
        
        return 0;
        
    }
    
    return localpsm_handle;
    
}

int load_cellDSGZArrays(void) {
    
    PSM_string* PDBcells_fields;
    PSM_string* CODentries_fields;
    PSM_string* CSDcells_fields;
    string pdbid;
    string codid;
    string csdref;
    string SG;
    double a=1.;
    double b=1.;
    double c=1.;
    double alpha=90.;
    double beta=90.;
    double gamma=90.;
    int Z;
    size_t ii;
    size_t rejected=0;
    
    PDBcells_fields   = (PSM_string*)malloc(PDBcells_numfields*sizeof(PSM_string));
    CODentries_fields = (PSM_string*)malloc(CODentries_numfields*sizeof(PSM_string));
    CSDcells_fields   = (PSM_string*)malloc(CSDcells_numfields*sizeof(PSM_string));
    
    if (PDBcells) {
        
        std::cout << "Processing " << PDBcells->str_index_len << " PDB cells " << std::endl;
	rejected = 0;
        
        for (ii = 0; ii < PDBcells->str_index_len; ii++){
            if (ii%20000 == 0) std::cout<< "." ; /*processing PDB" << ii << std::endl; */
            if (PSM_split_psm_string(PDBcells,PDBcells_fields,PDBcells_numfields,
                                     PDBcells->str_index[ii],PDBcells_sep_char)) {
                pdbid = std::string(PDBcells->chars+PDBcells_fields[PDBcells_id].offset,
                                    PDBcells_fields[PDBcells_id].length);
                SG = std::string(PDBcells->chars+PDBcells_fields[PDBcells_space_group].offset,
                                 PDBcells_fields[PDBcells_space_group].length);
                a=atof(std::string(PDBcells->chars+PDBcells_fields[PDBcells_cell_length_a].offset,
                                   PDBcells_fields[PDBcells_cell_length_a].length).c_str());
                b=atof(std::string(PDBcells->chars+PDBcells_fields[PDBcells_cell_length_b].offset,
                                   PDBcells_fields[PDBcells_cell_length_b].length).c_str());
                c=atof(std::string(PDBcells->chars+PDBcells_fields[PDBcells_cell_length_c].offset,
                                   PDBcells_fields[PDBcells_cell_length_c].length).c_str());
                alpha=atof(std::string(PDBcells->chars+PDBcells_fields[PDBcells_cell_angle_alpha].offset,
                                       PDBcells_fields[PDBcells_cell_angle_alpha].length).c_str());
                beta=atof(std::string(PDBcells->chars+PDBcells_fields[PDBcells_cell_angle_beta].offset,
                                      PDBcells_fields[PDBcells_cell_angle_beta].length).c_str());
                gamma=atof(std::string(PDBcells->chars+PDBcells_fields[PDBcells_cell_angle_gamma].offset,
                                       PDBcells_fields[PDBcells_cell_angle_gamma].length).c_str());
                Z = atoi(std::string(PDBcells->chars+PDBcells_fields[PDBcells_Z].offset,
                                     PDBcells_fields[PDBcells_Z].length).c_str());
                /* if (ii < 25) {
                  std::cout << " ii: " << ii << " pdbid: " << pdbid
                            << " SG: " << SG 
                            << " a, b, c, alpha, beta, gamma: " 
                            << a << ", " << b << ", " << c << ", " << alpha << ", " << beta << ", " << gamma 
                            << " Z: " << Z  << std::endl; 
                } */
                if (pdbid.length() >= 4 && 
                    pdbid.length() < 25 && 
                    a > 1. && b > 1. && c > 1.
                    && a==a && b==b && c==c && a <  1.e6 && b < 1.e6 && c < 1.e6
                    && alpha >= 5. && alpha <= 175.
                    && beta  >= 5. && beta  <= 175.
                    && gamma >= 5. && gamma <= 175.
                    && (SG.substr(0,1) == "P" || SG.substr(0,1) == "p"
                        || SG.substr(0,1) == "V" || SG.substr(0,1) == "v"
                        || SG.substr(0,1) == "U" || SG.substr(0,1) == "u"
                        || SG.substr(0,1) == "I" || SG.substr(0,1) == "i"
                        || SG.substr(0,1) == "F" || SG.substr(0,1) == "f"
                        || SG.substr(0,1) == "A" || SG.substr(0,1) == "a"
                        || SG.substr(0,1) == "B" || SG.substr(0,1) == "b"
                        || SG.substr(0,1) == "C" || SG.substr(0,1) == "c"
                        || SG.substr(0,1) == "R" || SG.substr(0,1) == "r"
                        || SG.substr(0,1) == "H" || SG.substr(0,1) == "h")) {
                        cellDArray.push_back(sixarray<double>(a,b,c,alpha,beta,gamma));
                        zArray.push_back(Z);
                        idArray.push_back(pdbid);
                        spaceArray.push_back(SG);
                        cellDB.push_back(PDB_DBTYPE);
                    } else {
			rejected++;
	                /*std::cout << "Rejected "<< pdbid <<" ["<<a<<","<<b<<","<<c<<","<<alpha<<","<<beta<<","<<gamma<<"] "<<SG <<std::endl;*/
                    }
            }
        }
	if (rejected > 0) {
		std::cout << "Rejected "<<rejected<< " PDB cells" << std::endl;
	}
    }
    
    if (CODentries) {
        
        std::cout << "Processing " << CODentries->str_index_len << " COD cells " << std::endl;

	rejected = 0;
        
        for (ii = 0; ii < CODentries->str_index_len; ii++) {
            if (ii%20000 == 0) std::cout<< "."; 
            if (PSM_split_psm_string(CODentries,CODentries_fields,CODentries_numfields,
                                     CODentries->str_index[ii],CODentries_sep_char)) {
                codid = std::string(CODentries->chars+CODentries_fields[CODentries_id].offset,
                                    CODentries_fields[CODentries_id].length);
                SG = std::string(CODentries->chars+CODentries_fields[CODentries_symmetry_space_group_name_H_M].offset,
                                 CODentries_fields[CODentries_symmetry_space_group_name_H_M].length);
                a=atof(std::string(CODentries->chars+CODentries_fields[CODentries_cell_length_a].offset,
                                   CODentries_fields[CODentries_cell_length_a].length).c_str());
                b=atof(std::string(CODentries->chars+CODentries_fields[CODentries_cell_length_b].offset,
                                   CODentries_fields[CODentries_cell_length_b].length).c_str());
                c=atof(std::string(CODentries->chars+CODentries_fields[CODentries_cell_length_c].offset,
                                   CODentries_fields[CODentries_cell_length_c].length).c_str());
                alpha=atof(std::string(CODentries->chars+CODentries_fields[CODentries_cell_angle_alpha].offset,
                                       CODentries_fields[CODentries_cell_angle_alpha].length).c_str());
                beta=atof(std::string(CODentries->chars+CODentries_fields[CODentries_cell_angle_beta].offset,
                                      CODentries_fields[CODentries_cell_angle_beta].length).c_str());
                gamma=atof(std::string(CODentries->chars+CODentries_fields[CODentries_cell_angle_gamma].offset,
                                       CODentries_fields[CODentries_cell_angle_gamma].length).c_str());
                Z = atoi(std::string(CODentries->chars+CODentries_fields[CODentries_cell_formula_units_Z].offset,
                                     CODentries_fields[CODentries_cell_formula_units_Z].length).c_str());                
            } 
                
            if (codid.length() >= 6 && a > 1. && b > 1. && c > 1.
                    && a==a && b==b && c==c && a <  1.e6 && b < 1.e6 && c < 1.e6
                    && alpha >= 5. && alpha <= 175.
                    && beta  >= 5. && beta  <= 175.
                    && gamma >= 5. && gamma <= 175.
                    && (SG.substr(0,1) == "P" || SG.substr(0,1) == "p"
                        || SG.substr(0,1) == "V" || SG.substr(0,1) == "v"
                        || SG.substr(0,1) == "U" || SG.substr(0,1) == "u"
                        || SG.substr(0,1) == "I" || SG.substr(0,1) == "i"
                        || SG.substr(0,1) == "F" || SG.substr(0,1) == "f"
                        || SG.substr(0,1) == "A" || SG.substr(0,1) == "a"
                        || SG.substr(0,1) == "B" || SG.substr(0,1) == "b"
                        || SG.substr(0,1) == "C" || SG.substr(0,1) == "c"
                        || SG.substr(0,1) == "R" || SG.substr(0,1) == "r"
                        || SG.substr(0,1) == "H" || SG.substr(0,1) == "h")) {
                        cellDArray.push_back(sixarray<double>(a,b,c,alpha,beta,gamma));
                        zArray.push_back(Z);
                        idArray.push_back(codid);
                        spaceArray.push_back(SG);
                        cellDB.push_back(COD_DBTYPE);
            } else {
	      rejected++;
            }
        }
	if (rejected > 0) {
		std::cout << "Rejected "<<rejected<< " COD cells" << std::endl;
	}
    }
    
    if (CSDcells) {
        
        std::cout << "Processing " << CSDcells->str_index_len << " CSD cells " << std::endl;
        
        for (ii = 0; ii < CSDcells->str_index_len; ii++){
            if (ii%20000 == 0) std::cout<< ".";  /* "processing CSD" << ii << std::endl; */
            if (PSM_split_psm_string(CSDcells,CSDcells_fields,CSDcells_numfields,
                                     CSDcells->str_index[ii],CSDcells_sep_char)) {
                csdref = std::string(CSDcells->chars+CSDcells_fields[CSDcells_refcode].offset,
                                     CSDcells_fields[CSDcells_refcode].length);
                SG = std::string(CSDcells->chars+CSDcells_fields[CSDcells_space_group].offset,
                                 CSDcells_fields[CSDcells_space_group].length);
                a=atof(std::string(CSDcells->chars+CSDcells_fields[CSDcells_cell_length_a].offset,
                                   CSDcells_fields[CSDcells_cell_length_a].length).c_str());
                b=atof(std::string(CSDcells->chars+CSDcells_fields[CSDcells_cell_length_b].offset,
                                   CSDcells_fields[CSDcells_cell_length_b].length).c_str());
                c=atof(std::string(CSDcells->chars+CSDcells_fields[CSDcells_cell_length_c].offset,
                                   CSDcells_fields[CSDcells_cell_length_c].length).c_str());
                alpha=atof(std::string(CSDcells->chars+CSDcells_fields[CSDcells_cell_angle_alpha].offset,
                                       CSDcells_fields[CSDcells_cell_angle_alpha].length).c_str());
                beta=atof(std::string(CSDcells->chars+CSDcells_fields[CSDcells_cell_angle_beta].offset,
                                      CSDcells_fields[CSDcells_cell_angle_beta].length).c_str());
                gamma=atof(std::string(CSDcells->chars+CSDcells_fields[CSDcells_cell_angle_gamma].offset,
                                       CSDcells_fields[CSDcells_cell_angle_gamma].length).c_str());
                Z = 0;
                
                if (csdref.length() >= 6 && a > 1. && b > 1. && c > 1.
                    && a==a && b==b && c==c && a <  1.e6 && b < 1.e6 && c < 1.e6
                    && alpha >= 5. && alpha <= 175.
                    && beta  >= 5. && beta  <= 175.
                    && gamma >= 5. && gamma <= 175.
                    && (SG.substr(0,1) == "P" || SG.substr(0,1) == "p"
                        || SG.substr(0,1) == "V" || SG.substr(0,1) == "v"
                        || SG.substr(0,1) == "U" || SG.substr(0,1) == "u"
                        || SG.substr(0,1) == "I" || SG.substr(0,1) == "i"
                        || SG.substr(0,1) == "F" || SG.substr(0,1) == "f"
                        || SG.substr(0,1) == "A" || SG.substr(0,1) == "a"
                        || SG.substr(0,1) == "B" || SG.substr(0,1) == "b"
                        || SG.substr(0,1) == "C" || SG.substr(0,1) == "c"
                        || SG.substr(0,1) == "R" || SG.substr(0,1) == "r"
                        || SG.substr(0,1) == "H" || SG.substr(0,1) == "h")) {
                        
                        cellDArray.push_back(sixarray<double>(a,b,c,alpha,beta,gamma));
                        zArray.push_back(Z);
                        idArray.push_back(csdref);
                        spaceArray.push_back(SG);
                        cellDB.push_back(CSD_DBTYPE);
                } else {
			rejected++;
		}
            }
        }
	if (rejected > 0) {
		std::cout << "Rejected "<<rejected<< " CSD cells" << std::endl;
	}
    }
    
    std::cout << "Processed " << cellDB.size() << " database entries " << std::endl;
    return 0;
    
}

int make_mmapfiles(void) {
    
    size_t PDBcfieldnumbers[1] = {PDBcells_id+1};
    size_t PDBefieldnumbers[2] = {PDBentries_id+1,PDBentries_head+1};
    size_t CSDfieldnumbers[2] = {CSDcells_refcode+1,
        CSDcells_refcode_family+1};
    size_t CODfieldnumbers[2] = {CODentries_id+1,CODentries_chemical_formula_sum+1};
    size_t numdb=0;
    
    PDBentries = make_psmfile(PDBentries_mmap_file, PDBentries_raw_file,
                              2, PDBefieldnumbers,
                              PDBentries_skip_lines, PDBentries_sep_char, 0);
    PDBcells = make_psmfile(PDBcells_mmap_file, PDBcells_raw_file,
                            1, PDBcfieldnumbers,
                            PDBcells_skip_lines, PDBcells_sep_char, 0);
    CSDcells = make_psmfile(CSDcells_mmap_file, CSDcells_raw_file,
                            2, CSDfieldnumbers,
                            CSDcells_skip_lines, CSDcells_sep_char, 1);
    CODentries = make_psmfile(CODentries_mmap_file, CODentries_raw_file,
                              2, CODfieldnumbers,
                              CODentries_skip_lines, CODentries_sep_char, 0);
    if (PDBcells != 0 && PDBcells->str_index_len > 0) {
        numdb++;
        std::cout << "PDB cells available: " << PDBcells->str_index_len <<std::endl;
    }
    if (PDBentries != 0 && PDBentries->str_index_len > 0 ) {
        numdb++;
        std::cout << "PDB entries available: " << PDBentries->str_index_len <<std::endl;
    }
    if (CSDcells != 0  && CSDcells->str_index_len > 0 ) {
        numdb++;
        std::cout << "CSD data available: " << CSDcells->str_index_len << std::endl;
    }
    if (CODentries != 0  && CODentries->str_index_len > 0 ) {
        numdb++;
        std::cout << "COD data available: " << CODentries->str_index_len << std::endl;
    }
    
    return (numdb > 0)?0:1;
    
}




/*****************************************************************************/
bool makeprimredprobe( void )
{
    int iii;
    string latsym;
    char clatsym;
    G6 v6cell;
    double d7cell[7];
    double d7red[7];
    double s6cell[6];
    double s6red[6];
    G6 g6cent;
    int reduced;
    double dmc[36];
    Mat66 mc;
    Mat66 m;
    G6 xrawcell;
    G6 recipcell;
    G6 reducedBase;
    G6 xreducedBase;
    int xreduced;
    Cell rawcell(probeArray[0],probeArray[1],probeArray[2],
                 probeArray[3],probeArray[4],probeArray[5]);
    int ii;
    bool ret;
    if (probelattice.size()< 1) {
        latsym = "P";
    } else {
        latsym = probelattice.substr(0,1);
    }
    clatsym = latsym[0];
    xrawcell = rawcell.Cell2V6();
    std::cout << "xrawcell: " << xrawcell[0] << ", " << xrawcell[1] << ", "  << xrawcell[2] << ", "
    << xrawcell[3] << ", "  << xrawcell[4] << ", "  << xrawcell[5] <<  std::endl;
    switch (clatsym) {
        case 'P':
        case 'p':
        case 'A':
        case 'a':
        case 'B':
        case 'b':
        case 'C':
        case 'c':
        case 'I':
        case 'i':
        case 'F':
        case 'f':
        case 'R':
        case 'r':
        case 'H':
        case 'h':
            g6cent=rawcell.Cell2V6();
            CS6M_LatSymMat66(g6cent,clatsym,dmc,primcell);
            /*mc = rawcell.LatSymMat66(latsym);
            primcell = mc*(rawcell.Cell2V6());*/
            break;
        case 'V':
        case 'v':
            for (ii=0; ii< 6; ii++) {
                primcell[ii] = probeArray[ii];
            }
            break;
        default:
            cerr << "Unrecognized lattice symbol "<< probelattice<<" treated as P" << std::endl;
            latsym = "P";
            /* mc = rawcell.LatSymMat66(latsym);*/
            primcell = rawcell.Cell2V6();
            break;
    }
    switch (choiceAlgorithm) {
        case 1:  /* L1 */
        case 2:  /* L2 */
        case 3:  /* NCDist */
        case 4:  /* V7 */
        case 7:  /* DC7unsrt */
             CS6M_G6Reduce(primcell,redprimcell,reduced);
             break;
        case 5:  /* D7Dist */
             CS6M_G6toD7(primcell,d7cell);
             CS6M_D7Reduce(d7cell,d7red,reduced);
             CS6M_D7toG6(d7red,redprimcell);
             break;
        case 6:  /* S6Dist */
        default: /* and anything else */
             CS6M_G6toS6(primcell,s6cell);
             std::cout<<"G6toS6: " << "[" <<primcell[0]<<","  <<primcell[1]<<"," <<primcell[2]<<"," <<primcell[3]<<"," <<primcell[4]<<"," <<primcell[5]<<"],"
             << "[" <<s6cell[0]<<","  <<s6cell[1]<<"," <<s6cell[2]<<"," <<s6cell[3]<<"," <<s6cell[4]<<"," <<s6cell[5]<<"]"<<std::endl;
             CS6M_S6Reduce(s6cell,s6red,reduced);
             std::cout<<"S6Reduce: " << "[" <<s6red[0]<<","  <<s6red[1]<<"," <<s6red[2]<<"," <<s6red[3]<<"," <<s6red[4]<<"," <<s6red[5]<<"]"<<std::endl;
             CS6M_S6toG6(s6red,redprimcell);
             break;
    }  
    primredprobe = Cell(redprimcell).CellWithDegrees();
    std::cout << "Primitive Reduced Probe Cell: " <<
    primredprobe[0]<<" "<<
    primredprobe[1]<<" "<<
    primredprobe[2]<<" "<<
    primredprobe[3]<<" "<<
    primredprobe[4]<<" "<<
    primredprobe[5] << std::endl;
    std::cout << "Volume :" << Cell(redprimcell).Volume() << std::endl;
    crootvol = pow(Cell(redprimcell).Volume(),1./3.);
    Reducer::Reduce((Cell(redprimcell).Inverse()).Cell2V6(),m,xreducedBase,0.0);
    CS6M_G6Reduce((Cell(redprimcell).Inverse()).Cell2V6(),reducedBase,xreduced);
    for (iii=0; iii<6; iii++ ) {
      if (xreducedBase[iii] != reducedBase[iii] ) {
          std::cout<< "redprimcell : " << redprimcell[0]<<", " 
                                       << redprimcell[1]<<", "
                                       << redprimcell[2]<<", "
                                       << redprimcell[3]<<", "
                                       << redprimcell[4]<<", "
                                       << redprimcell[5]<<std::endl;
          std::cout<< "Inverse : "     << (Cell(redprimcell).Inverse())[0]<<", " 
                                       << (Cell(redprimcell).Inverse())[1]<<", "
                                       << (Cell(redprimcell).Inverse())[2]<<", "
                                       << (Cell(redprimcell).Inverse())[3]<<", "
                                       << (Cell(redprimcell).Inverse())[4]<<", "
                                       << (Cell(redprimcell).Inverse())[5]<<std::endl;
          std::cout<< "xreducedBase : "<< xreducedBase[0]<<", " 
                                       << xreducedBase[1]<<", "
                                       << xreducedBase[2]<<", "
                                       << xreducedBase[3]<<", "
                                       << xreducedBase[4]<<", "
                                       << xreducedBase[5]<<std::endl;
          std::cout<< "reducedBase : "<< reducedBase[0]<<", " 
                                       << reducedBase[1]<<", "
                                       << reducedBase[2]<<", "
                                       << reducedBase[3]<<", "
                                       << reducedBase[4]<<", "
                                       << reducedBase[5]<<std::endl;

        break;
      }
    }
    
    recipcell = (Cell(redprimcell).Inverse()).CellWithDegrees();
    
    std::cout << "Reciprocal of Primitive Probe Cell: " <<
    recipcell[0]<<" "<<
    recipcell[1]<<" "<<
    recipcell[2]<<" "<<
    recipcell[3]<<" "<<
    recipcell[4]<<" "<<
    recipcell[5]<< std::endl;
    std::cout << "Volume of Reciprocal Cell: " <<
    (Cell(redprimcell).Inverse()).Volume() << std::endl;
    std::cout << "V7 linearized and scaled: "
    << primredprobe[0]*std::sqrt(6./7.)<<" "
    << primredprobe[1]*std::sqrt(6./7.)<<" "
    << primredprobe[2]*std::sqrt(6./7.)<<" "
    << std::sqrt(1./reducedBase[0])*std::sqrt(6./7.)<<" "
    << std::sqrt(1./reducedBase[1])*std::sqrt(6./7.)<<" "
    << std::sqrt(1./reducedBase[2])*std::sqrt(6./7.)<<" "
    << " "<<
    crootvol*std::sqrt(6./7.)<<std::endl;
    if (latsym[0] == 'V' || latsym[0] == 'v') {
        std::cout << "raw G6 vector: "
        << primcell[0]<<" "
        << primcell[1]<<" "
        << primcell[2]<<" "
        << primcell[3]<<" "
        << primcell[4]<<" "
        << primcell[5]<<std::endl;
    } else {
        std::cout << "raw G6 vector: "
        << primredprobe[0]*primredprobe[0]<<" "
        << primredprobe[1]*primredprobe[1]<<" "
        << primredprobe[2]*primredprobe[2]<<" "
        << 2.*primredprobe[1]*primredprobe[2]*cos(primredprobe[3]*std::atan(1.0)/45.)<<" "
        << 2.*primredprobe[0]*primredprobe[2]*cos(primredprobe[4]*std::atan(1.0)/45.)<<" "
        << 2.*primredprobe[0]*primredprobe[1]*cos(primredprobe[5]*std::atan(1.0)/45.)<<std::endl;
    }
    std::cout << std::endl;
    return ret;
}

#define putoutst(serialout,value) \
if (value != ULONG_MAX) { \
(serialout) << (value); \
} else { \
(serialout) << "-1"; \
} \


//*****************************************************************************/
void buildNearTree( void )
{
    Mat66 m;
    Mat66 mc;
    G6 searchcell;
    double dsearchcell[6];
    std::ofstream serialout;
    std::ifstream serialin;
    size_t si;
    size_t sv;
    double cell[6];
    double row;
    char dbid;
    unitcell ucell;
    std::vector<long> * DelayedIndices; // objects queued for insertion, possibly in random order
    std::vector<unitcell>  * ObjectStore;    // all inserted objects go here
    std::vector<size_t>    * ObjectCollide;  // overflow chain of colliding objects
    size_t            DeepestDepth;     // maximum depth of the tree
    std::vector< CNearTree<unitcell>::NearTreeNode<unitcell> * >
    * NearTreeNodes;  // vector of pointers to nodes to build the tree
    CNearTree<unitcell>::NearTreeNode<unitcell>
    * BaseNode;       // the tree's data is stored down
    // this node in m_NearTreeNodes
    long              Flags;            // flags for operational control (mainly for testing)
    double            DiamEstimate;     // estimated diameter
    double            SumSpacings;      // sum of spacings at time of insertion
    double            SumSpacingsSq;    // sum of squares of spacings at time of insertion
    double            DimEstimate;      // estimated dimension
    double            DimEstimateEsd;   // estimated dimension estimated standard deviation
#ifdef CNEARTREE_INSTRUMENTED
    size_t            NodeVisits;       // number of node visits
#endif
    CNearTree<unitcell>::NearTreeNode<unitcell> * curntn;
    int objno = 0;
    bool gotckp = false;
    
    serialin.open(sauc_NT_ckp_names[choiceAlgorithm-1].c_str(),std::ios::in);
    objno = 0;
    while (true) {
        string token;
        int Algorithm = 0;
        if (serialin.is_open()) {
            std::cout << "Reading database " << sauc_NT_ckp_names[choiceAlgorithm-1] << std::endl;
            serialin >> token; if (!serialin.good() ||
                                   (token != string("Algorithm:")
                                    && token != string("Metric:"))) {
                                       std::cout << sauc_NT_ckp_names[choiceAlgorithm-1] << " badly formatted, no 'Metric' or 'Algorithm:' token" << std::endl;
                                       break;
                                   }
            serialin >> Algorithm;
            if (!serialin.good() || unitcell::algorithm != choiceAlgorithm) {
                std::cout << sauc_NT_ckp_names[choiceAlgorithm-1] << " badly formatted, algorithm value wrong" << std::endl;
                break;
            }
            
            serialin >> token; if (!serialin.good() || token != string("DelayedIndices:")) {
                std::cout << sauc_NT_ckp_names[choiceAlgorithm-1] << " badly formatted, no 'DelayedIndices:' token" << std::endl;
                break;
            }
            serialin >> token; if (!serialin.good() || token != string("{")) {
                std::cout << sauc_NT_ckp_names[choiceAlgorithm-1] << " badly formatted, no DelayedIndices '{' token" << std::endl;
                break;
            }
            DelayedIndices = new std::vector<long>();
            while (serialin.good()) {
                serialin >> token;
                if (!serialin.good()) break;
                if (token == string("}")) break;
                DelayedIndices->push_back(atol(token.c_str()));
            }
            if (!serialin.good()) {
                std::cout << sauc_NT_ckp_names[choiceAlgorithm-1] << " badly formatted DelayedIndices" << std::endl;
                break;
            }
            
            serialin >> token; if (!serialin.good() || token != string("ObjectStore:")) {
                std::cout << sauc_NT_ckp_names[choiceAlgorithm-1] << " badly formatted, no 'ObjectStore:' token" << std::endl;
                break;
            }
            serialin >> token; if (!serialin.good() || token != string("{")) {
                std::cout << sauc_NT_ckp_names[choiceAlgorithm-1] << " badly formatted, no ObjectStore '{' token" << std::endl;
                break;
            }
            ObjectStore = new std::vector<unitcell>();
            while (serialin.good()) {
                double cell[6];
                double row;
                char dbid;
                serialin >> token;
                if (token == string("}")) break;
                if (!serialin.good() || token != string("{")) {
                    std::cout << sauc_NT_ckp_names[choiceAlgorithm-1] << " badly formatted, missing ObjectStore '{' token" << std::endl;
                    break;
                }
                serialin >> cell[0] >> cell[1] >> cell[2] >> cell[3] >> cell[4] >> cell[5] >> row >> dbid;
                objno++;
                if (!serialin.good()) {
                    std::cout << sauc_NT_ckp_names[choiceAlgorithm-1] << " badly formatted, bad object: " << objno  << std::endl;
                    break;
                }
                ObjectStore->push_back(unitcell(cell,row,dbid));
                serialin >> token; if (!serialin.good() || token != string("}")) {
                    std::cout << sauc_NT_ckp_names[choiceAlgorithm-1] << " badly formatted, missing ObjectStore '}' token" << std::endl;
                    break;
                }
                
            }
            if (!serialin.good()) {
                std::cout << sauc_NT_ckp_names[choiceAlgorithm-1] << " badly formatted ObjectStore" << std::endl;
                break;
            }
            
            serialin >> token; if (!serialin.good() || token != string("ObjectCollide:")) {
                std::cout << sauc_NT_ckp_names[choiceAlgorithm-1] << " badly formatted, no 'ObjectCollide:' token" << std::endl;
                break;
            }
            serialin >> token; if (!serialin.good() || token != string("{")) {
                std::cout << sauc_NT_ckp_names[choiceAlgorithm-1] << " badly formatted, no ObjectCollide '{' token" << std::endl;
                break;
            }
            ObjectCollide = new std::vector<size_t>();
            while (serialin.good()) {
                long longval;
                serialin >> token;
                if (!serialin.good()) break;
                if (token == string("}")) break;
                longval = atol(token.c_str());
                if (longval != -1) {
                    ObjectCollide->push_back(size_t(longval));
                } else {
                    ObjectCollide->push_back(ULONG_MAX);
                }
            }
            if (!serialin.good()) {
                std::cout << sauc_NT_ckp_names[choiceAlgorithm-1] << " badly formatted ObjectCollide" << std::endl;
                break;
            }
            
            serialin >> token; if (!serialin.good() || token != string("DeepestDepth:")) {
                std::cout << sauc_NT_ckp_names[choiceAlgorithm-1] << " badly formatted, no 'DeepestDepth:' token" << std::endl;
                break;
            }
            
            serialin >> DeepestDepth; if (!serialin.good()) {
                std::cout << sauc_NT_ckp_names[choiceAlgorithm-1] << " badly formatted DeepestDepth" << std::endl;
                break;
            }
            
            serialin >> token; if (!serialin.good() || token != string("NearTreeNodes:")) {
                std::cout << sauc_NT_ckp_names[choiceAlgorithm-1] << " badly formatted, no 'NearTreeNodes:' token" << std::endl;
                break;
            }
            serialin >> token; if (!serialin.good() || token != string("{")) {
                std::cout << sauc_NT_ckp_names[choiceAlgorithm-1] << " badly formatted, no NearTreeNodes '{' token" << std::endl;
                break;
            }
            NearTreeNodes = new std::vector< CNearTree<unitcell>::NearTreeNode<unitcell> * >;
            BaseNode = NULL;
            while (serialin.good()) {
                long longval;
                size_t            ptLeft;            // index of left m_Object (of type TNode) stored in this node
                size_t            ptRight;           // index of right m_Object (of type TNode) stored in this node
                double            dMaxLeft;          // longest distance from the left m_Object to
                // anything below it in the tree
                double            dMaxRight;         // longest distance from the right m_Object to
                // anything below it in the tree
                size_t            pLeftBranch;       // tree descending from the left m_Object
                size_t            pRightBranch;      // tree descending from the right m_Object
                size_t            iTreeSize;         // size of this node tree
#ifdef CNEARTREE_INSTRUMENTED
                size_t            iHeight;           // height of the tree
                size_t            imultLeft;         // multiplicity at the left m_Object position
                size_t            imultRight;        // multiplicity at the right m_Object position
#endif
                serialin >> token;
                if (!serialin.good()) break;
                if (token == string("}")) break;
                
                if (token != string("{")) {
                    std::cout << sauc_NT_ckp_names[choiceAlgorithm-1] << " badly formatted, no NearTreeNode '{' token " << token << std::endl;
                    break;
                }
                
                serialin >> token;
                if (!serialin.good()) break;
                longval = atol(token.c_str());
                if (longval != -1) {
                    ptLeft = size_t(longval);
                } else {
                    ptLeft = ULONG_MAX;
                }
                
                serialin >> token;
                if (!serialin.good()) break;
                longval = atol(token.c_str());
                if (longval != -1) {
                    ptRight = size_t(longval);
                } else {
                    ptRight = ULONG_MAX;
                }
                
                serialin >> dMaxLeft >> dMaxRight;
                if (!serialin.good()) break;
                
                serialin >> token;
                if (!serialin.good()) break;
                longval = atol(token.c_str());
                if (longval != -1) {
                    pLeftBranch = size_t(longval);
                } else {
                    pLeftBranch = ULONG_MAX;
                }
                
                serialin >> token;
                if (!serialin.good()) break;
                longval = atol(token.c_str());
                if (longval != -1) {
                    pRightBranch = size_t(longval);
                } else {
                    pRightBranch = ULONG_MAX;
                }
                
                serialin >> token;
                if (!serialin.good()) break;
                longval = atol(token.c_str());
                if (longval != -1) {
                    iTreeSize = size_t(longval);
                } else {
                    iTreeSize = ULONG_MAX;
                }
                
#ifdef CNEARTREE_INSTRUMENTED
                serialin >> token;
                if (!serialin.good()) break;
                longval = atol(token.c_str());
                if (longval != -1) {
                    iHeight = size_t(longval);
                } else {
                    iHeight = ULONG_MAX;
                }
                
                serialin >> token;
                if (!serialin.good()) break;
                longval = atol(token.c_str());
                if (longval != -1) {
                    imultLeft = size_t(longval);
                } else {
                    imultLeft = ULONG_MAX;
                }
                
                serialin >> token;
                if (!serialin.good()) break;
                longval = atol(token.c_str());
                if (longval != -1) {
                    imultRight = size_t(longval);
                } else {
                    imultRight = ULONG_MAX;
                }
#endif
                serialin >> token; if (!serialin.good() || token != string("}")) {
                    std::cout << sauc_NT_ckp_names[choiceAlgorithm-1] << " badly formatted, no NearTreeNode '}' token: " << token <<std::endl;
                    break;
                }
                
                if (!BaseNode) {
                    BaseNode = new CNearTree<unitcell>::NearTreeNode<unitcell>
                    (ptLeft, ptRight, dMaxLeft, dMaxRight,pLeftBranch, pRightBranch,iTreeSize
#ifdef CNEARTREE_INSTRUMENTED
                     , iHeight, imultLeft, imultRight
#endif
                     , * NearTreeNodes, *ObjectStore, *ObjectCollide);
                } else {
                    NearTreeNodes->push_back(new CNearTree<unitcell>::NearTreeNode<unitcell>
                                             (ptLeft, ptRight, dMaxLeft, dMaxRight,pLeftBranch, pRightBranch,iTreeSize
#ifdef CNEARTREE_INSTRUMENTED
                                              , iHeight, imultLeft, imultRight
#endif
                                              , * NearTreeNodes, * ObjectStore, * ObjectCollide));
                }
                
            }
            if (!serialin.good()) {
                std::cout << sauc_NT_ckp_names[choiceAlgorithm-1] << " badly formatted NearTreeNodes" << std::endl;
                break;
            }
            
            serialin >> token; if (!serialin.good() || token != string("Flags:")) {
                std::cout << sauc_NT_ckp_names[choiceAlgorithm-1] << " badly formatted, no 'Flags:' token" << std::endl;
                break;
            }
            
            serialin >> Flags; if (!serialin.good()) {
                std::cout << sauc_NT_ckp_names[choiceAlgorithm-1] << " badly formatted Flags" << std::endl;
                break;
            }
            
            serialin >> token; if (!serialin.good() || token != string("DiamEstimate:")) {
                std::cout << sauc_NT_ckp_names[choiceAlgorithm-1] << " badly formatted, no 'DiamEstimate:' token" << std::endl;
                break;
            }
            
            serialin >> DiamEstimate; if (!serialin.good()) {
                std::cout << sauc_NT_ckp_names[choiceAlgorithm-1] << " badly formatted DiamEstimate" << std::endl;
                break;
            }
            
            serialin >> token; if (!serialin.good() || token != string("SumSpacings:")) {
                std::cout << sauc_NT_ckp_names[choiceAlgorithm-1] << " badly formatted, no 'SumSpacings:' token" << std::endl;
                break;
            }
            
            serialin >> SumSpacings; if (!serialin.good()) {
                std::cout << sauc_NT_ckp_names[choiceAlgorithm-1] << " badly formatted SumSpacings" << std::endl;
                break;
            }
            
            serialin >> token; if (!serialin.good() || token != string("SumSpacingsSq:")) {
                std::cout << sauc_NT_ckp_names[choiceAlgorithm-1] << " badly formatted, no 'SumSpacingsSq:' token" << std::endl;
                break;
            }
            
            serialin >> SumSpacingsSq; if (!serialin.good()) {
                std::cout << sauc_NT_ckp_names[choiceAlgorithm-1] << " badly formatted SumSpacings" << std::endl;
                break;
            }
            
            serialin >> token; if (!serialin.good() || token != string("DimEstimate:")) {
                std::cout << sauc_NT_ckp_names[choiceAlgorithm-1] << " badly formatted, no 'DimEstimate:' token" << std::endl;
                break;
            }
            
            serialin >> DimEstimate; if (!serialin.good()) {
                std::cout << sauc_NT_ckp_names[choiceAlgorithm-1] << " badly formatted DimEstimate" << std::endl;
                break;
            }
            
            serialin >> token; if (!serialin.good() || token != string("DimEstimateEsd:")) {
                std::cout << sauc_NT_ckp_names[choiceAlgorithm-1] << " badly formatted, no 'DimEstimateEsd:' token" << std::endl;
                break;
            }
            
            serialin >> DimEstimateEsd; if (!serialin.good()) {
                std::cout << sauc_NT_ckp_names[choiceAlgorithm-1] << " badly formatted DimEstimateEsd" << std::endl;
                break;
            }
            
#ifdef CNEARTREE_INSTRUMENTED
            serialin >> token; if (!serialin.good() || token != string("NodeVisits:")) {
                std::cout << sauc_NT_ckp_names[choiceAlgorithm-1] << " badly formatted, no 'NodeVisits:' token" << std::endl;
                break;
            }
            
            serialin >> NodeVisits; if (!serialin.good()) {
                std::cout << sauc_NT_ckp_names[choiceAlgorithm-1] << " badly formatted NodeVisits" << std::endl;
                break;
            }
#endif
            cellTree[choiceAlgorithm-1] =
            new CNearTree <unitcell> (*DelayedIndices, *ObjectStore, *ObjectCollide, DeepestDepth,
                                      *NearTreeNodes, *BaseNode, Flags,DiamEstimate, SumSpacings, SumSpacingsSq,
                                      DimEstimate, DimEstimateEsd  // estimated dimension estimated standard deviation
#ifdef CNEARTREE_INSTRUMENTED
                                      , NodeVisits
#endif
                                      );
            gotckp = true;
            delete DelayedIndices;
            delete ObjectStore;
            delete ObjectCollide;
            delete NearTreeNodes;
            delete BaseNode;
            
        }
        break;
    }
    if (serialin.is_open()) serialin.close();
    if (gotckp) return;
    
    
    cellTree[choiceAlgorithm-1] = new CNearTree <unitcell> ();
    
    std::cout << "processing " << (int)(cellDArray.size()) << " cells" << std::endl;
    
    for (size_t i = 0; i < (size_t)(cellDArray.size()); i++)
    {
        double d7cell[7];
        double d7red[7];
        double s6cell[6];
        double s6red[6];
        G6 g6cent;
        double dmc[36];
        int reduced;
        Cell rawcell(cellDArray[i][0], cellDArray[i][1], cellDArray[i][2], cellDArray[i][3], cellDArray[i][4], cellDArray[i][5]);
        if (i%20000 == 0) std::cout << "." ;
        g6cent=rawcell.Cell2V6();
        CS6M_LatSymMat66(g6cent,spaceArray[i].c_str()[0],dmc,primcell);
        switch (choiceAlgorithm) {
            case 1:  /* L1 */
            case 2:  /* L2 */
            case 3:  /* NCDist */
            case 4:  /* V7 */
            case 7:  /* DC7unsrt */
                 CS6M_G6Reduce(primcell,redprimcell,reduced);
                 break;
            case 5:  /* D7Dist */
                 CS6M_G6toD7(primcell,d7cell);
                 CS6M_D7Reduce(d7cell,d7red,reduced);
                 CS6M_D7toG6(d7red,redprimcell);
                 break;
            case 6:  /* S6Dist */
            default: /* and anything else */
                 CS6M_G6toS6(primcell,s6cell);
                 CS6M_S6Reduce(s6cell,s6red,reduced);
                 CS6M_S6toG6(s6red,redprimcell);
                 break;

        }
        /* if (i < 25 || (i > 357200 &&  i < 357210)) {
          std::cout << "i:  " << i  << (spaceArray[i]).substr(0,1) 
          << " " << cellDArray[i][0] << " " << cellDArray[i][1] << " " <<  cellDArray[i][2] 
          << " " << cellDArray[i][3] << " " << cellDArray[i][4] << " " <<  cellDArray[i][5] << std::endl
          << "primcell: " << primcell[0] << " " << primcell[1] << " " << primcell[2] << " " 
          << primcell[3] << " " << primcell[4] << " " << primcell[5] << std::endl
          << "redprimcell: " << redprimcell[0] << " " << redprimcell[1] << " " << redprimcell[2] << " " 
          << redprimcell[3] << " " << redprimcell[4] << " " << redprimcell[5] << std::endl; 
        } */
             
        if (!reduced){
          std::cout << "Reduction failed for "<<idArray[i]<<" "<<
          cellDArray[i][0]<<" "<<
          cellDArray[i][1]<<" "<<
          cellDArray[i][2]<<" "<<
          cellDArray[i][3]<<" "<<
          cellDArray[i][4]<<" "<<
          cellDArray[i][5]<<" "<<
          spaceArray[i] << std::endl;
          std::cout << "Primitive G6 " << primcell[0]<<" "<<
          primcell[1]<<" "<<
          primcell[2]<<" "<<
          primcell[3]<<" "<<
          primcell[4]<<" "<<
          primcell[5] << std::endl;
        };
        searchcell = Cell(redprimcell).CellWithDegrees();
        dsearchcell[0]=searchcell[0];
        dsearchcell[1]=searchcell[1];
        dsearchcell[2]=searchcell[2];
        dsearchcell[3]=searchcell[3];
        dsearchcell[4]=searchcell[4];
        dsearchcell[5]=searchcell[5];
        unitcell cellData(dsearchcell,spaceArray[i].c_str()[0],(size_t)i,cellDB[i],choiceAlgorithm);
        cellTree[choiceAlgorithm-1]->insert(cellData);
        
    }
    
    std::cout << "Neartree loaded, dumping checkpoint " << std::endl;
    std::cout << "CompleteDelayed Insert" << std::endl;
    
    cellTree[choiceAlgorithm-1]->CompleteDelayedInsert();
    cellTree_itend[choiceAlgorithm-1] = cellTree[choiceAlgorithm-1]->end();
    std::cout << "Get_Checkpoint" << std::endl;
    cellTree[choiceAlgorithm-1]->Get_Checkpoint(&DelayedIndices,
                                                &ObjectStore,
                                                &ObjectCollide,
                                                &DeepestDepth,
                                                &NearTreeNodes,
                                                &BaseNode,
                                                &Flags,
                                                &DiamEstimate,
                                                &SumSpacings,
                                                &SumSpacingsSq,
                                                &DimEstimate,
                                                &DimEstimateEsd
#ifdef CNEARTREE_INSTRUMENTED
                                                , &NodeVisits
#endif
                                                );
    std::cout << "Dump" << std::endl;
    serialout.open(sauc_NT_ckp_names[choiceAlgorithm-1].c_str(),std::ios::out|std::ios::trunc);
    if (serialout.is_open()) {
        serialout << "Metric: "<< choiceAlgorithm << std::endl;
        serialout << "DelayedIndices: { ";
        for (si = 0; si < DelayedIndices->size(); si++) {
            serialout << (*DelayedIndices)[si];
            if (si+1 < DelayedIndices->size()) serialout <<" ";
            if (si%64==63) serialout << std::endl;
        }
        serialout << " }" << std::endl;
        
        serialout << "ObjectStore: { ";
        for (si = 0; si < ObjectCollide->size(); si++) {
            ucell = (*ObjectStore)[si];
            ucell.getCell(cell,&row, &dbid);
            /* std::cout << "{ " << cell[0] << " "
             << cell[1] << " "
             << cell[2] << " "
             << cell[3] << " "
             << cell[4] << " "
             << cell[5] << " "
             << " " << row << " " << dbid << " }"  << std::endl; */
            
            serialout << "{ " << cell[0] << " "
            << cell[1] << " "
            << cell[2] << " "
            << cell[3] << " "
            << cell[4] << " "
            << cell[5] << " "
            << " " << row << " " << dbid << " }"  << std::endl;
        }
        serialout << " }" << std::endl;
        
        serialout << "ObjectCollide: { ";
        for (si = 0; si < ObjectCollide->size(); si++) {
            sv = (*ObjectCollide)[si];
            putoutst(serialout,sv);
            if (si+1 < ObjectCollide->size()) serialout <<" ";
            if (si%64==63) serialout << std::endl;
        }
        serialout << " }" << std::endl;
        
        serialout << "DeepestDepth: " << DeepestDepth << std::endl;
        
        serialout << "NearTreeNodes: { ";
        
        serialout << "{ ";
        putoutst(serialout,BaseNode->m_ptLeft); serialout << " ";
        putoutst(serialout,BaseNode->m_ptRight); serialout << " ";
        serialout << BaseNode->m_dMaxLeft << " "<< BaseNode->m_dMaxRight << " ";
        putoutst(serialout,BaseNode->m_pLeftBranch); serialout << " ";
        putoutst(serialout,BaseNode->m_pRightBranch); serialout << " ";
        putoutst(serialout,BaseNode->m_iTreeSize); serialout << " ";
#ifdef CNEARTREE_INSTRUMENTED
        putoutst(serialout,BaseNode->m_iHeight); serialout << " ";
        putoutst(serialout,BaseNode->m_imultLeft); serialout << " ";
        putoutst(serialout,BaseNode->m_imultRight); serialout << " ";
#endif
        serialout << " }" << std::endl;
        
        for (si = 0; si < NearTreeNodes->size(); si++) {
            curntn= (*NearTreeNodes)[si];
            serialout << "{ ";
            putoutst(serialout,curntn->m_ptLeft); serialout << " ";
            putoutst(serialout,curntn->m_ptRight); serialout << " ";
            serialout << curntn->m_dMaxLeft << " " << curntn->m_dMaxRight << " ";
            putoutst(serialout,curntn->m_pLeftBranch); serialout << " ";
            putoutst(serialout,curntn->m_pRightBranch); serialout << " ";
            putoutst(serialout,curntn->m_iTreeSize); serialout << " ";
#ifdef CNEARTREE_INSTRUMENTED
            putoutst(serialout,curntn->m_iHeight); serialout << " ";
            putoutst(serialout,curntn->m_imultLeft); serialout << " ";
            putoutst(serialout,curntn->m_imultRight); serialout << " ";
#endif
            serialout << " }";
            if (si+1 < NearTreeNodes->size()) serialout <<std::endl;
        }
        serialout << " }" << std::endl;
        serialout << "Flags: " << Flags  << std::endl;
        serialout << "DiamEstimate: " << DiamEstimate  << std::endl;
        serialout << "SumSpacings: " << SumSpacings  << std::endl;
        serialout << "SumSpacingsSq: " << SumSpacingsSq  << std::endl;
        serialout << "DimEstimate: " << DimEstimate  << std::endl;
        serialout << "DimEstimateEsd: " << DimEstimateEsd  << std::endl;
#ifdef CNEARTREE_INSTRUMENTED
        serialout << "NodeVisits: " << NodeVisits  << std::endl;
#endif
        serialout.close();
        delete DelayedIndices;
        delete ObjectStore;
        delete ObjectCollide;
        delete NearTreeNodes;
        delete BaseNode;
        
    } else {
        std::cout << "unable to save database checkpoint file "<<sauc_NT_ckp_names[choiceAlgorithm-1] << std::endl;
    }
}


//*****************************************************************************/
void NearestInputReport( std::ostream& out, const G6& probeArray, const G6& primredprobe )
{
    out << "Raw Unknown Cell [" <<
    probeArray[0] << ", " <<
    probeArray[1] << ", " <<
    probeArray[2] << ", " <<
    probeArray[3] << ", " <<
    probeArray[4] << ", " <<
    probeArray[5] <<
    "] Lattice: "<< probelattice <<  std::endl;
    out << "Reduced Primitive Cell: [" <<
    primredprobe[0] << ", " <<
    primredprobe[1] << ", " <<
    primredprobe[2] << ", " <<
    primredprobe[3] << ", " <<
    primredprobe[4] << ", " <<
    primredprobe[5] << "]" << std::endl;
}

//*****************************************************************************
void findNearest( int k, double distance, int dont_sort_by_family )
{
    vector <unitcell> myvector;
    vector <size_t> myindices;
    vector <double> mydistances;
    long spheredata;
    double dtest;
    
    std::cout << "entering findNearest " << k << distance << std::endl;
    unitcell unknownCell = unitcell(primredprobe[0], primredprobe[1], primredprobe[2], primredprobe[3], primredprobe[4], primredprobe[5],(size_t)0L,'p');
    NearestInputReport( std::cout, probeArray, primredprobe );
    
    std::cout <<  "crootvol: " << crootvol << std::endl;
    dtest = distance;
    // if (distance > crootvol*.025 || distance <= 0.) dtest = crootvol*.025;
    if (distance <= 0.) dtest = 1.e38;
    if (distance > 0. && dtest > distance) dtest = distance;
    
    /*std::cout <<"calling FindK_NearestNeighbors " << "k: "<<k << " dtest: " << dtest << std::endl;*/
    spheredata = cellTree[choiceAlgorithm-1]->FindK_NearestNeighbors(k,dtest, myvector,
                                                                     myindices,mydistances,unknownCell);
    std::cout <<"returned spheredata " << spheredata << std::endl;
    /*if (spheredata <  k && (dtest < distance || distance <= 0.) )  {
     dtest = distance;
     if (dtest <= 0.) dtest = crootvol;
     std::cout <<"calling FindK_NearestNeighbors " << "k: "<<k << " dtest: " << dtest << std::endl;
     spheredata = cellTree[choiceAlgorithm-1]->FindK_NearestNeighbors(k,dtest, myvector,
     myindices,mydistances,unknownCell);
     std::cout <<"returned spheredata " << spheredata << std::endl;
     
     if (spheredata <  (k+1)/2 && (dtest < distance || distance <= 0.) )  {
     dtest = crootvol*2.;
     std::cout <<"calling FindK_NearestNeighbors " << "k: "<<k << " dtest: " << dtest << std::endl;
     spheredata = cellTree[choiceAlgorithm-1]->FindK_NearestNeighbors(k,dtest, myvector,
     myindices,mydistances,unknownCell);
     std::cout <<"returned spheredata " << spheredata << std::endl;
     
     if (spheredata <  1 )  {
     dtest = crootvol*5.;
     std::cout <<"calling FindK_NearestNeighbors " << "k: "<<k << " dtest: " << dtest << std::endl;
     spheredata = cellTree[choiceAlgorithm-1]->FindK_NearestNeighbors(k,dtest, myvector,
     myindices,mydistances,unknownCell);
     std::cout <<"returned spheredata " << spheredata << std::endl;
     if (spheredata < 1) {
     spheredata = cellTree[choiceAlgorithm-1]->FindK_NearestNeighbors(k,1.e38, myvector,
     myindices,mydistances,unknownCell);
     }
     }
     }
     }*/
    if (spheredata != 0) {
        std::cout << "Depth: " << cellTree[choiceAlgorithm-1]->GetDepth() << std::endl;
        SphereResults( std::cout, myvector, myindices, mydistances, unknownCell, dont_sort_by_family);
        
        if (!sauc_batch_mode)
            std::cout << "File name if you want the output saved" << std::endl << std::flush;
        
        std::string s;
        std::getline( std::cin, s );
        std::cin.clear();
        if ( ! s.empty() )
        {
            std::string filename;
            std::stringstream ss(s);
            if ( ss >> filename )
            {
                std::cout << " ";
                std::ofstream output( filename.c_str() );
                NearestInputReport( output, probeArray, primredprobe );
                SphereResults( output, myvector, myindices, mydistances, unknownCell, dont_sort_by_family);
                output.close();
                if (sauc_batch_mode)
                    std::cout << "Output saved to: " << filename << std::endl;
            }
        }
    }
    
}

//*****************************************************************************
void SphereResults( std::ostream& out,
                   const std::vector<unitcell>&  myvector,
                   const std::vector<size_t>&    myindices,
                   const std::vector<double>&    mydistances,
                   const unitcell&               unknownCell,
                   const int&                    dont_sort_by_family)
{
    long entry;
    string head;
    vector<long> myentries;
    vector<long> myfamilies;
    vector<long> myfamiliesordinal;
    vector<long> myfamilylist;
    vector<long> myfamilycount;
    vector<long> mythread;
    PSM_string * split_fields = NULL;
    std::string myfamily;
    size_t myfamily_index=0;
    
    int numRow;
    myentries.assign(myvector.size(),-1L);   /* The entry with this PDB ID */
    myfamilies.assign(myvector.size(),-1L);  /* The base entry with the same header */
    myfamiliesordinal.assign(myvector.size(),-1L);/* ordinal in the family list */
    myfamilylist.assign(myvector.size(),-1L);/* List in distance order of first
                                              entry in each family */
    myfamilycount.assign(myvector.size(),-1L);/* counts of hits in each family */
    mythread.assign(myvector.size(),-1L);     /* links on hashed headers */
    long ind, family_size;
    long nextthread, prevthread, firstthread, numhit, thread[257];
    int ii;
    family_size=0;
    int PDBcount=0, CODcount=0, CSDcount=0;
    
    split_fields = (PSM_string *)malloc(sizeof(PSM_string)*16);
    
    for (ii=0; ii < 257; ii++) thread[ii] = -1;
    out << "\nSphere Results " << myvector.size() << " Cells" <<std::endl;
    /* find the header threads of the results,
     this depends on the assumption that the original
     list is sorted by distance
     */
    for (ind=0; ind < (long)myvector.size(); ind++) {
        myentries[ind] = myfamilies[ind] = myfamiliesordinal[ind] = myfamilylist[ind] = myfamilycount[ind] = mythread[ind] = -1;
    }
    for (ind=0; ind < (long)myvector.size(); ind++) {
        char dbtype;
        const unitcell * const cell = & myvector[ind];
        std::string dbname;
        numRow = (int)(*((unitcell *)cell)).getRow();
        dbtype = cellDB[numRow];
        entry =  PSM_getstrindex_by_key(
                                        (dbtype==PDB_DBTYPE)?PDBentries:
                                        ((dbtype==CSD_DBTYPE)?CSDcells:CODentries),
                                        idArray[numRow].c_str(),0);
        if (entry < 0) {
            out << "entry for "<< idArray[numRow] << " is negative, row " << numRow << std::endl;
            myentries[ind] = -1;
            myfamilies[ind] = -1;
            myfamily = std::string("__UNK__");
            dbname = std::string("UNK");
        } else {
            /* Split the entry row into fields and extract the family */
            
            switch (dbtype) {
                    
                case PDB_DBTYPE:
		    PDBcount++;
                    if (PSM_split_psm_string(PDBentries,split_fields,PDBentries->maxfieldno,
                                             PDBentries->str_index[entry],PDBentries_sep_char)) {
                        dbname = std::string("PDB");
                        myfamily = std::string(PDBentries->chars+split_fields[PDBentries_head].offset,
                                               split_fields[PDBentries_head].length);
                    }
                    break;
                case CSD_DBTYPE:
		    CSDcount++;
                    if (PSM_split_psm_string(CSDcells,split_fields,CSDcells->maxfieldno,
                                             CSDcells->str_index[entry],CSDcells_sep_char)) {
                        dbname = std::string("CSD");
                        myfamily = std::string(CSDcells->chars+split_fields[CSDcells_refcode_family].offset,
                                               split_fields[CSDcells_refcode_family].length);
                    }
                    break;
                case COD_DBTYPE:
		    CODcount++;
                    if (PSM_split_psm_string(CODentries,split_fields,CODentries->maxfieldno,
                                             CODentries->str_index[entry],CODentries_sep_char)) {
                        dbname = std::string("COD");
                        myfamily = std::string(CODentries->chars+split_fields[CODentries_chemical_formula_sum].offset,
                                               split_fields[CODentries_chemical_formula_sum].length);
                    }
                    break;
                    
                default: myfamily = std::string("__UNK__"); break;
            }
            
            
            myentries[ind] = entry;                                                                   
            /* std::cerr << "ind, myfamily: " << ind << " " << myfamily << std::endl; */
            myfamilies[ind] = PSM_getstrindex_by_key((dbtype==PDB_DBTYPE)?PDBentries:
                                                     ((dbtype==CSD_DBTYPE)?CSDcells:CODentries),
                                                     myfamily.c_str(),1);
            if (myfamilies[ind] < 0 ) {
                out << "family for "<< idArray[numRow] <<
                " row " << numRow <<
                " in " << dbname <<
                " is unknown" << std::endl;
                std::cerr << "family for "<< idArray[numRow] <<
                " row " << numRow <<
                " in " << dbname <<
                " is unknown" << std::endl;
            }
        }
        ii = (myfamilies[ind])&0xFF;  /* hash code for families */
        if (entry < 0) ii = 256;
        nextthread = thread[ii];
        if (nextthread < 0) {
            thread[ii] = ind;
            myfamiliesordinal[ind] = family_size;
            mythread[ind] = -1;
            myfamilylist[family_size] = ind;
            myfamilycount[family_size] = 1L;
            family_size++;
        } else {
            firstthread = -1;
            prevthread = -1;
            numhit = 0;
            while (nextthread >= 0) {
                prevthread = nextthread;
                nextthread = mythread[prevthread];
                if (myfamilies[prevthread] == myfamilies[ind]) {
                    if (numhit==0) firstthread = prevthread;
                    numhit++;
                    myfamiliesordinal[ind] = myfamiliesordinal[prevthread];
                }
            }
            if (numhit==0) {
                myfamiliesordinal[ind] = family_size;
                myfamilylist[family_size] = ind;
                myfamilycount[family_size] = 1L;
                family_size++;
            } else {
                myfamiliesordinal[ind] = myfamiliesordinal[firstthread];
                myfamilycount[myfamiliesordinal[ind]]++;
            }
            mythread[prevthread] = ind;
            mythread[ind] = -1;
        }
    }
    
    out << "Found " << family_size << " families organized by ";
	if (PDBcount > 0) out << "PDB header";
	if (PDBcount > 0 && (CSDcount > 0 || CODcount > 0 )) out << ", ";
	if (CSDcount > 0 ) out << "CSD Refcode";
	if (CSDcount > 0 && CODcount > 0 ) out << ", ";
	if (CODcount > 0) out << "COD formula";
	out << std::endl << std::endl;

    if (dont_sort_by_family) for (ind=0; ind < (long)myvector.size(); ind++) {
        char dbtype;
        std::string dbname;
        size_t numfields;
        ssize_t compoundfield;
        ssize_t sourcefield;
        ssize_t resfield;
        ssize_t expfield;
        std::string myfamily;
        long familyordinal;
        myfamily = myfamilies[ind];
        string pdbid;
        {
            char dbtype;
            const unitcell * const cell = & myvector[ind];
            numRow = (int)((*(unitcell *)cell).getRow());
            dbtype = cellDB[numRow];
            entry =  PSM_getstrindex_by_key(
                                            (dbtype==PDB_DBTYPE)?PDBentries:
                                            ((dbtype==CSD_DBTYPE)?CSDcells:CODentries),
                                            idArray[numRow].c_str(),0);
            /* Split the entry row into fields and extract the family */
            
            switch (dbtype) {
                    
                case PDB_DBTYPE:
                    if (PSM_split_psm_string(PDBentries,split_fields,PDBentries->maxfieldno,
                                             PDBentries->str_index[entry],PDBentries_sep_char)) {
                        dbname = std::string("PDB");
                        myfamily = PSM_getstringfield(PDBentries,split_fields[PDBentries_head]);
                        myfamily_index = PSM_getstrindex_by_key(PDBentries,myfamily.c_str(),1);
                    }
                    break;
                case CSD_DBTYPE:
                    if (PSM_split_psm_string(CSDcells,split_fields,CSDcells->maxfieldno,
                                             CSDcells->str_index[entry],CSDcells_sep_char)) {
                        dbname = std::string("CSD");
                        myfamily = PSM_getstringfield(CSDcells,split_fields[CSDcells_refcode_family]);
                        myfamily_index = PSM_getstrindex_by_key(CSDcells,myfamily.c_str(),1);
                    }
                    break;
                case COD_DBTYPE:
                    if (PSM_split_psm_string(CODentries,split_fields,CODentries->maxfieldno,
                                             CODentries->str_index[entry],CODentries_sep_char)) {
                        dbname = std::string("COD");
                        myfamily = PSM_getstringfield(CODentries,split_fields[CODentries_chemical_formula_sum]);
                        myfamily_index = PSM_getstrindex_by_key(CODentries,myfamily.c_str(),1);
                        break;
                        
                    }
                default:  myfamily_index=-1; myfamily = "UNKNOWN FAMILY";break;
            }
            
            if (sauc_javascript) {
                if (dbtype==CSD_DBTYPE) {
                    pdbid = "<b><a href=\"https://summary.ccdc.cam.ac.uk/structure-summary?refcode=" +
                    idArray[numRow] + "\" target=\"_blank\">" + idArray[numRow] + "</a></b>";
                } else if (dbtype==COD_DBTYPE) {
                    pdbid = "<b><a href=\"http://www.crystallography.net/cod/"+idArray[numRow]+".html\"" +
                    "\" target=\"_blank\">" + idArray[numRow] + "</a></b>";
                } else {
                    pdbid = "<b><a href=\"http://www.rcsb.org/pdb/explore.do?structureId=" +
                    idArray[numRow] + "\" target=\"_blank\">" + idArray[numRow] + "</a></b>";
                }
            } else {
                if (dbtype==CSD_DBTYPE) {
                    pdbid = "CSD:"+idArray[numRow];
                } else if (dbtype==COD_DBTYPE) {
                    pdbid = "COD:"+idArray[numRow];
                } else {
                    pdbid = "PDB:"+idArray[numRow];
                }
            }
            {
                char dbtype;
                
                dbtype = cellDB[numRow];
                entry = PSM_getstrindex_by_key(
                                               (dbtype==PDB_DBTYPE)?PDBentries:
                                               ((dbtype==CSD_DBTYPE)?CSDcells:CODentries),
                                               idArray[numRow].c_str(),0);
                out << ind+1 << ": "<< html_escape(myfamily);

                out << std::endl;
            }
            out << "    " << pdbid << " Dist: " <<
            mydistances[ind] << " Cell: [" <<
            cellDArray[numRow][0] << ", " <<
            cellDArray[numRow][1] << ", " <<
            cellDArray[numRow][2] << ", " <<
            cellDArray[numRow][3] << ", " <<
            cellDArray[numRow][4] << ", " <<
            cellDArray[numRow][5] << "], SG: " <<
            spaceArray[numRow];
            if (zArray[numRow] > 0) {
                out << ", Z: " <<
                zArray[numRow] << " ";
            } else {
                out << ", ";
            }
            out << " Prim. Red. Cell: ["<<
            (*((unitcell *)cell)).getData(0) << ", " <<
            (*((unitcell *)cell)).getData(1) << ", " <<
            (*((unitcell *)cell)).getData(2) << ", " <<
            (*((unitcell *)cell)).getData(3) << ", " <<
            (*((unitcell *)cell)).getData(4) << ", " <<
            (*((unitcell *)cell)).getData(5) << "]" << std::endl;
            dbtype = cellDB[numRow];
            entry =  PSM_getstrindex_by_key(
                                            (dbtype==PDB_DBTYPE)?PDBentries:
                                            ((dbtype==CSD_DBTYPE)?CSDcells:CODentries),
                                            idArray[numRow].c_str(),0);
            if (entry >= 0) {
                PSM_localpsm_handle psm_handle;
                if (dbtype==PDB_DBTYPE) {
                    dbname = std::string("PDB");
                    psm_handle = PDBentries;
                    numfields = PDBentries_numfields;
                    compoundfield = PDBentries_cmpd;
                    sourcefield = PDBentries_src;
                    resfield = PDBentries_res;
                    expfield = PDBentries_exp;
                    
                }
                else if (dbtype==CSD_DBTYPE) {
                    dbname = std::string("CSD");
                    psm_handle = CSDcells;
                    numfields = CSDcells_numfields;
                    compoundfield = CSDcells_refcode_family;
                    sourcefield = -1;
                    resfield = -1;
                    expfield = -1;
                    
                }
                else {
                    dbname = std::string("COD");
                    psm_handle = CODentries;
                    numfields = CODentries_numfields;
                    compoundfield = CODentries_chemical_formula_sum;;
                    sourcefield = CODentries_chemical_name_common;
                    resfield = -1;
                    expfield = -1;
                }
                
                PSM_split_psm_string(psm_handle,split_fields,psm_handle->maxfieldno,
                                     psm_handle->str_index[entry],psm_handle->split_char);
                if (compoundfield >= 0 && split_fields[compoundfield].length > 0 &&
                    compoundfield < psm_handle->maxfieldno) {
                    out << "      " <<
                    html_escape(PSM_getstringfield(psm_handle,split_fields[(size_t)compoundfield])) << std::endl;
                }
                if (sourcefield >= 0 && split_fields[sourcefield].length > 0 &&
                    sourcefield < psm_handle->maxfieldno) {
                    out << "      " <<
                    html_escape(PSM_getstringfield(psm_handle,split_fields[(size_t)sourcefield]));
                }
                if (resfield >= 0 && split_fields[resfield].length > 0 &&
                    resfield < psm_handle->maxfieldno) {
                    out << " " << PSM_getstringfield(psm_handle,split_fields[(size_t)resfield]);
                }
                if (expfield >= 0 && split_fields[expfield].length> 0 &&
                    expfield < psm_handle->maxfieldno) {
                    out << " " << PSM_getstringfield(psm_handle,split_fields[(size_t)expfield]);;
                }
                out << std::endl << std::flush;
            }
        }
        out << std::endl << std::flush;
    }

    if (!dont_sort_by_family) for (ii = 0; ii < family_size; ii++) {
        char dbtype;
        std::string dbname;
        size_t numfields;
        ssize_t compoundfield;
        ssize_t sourcefield;
        ssize_t resfield;
        ssize_t expfield;
        std::string myfamily;
        long familyordinal;
        ind = myfamilylist[ii];
        myfamily = myfamilies[ind];
        familyordinal = 0;
        string pdbid;
        while (ind >= 0) {
            char dbtype;
            const unitcell * const cell = & myvector[ind];
            numRow = (int)(*((unitcell *)cell)).getRow();
            dbtype = cellDB[numRow];
            entry =  PSM_getstrindex_by_key(
                                            (dbtype==PDB_DBTYPE)?PDBentries:
                                            ((dbtype==CSD_DBTYPE)?CSDcells:CODentries),
                                            idArray[numRow].c_str(),0);
            /* Split the entry row into fields and extract the family */
            
            switch (dbtype) {
                    
                case PDB_DBTYPE:
                    if (PSM_split_psm_string(PDBentries,split_fields,PDBentries->maxfieldno,
                                             PDBentries->str_index[entry],PDBentries_sep_char)) {
                        dbname = std::string("PDB");
                        myfamily = PSM_getstringfield(PDBentries,split_fields[PDBentries_head]);
                        myfamily_index = PSM_getstrindex_by_key(PDBentries,myfamily.c_str(),1);
                    }
                    break;
                case CSD_DBTYPE:
                    if (PSM_split_psm_string(CSDcells,split_fields,CSDcells->maxfieldno,
                                             CSDcells->str_index[entry],CSDcells_sep_char)) {
                        dbname = std::string("CSD");
                        myfamily = PSM_getstringfield(CSDcells,split_fields[CSDcells_refcode_family]);
                        myfamily_index = PSM_getstrindex_by_key(CSDcells,myfamily.c_str(),1);
                    }
                    break;
                case COD_DBTYPE:
                    if (PSM_split_psm_string(CODentries,split_fields,CODentries->maxfieldno,
                                             CODentries->str_index[entry],CODentries_sep_char)) {
                        dbname = std::string("COD");
                        myfamily = PSM_getstringfield(CODentries,split_fields[CODentries_chemical_formula_sum]);
                        myfamily_index = PSM_getstrindex_by_key(CODentries,myfamily.c_str(),1);
                        break;
                        
                    }
                default:  myfamily_index=-1; myfamily = "UNKNOWN FAMILY";break;
            }
            
            if (sauc_javascript) {
                if (dbtype==CSD_DBTYPE) {
                    pdbid = "<b><a href=\"https://summary.ccdc.cam.ac.uk/structure-summary?refcode=" +
                    idArray[numRow] + "\" target=\"_blank\">" + idArray[numRow] + "</a></b>";
                } else if (dbtype==COD_DBTYPE) {
                    pdbid = "<b><a href=\"http://www.crystallography.net/cod/"+idArray[numRow]+".html\"" +
                    "\" target=\"_blank\">" + idArray[numRow] + "</a></b>";
                } else {
                    pdbid = "<b><a href=\"http://www.rcsb.org/pdb/explore.do?structureId=" +
                    idArray[numRow] + "\" target=\"_blank\">" + idArray[numRow] + "</a></b>";
                }
            } else {
                if (dbtype==CSD_DBTYPE) {
                    pdbid = "CSD:"+idArray[numRow];
                } else if (dbtype==COD_DBTYPE) {
                    pdbid = "COD:"+idArray[numRow];
                } else {
                    pdbid = "PDB:"+idArray[numRow];
                }
            }
            familyordinal++;
            if (familyordinal == 1) {
                char dbtype;
                
                dbtype = cellDB[numRow];
                entry = PSM_getstrindex_by_key(
                                               (dbtype==PDB_DBTYPE)?PDBentries:
                                               ((dbtype==CSD_DBTYPE)?CSDcells:CODentries),
                                               idArray[numRow].c_str(),0);
                out << ii+1 << ": "<< html_escape(myfamily);

                if (myfamilycount[ii] > 1) {
                    out << " [" << myfamilycount[ii] << " cells found] ";
                    if (sauc_javascript) {
                        out << "<a href=\"javascript:open_close('famfloat"<< ii+1 << "');\">(expand/collapse)</a>";
                    }
                }
                out << std::endl;
            }
            if (familyordinal == 2 && sauc_javascript) {
                out << "<div id=\"famfloat" << ii+1 <<
                "\" style=\"display:none\">";
            }
            out << "    " << familyordinal << ": " <<
            pdbid << " Dist: " <<
            mydistances[ind] << " Cell: [" <<
            cellDArray[numRow][0] << ", " <<
            cellDArray[numRow][1] << ", " <<
            cellDArray[numRow][2] << ", " <<
            cellDArray[numRow][3] << ", " <<
            cellDArray[numRow][4] << ", " <<
            cellDArray[numRow][5] << "], SG: " <<
            spaceArray[numRow];
            if (zArray[numRow] > 0) {
                out << ", Z: " <<
                zArray[numRow] << " ";
            } else {
                out << ", ";
            }
            out << " Prim. Red. Cell: ["<<
            (*((unitcell *)cell)).getData(0) << ", " <<
            (*((unitcell *)cell)).getData(1) << ", " <<
            (*((unitcell *)cell)).getData(2) << ", " <<
            (*((unitcell *)cell)).getData(3) << ", " <<
            (*((unitcell *)cell)).getData(4) << ", " <<
            (*((unitcell *)cell)).getData(5) << "]" << std::endl;
            dbtype = cellDB[numRow];
            entry =  PSM_getstrindex_by_key(
                                            (dbtype==PDB_DBTYPE)?PDBentries:
                                            ((dbtype==CSD_DBTYPE)?CSDcells:CODentries),
                                            idArray[numRow].c_str(),0);
            if (entry >= 0) {
                PSM_localpsm_handle psm_handle;
                if (dbtype==PDB_DBTYPE) {
                    dbname = std::string("PDB");
                    psm_handle = PDBentries;
                    numfields = PDBentries_numfields;
                    compoundfield = PDBentries_cmpd;
                    sourcefield = PDBentries_src;
                    resfield = PDBentries_res;
                    expfield = PDBentries_exp;
                    
                }
                else if (dbtype==CSD_DBTYPE) {
                    dbname = std::string("CSD");
                    psm_handle = CSDcells;
                    numfields = CSDcells_numfields;
                    compoundfield = CSDcells_refcode_family;
                    sourcefield = -1;
                    resfield = -1;
                    expfield = -1;
                    
                }
                else {
                    dbname = std::string("COD");
                    psm_handle = CODentries;
                    numfields = CODentries_numfields;
                    compoundfield = CODentries_chemical_formula_sum;;
                    sourcefield = CODentries_chemical_name_common;
                    resfield = -1;
                    expfield = -1;
                }
                
                PSM_split_psm_string(psm_handle,split_fields,psm_handle->maxfieldno,
                                     psm_handle->str_index[entry],psm_handle->split_char);
                if (compoundfield >= 0 && split_fields[compoundfield].length > 0 &&
                    compoundfield < psm_handle->maxfieldno) {
                    out << "      " <<
                    html_escape(PSM_getstringfield(psm_handle,split_fields[(size_t)compoundfield])) << std::endl;
                }
                if (sourcefield >= 0 && split_fields[sourcefield].length > 0 &&
                    sourcefield < psm_handle->maxfieldno) {
                    out << "      " <<
                    html_escape(PSM_getstringfield(psm_handle,split_fields[(size_t)sourcefield]));
                }
                if (resfield >= 0 && split_fields[resfield].length > 0 &&
                    resfield < psm_handle->maxfieldno) {
                    out << " " << PSM_getstringfield(psm_handle,split_fields[(size_t)resfield]);
                }
                if (expfield >= 0 && split_fields[expfield].length> 0 &&
                    expfield < psm_handle->maxfieldno) {
                    out << " " << PSM_getstringfield(psm_handle,split_fields[(size_t)expfield]);;
                }
                out << std::endl << std::flush;
            }
            ind = mythread[ind];
            while (ind >= 0) {
                if (myfamilies[ind] == myfamily_index) break;
                ind = mythread[ind];
            }
        }
        if (myfamilycount[ii] > 1 && sauc_javascript) {
            out << "</div>";
        }
        out << std::endl << std::flush;
    }
    if (split_fields) free(split_fields);
    
}

//*****************************************************************************/
void SphereInputReport( std::ostream& out )
{
    out << "Raw Unknown Cell: ["<<
    probeArray[0] << ", " <<
    probeArray[1] << ", " <<
    probeArray[2] << ", " <<
    probeArray[3] << ", " <<
    probeArray[4] << ", " <<
    probeArray[5] <<
    "] Lattice: "<< probelattice <<  std::endl;
    out << "Primitive Reduced Cell: [" <<
    primredprobe[0] << ", " <<
    primredprobe[1] << ", " <<
    primredprobe[2] << ", " <<
    primredprobe[3] << ", " <<
    primredprobe[4] << ", " <<
    primredprobe[5] << "]" << std::endl;
}

//*****************************************************************************/
void findSphere( int limit, int dont_sort_by_family )
{
    unitcell unknownCell = unitcell(primredprobe[0], primredprobe[1], primredprobe[2], primredprobe[3], primredprobe[4], primredprobe[5],(size_t)0L,'p');
    
    std::cout << std::endl;
    SphereInputReport( std::cout );
    vector <unitcell> myvector;
    vector <size_t> myindices;
    vector <double> mydistances;
    const long sphereData = cellTree[choiceAlgorithm-1]->FindK_NearestNeighbors(limit,sphereRange, myvector,
                                                                                myindices,mydistances,unknownCell);
    
    SphereResults( std::cout, myvector, myindices, mydistances, unknownCell, dont_sort_by_family);
    
    if (!sauc_batch_mode)
        std::cout << "File name if you want the output saved" << std::endl;
    
    std::string s;
    std::getline( std::cin, s );
    std::cin.clear();
    if ( ! s.empty() )
    {
        std::string filename;
        std::stringstream ss(s);
        if ( ss >> filename )
        {
            std::cout << " ";
            std::ofstream output( filename.c_str() );
            SphereInputReport( output );
            SphereResults( output, myvector, myindices, mydistances, unknownCell, dont_sort_by_family);
            output.close();
            if (sauc_batch_mode)
                std::cout << "Output saved to: " << filename << std::endl << std::flush;
        }
    }
}

//*****************************************************************************/
void findRange( void )
{
    std::cout << std::endl;
    std::cout << "Unknown Cell\n" <<
    "A: "     << probeArray[0] << " " <<
    "B: "     << probeArray[1] << " " <<
    "C: "     << probeArray[2] << " " <<
    "Alpha: " << probeArray[3] << " " <<
    "Beta: "  << probeArray[4] << " " <<
    "Gamma: " << probeArray[5] << std::endl;
    std::cout << "\nRange Results\n";
    for (int i = 0; i < (int)(cellDArray.size()); i++)
    {
        if ((probeArray[0] + numRangeA    ) >= cellDArray[i][0] && (probeArray[0] - numRangeA    ) <= cellDArray[i][0] &&
            (probeArray[1] + numRangeB    ) >= cellDArray[i][1] && (probeArray[1] - numRangeB    ) <= cellDArray[i][1] &&
            (probeArray[2] + numRangeC    ) >= cellDArray[i][2] && (probeArray[2] - numRangeC    ) <= cellDArray[i][2] &&
            (probeArray[3] + numRangeAlpha) >= cellDArray[i][3] && (probeArray[3] - numRangeAlpha) <= cellDArray[i][3] &&
            (probeArray[4] + numRangeBeta ) >= cellDArray[i][4] && (probeArray[4] - numRangeBeta ) <= cellDArray[i][4] &&
            (probeArray[5] + numRangeGamma) >= cellDArray[i][5] && (probeArray[5] - numRangeGamma) <= cellDArray[i][5])
        {
            std::cout << "PDBID: " << idArray[i] << " " <<
            "A: "     << cellDArray[i][0] << " " <<
            "B: "     << cellDArray[i][1] << " " <<
            "C: "     << cellDArray[i][2] << " " <<
            "Alpha: " << cellDArray[i][3] << " " <<
            "Beta: "  << cellDArray[i][4] << " " <<
            "Gamma: " << cellDArray[i][5] << " " <<
            "Space Group: " << spaceArray[i];
            
            if (zArray[i] > 0) {
                std::cout << ", Z: " <<
                zArray[i] << std::endl;
            } else {
                std::cout << std::endl;
            }
        }
    }
}

/* EntryArray holds:
 
 entryArray[i][0] = id;   ID code
 entryArray[i][1] = head; header
 entryArray[i][2] = acc;  accession date
 entryArray[i][3] = cmpd; compound
 entryArray[i][4] = src;  source
 entryArray[i][5] = aut;  author list
 entryArray[i][6] = res;  resolution
 entryArray[i][7] = exp;  experiment type (if not X-ray).
 
 */



//*****************************************************************************/
int main ()
{
    
    std::cout << "sauc Copyright (C) Keith McGill 2013, 2014" << std::endl;
    std::cout << "This program comes with ABSOLUTELY NO WARRANTY" << std::endl;
    std::cout << "This is free software, and you are welcome to" << std::endl;
    std::cout << "redistribute it under the GPL or LGPL" << std::endl;
    std::cout << "See the program documentation for details" << std::endl;
    std::cout << "Rev 0.8, 24 Apr 2014, Mojgan Asadi, Herbert J. Bernstein" << std::endl;
    std::cout << "Rev 0.9, 18 Jul 2015, Herbert J. Bernstein" << std::endl;
    std::cout << "Rev 1.0, 4 Oct 2018, Herbert J. Bernstein" << std::endl;
    std::cout << "Rev 1.1, 11 Apr 2019, Herbert J. Bernstein" << std::endl;
    std::cout << "Rev 1.2, 15 Oct 2022, Herbert J. Bernstein" << std::endl;
    
    // Check for sauc html run
    
    if (std::getenv("SAUC_BATCH_MODE")) {
        sauc_batch_mode = 1;
    }
    
    if (std::getenv("SAUC_JAVASCRIPT")) {
        sauc_javascript = 1;
    }
    
    make_mmapfiles();
    load_cellDSGZArrays();
    unitcell cell;
    
    
    while (endProgram != 1)
    {
        if (goBack != 1)
        {
            int ii;
            double edgemax;
            for (ii=0; ii < 7; ii++) probeArray[ii] = 0;
            std::cout << "Search of Alternate Unit Cells\n";
            if (!sauc_batch_mode) std::cout << "\nPlease Input Your Data\n";
            if (!sauc_batch_mode) std::cout << "Lattice Centering (P, A, B, C, F, I, R, H, V, U): ";
            std::cin >> probelattice; std::cin.clear();
            if (probelattice.size() == 0 ||
                probelattice[0]=='V' || probelattice[0]=='v' ||
                probelattice[0]=='U' || probelattice[0]=='u') {
              if (!sauc_batch_mode) std::cout << "r: ";
              std::cin >> probeArray[0]; std::cin.clear();
              if (!sauc_batch_mode) std::cout << "s: ";
              std::cin >> probeArray[1]; std::cin.clear();
              if (!sauc_batch_mode) std::cout << "t:: ";
              std::cin >> probeArray[2]; std::cin.clear();
              if (probelattice[0]=='V' || probelattice[0]=='v' && !sauc_batch_mode) {
                std::cout << "u: ";
              } else if (!sauc_batch_mode) {
                std::cout << "s+t-|u|: ";
              }
              std::cin >> probeArray[3]; std::cin.clear();
              if (probelattice[0]=='V' || probelattice[0]=='v' && !sauc_batch_mode) {
                std::cout << "v: ";
              } else if (!sauc_batch_mode) {
                std::cout << "r+t-|v|: ";
              }
              std::cin >> probeArray[4]; std::cin.clear();
              if (probelattice[0]=='V' || probelattice[0]=='v' && !sauc_batch_mode) {
                std::cout << "w: ";
              } else if (!sauc_batch_mode) {
                std::cout << "r+s-|w|: ";
              }
              std::cin >> probeArray[5]; std::cin.clear();
              if (probelattice[0]=='U' || probelattice[0]=='u' ) {
                double absu, absv, absw, tau;
                if (!sauc_batch_mode) std::cout << "MDB^2: ";
                std::cin >> probeArray[6]; std::cin.clear();
                absu=probeArray[1]+probeArray[2]-probeArray[3];
                absv=probeArray[0]+probeArray[2]-probeArray[4];
                absw=probeArray[0]+probeArray[1]-probeArray[5];
                tau=probeArray[0]+probeArray[1]+probeArray[2]-absu-absv-absw;
                if (absu < -1.e-6 || absv < -1.e-6 || absw < -1.e-6 || tau > probeArray[6]+1.e-6 ) {
                  std::cout << "Invalid DC7 unsorted cell: " 
                    << probeArray[0] << " "
                    << probeArray[1] << " "
                    << probeArray[2] << " "
                    << probeArray[3] << " "
                    << probeArray[4] << " "
                    << probeArray[5] << " "
                    << probeArray[6] << std::endl;
                }
                if (tau < probeArray[6]-1.e-6) {
                  probeArray[3]=absu;
                  probeArray[4]=absv;
                  probeArray[5]=absw;
                } else {
                  probeArray[3]=-absu;
                  probeArray[4]=-absv;
                  probeArray[5]=-absw;
                }
                probeArray[6]=0.;
                probelattice[0]='v';
              }
            } else {
              if (!sauc_batch_mode) std::cout << "A: ";
              std::cin >> probeArray[0]; std::cin.clear();
              if (!sauc_batch_mode) std::cout << "B: ";
              std::cin >> probeArray[1]; std::cin.clear();
              if (!sauc_batch_mode) std::cout << "C: ";
              std::cin >> probeArray[2]; std::cin.clear();
              if (!sauc_batch_mode) std::cout << "Alpha: ";
              std::cin >> probeArray[3]; std::cin.clear();
              if (!sauc_batch_mode) std::cout << "Beta: ";
              std::cin >> probeArray[4]; std::cin.clear();
              if (!sauc_batch_mode) std::cout << "Gamma: ";
              std::cin >> probeArray[5]; std::cin.clear();
            }
            std::cout << std::endl;
            if (sauc_batch_mode) {
                std::cout << "Centering and Probe cell :"<<  probelattice << " "
                << probeArray[0] << " "
                << probeArray[1] << " "
                << probeArray[2] << " "
                << probeArray[3] << " "
                << probeArray[4] << " "
                << probeArray[5] << std::endl;
            }
            edgemax = 0;
            for (ii=0; ii < 6; ii++) {
                if (ii<3 && probeArray[ii] > edgemax)edgemax = probeArray[ii];
                if (probelattice.size() > 0
                    && (probelattice[0]=='V' || probelattice[0]=='v')) {
                    if ((ii<3 && probeArray[ii] <= 1.)
                        || (ii > 2
                            && std::abs(probeArray[ii]) > edgemax*1.01)) {
                            std::cout << "Invalid G6 cell "
                            << probeArray[0] << " "
                            << probeArray[1] << " "
                            << probeArray[2] << " "
                            << probeArray[3] << " "
                            << probeArray[4] << " "
                            << probeArray[5] << " "
                            << std::endl;
                            continue;
                        }
                } else if ((ii< 3 && probeArray[ii]<=1.)
                           ||( ii >2 && std::abs(probeArray[ii]-90.)> 75)) {
                    std::cout << "Invalid "<<probelattice<<" cell "
                    << probeArray << std::endl;
                    continue;
                }
            }
        }
        makeprimredprobe();
        
        while (quitAlgorithm != 1 && endProgram != 1)
        {
            goBack = 0;
            if (!sauc_batch_mode) std::cout << "1. L1";
            if (!sauc_batch_mode) std::cout << "\n2. L2";
            if (!sauc_batch_mode) std::cout << "\n3. NCDist";
            if (!sauc_batch_mode) std::cout << "\n4. V7";
            if (!sauc_batch_mode) std::cout << "\n5. D7Dist";
            if (!sauc_batch_mode) std::cout << "\n6. S6Dist";
            if (!sauc_batch_mode) std::cout << "\n7. DC7unsrt";
            if (!sauc_batch_mode) std::cout << "\n8. Quit";
            if (!sauc_batch_mode) std::cout << "\nChoose a Metric: ";
            priorAlgorithm = choiceAlgorithm;
            std::cin >> choiceAlgorithm; std::cin.clear();
            std::cin.ignore(100000,'\n');
            std::cout << choiceAlgorithm << std::endl;

            
            if (sauc_batch_mode && choiceAlgorithm > 0 && choiceAlgorithm < 8 ) {
                switch (choiceAlgorithm) {
                    case 1: std::cout << "L1 metric search algorithm" << std::endl; break;
                    case 2: std::cout << "L2 metric search algorithm" << std::endl; break;
                    case 3: std::cout << "NCDist metric search algorithm" << std::endl; break;
                    case 4: std::cout << "V7 metric search algorithm" << std::endl; break;
                    case 5: std::cout << "D7Dist metric search algorithm" << std::endl; break;
                    case 6: std::cout << "S6Dist metric search algorithm" << std::endl; break;
                    case 7: std::cout << "DC7unsrt metric search algorithm" << std::endl; break;
                }
            }
            if ( choiceAlgorithm > 0 && choiceAlgorithm < 8 ) {
                double d7cell[7],d7red[7],s6cell[6],s6red[6];
                int reduced;
                G6 redprimcell;
                switch (choiceAlgorithm) {
                    case 1:  /* L1 */
                    case 2:  /* L2 */
                    case 3:  /* NCDist */
                    case 4:  /* V7 */
                    case 7:  /* DC7unsrt */
                         CS6M_G6Reduce(primcell,redprimcell,reduced);
                         break; 
                    case 5:  /* D7Dist */
                         CS6M_G6toD7(primcell,d7cell);
                         CS6M_D7Reduce(d7cell,d7red,reduced);
                         CS6M_D7toG6(d7red,redprimcell);
                         break;
                    case 6:  /* S6Dist */
                    default: /* or anything else */
                         CS6M_G6toS6(primcell,s6cell);
                         CS6M_S6Reduce(s6cell,s6red,reduced);
                         CS6M_S6toG6(s6red,redprimcell);
                         break;
                }
                primredprobe = Cell(redprimcell).CellWithDegrees();
                std::cout << "Primitive Reduced Probe Cell: " <<
                    primredprobe[0]<<" "<<
                    primredprobe[1]<<" "<<
                    primredprobe[2]<<" "<<
                    primredprobe[3]<<" "<<
                    primredprobe[4]<<" "<<
                    primredprobe[5] << std::endl;
            }
            
            if (choiceAlgorithm == 1 && priorAlgorithm!= 1)
            {
                cell.changeOperator(1);
                cell.changeAlgorithm(1);
                cell.changeScaledist(1./std::sqrt(6.));
                quitAlgorithm = 1;
                //Build Tree
                if (!cellTree[choiceAlgorithm-1]) buildNearTree();
            }
            else if (choiceAlgorithm == 2 && priorAlgorithm!= 2)
            {
                cell.changeOperator(1);
                cell.changeAlgorithm(2);
                cell.changeScaledist(1.);
                quitAlgorithm = 1;
                //Build Tree
                if (!cellTree[choiceAlgorithm-1]) buildNearTree();
            }
            else if (choiceAlgorithm == 3 && priorAlgorithm!= 3)
            {
                cell.changeOperator(2);
                cell.changeAlgorithm(3);
                cell.changeScaledist(0.1);
                quitAlgorithm = 1;
                //Build Tree
                if (!cellTree[choiceAlgorithm-1]) buildNearTree();
            }
            else if (choiceAlgorithm == 4 && priorAlgorithm!= 4)
            {
                cell.changeOperator(2);
                cell.changeAlgorithm(4);
                cell.changeScaledist(std::sqrt(6./7.));
                quitAlgorithm = 1;
                //Build Tree
                if (!cellTree[choiceAlgorithm-1]) buildNearTree();
            }
            else if (choiceAlgorithm == 5 && priorAlgorithm!= 5)
            {
                cell.changeOperator(2);
                cell.changeAlgorithm(5);
                cell.changeScaledist(0.1);
                quitAlgorithm = 1;
                //Build Tree
                if (!cellTree[choiceAlgorithm-1]) buildNearTree();
            }
            else if (choiceAlgorithm == 6 && priorAlgorithm!= 6)
            {
                cell.changeOperator(2);
                cell.changeAlgorithm(6);
                cell.changeScaledist(0.1);
                quitAlgorithm = 1;
                //Build Tree
                if (!cellTree[choiceAlgorithm-1]) buildNearTree();
            }
            else if (choiceAlgorithm == 7 && priorAlgorithm!= 7)
            {
                cell.changeOperator(2);
                cell.changeAlgorithm(7);
                cell.changeScaledist(0.1);
                quitAlgorithm = 1;
                //Build Tree
                if (!cellTree[choiceAlgorithm-1]) buildNearTree();
            }
            else if (choiceAlgorithm == 8 && priorAlgorithm!= 8)
            {
                quitAlgorithm = 1;
                endProgram = 1;
                quitContinue = 1;
            }
            else if (choiceAlgorithm < 1 || choiceAlgorithm > 8)
            {
                std::cout << "Incorrect Choice. Please Choose Again.\n";
                choiceAlgorithm = priorAlgorithm;
            }
            else
            {
                quitAlgorithm = 1;
            }
            std::cout << std::endl;
        }
        quitAlgorithm = 0;
        
        while (quitSimilar != 1 && endProgram != 1)
        {
            if (!sauc_batch_mode) std::cout << "1. Nearest";
            if (!sauc_batch_mode) std::cout << "\n2. Sphere";
            if (!sauc_batch_mode) std::cout << "\n3. Range";
            if (!sauc_batch_mode) std::cout << "\n4. Back";
            if (!sauc_batch_mode) std::cout << "\n5. Quit";
            if (!sauc_batch_mode) std::cout << "\nChoose A Similarity: ";
            std::cin >> choiceSimilar;
            std::cin.clear(); std::cin.ignore(10000,'\n');
            std::cout << choiceSimilar << std::endl;

            if (sauc_batch_mode && choiceSimilar > 0 && choiceSimilar < 5) {
                switch(choiceSimilar) {
                    case 1:  std::cout << "Finding Nearest Cell" << std::endl; break;
                    case 2:  std::cout << "Searching in a Sphere" << std::endl; break;
                    case 3:  std::cout << "Brute Force Range Search" << std::endl; break;
                }
            }
            if (choiceSimilar == 1)
            {
                findNearest(1,0.0,0);
                quitSimilar = 1;
            }
            else if (choiceSimilar == 2)
            {
                string token;
                string sradline;
                int limit, klimit;
                int sortmode, dont_sort_by_family;
                int ll;
                klimit = 10;
                dont_sort_by_family = 0;
                if (!sauc_batch_mode) std::cout << "\nPlease Input Your Sphere's Radius, Optional Limit K, Sort Mode: ";
                std::getline(std::cin,sradline);
                std::stringstream ssradline(sradline);
                ssradline >> token;
                ll = token.length();
                if (ll > 1 && token[ll-1]=='%') {
                    sphereRange=convertToDouble(token.substr(0,ll-1))*crootvol/100.;
                    std::cout << "crootvol: " << crootvol << std::endl;
                    std::cout << "sphereRange: " << sphereRange << std::endl;
                } else {
                    sphereRange=convertToDouble(token);
                }
                if (ssradline >> limit) {
                    if (limit >= 1) klimit = limit;
                } else {
                    klimit = 100000;
                }
                if (ssradline >> sortmode) {
                    if (sortmode > 0) dont_sort_by_family=1;;
                } else {
                    dont_sort_by_family=0;
                }
             
                if (sauc_batch_mode) {
                    std::cout << "Sphere Radius: "<< sphereRange << " Limit: " << klimit << std::endl;
                    if (dont_sort_by_family == 1) std::cout << "Don't sort by family" << std::endl;
                }
                findNearest(klimit,sphereRange,dont_sort_by_family);
                quitSimilar = 1;
            }
            else if (choiceSimilar == 3)
            {
                if (!sauc_batch_mode) std::cout << "\nPlease Input Your Ranges\n";
                if (!sauc_batch_mode) std::cout << "A: ";
                std::cin >> numRangeA; std::cin.clear();
                if (!sauc_batch_mode) std::cout << "B: ";
                std::cin >> numRangeB; std::cin.clear();
                if (!sauc_batch_mode) std::cout << "C: ";
                std::cin >> numRangeC; std::cin.clear();
                if (!sauc_batch_mode) std::cout << "Alpha: ";
                std::cin >> numRangeAlpha; std::cin.clear();
                if (!sauc_batch_mode) std::cout << "Beta: ";
                std::cin >> numRangeBeta; std::cin.clear();
                if (!sauc_batch_mode) std::cout << "Gamma: ";
                std::cin >> numRangeGamma; std::cin.clear();
                std::cin.clear();
                std::cin.ignore(100000,'\n');
                if (sauc_batch_mode) {
                    std::cout << "A, B, C, Alpha, Beta, Gamma ranges: "
                    << numRangeA << ", "
                    << numRangeB << ", "
                    << numRangeC << ", "
                    << numRangeAlpha << ", "
                    << numRangeBeta << ", "
                    << numRangeGamma << std::endl;
                }
                findRange();
                quitSimilar = 1;
            }
            else if (choiceSimilar == 4)
            {
                quitSimilar = 1;
                goBack = 1;
            }
            else if (choiceSimilar == 5)
            {
                quitSimilar = 1;
                endProgram = 1;
                quitContinue = 1;
            }
            else
            {
                if (sauc_batch_mode) {
                    quitSimilar = 1;
                    endProgram = 1;
                    quitContinue = 1;
                } else {
                    std::cout << "Incorrect Choice. Please Choose Again.\n";
                }
            }
            std::cout << std::endl;
        }
        quitSimilar = 0;
        
        while (quitContinue == 0)
        {
            if (!sauc_batch_mode) std::cout << "1. Input New Values";
            if (!sauc_batch_mode) std::cout << "\n2. Choose a New Metric";
            if (!sauc_batch_mode) std::cout << "\n3. Choose a New Similarity";
            if (!sauc_batch_mode) std::cout << "\n4. Quit";
            if (!sauc_batch_mode) std::cout << "\nChoice: ";
            std::cin >> choiceContinue; std::cin.clear();
            std::cin.ignore(100000,'\n');
            std::cout << choiceContinue << std::endl;
            if (choiceContinue == 1)
            {
                goBack = 0; //Added so that it would loop back to main menu
                quitContinue = 1;
            }
            else if (choiceContinue == 2)
            {
                goBack = 1;
                quitContinue = 1;
            }
            else if (choiceContinue == 3)
            {
                goBack = 1;
                quitAlgorithm = 1;
                quitContinue = 1;
            }
            else if (choiceContinue == 4)
            {
                endProgram = 1;
                quitContinue = 1;
            }
            else
            {
                if (sauc_batch_mode) {
                    endProgram = 1;
                    quitContinue = 1;
                } else {
                    std::cout << "Incorrect Choice. Please Choose Again.\n";
                }
            }
            std::cout << std::endl;
        }
        quitContinue = 0;
    }
    
    return 0;
}

