`#!/bin/csh
# sauc.csh
#
# Keith J. McGill
#
# September 23, 2013
#
# This is a service script for the sauc.html web page
# It must be placed in an appropriate cgi-bin directory on
# the server pointed to by sauc.html
#
#
# Set the following to the best available search URL
set searchurl="'SEARCHURL()`"
#
# To operate correctly, the programs tr and sed must be in the
# default path and the /bin/echo version of echo must follow
# system V conventions sufficiently to produce an empty line
# call, below
#
/bin/echo "Content-type: text/html"
/bin/echo 
echo "<head>"
echo "<title>SAUC"
echo "</title>"
echo ''`<script type="text/javascript">''`
echo ''`	function open_close(id) {''`
echo ''`		var display = document.getElementById(id).style.display;''`
echo ''`		if (display == "block") {''`
echo ''`		document.getElementById(id).style.display = "none";''`
echo ''`		}''`
echo ''`		else {''`
echo ''`		document.getElementById(id).style.display = "block";''`
echo ''`        }''`
echo ''`	}''`
echo ''`</script>''`
echo "</head>"
echo ''`<body><font face="Arial,Helvetica,Times">''`
'ifelse(CGIMETHOD(),`GET',`echo $QUERY_STRING |')` tr ''`\&''` ''`\n''`  |sed "s/^./set &/" | sed "s/%2B/+/g" > /tmp/outstr$$
#cat /tmp/outstr$$
source /tmp/outstr$$
rm /tmp/outstr$$
if (! $?Centering)    set Centering = "P"
if (! $?Algorithm)    set Algorithm = "1"
if (! $?Similarity)   set Similarity = "2"
if (! $?NumHits)      set NumHits = "20"
if (! $?RangeSphere)  set RangeSphere = "1.5"
if (! $?UsePercent)   set UsePercent = "no"
if (! $?SortbyFam)    set SortbyFam = "no"
if (! $?OutputStyle)  set OutputStyle = "1"
if (! $?A)            set A = "5.0"
if (! $?B)            set B = "5.0"
if (! $?C)            set C = "5.0"
if (! $?Alpha)        set Alpha = "90.0"
if (! $?Beta)         set Beta = "90.0"
if (! $?Gamma)        set Gamma = "90.0"
if (! $?RangeA)       set RangeA = "1.0"
if (! $?RangeB)       set RangeB = "1.0"
if (! $?RangeC)       set RangeC = "1.0"
if (! $?RangeAlpha)   set RangeAlpha = "5.0"
if (! $?RangeBeta)    set RangeBeta = "5.0"
if (! $?RangeGamma)   set RangeGamma = "5.0"
echo ''`<center>''`
echo ''`| <a href="#Results">GO TO RESULTS</a>''`
echo ''`| <a href="''`${searchurl}''`">NEW SEARCH</a> |</center>''`
echo ''`<hr />''`
echo "<h1 align=center>SAUC</h1>"
echo ''`<center>''`
echo "Search of Alternate Unit Cells"
echo "<br />Copyright Keith J. McGill 2013"
echo "<br />Rev 0.8, 24 Apr 2014 Mojgan Asadi, Herbert J. Bernstein"
echo "<br />Rev 0.9.0, 14 Aug 2015 Herbert J. Bernstein"
echo "<br />Rev 1.0.0, 22 Nov 2016 Herbert J. Bernstein"
echo "<br />Rev 1.1.1, 17 Apr 2019 Herbert J. Bernstein"
echo "<br />Rev 1.2, 15 Oct 2022, Herbert J. Bernstein"
echo "<br />Rev 1.2.1, 10 Feb 2025, Herbert J. Bernstein"
echo ''`</center>''`
echo "<p>"
echo "<center>"
echo "" >/tmp/instr$$
echo ${Centering} " " >> /tmp/instr$$
echo "<P>| Lattice Centering: " $Centering 
echo $A " " >>/tmp/instr$$
echo $B " " >>/tmp/instr$$
echo $C " " >>/tmp/instr$$
echo $Alpha " " >>/tmp/instr$$
echo $Beta " " >>/tmp/instr$$
echo $Gamma " " >>/tmp/instr$$
echo "| Cell: " A:$A B:$B C:$C Alpha:$Alpha Beta:$Beta Gamma:$Gamma "|"
echo "<br />"
echo ${Algorithm} " "  >> /tmp/instr$$
echo "| Metric: "
if ($Algorithm == 1) then
	echo "L1"
else if ($Algorithm == 2) then
	echo "L2"
else if ($Algorithm == 3) then
	echo "NCDist"
else if ($Algorithm == 4) then
	echo "V7"
else if ($Algorithm == 5) then
	echo "D7"
else if ($Algorithm == 6) then
	echo "S6"
else if ($Algorithm == 7) then
	echo "DC7unsrt"
else
	echo "??"
endif
if ($Similarity == 1) then
  if ($NumHits != 1) then
    set Similarity="2"
    set RangeSphere="0"
    set UsePercent="no"
  endif
endif
echo ${Similarity} >> /tmp/instr$$
echo "| Similarity: "
if ($Similarity == 1) then
	echo "Nearest"
else if ($Similarity == 2) then
	echo "Sphere"
else
        echo "Range"
endif
if ( $Similarity == 2) then
if ( $SortbyFam == "yes" ) then
    set sortflag=" "
else
    set sortflag=1
endif
if ($UsePercent == "yes") then
        echo ${RangeSphere}% ${NumHits} ${sortflag} >> /tmp/instr$$
        echo "| Radius of S: " ${RangeSphere}%", Max Hits: " ${NumHits} " |"
        echo "| Sort by Family: " ${SortbyFam} " |"
else
	echo ${RangeSphere}  ${NumHits} ${sortflag} >> /tmp/instr$$
	echo "| Radius of S: " ${RangeSphere}", Max Hits: " ${NumHits} " |"
        echo "| Sort by Family: " ${SortbyFam} " |"
endif
else if ($Similarity == 3) then
	echo ${RangeA} >> /tmp/instr$$
	echo "| Range of A: " $RangeA
	echo ${RangeB} >> /tmp/instr$$
	echo "| Range of B: " $RangeB
	echo ${RangeC} >> /tmp/instr$$
	echo "| Range of C: " $RangeC
	echo ${RangeAlpha} >> /tmp/instr$$
	echo "| Range of Alpha: " $RangeAlpha
	echo ${RangeBeta} >> /tmp/instr$$
	echo "| Range of Beta: " $RangeBeta
	echo ${RangeGamma} >> /tmp/instr$$
	echo "| Range of Gamma: " $RangeGamma " |"
else
	echo " |"
endif
echo "" >> /tmp/instr$$
echo "4 " >> /tmp/instr$$
echo "" >>/tmp/instr$$
echo "</center>"
echo ''`<p><hr /><p><h2><a name="Results"></a>Results of SAUC Run</h2>''`
setenv SAUC_BATCH_MODE YES
setenv SAUC_JAVASCRIPT YES
setenv ITERATE_QUERY NO
setenv OUTPUT_STYLE $OutputStyle
echo ''`<PRE><font face="Monaco, Anadale Mono" size="2">''`
cd' HTDOCS()`
time 'BINPATH()` < /tmp/instr$$
rm /tmp/instr$$
#cat /tmp/instr$$ 
echo "</font></pre>"
echo ''`<p>''`
echo "<hr />"
echo ''`<center>''`
echo ''`| <a href="#Results">GO TO RESULTS</a>''`
echo ''`| <a href="''`${searchurl}''`">NEW SEARCH</a> |</center>''`
echo "</font>"
echo "</body>"'
