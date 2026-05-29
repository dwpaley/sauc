# SAUC Deployment Guide

This document describes how to deploy SAUC (Search of Alternate Unit Cells) to a production server.

## Prerequisites

### On the build/deploy machine:
- GCC with OpenMP support (`gcc`, `g++` with `-fopenmp`)
- GNU m4 (for HTML template processing)
- Python 3.7+ (for the CGI script)

### On the target server:
- Apache with mod_cgi enabled
- Python 3.7+ (for `subprocess.run(..., capture_output=True)`)
- Write access to the web root (or sudo)

## Quick Start

```bash
# 1. Clone the repository on the target server
git clone <repo-url> /path/to/sauc
cd /path/to/sauc

# 2. Deploy code (compiles binary, generates CGI and HTML, installs to HTDOCS)
./deploy.sh config_viper_lbl.sh

# 3. Deploy database files (only needed once, or when databases update)
./deploy_db.sh config_viper_lbl.sh

# 4. Configure Apache (see below) - requires sudo
# 5. Set file ownership (see below) - requires sudo
```

## Local Testing

You can test the full stack (static pages + CGI + binary) on your laptop without installing Apache. The repo includes `serve.py`, a minimal CGI-capable HTTP server built on Python's stdlib.

### One-time setup

```bash
cd /path/to/sauc

# Build the binary and generate CGI/HTML into ../sauc-deploy/
./deploy.sh config_localhost.sh

# Decompress and copy the database files (~895 MB, takes a minute)
./deploy_db.sh config_localhost.sh
```

### Start the server

```bash
./serve.py
# or explicitly:
./serve.py config_localhost.sh
```

The server listens on `http://localhost:8000/` by default. Open that URL in a browser — you should see the SAUC query form. Submitting the form calls the CGI script at `/cgi-bin/sauc_cgi.py`.

### Iterating on changes

- **Changes to `sauc_cgi.py`, `sauc.html.m4`, or C++ source:** rerun `./deploy.sh config_localhost.sh` (it will recompile the binary if source changed). No need to restart `serve.py` — it re-exec's the CGI for each request.
- **Changes to database files:** rerun `./deploy_db.sh config_localhost.sh`.
- **Changes to `serve.py`:** restart the server with Ctrl+C and re-run it.

### Quick CGI test via curl

```bash
curl "http://localhost:8000/cgi-bin/sauc_cgi.py?Centering=P&A=78&B=78&C=38&Alpha=90&Beta=90&Gamma=90&Algorithm=2&Similarity=2&NumHits=5&RangeSphere=1.5&UsePercent=no&SortbyFam=no&OutputStyle=1"
```
Expected: HTML starting with `<!DOCTYPE html>` containing matched PDB entries (lysozyme).

### Notes

- `serve.py` uses Python's `CGIHTTPRequestHandler`. It binds to `127.0.0.1` only — not exposed on the network.
- The host and port come from `HTTPDSERVER` in the config file (e.g., `localhost:8000`).
- If port 8000 is busy, edit `HTTPDSERVER` in `config_localhost.sh` and rerun `./deploy.sh` + `./serve.py`.

## Configuration Files

Two config files are provided:

| Config File | Purpose |
|-------------|---------|
| `config_localhost.sh` | Local development (serves from `../sauc-deploy/` via `serve.py`) |
| `config_viper_lbl.sh` | Production deployment to viper.lbl.gov (HTTPS) |

To deploy to a different server, copy `config_viper_lbl.sh` and modify the variables:

```bash
HTTPDSERVER=yourserver.example.com:80   # Host:port for URLs
HTDOCS=/var/www/sauc                    # Web root directory
SEARCHURL=http://yourserver.example.com:80/
CGIPATH=http://yourserver.example.com:80
CGIBIN=/var/www/sauc                    # Where CGI script lives (can equal HTDOCS)
BINPATH=/var/www/sauc/sauc-1.2.1.exe    # Absolute path to executable
MATHSCRIBEURL=http://yourserver.example.com:80/mathscribe-0.4.6
PYTHON=python3                          # Python interpreter path
CXXFLAGS="-Wall -O3 -DUSE_LOCAL_HEADERS -g -fopenmp -ftree-parallelize-loops=8"
CFLAGS="-Wall -O3 -DUSE_LOCAL_HEADERS -g -fopenmp -ftree-parallelize-loops=8"
```

## What the Deploy Scripts Do

### deploy.sh
1. Compiles `sauc-1.2.1.exe` if source files have changed
2. Generates `sauc_cgi.py` from template (substitutes BINPATH, HTDOCS, SEARCHURL)
3. Generates `sauc-1.2.1.html` from m4 template
4. Copies exe, CGI, HTML, and license files to HTDOCS/CGIBIN
5. Sets file permissions (755 for executables, 644 for static files)
6. Creates `index.html` symlink
7. Writes `DEPLOYED_VERSION` stamp

### deploy_db.sh
1. Decompresses any `.bz2` database files
2. Copies `.dmp`, `.tsv`, `.idx` files to HTDOCS
3. Copies mathscribe JavaScript library
4. Sets file permissions

## Apache Configuration (Required)

The deploy scripts do NOT configure Apache. You must add this configuration manually.

### Recommended secure configuration:

```apache
<VirtualHost *:8083>
    DocumentRoot "/var/www/sauc"

    <Directory "/var/www/sauc">
        AllowOverride All
        Require all granted
        Options -ExecCGI
    </Directory>

    # Enable CGI ONLY for sauc_cgi.py (not all .py files)
    <Files "sauc_cgi.py">
        Options +ExecCGI
        SetHandler cgi-script
    </Files>

    # Deny direct download of database files
    <FilesMatch "\.(dmp|tsv|idx)$">
        Require all denied
    </FilesMatch>
</VirtualHost>
```

### Why this configuration?

- **`Options -ExecCGI` on directory**: Prevents arbitrary .py files from being executed as CGI
- **`<Files "sauc_cgi.py">`**: Enables CGI only for the specific script
- **`<FilesMatch>`**: Prevents direct download of large database files (~895 MB total)

### For viper.lbl.gov specifically:

Apache runs from a conda environment:
```bash
cd /var/www/cctbx_xfel/conda
source etc/profile.d/conda.sh
conda activate httpd

# Edit config
vim /var/www/cctbx_xfel/conda/envs/httpd/conf/httpd.conf

# Test and reload
httpd -t && apachectl graceful
```

## File Ownership (Required)

The deploy scripts set `root:apache` ownership so Apache can read but not write to the web root. This requires sudo, so the scripts print a warning if it fails. Verify ownership manually:

```bash
sudo chown -R root:apache /var/www/sauc
sudo chmod 750 /var/www/sauc
sudo chmod 755 /var/www/sauc/sauc_cgi.py
sudo chmod 755 /var/www/sauc/sauc-1.2.1.exe
sudo chmod 644 /var/www/sauc/*.html /var/www/sauc/*.dmp /var/www/sauc/*.tsv /var/www/sauc/*.idx /var/www/sauc/*.txt
sudo chmod -R 755 /var/www/sauc/mathscribe-0.4.6
```

## Verification

### 1. Test the executable directly:
```bash
cd /var/www/sauc
printf "P\n78\n78\n38\n90\n90\n90\n2\n2\n1.5 20 1\n\n4\n\n" | \
  SAUC_BATCH_MODE=YES SAUC_JAVASCRIPT=YES ITERATE_QUERY=NO OUTPUT_STYLE=1 \
  ./sauc-1.2.1.exe
```
Expected: HTML output with PDB entries matching lysozyme.

### 2. Test the CGI script:
```bash
QUERY_STRING="Centering=P&A=78&B=78&C=38&Alpha=90&Beta=90&Gamma=90&Algorithm=2&Similarity=2&NumHits=10&RangeSphere=1.5" \
  python3 /var/www/sauc/sauc_cgi.py
```
Expected: Output starting with `Content-type: text/html`.

### 3. Test via HTTP:
```bash
curl -s "https://viper.lbl.gov/sauc_cgi.py?Centering=P&A=78&B=78&C=38&Alpha=90&Beta=90&Gamma=90&Algorithm=2&Similarity=2&NumHits=5&RangeSphere=1.5" | head -20
```
Expected: HTML with SAUC results.

### 4. Test in browser:
Navigate to `https://viper.lbl.gov/` and submit the form with default values.

## Troubleshooting

### 500 Internal Server Error
- Check Apache error log: `tail -50 /var/www/cctbx_xfel/conda/envs/httpd/logs/error_log`
- Verify Python 3.7+ is available: `python3 --version`
- Check file permissions on `sauc_cgi.py` (must be 755)
- Check SELinux context (RHEL/CentOS): `sudo chcon -t httpd_sys_script_exec_t /var/www/sauc/sauc_cgi.py`

### 403 Forbidden
- Check directory permissions (750) and ownership (root:apache)
- Check SELinux: `sudo chcon -R -t httpd_sys_content_t /var/www/sauc`

### 404 Not Found
- Verify files are in the correct directory
- Check that Apache config has the correct DocumentRoot

### Binary fails with "GLIBC not found" or "Exec format error"
- The binary must be compiled on a machine with compatible glibc
- Recompile on the target server: delete `sauc-1.2.1.exe` and run `./deploy.sh config_viper_lbl.sh`

### CGI returns no results
- Verify database files (.dmp, .tsv, .idx) are present in HTDOCS
- The executable must run from the directory containing the database files

## Security Notes

The Python CGI script (`sauc_cgi.py`) includes these security measures:
- All user input is validated against strict allowlists and ranges
- Subprocess execution uses `shell=False` to prevent command injection
- 60-second timeout prevents runaway processes
- Output is HTML-escaped to prevent XSS
- Maximum 1000 results per query

For production deployments, also consider:
- **HTTPS**: Prevents response modification in transit
- **Rate limiting**: Use `mod_ratelimit` or `mod_evasive` to prevent abuse

## Files Reference

| File | Purpose |
|------|---------|
| `deploy.sh` | Main deployment script (code, CGI, HTML) |
| `deploy_db.sh` | Database deployment script |
| `serve.py` | Local CGI-capable HTTP server for testing |
| `config_*.sh` | Environment-specific configuration |
| `sauc_cgi.py` | CGI script template (has `__BINPATH__` etc. placeholders) |
| `sauc.html.m4` | HTML template (m4 macros) |
| `*.cpp`, `*.c`, `*.h` | Source code for sauc-1.2.1.exe |
| `*.dmp.bz2` | Compressed NearTree database files |
| `*.tsv`, `*.idx` | PDB/COD data files |
