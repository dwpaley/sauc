`<!doctype html public "-//IETF//DTD HTML 2.0//EN">
<HTML>
<HEAD>
	<meta charset="utf-8">
	
	<link rel="stylesheet" href="http://fonts.googleapis.com/css?family=UnifrakturMaguntia">
        <link rel="stylesheet" href="'MATHSCRIBEURL()`/jqmath-0.4.6.css">
	
	<script src="'MATHSCRIBEURL()`/jquery-1.4.3.min.js"></script>
	<script src="'MATHSCRIBEURL()`/jqmath-etc-0.4.6.min.js"></script>
	<!-- <script>M.MathPlayer = false;</script> -->

<!--
To use jqMath in an html file, use a text editor to copy the above lines to the beginning of
your file, remove any corresponding old tags, and save the file in UTF-8 format.

If you aren''`t writing in English, change "en" above to the correct language code, especially if
your language allows a comma as a decimal mark (instead of a period).

If you don''`t use Fraktur characters, you can remove the link on line 7, as this may speed up
download times slightly.

You may need to change each "http://arcib.dowling.edu/~mcgillk/bgaol/mathscribe-0.4.6" path above, depending on the directory structure
that you choose.

You can use a different version of jquery (1.0+) if you prefer.  We''`ve just provided this one
for your convenience.

If you need to use the \html macro, then uncomment the M.MathPlayer line.

The files jscurry-0.4.5.js, jscurry-0.4.5.min.js, and jqmath-0.4.6.js are provided just for your
information.  They are all included, and compressed, in jqmath-etc-0.2.0.min.js.
-->
<script 
language="javascript" type="text/javascript">
function ifnotcheckedhide(testfield,hidefield) {
    //if testfield is not checked, hide hidefield;
    if (testfield.checked) {
        hidefield.style.visibility="visible";
    } else {
        hidefield.style.visibility="hidden";
    }
    return hidefield.style.visibility;
}

function ifcheckedhide(testfield,hidefield) {
    //if testfield is checked, hide hidefield;
    if (testfield.checked) {
        hidefield.style.visibility="hidden";
    } else {
        hidefield.style.visibility="visible";
    }
    return hidefield.style.visibility;
}

function pfloat(pfield){
    // validate for non-negative float   
    var charsAllowed="0123456789.+";
    var allowed;
    var plusfound;
    var dotfound;
    plusfound = 0;
    dotfound = 0;
    for(var i=0;i<pfield.value.length;i++){       
        allowed=false;
        for(var j=0;j<charsAllowed.length;j++){
            if( pfield.value.charAt(i)==charsAllowed.charAt(j) ){ 
               allowed=true;
               if (j == 11) {
                 plusfound++;
                 allowed=false;
                 plusfound--;
                 break;
               } else if (j == 10) {
                 dotfound++;
                 if (dotfound > 1) {
                   allowed=false;
                   dotfound--;
                   break;
                 }
               } 
               break;
            }
        }
        if(allowed==false){ pfield.value = pfield.value.replace(pfield.value.charAt(i),""); i--; }
    }
    return true;
}

function pint(ifield){
    // validate for positive int   
    var charsAllowed="0123456789";
    var allowed;
    var nonzero;
    nonzero = false;
    for(var i=0;i<ifield.value.length;i++){       
        allowed=false;
        for(var j=0;j<charsAllowed.length;j++){
            if( ifield.value.charAt(i)==charsAllowed.charAt(j) ){ 
               allowed=true;
               if (j > 0) nonzero = true;
               break;
            }
        }
        if(allowed==false){ ifield.value = ifield.value.replace(ifield.value.charAt(i),""); i--; }
    }
    return nonzero;
}

function gfloat(field){
    // validate for float   
    var charsAllowed="0123456789.+-";
    var allowed;
    var plusminusfound;
    var dotfound;
    plusminusfound = 0;
    dotfound = 0;
    otherfound = 0;
    for(var i=0;i<field.value.length;i++){       
        allowed=false;
        for(var j=0;j<charsAllowed.length;j++){
            if( field.value.charAt(i)==charsAllowed.charAt(j) ){ 
               allowed=true;
               if (j == 11 || j == 12) {
                 if (otherfound>0 || dotfound>0 || plusminusfound>0) {
                   allowed=false;
                 } else {
                 plusminusfound++;
                 }
               } else if (j == 10) {
                 if (dotfound>0) {
                   allowed=false;
                 } else {
                 dotfound++;
                 } 
               } else {
                 otherfound++;
               } 
            }
        }
        if(allowed==false){ field.value = field.value.replace(field.value.charAt(i),""); i--; }
    }
    return true;
}
</script>

<TITLE>
Search of Alternate Unit Cells - SAUC
</TITLE> 
</HEAD> 
<BODY>
<font face="Arial,Helvetica,Times" size="2">
<center>
| <a href="#Search">Search</a>
| <a href="#Source">Source</a>
| <a href="#Introduction">Introduction</a>
| <a href="#History">History</a>
| <a href="#Background">Background</a>
| <a href="#Implementation1">Implementation - Distance</a>
| <a href="#Implementation2">Implementation - Searching</a>
| <a href="#Comparison">Comparison of Search Methods</a>
| <a href="#Tables">Tables</a>
| <a href="#References">References</a>
|
</center>
<hr />
<center>
<H1>SAUC</H1>
Search of Alternate Unit Cells
<br />Copyright Keith J. McGill 2013, 2014
<br />SAUC was created by: Keith J McGill, Mojgan Asadi, Maria T. Karakasheva, Lawrence C. Andrews, Herbert J. Bernstein.
<br />Rev 0.8, 24 Apr 2014, Mojgan Asadi, Herbert J. Bernstein
<br />Rev 0.9, 21 Jul 2015, Herbert J. Bernstein
<br />Rev 1.0, 22 Nov 2016, Herbert J. Bernstein
<br />Rev 1.1, 11 Apr 2019, Herbert J. Bernstein
<br />Rev 1.2, 15 Oct 2022, Herbert J. Bernstein
<br />Rev 1.2.1, 10 Feb 2025, Herbert J. Bernstein
<br />Currently maintained by: Daniel Paley, Aaron Brewster, Herbert Bernstein as a project of the DIALS US National Resource, supported by National Institutes of Health / National Institute of General Medical Sciences grant R24GM154040.
<br />Contact: Daniel Paley <a href=mailto:dwpaley@lbl.gov>dwpaley@lbl.gov</a>
<FORM method='CGIMETHOD()` ACTION="'CGIBIN()`/'SAUCCGI()`">
</center>
<font size="-1">
A crystallographic cell is a representation of a lattice, but each lattice can be represented
just as well by any of an infinite number of such unit cells. Searching
for matches to an experimentally determined crystallographic unit cell in a large
collection of previously determined unit cells is a useful verification step in synchrotron
data collection and can be a screen for &quot;similar&quot; substances, but it is more
useful to search for a match to the lattice represented by the experimentally determined
cell. For identification of substances with small cells, a unit cell match
may be sufficient for unique identification. Due to experimental error and multiple
choices of cells and differing choices of lattice centering representing the
same lattice, simple searches based on raw cell edges and angles can miss similarities
among lattices. A database of lattices using the G<sup>6</sup> representation of the
Niggli-reduced cell as the search key provides a more robust and complete search.
A database using the S<sup>6</sup> representation provides very similar results
with much faster times both for database builds and searches, and is now the preferred
metric,
<p>
Searching is implemented by finding the distance from the probe cell to related
cells using a topological embedding of the Niggli reduction in G<sup>6</sup>, so that all cells
representing similar lattices will be found.  The space S<sup>6</sup> uses Selling reduction
to restrict the reduced cells considered to only cell with obtuse or 90 degree angles, 
avoiding much of the complexity of boundaries for Niggli reduced cells. The comparison of results with those from
older cell-based search algorithms suggests significant value in the new approach.
The space of unsorted DC<sup>7</sup> cells consists of Niggli reduced cells that have
been coverted to Wigner-Seitz cells in the form of the three cell edges, the three shorter
face diagonals and the shortest body diagonal. 
</font>
<center>
<br /><STRONG>
Please read the <a href="#notice">NOTICE</a> below before use of this web page
</STRONG>
<p>
<a name="Search"></a>
<INPUT type="submit">
<INPUT type="reset">
<br />
<input type=hidden name="OutputStyle" value="TEXT" />
<table border=2>
<tr><td valign=top>
<table>
<tr><th align="left">
Lattice Centering:</th>
</tr>
<tr>
<td> 
<SELECT name="Centering" size="9">
<option selected value="P">P (primitive)
<option value="A">A (a-centered)
<option value="B">B (b-centered)
<option value="C">C (c-centered)
<option value="F">F (all-faces-centered)
<option value="I">I (body-centered)
<option value="R">R (rhombohedral as hexagonal)
<option value="H">H (hexagonal)
<option value="V">V (raw g6 vector)
</SELECT>
</td>
</tr>
<th align="left">
Metric:</th>
<tr><td>
<input type="radio" name="Algorithm" value="1" />L1<br />
<input type="radio" name="Algorithm" value="2" />L2<br />
<input type="radio" name="Algorithm" value="3" />NCDist<br />
<input type="radio" name="Algorithm" value="4" />V7<br />
<input type="radio" name="Algorithm" value="6" />S6<br />
<input type="radio" name="Algorithm" value="7" checked  />DC7unsrt<br />

</td></tr>
</table>
</td>
<td valign=top>
<table>
<tr><th align="left" colspan=4>
Cell Lengths and Angles:
</th>
</tr>
<tr>
<td>Length of A</td><td><INPUT TYPE="text" onchange="pfloat(this)" NAME="A" 
VALUE="100.0" SIZE="9"></td> 
<td>Angle of Alpha</td><td> <INPUT TYPE="text" onchange="gfloat(this)" NAME="Alpha" 
VALUE="90.0" SIZE="9"></td>
</tr><tr>  
<td>Length of B</td><td><INPUT TYPE="text" onchange="pfloat(this)" NAME="B" 
VALUE="100.0" SIZE="9"> </td>
<td>Angle of Beta</td><td> <INPUT TYPE="text" onchange="gfloat(this)" NAME="Beta" 
VALUE="90.0" SIZE="9"></td>
</tr><tr>
<td>Length of C</td><td><INPUT TYPE="text" onchange="pfloat(this)" NAME="C" 
VALUE="100.0" SIZE="9"> 
<td>Angle of Gamma</td><td> <INPUT TYPE="text" onchange="gfloat(this)" NAME="Gamma" 
VALUE="90.0" SIZE="9"></td>
</tr>
<th align="left">
Similarity:</th>
<td id="NumHitsLabel" colspan=2 align="right" style="visibility:visible">
Limit Nearest or Sphere to <INPUT TYPE="text" onchange="pint(this)" NAME="NumHits" VALUE="50" SIZE="9">
Hits</td>
<tr><td>
<input id="SimNear" type="radio" name="Similarity" value="1" 
onchange="ifnotcheckedhide(document.getElementById(''`SimRange''`),document.getElementById(''`rangea''`));
ifnotcheckedhide(document.getElementById(''`SimRange''`),document.getElementById(''`rangeb''`));
ifnotcheckedhide(document.getElementById(''`SimRange''`),document.getElementById(''`rangec''`));
ifnotcheckedhide(document.getElementById(''`SimSphere''`),document.getElementById(''`RangeSphereLabel''`));
ifnotcheckedhide(document.getElementById(''`SimSphere''`),document.getElementById(''`SortSphereLabel''`));
ifcheckedhide(document.getElementById(''`SimRange''`),document.getElementById(''`NumHitsLabel''`))"/>
Nearest</td></tr>
<tr><td>&nbsp;</td></tr>
<tr><td>
<input id="SimSphere" type="radio" name="Similarity" value="2" checked 
onchange="ifnotcheckedhide(document.getElementById(''`SimRange''`),document.getElementById(''`rangea''`));
ifnotcheckedhide(document.getElementById(''`SimRange''`),document.getElementById(''`rangeb''`));
ifnotcheckedhide(document.getElementById(''`SimRange''`),document.getElementById(''`rangec''`));
ifnotcheckedhide(document.getElementById(''`SimSphere''`),document.getElementById(''`RangeSphereLabel''`));
ifnotcheckedhide(document.getElementById(''`SimSphere''`),document.getElementById(''`SortSphereLabel''`));
ifcheckedhide(document.getElementById(''`SimRange''`),document.getElementById(''`NumHitsLabel''`))"/>
Sphere</td></tr>
<tr id="RangeSphereLabel" style="visibility:visible"><td colspan=4 align="right">Radius of Sphere
<INPUT TYPE="text" onchange="pfloat(this)" NAME="RangeSphere" VALUE="2.5" SIZE="9">
&nbsp;&nbsp;&nbsp;&nbsp;<input type="radio" name="UsePercent" value="yes" checked />%
&nbsp;&nbsp;&nbsp;&nbsp;<input type="radio" name="UsePercent" value="no" />Angstroms</td></tr>
<tr id="SortSphereLabel" style="visibility:visible"><td colspan=4 align="center">Sort by <input type="radio" name="SortbyFam" value="yes" checked />family
&nbsp;<input type="radio" name="SortbyFam" value="no" />distance</td></tr>
<tr><td>
<input id="SimRange" type="radio" name="Similarity" value="3"
onchange="ifnotcheckedhide(document.getElementById(''`SimRange''`),document.getElementById(''`rangea''`));
ifnotcheckedhide(document.getElementById(''`SimRange''`),document.getElementById(''`rangeb''`));
ifnotcheckedhide(document.getElementById(''`SimRange''`),document.getElementById(''`rangec''`));
ifnotcheckedhide(document.getElementById(''`SimSphere''`),document.getElementById(''`RangeSphereLabel''`));
ifnotcheckedhide(document.getElementById(''`SimSphere''`),document.getElementById(''`SortSphereLabel''`));
ifcheckedhide(document.getElementById(''`SimRange''`),document.getElementById(''`NumHitsLabel''`))"/>
Range<br />
</td></tr>
<tr id="rangea" style="visibility:hidden">
<td>Range of A</td><td><INPUT TYPE="text" onchange="pfloat(this)" NAME="RangeA" 
VALUE="1.0" SIZE="9"></td> 
<td>Range of Alpha</td><td> <INPUT TYPE="text" onchange="gfloat(this)" NAME="RangeAlpha" 
VALUE="1.0" SIZE="9"></td>
</tr><tr id="rangeb" style="visibility:hidden">
<td>Range of B</td><td><INPUT TYPE="text" onchange="pfloat(this)" NAME="RangeB" 
VALUE="1.0" SIZE="9"> </td>
<td>Range of Beta</td><td> <INPUT TYPE="text" onchange="gfloat(this)" NAME="RangeBeta" 
VALUE="1.0" SIZE="9"></td>
</tr><tr id="rangec" style="visibility:hidden">
<td>Range of C</td><td><INPUT TYPE="text" onchange="pfloat(this)" NAME="RangeC" 
VALUE="1.0" SIZE="9"> 
<td>Range of Gamma</td><td> <INPUT TYPE="text" onchange="gfloat(this)" NAME="RangeGamma" 
VALUE="1.0" SIZE="9"></td>
</tr>
</table>
</td>
</tr>
</table>
<INPUT type="hidden" NAME="Flush" VALUE="DUMMY">
<INPUT type="submit">
<INPUT type="reset">
</Form> <hr>
</center>
<a name="notice"><H2 align="center">NOTICE</H2></a>
<center>
<table border=0>
<tr><td width=200>
<font size="2">
<P>
You may redistribute this program under the terms of the <a href="gpl.txt">GPL</a>.
<p>
Alternatively you may redistribute the functions
and subroutines of this program as an API under the
terms of the <a href="lgpl.txt">LGPL</a>.
<p>
</td>
<td>
<div style="width:800px;height:300px;overflow:scroll;border:2px solid #0000FF;">
<font size="2">
<table border=2>
<tr>
<th align="center">GPL NOTICES</th>
<th align="center">LGPL NOTICES</th></tr>
<tr><td><font size="2">
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of
(the License, or (at your option) any later version.
<p>
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
<p>
You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
02111-1307  USA</font>
</td>
<td><font size="2">
This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.
<p>
This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.
<p>
You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free
Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
MA  02110-1301  USA</font>
</td>
</tr>
</table>


<P>
<STRONG>

Some of the software and documents included within this software package are the intellectual property of 
various parties, and placement in this package does not in anyway imply that any such rights have in any 
way been waived or diminished.

<P>

With respect to any software or documents for which a copyright exists, ALL RIGHTS ARE RESERVED TO THE 
OWNERS OF SUCH COPYRIGHT.

<P>

Even though the authors of the various documents and software found here have made a good faith effort to 
ensure that the documents are correct and that the software performs according to its documentation, and 
we would greatly appreciate hearing of any problems you may encounter, the programs and documents any 
files created by the programs are provided **AS IS** without any warrantee as to correctness, 
merchantability or fitness for any particular or general use.

<P>

THE RESPONSIBILITY FOR ANY ADVERSE CONSEQUENCES FROM THE USE OF PROGRAMS OR DOCUMENTS OR ANY FILE OR FILES 
CREATED BY USE OF THE PROGRAMS OR DOCUMENTS LIES SOLELY WITH THE USERS OF THE PROGRAMS OR DOCUMENTS OR 
FILE OR FILES AND NOT WITH AUTHORS OF THE PROGRAMS OR DOCUMENTS.

</STRONG>
</font>
</div>
</td>
</tr>
</table>
</center>
<P>
<hr>
</font>
<a name="source"></a>
<H2>Access to the source of SAUC</H2>

<P>
This program and related scripts are available from github
at <a href="https://github.com/yayahjb/sauc">https://github.com/yayahjb/sauc</a>
or as a zip file from
<a href="https://github.com/yayahjb/sauc/archive/master.zip">https://github.com/yayahjb/sauc/archive/master.zip</a>.
<br />The information below is available for download from 

<a href="http://arxiv.org/abs/1307.1811">arxiv.org/abs/1307.1811</a>.
<p>

<a name="Rev_1.0"></a>
<h2>Revison 1.0</h2>
Revision 1.0 of 22 November 2016 adds support for searching cells from the Crystallographic
Open Database (COD), and has considerable speed improvements.  Use of COD in place of CSD avoids
license issues, but provides fewer small molecule cells. 
<p>
Revision 0.9 of 11 August 2015 added support for searching cell from the Cambridge Structural Database
from Cambridge Crystallographic Data Centre as a development effort in support of image identification
for synchrotrons.  That version was experimental.  Support for specifying the sphere radius as
a percentage and for specifying a limit on the number of hits are provided.
<p>
Revision 0.8 of 24 April 2014 had revised output, following the suggestions of Graeme Winter and
the good example of Nearest-Cell (Ramraj et al., 2011) in giving some of the provenance of the
cell and organizing long output lists into collapsed lists of families.  Unlike Nearest-Cell
we have chosen to organize the families on the PDB HEADER field, rather than the sequence.
For clarity, many uses of the term "algorithm" have been changed to "metric".  The output
of the nearest cell search has been changed to include the distance and to conform to the
style of the style of the Sphere search output.  The NCDist metric algorithm has been updated
to the most current release.  -- MA &amp; HJB
<p>
<a name="Introduction"></a>
<h2>Introduction</h2> 
<p>

(Andrews &amp; Bernstein, 2012) introduced a topological embedding
of the Niggli &quot;cone&quot; of reduced cells with the goal of calculating
a meaningful distance between unit cells. In the second
paper of this series, the embedding was used to determine likely
Bravais lattices for a unit cell. Here we apply the embedding to
searching within a database for lattices &quot;close&quot; to the lattice of
a given probe cell.
<p>
A crystallographic cell is a representation of a lattice, but
each lattice can be represented just as well by any of an infinite
number of such unit cells. Searching for matches to an experimentally
determined crystallographic unit cell in a large collection
of previously determined unit cells is a useful verification
step in synchrotron data collection and can be a screen for &quot;similar&quot;
substances (Ramraj et al., 2011) (Mighell, 2002), but it is
more useful to search for a match to the lattice represented by
the experimentally determined cell, which may involve many
more cells. For identification of substances with small cells,
a unit cell match may be sufficient for unique identification
(Mighell, 2001).
<p>
Due to experimental error and multiple cells representing the
same lattice and differing choices of lattice centering, simple
searches based on raw cell edges and angles can miss similarities.
A database of lattices using the G<sup>6</sup> representation of the
Niggli-reduced cell as the search key provides a more robust
and complete search. Searching is implemented by finding the
distances from the probe cell to related cells using a topological
embedding of the cone of Niggli reduced cells in G<sup>6</sup>. Comparison
of results to those from older cell-based search algorithms
suggests significant value in the new approach.
<p>

<a name="History"></a>
<h2>History</h2>

<p>

Tabulations of data for the identification of minerals dates to
the 18th and 19th centuries. Data collected included interfacial
angles of crystals (clearly related to unit cell parameters) and
optical effects. See the historical review in (Burchard, 1998).
With the discovery of x-ray diffraction, those tables were supplanted
by new collections. Early compilations that included
unit cell parameters arranged for material identification were
&quot;Crystal Structures&quot; (Wyckoff, 1931), &quot;Crystal Data Determinative
Tables&quot; (Donnay, 1943), and Handbook for Metals and
Alloys (Pearson, 1958). Early computerized searches were created
by JCPDS in the mid-1960''`s (Johnson, 2013) and the
Cambridge Structural Data file and its search programs (Allen
et al., 1973).
<p>
Those first searches were sensitive to the issues of differing
equivalent presentations of the same lattice. The first effective
algorithm for resolving that issue was (Andrews et al., 1980)
using the V7 algorithm (NIH/EPA, 1980). Subsequently, other
programs using the V7 algorithm have been described (see
Table 1). The V7 algorithm has the advantage over simple
Niggli-reduction based cell searches of being stable under
experimental error. However, sensitivity to a change in an angle
is reduced as that angle nears 90 degrees.

<p>

<a name="Background"></a>
<h2>Background</h2>
<p>

An effective search method must find ways to search for related
unit cells, even when they appear to be tabulated in ways that
make them seem different. A trivial example is:<br/>
a = 10.0, b = 10.01, c = 20, &#945; = 65, &#946; = 75, &#947; = 90
<br/>versus<br/>
a = 10.0, b = 10.05, c = 20, &#945; = 75, &#946; = 65,  &#947; = 90.
<p>
Clearly, these unit cells are almost identical, but simple tabulations
might separate them. A somewhat more complex example
includes the following primitive cells:<br/>
a = 3.1457, b = 3.1457, c = 3.1541, &#945; = 60.089, &#946; = 60.0887, &#947; = 60.104
<br/>versus<br/>
a = 3.1456, b = 3.1458, c = 3.1541, &#945; = 90.089, &#946; = 119.907, &#947; = 119.898.
<p>
Here the relationship is not as obvious. The embedding of
(Andrews &amp; Bernstein, 2012) can be used to show that the
distance between these two cells is quite small in G<sup>6</sup> (0.004
&#8491;ngstrom units squared in G<sup>6</sup>).
<p>


<a name="Implementation1"></a>
<h2>Implementation - Distance</h2>
<p>
The program SAUC is structured to allow use of several alternative
metrics for searching among cells in an attempt to identify
cells representing similar lattices. To simplify comparisons
among results with the different metrics, all have been linearized
and normalized, i.e. converted to &#8491;ngstrom units and
scaled to be commensurate with the L<sub>2</sub> norm given below:
<p>
A simple L<sub>1</sub> or L<sub>2</sub> norm based on<br/>
[a, b, c, 
&#945;(b + c)/2, &#946;(a + c)/2, 
&#947;(a + b)/2]<br/>
with the distance scaled by 1/&#8730;(6) in the case of the L<sub>1</sub>
norm and unscaled in case of the L<sub>2</sub> norm. The angles
as assumed to be in radians and the edges in &#8491;ngstroms.
The angles were converted to &#8491;ngstroms by multiplying
by the average to the relevant edge lengths.
<p>
The square root of the BGAOL Niggli cone embedding
distance NCDist based on<br/>
[a<sup>2</sup>, b<sup>2</sup>, c<sup>2</sup>, 2bc cos(&#945;), 2ac cos(&#946;), 2ab cos(&#947;)].<br/>

Before taking the square root, the distances are scaled by &#8730;(6) divided by 
the average length of the cell edges.  
The overall square root linearizes the
metric to &#8491;ngstrom units.
<p>
The V7 distances based on individual components linearized
to &#8491;ngstrom units<br/>
[a, b, c, 1/a<sup>&#8727;</sup>, 1/b<sup>&#8727;</sup>, 1/c<sup>&#8727;</sup>,V<sup>1/3</sup>]<br/>
and scaled by &#8730;(6/7). V is the volume.
<p>
These metrics are applied to reduced primitive
cells [a, b, c, 
&#945;, &#946;, 
&#947;] and, when the reciprocal cell
[a<sup>&#8727;</sup>, b<sup>&#8727;</sup>, c<sup>&#8727;</sup>, 
&#945;<sup>&#8727;</sup>, &#946;<sup>&#8727;</sup>, 
&#947;<sup>&#8727;</sup>] is needed for the V7 metric, that cell
is also reduced.
<p>
In order to facilitate comparisons to older searches that just
consider simple ranges in [a, b, c, &#945;, &#946;, &#947;], an option for such
searches was also included in SAUC.
<h3>Validity of using the square root</h3>
The use of the square root on a metric preserves the triangle
inequality, which is important in order to preserve the metric
as a metric-space &quot;metric&quot;. The triangle inequality states
that for any triangle, the sum of the lengths of any two sides
is greater than the length of the third side. In metric space
terms, the metric d(x, y) of a metric space M satisfies d(x, z) &#8804;
d(x, y) + d(y, z), &#8704;x, y, z &#8712; M. Suppose a function f satisfies
the following conditions:<br/>
u &#8805; v &#8594; f(u) &#8805; f(v), &#8704;u, v<br/>
f(u + v) &#8804; f(u) + f(v), &#8704;u, v<br/>
then, if d(x,y) satisfies the triangle inequality, f(d(x,y))will also
satisfy the triangle inequality:<br/>
d(x, z) &#8804; d(x, y) + d(y, z)<br/>
&#8594; f(d(x, z)) &#8804; f(d(x, y) + d(y, z)) &#8804; f(d(x, y)) + f(d(y, z))
<p>
The square root satisfies the stated requirements. It is monotone,
and<br/>
&#8730;(u + v) &#8804; &#8730;(u) + &#8730;(v)<br/>
&#8596; u + v &#8804; (&#8730;(u) + &#8730;(v))<sup>2</sup> = u + v + 2&#8730;(uv)<br/>
which is clearly true.

<p>

<a name="Implementation2"></a>
<h2>Implementation - Searching</h2>
<p>
Range searching in amapped embedding needs to be done using
a nearest-neighbor algorithm (or &quot;post-office problem&quot; algorithm
(Knuth, 1973)). Exact matches are unlikely since most
unit cells representing lattices in a database are experimental,
and probe cells are also likely to be experimental data. Several
efficient algorithms are available; we have used an implementation
of neartree (Andrews, 2001).
<p>
The raw unit cell data is loaded into the tree once and serialized
to a dump file on disk; subsequent searches do not need to
wait for the O(Nlog(N)) tree build, which for the 70,000+ cells
from the PDB can take half an hour in the BGAOL NCDist metric.
The linearization makes the search space more compact and
reduces the tree depth, thereby speeding searches. Because the
PDB unit cell database contains many identical cells, we modified
NearTree to handle the duplicates in auxiliary lists, further
reducing the tree depth and speeding searches.

<p>

<a name="Comparison"></a>
<h2>Comparison of Search Methods</h2>
<p>

The simplest approach to lattice searching is a simple
box search on ranges in unit cell a, b and c
and possibly on &#945;, &#946;, and &#947;, as for example in the
&quot;cell dimensions&quot; option in the RCSB advanced search at
http://www.rcsb.org/pdb/search/advSearch.do for the Protein
Data Bank (Berman et al., 2000). In the following examples, we
will call that type of search &quot;Range&quot;. For the reasons discussed
above, such simple searches can fail to find unit cells representing
similar lattices but with very different edges that actually
represent similar lattices. Such searches are best characterized
as cell searches, rather than as lattice searches.
<P>
Searching on primitive reduced cells greatly improves the
reliability of a search, as for example in (Ramraj et al., 2011) at
http://www.strubi.ox.ac.uk/nearest-cell/nearest-cell.cgi, which
uses a metric based on the reduced cell and all permutations of
axes.While an improvement over simple range searches as discussed
above, such searches can also miss similar lattices if the
number of alternate lattice presentations considered is not complete.
One way to reduce such gaps in searches is to use only
parameters that do not depend on the choice of reduced presentation.
The (Andrews et al., 1980) approach using 7 parameters
(three reduced cell edges, three reduced reciprocal cell edges
and the volume), &quot;V7&quot;, helps, but has difficulty distinguishing
cells with angles near 90 degrees. The NCDist approach used
here, derived from (Andrews &amp; Bernstein, 2012) both fills in
the gaps and handles angles near to 90 degrees.
<P>
Consider, for example, the unit cells of phospholipase A<sub>2</sub> discussed
by (Le Trong &amp; Stenkamp, 2007). They present three
alternate cells from three different PDB entries that are actually
for the same structure:<br/>
[57.98, 57.98, 57.98, 92.02, 92.02, 92.02] from entry 1FE5
(Singh et al., 2001) in space group R32,<br/>
[80.36, 80.36, 99.44, 90, 90, 120] from entry 1U4J (Singh et al.,
2005) in space group R3 and<br/>
[80.949, 80.572, 57.098, 90.0, 90.35, 90.0] from entry 1G2X
(Singh et al., 2004) in space group C2. No simple Range
search can bring these three cells together. For example, if
we use the PDB advanced cell dimensions search around the
cell from IU4J with edge ranges of +-3 &#8491;ngstroms and angle
ranges of +-1 degree, we get 28 hits: 1CG5, 1CNV, 1FW2,
1G0Z, 1GS7, 1GS8, 1HAU, 1ILD, 1ILZ, 1IM0, 1LR0, 1NDT,
1OE1, 1OE2, 1OE3, 1QD5, 1U4J, 2BM3, 2BO0, 2H8A, 2HZ5,
2OHG, 2REW, 2WCE, 3I06, 3KKU, 3Q98, 3RP2, of which
only three actually have cells close to the target using the linearized
NCDist metric : 2WCE at 2.96 &#8491;ngstroms, 1G0Z at 0
&#8491;ngstroms, and 1U4J, the target itself. The remaining cells are,
as we will see, rejected under the Nearest-Cell and the V7 metric.
The simple Range searches are not appropriate to this problem.
<P>
Table 2 shows partial results from a lattice search using
Nearest-Cell, and a V7 search using SAUC and a NCDist search
using SAUC. We have restricted the searches to NCDist distances
&#8804; 3.5 &#8491;ngstroms. The Nearest-Cell metric appears to
be in &#8491;<sup>2</sup>. The column with the square root of the Nearest-
Cell metric facilitates comparison with the linearized SAUC V7
and NCDist metrics. The searches showed consistent behavior:
The three cells noted by (Le Trong &amp; Stenkamp, 2007)
are found in the same relative positions by all three searches.
All cells found by Nearest-Cell are also found by both V7 and
NCDist. Of the 42 structures found by all three metrics within
3.5 &#8491;ngstroms under the NCDist metric, four (1G0Z, 1G2X,
1DPY and 1FE5) are E.C. class 3.1.1.4 Phospholipase A2 structures,
and three (1PKR, 1SGC and 1VRI) are other hydrolases
(E. C. classes 3.4.21.7, 3.4.21.80, and 3.4.19.2, respectively)
However, ten cells found by V7 and NCDist were not
found by Nearest-Cell (2OSN, 2CMP, 3MIJ, 2SGA, 2YZU,
3SGA, 4SGA, 5SGA, 1CDC and 2CVK). Of those ten, one
(2OSN) is an E.C class 3.1.1.4 Phospholipase A2 structure
and four (2SGA, 3SGA, 4SGA and 5SGA are hydrolases,
specifically E.C. class 3.4.21.80 Proteinase A. Two of the ten
(2YZU and 2CVK) are thioredoxin, for which the ProMOL
(Craig et al., submitted) motif finder shows significant active
site homologies to multiple hydorolase motifs (2YZU has site
homologies to 132L, 135L and 1LZ1 in E.C. class 3.2.1.17 and
to 4HOH in E.C. class 3.1.27.3, 2CVK to 1AMY in E.C. class
3.2.1.1, to 1BF2 in E.C. class 3.2.1.68, to 1EYI in class 3.2.3.11,
etc.). For 1CDC, a &quot;metastable structure of CD2&quot;, proMOL
shows an active site homology to 1ALK of E.C. class 3.1.3.1,
another hydrolase.
<P>
The significant gaps in the Nearest-Cell search do not appear
to be a problem of the distance for the Nearest-Cell search having
been cut off at a too-small value. For the common hits
between the square root of the Nearest-Cell metric and the linearized
NCDist metric, a linear fit is excellent, with R<sup>2</sup> = 0.89
and no points are very far from the line. The agreement of the
linearized V7 to the other two metrics is much noisier because
of loss of sensitivity of the V7 metric for angles near 90 degrees
and the inherent difficulty the V7 metric has in discriminating
between the + + + and - - - parts of the Niggli cone. For
example, 1GUT (Sch&#252;ttelkopf et al., 2002) is at distances 1.2
and 3.7 from 1UJ4 in the Nearest-Cell and linearized NCDist
metrics, respectively, but only 0.1 in the V7 metric. The 1GUT
cell is<p>
[78.961, 82.328, 57.031, 90.00, 93.44, 90.00] in C 1 2 1, Z=24,
with a primitive cell<p>
[57.031, 57.0367, 57.0367, 92.3918, 92.3804, 92.3804]
which corresponds to a G<sup>6</sup> vector<p>
[3252.53, 3253.18, 3253.18,-271.53,-270.208,-270.208]
and a linearized V7 vector<p>
[52.8004, 52.8057, 52.8057, 52.7101, 52.7101, 52.7053, 52.7569].
The 1U4J cell is<p>
[80.36, 80.36, 99.44, 90, 90, 120] in R3, Z=18,
with a primitive cell<p>
[57.02, 57.02, 57.02, 89.605, 89.605, 89.605]
which corresponds to a G<sup>6</sup> vector<p>
[3251.28, 3251.28, 3251.28, 44.8265, 44.8265, 44.8265]
and a linearized V7 vector<p>
[52.7902, 52.7902, 52.7902, 52.7878, 52.7878, 52.7878, 52.789]<p>
This is almost identical to the 1GUT V7 vector, even though the
corresponding primitive cells and G<sup>6</sup> cells differ significantly.

<p>
<a name="Tables"></a>
<h2>Tables</h2>
<p>
<center>
<table border=2>
<caption>
<b>Table 1.</b> Programs designed to perform effective searches in a unit cell database</caption>
<tr><th>Program</th><th>Reference</th><th Method</th></tr>
<tr><td valign="top">Cryst</td><td>Andrews et al., 1980<br />NIH/EPA, 1980</td><td valign="top">V7</td></tr>
<tr><td>cdsearch</td><td>Toby, 1994</td><td>V7</td></tr>
<tr><td>Quest</td><td>Allen et al., 1973</td><td>Reduced cell</td></tr>
<tr><td>Nearest-Cell</td><td>Ramraj et al., 2011</td><td>Reduced cell</td></tr>
<tr><td>WebCSD, Conquest</td><td>Thomas et al., 2010</td><td>G<sup>6</sup> iterative</td></tr>
<tr><td>SAUC</td><td><i>this work</i></td><td>G<sup>6</sup>, Niggli embedding</td></tr>
</table>
</center>

<p>

<center>
<table border=2>
<caption>
<b>Table 2.</b>  Comparison of search results for cell [80.36, 80.36, 99.44, 90, 90, 120] from 
entry 1U4J in space group R3.  See  (Le Trong &amp; Stenkamp, 2007).   
In each case the PDB ID (Bernstein et al., 1977) (Berman et al., 2000) 
found is shown with the distance metric for that method. In the case of Nearest-Cell
(Ramraj et al., 2011) a second column with
the square root of the metric is provided as well.   The results are sorted by the NCDist distance.  
Results have been cut off at 3.5 Angstroms in the NCDist metric.  
The three alternate cells cited by (Le Trong &amp; Stenkamp, 2007) are marked with &quot;(*)&quot;.
The Nearest Cell results are from the 
<a href="http://www.strubi.ox.ac.uk/nearest-cell/nearest-cell.cgi">www.strubi.ox.ac.uk/nearest-cell/nearest-cell.cgi</a> 
web site. 
The NCDist and V7 results are from SAUC.</caption>
<tr><th valign="top">PDB ID</th><th>Nearest-<br />Cell<br />metric</th>
             <th>Sqrt of<br />Nearest-<br />Cell</th>
             <th valign="top">V7<br />metric</th>
             <th valign="top">NCDist<br />metric</th>
             <th valign="top">Molecule</th>
             <th valign="top">E.C. Code</th></tr>
<tr><td><b>1U4J (*)</b></td><td>0</td><td>0</td><td>0</td><td>0</td><td>Phospholipase A2 isoform 2</td><td>3.1.1.4</td></tr>
<tr><td>1G0Z</td><td>0</td><td>0</td><td>0</td><td>0</td><td>Phospholipase A2</td><td>3.1.1.4</td></tr>
<tr><td><b>1G2X (*)</b></td><td>0.11</td><td>0.33</td><td>0.2</td><td>0.9</td><td>Phospholipase A2</td><td> 3.1.1.4</td></tr>
<tr><td>2OSN</td><td></td><td></td><td>0.2</td><td>0.9</td><td>Phospholipase A2 isoform 3</td><td>3.1.1.4</td></tr>
<tr><td>2CMP</td><td></td><td></td><td>0.7</td><td>1.5</td><td>Terminase small subunit</td><td></td></tr>
<tr><td>3KP8</td><td>0.43</td><td>0.66</td><td>1.1</td><td>1.7</td><td>VKORC1/Thioredoxin domain protein</td><td></td></tr>
<tr><td valign="top">3MIJ</td><td></td><td valign="top"></td><td valign="top">1.0</td><td valign="top">1.7</td><td>RNA (5''`-R(*UP*AP*GP*GP*GP*UP<br />
                                                               &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;*UP*AP*GP*GP*GP*U)-3''`)</td><td></td></tr>
<tr><td>3E56</td><td>0.4</td><td>0.63</td><td>1.5</td><td>1.9</td><td>Putative uncharacterized protein</td></tr>
<tr><td>1CSQ</td><td>0.49</td><td>0.7</td><td>1.8</td><td>2.0</td><td>Cold Shock Protein B (CSPB)</td></tr>
<tr><td>3SVI</td><td>0.54</td><td>0.73</td><td>1.9</td><td>2.1</td><td>Type III effector HopAB2</td></tr>
<tr><td>1FKF</td><td>0.83</td><td>0.91</td><td>2.7</td><td>2.4</td><td>FK506 binding protein</td><td>5.2.1.8</td></tr>
<tr><td>1FKJ</td><td>0.83</td><td>0.91</td><td>2.7</td><td>2.4</td><td>FK506 binding protein</td><td>5.2.1.8</td></tr>
<tr><td>1BKF</td><td>0.91</td><td>0.95</td><td>2.8</td><td>2.5</td><td>Subtilisin Carlsberg</td><td>3.4.21.62</td></tr>
<tr><td>1FKD</td><td>0.86</td><td>0.93</td><td>2.8</td><td>2.5</td><td>FK506 binding protein</td><td>5.2.1.8</td></tr>
<tr><td>2FKE</td><td>0.91</td><td>0.95</td><td>2.9</td><td>2.6</td><td>FK506 binding protein</td><td>5.2.1.8</td></tr>
<tr><td>3TJY</td><td>0.88</td><td>0.94</td><td>3.0</td><td>2.6</td><td>Effector protein hopAB3</td><td></td></tr>
<tr><td>2I5L</td><td>1.06</td><td>1.03</td><td>3.7</td><td>2.7</td><td>Cold shock protein cspB</td><td></td></tr>
<tr><td>2WCE</td><td>1.21</td><td>1.1</td><td>3.5</td><td>3.0</td><td>Protein S100-A12</td><td></td></tr>
<tr><td>3P63</td><td>1.28</td><td>1.13</td><td>4.0</td><td>3.0</td><td>Ferredoxin</td><td></td></tr>
<tr><td>1F9P</td><td>1.37</td><td>1.17</td><td>4.7</td><td>3.1</td><td>Connective tissue activating peptide-III</td><td></td></tr>
<tr><td valign="top">2CXD</td><td valign="top">1.36</td><td valign="top">1.17</td><td valign="top">4.7</td><td>3.1</td><td>Conserved hypothetical<br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;protein, TTHA0068</td><td></td></tr>
<tr><td>2SGA</td><td></td><td></td><td>4.9</td><td>3.1</td><td>Proteinase A</td><td>3.4.21.80</td></tr>
<tr><td>2YZU</td><td></td><td></td><td>4.8</td><td>3.1</td><td>Thioredoxin</td><td></td></tr>
<tr><td>3SGA</td><td></td><td></td><td>5.0</td><td>3.1</td><td>Proteinase A (SGPA)</td><td>3.4.21.80</td></tr>
<tr><td>4SGA</td><td></td><td></td><td>4.8</td><td>3.1</td><td>Proteinase A (SGPA)</td><td>3.4.21.80</td></tr>
<tr><td>5SGA</td><td></td><td></td><td>4.9</td><td>3.1</td><td>Proteinase A (SGPA)</td><td>3.4.21.80</td></tr>
<tr><td>1GUS</td><td>1.15</td><td>1.07</td><td>0.3</td><td>3.2</td><td>Molybdate binding protein II</td></tr>
<tr><td>1PKR</td><td>1.44</td><td>1.2</td><td>4.7</td><td>3.2</td><td>Plasminogen</td><td>3.4.21.7</td></tr>
<tr><td>1SGC</td><td>2.45</td><td>1.57</td><td>5.2</td><td>3.2</td><td>Proteinase A</td><td>3.4.21.80</td></tr>
<tr><td>2VRI</td><td>1.5</td><td>1.22</td><td>5.2</td><td>3.2</td><td>Non-structural protein 3</td><td>3.4.19.12</td></tr>
<tr><td>1CDC</td><td></td><td></td><td>4.9</td><td>3.3</td><td>CD2</td><td></td></tr>
<tr><td>1DPY</td><td>1.24</td><td>1.11</td><td>2.3</td><td>3.3</td><td>Phospholipase A2</td><td>3.1.1.4</td></tr>
<tr><td><b>1FE5 (*)</b></td><td>1.24</td><td>1.11</td><td>2.3</td><td>3.3</td><td>Phospholipase A2</td><td>3.1.1.4</td></tr>
<tr><td>1GUT</td><td>1.2</td><td>1.1</td><td>0.1</td><td>3.37</td><td>Molybdate binding protein II</td><td></td></tr>
<tr><td>2C9Q</td><td>1.46</td><td>1.21</td><td>4.8</td><td>3.3</td><td>Copper resistance protein C</td><td></td></tr>
<tr><td>2CVK</td><td></td><td></td><td>5.2</td><td>3.3</td><td>Thioredoxin</td><td></td></tr>
<tr><td>2HE2</td><td>1.48</td><td>1.22</td><td>4.8</td><td>3.3</td><td>Discs large homolog 2</td><td></td></tr>
<tr><td>2IT5</td><td>1.62</td><td>1.27</td><td>5.6</td><td>3.3</td><td>CD209 antigen, DCSIGN-CRD</td><td></td></tr>
<tr><td>3SU1</td><td>1.59</td><td>1.26</td><td>5.2</td><td>3.3</td><td>Genome polyprotein</td><td></td></tr>
<tr><td>3SU5</td><td>1.58</td><td>1.26</td><td>5.1</td><td>3.3</td><td>NS3 protease, NS4A protein</td><td></td></tr>
<tr><td>3SU6</td><td>1.52</td><td>1.23</td><td>5.0</td><td>3.3</td><td>NS3 protease, NS4A protein</td><td></td></tr>
<tr><td>1SL4</td><td>1.68</td><td>1.3</td><td>5.8</td><td>3.4</td><td>mDC-SIGN1B type I isoform</td><td></td></tr>
<tr><td>2IT6</td><td>1.73</td><td>1.32</td><td>6.0</td><td>3.4</td><td>CD209 antigen</td><td></td></tr>
<tr><td>3CYO</td><td>1.81</td><td>1.35</td><td>5.6</td><td>3.4</td><td>Transmembrane protein</td><td></td></tr>
<tr><td>3SU2</td><td>1.6</td><td>1.26</td><td>5.2</td><td>3.4</td><td>Genome polyprotein</td><td></td></tr>
<tr><td>3SU3</td><td>1.64</td><td>1.28</td><td>5.3</td><td>3.4</td><td>NS3 protease, NS4A protein</td><td></td></tr>
<tr><td>1H9M</td><td>1.18</td><td>1.09</td><td>1.1</td><td>3.5</td><td>Molybdenum-binding-protein</td><td></td></tr>
<tr><td valign="top">1X90</td><td valign="top">1.34</td><td valign="top">1.16</td><td valign="top">5.2</td><td valign="top">3.5</td><td>Invertase/pectin methylesterase<br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;inhibitor family protein</td><td></td></tr>
<tr><td>2E6L</td><td>1.78</td><td>1.33</td><td>5.8</td><td>3.5</td><td>Nitric oxide synthase, inducible</td><td>1.14.13.39</td></tr>
<tr><td>3CP1</td><td>1.98</td><td>1.41</td><td>6.1</td><td>3.5</td><td>Transmembrane protein</td><td></td></tr>
<tr><td>3SU0</td><td>1.75</td><td>1.32</td><td>5.7</td><td>3.5</td><td>Genome polyprotein</td><td></td></tr>
<tr><td>3SV6</td><td>1.74</td><td>1.32</td><td>5.6</td><td>3.5</td><td>NS3 protease, NS4A protein</td><td></td></tr>
<tr><td>3SV7</td><td>1.73</td><td>1.32</td><td>5.6</td><td>3.5</td><td>NS3 protease, NS4A protein</td><td></td></tr>
</table>
</center>


<p>
<a name="References"></a>
<h2>References</h2>
<p>

Allen, F. H., Kennard, O., Motherwell, W. D. S., Town, W. G. &amp; Watson, D. G. (1973). Journal of Chemical Documentation, 13(3), 119 -- 123.<br />
Andrews, L. (2001). C/C++ Users Journal, 19, 40 -- 49. http://sf.net/projects/ neartree.<br />
Andrews, L. C. &amp; Bernstein, H. J. (2012). arXiv preprint arXiv:1203.5146. <a href=http://arxiv.org/abs/1203.5146>arxiv.org/abs/1203.5146</a>.<br />
Andrews, L. C. &amp; Bernstein, H. J. (2013). arXiv preprint arXiv:1305.6561. <a href=http://arxiv.org/abs/1305.6561>arxiv.org/abs/1305.6561</a>.<br />
Andrews, L. C., Bernstein, H. J. &amp; Pelletier, G. A. (1980). Acta Crystallogr. A36, 248 -- 252. Berman, H. M., Westbrook, J., Feng, Z., Gilliland, G., Bhat, T. N., Weissig, H., Shindyalov,
I. N. &amp; Bourne, P. E. (2000). Nucleic Acids Res. 28, 235 -- 242.<br />
Bernstein, F. C., Koetzle, T. F., Williams, G. J. B., Meyer, Jr., E. F., Brice, M. D., Rodgers,
J. R., Kennard, O., Shimanouchi, T. &amp; Tasumi, M. (1977). J. Mol. Biol. 112, 535 -- 542.<br />
Burchard, U. (1998). The Mineralogical Record, 29(6), 517 -- 583.<br />
Craig, P. A., Hanson, B., Westin, C., Rosa, M., Bernstein, H. J., Grier, A., Osipovitch, M., MacDonald, M., Dodge, G., Boli, P. M., Corwin, C. W. &amp; Kessler, H. (submitted). B. M. C. Bioinformatics.<br />
Donnay, J. D. H. (1943). The American Minerologist, 28, 313.<br />
Johnson, G. G. (2013). Private Communication.<br />
Knuth, D. E. (1973). Sorting and Searching (The Art of Computer Programming volume 3). Addison Wesley, Reading, MA.<br />
Le Trong, I. &amp; Stenkamp, R. E. (2007). Acta Crystallogr. D63, 548 -- 549.<br />
Mighell, A. D. (2001). Journal of Research - National Institute of Standards and Technology,
106(6), 983 -- 996.<br />
Mighell, A. D. (2002). Journal of Research - National Institute of Standards and Technology,
107(5), 425 -- 430.<br />
NIH/EPA (1980). User''`s Manual NIH-EPA Chemical Information System, chap. User''`s Guide to CRYST The X-Ray Crystallographic Search System. National Institutes of Health, Environmental Protections Agency.<br />
Pearson, W. B. (1958). Handbook of Lattice Spacings and Structures of Metals and Alloys. International Series of Monographs on Metal and Physics and Physical Metallurgy, G. V. Raynor (ed.). Pergamon Press.<br />
Ramraj, V., Esnouf, R. &amp; Diprose, J. (2011). Nearest-Cell A fast and easy tool for locating crystal matches in the PDB. Tech. rep. Division of Structural Biology, University of Oxford. http://www.strubi.ox.ac.uk/nearest-cell/nearest-cell.cgi.<br />
Sch&uuml;ttelkopf, A. W., Harrison, J. A., Boxer, D. H. &amp; Hunter, W. N. (2002). Journal of Biological Chemistry, 277(17), 15013 -- 15020.<br />
Singh, G., Gourinath, S., Saravanan, K., Sharma, S., Bhanumathi, S., Betzel, C., Srinivasan, A. &amp; Singh, T. P. (2004). Acta Crystallogr. F61(1), 8 --  13.<br />
Singh, G., Gourinath, S., Sarvanan, K., Sharma, S., Bhanumathi, S., Betzel, C., Yadav, S., Srinivasan, A. &amp; Singh, T. P. (2005). J. Struct. Biol. 149(3), 264 -- 272.<br />
Singh, G., Gourinath, S., Sharma, S., Paramasivam, M., Srinivasan, A. &amp; Singh, T. P. (2001). J. Mol. Biol. 307(4), 1049 -- 1059.<br />
Thomas, I. R., Bruno, I. J., Cole, J. C., Macrae, C. F., Pidcock, E. &amp; Wood, P. A. (2010). J. Appl. Crystallogr. 43(2), 362 -- 366.<br />
Toby, B. (1994). Private Communication.<br />
Wyckoff, R. W. G. (1931). The structure of crystals. No. 19. The Chemical Catalog Company. inc.<br />


<p>
<hr />
<center>
| <a href="#Search">Search</a>
| <a href="#Source">Source</a>
| <a href="#Introduction">Introduction</a>
| <a href="#History">History</a>
| <a href="#Background">Background</a>
| <a href="#Implementation1">Implementation - Distance</a>
| <a href="#Implementation2">Implementation - Searching</a>
| <a href="#Comparison">Comparison of Search Methods</a>
| <a href="#Tables">Tables</a>
| <a href="#References">References</a>
|
</center>

<hr>
Updated 27 December 2017.
</font>
</body>
</html>'


