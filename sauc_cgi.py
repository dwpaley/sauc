#!/usr/bin/env python3
# sauc_cgi.py
#
# Secure Python CGI replacement for the legacy sauc.csh script.
# Handles SAUC (Search of Alternate Unit Cells) web requests safely.
#
# Configuration constants substituted by m4 at build/deploy time.
BINPATH = "__BINPATH__"       # m4 will substitute: path to sauc-1.2.1.exe
HTDOCS = "__HTDOCS__"         # m4 will substitute: htdocs directory (cwd for binary)
SEARCHURL = "__SEARCHURL__"   # m4 will substitute: URL for "New Search" link
TIMEOUT_SECONDS = 60
MAX_HITS = 1000

import os
import sys
import html
import subprocess
import tempfile
import urllib.parse

# ---------------------------------------------------------------------------
# Parameter validation helpers
# ---------------------------------------------------------------------------

ALLOWED_CENTERINGS = set("PABCFIRHUV")
ALGORITHM_NAMES = {
    1: "L1", 2: "L2", 3: "NCDist", 4: "V7", 5: "D7", 6: "S6", 7: "DC7unsrt"
}


def _first(d, key, default=""):
    """Return the first value for key from a parse_qs dict, or default."""
    vals = d.get(key)
    return vals[0].strip() if vals else default


def _parse_float(value, lo, hi, default):
    """Parse a float, clamping to [lo, hi]. Returns default on error."""
    try:
        f = float(value)
    except (ValueError, TypeError):
        return default
    if f < lo or f > hi:
        return default
    return f


def _parse_int(value, lo, hi, default):
    """Parse an int, clamping to [lo, hi]. Returns default on error."""
    try:
        i = int(value)
    except (ValueError, TypeError):
        return default
    if i < lo or i > hi:
        return default
    return i


def parse_params(query_string):
    """Parse and validate all CGI parameters. Returns a dict of safe values."""
    raw = urllib.parse.parse_qs(query_string, keep_blank_values=False)

    centering_raw = _first(raw, "Centering", "P").upper()
    centering = centering_raw if (len(centering_raw) == 1 and centering_raw in ALLOWED_CENTERINGS) else "P"

    a = _parse_float(_first(raw, "A", "5.0"), 0.1, 10000.0, 5.0)
    b = _parse_float(_first(raw, "B", "5.0"), 0.1, 10000.0, 5.0)
    c = _parse_float(_first(raw, "C", "5.0"), 0.1, 10000.0, 5.0)
    alpha = _parse_float(_first(raw, "Alpha", "90.0"), 1.0, 179.0, 90.0)
    beta  = _parse_float(_first(raw, "Beta",  "90.0"), 1.0, 179.0, 90.0)
    gamma = _parse_float(_first(raw, "Gamma", "90.0"), 1.0, 179.0, 90.0)

    algorithm  = _parse_int(_first(raw, "Algorithm",  "1"), 1, 7, 1)
    similarity = _parse_int(_first(raw, "Similarity", "2"), 1, 3, 2)
    num_hits   = _parse_int(_first(raw, "NumHits", "20"), 1, MAX_HITS, 20)

    use_percent_raw = _first(raw, "UsePercent", "no").lower()
    use_percent = use_percent_raw == "yes"

    sort_by_fam_raw = _first(raw, "SortbyFam", "no").lower()
    sort_by_fam = sort_by_fam_raw == "yes"

    output_style_raw = _first(raw, "OutputStyle", "1")
    if output_style_raw.upper() in ("TEXT", "HTML"):
        output_style = output_style_raw.upper()
    else:
        output_style = str(_parse_int(output_style_raw, 1, 9, 1))

    # RangeSphere: either a plain float or "X%" format
    range_sphere_raw = _first(raw, "RangeSphere", "1.5")
    if range_sphere_raw.endswith("%"):
        rs_val = _parse_float(range_sphere_raw[:-1], 0.0, 1000.0, 1.5)
        range_sphere = f"{rs_val}%"
    else:
        rs_val = _parse_float(range_sphere_raw, 0.0, 10000.0, 1.5)
        range_sphere = str(rs_val)

    # Range mode parameters (Similarity == 3)
    range_a     = _parse_float(_first(raw, "RangeA",     "1.0"), 0.0, 10000.0, 1.0)
    range_b     = _parse_float(_first(raw, "RangeB",     "1.0"), 0.0, 10000.0, 1.0)
    range_c     = _parse_float(_first(raw, "RangeC",     "1.0"), 0.0, 10000.0, 1.0)
    range_alpha = _parse_float(_first(raw, "RangeAlpha", "5.0"), 0.0, 180.0, 5.0)
    range_beta  = _parse_float(_first(raw, "RangeBeta",  "5.0"), 0.0, 180.0, 5.0)
    range_gamma = _parse_float(_first(raw, "RangeGamma", "5.0"), 0.0, 180.0, 5.0)

    return dict(
        centering=centering, a=a, b=b, c=c,
        alpha=alpha, beta=beta, gamma=gamma,
        algorithm=algorithm, similarity=similarity, num_hits=num_hits,
        use_percent=use_percent, sort_by_fam=sort_by_fam,
        output_style=output_style, range_sphere=range_sphere,
        range_a=range_a, range_b=range_b, range_c=range_c,
        range_alpha=range_alpha, range_beta=range_beta, range_gamma=range_gamma,
    )


def build_stdin(p):
    """Build the stdin string for the sauc binary from validated params."""
    lines = []
    lines.append(p["centering"])
    lines.append(str(p["a"]))
    lines.append(str(p["b"]))
    lines.append(str(p["c"]))
    lines.append(str(p["alpha"]))
    lines.append(str(p["beta"]))
    lines.append(str(p["gamma"]))
    lines.append(str(p["algorithm"]))
    lines.append(str(p["similarity"]))

    sim = p["similarity"]
    if sim == 2:
        sort_flag = " " if p["sort_by_fam"] else "1"
        lines.append(f"{p['range_sphere']} {p['num_hits']} {sort_flag}")
    elif sim == 3:
        lines.append(str(p["range_a"]))
        lines.append(str(p["range_b"]))
        lines.append(str(p["range_c"]))
        lines.append(str(p["range_alpha"]))
        lines.append(str(p["range_beta"]))
        lines.append(str(p["range_gamma"]))

    lines.append("")   # blank separator
    lines.append("4")  # output format
    lines.append("")   # trailing blank

    return "\n".join(lines) + "\n"


def print_headers():
    sys.stdout.write("Content-Type: text/html; charset=utf-8\r\n")
    sys.stdout.write("X-Content-Type-Options: nosniff\r\n")
    sys.stdout.write("\r\n")
    sys.stdout.flush()


def render_page(p, binary_output, binary_error):
    """Emit the full HTML page."""
    alg_name = ALGORITHM_NAMES.get(p["algorithm"], "??")
    sim_names = {1: "Nearest", 2: "Sphere", 3: "Range"}
    sim_name = sim_names.get(p["similarity"], "??")
    search_url_safe = html.escape(SEARCHURL)

    print("<!DOCTYPE html>")
    print("<html>")
    print("<head>")
    print("<title>SAUC</title>")
    print("""<script type="text/javascript">
    function open_close(id) {
        var display = document.getElementById(id).style.display;
        if (display == "block") {
            document.getElementById(id).style.display = "none";
        } else {
            document.getElementById(id).style.display = "block";
        }
    }
</script>""")
    print("</head>")
    print('<body><font face="Arial,Helvetica,Times">')

    print(f'<center>| <a href="#Results">GO TO RESULTS</a> | '
          f'<a href="{search_url_safe}">NEW SEARCH</a> |</center>')
    print("<hr />")
    print('<h1 align="center">SAUC</h1>')
    print("<center>")
    print("Search of Alternate Unit Cells")
    print("<br />Copyright Keith J. McGill 2013")
    print("<br />Rev 0.8, 24 Apr 2014 Mojgan Asadi, Herbert J. Bernstein")
    print("<br />Rev 0.9.0, 14 Aug 2015 Herbert J. Bernstein")
    print("<br />Rev 1.0.0, 22 Nov 2016 Herbert J. Bernstein")
    print("<br />Rev 1.1.1, 17 Apr 2019 Herbert J. Bernstein")
    print("<br />Rev 1.2, 15 Oct 2022, Herbert J. Bernstein")
    print("<br />Rev 1.2.1, 10 Feb 2025, Herbert J. Bernstein")
    print("</center>")
    print("<p>")
    print("<center>")
    print(f"<P>| Lattice Centering: {html.escape(p['centering'])}")
    print(f"| Cell: A:{p['a']} B:{p['b']} C:{p['c']} "
          f"Alpha:{p['alpha']} Beta:{p['beta']} Gamma:{p['gamma']} |")
    print("<br />")
    print(f"| Metric: {html.escape(alg_name)}")
    print(f"| Similarity: {html.escape(sim_name)}")
    if p["similarity"] == 2:
        print(f"| Radius of S: {html.escape(p['range_sphere'])}, "
              f"Max Hits: {p['num_hits']} |")
        print(f"| Sort by Family: {'yes' if p['sort_by_fam'] else 'no'} |")
    elif p["similarity"] == 3:
        print(f"| Range A:{p['range_a']} B:{p['range_b']} C:{p['range_c']} "
              f"Alpha:{p['range_alpha']} Beta:{p['range_beta']} Gamma:{p['range_gamma']} |")
    print("</center>")
    print("<p>")
    print('<hr /><p><h2><a name="Results"></a>Results of SAUC Run</h2>')
    print('<PRE><font face="Monaco, Andale Mono" size="2">')
    if binary_output:
        print(html.escape(binary_output))
    if binary_error:
        print(f'<!-- stderr: {html.escape(binary_error[:500])} -->')
    print("</font></pre>")
    print("<p><hr />")
    print(f'<center>| <a href="#Results">GO TO RESULTS</a> | '
          f'<a href="{search_url_safe}">NEW SEARCH</a> |</center>')
    print("</font>")
    print("</body>")
    print("</html>")


def error_page(message):
    """Emit a minimal error page (headers already sent assumption)."""
    print("<!DOCTYPE html><html><head><title>SAUC Error</title></head>")
    print("<body><h1>SAUC Error</h1>")
    print(f"<p>{html.escape(message)}</p>")
    print("</body></html>")


def main():
    query_string = os.environ.get("QUERY_STRING", "")
    p = parse_params(query_string)

    stdin_data = build_stdin(p)

    env = os.environ.copy()
    env["SAUC_BATCH_MODE"] = "YES"
    env["SAUC_JAVASCRIPT"] = "YES"
    env["ITERATE_QUERY"] = "NO"
    env["OUTPUT_STYLE"] = str(p["output_style"])

    print_headers()

    try:
        result = subprocess.run(
            [BINPATH],
            input=stdin_data,
            capture_output=True,
            text=True,
            timeout=TIMEOUT_SECONDS,
            cwd=HTDOCS,
            env=env,
        )
        binary_output = result.stdout
        binary_error = result.stderr
    except subprocess.TimeoutExpired:
        binary_output = ""
        binary_error = f"Search timed out after {TIMEOUT_SECONDS} seconds."
    except OSError as exc:
        binary_output = ""
        binary_error = f"Failed to execute binary: {exc}"

    render_page(p, binary_output, binary_error)


if __name__ == "__main__":
    main()
