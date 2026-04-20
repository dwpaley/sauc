#!/bin/sh
HTTPDSERVER=http://flops.arcib.org:8084;export HTTPDSERVER
HTDOCS=/var/www/html/sauc-1.2.1;export HTDOCS;
SEARCHURL=http://flops.arcib.org:8084/sauc-1.2.1/;export SEARCHURL;
CGIPATH=http://flops.arcib.org:8084/cgi-bin;export CGIPATH;
CGIBIN=/var/www/cgi-bin;export CGIBIN;
CGIMETHOD=GET;export CGIMETHOD;
BINDEST=/var/www/cgi-bin;export BINDEST;
PDBCELLINDEXURL=http://ftp.wwpdb.org/pub/pdb/derived_data/index/crystal.idx;export PDBCELLINDEXURL;
PDBENTRIESURL=http://ftp.wwpdb.org/pub/pdb/derived_data/index/entries.idx;export PDBENTRIESURL;
#PDBCELLINDEXURL=http://www.bmrb.wisc.edu/ftp/pub/pdb/derived_data/index/crystal.idx;export PDBCELLINDEXURL;
#PDBENTRIESURL=http://www.bmrb.wisc.edu/ftp/pub/pdb/derived_data/index/entries.idx;export PDBENTRIESURL;
export CC=gcc; export CXX=g++; export FC=gfortran
export CXXFLAGS="-Wall -O3 -DUSE_LOCAL_HEADERS -g -fopenmp  -ftree-parallelize-loops=8 -I.";

REMOTE_HOST=""
export REMOTE_HOST
REMOTE_PATH_PREFIX=""
export REMOTE_PATH_PREFIX
