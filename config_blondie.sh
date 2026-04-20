#!/bin/sh
HTTPDSERVER=http://blondie.arcib.org:8083;export HTTPDSERVER
HTDOCS=/var/www/sauc-1.0.2;export HTDOCS;
SEARCHURL=http://blondie.arcib.org:8083/sauc-1.0.2/;export SEARCHURL;
CGIPATH=http://blondie.arcib.org:8083/cgi-bin;export CGIPATH;
CGIBIN=/var/www/cgi-bin;export CGIBIN;
CGIMETHOD=GET;export CGIMETHOD;
BINDEST=/var/www/cgi-bin;export BINDEST;
PDBCELLINDEXURL=http://ftp.wwpdb.org/pub/pdb/derived_data/index/crystal.idx;export PDBCELLINDEXURL;
PDBENTRIESURL=http://ftp.wwpdb.org/pub/pdb/derived_data/index/entries.idx;export PDBENTRIESURL;
export CXXFLAGS="-Wall -O3 -DUSE_LOCAL_HEADERS -g -fopenmp  -ftree-parallelize-loops=8 -mfpmath=sse -msse2 -I.";

REMOTE_HOST=""
export REMOTE_HOST
REMOTE_PATH_PREFIX=""
export REMOTE_PATH_PREFIX
