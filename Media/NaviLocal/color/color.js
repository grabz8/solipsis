/* DHTML Color Picker v1.0.3, Programming by Ulyses, ColorJack.com */
/* Updated August 24th, 2007 */

function $(v) { return(document.getElementById(v)); }
function $S(v) { return(document.getElementById(v).style); }
function absPos(o) { var r={x:o.offsetLeft,y:o.offsetTop}; if(o.offsetParent) { var v=absPos(o.offsetParent); r.x+=v.x; r.y+=v.y; } return(r); }  
function agent(v) { return(Math.max(navigator.userAgent.toLowerCase().indexOf(v),0)); }
function toggle(v) { $S(v).display=($S(v).display=='none'?'block':'none'); }
function within(v,a,z) { return((v>=a && v<=z)?true:false); }
function XY(e,v) { var z=agent('msie')?[event.clientX+document.body.scrollLeft,event.clientY+document.body.scrollTop]:[e.pageX,e.pageY]; return(z[zero(v)]); }
function XYwin(v) { var z=agent('msie')?[document.body.clientHeight,document.body.clientWidth]:[window.innerHeight,window.innerWidth]; return(!isNaN(v)?z[v]:z); }
function zero(v) { v=parseInt(v); return(!isNaN(v)?v:0); }

/* PLUGIN */

var maxValue={'h':360,'s':100,'v':100}, HSV={0:360,1:100,2:100};
var hSV=165, wSV=162, hH=163, slideHSV={0:360,1:100,2:100}, zINDEX=15, stop=1;
var panel = 'pAmbient';
var lockAmbientDiffuse = 0;

function HSVslide(d,o,e) {

	function tXY(e) { tY=XY(e,1)-ab.y; tX=XY(e)-ab.x; }
	function mkHSV(a,b,c) { return(Math.min(a,Math.max(0,Math.ceil((parseInt(c)/b)*a)))); }
	function ckHSV(a,b) { if(within(a,0,b)) return(a); else if(a>b) return(b); else if(a<0) return('-'+oo); }
	function drag(e) { 
		if(!stop) { 
			if(d!='drag') tXY(e);
	
			if(d=='SVslide') { 
				ds.left=ckHSV(tX-oo,wSV)+'px'; 
				ds.top=ckHSV(tY-oo,wSV)+'px';
				slideHSV[1]=mkHSV(100,wSV,ds.left); 
				slideHSV[2]=100-mkHSV(100,wSV,ds.top); 
				HSVupdate();
				
				if(panel == 'pAmbient') {
					Client.Ambient(hsv2rgb(HSV));
				}
				else if(panel == 'pDiffuse') 
				{   
					Client.Diffuse(hsv2rgb(HSV));
				}
				else if(panel == 'pSpecular') 
				{
					Client.Specular(hsv2rgb(HSV));
				}
			}
			else if(d=='Hslide') { 
				var ck=ckHSV(tY-oo,hH), j, r='hsv', z={};
		
				ds.top=(ck-5)+'px'; 
				slideHSV[0]=mkHSV(360,hH,ck);
	 
				for(var i=0; i<=r.length-1; i++) { 
					j=r.substr(i,1); 
					z[i]=(j=='h')?maxValue[j]-mkHSV(maxValue[j],hH,ck):HSV[i]; 
				}

				HSVupdate(z); 
				$S('SV').backgroundColor='#'+hsv2hex([HSV[0],100,100]);

				if(panel == 'pAmbient') 
				{
					Client.Ambient(hsv2rgb(HSV));
				}
				else if(panel == 'pDiffuse') {
					Client.Diffuse(hsv2rgb(HSV));
				}
				else if(panel == 'pSpecular') {
					Client.Specular(hsv2rgb(HSV));
				}
			}
		}
	}

	if(stop) { 
		stop=''; 
		var ds=$S(d!='drag'?d:o);

		if(d=='drag') { 
			var oX=parseInt(ds.left), oY=parseInt(ds.top), eX=XY(e), eY=XY(e,1); 
			$S(o).zIndex=zINDEX++; 
		}
		else {
			var ab=absPos($(o)), tX, tY, oo=(d=='Hslide')?2:4; 
			ab.x+=10; 
			ab.y+=22; 
			if(d=='SVslide') slideHSV[0]=HSV[0]; 
		}

		document.onmousemove=drag; 
		document.onmouseup=function() { 
			stop=1; 
			document.onmousemove=''; 
			document.onmouseup=''; 
		}; 
		drag(e);
	}
}

function HSVupdate(v) { v=hsv2hex(HSV=v?v:slideHSV);
	var rgb = hsv2rgb(HSV);
	
	//$('plugHEX').innerHTML=v;
	$('plugHEX').innerHTML=rgb;
	
//	$S('plugCUR').background='#'+v;
	
    if(panel == 'pDiffuse' || panel == 'pAmbient')
    {
        if($('lockAmbientdiffuse').checked == true) 
        {
            $S('pAmbient').background='#'+v;
            $S('pDiffuse').background='#'+v;
        }
        else
            $S(panel).background='#'+v;
    }
    else
        $S(panel).background='#'+v;
    
	return(v);
}

function loadSV() { var z='';

	for(var i=hSV; i>=0; i--) z+="<div style=\"BACKGROUND: #"+hsv2hex([Math.round((360/hSV)*i),100,100])+";\"><br /><\/div>";
	
	$('Hmodel').innerHTML=z;
}

function initColor() {
	$S('plugin').left='170px';
	$S('plugin').top='60px';
	$S('plugin').display='block';

	//$S('SV').background='#FFFFFF';

	//$S('SV').background = $S(panel).background;
	//$S('Hslide').top='10px';
	
	//HSVslide('Hslide','plugin',event);
	//stop = 1;
}




/* CONVERSIONS */

function toHex(v) { v=Math.round(Math.min(Math.max(0,v),255)); return("0123456789ABCDEF".charAt((v-v%16)/16)+"0123456789ABCDEF".charAt(v%16)); }
function rgb2hex(r) { return(toHex(r[0])+toHex(r[1])+toHex(r[2])); }
function hsv2hex(h) { return(rgb2hex(hsv2rgb(h))); }	

function hsv2rgb(r) { // easyrgb.com/math.php?MATH=M21#text21

    var R,B,G,S=r[1]/100,V=r[2]/100,H=r[0]/360;

    if(S>0) { if(H>=1) H=0;

        H=6*H; F=H-Math.floor(H);
        A=Math.round(255*V*(1.0-S));
        B=Math.round(255*V*(1.0-(S*F)));
        C=Math.round(255*V*(1.0-(S*(1.0-F))));
        V=Math.round(255*V); 

        switch(Math.floor(H)) {
            case 0: R=V; G=C; B=A; break;
            case 1: R=B; G=V; B=A; break;
            case 2: R=A; G=V; B=C; break;
            case 3: R=A; G=B; B=V; break;
            case 4: R=C; G=A; B=V; break;
            case 5: R=V; G=A; B=B; break;
        }
        return([R?R:0,G?G:0,B?B:0]);
    }
    else return([(V=Math.round(V*255)),V,V]);
}
