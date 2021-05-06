const maxApi = require('max-api');
const fs = require('fs');

maxApi.addHandlers({
    // Write the .cpp file
    create: async (name) => {
        let basedir = process.cwd() + '/../../../';  //  =>  /gen-rack

        // Replace GEN_RACK_DISPLAY_NAME with all lowercase name
        // Replace GEN_RACK_NAME with name with the first letter capitalized
        fs.readFile(basedir + 'src/module.in', 'utf8', function (err, data) {
            if (err) {
                maxApi.post('Error reading module.in.');
                return;
            }
            let result = data.split('GEN_RACK_DISPLAY_NAME').join(name.toLowerCase());
            result = result.split('GEN_RACK_NAME').join(name.charAt(0).toUpperCase() + name.slice(1));

            fs.writeFile(basedir + 'src/' + name + '.cpp', result, 'utf8', function (err) {
                if (err) {
                    maxApi.post('Error writing .cpp');
                }
            })
        });
        // TODO - Add to plugin.cpp, plugin.hpp, and plugin.json
    },
    
    // TODO - Run `make install`
    build: async (name) => {
    
    }
});