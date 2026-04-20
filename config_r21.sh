#!/bin/sh
HTTPDSERVER=http://r21-001.nsls2.bnl.gov;export HTTPDSERVER
HTDOCS=/home/hbernstein/public_html/sauc-1.1.1;export HTDOCS;
SEARCHURL=http://r21-001.nsls2.bnl.gov/~hbernstein/sauc-1.1.1/;export SEARCHURL;
CGIPATH=http://r21-001.nsls2.bnl.gov/~hbernstein/cgi-bin;export CGIPATH;
CGIBIN=/home/hbernstein/pubic_html/cgi-bin;export CGIBIN;
CGIMETHOD=GET;export CGIMETHOD;
BINDEST=/home/hbernstein/public_html/cgi-bin;export BINDEST;
PDBCELLINDEXURL=http://ftp.wwpdb.org/pub/pdb/derived_data/index/crystal.idx;export PDBCELLINDEXURL;
PDBENTRIESURL=http://ftp.wwpdb.org/pub/pdb/derived_data/index/entries.idx;export PDBENTRIESURL;
#PDBCELLINDEXURL=http://www.bmrb.wisc.edu/ftp/pub/pdb/derived_data/index/crystal.idx;export PDBCELLINDEXURL;
#PDBENTRIESURL=http://www.bmrb.wisc.edu/ftp/pub/pdb/derived_data/index/entries.idx;export PDBENTRIESURL;
export CXXFLAGS="-Wall -O3 -DUSE_LOCAL_HEADERS -g -fopenmp  -ftree-parallelize-loops=8 -mfpmath=sse -msse2 -I.";

REMOTE_HOST=""
export REMOTE_HOST
REMOTE_PATH_PREFIX=""
export REMOTE_PATH_PREFIX
