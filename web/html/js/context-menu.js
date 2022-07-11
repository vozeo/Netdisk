const ContextMenu = function (options) {
    let instance;

    function createMenu() {
        const ul = document.createElement("ul");
        ul.classList.add("custom-context-menu");
        const { menus } = options;
        if (menus && menus.length > 0) {
            for (let menu of menus) {
                const li = document.createElement("li");
                li.textContent = menu.name;
                li.onclick = menu.onClick;
                ul.appendChild(li);
            }
        }
        const body = document.querySelector("body");
        body.appendChild(ul);
        return ul;
    }

    return {
        getInstance: function () {
            if (!instance) {
                instance = createMenu();
            }
            return instance;
        },
    };
};

let lastX = 0, lastY = 0;

function getLastID() {
    const elements = document.elementsFromPoint(lastX, lastY);
    let srcId;
    for (let i in elements) {
        if (elements[i].className == "btn file-item") {
            srcId = elements[i].id;
        }
    }
    return srcId.replace(/[^0-9]/ig, "");
}

const contextMenu = ContextMenu({
    menus: [
        {
            name: "Copy",
            onClick: function (e) {
                let src = getLastID();
                document.getElementById('largeTitle').innerText = "Copy to";
                document.getElementById('largeButton').innerText = "Paste";
                $('#largeButton').off().on('click', function () {
                    upPost("13", [src, globalPath_sub[globalPath_sub.length - 1].ufile_id]);
                    $("#largeBackdrop").modal('hide');
                });
                $("#largeBackdrop").modal('show');
                globalPath_sub = [globalPath[0]];
                getData(globalPath_sub, "file-manager-sub", "bread-menu-sub", "return-button-sub", "file-sub");
            },
        },
        {
            name: "Move",
            onClick: function (e) {
                let src = getLastID();
                document.getElementById('largeTitle').innerText = "Move to";
                document.getElementById('largeButton').innerText = "Paste";
                $('#largeButton').off().on('click', function () {
                    upPost("14", [src, globalPath_sub[globalPath_sub.length - 1].ufile_id]);
                    $("#largeBackdrop").modal('hide');
                });
                $("#largeBackdrop").modal('show');
                globalPath_sub = [globalPath[0]];
                getData(globalPath_sub, "file-manager-sub", "bread-menu-sub", "return-button-sub", "file-sub");
            },
        },
        {
            name: "Rename",
            onClick: function (e) {
                let src = getLastID();
                document.getElementById('smallTitle').innerText = "Rename to";
                document.getElementById('smallButton').innerText = "OK";
                document.getElementById('smallInput').value = "";
                $('#smallButton').off().on('click', function () {
                    upPost("16", [src, document.getElementById('smallInput').value]);
                    $("#smallModal").modal('hide');
                });
                $("#smallModal").modal('show');
            },
        },
        {
            name: "Download",
            onClick: function (e) {
                let src = getLastID();
                download({
                    url: "data.php",
                    ufile_id: src,
                    poolLimit: 1,
                });
            },
        },
        {
            name: "Delete",
            onClick: function (e) {
                let src = getLastID();
                document.getElementById('smallTitle').innerText = "Delete";
                document.getElementById('smallButton').innerText = "OK";
                document.getElementById('smallBody').innerText = "Are you sure to delete?";
                $('#smallButton').off().on('click', function () {
                    upPost("15", [src]);
                    $("#smallModal").modal('hide');
                });
                $("#smallModal").modal('show');
            },
        },
    ],
});

function showMenu(e) {
    const element = document.elementFromPoint(e.clientX, e.clientY);
    lastX = e.clientX, lastY = e.clientY;
    const name = /return*/g;
    if ($('#largeBackdrop').css('display') == "none"
        && !name.test(element.id)
        && (element.classList.contains("file-item")
            || element.classList.contains("file-item-icon")
            || element.classList.contains("file-item-name"))) {
        e.preventDefault();
        const menus = contextMenu.getInstance();
        menus.style.top = `${e.clientY}px`;
        menus.style.left = `${e.clientX}px`;
        menus.classList.remove("hidden");
    }
}

function hideMenu(event) {
    const menus = contextMenu.getInstance();
    menus.classList.add("hidden");
}

document.addEventListener("contextmenu", showMenu);
document.addEventListener("click", hideMenu);