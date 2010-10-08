/**
 * @license RequireJS jsonp Copyright (c) 2004-2010, The Dojo Foundation All Rights Reserved.
 * Available via the MIT, GPL or new BSD license.
 * see: http://github.com/jrburke/requirejs for details
 */
/*jslint nomen: false, plusplus: false */
/*global require: false, setTimeout: false */
"use strict";

(function () {
    var countId = 0;
	var o3 = 0;
	var bEmbed;	
	var version;
	var id = "o3";
	var name = "o3-8a66ecac-63fd-4afa-9d42-3034d18c88f4";
	var approvalDiv;
    //A place to hold callback functions
    require._jsonp = {};

	function detect() {

		var version;
		
		if (window.external && window.external.o3) {    
			version = window.external.o3.versionInfo.match(/v([\d]+\.[\d]+)/)[1];        
			embedded = true;
		}
		
		else { 
			try {
				var axo = new ActiveXObject(name);
				version = axo.versionInfo.match(/v([\d]+\.[\d]+)/)[1];
			}
			catch (e) {}
		}
		
		if (navigator.plugins && !version) {
			if (navigator.plugins[name]) {
				version = navigator.plugins[name].description.match(/v([\d]+\.[\d]+)/)[1];
			}
			else {           
				// try sniffing the mimeTypes
				name = "application/" + name;
				for (var i = 0, l = navigator.mimeTypes.length; i < l; ++i) {
					if (navigator.mimeTypes[i].type == name)
						version = "0.9";
				}
			}
		}
		
		return version ? version : "none";
	};
	
	function sniff() {
		var sAgent = navigator.userAgent.toLowerCase();
		var is_opera     = sAgent.indexOf("opera") !== -1;
		var is_konqueror = sAgent.indexOf("konqueror") != -1;
		var is_safari    = !is_opera && ((navigator.vendor
				&& navigator.vendor.match(/Apple/) ? true : false)
				|| sAgent.indexOf("safari") != -1 || is_konqueror);
		var is_ie        = (document.all && !is_opera && !is_safari);
		bEmbed           = !(is_ie && !is_opera);
	};
	
	function createHtml(options) {
		sniff();
		var out = [],
			width = 0,
			height = 0
			clsid = '8A66ECAC-63FD-4AFA-9D42-3034D18C88F4';
			
			
			
		out.push(bEmbed
			? '<embed id="' + id + '" type="application/' + name 
				+ '" width="' + width 
				+ '" height="' + height + '" '
			: '<object id="' + id + '" width="' + width 
				+ '" height="' + height + '"' 
				+ ' classid="CLSID:' + clsid + '"' 
				+ '>');

		out.push(bEmbed ? '> </embed>' : '</object>');
			
		return out.join("");
	};	
	
    require.plugin({
		prefix: "o3",

        /**
         * This callback is prefix-specific, only gets called for this prefix
         */
        require: function (name, deps, callback, context) {
            //No-op, require never gets these jsonp items, they are always
            //a dependency, see load for the action.
		},

        /**
         * Called when a new context is defined. Use this to store
         * context-specific info on it.
         */
        newContext: function (context) {
            require.mixin(context, {
                o3Waiting: []
            });
        },

        /**
         * Called when a dependency needs to be loaded.
         */
        load: function (name, contextName) {
			var context = require.s.contexts[contextName];
			context.loaded[name] = false;
			require.ready(function() {
				if (!o3){
					version = detect();
					if (version == "none")
						return; 
					
					var objHtml = createHtml();
					document.body.appendChild(
					 document.createElement("div")).innerHTML = objHtml;        
				  
					o3 = document.getElementById(id);
				}	
					
			
				//var context = require.s.contexts[contextName];
				o3.require(name);
				//moduleNames.push(name);
				require.checkLoaded(contextName);
				//head.appendChild(node);			
				context.o3Waiting.push({
					name: name
				});
				context.loaded[name] = true;
			});
        },

        /**
         * Called when the dependencies of a module are checked.
         */
        checkDeps: function (name, deps, context) {
            //No-op, checkDeps never gets these jsonp items, they are always
            //a dependency, see load for the action.
        },

        /**
         * Called to determine if a module is waiting to load.
         */
        isWaiting: function (context) {
			return false;//return !loaded;
        },
        
        beforeFinalCallback : function(cb, context){
   			//loadmodules
            /*setTimeout(function(){
                context.defined[context.o3Waiting[0].name] = {pretend: "o3"};
                cb();
            }, 1000);*/
            
			
            if (!context.o3Waiting.length) 
				return;    

			var loaded = function() {
				alert("components loaded");				
				finished = true;
				//cb();
				
				for (var i=0; i<context.o3Waiting.length; i++) {
				    var name = context.o3Waiting[i].name;
					context.defined[name] = o3[name];
				}
                cb();
			}
					
			var loaderror = function() {
				alert('kabooom!!!');
				//cb();
			}		
			
			var approve = function(obj) {
				var url = obj.approvalURL;
				
				var div = document.getElementById("test");
				if (approvalDiv) {
					approvalDiv.innerHTML = "";
				}
				else {
					approvalDiv = document.body.appendChild(
					  document.createElement("div")).innerHTML = 
						"<iframe src='" + url + "'></iframe>";			
				}
			}
			
			o3.ondone = loaded;
			//o3obj.onprogress = progress;
			o3.onfail = loaderror;
			o3.onapprove = approve;            			
			o3.loadModules();
			
        },

        /**
         * Called when all modules have been loaded.
         */
        orderDeps: function (context) {
            return;
        }
		

	});
}()); 