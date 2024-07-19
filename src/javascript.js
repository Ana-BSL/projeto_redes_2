let port;
let reader;
let inputDone;
let outputDone;
let inputStream;
let outputStream;
let writer;

document.getElementById('connectButton').addEventListener('click', async () => {
    try {
        port = await navigator.serial.requestPort();
        await port.open({ baudRate: 115200 });

        const textDecoder = new TextDecoderStream();
        inputDone = port.readable.pipeTo(textDecoder.writable);
        inputStream = textDecoder.readable;
        reader = inputStream.getReader();

        const textEncoder = new TextEncoderStream();
        outputDone = textEncoder.readable.pipeTo(port.writable);
        outputStream = textEncoder.writable;
        writer = outputStream.getWriter();

        document.getElementById('status').innerText = "Connected to ESP32";
        document.getElementById('Lverde').disabled = false;
        document.getElementById('Lamarelo').disabled = false;
    } catch (error) {
        console.error('Error connecting to ESP32:', error);
        document.getElementById('status').innerText = "Failed to connect";
    }
});

document.getElementById('Lverde').addEventListener('click', async () => {
    if (writer) {
        await writer.write('presente\n');
        const { value } = await reader.read();
        if (value) {
            document.getElementById('response').innerText = value;
        }
    }
});

document.getElementById('Lamarelo').addEventListener('click', async () => {
    if (writer) {
        await writer.write('faltou\n');
        const { value } = await reader.read();
        if (value) {
            document.getElementById('response').innerText = value;
        }
    }
});
async function readLoop() {
    while (true) {
        try {
            const { value, done } = await reader.read();
            if (done) {
                console.log('Reader closed');
                reader.releaseLock();
                break;
            }
            console.log('Read from ESP32: ${value}');
            document.getElementById('status').innerText = value;
        } catch (error) {
            console.error('Error in readLoop:', error);
            break;
        }
    }
}

if (port) {
    readLoop();
}