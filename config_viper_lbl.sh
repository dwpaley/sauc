#!/bin/sh
# config_viper_lbl.sh - SAUC production config for viper.lbl.gov
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

PYTHON=python3
export PYTHON

# Compiler flags - same as other configs
CXXFLAGS="-Wall -O3 -DUSE_LOCAL_HEADERS -g -fopenmp -ftree-parallelize-loops=8"
export CXXFLAGS
CFLAGS="-Wall -O3 -DUSE_LOCAL_HEADERS -g -fopenmp -ftree-parallelize-loops=8"
export CFLAGS
