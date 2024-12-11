import { app } from "./firebase-config.js"; // Ensure you include the .js extension
import { getAuth, signOut, onAuthStateChanged } from "https://www.gstatic.com/firebasejs/11.0.2/firebase-auth.js";
import { getDatabase, ref, set, onValue } from "https://www.gstatic.com/firebasejs/11.0.2/firebase-database.js";

const database = getDatabase(app);

const auth = getAuth(app);

var currentText = "";

onAuthStateChanged(auth, (user) => {
    if (user) {
        console.log("User is signed in:", user.email);
    } else {
        window.location.href = "login.html";
    }
});

async function getDataAndUpdateHolder() {
    const devicesRef = ref(database);

    onValue(devicesRef, (snapshot) => {
        const data = snapshot.val();

        if (data) {
            //update lab3.2
            const labelHolder = document.getElementById('label');
            if (labelHolder) {
                currentText = data.lab3;
                labelHolder.value = data.lab3;
            }

            // update lab3-1
            const lab31 = data['lab3-1'];
            if (lab31) {
                const humidityHolder = document.getElementById('humidity');
                if (humidityHolder) {
                    humidityHolder.innerText = "Humidity: " + lab31.humidity + "%";
                }
                const temperatureHolder = document.getElementById('temperature');
                if (temperatureHolder) {
                    temperatureHolder.innerText = "Temperature: " + lab31.temperature + "°C";
                }
                const pumpData = lab31.isPumpOn === true;
                const pumpHolder = document.getElementById('pump');
                if (pumpHolder) {
                    pumpHolder.classList.toggle('on', pumpData);
                }
            }
        }
    });
}

async function setupButton() {
    const button = document.getElementById('pump');
    button.addEventListener('click', (event) => {
        let state = false;

        if (button.classList.contains('on')) {
            state = false;
            button.classList.remove('on');
        } else {
            state = true;
            button.classList.add('on');
        }
        const location = "lab3-1";
        const deviceType = "isPumpOn";
        updateButton(location, deviceType, state);
    });
}

async function updateButton(location, deviceType, state) {
    const deviceRef = ref(database, `${location}/${deviceType}`);
    await set(deviceRef, state);
}

async function logout() {
    const logoutButton = document.getElementById('logoutButton');
    logoutButton.addEventListener('click', async () => {
        await signOut(auth).then(() => {
            window.location.href = "login.html";
        }).catch((error) => {
            console.error("Logout error:", error);
        });
    });
}


function setupTextField() {
    const field = document.getElementById('label');
    field.addEventListener('keypress', async (e) => {
        if (e.key === 'Enter') {
            if (!field.value || field.value == currentText) return;
            await updateDeviceState("lab3", field.value);
        }
    });
}

async function updateDeviceState(location, text) {
    const dbRef = ref(database, location);
    await set(dbRef, text);
}



window.addEventListener('DOMContentLoaded', () => {
    setupTextField();
    getDataAndUpdateHolder();
    logout();
    setupButton();
});


