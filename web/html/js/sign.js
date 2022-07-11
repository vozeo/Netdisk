$('#signin').off().on('click', function () {
    $.ajax({
        url: 'signin.php',
        type: 'POST',
        data: {
            username: $('#username').val(),
            password: $('#password').val(),
        },
        dataType: 'json',
        success: function (data) {
            showToast('Sign in', data['message'], true);
            window.setTimeout(function() {
                window.location.replace('home.php');
            }, 2000);
        },
        error: function (data) {
            alert(JSON.parse(data.responseText).message);
        }
    });
});

$('#signup').off().on('click', function () {
    $.ajax({
        url: 'signup.php',
        type: 'POST',
        data: {
            username: $('#username').val(),
            password: $('#password').val(),
        },
        dataType: 'json',
        success: function (data) {
            showToast('Sign up', data['message'], true);
            window.setTimeout(function() {
                window.location.replace('home.php');
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