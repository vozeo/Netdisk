const { ipcRenderer } = require('electron');

$('#signin').off().on('click', function () {
    let username = $('#username').val();
    $.ajax({
        url: 'signin.php',
        type: 'POST',
        data: {
            username: username,
            password: $('#password').val(),
        },
        dataType: 'json',
        error: function (data) {
            alert(JSON.parse(data.responseText).message);
        }
    }).done(function (data) {
        showToast('Sign in', data['message'], true);
        ipcRenderer.sendSync('setPath', [{ name: 'Home', ufile_id: data['rootdir'] }], username);
        window.setTimeout(function () {
            window.location.replace('index.html');
        }, 2000);
    });
});

$('#signup').off().on('click', function () {
    let username = $('#username').val();
    $.ajax({
        url: 'signup.php',
        type: 'POST',
        data: {
            username: username,
            password: $('#password').val(),
        },
        dataType: 'json',
        success: function (data) {
            showToast('Sign up', data['message'], true);
            ipcRenderer.sendSync('setPath', [{ name: 'Home', ufile_id: data['rootdir'] }], username);
            window.setTimeout(function () {
                window.location.replace('index.html');
            }, 2000);
        },
        error: function (data) {
            alert(JSON.parse(data.responseText).message);
        }
    });
});

function showToast(title, content, autohide) {
    toastLive = document.getElementById('liveToast');
    document.getElementById('toast-title').innerText = title;
    document.getElementById('liveToast').setAttribute('data-bs-autohide', autohide);
    document.getElementById('toast-content').innerText = content;
    toast = new bootstrap.Toast(toastLive);
    toast.show();
}