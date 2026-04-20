#!/bin/sh
# deploy.sh - Deploy SAUC code (exe, CGI, HTML) to target environment
# Usage: ./deploy.sh config_viper_lbl.sh
#        ./deploy.sh config_localhost.sh
#
# NOTE: This does NOT deploy database files. Run deploy_db.sh for that.
# NOTE: Do NOT run 'make updatedb' unless you have a week of compute time.

set -e

# --- Argument parsing ---
if [ -z "$1" ]; then
    echo "Usage: $0 <config_file>"
    echo "Example: $0 config_viper_lbl.sh"
    exit 1
fi

CONFIG_FILE="$1"
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

if [ ! -f "$SCRIPT_DIR/$CONFIG_FILE" ]; then
    echo "ERROR: Config file not found: $SCRIPT_DIR/$CONFIG_FILE"
    exit 1
fi

# --- Source config ---
. "$SCRIPT_DIR/$CONFIG_FILE"

# --- Verify required variables ---
[ -z "$HTTPDSERVER" ] && echo "ERROR: HTTPDSERVER not set in $CONFIG_FILE" && exit 1
[ -z "$HTDOCS" ] && echo "ERROR: HTDOCS not set in $CONFIG_FILE" && exit 1
[ -z "$BINPATH" ] && echo "ERROR: BINPATH not set in $CONFIG_FILE" && exit 1
[ -z "$SEARCHURL" ] && echo "ERROR: SEARCHURL not set in $CONFIG_FILE" && exit 1
[ -z "$CGIPATH" ] && echo "ERROR: CGIPATH not set in $CONFIG_FILE" && exit 1
[ -z "$CGIBIN" ] && echo "ERROR: CGIBIN not set in $CONFIG_FILE" && exit 1
[ -z "$MATHSCRIBEURL" ] && echo "ERROR: MATHSCRIBEURL not set in $CONFIG_FILE" && exit 1
[ -z "$PYTHON" ] && echo "ERROR: PYTHON not set in $CONFIG_FILE" && exit 1
[ -z "$CXXFLAGS" ] && echo "ERROR: CXXFLAGS not set in $CONFIG_FILE" && exit 1
[ -z "$CFLAGS" ] && echo "ERROR: CFLAGS not set in $CONFIG_FILE" && exit 1

# Default remote vars to empty if not set by config (local deploy)
REMOTE_HOST="${REMOTE_HOST:-}"
REMOTE_PATH_PREFIX="${REMOTE_PATH_PREFIX:-}"

# Helper: run a command locally or on the remote host
remote_sh() {
    if [ -n "$REMOTE_HOST" ]; then
        ssh "$REMOTE_HOST" "$@"
    else
        "$@"
    fi
}

echo "Deploying SAUC to $HTTPDSERVER"
echo "  HTDOCS: $HTDOCS"
echo "  CGIBIN: $CGIBIN"

# --- Compile if needed ---
NEED_COMPILE=0
if [ ! -f "$SCRIPT_DIR/sauc-1.2.1.exe" ]; then
    NEED_COMPILE=1
else
    for src in "$SCRIPT_DIR"/*.cpp "$SCRIPT_DIR"/*.c "$SCRIPT_DIR"/*.h; do
        if [ "$src" -nt "$SCRIPT_DIR/sauc-1.2.1.exe" ]; then
            NEED_COMPILE=1
            break
        fi
    done
fi

if [ "$NEED_COMPILE" = "1" ]; then
    echo "Compiling sauc-1.2.1.exe..."
    cd "$SCRIPT_DIR"
    gcc $CFLAGS -c NCDist.c -o NCDist.o
    gcc $CFLAGS -c CS6Dist.c -o CS6Dist.o
    gcc $CFLAGS -c D7Dist.c -o D7Dist.o
    gcc $CFLAGS -c pststrmgr.c -o pststrmgr.o
    g++ $CXXFLAGS -c unitcell.cpp -o unitcell.o
    g++ $CXXFLAGS -o sauc-1.2.1.exe \
        BasicDistance.cpp Cell.cpp D6.cpp D7.cpp DeloneTetrahedron.cpp \
        fgetln.c G6.cpp inverse.cpp Mat66.cpp MatMN.cpp MatN.cpp \
        sauc.cpp Reducer.cpp V7.cpp VecN.cpp VectorTools.cpp \
        Vec_N_Tools.cpp vector_3d.cpp \
        unitcell.o NCDist.o CS6Dist.o D7Dist.o pststrmgr.o -lpthread
    echo "Compilation complete."
else
    echo "Binary is up to date, skipping compilation."
fi

# --- Generate sauc_cgi.py ---
echo "Generating sauc_cgi.py..."
SHEBANG="#!${PYTHON}"
sed -e "1s|^#!.*|${SHEBANG}|" \
    -e "s|__BINPATH__|${BINPATH}|g" \
    -e "s|__HTDOCS__|${HTDOCS}|g" \
    -e "s|__SEARCHURL__|${SEARCHURL}|g" \
    "$SCRIPT_DIR/sauc_cgi.py" > "$SCRIPT_DIR/sauc_cgi.py.configured"

# Verify substitution worked
if grep -q "__BINPATH__\|__HTDOCS__\|__SEARCHURL__" "$SCRIPT_DIR/sauc_cgi.py.configured"; then
    echo "ERROR: sed substitution failed - placeholders remain"
    rm -f "$SCRIPT_DIR/sauc_cgi.py.configured"
    exit 1
fi

# --- Generate sauc-1.2.1.html ---
echo "Generating sauc-1.2.1.html..."
m4 \
    -DCGIBIN="$CGIPATH" \
    -DHTTPDSERVER="$HTTPDSERVER" \
    -DMATHSCRIBEURL="$MATHSCRIBEURL" \
    -DSAUCTARBALLURL=http://downloads.sf.net/iterate/sauc-1.2.1.tar.gz \
    -DSAUCZIPURL=http://downloads.sf.net/iterate/sauc-1.2.1.zip \
    -DHTDOCS="$HTDOCS" \
    -DCGIMETHOD=GET \
    -DSAUCHTML=sauc-1.2.1.html \
    -DSAUCCGI=sauc_cgi.py \
    -DSAUCEXE=sauc-1.2.1.exe \
    < "$SCRIPT_DIR/sauc.html.m4" > "$SCRIPT_DIR/sauc-1.2.1.html.configured"

# --- Create destination directories ---
remote_sh mkdir -p "$HTDOCS"
remote_sh mkdir -p "$CGIBIN"

# --- Set permissions on local source files before copy ---
chmod 755 "$SCRIPT_DIR/sauc-1.2.1.exe"
chmod 755 "$SCRIPT_DIR/sauc_cgi.py.configured"
chmod 644 "$SCRIPT_DIR/sauc-1.2.1.html.configured"
chmod 644 "$SCRIPT_DIR/gpl.txt"
chmod 644 "$SCRIPT_DIR/lgpl.txt"

# --- Install files (scp -p or cp -p to preserve mode bits) ---
echo "Installing files..."
if [ -n "$REMOTE_HOST" ]; then
    scp -p "$SCRIPT_DIR/sauc-1.2.1.exe"             "${REMOTE_PATH_PREFIX}${HTDOCS}/"
    scp -p "$SCRIPT_DIR/sauc_cgi.py.configured"     "${REMOTE_PATH_PREFIX}${CGIBIN}/sauc_cgi.py"
    scp -p "$SCRIPT_DIR/sauc-1.2.1.html.configured" "${REMOTE_PATH_PREFIX}${HTDOCS}/sauc-1.2.1.html"
    scp -p "$SCRIPT_DIR/gpl.txt"                    "${REMOTE_PATH_PREFIX}${HTDOCS}/"
    scp -p "$SCRIPT_DIR/lgpl.txt"                   "${REMOTE_PATH_PREFIX}${HTDOCS}/"
    if [ "$CGIBIN" != "$HTDOCS" ]; then
        scp -p "$SCRIPT_DIR/sauc-1.2.1.exe"         "${REMOTE_PATH_PREFIX}${CGIBIN}/"
    fi
else
    cp -p "$SCRIPT_DIR/sauc-1.2.1.exe"               "$HTDOCS/"
    cp -p "$SCRIPT_DIR/sauc_cgi.py.configured"        "$CGIBIN/sauc_cgi.py"
    cp -p "$SCRIPT_DIR/sauc-1.2.1.html.configured"    "$HTDOCS/sauc-1.2.1.html"
    cp -p "$SCRIPT_DIR/gpl.txt"                       "$HTDOCS/"
    cp -p "$SCRIPT_DIR/lgpl.txt"                      "$HTDOCS/"
    if [ "$CGIBIN" != "$HTDOCS" ]; then
        cp -p "$SCRIPT_DIR/sauc-1.2.1.exe"            "$CGIBIN/"
    fi
fi

echo "Note: chown root:apache must be run manually on the target server (requires sudo)."
echo "      See post_deploy_sudo.sh for the commands to run."

# --- Create index.html symlink ---
remote_sh ln -sf sauc-1.2.1.html "$HTDOCS/index.html"

# --- Write version stamp ---
GIT_COMMIT=""
if [ -d "$SCRIPT_DIR/.git" ]; then
    GIT_COMMIT=$(git -C "$SCRIPT_DIR" rev-parse HEAD 2>/dev/null || echo "unknown")
fi

VERSION_CONTENT="version: 1.2.1
deployed: $(date -Iseconds)
host: $(hostname)
git_commit: ${GIT_COMMIT:-none}
deployer: ${USER:-unknown}
config: $CONFIG_FILE"

if [ -n "$REMOTE_HOST" ]; then
    printf '%s\n' "$VERSION_CONTENT" | ssh "$REMOTE_HOST" "cat > ${HTDOCS}/DEPLOYED_VERSION"
else
    printf '%s\n' "$VERSION_CONTENT" > "$HTDOCS/DEPLOYED_VERSION"
fi

# --- Cleanup temp files ---
rm -f "$SCRIPT_DIR/sauc_cgi.py.configured"
rm -f "$SCRIPT_DIR/sauc-1.2.1.html.configured"

# --- Summary ---
echo ""
echo "=== Deployment Complete ==="
echo "SAUC 1.2.1 deployed to $HTDOCS"
echo "Test URL: ${SEARCHURL}sauc_cgi.py?Centering=P&A=78&B=78&C=38&Alpha=90&Beta=90&Gamma=90&Algorithm=7&Similarity=2&NumHits=5&RangeSphere=5%25"
echo ""
echo "NOTE: Database files were NOT deployed. Run deploy_db.sh if needed."
