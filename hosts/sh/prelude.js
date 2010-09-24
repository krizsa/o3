o3.require('fs');
o3.require('console');

function include(file, includeTrace) {	
	var scriptFolder = o3.cwd;    
	
	if (includeTrace)
        o3.print('include: ' + file + '\n');
        
	data = scriptFolder.get(file).data;
	if (!data.length && includeTrace)
        o3.print('open file failed!');
    else        
        eval(data);        
    
    if (includeTrace)    
        o3.print('include finished: ' + file + '\n');
}

this.include = include;