#!/bin/node
var fs = require('fs');
var sys = require('sys');

var includeTrace = false, 
    immLog = false, log = false;
    logFileName = 'codegenLog.txt',
    errFileName = 'codegenErr.txt';	
var i,l,files = [], arguments = process.argv, arg;

for (i=2, l=arguments.length; i<l; i++) {
	arg = arguments[i];
	switch(arg) {
		case '-h':
			// TODO:
			break;
		case '-l':
			log = true;
			break;
		case '-v':
			immLog = true;
			break;
		case '-trace':
			includeTrace = true;
			break;
		default:
			files.push(arg);			
	}	
}	

function readFile(file) {
    var size = fs.statSync(file).size,
		buf = new Buffer(size),
		fd = fs.openSync(file, 'r');
	if (!size)
		return "";
	fs.readSync(fd, buf, 0, size, 0);
	fs.closeSync(fd);
	return buf.toString();
}

function writeFile(file, data) {
    var size = data.length,
		buf = new Buffer(data),
		fd = fs.openSync(file, 'w');
		
	fs.writeSync(fd, buf, 0, size, 0);
	fs.closeSync(fd);
}


function fileWriter(fileName) {
    return function(str) {
        writeFile(fileName, str);
    }
} 

//include("tools/o3_Reporter.js");
var Reporter = {
    errors : {general:[]},
    logs   : {general:[]},        
    currentFile   : 'general',    
    logWriter : console.log, //o3.print,//o3.stdOut.write,
    errorWriter: console.log, //o3.print,//o3.stdOut.write,
    immediate: false,    
    
    reset : function() {
        with(this) { 
            errors = {general:[]};
            logs = {general:[]};
            currentFile = 'general';
        }
    },
    newFile : function(fileName) {
        with(this) { 
            currentFile = fileName;
            if (!logs[fileName]) 
                logs[fileName] = [];
            if (!errors[fileName])
                errors[fileName]= [];           
        }        
    },
    error : function(){
        with(this) {
            var i,l;
            errorWriter('ERROR: ' + currentFile + ': ');
            for (i=0,l=arguments.length; i<l; i++) {
                errors[currentFile].push(arguments[i]);
                errorWriter(arguments[i]);
            }    
        }
    },
    globalError : function() {
        with(this) {
            var i,l;
                errorWriter('GLOBAL ERROR: ');
            for (i=0,l=arguments.length; i<l; i++) {
                errors['general'].push(arguments[i]);
                errorWriter(arguments[i]);
            }    
        }
    },
    log : function(){
        with(this) {
            var i,l;
            if (immediate)
                    //o3.stdOut.write(currentFile + ': ');
					logWriter(currentFile + ': ');
            for (i=0,l=arguments.length; i<l; i++) {
                logs[currentFile].push(arguments[i]);
                if (immediate)
                    //o3.stdOut.write(arguments[i]);
					logWriter(arguments[i]);
            }    
        }
    },
    dumpLogs : function(){
        with(this) {
            dump("Logs", logs, logWriter);
        }    
    },
    dumpErrors : function(){
        with(this) { 
            dump("Errors", errors, errorWriter);            
        }    
    },
    dump : function(title, dataObj, writeMethod) {
        var file, data, t=[];
        t.push(title, ': \n================================\n');
        for (file in dataObj) {
            data = dataObj[file];
            if (data.length) 
                t.push('\n',file,': \n','-----------------\n',data.join(''));            
        }
        t.push('\n\n');
        writeMethod(t.join(''));
    } 
};

//include("tools/o3_FileHandler.js");
var FileHandler = {
   scanFiles : function(topNode) {
		function checkFile(file) {			
			var name = file.substring(file.lastIndexOf('/')+1),
                glueName,
                glueFile,            
                data = readFile(file);

            if (data.indexOf('o3_glue_gen()') == -1 || name == "cScr.h") {
				//Reporter.log('no need to gen glue for: ',name,'\n');
				return false;
            }
            
            glueName = name.substring(0,name.lastIndexOf('.h'))
				+ '_glue.h';
            glueFile = file.substring(0,file.lastIndexOf('/')+1) + glueName;
			var stat,fd;
			try {
				stat = fs.statSync(glueFile)
			} catch (e) {	
                if (fd = fs.openSync(glueFile, 'w')) {
                    fs.closeSync(fd);
					Reporter.log('created glue file: ',glueName,'\n');
                    return glueFile;
                }
                else {
                    Reporter.error('glue file could not be created: ' 
                        ,glueName,'\n');
                    return 0;    
                }
            }

            // TODO: this should be '<'
            if (stat.mtime != fs.statSync(file).mtime) {
                Reporter.log('found old glue file, old time: '
                    ,stat.mtime,'\n');
                return glueFile;
            } 
            
            Reporter.log('glue file was uptodate: ',topNode.name,'\n');
            return 0;    
        }            

        var glueFile;
        /*if (!topNode.exists) {
            Reporter.globalError('filehandler: file does not exist? ', 
                topNode.name, '\n');
            return;
        }*/
		var stat = fs.statSync(topNode)
        if (stat.isFile()) {
            Reporter.newFile(topNode);
            if (glueFile = checkFile(topNode))
                this.genGlue(topNode, glueFile);

        }
        else if (stat.isDirectory()){
			var i,l,children = fs.readdirSync(topNode);
			for (i=0, l=children.length; i<l; i++) {
                this.scanFiles(topNode + '/' + children[i]);
            }
        }
        else {
            Reporter.globalError('invalid file node: ',topNode,'\n');
        }    
          
   },
   genGlue : function(srcFile, glueFile) {     
        var i,l,result = [],
            scanned = Lexer.scan(readFile(srcFile)),
            classes = Parser.parse(scanned.tree);
        
        for (i=0, l=classes.length; i<l; i++) {
            if (classes[i].gen)
				result.push(Generator.run(classes[i].struct, classes[i].traits));
        }
        try{    
            writeFile(glueFile,result.join(''));   
        } catch(e) {
            Reporter.error('Could not write to the file: ', glueFile, '\n');
        }     
   }
};

//include("tools/o3_Parser.js");
var Lexer = {    
    scan : function(data) {
        
        var tree = [],          // parse tree
            plain_text = 0,     // block of raw text data (from c++ static string or comment)
            stack = [],         // tree stack
            type = 0,           // token type
            text_tok = 0,       // plain_text block starting token
            last_token,         // tempvar
            lines = []          // array of linepositions
            
        data.replace(TokRegexp, function(token, rx_lut, rx_ws, rx_word, rx_misc, pos) {
			// check the type
            if (rx_lut)
                type = TokenType[rx_lut];
            else if (rx_ws)
                type = TokenType.WhiteSpace;
            else if (rx_word)
                type = TokenType.Word;
            else 
                type = 0;    
            
            // if we are not inside a plain text block:
            if (!text_tok) {
                switch (type) {
                    case TokenType.Quot:
                    case TokenType.Comment:
                        tree[tree.length] = {type: type, pos: pos, token: token, plain_text: null};
                        // start a new plain text block: 
                        text_tok = token;
                        plain_text = [token];
                        break;
                        
                    case TokenType.BrackStart: 
                        tree[tree.length] = last_token = 
                            {type: type, pos: pos, token: token, subtree: null};
                        // push current tree on the stack
                        stack.push(tree);                
                        // start new tree, and the old tree (current) last element 
                        // will point to the new tree    
                        tree = last_token.subtree = [{type: 0, pos: pos, token: token}];
                        break;
                        
                    case TokenType.BrackEnd:
                        // check if the bracket match with the starting bracket
                        if (tree[0].token != TokClose[token])  {                            
                          // report error: brackets dont match
                          Reporter.error("lexer: closure token did not match, ' opened: '", 
						  tree[0].token, "' closed: '", token, "' position : ", 
							position(lines, pos).toString(),"\n");						  
                        }
                        else {
                            tree[tree.length] = {type: type, pos: pos, token: token};
                            // continue the last tree on the stack
                            tree = stack.pop();
                        }
                        break;
                        
                    case TokenType.EndOfLine: 
                        lines[lines.length] = pos;
                        break;
                        
                    case TokenType.WhiteSpace:
                        break;
                    
                    default: 
                        tree[tree.length] = {type: type, pos: pos, token: token};
                        break;
                }
            }
            // if we are in a plain_text block
            else {
                plain_text.push(token);
                switch (type) {
                    case TokenType.Quot: 
                        if (text_tok == token){
                            text_tok = 0;
                            tree[tree.length-1].plain_text = plain_text.join('');
                        }
                        break;                
                    case TokenType.Comment: 
                        if (token == '*/' && text_tok == '/*') {
                            text_tok = 0;
                            tree[tree.length-1].plain_text = plain_text.join('');
                        }
                        break;
                    case TokenType.EndOfLine: 
                        lines[lines.length] = pos;
                        if (text_tok == '//'){
                            text_tok = 0;
                            plain_text.pop();
                            tree[tree.length-1].plain_text = plain_text.join('');
                        }
                        break;
                }                
            }
        });
        while (stack.length) {
            // report error not closed
            Reporter.error("lexer: closure problem, bracket not closed.\n");
            stack.pop();
        }
        if (text_tok)
            // plain_text mode not closed      
            Reporter.error("lexer: text block not closed.\n");

        return {tree: tree, lines: lines};            
} 
}; // lexer

var Parser = {
	readName : function(tree, i, traits) {
        Reporter.log("readName: ", tree[i].token, '\n');
         traits.___name = tree[i+1].subtree[1].plain_text;
		return 0;
    },
    readExt : function(tree, i, traits) {
        Reporter.log("readExt: ", tree[i].token, '\n');
        traits.___ext = tree[i+1].subtree[1].plain_text;
        return 0;
    },
	readProp : function(tree, i, traits) {
		var index = i,
			t;
			
		while ( (t = tree[index].token) != ';' && t != '(')
			index++;
			
		if (t == ';')
			return this.readImmProp(tree, i, traits);
		else
			return this.readFunLike(tree, i, traits);	
		
	},
    readImmProp : function(tree, i, traits){
        Reporter.log("readImmProp: ",tree[i].token,'\n');
        var name,
			index=i,
			type = tree[i].token,
			getter = (type != 'o3_set'),
			setter = (type != 'o3_get');		
		
		while ( (t = tree[index].token) != ';')
			index++;
		
		if (!(name = this.checkName(traits))) {					
			name = tree[index-1].token.replace(/^m?\_+/, "");
			name = name.replace(/_[a-z]/g, function(m,a) {
				return m.slice(1).toUpperCase();                    
			});		
		}		
			
		if (getter)
			traits.add(name , {
				imm: true,
				type:'get',
				ret: tree[i+1].token,
				toString:0,
				member: tree[index-1].token
			});
		
		if (setter)
			traits.add(name, {
				imm: true,
				type:'set',
				ret: tree[i+1].token,
				toString:0,
				member: tree[index-1].token    
			});

        return 1;
    },
    readFunLike : function(tree, index, traits) {
        Reporter.log("readFunLike: ",tree[index].token,'\n');
        var br, // position of the '(' 
            subtree,
			name,
            ret = [],
            r,i,j,l,t,tl,coma,tgt=0,
            op_count, name_pos, arg_type, arg_name, arg_def, args = [],
            eq, def;
        
        // check where the bracket starts with the args    
        for (br = index; tree[br].token != '(' && br<index+10; br++);
            
        if (br==i+5) {
            Reporter.error("tokenizer: could not find the args for the function!!!\n");
            return 1;
        }
                
        for (r = index+1; r<br-1; r++)
            ret.push(tree[r].token);
        
        // travers the function arguments
        for (subtree = tree[br].subtree, l = subtree.length, i=1; i<l; i++) {
            eq = mod = arg_def = arg_name = tgt = op_count = mod_count = 0;
            arg_type = []; 
            
            // function with no args...
            if (subtree[i].token == ')')
                break;
                
            // traverse an argument
            for (j = i; j < l-1 && subtree[j].token != ','; j++) {
                
                switch (subtree[j].token) {
                    case 'o3_tgt':
                        tgt = 1;
                        mod_count++;
                        break;
                    case 'const':
                    case 'unsigned':                    
                        mod_count++;
                        break;
                    case '*':
                    case '&':
                        op_count++;
                        break;
                    case '=': 
                        eq = j; 
                        break;                        
                }        
            }
            // at this point j is the pos of the next ',' or trailing ')'                
            
            if (eq) {
                name_pos = eq - 1;                
				if (subtree[eq+1].plain_text)
                    def = subtree[eq+1].plain_text;
                else {
                    def = subtree[eq+1].token;
					if (def == '-')
						def += subtree[eq+2].token;
					else if(def == 'Str')
						def += subtree[eq+2].token + 
							   subtree[eq+2].subtree[1].plain_text +
							   subtree[eq+3].token;
				}	
            } 
            else if (op_count + mod_count + 1 == j-i ) {
                // no name for the arg
                name_pos = null;             
            }
            else {
                name_pos = j-1;
            }
                        
            // parse the full type            
            for (t=i+tgt, tl = name_pos ? name_pos : j; t<tl; t++) {
                arg_type.push(subtree[t].token);
            }                
                        
            args.push({
                tgt : tgt,
                def : def,
                name: name_pos ? subtree[name_pos].token : null,
                type: arg_type.join(' ')
            });
            i = j;
        }
        
        var ext = this.checkExt(traits);        
		if (!(name = this.checkName(traits)))
			name = tree[br-1].token;
		
		traits.add(name, {
            type:tree[index].token.replace('o3_', ''),
            ret:ret.join(''),
            args:args,            
            ftype:0,
            ext: ext,
            name: tree[br-1].token,
			toString:0            
        });
        return 1;
    },
    checkExt : function(traits) {
        if (traits.___ext) {            
            var ret = traits.___ext;
            traits.___ext = null;
            delete traits.___ext;
            return ret.replace(/\"/g, '');
        }
        return 0;    
    },
	checkName : function(traits) {
        if (traits.___name) {            
            var ret = traits.___name;
            traits.___name = null;
            delete traits.___name;
            return ret.replace(/\"/g, '');
        }
        return 0;    
    },
    readEnum : function(tree, index, traits) {	
        var subtree = tree[index+1].subtree,i,j,l,enumName,value=-1;
        
        enumName = subtree[1].plain_text.replace(/\"/g, '');
        Enums[enumName] = 1;
        
        for (i=3; i<subtree.length; ++i) {
            for(j=i, l=subtree.length-1; j<l && subtree[j].token != ','; j++)
                ;
            
            switch(j-i) {
                case 1: // simple case
                    value++; 
                    break;
                case 3: // enum entry with value
                    value = subtree[j-1].token;
                    break
                default:
                    Reporter.error("tokenizer: syntax error in enum.\n");
            }
            
            traits.add(subtree[i].token, {
                type:'get',
                ret:'int',
                value: value,
                name: subtree[i],
                enu: enumName,
                toString:0            
            });
            
            i=j;
        }
        return 1;
    },
    
    parse : function(tree, classes, in_struct, scope) {
        var struct_found,           // struct_found found in this level
            ns_found,               // ns_found found in this level
            traits,                 // array of traits collected
            first_recur,            // call was first recursion
            tagname,                // tag name (o3_ext, o3_fun, etc.) 
            elem;                   // next tree element in the loop
                
        if (!classes) {
            classes = [];
            first_recur = 1;
        }
        
        for (var i = 0, i_len = tree.length; i < i_len; i++) {            
            elem = tree[i];
            
            if (elem.token == '{' && (ns_found || struct_found)) {
                // traverse the sub tree with a recursive call
                this.parse(elem.subtree, classes, struct_found, 
                    ns_found || struct_found);
                // class/namespace parsed, reset and continue
                ns_found = struct_found = base_class_found = null;
                continue;
            }
            
            if (elem.type != TokenType.Word) 
                continue;
                
            switch (elem.token) {
                    case 'class':
                    case 'struct':                    
                        struct_found = tree[i+1].token;        
                        break;
                    case 'namespace':
                        ns_found = tree[i+1].token;
                        break;  
                    default:
                        // skipp tokens untill we find an o3 tag
                        if (!(typename = elem.token.match(/^o3_(\w+)/)))
                            continue;
                        if (!in_struct)
                            // report error, o3_ tag out of class scope
                            continue;
                        if (!traits) {
                            // first trait for the current struct, lets create
                            // a container object for the trait of this struct
                            classes.push({
                                traits: traits = {
                                    ext : {},                   // ext traits
                                    cls : {},                   // cls traits
                                    add : function(name, o) {   // add trait function
                                        var tName = name,
                                            tType = o.ext ? 'ext' : 'cls';
                                        if (o.type == 'set')
                                            tName = name.replace(/^set(\w)/, 
                                                function(m, a){ return a.toLowerCase();});
                                          
                                        if (tName == 'toString')
                                            tName = '_toString';
                                        (this[tType][tName] ? this[tType][tName] : this[tType][tName] = []).push(o);        
                                    }   
                                },
                                struct: in_struct,
                                base:''
                            });
                        }
                        switch(typename[0]) {
                            case 'o3_name': 
                                i += this.readName(tree, i, traits);                        
                                break;
							case 'o3_prop': 
                                i += this.readImmProp(tree, i, traits);                        
                                break;
                            case 'o3_ext':
                                i += this.readExt(tree, i, traits);
                                break;        
                            case 'o3_fun':
								i += this.readFunLike(tree, i, traits);                        
                                break;
                            case 'o3_get':
                            case 'o3_set':                             
                                i += this.readProp(tree, i, traits);                        
                                break;
                            case 'o3_enum':                             
                                i += this.readEnum(tree, i, traits);                        
                                break;                                
                            case 'o3_begin_class':
                                traits.base = tree[i+1].subtree[1].token;                                    
                            case 'o3_end_class': 
                            case 'o3_add_iface':
                            case 'o3_cls':    
                                break;
                            case 'o3_glue_gen':
                                classes[classes.length-1].gen = 1;
                                // macro was not in a comment block...
                                break
                            default : 
                                // report error
                                Reporter.error("tokenizer: found unknown o3_tag : ",typename[0],"\n");
                        }
            } 
                         
                              
        }
        if (first_recur)
            return classes;
    }
}; // parser    

var TokenType = {
    BrackStart : 2,
    BrackEnd : 3,
    Quot : 4,
    Word : 5,
    EndOfLine : 6,            
    Comment : 7,
    WhiteSpace : 8,
    '"': 4, 
    '\'': 4,
    '[': 2,
    ']': 3, 
    '{': 2, 
    '}': 3, 
    '(': 2, 
    ')': 3,
    '\n': 6, 
    '\r\n': 6, 
    '//': 7, 
    '/*': 7, 
    '*/': 7
};

var TokClose = {'}': '{', ']': '[', ')': '('};
var TokRegexp = /(["'{(\[\])}\]]|\r?[\n]|\/[\/*]|\*\/)|([ \t]+)|([\w._])+|(\\?[\w._?,:;!=+-\\\/^&|*"'[\]{}()%$#@~`<>])/g;
 
function posToString(){
	return 'Ln : ' + this.line + ', Col : ' + this.col;
}

function position(lines,pos) {
	for (var i = 0, j = lines.length; i < j && lines[i] < pos; i++);
	return {line: i+1, col: pos - lines[i - 1], toString: posToString};
};

//include("tools/o3_Generator.js");
var Generator = {
    //properties:
    //{
    //      cls: {
    //          prop1: [trait1,trait2,...],
    //          prop2: [...]
    //          ...
    //      }
    //      ext: {
    //          prop1: [...],
    //          ...
    //      }
    // }
 
    // Generates the whole glue file for a class
    run : function(className, properties) {
        var t=[];
        t.push('#ifdef O3_WITH_GLUE\n',
            '#pragma warning( disable : 4100)\n',
            '#pragma warning( disable : 4189)\n',            
            'namespace o3 {\n\n\n',        
            this.genSelect(className),
            this.genTables(className, properties),
            this.genInvokes(className, properties),
            '}\n#endif\n',
            '#pragma warning(default : 4100)\n',
            '#pragma warning(default : 4189)\n');
        return t.join('');    
    },
    
    // Generates the select function
    genSelect : function(className) {
        var t = [];
        t.push('Trait* ',className,'::select()\n{\n   return clsTraits();\n}\n\n');
        return t.join('');
    },

    // Generates the tables
    genTables : function(className, properties) {
        function wss(n) {
            var i,t=[];
            for (i=0; i<n; i++)
                t.push(' ');
            return t.join('');            
        }

        var uid=0,  // unique id (for properties) (Trait::offset)
        sid=0,      // serial id (for traits)     (Trait::index) 
        ext=0,      
        set,        // set of properties to generater invoke for 
        prop,propName, // current property in the loop and its name
        i,i_l,  
        tr,         // current trait for the current property in the loop
        tType,      // first round its 'cls' then 'ext' in the second
        clsName,    // name of the class for cls traits, name of the class
                    // to extend for ext traits
        t=[];       // table
                   
        // traverse first the ext then the cls properties
        for (;ext<2;ext++) {                       
            tType = ext?'ext':'cls';
            set = properties[tType];
            
            t.push('Trait* ', className,'::', tType , 'Traits()\n',
                '{\n', '      ', 'static Trait TRAITS[] = {\n');
            t.push('         {      0,      Trait::TYPE_BEGIN,      "', className, 
                '",',wss(19-className.length),'0,',wss(20),'0,              0,      ', 
                (ext ? '0                  }' : (properties.base + '::clsTraits()  }')),
                ',\n');    
            // traverse the selected set of properties by trait names
            for (propName in set) {
                prop = set[propName];
                prop.uid = uid;
                // bahhh temp. workaround for a stupid IE bug...
                if (propName == '_toString')
                    propName = 'toString';
                
                // traverse the set of properties that has the same name 
                for (i=0, i_l=prop.length; i<i_l; i++) {
                        tr = prop[i];
                        tr.sid = sid++;        
                        clsName = ext ? tr.ext : className;
                        t.push('         {      ', uid, uid > 9 ? ',     ' : ',      ',
                            'Trait::TYPE_', tr.type.toUpperCase(), ',        "', 
                            clsName, '",',wss(19-clsName.length),'"', propName, 
                            '",', wss(19-propName.length), tType, 'Invoke,      ', 
                            tr.sid, tr.sid > 9 ? ',     ' : ',      ','0                  },\n' );
                }
                uid++;
            }
            t.push('         {      0,      Trait::TYPE_END,        "', className, 
                '",',wss(19-className.length),'0,',wss(20),'0,              0,      ',
                '0                  }', ',\n');
            t.push('      };\n\n','      ','return TRAITS;\n}\n\n');                        
            uid = sid = 0;        
        }
        
        return t.join('');
    },
    
    // Generates the invoke functions
    genInvokes : function(className, properties) {
        var t = [],
            tType,
            set,
            propName,
            prop,
            hasProps = {ext:false, cls:false},
            ext;
        
        // check if there are any properties at all...
        for (ext=0;ext<2;ext++) { 
            tType = ext?'ext':'cls';
            for (var i in properties[tType]) {
                hasProps[tType] = true;
                break;
            }
        }
        
        // generate the clsInvoke and extInvoke functions    
        for (ext=0;ext<2;ext++) { 
            tType = ext?'ext':'cls';
            set = properties[tType];            
            t.push('siEx ',className,'::',tType,'Invoke(iScr* pthis, iCtx* ctx, int index, int argc,\n',
                   '           const Var* argv, Var* rval)\n{\n');
            // against unreference arguments warning: 
            t.push('      siEx ex;\n');
            // generate the switch only if there are any properties
            if (hasProps[tType]) {
                t.push('      ', className, '* pthis1 = (', className, '*) pthis;\n\n');
                t.push('      ', 'switch(index) {\n');
                for (propName in set) {
                    prop = set[propName];
                    this.genBlockForProp(prop,t,'         ');        
                }    
                t.push('      }\n');
            }
            t.push('      ','return ex;\n}\n\n');
        }
        
        return t.join('');    
    },
    
    // Generates all the 'case' blocks in the invoke function for a property
    genBlockForProp : function(prop,t,ws) {                
        function overloadType(prop) {
            if (prop.length < 2)
                return 0;
            if (prop.length == 2) { 
                var t1 = prop[0].type,
                    t2 = prop[1].type;
                if (  (t1 = 'get' &&  t2 == 'set') 
                    ||(t1 = 'set' &&  t2 == 'get') )
                        return 0;
                return 1;        
            }            
            for (var i in prop)
                if (prop[i].type == 'get') 
                    return 2;
            return 1;                
        };
    
        var v, trait, first, overloads = [], ws2 = ws + '   ';
        
        // for one property there can belong more traits, 
        // a getter and a setter for example, or several overloads
        // for the same function, or a getter and several overloads
        // for the setter function...
        switch(overloadType(prop)){
            // no overloads
            case 0:
                for (v=0;v<prop.length; v++) {
                    trait = prop[v];
                    t.push(ws, 'case ', trait.sid, ':\n');
                    this.genBlockForTrait(trait,t,ws2, true);
                    t.push('\n',ws2, 'break;\n');
                }                    
                break;
            
            // overloaded function / setter
            case 1:                
                t.push(ws, 'case ', prop[0].sid, ':\n');
                this.genBlockForOverloads(prop, t, ws2);
                t.push(ws2, 'break;\n');
                break;
            
            // overloaded setter with a getter
            case 2:
                for (v=0;v<prop.length; v++) {
                    trait = prop[v];
                    if (trait.type == 'get') {
                        t.push(ws, 'case ', trait.sid, ':\n');
                        this.genBlockForTrait(trait,t,ws2);
                        t.push('\n',ws2, 'break;\n');
                    }
                    else { 
                      overloads.push(trait);  
                      if (!first)
                        first = v;    
                    }
                }
                t.push(ws, 'case ', prop[first].sid, ':\n');
                this.genBlockForOverloads(overloads, t, ws2);
                t.push('\n',ws2, 'break;\n');
                break;
            
            // error
            default:
                // TODO: implement checking for invalid overloads...
                Reporter.error('generator: illegal overload');
        }
    },
    
    
    genBlockForTrait : function(trait,t,ws,check) {        
        function genImmCall(trait) {
            if (trait.type == 'get')
                return {call: 'pthis1->' + trait.member};
            
            return genArgsForCall({args: [{type:trait.ret.replace(/si[A-Z][\w]+/, function(m,a){
											return m.slice(1)+' *';
									}
								)}]});     
        };
        function genEnumCall(trait) {            
            return trait.value;
        };

        // '*rval = ' type of assignement
        function genCallWrapper(trait) {
            var siSg, base;
            
            if (trait.imm && trait.type == 'set') {
                return {start:'pthis1->' + trait.member + ' = ', close:';'};
            }
            
            switch (trait.ret) {
                case 'void': 
                    return {start:'', close:';'};
                case 'bool':    case 'int':     case 'int32_t':
                case 'int64_t': case 'size_t':  case 'double':
                case 'Str':     case 'WStr':    case 'siScr':
                case 'Var':
                    return {start:'*rval = ', close:';'};
                case 'Buf':
                    return {start:'*rval = o3_new(cScrBuf)(', close:');'};            
                default: {
                    if (trait.ret.indexOf('tVec') != -1) {
                        base = trait.ret.match(/<[\w]+>/);
                        base = base[0].substring(1,base[0].length-1);
                        return {start:'*rval = o3_new(tScrVec<' + base + '>)(', close:');'};
                    }                    
                    if (siSg = trait.ret.match(/si[\w]+/)){
                        return {start:'*rval = ' + siSg[0] + '(', close:');'};
                    }
                    if (Enums[trait.ret]) {
                        return {start:'*rval = ', close:';'};
                    }
                    Reporter.error('generator: unknown return type: ', trait.ret, '\n');
                }
            
            }
        };
        // arguments for the function call + arg count check    
        function genArgsForCall(trait) {
            var args = trait.args, i, min=-1, max=0, fetch='', spec_arg=false, def_start='', def_close='', 
            wrap_start='', wrap_close='', argc_check=[], call = [],info,arglist=false;    
            
            for (i=0; i<args.length; i++) {
                if (!(info = ArgInfo[args[i].type]))
                    info = ArgInfo.si(args[i].type)
                 
				if (info.arglist) {
					arglist=true;
					call.push(info.fetch);					
					call.push(',');
					if (!args[i+1] || args[i+1].type != 'int')
						Reporter.error('generator: genArgsForCall failed: Var* as '
							+'function argument must be followed by an int argument (argc)');
					i++; 
					continue;
				}
				 
                fetch = args[i].tgt ? 'pthis' : info.fetch;
                wrap_start = info.wrap ? info.wrap + '(' : '';
                wrap_close = info.wrap ? ')' : '';

				// second wrapper for the Buf...
                if (info.wrap2) {
                    wrap_start = info.wrap2 + '(' + wrap_start;
                    wrap_close += ')';
                }     

				// not real script argument like siEx*, iCtx*, o3_tgt, etc.
                spec_arg = info.type ? (args[i].tgt ? true : false) : true;
                
                if (args[i].def && !spec_arg) {
                    if (min<0)
                        min = max;
                    // if it was an siEx* param for example we dont want to check the arg count
					def_start = spec_arg ? '' : ('argc > ' + max + ' ? ');
                    def_close = ' : ' + args[i].def;
                }
                    
                call.push(wrap_start, def_start);
                if (!spec_arg) {
                    call.push('argv[',max++,']');
					if (!info.direct)	
						call.push('.');
				}	
				
                call.push(fetch,def_close,wrap_close, ',');            
                                    
                iSg=fetch=def_start=def_close=wrap_start=wrap_close='';
                spec_arg= false;        
            }
            if (args.length > 0)
                call.pop(); // remove last ',' 
            			
            if (min>0)
                argc_check.push('argc < ', min, ' && ');
            if (min==-1)
                argc_check.push('argc != ', max);
            else
                argc_check.push('argc > ', max );
            
            return {
					call: call.join(''), 
					argc_check: arglist ? null : argc_check.join('')
				};
					
            // like: {call: 'ctx, argv[0].toInt(), &ex', argc_check: 'argc!=1'}
        };    
        
        var args, wrapper = genCallWrapper(trait);
        
        if (trait.imm)
            t.push(ws, wrapper.start, genImmCall(trait).call, wrapper.close)
        else if (trait.enu)
            t.push(ws, wrapper.start, genEnumCall(trait), wrapper.close)
        else { 
            args = genArgsForCall(trait);
            if (check)             
                t.push(ws, 'if (', args.argc_check, ')\n', ws, '   ', 
                    'return o3_new(cEx)("Invalid argument count. ( ',trait.name,' )");\n');
            
            t.push(ws, wrapper.start, 'pthis1->',
                trait.cName ? trait.cName : trait.name, '(', args.call, ')' , wrapper.close);
        }
    },
    
    genBlockForOverloads : function(overloads, t, ws) {
        function classifyArgs(fun) {            
            var min=0, max=0, spec=[], scr=[], foundDef, arg, varInfo;
            for (var i=0; i<fun.args.length; i++) {
                arg = fun.args[i];
                if (arg.tgt) {
                    spec.push(arg);
                    continue;
                }                
                
                varInfo = ArgInfo[arg.type] ? ArgInfo[arg.type] : ArgInfo.si(arg.type);
                                                
                if ( varInfo && varInfo.type ) {
                    arg.si=varInfo.wrap;
                    arg.varType = varInfo.type;
                    scr.push(arg);
                    if (arg.def)
                        foundDef = true;                    
                    max++;
                    if (!foundDef)
                        min++;
                }                
                
                else {
                    spec.push(arg);
                    continue;
                }                                            
            }
            fun.specArgs = spec;    // helper arguments of the c++ function (ex,ctx,pthis)
            fun.scrArgs = scr;      // arguments of the js method
            fun.min = min;          // min argc for the js method
            fun.max = max;          // max argc for the js method
        };
        // compares two array of overloads
        function funListEqual(funs1, funs2) {
            diff = false;
            if (funs1.length == funs2.length) {
                for (var j=0; j<funs1.length; j++) 
                    if (funs1[j] != funs2[j]) {
                        diff = true;
                        break;        
                    }
			}		
			else 
				diff = true;		
            return !diff;        
        };
        // grouping overloads based on their possible argument counts
        // example: with 1 arg overload1 and overload2 can be called,
        //          with 2-3 args only overload2 can be called, etc...
        function argcPartition(funs) {
            var valid = {}, ret = [],
                group;
            
            for (var f=0; f<funs.length; f++)
                for (var i=funs[f].min; i<funs[f].max+1; i++)
                    (valid[i] ? valid[i] : valid[i]=[]).push(funs[f]);                                       
                
            for (var i in valid) {
                if (group && funListEqual(group, valid[i])) {
                    ret[ret.length-1].max = i;                        
                    continue;
                }
                
                else {                
                    group = valid[i];
                    ret.push({
                        fun : group,
                        min : i,
                        max : i
                    });
                    continue;    
                }                                                    
            }
            
            return ret;
        };
        // grouping overloads based on the type of the 'index'-th script argument
        // example: index = 2
        //          if the type (=t) of 2nd script argument is 
        //          TYPE_NULL <= t >=TYPE_INT32 onverload1 must be called 
        //          else either overload2 or overload3 can be called check the 3rd arg
        function typePartition(funs, index) {
            var i,blocks=[],type,sorted = {si:{}},isSi,si,last=-1;
            
            for (i=0; i<funs.length; i++) {
                type = funs[i].scrArgs[index].varType;
                si = funs[i].scrArgs[index].si;
                
                if (type && !si)
                    (sorted[type] ? sorted[type] : sorted[type] = []).push(funs[i]);
                else if (type){
                    (sorted.si[si] ? sorted.si[si] : sorted.si[si] = []).push(funs[i]);
                } else {
                    Reporter.error('generator: type partition failed: ',type,'\n');
                }
            }                                                
                            
            for (i in Types) {
                type = Types[i];                            
                
                if (!sorted[type])
                    continue;
                
                if (type == 'STR') {
                    // stupid exception case, dont know how to get rid of it yet...
                    ++i;
                } 
                
                blocks.push({
                    fun : sorted[type],
                    min : Types[last+1], 
                    max : Types[i]
                });                               
                
                last = i*1;                                                
            }

            if (blocks.length)
                blocks[blocks.length-1].max = Types[Types.length-1];              
            
            for (si in sorted.si) {
                blocks.push({
                    fun : sorted.si[si],
                    si : si  
                });
                continue;
            }
            
            return blocks;
        };
        function recursiveGen(blocks, ws, t, level) {
            var i,j,si,siVar,funs,b,toCheck = 'argc', min, max, index = 0;
            with(Generator) {                        
                if (level) {
                    toCheck = 'type' + (level-1);
                    t.push(ws, 'Var::Type ',toCheck,' = argv[',level-1,
                        '].type();\n');

                    for (i=0; i<blocks.length; i++) {                        
                        if (si = blocks[i].si) {
                            siVar = si.toLowerCase(),'_', level-1;                            
                            
                            t.push(ws, index++ ? 'else if(' : 'if (' ,si, ' ', siVar,
                                ' = ', si, '(argv[', level-1, '].toScr())',') {\n');                                                            
                        
                            funs = blocks[i].fun;
                            if (funs.length == 1) {
                                genBlockForTrait(funs[0],t,ws + '   ');
                                t.push('\n');
                            } 
                            else {
                                recursiveGen(typePartition(funs, level),
                                    ws + '   ', t, level+1);
                            } 
                            t.push(ws, '   return ex;\n')
                            t.push(ws, '}\n');
                        }    
                    }
                } 
                
                for (i=0; i<blocks.length; i++) {
                    b = blocks[i];
                    if (b.si)
                        continue; // already handled
                    
                    min = level ? "Var::TYPE_" + b.min : b.min;
                    max = level ? "Var::TYPE_" + b.max : b.max;
                    t.push(ws, index++ ? 'else if(' : 'if (');
                    if (b.min == b.max) {
                        t.push(toCheck, '==', min, ') {\n');
                    }
                    else {
                        t.push(min, ' <= ', toCheck, 
                            ' && ' , max,' >= ', toCheck, ') {\n');                        
                    }
                    
                    if (b.fun.length == 1) {
                        genBlockForTrait(b.fun[0],t,ws + '   ');
                        t.push('\n');
                    }
                    else {
                        recursiveGen(typePartition(b.fun, level),
                                ws + '   ', t, level+1);
                    }
                    
                    t.push(ws,'}\n');                        
                }
				
				if (blocks.length)
					t.push(ws,'else{\n',ws,'   return o3_new(cEx)(', 
						level ? '"Invalid argument type."' : '"Invalid argument count."',
						');\n', ws,  '}\n');
            }    
        };
        
        for (var i=0; i<overloads.length; i++)    
            classifyArgs(overloads[i]);
            
        return recursiveGen(argcPartition(overloads), ws, t, 0);                
    }
}; // generator

var ArgInfo = {
    'bool'              : {fetch:'toBool()',    type:'BOOL'},
    'int'               : {fetch:'toInt32()',   type:'INT32'},
    'int32_t'           : {fetch:'toInt32()',   type:'INT32'},
    'size_t'            : {fetch:'toInt32()',   type:'INT32'},
    'int64_t'           : {fetch:'toInt64()',   type:'INT64'},
    'double'            : {fetch:'toDouble()',  type:'DOUBLE'},
    'const char *'       : {fetch:'toStr()',     type:'STR'},
    'const Str &'       : {fetch:'toStr()',     type:'STR'},
    'const wchar_t *'   : {fetch:'toWStr()',    type:'WSTR'},
	'const Var &'		: {fetch:'', 			type:'VAR', 
		direct: true},
    'Var *'				: {fetch:'argv,argc',	arglist: true},
	'const WStr &'      : {fetch:'toWStr()',    type:'WSTR'},
    'const Buf &'       : {fetch:'toScr()',     type:'SCR', 
        wrap : 'siBuf', wrap2 : 'Buf'},
    'iScr *'            : {fetch:'toScr()',     type:'SCR'},
    'iCtx *'            : {fetch:'ctx'},
    'siEx *'            : {fetch:'&ex'},

    si : function(si) {
        if (si.match(/i[\w]+ \*/)) 
            return {
                fetch : 'toScr()',
                wrap : 's' + si.substring(0,si.indexOf('*')),
                type : 'SCR'
            }
        else
            Reporter.error('generator: unknown arg type: ',si,'\n');
        return {};    
    }    
};

var Types = [
      'VOID',
      'NULL',
      'BOOL',
      'INT32',
      'INT64',
      'DOUBLE',
      'STR',
      'WSTR',
      'SCR'           
];   

var Enums = {};

Reporter.immediate = immLog;
Reporter.logWriter = console.log;
Reporter.errorWriter = console.log;

// by default it generates all glue in ./include
if (files.length == 0) {
	FileHandler.scanFiles('include');
	FileHandler.scanFiles('modules');	
}

// if there were files/folders specified, let's traverse them

for (i=0; i<files.length; i++) {    
	FileHandler.scanFiles(files[i]);
} 

// if -l was specified let's log to stdout 
// (if -i was specified then it is already logged out at this point no need to log it again)
if (!Reporter.immediate && log) 
    Reporter.dumpLogs();

// in any case, let's save the logs and errors into files
Reporter.logWriter = fileWriter(logFileName);
Reporter.errorWriter = fileWriter(errFileName);    
Reporter.dumpLogs();
Reporter.dumpErrors();

