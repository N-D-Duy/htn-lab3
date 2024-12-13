import { app } from "./firebase-config.js"; // Ensure you include the .js extension
import { getAuth, signOut, onAuthStateChanged } from "https://www.gstatic.com/firebasejs/11.0.2/firebase-auth.js";
import { getDatabase, ref, set, onValue } from "https://www.gstatic.com/firebasejs/11.0.2/firebase-database.js";

const database = getDatabase(app);

const auth = getAuth(app);

var currentText = "";
var currentMode = "";
const location = "lab3-1";

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
            const labelHolder = document.getElementById('label');
            if (labelHolder) {
                currentText = data.lab3;
                labelHolder.value = data.lab3;
            }

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

                const mode = document.getElementById('mode');
                if(mode) {
                    mode.innerText = "Mode: " + lab31.mode;
                    currentMode = lab31.mode;
                }

                const power = document.getElementById('power');
                const powerData = lab31.power === true;
                if(power){
                    power.classList.toggle('on', powerData);
                }
            }
        }
    });
}

async function setupButton() {
    const button = document.getElementById('pump');
    button.addEventListener('click', (event) =>{
        let state = false;

        if (button.classList.contains('on')) {
            state = false;
            button.classList.remove('on');
        } else {
            state = true;
            button.classList.add('on');
        }
        
        const deviceType = "isPumpOn";
        if(currentMode==="AUTO" && state===true){
            currentMode="MANUAL";
        } else if(currentMode==="MANUAL" && state===false){
            currentMode="AUTO";
        }
        console.log(currentMode);
        console.log(state);
        updatePumpMode(location, currentMode);
        updateButton(location, deviceType, state);
        
    });

    const power = document.getElementById('power');
    power.addEventListener('click', (event) =>{
        let state = false;
        if(power.classList.contains('on')){
            state = false;
            power.classList.remove('on');
        } else {
            state = true;
            power.classList.add('on');
        }
        updatePower(location, state);
    });
}

async function updatePower(location, state) {
    const deviceRef = ref(database, `${location}/power`);
    await updateButton(location, "isPumpOn", false);
    await updatePumpMode(location, "AUTO");
    await set(deviceRef, state);
}

async function updateButton(location, deviceType, state) {
    const deviceRef = ref(database, `${location}/${deviceType}`);
    await set(deviceRef, state);
}

async function updatePumpMode(location, mode) {
    const deviceRef = ref(database, `${location}/mode`);
    if(mode!=null){
        console.log(mode);
        await set(deviceRef, mode);        
    } else{
        console.log("Mode is null");
    }
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


