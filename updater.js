let allow_beta_update = configs['allow_beta_update'];
let exitFlag = false;
let resp = httpget("https://api.github.com/repos/qr243vbi/nekobox/releases");

if (!resp.error){
    let array = JSON.parse(resp.text);
    for (let release of array){
        if (!allow_beta_update) {
            if (release["prerelease"]){
                continue;
            }
        }

        for (let asset of release["assets"]) {
            let asset_name = asset["name"];
            if (asset_name.includes(search) && asset_name.endsWith(".zip")) {
                note_pre_release = release["prerelease"] ? " (Pre-release)" : "";
                release_url = release["html_url"];
                release_note = release["body"];
                assets_version = asset["name"];
                release_download_url = asset["browser_download_url"];
                exitFlag = true;
                break;
            }
        }
        if (exitFlag) break;
    }
}

