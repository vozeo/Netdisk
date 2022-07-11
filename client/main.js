const { app, BrowserWindow, ipcMain, Menu, dialog, session } = require('electron');
const path = require('path');
const fs = require('fs');
const crypto = require('crypto');

app.commandLine.appendSwitch('disable-web-security');

let globalPath = [];
let username;
let mainWindow, uploadWindow, downloadWindow;

function createWindow() {
    mainWindow = new BrowserWindow({
        width: 1024,
        height: 768,
        title: '',
        autoHideMenuBar: true,
        transparent: true,
        show: false,
        webPreferences: {
            nodeIntegration: true,
            contextIsolation: false,
            preload: path.join(__dirname, 'preload.js')
        }
    });
    mainWindow.loadURL('http://124.221.51.35/index.html');
    //mainWindow.webContents.openDevTools();
    mainWindow.on('ready-to-show', function () {
        mainWindow.show();
    });
    mainWindow.webContents.on('did-fail-load', () => {
        mainWindow.loadFile('fail.html');
    });
    mainWindow.on('closed', function () {
        mainWindow = null;
    });
}

const downloadTemp = 'C:\\NetDiskDownloadTemp\\';
let downloadQueue = [];
let downloadProgress = {};
let uploadQueue = [];

app.whenReady().then(() => {
    createWindow();
    app.on('activate', function () {
        if (BrowserWindow.getAllWindows().length === 0) {
            createWindow()
        }
    });
    ipcMain.on('setPath', (event, arg, name) => {
        globalPath = arg;
        username = name;
        downloadQueue = loadDownloadQueue();
        uploadQueue = loadUploadQueue();
        event.returnValue = '';
    });
    ipcMain.on('getPath', (event, arg) => {
        event.returnValue = globalPath;
    });
    ipcMain.on('getUsername', (event) => {
        event.returnValue = username;
    });
    ipcMain.on('show-context-menu', (event) => {
        const template = [
            {
                label: 'Copy',
                click: () => { event.sender.send('context-menu-command', 'menu-item-1') }
            },
            {
                label: 'Move',
                click: () => { event.sender.send('context-menu-command', 'menu-item-2') }
            },
            {
                label: 'Rename',
                click: () => { event.sender.send('context-menu-command', 'menu-item-3') }
            },
            {
                label: 'Download',
                click: () => { event.sender.send('context-menu-command', 'menu-item-4') }
            },
            { type: 'separator' },
            {
                label: 'Delete',
                click: () => { event.sender.send('context-menu-command', 'menu-item-5') }
            },
        ]
        const menu = Menu.buildFromTemplate(template)
        menu.popup(BrowserWindow.fromWebContents(event.sender))
    });
    ipcMain.on('openDialog', (event, fid) => {
        dialog.showOpenDialog({
            properties: ['openDirectory']
        }).then(result => {
            let dirname = result.filePaths[0].split('\\');
            //console.log(dirname)
            event.reply('newDir', result.filePaths[0], fid, dirname[dirname.length - 1]);
        });
    });
    ipcMain.on('newDir', (event, path, fid) => {
        let subFiles = fs.readdirSync(path);
        //console.log(subFiles);
        for (let i in subFiles) {
            let file = path + '\\' + subFiles[i];
            fs.stat(file, (err, stats) => {
                if (stats.isDirectory()) {
                    event.reply('newDir', file, fid, subFiles[i]);
                } else {
                    fs.readFile(file, (err, data) => {
                        event.reply('uploadFile', fid, data, subFiles[i], file);
                    });
                }
            });
        }
    });
    ipcMain.on('startDownload', (event, downloadJson) => {
        dialog.showOpenDialog({
            properties: ['openDirectory']
        }).then(result => {
            initDownloadQueue(result.filePaths[0], [JSON.parse(downloadJson)]);
            //console.log(downloadQueue);
            initDownloadProgress();
            saveDownloadQueue();
            if (downloadWindow) {
                downloadWindow.webContents.send('init-download-list', downloadQueue, downloadProgress);
            }
            let par = getNextSlice();
            event.reply('download-slice', par[0], par[1]);
        });
    });
    ipcMain.on('handle-slice', (event, md5, i, data) => {
        //console.log(`slice ${md5} ${i}`);
        if (md5 === undefined || i === undefined) {
            return;
        }
        fs.writeFileSync(downloadTemp + md5 + '_' + i, new Uint8Array(data));
        if (downloadWindow) {
            downloadWindow.webContents.send('update-download', downloadQueue, downloadProgress, md5);
        }
        if (downloadQueue.length >= 1) {
            let par = getNextSlice();
            if (par[0] !== undefined && par[1] !== undefined) {
                event.reply('download-slice', par[0], par[1]);
            }
        }
    });
    ipcMain.on('set-upload', (event, item) => {
        uploadQueue.push(item);
        saveUploadQueue();
        if (uploadWindow) {
            uploadWindow.webContents.send('init-upload-list', uploadQueue);
        }
    });
    ipcMain.on('upload-finish', (event, item) => {
        for (let i = uploadQueue.length - 1; i >= 0; i -= 1) {
            if (uploadQueue[i].md5 == item.md5 && uploadQueue[i].ufile_id == item.ufile_id) {
                uploadQueue.splice(i, 1);
            }
        }
        if (uploadWindow) {
            uploadWindow.webContents.send('init-upload-list', uploadQueue);
        }
        saveUploadQueue();
    });
    ipcMain.on('get-upload', (event, item) => {
        let ret = false;
        //console.log(item);
        for (i of uploadQueue) {
            if (i.md5 == item.md5 && i.ufile_id == item.ufile_id) {
                ret = i.ctn;
                i.progress = item.progress;
                break;
            }
        }
        if (uploadWindow) {
            uploadWindow.webContents.send('update-upload', uploadQueue);
        }
        event.returnValue = ret;
    });
    ipcMain.on('update-upload-last', (event, queue) => {
        uploadQueue = queue;
    });
    ipcMain.on('change-upload', (event, idx, arg) => {
        for (let item of uploadQueue) {
            if (item.md5 == idx.md5 && item.ufile_id == idx.ufile_id) {
                item.ctn = arg;
            }
        }
        if (arg == true) {
            let file = fs.readFileSync(idx.path);
            mainWindow.webContents.send('upload-slice', idx, file);
        }
    });
    ipcMain.on('update-download-last', (event, queue) => {
        downloadQueue = queue;
    });
    ipcMain.on('change-download', (event, md5, arg) => {
        for (let item of downloadQueue) {
            if (item.md5 == md5) {
                item.ctn = arg;
            }
        }
        if (arg == true) {
            let par = getNextSlice();
            mainWindow.webContents.send('download-slice', par[0], par[1]);
        }
    });
    ipcMain.on('upload-window', (event) => {
        uploadWindow = new BrowserWindow({
            width: 1024,
            height: 768,
            title: '',
            autoHideMenuBar: true,
            transparent: true,
            show: false,
            webPreferences: {
                nodeIntegration: true,
                contextIsolation: false,
                preload: path.join(__dirname, 'preload.js')
            }
        });
        uploadWindow.loadURL('http://124.221.51.35/upload.html');
        //uploadWindow.webContents.openDevTools();
        uploadWindow.on('ready-to-show', function () {
            uploadWindow.show();
            uploadWindow.webContents.send('init-upload-list', uploadQueue);
        });
        uploadWindow.webContents.on('did-fail-load', () => {
            uploadWindow.loadFile('fail.html');
        });
        uploadWindow.on('closed', function () {
            uploadWindow = null;
        });
    });
    ipcMain.on('download-window', (event) => {
        downloadWindow = new BrowserWindow({
            width: 1024,
            height: 768,
            title: '',
            autoHideMenuBar: true,
            transparent: true,
            show: false,
            webPreferences: {
                nodeIntegration: true,
                contextIsolation: false,
                preload: path.join(__dirname, 'preload.js')
            }
        });
        downloadWindow.loadURL('http://124.221.51.35/download.html');
        //downloadWindow.webContents.openDevTools();
        downloadWindow.on('ready-to-show', function () {
            downloadWindow.show();
            initDownloadProgress();
            downloadWindow.webContents.send('init-download-list', downloadQueue, downloadProgress);
        });
        downloadWindow.webContents.on('did-fail-load', () => {
            downloadWindow.loadFile('fail.html');
        });
        downloadWindow.on('closed', function () {
            downloadWindow = null;
        });
    });
});

function initDownloadQueue(path, downloadList) {
    for (let i in downloadList) {
        if ('subs' in downloadList[i]) {
            if (!fs.existsSync(path)) {
                fs.mkdirSync(path);
            }
            initDownloadQueue(path + '\\' + downloadList[i].name, downloadList[i].subs);
        } else {
            let file = path + '\\' + downloadList[i].name;
            let md5 = '';
            if (fs.existsSync(file)) {
                const buffer = fs.readFileSync(file);
                const hash = crypto.createHash('md5');
                hash.update(buffer);
                md5 = hash.digest('hex');
            }
            let res = 0;
            if (md5 == downloadList[i].md5) {
                res = dialog.showMessageBoxSync(mainWindow, {
                    type: "info",
                    buttons: ["OK", "NO"],
                    title: "Hint",
                    message: "Duplicate filename and different MD5 detected, download and overwrite?",
                    defaultId: 0,
                    cancelId: 1
                });
            }
            if (res === 0) {
                downloadQueue.push({
                    path: file,
                    md5: downloadList[i].md5,
                    chunks: downloadList[i].slice_num,
                    ctn: true,
                    lstTime: Date.now()
                });
            }
        }
    }
}

function saveDownloadQueue() {
    let path = downloadTemp + username + '\\';
    if (!fs.existsSync(downloadTemp)) {
        fs.mkdirSync(downloadTemp);
    }
    if (!fs.existsSync(path)) {
        fs.mkdirSync(path);
    }
    fs.writeFileSync(path + 'DONOTDELETE!', JSON.stringify(downloadQueue));
}

function saveUploadQueue() {
    let path = downloadTemp + username + '\\';
    if (!fs.existsSync(downloadTemp)) {
        fs.mkdirSync(downloadTemp);
    }
    if (!fs.existsSync(path)) {
        fs.mkdirSync(path);
    }
    fs.writeFileSync(path + 'PLEASE!', JSON.stringify(uploadQueue));
}

function loadUploadQueue() {
    let path = downloadTemp + username + '\\' + 'PLEASE!';
    if (!fs.existsSync(path)) {
        return JSON.parse('[]');
    }
    let queue = fs.readFileSync(path);
    let ret = JSON.parse(queue);
    for (let item of ret) {
        item.ctn = false;
    }
    return ret;
}

function loadDownloadQueue() {
    let path = downloadTemp + username + '\\' + 'DONOTDELETE!';
    if (!fs.existsSync(path)) {
        return JSON.parse('[]');
    }
    let queue = fs.readFileSync(path);
    let ret = JSON.parse(queue);
    for (let item of ret) {
        item.ctn = false;
    }
    return ret;
}

function initDownloadProgress() {
    downloadProgress = {};
    for (let i = downloadQueue.length - 1; i >= 0; i -= 1) {
        let notList = [];
        for (let j = 1; j <= downloadQueue[i].chunks; j += 1) {
            if (!fs.existsSync(downloadTemp + downloadQueue[i].md5 + '_' + j.toString())) {
                notList.push(j);
            }
        }
        if (notList.length > 0) {
            downloadProgress[downloadQueue[i].md5] = notList;
        }
    }
}

function mkdirsSync(dirname) {
    if (fs.existsSync(dirname)) {
        return true;
    } else {
        if (mkdirsSync(path.dirname(dirname))) {
            fs.mkdirSync(dirname);
            return true;
        }
    }
}

function saveFile(file) {
    let notList = [];
    for (let i = 1; i <= file.chunks; i += 1) {
        if (!fs.existsSync(downloadTemp + file.md5 + '_' + i.toString())) {
            notList.push(i);
        }
    }
    if (notList.length > 0) {
        downloadQueue.unshift(file);
        downloadProgress[file.md5] = notList;
    } else {
        if (fs.existsSync(file.path)) {
            fs.unlinkSync(file.path);
        } else {
            mkdirsSync(path.dirname(file.path));
        }
        let fid = fs.openSync(file.path, 'w');
        fs.closeSync(fid);
        for (let i = 1; i <= file.chunks; i += 1) {
            let data = fs.readFileSync(downloadTemp + file.md5 + '_' + i.toString());
            mkdirsSync(path.dirname(file.path));
            fs.appendFileSync(file.path, data);
        }
        if (downloadWindow) {
            downloadWindow.webContents.send('init-download-list', downloadQueue, downloadProgress);
        }
        mainWindow.webContents.send('download-finish');
    }
}

function getNextSlice() {
    let dstMd5, dstI;
    for (let i = downloadQueue.length - 1; i >= 0; i -= 1) {
        if (downloadQueue[i].ctn == true && downloadProgress[downloadQueue[i].md5] && downloadProgress[downloadQueue[i].md5].length >= 1) {
            for (let j in downloadProgress[downloadQueue[i].md5]) {
                dstMd5 = downloadQueue[i].md5, dstI = downloadProgress[downloadQueue[i].md5][j];
                downloadProgress[downloadQueue[i].md5].splice(j, 1);
                break;
            }
        } else {
            let file = downloadQueue[i];
            if (downloadProgress[downloadQueue[i].md5]) {
                delete downloadProgress[downloadQueue[i].md5];
            }
            downloadQueue.splice(i, 1);
            saveFile(file);
            saveDownloadQueue();
        }
        if (dstMd5 && dstI) {
            if (downloadProgress[downloadQueue[i].md5] && downloadProgress[downloadQueue[i].md5].length <= 0) {
                let file = downloadQueue[i];
                delete downloadProgress[downloadQueue[i].md5];
                downloadQueue.splice(i, 1);
                saveFile(file);
                saveDownloadQueue();
            }
            break;
        }
    }
    //console.log(`Nextslice ${dstMd5} ${dstI}`);
    return [dstMd5, dstI];
}

app.on('window-all-closed', function () {
    if (process.platform !== 'darwin') {
        app.quit();
    }
});
