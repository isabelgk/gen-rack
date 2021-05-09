const maxApi = require('max-api');
const fs = require('fs');

maxApi.addHandlers({
    // Write the .cpp file
    create: async (name) => {
        let basedir = process.cwd() + '/../../../';  //  =>  /gen-rack

        // Replace GEN_RACK_DISPLAY_NAME with all lowercase name
        // Replace GEN_RACK_NAME with name with the first letter capitalized
        fs.readFile(basedir + 'src/modules/module.in', 'utf8', function (err, data) {
            if (err) {
                maxApi.post('Error reading module.in.');
                return;
            }
            let result = data.split('GEN_RACK_DISPLAY_NAME').join(name);
            result = result.split('GEN_RACK_NAME').join(name.charAt(0).toUpperCase() + name.slice(1));

            fs.writeFile(basedir + 'src/modules/' + name + '.cpp', result, 'utf8', function (err) {
                if (err) {
                    maxApi.post('Error writing .cpp');
                }
            });
        });

        let pluginFile = basedir + 'src/plugin.cpp';
        // line before end:
        // 'p->addModel(model' + name.charAt(0).toUpperCase() + name.slice(1) + ');';

        let headerFile = basedir + 'src/plugin.hpp';
        // 'extern Model* model' + name.charAt(0).toUpperCase() + name.slice(1) + ';';
        fs.appendFileSync(headerFile, 'extern Model* model' + name.charAt(0).toUpperCase() + name.slice(1) + ';\n');
    },
    
});