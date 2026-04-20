#!/bin/sh
# config_viper_lbl.sh - SAUC production config for viper.lbl.gov
# Remote build: build on a separate server and transfer to viper
# Source this before running deploy.sh or deploy_db.sh

HTTPDSERVER=viper.lbl.gov:8083
export HTTPDSERVER

HTDOCS=/var/www/sauc
export HTDOCS

SEARCHURL=http://viper.lbl.gov:8083/
export SEARCHURL

CGIPATH=http://viper.lbl.gov:8083
export CGIPATH

CGIBIN=/var/www/sauc
export CGIBIN

BINPATH=/var/www/sauc/sauc-1.2.1.exe
export BINPATH

MATHSCRIBEURL=http://viper.lbl.gov:8083/mathscribe-0.4.6
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

REMOTE_HOST="viper.lbl.gov"
export REMOTE_HOST
REMOTE_PATH_PREFIX="viper.lbl.gov:"
export REMOTE_PATH_PREFIX
