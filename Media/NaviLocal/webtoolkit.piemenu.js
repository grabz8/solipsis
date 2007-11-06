/**
*
*  javascript pie menu
*  http://www.webtoolkit.info/
*  author: Justas Vinevicius
*  modified: Mark Manyen
**/

var PieContextMenu = {

	// private properties.
	itemWidth : 64,
	itemHeight : 64,
	menuClassName : 'PieContextMenu',
	menus : new Array,
	menuElement : null,
	menuElementId : null,

	// private properties. Browser detect. Do not touch! :)
	IE : ( document.all && document.getElementById && !window.opera ),
	FF : (!document.all && document.getElementById && !window.opera),


	// removing this so that DOM created lists can work
	// private method. Initialize
	/*
	init : function () {

		if ( PieContextMenu.FF ) {
			document.addEventListener("DOMContentLoaded", PieContextMenu.domReady, false);
		}

		if ( PieContextMenu.IE ) {

			document.write("<scr" + "ipt id=__ieinit_PieContextMenu defer=true " +
				"src=//:><\/script>");

			var script = document.getElementById("__ieinit_PieContextMenu");
			script.onreadystatechange = function() {
				if ( this.readyState != "complete" ) return;
				this.parentNode.removeChild( this );
				PieContextMenu.domReady();
			};

			script = null;
		}

	},


	// removing it from the event and expecting a call after all the lists are created
	// also passing in the spacing data so that it can be dynamic
	
	// private method. Initialize all menus.
	//domReady : function () {
	*/
	init : function (w, h) {
		var xpos = 0;
		var ypos = 0;

		this.itemWidth = w;
		this.itemHeight =h; 
		var menus = document.body.getElementsByTagName('ul');

		for(var i = 0; i < menus.length; i++) {
			var className = menus[i].className.replace(/^\s+/g, "").replace(/\s+$/g, "");
			var classArray = className.split(/[ ]+/g);

			for (var k = 0; k < classArray.length; k++) {
				if (classArray[k] == PieContextMenu.menuClassName && menus[i].id) {
					PieContextMenu.initMenu(menus[i].id);
				}
			}
		}
		//alert(menus[0].id);
				
		xpos = window.innerWidth/2;
		ypos = window.innerHeight/2;

		PieContextMenu.menuElement = document.getElementById(menus[0].id);
		PieContextMenu.menuElement.style.left = xpos + 'px';
		PieContextMenu.menuElement.style.top = ypos + 'px';

	
		PieContextMenu.menuElement.style.display = 'block';
	
	},


	// private method.
	initMenu : function (menuId) {

		var list = document.getElementById(menuId);
		list.style.position = "absolute";
		list.style.margin = "0px";
		list.style.padding = "0px";
		list.style.listStyleType = "none";

		var items = list.getElementsByTagName('li');
		var images = list.getElementsByTagName('img');

		for(var i=0; i<images.length; i++) {
			images[i].style.border = "0px";
			/*
			if (document.all && document.getElementById && !window.opera) {
				images[i].style.filter = 'progid:DXImageTransform.Microsoft.AlphaImageLoader(src="icon-bg.png", sizingMethod="crop")';
			} else {
				images[i].style.background = "url(icon-bg.png)";
			}
			*/
		}

		var step = (2.0 * Math.PI) / (1.0 * (items.length));
		var rotation = (90 / 180) * Math.PI;
		var radius = Math.sqrt(this.itemWidth) * Math.sqrt(items.length) * 2.5;

		for(var i=0; i<items.length; i++) {
			var x = Math.cos(i*step - rotation) * radius - (this.itemWidth / 2);
			var y = Math.sin(i*step - rotation) * radius - (this.itemHeight / 2);
			items[i].style.position = "absolute";
			items[i].style.display = "block";
			items[i].style.fontSize = "0px";
			items[i].style.left = Math.round(x) + "px";
			items[i].style.top = Math.round(y) + "px";
		}

	},


	// public method. Attaches context menus to specific class names
	attach : function (classNames, menuId) {

		if (typeof(classNames) == "string") {
			PieContextMenu.menus[classNames] = menuId;
		}

		if (typeof(classNames) == "object") {
			for (x = 0; x < classNames.length; x++) {
				PieContextMenu.menus[classNames[x]] = menuId;
			}
		}

	},
}

// removed init call here. Call onload after all lists are created via DOM