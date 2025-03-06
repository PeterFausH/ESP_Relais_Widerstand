const char indexHTML[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
 <head>
  <title>WP-Faketemp-Relais</title>
  <meta http-equiv="content-type" content="text/html; charset=UTF-8">
  <meta name=viewport content="width=device-width, initial-scale=1">
  <script type="text/javascript">
var Url = "/";     
window.onload = start;       //Funktion nach dem laden des HTMLs ausführen

function start () 
{
myAjax = new XMLHttpRequest();   // Bilden AJAX objekts
myAjax.onreadystatechange=LesenAjax;  // Wenn Ereignis vom Server kommt, wird funktion LesenAjax ausgeführt
setInterval(aktualisieren,1000);    // Initialisiert das Timer-gesteuerte Ausführen von Funktion //aktualisieren();
aktualisieren ();
}

function aktualisieren () 
{
myAjax.open("GET",Url+"?Zustand=r",true);        // GET-Anfrage an Server senden
myAjax.send();	
}

function LesenAjax()   //AJAX-Ereignis ist passiert
{
    if (myAjax.readyState==4 && myAjax.status==200) // Wenn antwort volständig und OK
		{                                               // dann
		  var datenstr=myAjax.responseText;               // empfangene Daten als Text 
      if (datenstr == '0') document.getElementById('check').checked=false;
      if (datenstr == '1') document.getElementById('check').checked=true;     
    }
}

function httpGet ( sende )
      {
        myAjax.open ( "GET", Url + sende, true ) ;
        myAjax.send() ;
      }
</script>

<style type="text/css" media="screen, print, projection">
/*
 css für Schiebeschalter stammt von hier 
 https://www.htmllion.com/css3-toggle-switch-button.html
 */
.switch {
	position: relative;
	display: block;
	vertical-align: top;
	width: 100px;
	height: 30px;
	padding: 3px;
	margin: 0 10px 10px 0;
	background: linear-gradient(to bottom, #eeeeee, #FFFFFF 25px);
	background-image: -webkit-linear-gradient(top, #eeeeee, #FFFFFF 25px);
	border-radius: 18px;
	box-shadow: inset 0 -1px white, inset 0 1px 1px rgba(0, 0, 0, 0.05);
	cursor: pointer;
	box-sizing:content-box;
}
.switch-input {
	position: absolute;
	top: 0;
	left: 0;
	opacity: 0;
	box-sizing:content-box;
}
.switch-label {
	position: relative;
	display: block;
	height: inherit;
	font-size: 10px;
	text-transform: uppercase;
	background: #eceeef;
	border-radius: inherit;
	box-shadow: inset 0 1px 2px rgba(0, 0, 0, 0.12), inset 0 0 2px rgba(0, 0, 0, 0.15);
	box-sizing:content-box;
}
.switch-label:before, .switch-label:after {
	position: absolute;
	top: 50%;
	margin-top: -.5em;
	line-height: 1;
	-webkit-transition: inherit;
	-moz-transition: inherit;
	-o-transition: inherit;
	transition: inherit;
	box-sizing:content-box;
}
.switch-label:before {
	content: attr(data-on);
	right: 11px;
	color: #aaaaaa;
	text-shadow: 0 1px rgba(255, 255, 255, 0.5);
}
.switch-label:after {
	content: attr(data-off);
	left: 11px;
	color: #FFFFFF;
	text-shadow: 0 1px rgba(0, 0, 0, 0.2);
	opacity: 0;
}
.switch-input:checked ~ .switch-label {
	background: #E1B42B;
	box-shadow: inset 0 1px 2px rgba(0, 0, 0, 0.15), inset 0 0 3px rgba(0, 0, 0, 0.2);
}
.switch-input:checked ~ .switch-label:before {
	opacity: 0;
}
.switch-input:checked ~ .switch-label:after {
	opacity: 1;
}
.switch-handle {
	position: absolute;
	top: 4px;
	left: 4px;
	width: 28px;
	height: 28px;
	background: linear-gradient(to bottom, #FFFFFF 40%, #f0f0f0);
	background-image: -webkit-linear-gradient(top, #FFFFFF 40%, #f0f0f0);
	border-radius: 100%;
	box-shadow: 1px 1px 5px rgba(0, 0, 0, 0.2);
}
.switch-handle:before {
	content: "";
	position: absolute;
	top: 50%;
	left: 50%;
	margin: -6px 0 0 -6px;
	width: 12px;
	height: 12px;
	background: linear-gradient(to bottom, #eeeeee, #FFFFFF);
	background-image: -webkit-linear-gradient(top, #eeeeee, #FFFFFF);
	border-radius: 6px;
	box-shadow: inset 0 1px rgba(0, 0, 0, 0.02);
}
.switch-input:checked ~ .switch-handle {
	left: 74px;
	box-shadow: -1px 1px 5px rgba(0, 0, 0, 0.2);
}
 
.switch-label, .switch-handle {
	transition: All 0.3s ease;
	-webkit-transition: All 0.3s ease;
	-moz-transition: All 0.3s ease;
	-o-transition: All 0.3s ease;
}

.switch-left-right .switch-label {
	overflow: hidden;
}
.switch-left-right .switch-label:before, .switch-left-right .switch-label:after {
	width: 20px;
	height: 20px;
	top: 4px;
	left: 0;
	right: 0;
	bottom: 0;
	padding: 11px 0 0 0;
	text-indent: -12px;
	border-radius: 20px;
	box-shadow: inset 0 1px 4px rgba(0, 0, 0, 0.2), inset 0 0 3px rgba(0, 0, 0, 0.1);
}
.switch-left-right .switch-label:before {
	background: #eceeef;
	text-align: left;
	padding-left: 80px;
}
.switch-left-right .switch-label:after {
	text-align: left;
	text-indent: 9px;
	background: #FF7F50;
	left: -100px;
	opacity: 1;
	width: 100%;
}
.switch-left-right .switch-input:checked ~ .switch-label:before {
	opacity: 1;
	left: 100px;
}
.switch-left-right .switch-input:checked ~ .switch-label:after {
	left: 0;
}
.switch-left-right .switch-input:checked ~ .switch-label {
	background: inherit;
}

</style>

 </head>
 <body>
   <p align=middle>
     <label class="switch switch-left-right">
    	<input id="check" class="switch-input" type="checkbox" onclick="httpGet('?relais=2')"/>
    	<span class="switch-label" data-on="OFF" data-off="ON"></span> 
    	<span class="switch-handle"></span> 
    </label>
    </p>
<body lang="de-DE" dir="ltr"><p align="center"><font color="#c9211e"><font face="Bitstream Vera Sans Mono, monospace"><font size="4" style="font-size: 14pt">Wärmepumpen-Bscheisserle</font></font></font></p>
<p align="center"><font face="Bitstream Vera Sans Mono, monospace">Relais aktiviert Vorwiderstand für Temp-Sensor</font>
</p>
<p align="center"><font face="Bitstream Vera Sans Mono, monospace">und senkt Temperaturmessung ab -> WP arbeitet mehr.</font>
</p>
<p align="center"><font face="Bitstream Vera Sans Mono, monospace">http mit IP/?relais=1 oder Taster oder curl-Kommando</font>
</p>
<p align="center"><font face="Bitstream Vera Sans Mono, monospace">per
curl -X  POST http://192.168.20.179/? --data "relais=2"'</font></p>
<p align="center"><font face="Bitstream Vera Sans Mono, monospace">Peter
F. aus H. im März 2025</font></p>
</body>
</html>

)=====" ;
