#!/bin/sh
# post_deploy_sudo.sh - Commands requiring root, to be run on the TARGET server after deploy
#
# Run this script on viper.lbl.gov (or whichever server hosts SAUC):
#   sudo sh post_deploy_sudo.sh
#
# These commands were extracted from deploy.sh because they require root/sudo.
# They set directory ownership so Apache can read but not write to the web root.
# Run this once after initial setup, or if ownership is reset. Re-running is safe
# (chown/chmod are idempotent).

HTDOCS=/var/www/sauc
CGIBIN=/var/www/sauc    # Same as HTDOCS for viper config; edit if different

chown root:apache "$HTDOCS"
chown -R root:apache "$HTDOCS"
chmod 750 "$HTDOCS"

if [ "$CGIBIN" != "$HTDOCS" ]; then
    chown -R root:apache "$CGIBIN"
    chmod 750 "$CGIBIN"
fi

echo "Done. Ownership set to root:apache."
