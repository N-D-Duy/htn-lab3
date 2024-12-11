import { app } from "./firebase-config.js";
import { getAuth, signInWithEmailAndPassword } from "https://www.gstatic.com/firebasejs/11.0.2/firebase-auth.js";

$(document).ready(function () {
    $('#loginButton').click(function (e) {
        e.preventDefault();
        
        let email = document.getElementById('exampleInputEmail').value;
        let password = document.getElementById('exampleInputPassword').value;

        const auth = getAuth(app);
        signInWithEmailAndPassword(auth, email, password)
            .then((userCredential) => {
                alert('Đăng nhập thành công');
                window.location.href = 'index.html';
            })
            .catch((error) => {
                var errorMessage = error.message;
                alert('Đăng nhập thất bại: ' + errorMessage);
            });
    });
});