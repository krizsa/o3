var o3 = {
  onLoad: function() {
    // initialization code
    this.initialized = true;
    this.strings = document.getElementById("o3-strings");
    document.getElementById("contentAreaContextMenu")
            .addEventListener("popupshowing", function(e) { this.showContextMenu(e); }, false);
  },

  showContextMenu: function(event) {
    // show or hide the menuitem based on what the context menu is on
    // see http://kb.mozillazine.org/Adding_items_to_menus
    document.getElementById("context-o3").hidden = gContextMenu.onImage;
  },
  onMenuItemCommand: function(e) {
	window.open("http://www.ajax.org/o3test/v0_9/settings.html");
  },

};
window.addEventListener("load", function(e) { o3.onLoad(e); }, false);
