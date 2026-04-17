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

if [ -z "$HTDOCS" ]; then
    echo "ERROR: HTDOCS not set in $CONFIG_FILE"
    exit 1
fi

echo "Deploying database files to $HTDOCS"

# Create destination
mkdir -p "$HTDOCS"

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
        cp -v "$f" "$HTDOCS/"
    done
done

# Copy mathscribe
if [ -d "$SCRIPT_DIR/mathscribe-0.4.6" ]; then
    echo "Copying mathscribe..."
    cp -r "$SCRIPT_DIR/mathscribe-0.4.6" "$HTDOCS/"
fi

# Copy last_update if present
[ -f "$SCRIPT_DIR/last_update" ] && cp "$SCRIPT_DIR/last_update" "$HTDOCS/"

# Set permissions
chmod 644 "$HTDOCS"/*.dmp "$HTDOCS"/*.tsv "$HTDOCS"/*.idx 2>/dev/null || true
chmod -R 755 "$HTDOCS/mathscribe-0.4.6" 2>/dev/null || true

# Try to set ownership (root:apache so Apache can read but not write)
chown -R root:apache "$HTDOCS" 2>/dev/null || echo "Warning: Could not chown (need sudo?)"

echo ""
echo "=== Database Deployment Complete ==="
echo "Files deployed to $HTDOCS"
ls -lh "$HTDOCS"/*.dmp 2>/dev/null | head -5
echo "..."
