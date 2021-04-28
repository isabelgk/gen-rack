inlets = 1;
outlets = 2;
autowatch = 1;

function name() {
    n = arguments[0];
    outlet(0, n);
}

function path() {
    loc = arguments[0];
    lSplit = loc.split("/");
    
    result = "";
    for (var i = 0; i < lSplit.length; i++) {
        if (lSplit[i] === "projects") {
            result += "/exports/";
            break;
        }
        else {
            if (i !== 0) {
                result += "/";
            }
            result += lSplit[i];
        }
    }

    post(result);

    outlet(1, result);
}