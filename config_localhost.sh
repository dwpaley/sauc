#!/bin/sh
# config_localhost.sh - SAUC local development config
# Source this before running deploy.sh or deploy_db.sh

# Compute paths relative to this script's location
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
REPO_ROOT="$(dirname "$SCRIPT_DIR")"

HTTPDSERVER=localhost:8000
export HTTPDSERVER

HTDOCS="${REPO_ROOT}/sauc-deploy/htdocs"
export HTDOCS

SEARCHURL=http://localhost:8000/
export SEARCHURL

CGIPATH=http://localhost:8000/cgi-bin
export CGIPATH

CGIBIN="${REPO_ROOT}/sauc-deploy/cgi-bin"
export CGIBIN

BINPATH="${HTDOCS}/sauc-1.2.1.exe"
export BINPATH

MATHSCRIBEURL=http://localhost:8000/mathscribe-0.4.6
export MATHSCRIBEURL

CGIMETHOD=GET
export CGIMETHOD

# Use conda python if available, else system python3
if [ -x "${REPO_ROOT}/mc3/bin/python3" ]; then
    PYTHON="${REPO_ROOT}/mc3/bin/python3"
else
    PYTHON=python3
fi
export PYTHON

CXXFLAGS="-Wall -O3 -DUSE_LOCAL_HEADERS -g -fopenmp -ftree-parallelize-loops=8"
export CXXFLAGS
CFLAGS="-Wall -O3 -DUSE_LOCAL_HEADERS -g -fopenmp -ftree-parallelize-loops=8"
export CFLAGS
