#!/bin/sh
# deploy_db.sh - Deploy SAUC database files to target environment
# Usage: ./deploy_db.sh config_viper_lbl.sh
#
# Run this only when database files have been updated.
# Normal code deploys use deploy.sh only.
#
# WARNING: Do NOT run 'make updatedb' - it takes approximately one week.

set -e

if [ -z "$1" ]; then
    echo "Usage: $0 <config_file>"
    exit 1
fi

CONFIG_FILE="$1"
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

if [ ! -f "$SCRIPT_DIR/$CONFIG_FILE" ]; then
    echo "ERROR: Config file not found: $SCRIPT_DIR/$CONFIG_FILE"
    exit 1
fi

. "$SCRIPT_DIR/$CONFIG_FILE"

REMOTE_HOST="${REMOTE_HOST:-}"
REMOTE_PATH_PREFIX="${REMOTE_PATH_PREFIX:-}"

remote_sh() {
    if [ -n "$REMOTE_HOST" ]; then
        ssh "$REMOTE_HOST" "$@"
    else
        "$@"
    fi
}

if [ -z "$HTDOCS" ]; then
    echo "ERROR: HTDOCS not set in $CONFIG_FILE"
    exit 1
fi

echo "Deploying database files to $HTDOCS"

# Create destination
remote_sh mkdir -p "$HTDOCS"

# Decompress any .bz2 files that need it
echo "Checking for compressed files to decompress..."
for bz2 in "$SCRIPT_DIR"/*.bz2; do
    [ -f "$bz2" ] || continue
    base=$(basename "$bz2" .bz2)
    if [ ! -f "$SCRIPT_DIR/$base" ] || [ "$bz2" -nt "$SCRIPT_DIR/$base" ]; then
        echo "  Decompressing $base..."
        bunzip2 -k -f "$bz2"
    fi
done

# Copy database files
echo "Copying database files..."
for ext in dmp tsv idx; do
    for f in "$SCRIPT_DIR"/*.$ext; do
        [ -f "$f" ] || continue
        chmod 644 "$f"
        if [ -n "$REMOTE_HOST" ]; then
            scp -p "$f" "${REMOTE_PATH_PREFIX}${HTDOCS}/"
        else
            cp -pv "$f" "$HTDOCS/"
        fi
    done
done

# Copy mathscribe
if [ -d "$SCRIPT_DIR/mathscribe-0.4.6" ]; then
    echo "Copying mathscribe..."
    chmod -R 755 "$SCRIPT_DIR/mathscribe-0.4.6"
    if [ -n "$REMOTE_HOST" ]; then
        scp -rp "$SCRIPT_DIR/mathscribe-0.4.6" "${REMOTE_PATH_PREFIX}${HTDOCS}/"
    else
        cp -rp "$SCRIPT_DIR/mathscribe-0.4.6" "$HTDOCS/"
    fi
fi

# Copy last_update if present
if [ -f "$SCRIPT_DIR/last_update" ]; then
    chmod 644 "$SCRIPT_DIR/last_update"
    if [ -n "$REMOTE_HOST" ]; then
        scp -p "$SCRIPT_DIR/last_update" "${REMOTE_PATH_PREFIX}${HTDOCS}/"
    else
        cp -p "$SCRIPT_DIR/last_update" "$HTDOCS/"
    fi
fi

echo "Note: chown root:apache must be run manually on the target server (requires sudo)."
echo "      See post_deploy_sudo.sh for the commands to run."

echo ""
echo "=== Database Deployment Complete ==="
echo "Files deployed to $HTDOCS"
remote_sh sh -c "ls -lh ${HTDOCS}/*.dmp 2>/dev/null | head -5"
echo "..."
