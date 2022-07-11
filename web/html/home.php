<?php
error_reporting(E_ALL);
ini_set('display_errors', 1);
?>
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="gbk"/>
    <meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no"/>
    <title>Netdisk</title>
    <link rel="icon" type="image/x-icon" href="./asset/favicon.ico"/>
    <link href="./css/bootstrap-icons.css" rel="stylesheet"/>
    <link href="./css/styles.css" rel="stylesheet"/>
    <link href="./css/file-manager.css" rel="stylesheet"/>
    <link href="./css/context-menu.css" rel="stylesheet"/>
    <link rel="stylesheet" href="https://cdn.bootcdn.net/ajax/libs/font-awesome/5.14.0/css/all.min.css"/>
    <link rel="stylesheet" href="https://cdn.bootcdn.net/ajax/libs/ionicons/4.6.3/css/ionicons.min.css"/>
</head>

<body id="page-top">

<nav class="navbar navbar-expand-lg navbar-light fixed-top shadow-sm" id="mainNav">
    <div class="container px-5">
        <a class="navbar-brand fw-bold" href="/">Netdisk</a>
        <button class="navbar-toggler" type="button" data-bs-toggle="collapse" data-bs-target="#navbarResponsive"
                aria-controls="navbarResponsive" aria-expanded="false" aria-label="Toggle navigation">
            Menu
        </button>
        <div class="collapse navbar-collapse" id="navbarResponsive">
            <ul class="navbar-nav ms-auto me-4 my-3 my-lg-0">
                <li class="nav-item"><a class="nav-link me-lg-3" href="/">Home</a></li>
                <?php
                require_once 'user.php';
                session_start();
                $user = new user();
                $user->setRootdir(-1);
                if (isset($_SESSION["user"])) {
                    $user = $_SESSION['user'];
                }
                if ($user->getRootdir() > 0) {
                ?>
                <li class="nav-item"><a class="nav-link me-lg-3" href="/signout.php">Sign out</a></li>
                <?php } else {?>
                <li class="nav-item"><a class="nav-link me-lg-3" href="/signin.html">Sign in</a></li>
                <li class="nav-item"><a class="nav-link me-lg-3" href="/signup.html">Sign up</a></li>
                <?php }?>
            </ul>
            <?php if ($user->getRootdir() > 0) {?>
            <button class="btn btn-primary rounded-pill px-3 mb-2 mb-lg-0">
                    <span class="d-flex align-items-center">
                        <span class="small"><?php echo $user->getName(); ?></span>
                    </span>
            </button>
            <?php }?>
        </div>
    </div>
</nav>

<section>
    <?php if ($user->getRootdir() > 0) {?>
    <div class="container flex-grow-1 light-style">
        <div class="container-m-nx container-m-ny mb-3">
            <ol id="bread-menu" class="breadcrumb py-3 m-0 ms-3">
            </ol>

            <hr class="m-0"/>

            <div class="file-manager-actions py-2">
                <div>
                    <button id="upload" type="button" class="btn btn-primary m-2">
                        <input id="uploadFile" type="file" name="file" style="display: none;">
                        <i class="ion ion-md-cloud-upload"></i>
                        &nbsp;Upload
                    </button>
                    <button id="newFolder" type="button" class="btn btn-secondary m-2">
                        <i class="ion ion-md-add-circle-outline"></i>
                        &nbsp;New folder
                    </button>
                    <button id="refresh" type="button" class="btn btn-info m-2">
                        <i class="ion ion-md-refresh"></i>
                        &nbsp;Refresh
                    </button>
                </div>
            </div>

            <hr class="m-0"/>
        </div>

        <div id="file-manager" class="file-manager-container file-manager-col-view">
        </div>

        <div class="modal fade" id="smallModal" data-bs-backdrop="static" data-bs-keyboard="false" tabindex="-1" aria-labelledby="staticBackdropLabel" aria-hidden="true">
            <div class="modal-dialog modal-dialog-center">
                <div class="modal-content">
                    <div class="modal-header">
                        <h5 class="modal-title" id="smallTitle">Modal title</h5>
                        <button type="button" class="btn-close" data-bs-dismiss="modal" aria-label="Close"></button>
                    </div>
                    <div id="smallBody" class="modal-body">
                        <form>
                            <div class="mb-3">
                                <input type="text" class="form-control" id="smallInput" placeholder="">
                            </div>
                        </form>
                    </div>
                    <div class="modal-footer">
                        <button type="button" class="btn btn-secondary" data-bs-dismiss="modal">Close</button>
                        <button id="smallButton" type="button" class="btn btn-primary">Save changes</button>
                    </div>
                </div>
            </div>
        </div>

    </div>

    <div class="modal fade" id="largeBackdrop" data-bs-backdrop="static" data-bs-keyboard="false" tabindex="-1" aria-labelledby="staticBackdropLabel" aria-hidden="true">
        <div class="modal-dialog modal-xl modal-dialog-centered">
            <div class="modal-content">
                <div class="modal-header">
                    <h5 class="modal-title" id="largeTitle">Modal title</h5>
                    <button type="button" class="btn-close" data-bs-dismiss="modal" aria-label="Close"></button>
                </div>
                <div id="BackdropLabel" class="modal-body">
                    <ol id="bread-menu-sub" class="breadcrumb py-3 m-0"></ol>
                    <div id="file-manager-sub" class="file-manager-container file-manager-col-view"></div>
                </div>
                <div class="modal-footer">
                    <button type="button" class="btn btn-secondary" data-bs-dismiss="modal">Cancel</button>
                    <button id="largeButton" type="button" class="btn btn-primary">Understood</button>
                </div>
            </div>
        </div>
    </div>

    <div class="position-fixed bottom-0 end-0 p-3 pb-5" style="z-index: 11">
        <div id="liveToast" class="toast text-white bg-danger" role="alert" aria-live="assertive" aria-atomic="true">
            <div class="toast-header">
                <strong id="toast-title" class="me-auto"></strong>
            </div>
            <div id="toast-content" class="toast-body"></div>
        </div>
    </div>

    <?php } else {?>
        <h2 class="display-4 mb-3 text-center p-5">Please sign in or sign up.</h2>
    <?php }?>
</section>

<footer class="bg-gradient-primary-to-secondary text-center fixed-bottom p-1">
    <div class="container p-1">
        <div class="text-white-50 small">
            <div class="mb-2">&copy; Net-disk 2022. All Rights Reserved.</div>
        </div>
    </div>
</footer>

<script src="./js/bootstrap.bundle.min.js"></script>
<script src="./js/jquery-3.6.0.min.js"></script>
<script src="./js/spark-md5.min.js"></script>
<script type='text/javascript'>
let globalPath_sub = [];
<?php
if ($user->getRootdir() > 0) {
    $rootPath = $user->getRootdir();
    echo <<<JS
        let globalPath = [{
            name: "Home",
            ufile_id: $rootPath
        }];
    JS;
}
?>
</script>
<script src="./js/context-menu.js"></script>
<script src="./js/load-view.js"></script>

</body>

</html>