#!/bin/sh
HTTPDSERVER=www.bernstein-plus-sons.com;export HTTPDSERVER
HTDOCS=${HOME}/public_html/software/sauc;export HTDOCS
SEARCHURL=http://www.bernstein-plus-sons.com/software/sauc;export SEARCHURL
CGIPATH=http://www.bernstein-plus-sons.com/cgi-bin;export CGIPATH
PDBCELLINDEXURL=http://ftp.wwpdb.org/pub/pdb/derived_data/index/crystal.idx;export PDBCELLINDEXURL;
PDBENTRIESURL=http://ftp.wwpdb.org/pub/pdb/derived_data/index/entries.idx;export PDBENTRIESURL;
#PDBCELLINDEXURL=http://www.bmrb.wisc.edu/ftp/pub/pdb/derived_data/index/crystal.idx;export PDBCELLINDEXURL;
#PDBENTRIESURL=http://www.bmrb.wisc.edu/ftp/pub/pdb/derived_data/index/entries.idx;export PDBENTRIESURL;

REMOTE_HOST=""
export REMOTE_HOST
REMOTE_PATH_PREFIX=""
export REMOTE_PATH_PREFIX
