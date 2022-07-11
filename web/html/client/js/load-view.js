
function humanFileSize(size) {
    if (size === 0)
        return '0B';
    const i = Math.floor(Math.log(size) / Math.log(1024));
    return (size / Math.pow(1024, i)).toFixed(2) * 1 + ' ' + ['B', 'kB', 'MB', 'GB', 'TB'][i];
}

const solveType = new Map();
solveType.set('zip', 'archive');
solveType.set('rar', 'archive');
solveType.set('tar.bz', 'archive');
solveType.set('html', 'html5');
solveType.set('c', 'code');
solveType.set('cpp', 'code');

function returnButton(length, return_name) {
    let element = document.createElement('button');
    element.id = return_name;
    if (length > 1) {
        element.className = "btn file-item";
    } else {
        element.className = "btn disabled file-item";
    }

    let icon_span = document.createElement('span');
    icon_span.className = "file-item-icon file-item-level-up fas fa-level-up-alt text-secondary";
    element.appendChild(icon_span);

    let name_span = document.createElement('span');
    name_span.className = "file-item-name";
    name_span.innerText = "..";
    element.appendChild(name_span);

    return element;
}

function createFile(pre, file) {
    let element = document.createElement('button');
    element.id = String(pre + file.ufile_id);
    element.className = "btn file-item";
    element.title = "Name: " + file.name + "\nTime: " + file.upload_time + "\nSize: " + humanFileSize(file.size);

    let filetype_array = file.name.split('.');
    let filetype = filetype_array[filetype_array.length - 1];
    if (solveType.has(filetype)) {
        filetype = solveType.get(filetype);
    }

    let icon_span = document.createElement('span');
    if (!file.is_dir && (filetype == 'html5' || filetype == 'js' || filetype == 'php')) {
        icon_span.className = "file-item-icon text-secondary fab" + (file.is_dir ? " fa-folder" : (" fa-file fa-file-" + filetype + " fa-" + filetype));
    } else {
        icon_span.className = "file-item-icon text-secondary far" + (file.is_dir ? " fa-folder" : (" fa-file fa-file-" + filetype + " fa-" + filetype));
    }
    element.appendChild(icon_span);

    let name_span = document.createElement('span');
    name_span.className = "file-item-name";
    name_span.innerText = file.name;
    element.appendChild(name_span);
    return element;
}

function getData(path_array = globalPath, manager_name = "file-manager", menu_name = "bread-menu", return_name = "return-button", file_name = "file") {
    if (path_array.length <= 0) {
        return;
    }
    $.ajax({
        type: 'POST',
        url: 'data.php',
        data: {
            op: 10,
            par0: path_array[path_array.length - 1].ufile_id
        },
        responseType: "arraybuffer",
        success: function (data) {
            let file_array = JSON.parse(data);
            loadFile(manager_name, file_array, path_array, return_name, file_name);
            loadMenu(path_array, manager_name, menu_name, return_name, file_name);
            for (let i in file_array) {
                if (file_array[i].is_dir) {
                    document.getElementById(String(file_name + file_array[i].ufile_id)).addEventListener('click', function () {
                        path_array.push({ name: file_array[i].name, ufile_id: file_array[i].ufile_id });
                        getData(path_array, manager_name, menu_name, return_name, file_name);
                    }, true);
                }
            }
            document.getElementById(return_name).addEventListener('click', function () {
                path_array.pop();
                getData(path_array, manager_name, menu_name, return_name, file_name);
            }, true);
        }
    });
}

function upPost(op, par = []) {
    let data = { "op": op };
    for (let i in par) {
        data["par" + i] = par[i];
    }
    $.ajax({
        type: 'POST',
        url: 'data.php',
        data: data,
        responseType: "arraybuffer",
        success: function (data) {
            let res = JSON.parse(data);
            if (res['ret_code'] != 0) {
                let errMsg = '';
                switch (op) {
                    case '13':
                        errMsg = "Cannot copy to the current folder itself or its subdirectories, please try again in another location.";
                        break;
                    case '14':
                        errMsg = "Cannot copy to the current folder itself or its subdirectories, please try again in another location.";
                        break;
                }
                showToast('Error', errMsg, true);
            }
            getData();
        },
    });
}

function loadMenu(path_array, manager_name, menu_name, return_name, file_name) {
    const menu = document.getElementById(menu_name);
    menu.innerHTML = '';
    for (let i in path_array) {
        let item = document.createElement('li');
        item.className = "breadcrumb-item";
        let link = document.createElement('a');
        link.href = '#';
        link.innerText = path_array[i].name;
        link.addEventListener('click', function () {
            while (path_array[path_array.length - 1].name != link.innerText) {
                path_array.pop();
            }
            getData(path_array, manager_name, menu_name, return_name, file_name);
        });
        item.appendChild(link);
        menu.appendChild(item);
    }
}

function loadFile(manager_name, file_array, path_array, return_name, file_name) {
    const manager = document.getElementById(manager_name);
    manager.innerHTML = '';
    manager.appendChild(returnButton(path_array.length, return_name));
    for (let i in file_array) {
        manager.appendChild(createFile(file_name, file_array[i]));
    }
    for (let i = 1; i <= 8; i += 1) {
        manager.appendChild(document.createElement('i'));
    }
}

getData();

ipcRenderer.on('newDir', (event, path, fid, dirname) => {
    console.log(`newDir ${path}, ${fid}, ${dirname}`);
    $.ajax({
        type: 'POST',
        url: 'data.php',
        data: {
            op: 11,
            par0: fid,
            par1: dirname
        },
        responseType: "arraybuffer",
        success: function (data) {
            let res = JSON.parse(data);
            if (res['ret_code'] == 0) {
                ipcRenderer.send('newDir', path, res['ufile_id']);
            }
            getData();
        },
    });
});

ipcRenderer.on('uploadFile', (event, fid, data, filename, path) => {
    console.log(`dirUploadFile ${fid}, ${filename}`);
    uploadMD5Check(fid, new File([data], filename), path);
});

$('#upload').off().on('click', function () {
    $('#uploadFile').val('');
    document.getElementById("uploadFile").click();
});

$('#upload-dir').off().on('click', function () {
    ipcRenderer.send('openDialog', globalPath[globalPath.length - 1].ufile_id);
});

$('#refresh').off().on('click', function () {
    getData();
});

$('#newFolder').off().on('click', function () {
    document.getElementById('smallTitle').innerText = "New Folder";
    document.getElementById('smallButton').innerText = "OK";
    document.getElementById('smallInput').value = "";
    $('#smallButton').off().on('click', function () {
        upPost("11", [globalPath[globalPath.length - 1].ufile_id, document.getElementById('smallInput').value]);
        $("#smallModal").modal('hide');
    });
    $("#smallModal").modal('show');
});

$('#uploadFile').off().on('change', function () {
    showToast('Uploading', 'Checking md5...', false);
    let file = document.getElementById("uploadFile").files[0];
    curdir_id = globalPath[globalPath.length - 1].ufile_id;
    uploadMD5Check(curdir_id, file);
});

function uploadMD5Check(curdir_id, file, path = '') {
    const chunkSize = 1024 * 1024;
    const fileReader = new FileReader();
    const md5 = new SparkMD5();
    let index = 0;
    const loadFile = function () {
        const slice = file.slice(index, index + chunkSize);
        fileReader.readAsBinaryString(slice);
    }
    fileReader.onload = function (e) {
        md5.appendBinary(e.target.result);
        if (index < file.size) {
            index += chunkSize;
            loadFile();
        } else {
            let lastMd5 = md5.end();
            ipcRenderer.send('set-upload', {
                ufile_id: curdir_id,
                md5: lastMd5,
                ctn: true,
                lstTime: Date.now(),
                progress: 0,
                name: file.name,
                path: path == '' ? file.path : path
            });
            console.log(file.path);
            checkUploadFile(curdir_id, file, lastMd5);
        }
    };
    loadFile();
}

function checkUploadFile(curdir_id, file, md5) {
    $.ajax({
        type: 'POST',
        url: 'data.php',
        data: {
            op: 21,
            par0: md5,
            par1: file.size
        },
        responseType: "arraybuffer",
        success: function (data) {
            let res = JSON.parse(data);
            if (res['ret_code'] == 0) {
                showToast("Success", "Upload success in an instant.", true);
                touchFile(curdir_id, file, md5);
            } else {
                let parBar = document.createElement('div');
                parBar.className = "progress";
                let bar = document.createElement('div');
                bar.className = "progress-bar";
                bar.ariaRoleDescription = "progressbar";
                bar.id = "bar" + md5;
                parBar.appendChild(bar);

                let toastLive = document.getElementById('liveToast');
                document.getElementById('liveToast').setAttribute('data-bs-autohide', false);
                document.getElementById('toast-title').innerText = 'Uploading';
                document.getElementById('toast-content').innerHTML = '';
                document.getElementById('toast-content').appendChild(parBar);

                let toast = new bootstrap.Toast(toastLive);
                toast.id = "toast" + md5;
                toast.show();
                uploadFile(curdir_id, file, md5, res);
            }
        },
    });
}

ipcRenderer.on('upload-slice', (event, item, file) => {
    checkUploadFile(item.ufile_id, new File([file], item.name), item.md5);
});

function touchFile(curdir_id, file, md5) {
    $.ajax({
        type: 'POST',
        url: 'data.php',
        data: {
            op: 12,
            par0: curdir_id,
            par1: file.name,
            par2: md5
        },
        responseType: "arraybuffer",
        success: function (data) {
            let res = JSON.parse(data);
            if (res['ret_code'] != 0) {
                showToast("Error", "Upload error. Please try again.", true);
            }
            getData();
            ipcRenderer.send('upload-finish', {
                ufile_id: curdir_id,
                md5: md5
            })
        },
    });
}

function uploadFile(curdir_id, file, md5, sliceData) {
    let chunk = file.slice(sliceData['start'], sliceData['start'] + sliceData['slice_size']);
    let sliceName = md5 + '_' + sliceData['slice_id'];
    let formData = new FormData();
    formData.append("op", 22);
    formData.append("file", chunk, sliceName);
    formData.append("par0", md5);
    formData.append("par1", sliceData['slice_id']);
    $.ajax({
        type: 'POST',
        url: 'data.php',
        data: formData,
        contentType: false,
        processData: false,
        responseType: "arraybuffer"
    }).done(function (data) {
        let res = JSON.parse(data);
        if (res['ret_code'] == 0) {
            showToast('Success', 'Upload success.', true);
            touchFile(curdir_id, file, md5);
        } else {
            let progress = res['progress'];
            let bar = document.getElementById("bar" + md5);
            bar.style = `width: ${progress * 100}%`;
            bar.innerText = `${(progress * 100).toFixed(1)}%`;
            let par = ipcRenderer.sendSync('get-upload', {
                ufile_id: curdir_id,
                md5: md5,
                progress: progress
            });
            if (par == false) {
                return;
            }
            uploadFile(curdir_id, file, md5, res);
        }
    });
}

function getDownloadFile(ufile_id, op = 17) {
    let res;
    $.ajax({
        type: 'POST',
        url: 'data.php',
        data: {
            op: op,
            par0: ufile_id
        },
        responseType: "arraybuffer",
        async: false,
        success: function (data) {
            res = data;
        },
    });
    return res;
}

function showToast(title, content, autohide) {
    toastLive = document.getElementById('liveToast');
    document.getElementById('toast-title').innerText = title;
    document.getElementById('liveToast').setAttribute('data-bs-autohide', autohide);
    document.getElementById('toast-content').innerText = content;
    toast = new bootstrap.Toast(toastLive);
    toast.show();
}

ipcRenderer.on('download-slice', (event, md5, i) => {
    console.log('download-slice');
    let data = new FormData();
    data.append('op', 25);
    data.append('par0', md5);
    data.append('par1', i);
    let xhr = new XMLHttpRequest();
    xhr.open("POST", 'data.php', true);
    xhr.responseType = "arraybuffer";
    xhr.withCredentials = true;
    xhr.onload = function () {
        console.log('download-slice');
        ipcRenderer.send('handle-slice', md5, i, xhr.response);
    };
    xhr.send(data);
});


function downloadCtn(ufile_id) {
    console.log('click-download');
    const fileData = getDownloadFile(ufile_id);
    const chunks = fileData['slice_num'];
    const md5 = fileData['md5'];
    let path = getDownloadFile(ufile_id, 18);
    ipcRenderer.send('startDownload', path);
}

ipcRenderer.on('download-finish', (event) => {
    showToast("Success", "Download success.", true);
});