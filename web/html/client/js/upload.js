const { ipcRenderer } = require('electron');

function humanFileSize(size) {
    if (size === 0)
        return '0B';
    const i = Math.floor(Math.log(size) / Math.log(1024));
    return (size / Math.pow(1024, i)).toFixed(2) * 1 + ' ' + ['B', 'kB', 'MB', 'GB', 'TB'][i];
}

ipcRenderer.on('init-upload-list', (event, queue) => {
    console.log(queue);
    let group = document.createElement('div');
    group.className = "list-group";
    for (item of queue) {
        let groupItem = document.createElement('div');
        groupItem.className = 'list-group-item';
        let title = document.createElement('h5');
        title.className = 'm-3';

        let btnGroup = document.createElement('div');
        btnGroup.className = 'btn-group btn-group-sm';

        let changeButton = document.createElement('button');
        changeButton.innerText = 'Start';
        changeButton.className = 'btn btn-info';
        changeButton.onclick = () => {
            ipcRenderer.send('change-upload', item, true);
        };
        btnGroup.appendChild(changeButton);

        let secondButton = document.createElement('button');
        secondButton.innerText = 'Stop';
        secondButton.className = 'btn btn-success';
        secondButton.onclick = () => {
            ipcRenderer.send('change-upload', item, false);
        };
        btnGroup.appendChild(secondButton);
        title.appendChild(btnGroup);

        let span = document.createElement('span');
        span.innerText = item.name;
        span.className = 'mx-4'
        title.appendChild(span);

        let speed = document.createElement('span');
        speed.id = 'upload-speed' + item.md5 + item.ufile_id;
        speed.innerText = '';
        title.appendChild(speed);
        groupItem.appendChild(title);

        let bar = document.createElement('div');
        bar.id = 'upload-bar' + item.md5 + item.ufile_id;
        bar.className = 'progress-bar';
        let percent = 0;
        bar.style = `width: ${percent * 100}%;`;
        bar.innerText = `${(percent * 100).toFixed(1)}%`;
        let prog = document.createElement('div');
        prog.className = 'progress m-3';
        prog.appendChild(bar);
        groupItem.appendChild(prog);
        group.appendChild(groupItem);
    }
    document.getElementById('section-signed').innerHTML = '';
    document.getElementById('section-signed').appendChild(group);
});


ipcRenderer.on('update-upload', (event, queue) => {
    let timeNow = Date.now();
    for (let item of queue) {
        let bar = document.getElementById('upload-bar' + item.md5 + item.ufile_id);
        let percent = item.progress;
        bar.style = `width: ${percent * 100}%;`;
        bar.innerText = `${(percent * 100).toFixed(1)}%`;
        if (item.ctn == true) {
            let span = document.getElementById('upload-speed' + item.md5 + item.ufile_id);
            span.innerText = humanFileSize(1024 * 1024 * 1000 / (timeNow - item.lstTime)) + '/s';
        }
        item.lstTime = timeNow;
    }
    ipcRenderer.send('update-upload-last', queue);
});