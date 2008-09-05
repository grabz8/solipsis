function getURLParam(name) {
    name = name.replace(/[\[]/,"\\\[").replace(/[\]]/,"\\\]");  
    var pattern = "[\\?&]"+name+"=([^&#]*)";  
    var regex = new RegExp(pattern);  
    var aResult = regex.exec(window.location.href);  
    if (aResult == null) {  
        return "";  
    } else {  
        return aResult[1];  
    }  
} 
