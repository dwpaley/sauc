#!/bin/sh
# config_viper_lbl.sh - SAUC production config for viper.lbl.gov
# Local version: build is running on viper.lbl.gov
# Source this before running deploy.sh or deploy_db.sh

HTTPDSERVER=sauc.lbl.gov
export HTTPDSERVER

HTDOCS=/var/www/sauc
export HTDOCS

SEARCHURL=https://sauc.lbl.gov/
export SEARCHURL

CGIPATH=https://sauc.lbl.gov
export CGIPATH

CGIBIN=/var/www/sauc
export CGIBIN

BINPATH=/var/www/sauc/sauc-1.2.1.exe
export BINPATH

MATHSCRIBEURL=https://sauc.lbl.gov/mathscribe-0.4.6
export MATHSCRIBEURL

CGIMETHOD=GET
export CGIMETHOD

PYTHON=/var/www/cctbx_xfel/conda/envs/httpd/bin/python3
export PYTHON

# Compiler flags - same as other configs
CXXFLAGS="-Wall -O3 -DUSE_LOCAL_HEADERS -g -fopenmp -ftree-parallelize-loops=8"
export CXXFLAGS
CFLAGS="-Wall -O3 -DUSE_LOCAL_HEADERS -g -fopenmp -ftree-parallelize-loops=8"
export CFLAGS

REMOTE_HOST=""
export REMOTE_HOST
REMOTE_PATH_PREFIX=""
export REMOTE_PATH_PREFIX
