#!/bin/sh
HTTPDSERVER=`hostname -f`/~${USER};export HTTPDSERVER
SEARCHURL=http://`hostname -f`/~${USER}/sauc;export SEARCHURL
CGIPATH=http://`hostname -f`/cgi-bin/cgiwrap/${USER};export CGIPATH
PDBCELLINDEXURL=http://ftp.wwpdb.org/pub/pdb/derived_data/index/crystal.idx;export PDBCELLINDEXURL;
PDBENTRIESURL=http://ftp.wwpdb.org/pub/pdb/derived_data/index/entries.idx;export PDBENTRIESURL;
#PDBCELLINDEXURL=http://www.bmrb.wisc.edu/ftp/pub/pdb/derived_data/index/crystal.idx;export PDBCELLINDEXURL;
#PDBENTRIESURL=http://www.bmrb.wisc.edu/ftp/pub/pdb/derived_data/index/entries.idx;export PDBENTRIESURL;
CGIMETHOD=GET;export CGIMETHOD;

REMOTE_HOST=""
export REMOTE_HOST
REMOTE_PATH_PREFIX=""
export REMOTE_PATH_PREFIX
