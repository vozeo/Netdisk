const { ipcRenderer } = require('electron');

const signElements = ['li-signout', 'section-signed'];
const unsignElements = ['li-signin', 'li-signup', 'section-unsigned'];

let globalPath_sub = [];
let globalPath = ipcRenderer.sendSync('getPath');
let username = ipcRenderer.sendSync('getUsername');
if (username) {
document.getElementById('username-button').innerText = username;
} else {
    document.getElementById('username-button').innerText = 'Not sign in';
}

if (globalPath && globalPath[0]) {
    for (let i in signElements) {
        document.getElementById(signElements[i]).style = 'display: inline;'
    }
    for (let i in unsignElements) {
        document.getElementById(unsignElements[i]).style = 'display: none;'
    }
    console.log(document.cookie);
}