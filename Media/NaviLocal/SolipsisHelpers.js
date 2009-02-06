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
function getParamIntoURL(url, name) {
    name = name.replace(/[\[]/,"\\\[").replace(/[\]]/,"\\\]");
    var pattern = "[\\?&]"+name+"=([^&#]*)";
    var regex = new RegExp(pattern);
    var aResult = regex.exec(url);
    if (aResult == null) {
        return "";
    } else {
        return aResult[1];
    }
}
function getURLWoParams(url) {
    var tab = url.split(/\?|=|&/)
    return tab[0];
} 
