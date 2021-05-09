const maxApi = require('max-api');
const exec = require('child_process');

maxApi.addHandlers({
    // copy and rename /gen/projects/example
    newProject: async (name) => {
        // let cmd = 'cd .. && mkdir ' + name + ' && sleep 1 && cp -R example ' + name;
        let cmd = 'cp -R projects/template projects/' + name;
        exec.exec(cmd, (error, stdout, stderr) => {
            if (error) {
                maxApi.post(error);
            }
            if (stderr) {
                maxApi.post(stderr);
            }
        });
    }
});