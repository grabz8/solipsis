/**
* The NaviData class is essentially a flexible key/value container that is used to pass data between the page and the application.
*
* @param dataName	The name of the NaviData object to create.
*
* @param dataObj	An optional shortcut to calling 'new NaviData(dataName).add(dataObj)'. See NaviData::add below.
*/
var NaviData = new Class({

	initialize: function(dataName, dataObj)
	{
		this.data = [];
		this.dataName = dataName;
		
		if($defined(dataObj))
			this.add.bind(this)(dataObj);
			
		return this;
	},
	
	/**
	* Adds data to the NaviData container.
	*
	* @param dataObj	Can be either:
	*					An object containing key/value pairs, such as: myData.add( { message: 'hello', seconds: 37, minutes: 5 } )
	*					An input element. The key/value of the data added will be the same as the current 'name' and 'value' properties of the element.
	*					A DOM element that has been instantiated as a NaviWidget (such as a <span> element turned into a ComboBox).
	*					A form element that may contain multiple input elements/NaviWidget containers.
	*/
	add: function(dataObj)
	{
		var pushData = (function(n,v) { this.data.push(n + '=' + encodeURIComponent(v)); }).bind(this);
		
		var pushInputEle = function(ele)
		{
			var n = ele.name;
			var v = ele.getValue();
			
			if(v === false || !n || ele.disabled)
				return;
			else
				pushData(n,v);
		};
			
		switch($type(dataObj))
		{
		case "object":
			this.data.push(Object.toQueryString(dataObj));
			break;
		case "element": 
			dataObj = $(dataObj);
			
			switch(dataObj.getTag())
			{
			case "input": case "select": case "textarea":
				pushInputEle(dataObj);
				break;
			case "form":
				dataObj.getFormElements().each(function(el){ pushInputEle(el); });
				$ES('*[alt=NaviWidget]', dataObj).each(function(el){ this.data.push(el.toQueryString()); }, this);
				break;
			default:
				if(dataObj.getProperty('alt') == 'NaviWidget')
					this.data.push(dataObj.toQueryString());
				break;
			}
			break;
		}
		
		return this;
	},
	
	/**
	* Adds a Query String to the NaviData container.
	*
	* @param	queryString	A standard query string with each value of every pair encoded using 'encodeURIComponent'.
	*/
	addQueryString: function(queryString){ this.data.push(queryString); return this; },
	
	/**
	* Retrieves the value of a certain key.
	*
	* @param key	The name of the value to retrieve.
	*
	* @return	If the key is found, this returns a string containing the value of the pair, otherwise 'null'.
	*/
	get: function(key)
	{
		var result = null;
		
		this.data.join('&').split('&').each( function(pair)
		{
			if(pair.split('=')[0] == key && !result)
				result = decodeURIComponent(pair.split('=')[1]);
		});
		
		return result;
	},
	
	/**
	* Retrieves the value of a certain key as a Number.
	*
	* @param key	The name of the value to retrieve.
	*
	* @return	If the key is found and it is a Number, this returns the value of the pair as a Number, otherwise 'null'.
	*/
	getNumber: function(key)
	{
		var result = this.get.bind(this)(key);
		
		if($defined(result) && !isNaN(result))
			return Number(result);

		return null;	
	},
	
	/**
	* Returns the name of this NaviData
	*/
	getName: function(){ return this.dataName; },
	
	/**
	* Sends this NaviData to the application.
	*/
	send: function()
	{
		window.statusbar = false;
		window.status = "NAVI_DATA:?" + this.dataName + "?" + this.data.join('&');	
		window.status = "";
		
		if(window.console)
		{
			console.group("NaviData sent!");
			console.log("Name: " + this.dataName);
			console.log("Data: " + this.data.join('&').split('&'));
			console.groupEnd();
		}
		
		return this;
	}
});




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
				
				//alert(panel);
				//new NaviData(panel).send();
				//if(panel == 'pAmbient') new NaviData('MdlrAmbient').add({rgb:$S(panel).background}).send();
				if(panel == 'pAmbient') {
				    new NaviData('MdlrAmbient').add({rgb:hsv2rgb(HSV)}).send();
				    new NaviData('AvatarAmbient').add({rgb:hsv2rgb(HSV)}).send();
				}
				else if(panel == 'pDiffuse') 
				{   
				    new NaviData('MdlrDiffuse').add({rgb:hsv2rgb(HSV)}).send();
				    new NaviData('AvatarDiffuse').add({rgb:hsv2rgb(HSV)}).send();
				}
				else if(panel == 'pSpecular') 
				{
				    new NaviData('MdlrSpecular').add({rgb:hsv2rgb(HSV)}).send();
				    new NaviData('AvatarSpecular').add({rgb:hsv2rgb(HSV)}).send();
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

				if(panel == 'pAmbient') {
					new NaviData('MdlrAmbient').add({rgb:hsv2rgb(HSV)}).send();
					if($('lockAmbientdiffuse').checked == true)
						new NaviData('MdlrAmbient').add({rgb:hsv2rgb(HSV)}).send();
				}
				else if(panel == 'pDiffuse') {
					new NaviData('MdlrDiffuse').add({rgb:hsv2rgb(HSV)}).send();
					if($('lockAmbientdiffuse').checked == true)
						new NaviData('MdlrDiffuse').add({rgb:hsv2rgb(HSV)}).send();
				}
				else if(panel == 'pSpecular') new NaviData('MdlrSpecular').add({rgb:hsv2rgb(HSV)}).send();
			}
/*			else if(d=='drag') {
				//var ds=$S(d!='drag'?d:o);
				//var oX=parseInt(ds.left), oY=parseInt(ds.top), eX=XY(e), eY=XY(e,1);
					 
				//ds.left=XY(e)+oX-eX+'px'; 
				//ds.top=XY(e,1)+oY-eY+'px';		
				ds.left=XY(e)+'px'; 
				ds.top=XY(e,1)+'px';		
			}*/
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
	if($('lockAmbientdiffuse').checked == true) {
		$S('pAmbient').background='#'+v;
		$S('pDiffuse').background='#'+v;
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