#!/bin/sh
HTTPDSERVER=http://r21-001:8125;export HTTPDSERVER
HTDOCS=/home/hbernstein/distl_dials_apache_server/apache/httpd/htdocs/sauc-1.1.1;export HTDOCS;
SEARCHURL=http://r21-001:8125/sauc-1.1.1/;export SEARCHURL;
CGIPATH=http://r21-001:8125/cgi-bin;export CGIPATH;
CGIBIN=/home/hbernstein/distl_dials_apache_server/apache/httpd/cgi-bin/;export CGIBIN;
CGIMETHOD=GET;export CGIMETHOD;
BINDEST=/home/hbernstein/distl_dials_apache_server/apache/httpd/cgi-bin/;export BINDEST;
PDBCELLINDEXURL=http://ftp.wwpdb.org/pub/pdb/derived_data/index/crystal.idx;export PDBCELLINDEXURL;
PDBENTRIESURL=http://ftp.wwpdb.org/pub/pdb/derived_data/index/entries.idx;export PDBENTRIESURL;
#PDBCELLINDEXURL=http://www.bmrb.wisc.edu/ftp/pub/pdb/derived_data/index/crystal.idx;export PDBCELLINDEXURL;
#PDBENTRIESURL=http://www.bmrb.wisc.edu/ftp/pub/pdb/derived_data/index/entries.idx;export PDBENTRIESURL;
export CXXFLAGS="-Wall -O3 -DUSE_LOCAL_HEADERS -g -fopenmp  -ftree-parallelize-loops=8 -mfpmath=sse -msse2 -I.";

REMOTE_HOST=""
export REMOTE_HOST
REMOTE_PATH_PREFIX=""
export REMOTE_PATH_PREFIX
