#!/usr/bin/env python3
"""
Local CGI-capable HTTP server for SAUC testing.

Reads HTDOCS, CGIBIN, and HTTPDSERVER from a config file (default
config_localhost.sh) and serves static files from HTDOCS with CGI
execution for /cgi-bin/* routed to CGIBIN.

Usage:
    ./serve.py                       # uses config_localhost.sh
    ./serve.py config_localhost.sh
"""
import os
import subprocess
import sys
from http.server import CGIHTTPRequestHandler, HTTPServer


def load_config(config_path):
    config_abs = os.path.abspath(config_path)
    config_dir = os.path.dirname(config_abs)
    config_name = os.path.basename(config_abs)
    # cd to config dir so the config's $(dirname "$0") logic resolves correctly
    script = f'cd "{config_dir}" && . "./{config_name}" && echo "$HTDOCS" && echo "$CGIBIN" && echo "$HTTPDSERVER"'
    result = subprocess.run(
        ["sh", "-c", script],
        capture_output=True, text=True, check=True,
    )
    htdocs, cgibin, server = result.stdout.strip().split("\n")
    return htdocs, cgibin, server


def main():
    script_dir = os.path.dirname(os.path.abspath(__file__))
    config = sys.argv[1] if len(sys.argv) > 1 else "config_localhost.sh"
    if not os.path.isabs(config):
        config = os.path.join(script_dir, config)
    if not os.path.isfile(config):
        sys.exit(f"Config file not found: {config}")

    htdocs, cgibin, server = load_config(config)
    host, port = server.rsplit(":", 1)
    port = int(port)

    if not os.path.isdir(htdocs):
        sys.exit(f"HTDOCS does not exist: {htdocs}\nRun: ./deploy.sh {os.path.basename(config)}")
    if not os.path.isdir(cgibin):
        sys.exit(f"CGIBIN does not exist: {cgibin}\nRun: ./deploy.sh {os.path.basename(config)}")

    class Handler(CGIHTTPRequestHandler):
        cgi_directories = ["/cgi-bin"]

        def translate_path(self, path):
            clean = path.split("?", 1)[0].split("#", 1)[0]
            if clean.startswith("/cgi-bin/"):
                return os.path.join(cgibin, clean[len("/cgi-bin/"):])
            if clean == "/":
                clean = "/sauc-1.2.1.html"
            return os.path.join(htdocs, clean.lstrip("/"))

    # CGIHTTPRequestHandler requires /cgi-bin/* to resolve under cwd,
    # so chdir to the common parent of HTDOCS and CGIBIN.
    # (The CGI script itself passes cwd=HTDOCS to the exe, so this doesn't affect the binary.)
    os.chdir(os.path.commonpath([htdocs, cgibin]))

    httpd = HTTPServer((host, port), Handler)
    print(f"SAUC server running at http://{host}:{port}/")
    print(f"  HTDOCS: {htdocs}")
    print(f"  CGIBIN: {cgibin}")
    print("Press Ctrl+C to stop.")
    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        print("\nShutting down.")
        httpd.shutdown()


if __name__ == "__main__":
    main()
